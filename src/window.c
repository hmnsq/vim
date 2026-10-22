/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

ساکن خالی cmd_with_گنتی(حرف *cmd, char_u *bufp, size_t bufsize, طویل Prenum);
ساکن خالی win_init(win_T *newp, win_T *oldp, صحیح flags);
ساکن خالی win_init_some(win_T *newp, win_T *oldp);
ساکن خالی چوکھٹا_comp_pos(چوکھٹا_T *topfrp, صحیح *صف, صحیح *ستون);
ساکن خالی چوکھٹا_setheight(چوکھٹا_T *curfrp, صحیح اونچائی);
ساکن خالی چوکھٹا_setwidth(چوکھٹا_T *curfrp, صحیح چوڑائی);
ساکن خالی win_exchange(طویل);
ساکن خالی win_rotate(صحیح, صحیح);
ساکن خالی win_equal_rec(win_T *اگلا_curwin, صحیح current, چوکھٹا_T *topfr, صحیح dir, صحیح ستون, صحیح صف, صحیح چوڑائی, صحیح اونچائی);
ساکن خالی trigger_winnewpre(خالی);
ساکن خالی trigger_winclosed(win_T *win);
ساکن win_T *win_free_mem(win_T *win, صحیح *dirp, tabpage_T *tp);
ساکن چوکھٹا_T *win_altframe(win_T *win, tabpage_T *tp);
ساکن tabpage_T *alt_tabpage(خالی);
ساکن win_T *frame2win(چوکھٹا_T *frp);
ساکن صحیح چوکھٹا_has_win(چوکھٹا_T *frp, win_T *wp);
ساکن خالی win_fix_scroll(صحیح resize);
ساکن خالی win_fix_cursor(صحیح معمول_4);
ساکن خالی چوکھٹا_new_اونچائی(چوکھٹا_T *topfrp, صحیح اونچائی, صحیح topfirst, صحیح wfh, صحیح سیٹ_ch);
ساکن صحیح چوکھٹا_fixed_اونچائی(چوکھٹا_T *frp);
ساکن صحیح چوکھٹا_fixed_چوڑائی(چوکھٹا_T *frp);
ساکن خالی چوکھٹا_جوڑو_statusline(چوکھٹا_T *frp);
ساکن خالی چوکھٹا_new_چوڑائی(چوکھٹا_T *topfrp, صحیح چوڑائی, صحیح leftfirst, صحیح wfw);
ساکن خالی چوکھٹا_سیٹ_vsep(چوکھٹا_T *frp, صحیح جوڑو);
ساکن صحیح چوکھٹا_minwidth(چوکھٹا_T *topfrp, win_T *اگلا_curwin);
ساکن خالی چوکھٹا_fix_چوڑائی(win_T *wp);
ساکن صحیح win_alloc_firstwin(win_T *oldwin);
ساکن خالی new_چوکھٹا(win_T *wp);
ساکن tabpage_T *alloc_tabpage(خالی);
ساکن صحیح leave_tabpage(buf_T *new_curbuf, صحیح trigger_leave_autocmds);
ساکن خالی enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, صحیح trigger_enter_autocmds, صحیح trigger_leave_autocmds);
ساکن خالی چوکھٹا_fix_اونچائی(win_T *wp);
ساکن صحیح چوکھٹا_minheight(چوکھٹا_T *topfrp, win_T *اگلا_curwin);
ساکن صحیح چوکھٹا_wincount_اندر_اونچائی(چوکھٹا_T *topfrp);
ساکن صحیح may_open_tabpage(خالی);
ساکن صحیح win_enter_ext(win_T *wp, صحیح flags);
ساکن خالی win_free(win_T *wp, tabpage_T *tp);
ساکن خالی win_append(win_T *after, win_T *wp);
ساکن خالی چوکھٹا_append(چوکھٹا_T *after, چوکھٹا_T *frp);
ساکن خالی چوکھٹا_داخل_کرو(چوکھٹا_T *before, چوکھٹا_T *frp);
ساکن خالی چوکھٹا_remove(چوکھٹا_T *frp);
ساکن خالی win_goto_ver(صحیح اوپر, طویل گنتی);
ساکن خالی win_goto_hor(صحیح بایاں, طویل گنتی);
ساکن خالی چوکھٹا_جوڑو_اونچائی(چوکھٹا_T *frp, صحیح n);
ساکن خالی آخری_حالت_rec(چوکھٹا_T *fr, صحیح statusline);
#اگر defined(FEAT_STL_OPT)
ساکن صحیح چوکھٹا_ڈھونڈو_global_stlh_rec(چوکھٹا_T *frp, صحیح h);
ساکن خالی چوکھٹا_change_statusline_اونچائی_rec(چوکھٹا_T *frp, صحیح global_stlh);
#اگر_ختم
ساکن خالی چوکھٹا_flatten(چوکھٹا_T *frp);
ساکن خالی winframe_restore(win_T *wp, صحیح dir, چوکھٹا_T *unflat_altfr);

ساکن صحیح make_snapshot_rec(چوکھٹا_T *fr, چوکھٹا_T **frp);
ساکن خالی clear_snapshot(tabpage_T *tp, صحیح idx);
ساکن خالی clear_snapshot_rec(چوکھٹا_T *fr);
ساکن صحیح check_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr);
ساکن win_T *restore_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr);
ساکن win_T *get_snapshot_curwin(صحیح idx);

ساکن صحیح چوکھٹا_check_اونچائی(چوکھٹا_T *topfrp, صحیح اونچائی);
ساکن صحیح چوکھٹا_check_چوڑائی(چوکھٹا_T *topfrp, صحیح چوڑائی);

ساکن win_T *win_alloc(win_T *after, صحیح hidden);

#تعریف_کرو NOWIN		((win_T *)-1)	// non-existing window

#تعریف_کرو صفیں_AVAIL (صفیں - p_ch - tabline_اونچائی())

// flags for win_enter_ext()
#تعریف_کرو WEE_UNDO_SYNC			0x01
#تعریف_کرو WEE_CURWIN_INVALID		0x02
#تعریف_کرو WEE_TRIGGER_NEW_AUTOCMDS	0x04
#تعریف_کرو WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#تعریف_کرو WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#تعریف_کرو WEE_ALLOW_PARSE_MESSAGES	0x20

ساکن حرف *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ساکن صحیح بانٹو_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
ساکن صحیح close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
ساکن صحیح چوکھٹا_locked = 0;

#اگر defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
ساکن صحیح stl_rendered_اونچائی = 1;
ساکن صحیح stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
ساکن صحیح stlo_mh = حالت_اونچائی_2; // "maxheight" value cache of global 'stlo'
#اگر_ختم

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    خالی
کھڑکی_layout_تالا(خالی)
{
    ++بانٹو_disallowed;
    ++close_disallowed;
}

    خالی
کھڑکی_layout_unlock(خالی)
{
    --بانٹو_disallowed;
    --close_disallowed;
}

    صحیح
چوکھٹے_locked(خالی)
{
    واپس چوکھٹا_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    صحیح
کھڑکی_layout_locked(شمار CMD_اشاریہ cmd)
{
    اگر (بانٹو_disallowed > 0 || close_disallowed > 0)
    {
	اگر (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_بانٹو_کھڑکی_when_closing_بفر));
	ورنہ
	    emsg(_(e_not_allowed_مقصد_change_کھڑکی_layout_اندر_this_autocmd));
	واپس TRUE;
    }
    واپس FALSE;
}

// #define WIN_DEBUG
#اگر_تعریف_شدہ WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    ساکن خالی
log_چوکھٹا_layout(چوکھٹا_T *چوکھٹا)
{
    ch_log(تہی, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    چوکھٹا->fr_layout == FR_برگ ? "LEAF"
				  : چوکھٹا->fr_layout == FR_صف ? "ROW" : "COL",
	    چوکھٹا->fr_چوڑائی,
	    چوکھٹا->fr_اونچائی,
	    چوکھٹا->fr_win == تہی ? -1 : چوکھٹا->fr_win->w_چوڑائی,
	    چوکھٹا->fr_win == تہی ? -1 : چوکھٹا->fr_win->w_اونچائی,
	    چوکھٹا->fr_win == تہی ? -1 : چوکھٹا->fr_win->w_id);
    اگر (چوکھٹا->fr_بچہ != تہی)
    {
	ch_log(تہی, "children");
	log_چوکھٹا_layout(چوکھٹا->fr_بچہ);
	اگر (چوکھٹا->fr_اگلا != تہی)
	    ch_log(تہی, "END of children");
    }
    اگر (چوکھٹا->fr_اگلا != تہی)
	log_چوکھٹا_layout(چوکھٹا->fr_اگلا);
}
#اگر_ختم

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    صحیح
check_can_سیٹ_curbuf_disabled(خالی)
{
    اگر (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_مقصد_بفر));
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    صحیح
check_can_سیٹ_curbuf_forceit(صحیح forceit)
{
    اگر (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_مقصد_بفر));
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(خالی)
{
    // In cmdwin, the alternative buffer should be used.
    واپس is_اندر_cmdwin() && prevwin != تہی ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = تہی;

    اگر (buf == تہی)
	واپس wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    اگر (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    اگر (wp == تہی && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    واپس wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    خالی
do_کھڑکی(
    صحیح		nchar,
    طویل	Prenum,
    صحیح		xchar)	    // extra char from ":wincmd gx" or NUL
{
    طویل	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
    صحیح		type = ڈھونڈو_DEFINE;
    صحیح		len;
#اگر_ختم
    char_u	cbuf[40];

    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;

#تعریف_کرو CHECK_CMDWIN \
    کرو { \
	اگر (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_اندر_cmdline_کھڑکی)); \
	    واپس; \
	} \
    } جب (0)

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
		    جاؤ newwindow;
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		(خالی)win_بانٹو((صحیح)Prenum, 0);
		توڑو;

// split current window in two parts, vertically
    حالت Ctrl_V:
    حالت 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		اگر (bt_quickfix(curbuf))
		    جاؤ newwindow;
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		(خالی)win_بانٹو((صحیح)Prenum, WSP_VERT);
		توڑو;

// split current window and edit alternate file
    حالت Ctrl_HAT:
    حالت '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		اگر (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == تہی)
		{
		    اگر (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    ورنہ
			semsg(_(e_بفر_nr_not_ملا), Prenum);
		    توڑو;
		}

		اگر (!curbuf_locked() && win_بانٹو(0, 0) == OK)
		    (خالی)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		توڑو;

// open new window
    حالت Ctrl_N:
    حالت 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		اگر (Prenum)
		    // window height
		    vim_snprintf((حرف *)cbuf, حجم(cbuf) - 5, "%ld", Prenum);
		ورنہ
		    cbuf[0] = NUL;
#اگر defined(FEAT_QUICKFIX)
		اگر (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#اگر_ختم
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		توڑو;

// quit current window
    حالت Ctrl_Q:
    حالت 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_گنتی("quit", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		توڑو;

// close current window
    حالت Ctrl_C:
    حالت 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_گنتی("close", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		توڑو;

#اگر defined(FEAT_QUICKFIX)
// close preview window
    حالت Ctrl_Z:
    حالت 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		توڑو;

// cursor to preview window
    حالت 'P':
		FOR_ALL_WINDOWS(wp)
		    اگر (wp->w_p_pvw)
			توڑو;
		اگر (wp == تہی)
		    emsg(_(e_there_is_no_preview_کھڑکی));
		ورنہ
		    win_goto(wp);
		توڑو;
#اگر_ختم

// close all but current window
    حالت Ctrl_O:
    حالت 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_گنتی("only", cbuf, حجم(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		توڑو;

// cursor to next window with wrap around
    حالت Ctrl_W:
    حالت 'w':
// cursor to previous window with wrap around
    حالت 'W':
		CHECK_CMDWIN;
		اگر (ONE_کھڑکی && Prenum != 1)	// just one window
		    beep_flush();
		ورنہ
		{
		    اگر (Prenum)			// go to specified window
		    {
			برائے (wp = firstwin; --Prenum > 0; )
			{
			    اگر (wp->w_اگلا == تہی)
				توڑو;
			    ورنہ
				wp = wp->w_اگلا;
			}
		    }
		    ورنہ
		    {
			اگر (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_پچھلا;
			    اگر (wp == تہی)
				wp = lastwin;	    // wrap around
			}
			ورنہ			    // go to next window
			{
			    wp = curwin->w_اگلا;
			    اگر (wp == تہی)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		توڑو;

// cursor to window below
    حالت 'j':
    حالت K_نیچے_کو:
    حالت Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		توڑو;

// cursor to window above
    حالت 'k':
    حالت K_اوپر:
    حالت Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		توڑو;

// cursor to left window
    حالت 'h':
    حالت K_بایاں:
    حالت Ctrl_H:
    حالت K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		توڑو;

// cursor to right window
    حالت 'l':
    حالت K_دایاں:
    حالت Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		توڑو;

// move window to new tab page
    حالت 'T':
		CHECK_CMDWIN;
		اگر (one_کھڑکی())
		    پیغام(_(m_onlyone));
		ورنہ
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
		توڑو;

// cursor to top-left window
    حالت 't':
    حالت Ctrl_T:
		win_goto(firstwin);
		توڑو;

// cursor to bottom-right window
    حالت 'b':
    حالت Ctrl_B:
		win_goto(lastwin);
		توڑو;

// cursor to last accessed (previous) window
    حالت 'p':
    حالت Ctrl_P:
		اگر (!win_valid(prevwin))
		    beep_flush();
		ورنہ
		    win_goto(prevwin);
		توڑو;

// exchange current and next window
    حالت 'x':
    حالت Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		توڑو;

// rotate windows downwards
    حالت Ctrl_R:
    حالت 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (صحیح)Prenum1);    // downwards
		توڑو;

// rotate windows upwards
    حالت 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (صحیح)Prenum1);	    // upwards
		توڑو;

// move window to the very top/bottom/left/right
    حالت 'K':
    حالت 'J':
    حالت 'H':
    حالت 'L':
		CHECK_CMDWIN;
		اگر (ONE_کھڑکی)
		    beep_flush();
		ورنہ
		{
		    صحیح dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_چوٹی : WSP_BOT);

		    (خالی)win_splitmove(curwin, (صحیح)Prenum, dir);
		}
		توڑو;

// make all windows the same width and/or height
    حالت '=':
		{
		    صحیح mod = cmdmod.cmod_بانٹو & (WSP_VERT | WSP_HOR);
#اگر_تعریف_شدہ FEAT_GUI
		    need_mouse_correct = TRUE;
#اگر_ختم
		    win_equal(تہی, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		توڑو;

// increase current window height
    حالت '+':
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		win_setheight(curwin->w_اونچائی + (صحیح)Prenum1);
		توڑو;

// decrease current window height
    حالت '-':
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		win_setheight(curwin->w_اونچائی - (صحیح)Prenum1);
		توڑو;

// set current window height
    حالت Ctrl__:
    حالت '_':
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		win_setheight(Prenum ? (صحیح)Prenum : 9999);
		توڑو;

// increase current window width
    حالت '>':
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		win_setwidth(curwin->w_چوڑائی + (صحیح)Prenum1);
		توڑو;

// decrease current window width
    حالت '<':
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		win_setwidth(curwin->w_چوڑائی - (صحیح)Prenum1);
		توڑو;

// set current window width
    حالت '|':
#اگر_تعریف_شدہ FEAT_GUI
		need_mouse_correct = TRUE;
#اگر_ختم
		win_setwidth(Prenum != 0 ? (صحیح)Prenum : 9999);
		توڑو;

// jump to tag and split window if tag exists (in preview window)
#اگر defined(FEAT_QUICKFIX)
    حالت '}':
		CHECK_CMDWIN;
		اگر (Prenum)
		    g_do_tagpreview = Prenum;
		ورنہ
		    g_do_tagpreview = p_pvh;
#اگر_ختم
		// FALLTHROUGH
    حالت ']':
    حالت Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		اگر (Prenum)
		    postponed_بانٹو = Prenum;
		ورنہ
		    postponed_بانٹو = -1;
#اگر_تعریف_شدہ FEAT_QUICKFIX
		اگر (nchar != '}')
		    g_do_tagpreview = 0;
#اگر_ختم

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_بانٹو = 0;
		توڑو;

// edit file name under cursor in a new window
    حالت 'f':
    حالت 'F':
    حالت Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		اگر (check_متن_or_curbuf_locked(تہی))
		    توڑو;

		ptr = grab_file_name(Prenum1, &lnum);
		اگر (ptr != تہی)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#اگر_تعریف_شدہ FEAT_GUI
		    need_mouse_correct = TRUE;
#اگر_ختم
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = تہی;
		    اگر ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    اگر (wp == تہی && win_بانٹو(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			اگر (do_ecmd(0, ptr, تہی, تہی, ECMD_LASTL,
						   ECMD_HIDE, تہی) == FAIL)
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
			ورنہ
			    wp = curwin;
		    }

		    اگر (wp != تہی && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		توڑو;

#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    حالت 'i':			    // Go to any match
    حالت Ctrl_I:
		type = ڈھونڈو_ANY;
		// FALLTHROUGH
    حالت 'd':			    // Go to definition, using 'define'
    حالت Ctrl_D:
		CHECK_CMDWIN;
		اگر ((len = ڈھونڈو_ident_under_cursor(&ptr, ڈھونڈو_IDENT)) == 0)
		    توڑو;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		اگر (ptr == تہی)
		    توڑو;

		ڈھونڈو_نمونہ_اندر_راستہ(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_بانٹو, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_سیٹ_curswant = true;
		توڑو;
#اگر_ختم

// Quickfix window only: view the result under the cursor in a new split.
#اگر defined(FEAT_QUICKFIX)
    حالت K_KENTER:
    حالت CAR:
		اگر (bt_quickfix(curbuf))
		    qf_view_نتیجہ(TRUE);
		توڑو;
#اگر_ختم

// CTRL-W g  extended commands
    حالت 'g':
    حالت Ctrl_G:
		CHECK_CMDWIN;
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#اگر_ختم
		++no_mapping;
		++allow_کلیدیں;   // no mapping for xchar, but allow key codes
		اگر (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_کلیدیں;
		(خالی)جوڑو_مقصد_showcmd(xchar);

		انتخاب (xchar)
		{
#اگر defined(FEAT_QUICKFIX)
		    حالت '}':
			xchar = Ctrl_RSB;
			اگر (Prenum)
			    g_do_tagpreview = Prenum;
			ورنہ
			    g_do_tagpreview = p_pvh;
#اگر_ختم
			// FALLTHROUGH
		    حالت ']':
		    حالت Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			اگر (Prenum)
			    postponed_بانٹو = Prenum;
			ورنہ
			    postponed_بانٹو = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_بانٹو = 0;
			توڑو;

		    حالت 'f':	    // CTRL-W gf: "gf" in a new tab page
		    حالت 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_اشاریہ(curtab) + 1;
			nchar = xchar;
			جاؤ wingotofile;

		    حالت 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((صحیح)Prenum);
			توڑو;

		    حالت 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(صحیح)Prenum1);
			توڑو;

		    حالت TAB:	    // CTRL-W g<Tab>: go to last used tab page
			اگر (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			توڑو;

		    طےشدہ:
			beep_flush();
			توڑو;
		}
		توڑو;

    طےشدہ:	beep_flush();
		توڑو;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    خالی
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
#اگر_ختم
    حالت 'f':
    حالت 'F':
    حالت Ctrl_F:
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
    حالت 'i':
    حالت Ctrl_I:
    حالت 'd':
    حالت Ctrl_D:
#اگر_ختم
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		توڑو;

    حالت Ctrl_HAT:
    حالت '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		توڑو;

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
		توڑو;

#اگر defined(FEAT_QUICKFIX)
    حالت Ctrl_Z:
    حالت 'z':
    حالت 'P':
#اگر_ختم
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
		توڑو;
    }
}

    ساکن خالی
cmd_with_گنتی(
    حرف	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    طویل	Prenum)
{
    اگر (Prenum > 0)
	vim_snprintf((حرف *)bufp, bufsize, "%s %ld", cmd, Prenum);
    ورنہ
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    صحیح
check_بانٹو_disallowed(win_T *wp)
{
    اگر (بانٹو_disallowed > 0)
    {
	emsg(_(e_cant_بانٹو_کھڑکی_while_closing_another));
	واپس FAIL;
    }
    اگر (wp->w_بفر->b_locked_بانٹو)
    {
	emsg(_(e_cannot_بانٹو_کھڑکی_when_closing_بفر));
	واپس FAIL;
    }
    واپس OK;
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
win_بانٹو(صحیح سائز, صحیح flags)
{
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس FAIL;

    اگر (check_بانٹو_disallowed(curwin) == FAIL)
	واپس FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    اگر (may_open_tabpage() == OK)
	واپس OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_بانٹو;
    اگر ((flags & WSP_چوٹی) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_بانٹو_topleft_and_botright_at_the_same_وقت));
	واپس FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    اگر (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    ورنہ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    اگر (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    ورنہ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    واپس win_بانٹو_ins(سائز, flags, تہی, 0, تہی);
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
win_بانٹو_ins(
    صحیح		سائز,
    صحیح		flags,
    win_T	*new_wp,
    صحیح		dir,
    چوکھٹا_T	*مقصد_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    صحیح		new_سائز = سائز;
    صحیح		i;
    صحیح		need_حالت = 0;
    صحیح		do_equal = FALSE;
    صحیح		needed;
    صحیح		دستیاب;
    صحیح		oldwin_اونچائی = 0;
    صحیح		layout;
    چوکھٹا_T	*frp, *curfrp, *frp2, *prevfrp;
    صحیح		before;
    صحیح		minheight;
    صحیح		wmh1;
    صحیح		did_سیٹ_fraction = FALSE;
    صحیح		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    اگر (new_wp == تہی)
	trigger_winnewpre();

    اگر (flags & WSP_چوٹی)
	oldwin = firstwin;
    ورنہ اگر (flags & WSP_BOT)
	oldwin = lastwin;
    ورنہ
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    اگر (ONE_کھڑکی && p_ls == 1 && oldwin->w_حالت_اونچائی == 0)
    {
	اگر (!(flags & WSP_FORCE_گنجائش) && VISIBLE_اونچائی(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_گنجائش));
	    جاؤ theend;
	}
	need_حالت = statusline_اونچائی(oldwin);
    }

#اگر_تعریف_شدہ FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    اگر (gui.اندر_use)
	برآمد_flush();
#اگر_ختم

    اگر (flags & WSP_VERT)
    {
	صحیح	wmw1;
	صحیح	minwidth;

	layout = FR_صف;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	اگر (flags & WSP_گنجائش)
	    needed += p_wiw - wmw1;
	اگر (flags & (WSP_BOT | WSP_چوٹی))
	{
	    minwidth = چوکھٹا_minwidth(topframe, NOWIN);
	    دستیاب = topframe->fr_چوڑائی;
	    needed += minwidth;
	}
	ورنہ اگر (p_ea)
	{
	    minwidth = چوکھٹا_minwidth(oldwin->w_چوکھٹا, NOWIN);
	    prevfrp = oldwin->w_چوکھٹا;
	    برائے (frp = oldwin->w_چوکھٹا->fr_والد; frp != تہی;
							frp = frp->fr_والد)
	    {
		اگر (frp->fr_layout == FR_صف)
		    FOR_ALL_چوکھٹے(frp2, frp->fr_بچہ)
			اگر (frp2 != prevfrp)
			    minwidth += چوکھٹا_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    دستیاب = topframe->fr_چوڑائی;
	    needed += minwidth;
	}
	ورنہ
	{
	    minwidth = چوکھٹا_minwidth(oldwin->w_چوکھٹا, NOWIN);
	    دستیاب = oldwin->w_چوکھٹا->fr_چوڑائی;
	    needed += minwidth;
	}
	اگر (!(flags & WSP_FORCE_گنجائش) && دستیاب < needed)
	{
	    emsg(_(e_not_enough_گنجائش));
	    جاؤ theend;
	}
	اگر (new_سائز == 0)
	    new_سائز = oldwin->w_چوڑائی / 2;
	اگر (new_سائز > دستیاب - minwidth - 1)
	    new_سائز = دستیاب - minwidth - 1;
	اگر (new_سائز < wmw1)
	    new_سائز = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	اگر (oldwin->w_چوڑائی - new_سائز - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	اگر (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_چوڑائی + new_سائز + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	اگر (!do_equal && p_ea && سائز == 0 && *p_ead != 'v'
	    && oldwin->w_چوکھٹا->fr_والد != تہی)
	    do_equal = TRUE;
    }
    ورنہ
    {
	layout = FR_ستون;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_اونچائی(curwin);
	needed = wmh1 + statusline_اونچائی(oldwin);
	اگر (flags & WSP_گنجائش)
	    needed += p_wh - wmh1;
	اگر (flags & (WSP_BOT | WSP_چوٹی))
	{
	    minheight = چوکھٹا_minheight(topframe, NOWIN) + need_حالت;
	    دستیاب = topframe->fr_اونچائی;
	    needed += minheight;
	}
	ورنہ اگر (p_ea)
	{
	    minheight = چوکھٹا_minheight(oldwin->w_چوکھٹا, NOWIN) + need_حالت;
	    prevfrp = oldwin->w_چوکھٹا;
	    برائے (frp = oldwin->w_چوکھٹا->fr_والد; frp != تہی;
							frp = frp->fr_والد)
	    {
		اگر (frp->fr_layout == FR_ستون)
		    FOR_ALL_چوکھٹے(frp2, frp->fr_بچہ)
			اگر (frp2 != prevfrp)
			    minheight += چوکھٹا_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    دستیاب = topframe->fr_اونچائی;
	    needed += minheight;
	}
	ورنہ
	{
	    minheight = چوکھٹا_minheight(oldwin->w_چوکھٹا, NOWIN) + need_حالت;
	    دستیاب = oldwin->w_چوکھٹا->fr_اونچائی;
	    needed += minheight;
	}
	اگر (!(flags & WSP_FORCE_گنجائش) && دستیاب < needed)
	{
	    emsg(_(e_not_enough_گنجائش));
	    جاؤ theend;
	}
	oldwin_اونچائی = oldwin->w_اونچائی;
	اگر (need_حالت)
	{
	    oldwin->w_حالت_اونچائی = statusline_اونچائی(oldwin);
	    oldwin_اونچائی -= oldwin->w_حالت_اونچائی;
	}
	// Estimated stl height for the new window.
	صحیح new_win_stlh = need_حالت != 0 ? need_حالت
			: statusline_اونچائی(new_wp != تہی ? new_wp : curwin);
	اگر (new_سائز == 0)
	    new_سائز = (oldwin_اونچائی - new_win_stlh + 1) / 2;
	اگر (new_سائز > دستیاب - minheight - new_win_stlh)
	    new_سائز = دستیاب - minheight - new_win_stlh;
	اگر (new_سائز < wmh1)
	    new_سائز = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	اگر (oldwin_اونچائی - new_سائز - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	اگر (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    سیٹ_fraction(oldwin);
	    did_سیٹ_fraction = TRUE;

	    win_setheight_win(oldwin->w_اونچائی + new_سائز
		    + statusline_اونچائی(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_اونچائی = oldwin->w_اونچائی;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	اگر (!do_equal && p_ea && سائز == 0 && *p_ead != 'h'
	    && oldwin->w_چوکھٹا->fr_والد != تہی)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    اگر ((flags & WSP_چوٹی) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_نیچے)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	اگر (new_wp == تہی)
	    wp = win_alloc(oldwin, FALSE);
	ورنہ
	    win_append(oldwin, wp);
    }
    ورنہ
    {
	اگر (new_wp == تہی)
	    wp = win_alloc(oldwin->w_پچھلا, FALSE);
	ورنہ
	    win_append(oldwin->w_پچھلا, wp);
    }

    اگر (new_wp == تہی)
    {
	اگر (wp == تہی)
	    جاؤ theend;

	new_چوکھٹا(wp);
	اگر (wp->w_چوکھٹا == تہی)
	{
	    win_free(wp, تہی);
	    جاؤ theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    اگر (مقصد_flatten != تہی)
	چوکھٹا_flatten(مقصد_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    اگر (flags & (WSP_چوٹی | WSP_BOT))
    {
	اگر ((topframe->fr_layout == FR_ستون && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_صف && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_بچہ;
	    اگر (flags & WSP_BOT)
		جب (curfrp->fr_اگلا != تہی)
		    curfrp = curfrp->fr_اگلا;
	}
	ورنہ
	    curfrp = topframe;
	before = (flags & WSP_چوٹی);
    }
    ورنہ
    {
	curfrp = oldwin->w_چوکھٹا;
	اگر (flags & WSP_نیچے)
	    before = FALSE;
	ورنہ اگر (flags & WSP_ABOVE)
	    before = TRUE;
	ورنہ اگر (flags & WSP_VERT)
	    before = !p_spr;
	ورنہ
	    before = !p_sb;
    }
    اگر (curfrp->fr_والد == تہی || curfrp->fr_والد->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(چوکھٹا_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_والد = curfrp;
	frp->fr_اگلا = تہی;
	frp->fr_پچھلا = تہی;
	curfrp->fr_بچہ = frp;
	curfrp->fr_win = تہی;
	curfrp = frp;
	اگر (frp->fr_win != تہی)
	    oldwin->w_چوکھٹا = frp;
	ورنہ
	    FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
		frp->fr_والد = curfrp;
    }

    اگر (new_wp == تہی)
	frp = wp->w_چوکھٹا;
    ورنہ
	frp = new_wp->w_چوکھٹا;
    frp->fr_والد = curfrp->fr_والد;

    // Insert the new frame at the right place in the frame list.
    اگر (before)
	چوکھٹا_داخل_کرو(curfrp, frp);
    ورنہ
	چوکھٹا_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    اگر (!did_سیٹ_fraction)
	سیٹ_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    اگر (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	اگر (need_حالت)
	{
	    win_new_اونچائی(oldwin, oldwin->w_اونچائی - need_حالت);
	    oldwin->w_حالت_اونچائی = need_حالت;
	}
	اگر (flags & (WSP_چوٹی | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    صحیح stl_اونچائی = p_ls > 0 ? statusline_اونچائی(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_اونچائی();
	    win_new_اونچائی(wp, curfrp->fr_اونچائی - stl_اونچائی
							  - WINBAR_اونچائی(wp));
	    wp->w_حالت_اونچائی = stl_اونچائی;
	}
	ورنہ
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_اونچائی(wp, VISIBLE_اونچائی(oldwin));
	    wp->w_حالت_اونچائی = oldwin->w_حالت_اونچائی;
	}
	frp->fr_اونچائی = curfrp->fr_اونچائی;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_چوڑائی(wp, new_سائز);
	اگر (before)
	    wp->w_vsep_چوڑائی = 1;
	ورنہ
	{
	    wp->w_vsep_چوڑائی = oldwin->w_vsep_چوڑائی;
	    oldwin->w_vsep_چوڑائی = 1;
	}
	اگر (flags & (WSP_چوٹی | WSP_BOT))
	{
	    اگر (flags & WSP_BOT)
		چوکھٹا_سیٹ_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    چوکھٹا_new_چوڑائی(curfrp, curfrp->fr_چوڑائی
		     - (new_سائز + ((flags & WSP_چوٹی) != 0)), flags & WSP_چوٹی,
								       FALSE);
	}
	ورنہ
	    win_new_چوڑائی(oldwin, oldwin->w_چوڑائی - (new_سائز + 1));
	اگر (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_سائز + 1;
	}
	ورنہ		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_چوڑائی + 1;
	چوکھٹا_fix_چوڑائی(oldwin);
	چوکھٹا_fix_چوڑائی(wp);
    }
    ورنہ
    {
	// width and column of new window is same as current window
	اگر (flags & (WSP_چوٹی | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_چوڑائی(wp, topframe->fr_چوڑائی);
	    wp->w_vsep_چوڑائی = 0;
	}
	ورنہ
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_چوڑائی(wp, oldwin->w_چوڑائی);
	    wp->w_vsep_چوڑائی = oldwin->w_vsep_چوڑائی;
	}
	frp->fr_چوڑائی = curfrp->fr_چوڑائی;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_اونچائی(wp, new_سائز);
	صحیح old_حالت_اونچائی = oldwin->w_حالت_اونچائی;
	اگر (flags & (WSP_چوٹی | WSP_BOT))
	{
	    صحیح new_fr_اونچائی = curfrp->fr_اونچائی - new_سائز
							  + WINBAR_اونچائی(wp);

	    اگر (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_اونچائی -= statusline_اونچائی(curfrp->fr_win);
	    اگر (flags & WSP_BOT)
		چوکھٹا_جوڑو_statusline(curfrp);
	    چوکھٹا_new_اونچائی(curfrp, new_fr_اونچائی, flags & WSP_چوٹی, FALSE,
									FALSE);
	}
	ورنہ
	    win_new_اونچائی(oldwin, oldwin_اونچائی - (new_سائز
			+ statusline_اونچائی(wp)));
	اگر (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_حالت_اونچائی = statusline_اونچائی(wp);
	    oldwin->w_winrow += wp->w_اونچائی + statusline_اونچائی(wp);
	}
	ورنہ		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_اونچائی(oldwin)
						    + statusline_اونچائی(oldwin);
	    wp->w_حالت_اونچائی = old_حالت_اونچائی;
	    اگر (!(flags & WSP_BOT))
		oldwin->w_حالت_اونچائی = statusline_اونچائی(oldwin);
	}
	چوکھٹا_fix_اونچائی(wp);
	چوکھٹا_fix_اونچائی(oldwin);
    }

    اگر (flags & (WSP_چوٹی | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    حالت_redraw_all();

    اگر (need_حالت)
    {
	پیغام_صف = صفیں - 1;
	پیغام_ستون = sc_ستون;
	پیغام_clr_eos_force();	// Old command/ruler may still be there
	comp_ستون();
	پیغام_صف = صفیں - 1;
	پیغام_ستون = 0;	// put position back at start of line
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
    ورنہ اگر (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    اگر (flags & WSP_VERT)
    {
	i = p_wiw;
	اگر (سائز != 0)
	    p_wiw = سائز;

#اگر_تعریف_شدہ FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	اگر (gui.اندر_use)
	    gui_init_which_components(تہی);
#اگر_ختم
    }
    ورنہ
    {
	i = p_wh;
	اگر (سائز != 0)
	    p_wh = سائز;
    }

    /*
     * make the new window the current window
     */
    (خالی)win_enter_ext(wp, (new_wp == تہی ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    اگر (flags & WSP_VERT)
	p_wiw = i;
    ورنہ
	p_wh = i;
    retval = OK;

theend:
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    واپس retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    ساکن خالی
win_init(win_T *newp, win_T *oldp, صحیح flags غیر_مستعمل)
{
    صحیح		i;

    newp->w_بفر = oldp->w_بفر;
#اگر_تعریف_شدہ FEAT_SYN_HL
    newp->w_s = &(oldp->w_بفر->b_s);
#اگر_ختم
    oldp->w_بفر->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_سیٹ_curswant = oldp->w_سیٹ_curswant;
    newp->w_topline = oldp->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#اگر_ختم
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_پچھلا_pcmark = oldp->w_پچھلا_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_پچھلا_fraction_صف = oldp->w_پچھلا_fraction_صف;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#اگر_تعریف_شدہ FEAT_QUICKFIX
    اگر (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = تہی;
	newp->w_llist_ref = تہی;
    }
    ورنہ
	copy_loclist_ڈھیر(oldp, newp);
#اگر_ختم
    newp->w_localdir = (oldp->w_localdir == تہی)
				    ? تہی : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == تہی)
				    ? تہی : vim_strsave(oldp->w_prevdir);

    اگر (*p_spk != 'c')
    {
	اگر (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_پچھلا_اونچائی = oldp->w_اونچائی - WINBAR_اونچائی(oldp);
	newp->w_پچھلا_winrow = oldp->w_winrow + 2 * WINBAR_اونچائی(oldp);
    }

    // copy tagstack and folds
    برائے (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*لیبل = &newp->w_tagstack[i];
	*لیبل = oldp->w_tagstack[i];
	اگر (لیبل->tagname != تہی)
	    لیبل->tagname = vim_strsave(لیبل->tagname);
	اگر (لیبل->user_ڈیٹا != تہی)
	    لیبل->user_ڈیٹا = vim_strsave(لیبل->user_ڈیٹا);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#اگر_تعریف_شدہ FEAT_FOLDING
    copyFoldingState(oldp, newp);
#اگر_ختم

    win_init_some(newp, oldp);

#اگر_تعریف_شدہ FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = ڈالو_highlight_overrides(newp->w_hl, newp->w_hl_len);
	اگر (newp->w_بفر->b_رکن != تہی)
	    رکن_init_default_colors(newp->w_بفر->b_رکن);
	اگر (pushed)
	    نکالو_highlight_overrides();
    }
#اگر_ختم
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    ساکن خالی
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
win_valid_popup(win_T *win غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	اگر (wp == win)
	    واپس TRUE;
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر (wp == win)
	    واپس TRUE;
#اگر_ختم
    واپس FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    صحیح
win_valid(win_T *win)
{
    win_T	*wp;

    اگر (win == تہی)
	واپس FALSE;
    FOR_ALL_WINDOWS(wp)
	اگر (wp == win)
	    واپس TRUE;
    واپس win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_ڈھونڈو_by_id(صحیح id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_id == id)
	    واپس wp;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_id == id)
	    واپس wp;
    FOR_ALL_POPUPWINS_اندر_TAB(curtab, wp)
	اگر (wp->w_id == id)
	    واپس wp;
#اگر_ختم
    واپس تہی;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    صحیح
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    اگر (win == تہی)
	واپس FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
	{
	    اگر (wp == win)
		واپس TRUE;
	}
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
	    اگر (wp == win)
		واپس TRUE;
#اگر_ختم
    }
    واپس win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    صحیح
win_گنتی(خالی)
{
    win_T	*wp;
    صحیح		گنتی = 0;

    FOR_ALL_WINDOWS(wp)
	++گنتی;
    واپس گنتی;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    صحیح
make_windows(
    صحیح		گنتی,
    صحیح		vertical)  // split windows vertically if TRUE
{
    صحیح		maxcount;
    صحیح		todo;

    اگر (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_چوڑائی + curwin->w_vsep_چوڑائی
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    ورنہ
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_اونچائی(curwin) + curwin->w_حالت_اونچائی
			- (p_wh - p_wmh)) / (p_wmh + statusline_اونچائی(curwin));
    }

    اگر (maxcount < 2)
	maxcount = 2;
    اگر (گنتی > maxcount)
	گنتی = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    اگر (گنتی > 1)
	آخری_حالت(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    بلاک_autocmds();

    // todo is number of windows left to create
    برائے (todo = گنتی - 1; todo > 0; --todo)
	اگر (vertical)
	{
	    اگر (win_بانٹو(curwin->w_چوڑائی - (curwin->w_چوڑائی - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		توڑو;
	}
	ورنہ
	{
	    اگر (win_بانٹو(curwin->w_اونچائی - (curwin->w_اونچائی - todo
			    * statusline_اونچائی(curwin)) / (todo + 1)
			- statusline_اونچائی(curwin), WSP_ABOVE) == FAIL)
		توڑو;
	}

    unblock_autocmds();

    // return actual number of windows
    واپس (گنتی - todo);
}

/*
 * Exchange current and next window
 */
    ساکن خالی
win_exchange(طویل Prenum)
{
    چوکھٹا_T	*frp;
    چوکھٹا_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    صحیح		عارضی;

    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;
    اگر (ONE_کھڑکی)	    // just one window
    {
	beep_flush();
	واپس;
    }
    اگر (متن_or_buf_locked())
    {
	beep_flush();
	واپس;
    }

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    /*
     * find window to exchange with
     */
    اگر (Prenum)
    {
	frp = curwin->w_چوکھٹا->fr_والد->fr_بچہ;
	جب (frp != تہی && --Prenum > 0)
	    frp = frp->fr_اگلا;
    }
    ورنہ اگر (curwin->w_چوکھٹا->fr_اگلا != تہی)	// Swap with next
	frp = curwin->w_چوکھٹا->fr_اگلا;
    ورنہ    // Swap last window in row/col with previous
	frp = curwin->w_چوکھٹا->fr_پچھلا;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    اگر (frp == تہی || frp->fr_win == تہی || frp->fr_win == curwin)
	واپس;
    wp = frp->fr_win;

/*
 * 1. remove curwin from the list. Remember after which window it was in wp2
 * 2. insert curwin before wp in the list
 * if wp != wp2
 *    3. remove wp from the list
 *    4. insert wp after wp2
 * 5. exchange the status line height and vsep width.
 */
    wp2 = curwin->w_پچھلا;
    frp2 = curwin->w_چوکھٹا->fr_پچھلا;
    اگر (wp->w_پچھلا != curwin)
    {
	win_remove(curwin, تہی);
	چوکھٹا_remove(curwin->w_چوکھٹا);
	win_append(wp->w_پچھلا, curwin);
	چوکھٹا_داخل_کرو(frp, curwin->w_چوکھٹا);
    }
    اگر (wp != wp2)
    {
	win_remove(wp, تہی);
	چوکھٹا_remove(wp->w_چوکھٹا);
	win_append(wp2, wp);
	اگر (frp2 == تہی)
	    چوکھٹا_داخل_کرو(wp->w_چوکھٹا->fr_والد->fr_بچہ, wp->w_چوکھٹا);
	ورنہ
	    چوکھٹا_append(frp2, wp->w_چوکھٹا);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    عارضی = curwin->w_حالت_اونچائی;
    اگر (عارضی != wp->w_حالت_اونچائی)
    {
	win_new_اونچائی(curwin, curwin->w_اونچائی + عارضی - wp->w_حالت_اونچائی);
	win_new_اونچائی(wp, wp->w_اونچائی + wp->w_حالت_اونچائی - عارضی);
    }
    curwin->w_حالت_اونچائی = wp->w_حالت_اونچائی;
    wp->w_حالت_اونچائی = عارضی;
    عارضی = curwin->w_vsep_چوڑائی;
    curwin->w_vsep_چوڑائی = wp->w_vsep_چوڑائی;
    wp->w_vsep_چوڑائی = عارضی;

    چوکھٹا_fix_اونچائی(curwin);
    چوکھٹا_fix_اونچائی(wp);
    چوکھٹا_fix_چوڑائی(curwin);
    چوکھٹا_fix_چوڑائی(wp);

    win_comp_pos();		// recompute window positions
#اگر defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اونچائی();
#اگر_ختم

    اگر (wp->w_بفر != curbuf)
	reset_VIsual_and_resel();
    ورنہ اگر (VIsual_فعال)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    ساکن خالی
win_rotate(صحیح upwards, صحیح گنتی)
{
    win_T	*wp1;
    win_T	*wp2;
    چوکھٹا_T	*frp;
    صحیح		n;

    اگر (ONE_کھڑکی)		// nothing to do
    {
	beep_flush();
	واپس;
    }

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    // Check if all frames in this row/col have one window.
    FOR_ALL_چوکھٹے(frp, curwin->w_چوکھٹا->fr_والد->fr_بچہ)
	اگر (frp->fr_win == تہی)
	{
	    emsg(_(e_cannot_rotate_when_another_کھڑکی_is_بانٹو));
	    واپس;
	}

    جب (گنتی--)
    {
	اگر (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_چوکھٹا->fr_والد->fr_بچہ;
	    wp1 = frp->fr_win;
	    win_remove(wp1, تہی);
	    چوکھٹا_remove(frp);

	    // find last frame and append removed window/frame after it
	    برائے ( ; frp->fr_اگلا != تہی; frp = frp->fr_اگلا)
		;
	    win_append(frp->fr_win, wp1);
	    چوکھٹا_append(frp, wp1->w_چوکھٹا);

	    wp2 = frp->fr_win;		// previously last window
	}
	ورنہ			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    برائے (frp = curwin->w_چوکھٹا; frp->fr_اگلا != تہی;
							   frp = frp->fr_اگلا)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_پچھلا;		    // will become last window
	    win_remove(wp1, تہی);
	    چوکھٹا_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_والد->fr_بچہ->fr_win->w_پچھلا, wp1);
	    چوکھٹا_داخل_کرو(frp->fr_والد->fr_بچہ, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_حالت_اونچائی;
	اگر (n != wp1->w_حالت_اونچائی)
	{
	    win_new_اونچائی(wp2, wp2->w_اونچائی + n - wp1->w_حالت_اونچائی);
	    win_new_اونچائی(wp1, wp1->w_اونچائی + wp1->w_حالت_اونچائی - n);
	}
	wp2->w_حالت_اونچائی = wp1->w_حالت_اونچائی;
	wp1->w_حالت_اونچائی = n;
	چوکھٹا_fix_اونچائی(wp1);
	چوکھٹا_fix_اونچائی(wp2);
	n = wp2->w_vsep_چوڑائی;
	wp2->w_vsep_چوڑائی = wp1->w_vsep_چوڑائی;
	wp1->w_vsep_چوڑائی = n;
	چوکھٹا_fix_چوڑائی(wp1);
	چوکھٹا_fix_چوڑائی(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#اگر defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اونچائی();
#اگر_ختم

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    صحیح
win_splitmove(win_T *wp, صحیح سائز, صحیح flags)
{
    صحیح		dir;
    صحیح		اونچائی = wp->w_اونچائی;
    چوکھٹا_T	*unflat_altfr;

    اگر (ONE_کھڑکی)
	واپس OK;	// nothing to do
    اگر (check_بانٹو_disallowed(wp) == FAIL)
	واپس FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, تہی, &unflat_altfr);
    win_remove(wp, تہی);
    آخری_حالت(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    اگر (win_بانٹو_ins(سائز, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_پچھلا, wp);
	واپس FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    اگر (سائز == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(اونچائی, wp);
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
#اگر_ختم
    واپس OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    خالی
win_move_after(win_T *win1, win_T *win2)
{
    صحیح		اونچائی;

    // check if the arguments are reasonable
    اگر (win1 == win2)
	واپس;

    // check if there is something to do
    اگر (win2->w_اگلا != win1)
    {
	اگر (win1->w_چوکھٹا->fr_والد != win2->w_چوکھٹا->fr_والد)
	{
	    iemsg("Trying to move a window into another frame");
	    واپس;
	}

	// may need to move the status line/vertical separator of the last
	//کھڑکی
	اگر (win1 == lastwin)
	{
	    اونچائی = win1->w_پچھلا->w_حالت_اونچائی;
	    win1->w_پچھلا->w_حالت_اونچائی = win1->w_حالت_اونچائی;
	    win1->w_حالت_اونچائی = اونچائی;
	    اگر (win1->w_پچھلا->w_vsep_چوڑائی == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_پچھلا->w_vsep_چوڑائی = 0;
		win1->w_پچھلا->w_چوکھٹا->fr_چوڑائی -= 1;
		win1->w_vsep_چوڑائی = 1;
		win1->w_چوکھٹا->fr_چوڑائی += 1;
	    }
	}
	ورنہ اگر (win2 == lastwin)
	{
	    اونچائی = win1->w_حالت_اونچائی;
	    win1->w_حالت_اونچائی = win2->w_حالت_اونچائی;
	    win2->w_حالت_اونچائی = اونچائی;
	    اگر (win1->w_vsep_چوڑائی == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_چوڑائی = 1;
		win2->w_چوکھٹا->fr_چوڑائی += 1;
		win1->w_vsep_چوڑائی = 0;
		win1->w_چوکھٹا->fr_چوڑائی -= 1;
	    }
	}
	win_remove(win1, تہی);
	چوکھٹا_remove(win1->w_چوکھٹا);
	win_append(win2, win1);
	چوکھٹا_append(win2->w_چوکھٹا, win1->w_چوکھٹا);

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
    خالی
win_equal(
    win_T	*اگلا_curwin,	// pointer to current window to be or NULL
    صحیح		current,	// do only frame with current window
    صحیح		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    اگر (dir == 0)
	dir = *p_ead;
    win_equal_rec(اگلا_curwin == تہی ? curwin : اگلا_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_اونچائی(),
		      topframe->fr_چوڑائی, topframe->fr_اونچائی);
#اگر defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اونچائی();
#اگر_ختم
    اگر (!is_aucmd_win(اگلا_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    ساکن خالی
win_equal_rec(
    win_T	*اگلا_curwin,	// pointer to current window to be or NULL
    صحیح		current,	// do only frame with current window
    چوکھٹا_T	*topfr,		// frame to set size off
    صحیح		dir,		// 'v', 'h' or 'b', see win_equal()
    صحیح		ستون,		// horizontal position for frame
    صحیح		صف,		// vertical position for frame
    صحیح		چوڑائی,		// new width of frame
    صحیح		اونچائی)		// new height of frame
{
    صحیح		n, m;
    صحیح		extra_sep = 0;
    صحیح		wincount, totwincount = 0;
    چوکھٹا_T	*fr;
    صحیح		اگلا_curwin_سائز = 0;
    صحیح		گنجائش = 0;
    صحیح		new_سائز;
    صحیح		has_اگلا_curwin = 0;
    صحیح		hnc;

    اگر (topfr->fr_layout == FR_برگ)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	اگر (topfr->fr_اونچائی != اونچائی || topfr->fr_win->w_winrow != صف
		|| topfr->fr_چوڑائی != چوڑائی || topfr->fr_win->w_wincol != ستون
	   )
	{
	    topfr->fr_win->w_winrow = صف;
	    چوکھٹا_new_اونچائی(topfr, اونچائی, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = ستون;
	    چوکھٹا_new_چوڑائی(topfr, چوڑائی, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    ورنہ اگر (topfr->fr_layout == FR_صف)
    {
	topfr->fr_چوڑائی = چوڑائی;
	topfr->fr_اونچائی = اونچائی;

	اگر (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = چوکھٹا_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    اگر (ستون + چوڑائی >= firstwin->w_wincol + topframe->fr_چوڑائی)
		extra_sep = 1;
	    ورنہ
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_اگلا_curwin = چوکھٹا_has_win(topfr, اگلا_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = چوکھٹا_minwidth(topfr, اگلا_curwin);
	    گنجائش = چوڑائی - m;
	    اگر (گنجائش < 0)
	    {
		اگلا_curwin_سائز = p_wiw + گنجائش;
		گنجائش = 0;
	    }
	    ورنہ
	    {
		اگلا_curwin_سائز = -1;
		FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
		{
		    اگر (!چوکھٹا_fixed_چوڑائی(fr))
			جاری;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = چوکھٹا_minwidth(fr, NOWIN);
		    new_سائز = fr->fr_چوڑائی;
		    اگر (چوکھٹا_has_win(fr, اگلا_curwin))
		    {
			گنجائش += p_wiw - p_wmw;
			اگلا_curwin_سائز = 0;
			اگر (new_سائز < p_wiw)
			    new_سائز = p_wiw;
		    }
		    ورنہ
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_اگلا == تہی
					? extra_sep : 0)) / (p_wmw + 1);
		    گنجائش -= new_سائز - n;
		    اگر (گنجائش < 0)
		    {
			new_سائز += گنجائش;
			گنجائش = 0;
		    }
		    fr->fr_newwidth = new_سائز;
		}
		اگر (اگلا_curwin_سائز == -1)
		{
		    اگر (!has_اگلا_curwin)
			اگلا_curwin_سائز = 0;
		    ورنہ اگر (totwincount > 1
			    && (گنجائش + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			اگلا_curwin_سائز = (گنجائش + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			گنجائش -= اگلا_curwin_سائز - p_wiw;
		    }
		    ورنہ
			اگلا_curwin_سائز = p_wiw;
		}
	    }

	    اگر (has_اگلا_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
	{
	    wincount = 1;
	    اگر (fr->fr_اگلا == تہی)
		// last frame gets all that remains (avoid roundoff error)
		new_سائز = چوڑائی;
	    ورنہ اگر (dir == 'v')
		new_سائز = fr->fr_چوڑائی;
	    ورنہ اگر (چوکھٹا_fixed_چوڑائی(fr))
	    {
		new_سائز = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ورنہ
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = چوکھٹا_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_اگلا == تہی ? extra_sep : 0))
								/ (p_wmw + 1);
		m = چوکھٹا_minwidth(fr, اگلا_curwin);
		اگر (has_اگلا_curwin)
		    hnc = چوکھٹا_has_win(fr, اگلا_curwin);
		ورنہ
		    hnc = FALSE;
		اگر (hnc)	    // don't count next_curwin
		    --wincount;
		اگر (totwincount == 0)
		    new_سائز = گنجائش;
		ورنہ
		    new_سائز = (wincount * گنجائش + ((غیرعلامتی)totwincount >> 1))
								/ totwincount;
		اگر (hnc)	    // add next_curwin size
		{
		    اگلا_curwin_سائز -= p_wiw - (m - n);
		    اگر (اگلا_curwin_سائز < 0)
			اگلا_curwin_سائز = 0;
		    new_سائز += اگلا_curwin_سائز;
		    گنجائش -= new_سائز - اگلا_curwin_سائز;
		}
		ورنہ
		    گنجائش -= new_سائز;
		new_سائز += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    اگر (!current || dir != 'v' || topfr->fr_والد != تہی
		    || (new_سائز != fr->fr_چوڑائی)
		    || چوکھٹا_has_win(fr, اگلا_curwin))
		win_equal_rec(اگلا_curwin, current, fr, dir, ستون, صف,
							    new_سائز, اونچائی);
	    ستون += new_سائز;
	    چوڑائی -= new_سائز;
	    totwincount -= wincount;
	}
    }
    ورنہ // topfr->fr_layout == FR_COL
    {
	topfr->fr_چوڑائی = چوڑائی;
	topfr->fr_اونچائی = اونچائی;

	اگر (dir != 'h')			// equalize frame heights
	{
	    totwincount = چوکھٹا_wincount_اندر_اونچائی(topfr);
	    has_اگلا_curwin = چوکھٹا_has_win(topfr, اگلا_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = چوکھٹا_minheight(topfr, اگلا_curwin);
	    گنجائش = اونچائی - m;
	    اگر (گنجائش < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		اگلا_curwin_سائز = p_wh + گنجائش;
		گنجائش = 0;
	    }
	    ورنہ
	    {
		اگلا_curwin_سائز = -1;
		FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
		{
		    اگر (!چوکھٹا_fixed_اونچائی(fr))
			جاری;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = چوکھٹا_minheight(fr, NOWIN);
		    new_سائز = fr->fr_اونچائی;
		    اگر (چوکھٹا_has_win(fr, اگلا_curwin))
		    {
			گنجائش += p_wh - p_wmh;
			اگلا_curwin_سائز = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			اگر (new_سائز < n + (p_wh - p_wmh))
			    new_سائز = n + (p_wh - p_wmh);
		    }
		    ورنہ
			// These windows don't use up room.
			totwincount -= چوکھٹا_wincount_اندر_اونچائی(fr);
		    گنجائش -= new_سائز - n;
		    اگر (گنجائش < 0)
		    {
			new_سائز += گنجائش;
			گنجائش = 0;
		    }
		    fr->fr_newheight = new_سائز;
		}
		اگر (اگلا_curwin_سائز == -1)
		{
		    اگر (!has_اگلا_curwin)
			اگلا_curwin_سائز = 0;
		    ورنہ اگر (totwincount > 1
			    && (گنجائش + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			اگلا_curwin_سائز = (گنجائش + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			گنجائش -= اگلا_curwin_سائز - p_wh;
		    }
		    ورنہ
			اگلا_curwin_سائز = p_wh;
		}
	    }

	    اگر (has_اگلا_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_چوکھٹے(fr, topfr->fr_بچہ)
	{
	    wincount = 1;
	    اگر (fr->fr_اگلا == تہی)
		// last frame gets all that remains (avoid roundoff error)
		new_سائز = اونچائی;
	    ورنہ اگر (dir == 'h')
		new_سائز = fr->fr_اونچائی;
	    ورنہ اگر (چوکھٹا_fixed_اونچائی(fr))
	    {
		new_سائز = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    ورنہ
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = چوکھٹا_minheight(fr, NOWIN);
		wincount = چوکھٹا_wincount_اندر_اونچائی(fr);
		m = چوکھٹا_minheight(fr, اگلا_curwin);
		اگر (has_اگلا_curwin)
		    hnc = چوکھٹا_has_win(fr, اگلا_curwin);
		ورنہ
		    hnc = FALSE;
		اگر (hnc)	    // don't count next_curwin
		    --wincount;
		اگر (totwincount == 0)
		    new_سائز = گنجائش;
		ورنہ
		    new_سائز = (wincount * گنجائش + ((غیرعلامتی)totwincount >> 1))
								/ totwincount;
		اگر (hnc)	    // add next_curwin size
		{
		    اگلا_curwin_سائز -= p_wh - (m - n);
		    new_سائز += اگلا_curwin_سائز;
		    گنجائش -= new_سائز - اگلا_curwin_سائز;
		}
		ورنہ
		    گنجائش -= new_سائز;
		new_سائز += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    اگر (!current || dir != 'h' || topfr->fr_والد != تہی
		    || (new_سائز != fr->fr_اونچائی)
		    || چوکھٹا_has_win(fr, اگلا_curwin))
		win_equal_rec(اگلا_curwin, current, fr, dir, ستون, صف,
							     چوڑائی, new_سائز);
	    صف += new_سائز;
	    اونچائی -= new_سائز;
	    totwincount -= wincount;
	}
    }
}

#اگر defined(FEAT_کام_CHANNEL)
    خالی
leaving_کھڑکی(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    اگر (!bt_prompt(win->w_بفر) || is_aucmd_win(win))
	واپس;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_بفر->b_prompt_داخل_کرو = restart_edit;
    اگر (restart_edit != 0 && منوال_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    اگر ((State & منوال_داخل_کرو) && !stop_داخل_کرو_منوال)
    {
	stop_داخل_کرو_منوال = TRUE;
	اگر (win->w_بفر->b_prompt_داخل_کرو == NUL)
	    win->w_بفر->b_prompt_داخل_کرو = 'A';
    }
}

    خالی
entering_کھڑکی(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    اگر (!bt_prompt(win->w_بفر) || is_aucmd_win(win))
	واپس;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    اگر (win->w_بفر->b_prompt_داخل_کرو != NUL)
	stop_داخل_کرو_منوال = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    اگر ((State & منوال_داخل_کرو) == 0)
	restart_edit = win->w_بفر->b_prompt_داخل_کرو;
}
#اگر_ختم

    ساکن خالی
win_init_خالی_خانے(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_سطریں_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.ستون = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.ستون = 0;
    wp->w_پچھلا_pcmark.lnum = 0;
    wp->w_پچھلا_pcmark.ستون = 0;
    wp->w_topline = 1;
#اگر_تعریف_شدہ FEAT_DIFF
    wp->w_topfill = 0;
#اگر_ختم
    wp->w_botline = 2;
    wp->w_valid = 0;
#اگر defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_بفر->b_s;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_reset_hlfwin(wp);
#اگر_ختم
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    خالی
curwin_init(خالی)
{
    win_init_خالی_خانے(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    خالی
close_windows(
    buf_T	*buf,
    صحیح		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    صحیح		گنتی = tabpage_اشاریہ(تہی);

    ++RedrawingDisabled;

    برائے (wp = firstwin; wp != تہی && !ONE_کھڑکی; )
    {
	اگر (wp->w_بفر == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_بفر->b_locked > 0))
	{
	    اگر (کھڑکی_layout_locked(CMD_سائز))
		جاؤ theend;  // Only give one error message.
	    اگر (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		توڑو;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	ورنہ
	    wp = wp->w_اگلا;
    }

    // Also check windows in other tab pages.
    برائے (tp = پہلا_tabpage; tp != تہی; tp = nexttp)
    {
	nexttp = tp->tp_اگلا;
	اگر (tp != curtab)
	    FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
		اگر (wp->w_بفر == buf
		    && !(win_locked(wp) || wp->w_بفر->b_locked > 0))
		{
		    اگر (کھڑکی_layout_locked(CMD_سائز))
			جاؤ theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = پہلا_tabpage;
		    توڑو;
		}
    }

theend:
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;

    اگر (گنتی != tabpage_اشاریہ(تہی))
	apply_autocmds(EVENT_TABCLOSED, تہی, تہی, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    صحیح
آخری_کھڑکی(خالی)
{
    واپس (one_کھڑکی() && پہلا_tabpage->tp_اگلا == تہی);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    صحیح
one_کھڑکی(خالی)
{
    win_T	*wp;
    صحیح		نظر_آیا_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	اگر (!is_aucmd_win(wp))
	{
	    اگر (نظر_آیا_one)
		واپس FALSE;
	    نظر_آیا_one = TRUE;
	}
    }
    واپس TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    ساکن صحیح
close_آخری_کھڑکی_tabpage(
    win_T	*win,
    صحیح		free_buf,
    tabpage_T   *پچھلا_curtab)
{
    اگر (!ONE_کھڑکی)
	واپس FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_بفر != تہی);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    اگر (valid_tabpage(پچھلا_curtab) && پچھلا_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, پچھلا_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    اگر (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    entering_کھڑکی(curwin);
#اگر_ختم
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, تہی, تہی, FALSE, curbuf);
#اگر defined(FEAT_TABPANEL)
    اگر (p_stpl > 0)
	shell_new_columns();
#اگر_ختم
    apply_autocmds(EVENT_WINENTER, تہی, تہی, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, تہی, تہی, FALSE, curbuf);
    اگر (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf);
    واپس TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    ساکن خالی
win_close_بفر(win_T *win, صحیح action, صحیح abort_if_آخری)
{
#اگر_تعریف_شدہ FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    اگر (win->w_بفر != تہی)
	reset_synblock(win);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    اگر (win->w_بفر != تہی && bt_quickfix(win->w_بفر)
					&& win->w_بفر->b_nwindows == 1)
	win->w_بفر->b_p_bl = FALSE;
#اگر_ختم

    // Close the link to the buffer.
    اگر (win->w_بفر != تہی)
    {
	bufref_T    bufref;

	سیٹ_bufref(&bufref, curbuf);
	++win->w_locked;
	close_بفر(win, win->w_بفر, action, abort_if_آخری, TRUE, TRUE);
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
    صحیح		other_بفر = FALSE;
    صحیح		close_curwin = FALSE;
    صحیح		dir;
    صحیح		help_کھڑکی = FALSE;
    صحیح		quickfix_کھڑکی = FALSE;
    tabpage_T   *پچھلا_curtab = curtab;
    چوکھٹا_T	*win_چوکھٹا = win->w_چوکھٹا->fr_والد;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		had_diffmode = win->w_p_diff;
#اگر_ختم
#اگر_تعریف_شدہ پیغام_قطار
    صحیح		did_decrement = FALSE;
#اگر_ختم

#اگر defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    اگر (may_close_رکن_popup() == OK)
	واپس OK;
#اگر_ختم
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس FAIL;

    اگر (آخری_کھڑکی())
    {
	emsg(_(e_cannot_close_آخری_کھڑکی));
	واپس FAIL;
    }
    اگر (کھڑکی_layout_locked(CMD_close))
	واپس FAIL;

    اگر (win_locked(win) || (win->w_بفر != تہی
					       && win->w_بفر->b_locked > 0))
	واپس FAIL; // window is already being closed
    اگر (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_کھڑکی));
	واپس FAIL;
    }
    اگر ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_کھڑکی())
    {
	emsg(_(e_cannot_close_کھڑکی_only_autocmd_کھڑکی_would_remain));
	واپس FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    اگر (close_آخری_کھڑکی_tabpage(win, free_buf, پچھلا_curtab))
	واپس FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    اگر (bt_help(win->w_بفر))
	help_کھڑکی = TRUE;
    ورنہ
	clear_snapshot(curtab, SNAP_HELP_IDX);

    اگر (bt_quickfix(win->w_بفر))
	quickfix_کھڑکی = TRUE;
    ورنہ
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    اگر (win == curwin)
    {
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	leaving_کھڑکی(curwin);
#اگر_ختم
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, تہی));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	اگر (wp->w_بفر != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_بفر = TRUE;
	    اگر (!win_valid(win))
		واپس FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, تہی, تہی, FALSE, curbuf);
	    اگر (!win_valid(win))
		واپس FAIL;
	    --win->w_locked;
	    اگر (آخری_کھڑکی())
		واپس FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, تہی, تہی, FALSE, curbuf);
	اگر (!win_valid(win))
	    واپس FAIL;
	--win->w_locked;
	اگر (آخری_کھڑکی())
	    واپس FAIL;
#اگر_تعریف_شدہ FEAT_EVAL
	// autocmds may abort script processing
	اگر (aborting())
	    واپس FAIL;
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    اگر (gui.اندر_use)
	برآمد_flush();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (popup_win_closed(win) && !win_valid(win))
	واپس FAIL;
#اگر_ختم

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    اگر (!win_valid_any_tab(win))
	واپس OK;

    win_close_بفر(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    اگر (win_valid(win) && win->w_بفر == تہی
#اگر defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#اگر_ختم
	    && آخری_کھڑکی())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	اگر (curwin->w_بفر == تہی)
	    curwin->w_بفر = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    اگر (curtab != پچھلا_curtab && win_valid_any_tab(win)
						      && win->w_بفر == تہی)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, پچھلا_curtab);
	واپس FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    اگر (!win_valid(win) || آخری_کھڑکی()
	    || close_آخری_کھڑکی_tabpage(win, free_buf, پچھلا_curtab))
	واپس FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++بانٹو_disallowed;
#اگر_تعریف_شدہ پیغام_قطار
    ++dont_parse_messages;
#اگر_ختم

    اگر (win->w_بفر != تہی)
	--win->w_بفر->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, تہی);

    اگر (help_کھڑکی || quickfix_کھڑکی)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *پچھلا_win = get_snapshot_curwin(help_کھڑکی ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	اگر (win_valid(پچھلا_win))
	    wp = پچھلا_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    اگر (win == curwin)
    {
	curwin = wp;
#اگر_تعریف_شدہ FEAT_QUICKFIX
	اگر (wp->w_p_pvw || bt_quickfix(wp->w_بفر))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    برائے (;;)
	    {
		اگر (wp->w_اگلا == تہی)
		    wp = firstwin;
		ورنہ
		    wp = wp->w_اگلا;
		اگر (wp == curwin)
		    توڑو;
		اگر (!wp->w_p_pvw && !bt_quickfix(wp->w_بفر))
		{
		    curwin = wp;
		    توڑو;
		}
	    }
	}
#اگر_ختم
	curbuf = curwin->w_بفر;
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
    آخری_حالت(FALSE);

    اگر (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_چوکھٹا->fr_والد == win_چوکھٹا, dir);
    ورنہ
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    اگر (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#اگر_تعریف_شدہ پیغام_قطار
	did_decrement =
#ورنہ
	(خالی)
#اگر_ختم
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	اگر (other_بفر)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf);
    }

    اگر (ONE_کھڑکی && curwin->w_locked && curbuf->b_locked_بانٹو
	    && پہلا_tabpage->tp_اگلا != تہی)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, تہی, تہی, FALSE, curbuf);

    --بانٹو_disallowed;
#اگر_تعریف_شدہ پیغام_قطار
    اگر (!did_decrement)
	--dont_parse_messages;
#اگر_ختم

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    اگر (help_کھڑکی || quickfix_کھڑکی)
	restore_snapshot(help_کھڑکی ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#اگر_تعریف_شدہ FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    اگر (diffopt_closeoff() && had_diffmode && curtab == پچھلا_curtab)
    {
	صحیح	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    اگر (dwin->w_p_diff)
		++diffcount;
	اگر (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#اگر_ختم

#اگر defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    اگر (gui.اندر_use && !win_hasvertsplit())
	gui_init_which_components(تہی);
#اگر_ختم

    redraw_all_later(UPD_NOT_VALID);
    واپس OK;
}

    ساکن خالی
trigger_winnewpre(خالی)
{
    کھڑکی_layout_تالا();
    apply_autocmds(EVENT_WINNEWPRE, تہی, تہی, FALSE, تہی);
    کھڑکی_layout_unlock();
}

    ساکن خالی
trigger_winclosed(win_T *win)
{
    ساکن صحیح	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    اگر (recursive)
	واپس;
    recursive = TRUE;
    vim_snprintf((حرف *)winid, حجم(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_بفر);
    recursive = FALSE;
}

    خالی
trigger_tabclosedpre(tabpage_T *tp)
{
    ساکن صحیح	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    اگر (!has_tabclosedpre() || recursive)
	واپس;

    اگر (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    کھڑکی_layout_تالا();
    apply_autocmds(EVENT_TABCLOSEDPRE, تہی, تہی, FALSE, تہی);
    کھڑکی_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    اگر (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    ورنہ
	// fall back to the first tabpage
	goto_tabpage_tp(پہلا_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    خالی
snapshot_windows_scroll_سائز(خالی)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_آخری_topline = wp->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
	wp->w_آخری_topfill = wp->w_topfill;
#اگر_ختم
	wp->w_آخری_leftcol = wp->w_leftcol;
	wp->w_آخری_skipcol = wp->w_skipcol;
	wp->w_آخری_چوڑائی = wp->w_چوڑائی;
	wp->w_آخری_اونچائی = wp->w_اونچائی;
    }
}

ساکن صحیح did_initial_scroll_سائز_snapshot = FALSE;

    خالی
may_make_initial_scroll_سائز_snapshot(خالی)
{
    اگر (!did_initial_scroll_سائز_snapshot)
    {
	did_initial_scroll_سائز_snapshot = TRUE;
	snapshot_windows_scroll_سائز();
    }
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    ساکن dict_T *
make_win_info_dict(
	صحیح چوڑائی,
	صحیح اونچائی,
	صحیح topline,
# ifdef FEAT_DIFF
	صحیح topfill,
# endif
	صحیح leftcol,
	صحیح skipcol)
{
    dict_T *d = dict_alloc();
    اگر (d == تہی)
	واپس تہی;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    جب (1)
    {
	typval_T tv;
	tv.v_تالا = 0;
	tv.v_type = VAR_نمبر;

	tv.vval.v_نمبر = چوڑائی;
	اگر (dict_جوڑو_tv(d, "width", &tv) == FAIL)
	    توڑو;
	tv.vval.v_نمبر = اونچائی;
	اگر (dict_جوڑو_tv(d, "height", &tv) == FAIL)
	    توڑو;
	tv.vval.v_نمبر = topline;
	اگر (dict_جوڑو_tv(d, "topline", &tv) == FAIL)
	    توڑو;
# ifdef FEAT_DIFF
	tv.vval.v_نمبر = topfill;
# ورنہ
	tv.vval.v_نمبر = 0;
# endif
	اگر (dict_جوڑو_tv(d, "topfill", &tv) == FAIL)
	    توڑو;
	tv.vval.v_نمبر = leftcol;
	اگر (dict_جوڑو_tv(d, "leftcol", &tv) == FAIL)
	    توڑو;
	tv.vval.v_نمبر = skipcol;
	اگر (dict_جوڑو_tv(d, "skipcol", &tv) == FAIL)
	    توڑو;
	واپس d;
    }
    dict_unref(d);
    واپس تہی;
}
#اگر_ختم

// Return values of check_window_scroll_resize():
#تعریف_کرو CWSR_SCROLLED	1  // at least one window scrolled
#تعریف_کرو CWSR_RESIZED	2  // at least one window size changed

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
    ساکن خالی
check_کھڑکی_scroll_resize(
	صحیح	*سائز_گنتی,
	win_T	**پہلا_scroll_win,
	win_T	**پہلا_سائز_win,
	list_T	*winlist غیر_مستعمل,
	dict_T	*v_event غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح listidx = 0;
    صحیح tot_چوڑائی = 0;
    صحیح tot_اونچائی = 0;
    صحیح tot_topline = 0;
# ifdef FEAT_DIFF
    صحیح tot_topfill = 0;
# endif
    صحیح tot_leftcol = 0;
    صحیح tot_skipcol = 0;
#اگر_ختم

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	صحیح ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	صحیح سائز_بدلا = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_آخری_چوڑائی != wp->w_چوڑائی
			    || wp->w_آخری_اونچائی != wp->w_اونچائی);
	اگر (سائز_بدلا)
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    اگر (winlist != تہی)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_تالا = 0;
		tv.v_type = VAR_نمبر;
		tv.vval.v_نمبر = wp->w_id;
		list_سیٹ_شے(winlist, listidx++, &tv);
	    }
	    ورنہ
#اگر_ختم
		اگر (سائز_گنتی != تہی)
	    {
		++*سائز_گنتی;
		اگر (*پہلا_سائز_win == تہی)
		    *پہلا_سائز_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		اگر (*پہلا_scroll_win == تہی && !ignore_scroll)
		    *پہلا_scroll_win = wp;
	    }
	}

	صحیح scroll_بدلا = !ignore_scroll
				&& (wp->w_آخری_topline != wp->w_topline
#اگر_تعریف_شدہ FEAT_DIFF
				|| wp->w_آخری_topfill != wp->w_topfill
#اگر_ختم
				|| wp->w_آخری_leftcol != wp->w_leftcol
				|| wp->w_آخری_skipcol != wp->w_skipcol);
	اگر (scroll_بدلا
	    && پہلا_scroll_win != تہی && *پہلا_scroll_win == تہی)
	    *پہلا_scroll_win = wp;

#اگر_تعریف_شدہ FEAT_EVAL
	اگر ((سائز_بدلا || scroll_بدلا) && v_event != تہی)
	{
	    // Add info about this window to the v:event dictionary.
	    صحیح چوڑائی = wp->w_چوڑائی - wp->w_آخری_چوڑائی;
	    صحیح اونچائی = wp->w_اونچائی - wp->w_آخری_اونچائی;
	    صحیح topline = wp->w_topline - wp->w_آخری_topline;
# ifdef FEAT_DIFF
	    صحیح topfill = wp->w_topfill - wp->w_آخری_topfill;
# endif
	    صحیح leftcol = wp->w_leftcol - wp->w_آخری_leftcol;
	    صحیح skipcol = wp->w_skipcol - wp->w_آخری_skipcol;
	    dict_T *d = make_win_info_dict(چوڑائی, اونچائی, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    اگر (d == تہی)
		توڑو;
	    حرف winid[NUMBUFLEN];
	    vim_snprintf(winid, حجم(winid), "%d", wp->w_id);
	    اگر (dict_جوڑو_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		توڑو;
	    }
	    --d->dv_refcount;

	    tot_چوڑائی += مطلق_قدر(چوڑائی);
	    tot_اونچائی += مطلق_قدر(اونچائی);
	    tot_topline += مطلق_قدر(topline);
# ifdef FEAT_DIFF
	    tot_topfill += مطلق_قدر(topfill);
# endif
	    tot_leftcol += مطلق_قدر(leftcol);
	    tot_skipcol += مطلق_قدر(skipcol);
	}
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (v_event != تہی)
    {
	dict_T *alldict = make_win_info_dict(tot_چوڑائی, tot_اونچائی, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	اگر (alldict != تہی)
	{
	    اگر (dict_جوڑو_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    ورنہ
		--alldict->dv_refcount;
	}
    }
#اگر_ختم
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    خالی
may_trigger_win_scrolled_resized(خالی)
{
    ساکن صحیح	    recursive = FALSE;
    صحیح		    do_resize = has_winresized();
    صحیح		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    اگر (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_سائز_snapshot)
	واپس;

    صحیح سائز_گنتی = 0;
    win_T *پہلا_scroll_win = تہی, *پہلا_سائز_win = تہی;
    check_کھڑکی_scroll_resize(&سائز_گنتی, &پہلا_scroll_win, &پہلا_سائز_win,
								    تہی, تہی);
    صحیح trigger_resize = do_resize && سائز_گنتی > 0;
    صحیح trigger_scroll = do_scroll && پہلا_scroll_win != تہی;
    اگر (!trigger_resize && !trigger_scroll)
	واپس;  // no relevant changes
#اگر_تعریف_شدہ FEAT_EVAL
    list_T *windows_list = تہی;
    اگر (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_اشیاء(سائز_گنتی);
	اگر (windows_list != تہی)
	    check_کھڑکی_scroll_resize(تہی, تہی, تہی, windows_list, تہی);
    }

    dict_T *scroll_dict = تہی;
    اگر (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	اگر (scroll_dict != تہی)
	{
	    scroll_dict->dv_refcount = 1;
	    check_کھڑکی_scroll_resize(تہی, تہی, تہی, تہی, scroll_dict);
	}
    }
#اگر_ختم

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_سائز();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    کھڑکی_layout_تالا();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    اگر (trigger_resize
#اگر_تعریف_شدہ FEAT_EVAL
	    && windows_list != تہی
#اگر_ختم
	    )
    {
#اگر_تعریف_شدہ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	اگر (dict_جوڑو_list(v_event, "windows", windows_list) == OK)
	{
	    dict_سیٹ_اشیاء_ro(v_event);
#اگر_ختم
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((حرف *)winid, حجم(winid), "%d",
							 پہلا_سائز_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     پہلا_سائز_win->w_بفر);
#اگر_تعریف_شدہ FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#اگر_ختم
    }

    اگر (trigger_scroll
#اگر_تعریف_شدہ FEAT_EVAL
	    && scroll_dict != تہی
#اگر_ختم
	    )
    {
#اگر_تعریف_شدہ FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", تہی);
	dict_سیٹ_اشیاء_ro(v_event);
	dict_unref(scroll_dict);
#اگر_ختم
	char_u winid[NUMBUFLEN];
	vim_snprintf((حرف *)winid, حجم(winid), "%d",
						       پہلا_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   پہلا_scroll_win->w_بفر);
#اگر_تعریف_شدہ FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#اگر_ختم
    }

    recursive = FALSE;
    کھڑکی_layout_unlock();
}

/*
 * Close window "win" in tab page "tp", which is not the current tab page.
 * This may be the last window in that tab page and result in closing the tab,
 * thus "tp" may become invalid!
 * Caller must check if buffer is hidden and whether the tabline needs to be
 * updated.
 */
    خالی
win_close_othertab(win_T *win, صحیح free_buf, tabpage_T *tp)
{
    win_T	*wp;
    صحیح		dir;
    tabpage_T   *ptp = تہی;
    صحیح		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    اگر (کھڑکی_layout_locked(CMD_سائز))
	واپس;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    اگر (win_locked(win) || (win->w_بفر != تہی
					       && win->w_بفر->b_locked > 0))
	واپس; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    اگر (win->w_بفر != تہی)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	اگر (!win_valid_any_tab(win))
	    واپس;
    }

    اگر (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	اگر (!win_valid_any_tab(win))
	    واپس;
    }

    اگر (win->w_بفر != تہی)
	// Close the link to the buffer.
	close_بفر(win, win->w_بفر, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    برائے (ptp = پہلا_tabpage; ptp != تہی && ptp != tp; ptp = ptp->tp_اگلا)
	;
    اگر (ptp == تہی || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	اگر (win_valid_any_tab(win) && win->w_بفر == تہی)
	{
	    win->w_بفر = firstbuf;
	    ++firstbuf->b_nwindows;
	    اگر (win == curwin)
		curbuf = curwin->w_بفر;
	    win_init_خالی_خانے(win);
	}
	واپس;
    }

    // Autocommands may have closed the window already.
    برائے (wp = tp->tp_firstwin; wp != تہی && wp != win; wp = wp->w_اگلا)
	;
    اگر (wp == تہی)
	واپس;

    // When closing the last window in a tab page remove the tab page.
    اگر (tp->tp_firstwin == tp->tp_lastwin)
    {
	صحیح	h = tabline_اونچائی();

	اگر (tp == پہلا_tabpage)
	    پہلا_tabpage = tp->tp_اگلا;
	ورنہ
	{
	    برائے (ptp = پہلا_tabpage; ptp != تہی && ptp->tp_اگلا != tp;
							   ptp = ptp->tp_اگلا)
		;
	    اگر (ptp == تہی)
	    {
		اندرونی_error("win_close_othertab()");
		واپس;
	    }
	    ptp->tp_اگلا = tp->tp_اگلا;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	اگر (h != tabline_اونچائی())
	    shell_new_صفیں();
    }

    اگر (win->w_بفر != تہی)
	--win->w_بفر->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    اگر (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    ساکن win_T *
win_free_mem(
    win_T	*win,
    صحیح		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    چوکھٹا_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == تہی ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_چوکھٹا;
    wp = winframe_remove(win, dirp, tp, تہی);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    اگر (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    واپس wp;
}

#اگر defined(EXITFREE)
    خالی
win_free_all(خالی)
{
    صحیح		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = تہی;
    cmdwin_win = تہی;

    جب (پہلا_tabpage->tp_اگلا != تہی)
	tabpage_close(TRUE);

    برائے (صحیح i = 0; i < AUCMD_WIN_گنتی; ++i)
	اگر (aucmd_win[i].auc_win != تہی)
	{
	    (خالی)win_free_mem(aucmd_win[i].auc_win, &dummy, تہی);
	    aucmd_win[i].auc_win = تہی;
	}

    جب (firstwin != تہی)
	(خالی)win_free_mem(firstwin, &dummy, تہی);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = تہی;
}
#اگر_ختم

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    صحیح		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    چوکھٹا_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    چوکھٹا_T	*frp, *frp2, *frp3;
    چوکھٹا_T	*frp_close = win->w_چوکھٹا;
    win_T	*wp;
    صحیح		صف, ستون;

    /*
     * If there is only one window there is nothing to remove.
     */
    اگر (tp == تہی ? ONE_کھڑکی : tp->tp_firstwin == tp->tp_lastwin)
	واپس تہی;

    چوکھٹا_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_والد);
    صف = wp->w_winrow;
    ستون = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    اگر (win->w_vsep_چوڑائی == 0 && frp_close->fr_والد->fr_layout == FR_صف &&
	    frp_close->fr_پچھلا != تہی)
	چوکھٹا_سیٹ_vsep(frp_close->fr_پچھلا, FALSE);

    // Remove this frame from the list of frames.
    چوکھٹا_remove(frp_close);

    اگر (frp_close->fr_والد->fr_layout == FR_ستون)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	اگر (frp2->fr_win != تہی && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_پچھلا;
	    frp3 = frp_close->fr_اگلا;
	    جب (frp != تہی || frp3 != تہی)
	    {
		اگر (frp != تہی)
		{
		    اگر (!چوکھٹا_fixed_اونچائی(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			توڑو;
		    }
		    frp = frp->fr_پچھلا;
		}
		اگر (frp3 != تہی)
		{
		    اگر (frp3->fr_win != تہی && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			توڑو;
		    }
		    frp3 = frp3->fr_اگلا;
		}
	    }
	}
	چوکھٹا_new_اونچائی(frp2, frp2->fr_اونچائی + frp_close->fr_اونچائی,
			    frp2 == frp_close->fr_اگلا, !چوکھٹا_fixed_اونچائی(frp2), FALSE);
	*dirp = 'v';
    }
    ورنہ
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	اگر (frp2->fr_win != تہی && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_پچھلا;
	    frp3 = frp_close->fr_اگلا;
	    جب (frp != تہی || frp3 != تہی)
	    {
		اگر (frp != تہی)
		{
		    اگر (!چوکھٹا_fixed_چوڑائی(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			توڑو;
		    }
		    frp = frp->fr_پچھلا;
		}
		اگر (frp3 != تہی)
		{
		    اگر (frp3->fr_win != تہی && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			توڑو;
		    }
		    frp3 = frp3->fr_اگلا;
		}
	    }
	}
	چوکھٹا_new_چوڑائی(frp2, frp2->fr_چوڑائی + frp_close->fr_چوڑائی,
			    frp2 == frp_close->fr_اگلا, !چوکھٹا_fixed_چوڑائی(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    اگر (frp2 != frp_close->fr_پچھلا)
	چوکھٹا_comp_pos(frp_close->fr_والد, &صف, &ستون);

    اگر (unflat_altfr == تہی)
	چوکھٹا_flatten(frp2);
    ورنہ
	*unflat_altfr = frp2;

    چوکھٹا_locked--;

    واپس wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    ساکن خالی
چوکھٹا_flatten(چوکھٹا_T *frp)
{
    چوکھٹا_T	*frp2, *frp3;

    اگر (frp->fr_اگلا != تہی || frp->fr_پچھلا != تہی)
	واپس;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_والد->fr_layout = frp->fr_layout;
    frp->fr_والد->fr_بچہ = frp->fr_بچہ;
    FOR_ALL_چوکھٹے(frp2, frp->fr_بچہ)
	frp2->fr_والد = frp->fr_والد;
    frp->fr_والد->fr_win = frp->fr_win;
    اگر (frp->fr_win != تہی)
	frp->fr_win->w_چوکھٹا = frp->fr_والد;
    frp2 = frp->fr_والد;
    اگر (topframe->fr_بچہ == frp)
	topframe->fr_بچہ = frp2;
    vim_free(frp);

    frp = frp2->fr_والد;
    اگر (frp != تہی && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	اگر (frp->fr_بچہ == frp2)
	    frp->fr_بچہ = frp2->fr_بچہ;
	frp2->fr_بچہ->fr_پچھلا = frp2->fr_پچھلا;
	اگر (frp2->fr_پچھلا != تہی)
	    frp2->fr_پچھلا->fr_اگلا = frp2->fr_بچہ;
	برائے (frp3 = frp2->fr_بچہ; ; frp3 = frp3->fr_اگلا)
	{
	    frp3->fr_والد = frp;
	    اگر (frp3->fr_اگلا == تہی)
	    {
		frp3->fr_اگلا = frp2->fr_اگلا;
		اگر (frp2->fr_اگلا != تہی)
		    frp2->fr_اگلا->fr_پچھلا = frp3;
		توڑو;
	    }
	}
	اگر (topframe->fr_بچہ == frp2)
	    topframe->fr_بچہ = frp;
	vim_free(frp2);
    }
}

/*
 * Undo changes from a prior call to winframe_remove, also restoring lost
 * vertical separators and statuslines, and changed window positions for
 * windows within "unflat_altfr".
 * Caller must ensure no other changes were made to the layout or window sizes!
 */
    ساکن خالی
winframe_restore(win_T *wp, صحیح dir, چوکھٹا_T *unflat_altfr)
{
    چوکھٹا_T	*frp = wp->w_چوکھٹا;

    // Put "wp"'s frame back where it was.
    اگر (frp->fr_پچھلا != تہی)
	چوکھٹا_append(frp->fr_پچھلا, frp);
    ورنہ
	چوکھٹا_داخل_کرو(frp->fr_اگلا, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    اگر (wp->w_vsep_چوڑائی == 0
	    && frp->fr_والد->fr_layout == FR_صف && frp->fr_پچھلا != تہی)
	چوکھٹا_سیٹ_vsep(frp->fr_پچھلا, TRUE);

    // Statuslines above may have been lost.  Restore them.
    اگر (wp->w_حالت_اونچائی == 0
	    && frp->fr_والد->fr_layout == FR_ستون && frp->fr_پچھلا != تہی)
	چوکھٹا_جوڑو_statusline(frp->fr_پچھلا);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    اگر (dir == 'v')
    {
	چوکھٹا_new_اونچائی(unflat_altfr, unflat_altfr->fr_اونچائی - frp->fr_اونچائی,
		unflat_altfr == frp->fr_اگلا, FALSE, FALSE);
    }
    ورنہ اگر (dir == 'h')
    {
	چوکھٹا_new_چوڑائی(unflat_altfr, unflat_altfr->fr_چوڑائی - frp->fr_چوڑائی,
		unflat_altfr == frp->fr_اگلا, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    اگر (unflat_altfr != frp->fr_پچھلا)
    {
	win_T	*topleft = frame2win(frp->fr_والد);
	صحیح	صف = topleft->w_winrow;
	صحیح	ستون = topleft->w_wincol;

	چوکھٹا_comp_pos(frp->fr_والد, &صف, &ستون);
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
    ساکن چوکھٹا_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    چوکھٹا_T	*frp;
    چوکھٹا_T	*other_fr, *ہدف_fr;

    اگر (tp == تہی ? ONE_کھڑکی : tp->tp_firstwin == tp->tp_lastwin)
	واپس alt_tabpage()->tp_curwin->w_چوکھٹا;

    frp = win->w_چوکھٹا;

    اگر (frp->fr_پچھلا == تہی)
	واپس frp->fr_اگلا;
    اگر (frp->fr_اگلا == تہی)
	واپس frp->fr_پچھلا;

    // By default the next window will get the space that was abandoned by this
    //کھڑکی
    ہدف_fr = frp->fr_اگلا;
    other_fr  = frp->fr_پچھلا;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    اگر (frp->fr_والد != تہی && frp->fr_والد->fr_layout == FR_ستون && p_sb)
    {
	ہدف_fr = frp->fr_پچھلا;
	other_fr  = frp->fr_اگلا;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    اگر (frp->fr_والد != تہی && frp->fr_والد->fr_layout == FR_صف && p_spr)
    {
	ہدف_fr = frp->fr_پچھلا;
	other_fr  = frp->fr_اگلا;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    اگر (frp->fr_والد != تہی && frp->fr_والد->fr_layout == FR_صف)
    {
	اگر (چوکھٹا_fixed_چوڑائی(ہدف_fr) && !چوکھٹا_fixed_چوڑائی(other_fr))
	    ہدف_fr = other_fr;
    }
    ورنہ
    {
	اگر (چوکھٹا_fixed_اونچائی(ہدف_fr) && !چوکھٹا_fixed_اونچائی(other_fr))
	    ہدف_fr = other_fr;
    }

    واپس ہدف_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    ساکن tabpage_T *
alt_tabpage(خالی)
{
    tabpage_T	*tp = تہی;
    صحیح		forward;

    // Use the last accessed tab page, if possible.
    اگر ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	واپس lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_اگلا != تہی &&
	    ((tcl_flags & TCL_بایاں) == 0 || curtab == پہلا_tabpage);

    اگر (forward)
	tp = curtab->tp_اگلا;
    ورنہ
	// Use the previous tab page.
	برائے (tp = پہلا_tabpage; tp->tp_اگلا != curtab; tp = tp->tp_اگلا)
	    ;

    واپس tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    ساکن win_T *
frame2win(چوکھٹا_T *frp)
{
    جب (frp->fr_win == تہی)
	frp = frp->fr_بچہ;
    واپس frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    ساکن صحیح
چوکھٹا_has_win(چوکھٹا_T *frp, win_T *wp)
{
    چوکھٹا_T	*p;

    اگر (frp->fr_layout == FR_برگ)
	واپس frp->fr_win == wp;

    FOR_ALL_چوکھٹے(p, frp->fr_بچہ)
	اگر (چوکھٹا_has_win(p, wp))
	    واپس TRUE;
    واپس FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
ساکن صحیح min_سیٹ_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    ساکن خالی
چوکھٹا_new_اونچائی(
    چوکھٹا_T	*topfrp,
    صحیح		اونچائی,
    صحیح		topfirst,	// resize topmost contained frame first
    صحیح		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    صحیح		سیٹ_ch)		// set 'cmdheight' to resize topframe
{
    چوکھٹا_T	*frp;
    صحیح		extra_سطریں;
    صحیح		h;

    اگر (topfrp->fr_والد == تہی && سیٹ_ch)
    {
	// topframe: update the command line height, with side effects.
	صحیح new_ch = MAX(min_سیٹ_ch, p_ch + topfrp->fr_اونچائی - اونچائی);
	صحیح save_ch = min_سیٹ_ch;
	اگر (new_ch != p_ch)
	    سیٹ_option_قدر((char_u *)"cmdheight", new_ch, تہی, 0);
	min_سیٹ_ch = save_ch;
	اونچائی = MIN(اونچائی, صفیں_AVAIL);
    }
    اگر (topfrp->fr_win != تہی)
    {
	// Simple case: just one window.
	win_new_اونچائی(topfrp->fr_win,
				    اونچائی - topfrp->fr_win->w_حالت_اونچائی
					      - WINBAR_اونچائی(topfrp->fr_win));
    }
    ورنہ اگر (topfrp->fr_layout == FR_صف)
    {
	کرو
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    {
		چوکھٹا_new_اونچائی(frp, اونچائی, topfirst, wfh, سیٹ_ch);
		اگر (frp->fr_اونچائی > اونچائی)
		{
		    // Could not fit the windows, make the whole row higher.
		    اونچائی = frp->fr_اونچائی;
		    توڑو;
		}
	    }
	}
	جب (frp != تہی);
    }
    ورنہ    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_بچہ;
	اگر (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    جب (چوکھٹا_fixed_اونچائی(frp))
	    {
		frp = frp->fr_اگلا;
		اگر (frp == تہی)
		    واپس;	    // no frame without 'wfh', give up
	    }
	اگر (!topfirst)
	{
	    // Find the bottom frame of this column
	    جب (frp->fr_اگلا != تہی)
		frp = frp->fr_اگلا;
	    اگر (wfh)
		// Advance back for frames with one window with 'wfh' set.
		جب (چوکھٹا_fixed_اونچائی(frp))
		    frp = frp->fr_پچھلا;
	}

	extra_سطریں = اونچائی - topfrp->fr_اونچائی;
	اگر (extra_سطریں < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    جب (frp != تہی)
	    {
		h = چوکھٹا_minheight(frp, تہی);
		اگر (frp->fr_اونچائی + extra_سطریں < h)
		{
		    extra_سطریں += frp->fr_اونچائی - h;
		    چوکھٹا_new_اونچائی(frp, h, topfirst, wfh, سیٹ_ch);
		}
		ورنہ
		{
		    چوکھٹا_new_اونچائی(frp, frp->fr_اونچائی + extra_سطریں,
							topfirst, wfh, سیٹ_ch);
		    توڑو;
		}
		اگر (topfirst)
		{
		    کرو
			frp = frp->fr_اگلا;
		    جب (wfh && frp != تہی && چوکھٹا_fixed_اونچائی(frp));
		}
		ورنہ
		{
		    کرو
			frp = frp->fr_پچھلا;
		    جب (wfh && frp != تہی && چوکھٹا_fixed_اونچائی(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		اگر (frp == تہی)
		    اونچائی -= extra_سطریں;
	    }
	}
	ورنہ اگر (extra_سطریں > 0)
	{
	    // increase height of bottom or top frame
	    چوکھٹا_new_اونچائی(frp, frp->fr_اونچائی + extra_سطریں, topfirst, wfh,
									سیٹ_ch);
	}
    }
    topfrp->fr_اونچائی = اونچائی;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    ساکن صحیح
چوکھٹا_fixed_اونچائی(چوکھٹا_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    اگر (frp->fr_win != تہی)
	واپس frp->fr_win->w_p_wfh;

    اگر (frp->fr_layout == FR_صف)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    اگر (چوکھٹا_fixed_اونچائی(frp))
		واپس TRUE;
	واپس FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	اگر (!چوکھٹا_fixed_اونچائی(frp))
	    واپس FALSE;
    واپس TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    ساکن صحیح
چوکھٹا_fixed_چوڑائی(چوکھٹا_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    اگر (frp->fr_win != تہی)
	واپس frp->fr_win->w_p_wfw;

    اگر (frp->fr_layout == FR_ستون)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    اگر (چوکھٹا_fixed_چوڑائی(frp))
		واپس TRUE;
	واپس FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	اگر (!چوکھٹا_fixed_چوڑائی(frp))
	    واپس FALSE;
    واپس TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    ساکن خالی
چوکھٹا_جوڑو_statusline(چوکھٹا_T *frp)
{
    win_T	*wp;

    اگر (frp->fr_layout == FR_برگ)
    {
	wp = frp->fr_win;
	wp->w_حالت_اونچائی = statusline_اونچائی(wp);
    }
    ورنہ اگر (frp->fr_layout == FR_صف)
    {
	// Handle all the frames in the row.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    چوکھٹا_جوڑو_statusline(frp);
    }
    ورنہ // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	برائے (frp = frp->fr_بچہ; frp->fr_اگلا != تہی; frp = frp->fr_اگلا)
	    ;
	چوکھٹا_جوڑو_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    ساکن خالی
چوکھٹا_new_چوڑائی(
    چوکھٹا_T	*topfrp,
    صحیح		چوڑائی,
    صحیح		leftfirst,	// resize leftmost contained frame first
    صحیح		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    چوکھٹا_T	*frp;
    صحیح		extra_ستون_تعداد;
    صحیح		w;
    win_T	*wp;

    اگر (topfrp->fr_layout == FR_برگ)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	برائے (frp = topfrp; frp->fr_والد != تہی; frp = frp->fr_والد)
	    اگر (frp->fr_والد->fr_layout == FR_صف && frp->fr_اگلا != تہی)
		توڑو;
	اگر (frp->fr_والد == تہی)
	    wp->w_vsep_چوڑائی = 0;
	win_new_چوڑائی(wp, چوڑائی - wp->w_vsep_چوڑائی);
    }
    ورنہ اگر (topfrp->fr_layout == FR_ستون)
    {
	کرو
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    {
		چوکھٹا_new_چوڑائی(frp, چوڑائی, leftfirst, wfw);
		اگر (frp->fr_چوڑائی > چوڑائی)
		{
		    // Could not fit the windows, make whole column wider.
		    چوڑائی = frp->fr_چوڑائی;
		    توڑو;
		}
	    }
	} جب (frp != تہی);
    }
    ورنہ    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_بچہ;
	اگر (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    جب (چوکھٹا_fixed_چوڑائی(frp))
	    {
		frp = frp->fr_اگلا;
		اگر (frp == تہی)
		    واپس;	    // no frame without 'wfw', give up
	    }
	اگر (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    جب (frp->fr_اگلا != تہی)
		frp = frp->fr_اگلا;
	    اگر (wfw)
		// Advance back for frames with one window with 'wfw' set.
		جب (چوکھٹا_fixed_چوڑائی(frp))
		    frp = frp->fr_پچھلا;
	}

	extra_ستون_تعداد = چوڑائی - topfrp->fr_چوڑائی;
	اگر (extra_ستون_تعداد < 0)
	{
	    // reduce frame width, rightmost frame first
	    جب (frp != تہی)
	    {
		w = چوکھٹا_minwidth(frp, تہی);
		اگر (frp->fr_چوڑائی + extra_ستون_تعداد < w)
		{
		    extra_ستون_تعداد += frp->fr_چوڑائی - w;
		    چوکھٹا_new_چوڑائی(frp, w, leftfirst, wfw);
		}
		ورنہ
		{
		    چوکھٹا_new_چوڑائی(frp, frp->fr_چوڑائی + extra_ستون_تعداد,
							      leftfirst, wfw);
		    توڑو;
		}
		اگر (leftfirst)
		{
		    کرو
			frp = frp->fr_اگلا;
		    جب (wfw && frp != تہی && چوکھٹا_fixed_چوڑائی(frp));
		}
		ورنہ
		{
		    کرو
			frp = frp->fr_پچھلا;
		    جب (wfw && frp != تہی && چوکھٹا_fixed_چوڑائی(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		اگر (frp == تہی)
		    چوڑائی -= extra_ستون_تعداد;
	    }
	}
	ورنہ اگر (extra_ستون_تعداد > 0)
	{
	    // increase width of rightmost frame
	    چوکھٹا_new_چوڑائی(frp, frp->fr_چوڑائی + extra_ستون_تعداد, leftfirst, wfw);
	}
    }
    topfrp->fr_چوڑائی = چوڑائی;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    ساکن خالی
چوکھٹا_سیٹ_vsep(چوکھٹا_T *frp, صحیح جوڑو)
{
    win_T	*wp;

    اگر (frp->fr_layout == FR_برگ)
    {
	wp = frp->fr_win;
	اگر (جوڑو && wp->w_vsep_چوڑائی == 0)
	{
	    اگر (wp->w_چوڑائی > 0)	// don't make it negative
		win_new_چوڑائی(wp, wp->w_چوڑائی - 1);
	    wp->w_vsep_چوڑائی = 1;
	}
	ورنہ اگر (!جوڑو && wp->w_vsep_چوڑائی == 1)
	{
	    win_new_چوڑائی(wp, wp->w_چوڑائی + 1);
	    wp->w_vsep_چوڑائی = 0;
	}
    }
    ورنہ اگر (frp->fr_layout == FR_ستون)
    {
	// Handle all the frames in the column.
	FOR_ALL_چوکھٹے(frp, frp->fr_بچہ)
	    چوکھٹا_سیٹ_vsep(frp, جوڑو);
    }
    ورنہ // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_بچہ;
	جب (frp->fr_اگلا != تہی)
	    frp = frp->fr_اگلا;
	چوکھٹا_سیٹ_vsep(frp, جوڑو);
    }
}

/*
 * Set frame width from the window it contains.
 */
    ساکن خالی
چوکھٹا_fix_چوڑائی(win_T *wp)
{
    wp->w_چوکھٹا->fr_چوڑائی = wp->w_چوڑائی + wp->w_vsep_چوڑائی;
}

/*
 * Set frame height from the window it contains.
 */
    ساکن خالی
چوکھٹا_fix_اونچائی(win_T *wp)
{
    wp->w_چوکھٹا->fr_اونچائی = VISIBLE_اونچائی(wp) + wp->w_حالت_اونچائی;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    ساکن صحیح
چوکھٹا_minheight(چوکھٹا_T *topfrp, win_T *اگلا_curwin)
{
    چوکھٹا_T	*frp;
    صحیح		m;
    صحیح		n;

    اگر (topfrp->fr_win != تہی)
    {
	اگر (topfrp->fr_win == اگلا_curwin)
	    m = p_wh + topfrp->fr_win->w_حالت_اونچائی;
	ورنہ
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_حالت_اونچائی;
	    اگر (topfrp->fr_win == curwin && اگلا_curwin == تہی)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		اگر (p_wmh == 0)
		    ++m;
		m += WINBAR_اونچائی(curwin);
	    }
	}
    }
    ورنہ اگر (topfrp->fr_layout == FR_صف)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    n = چوکھٹا_minheight(frp, اگلا_curwin);
	    اگر (n > m)
		m = n;
	}
    }
    ورنہ
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    m += چوکھٹا_minheight(frp, اگلا_curwin);
    }

    واپس m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
ساکن صحیح
چوکھٹا_wincount_اندر_اونچائی(چوکھٹا_T *topfrp)
{
    چوکھٹا_T	*frp;
    صحیح		c = 0;
    صحیح		n;

    اگر (topfrp->fr_win != تہی)
	c = 1;
    ورنہ اگر (topfrp->fr_layout == FR_صف)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    n = چوکھٹا_wincount_اندر_اونچائی(frp);
	    اگر (n > c)
		c = n;
	}
    }
    ورنہ
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    c += چوکھٹا_wincount_اندر_اونچائی(frp);
    }

    واپس c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    ساکن صحیح
چوکھٹا_minwidth(
    چوکھٹا_T	*topfrp,
    win_T	*اگلا_curwin)	// use p_wh and p_wiw for next_curwin
{
    چوکھٹا_T	*frp;
    صحیح		m, n;

    اگر (topfrp->fr_win != تہی)
    {
	اگر (topfrp->fr_win == اگلا_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_چوڑائی;
	ورنہ
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_چوڑائی;
	    // Current window is minimal one column wide
	    اگر (p_wmw == 0 && topfrp->fr_win == curwin && اگلا_curwin == تہی)
		++m;
	}
    }
    ورنہ اگر (topfrp->fr_layout == FR_ستون)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    n = چوکھٹا_minwidth(frp, اگلا_curwin);
	    اگر (n > m)
		m = n;
	}
    }
    ورنہ
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    m += چوکھٹا_minwidth(frp, اگلا_curwin);
    }

    واپس m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    خالی
close_others(
    صحیح		پیغام_2,
    صحیح		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    صحیح		r;

    اگر (one_کھڑکی())
    {
	اگر (پیغام_2 && !autocmd_busy)
	    پیغام(_(m_onlyone));
	واپس;
    }

    // Be very careful here: autocommands may change the window layout.
    برائے (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_اگلا;

	// autocommands messed this one up
	اگر (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_بفر;
	}

	اگر (wp == curwin)		// don't close current window
	    جاری;

	// autoccommands messed this one up
	اگر (!buf_valid(wp->w_بفر) && win_valid(wp))
	{
	    wp->w_بفر = تہی;
	    win_close(wp, 0);
	    جاری;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_بفر, forceit);
	اگر (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    جاری;
	}
	اگر (!r)
	{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر (پیغام_2 && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_بدلا(wp->w_بفر, FALSE);
		اگر (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    جاری;
		}
	    }
	    اگر (buf_Is_بدلا(wp->w_بفر))
#اگر_ختم
		جاری;
	}
	win_close(wp, !buf_hide(wp->w_بفر) && !buf_Is_بدلا(wp->w_بفر));
    }

    اگر (پیغام_2 && !ONE_کھڑکی)
	emsg(_(e_other_کھڑکی_شامل_ہے_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    خالی
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
ساکن صحیح command_چوکھٹا_اونچائی = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    خالی
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
win_alloc_پہلا(خالی)
{
    اگر (win_alloc_firstwin(تہی) == FAIL)
	واپس FAIL;

    پہلا_tabpage = alloc_tabpage();
    اگر (پہلا_tabpage == تہی)
	واپس FAIL;
    curtab = پہلا_tabpage;
    unuse_tabpage(پہلا_tabpage);

    واپس OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(خالی)
{
    win_T *wp;

    wp = win_alloc(تہی, TRUE);
    اگر (wp == تہی)
	واپس تہی;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_چوکھٹا(wp);
    واپس wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    خالی
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_بفر = buf;
    ++buf->b_nwindows;
    win_init_خالی_خانے(wp); // set cursor and topline to safe values

    // Make sure w_localdir is NULL to avoid a chdir() in win_enter_ext().
    VIM_CLEAR(wp->w_localdir);
}

/*
 * Allocate the first window or the first window in a new tab page.
 * When "oldwin" is NULL create an empty buffer for it.
 * When "oldwin" is not NULL copy info from it to the new window.
 * Return FAIL when something goes wrong (out of memory).
 */
    ساکن صحیح
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(تہی, FALSE);
    اگر (curwin == تہی)
	واپس FAIL;
    اگر (oldwin == تہی)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(تہی, تہی, 1L, BLN_LISTED);
	اگر (curwin == تہی || curbuf == تہی)
	    واپس FAIL;
	curwin->w_بفر = curbuf;
#اگر_تعریف_شدہ FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#اگر_ختم
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    ورنہ
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_چوکھٹا(curwin);
    اگر (curwin->w_چوکھٹا == تہی)
	واپس FAIL;
    topframe = curwin->w_چوکھٹا;
    topframe->fr_چوڑائی = COLUMNS_WITHOUT_TPL();
    topframe->fr_اونچائی = صفیں - p_ch;

    واپس OK;
}

/*
 * Create a frame for window "wp".
 */
    ساکن خالی
new_چوکھٹا(win_T *wp)
{
    چوکھٹا_T *frp = ALLOC_CLEAR_ONE(چوکھٹا_T);

    wp->w_چوکھٹا = frp;
    اگر (frp == تہی)
	واپس;
    frp->fr_layout = FR_برگ;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    خالی
win_init_سائز(خالی)
{
    firstwin->w_اونچائی = صفیں_AVAIL;
    firstwin->w_پچھلا_اونچائی = صفیں_AVAIL;
    topframe->fr_اونچائی = صفیں_AVAIL;
    firstwin->w_چوڑائی = topframe->fr_چوڑائی;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    ساکن tabpage_T *
alloc_tabpage(خالی)
{
    tabpage_T	*tp;
#اگر_تعریف_شدہ FEAT_GUI
    صحیح		i;
#اگر_ختم


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    اگر (tp == تہی)
	واپس تہی;

#اگر_تعریف_شدہ FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    اگر (tp->tp_vars == تہی)
    {
	vim_free(tp);
	واپس تہی;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    برائے (i = 0; i < 3; i++)
	tp->tp_پچھلا_which_scrollbars[i] = -1;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#اگر_ختم
    tp->tp_ch_مستعمل = p_ch;

    واپس tp;
}

    خالی
free_tabpage(tabpage_T *tp)
{
    صحیح idx;

#اگر_تعریف_شدہ FEAT_DIFF
    diff_clear(tp);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    جب (tp->tp_پہلا_popupwin != تہی)
	popup_close_tabpage(tp, tp->tp_پہلا_popupwin->w_id, TRUE);
#اگر_ختم
    برائے (idx = 0; idx < SNAP_گنتی; ++idx)
	clear_snapshot(tp, idx);
#اگر_تعریف_شدہ FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#اگر_ختم

    اگر (tp == lastused_tabpage)
	lastused_tabpage = تہی;
#اگر_تعریف_شدہ FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#اگر_ختم

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#اگر_تعریف_شدہ FEAT_PYTHON
    python_tabpage_free(tp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON3
    python3_tabpage_free(tp);
#اگر_ختم

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
    tabpage_T	*پچھلا_tp = curtab;
    tabpage_T	*newtp;
    صحیح		n;
#اگر defined(FEAT_TABPANEL)
    صحیح پچھلا_columns = COLUMNS_WITHOUT_TPL();
#اگر_ختم

    اگر (cmdwin_type != 0)
    {
	emsg(_(e_invalid_اندر_cmdline_کھڑکی));
	واپس FAIL;
    }
    اگر (کھڑکی_layout_locked(CMD_tabnew))
	واپس FAIL;

    newtp = alloc_tabpage();
    اگر (newtp == تہی)
	واپس FAIL;

    // Remember the current windows in this Tab page.
    اگر (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	واپس FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == تہی)
				    ? تہی : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    اگر (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	اگر (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_اگلا = پہلا_tabpage;
	    پہلا_tabpage = newtp;
	}
	ورنہ
	{
	    اگر (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		برائے (tp = پہلا_tabpage; tp->tp_اگلا != تہی
					       && n < after; tp = tp->tp_اگلا)
		    ++n;
	    }
	    newtp->tp_اگلا = tp->tp_اگلا;
	    tp->tp_اگلا = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_سائز();
	firstwin->w_winrow = tabline_اونچائی();
	firstwin->w_پچھلا_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	آخری_حالت(FALSE);

	lastused_tabpage = پچھلا_tp;

#اگر defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	entering_کھڑکی(curwin);
#اگر_ختم
#اگر defined(FEAT_TABPANEL)
	اگر (پچھلا_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    صحیح		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		اگر (tp2 != curtab)
		{
		    چوکھٹا_new_چوڑائی(tp2->tp_topframe, w, FALSE, TRUE);
		    اگر (!چوکھٹا_check_چوڑائی(tp2->tp_topframe, w))
			چوکھٹا_new_چوڑائی(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#اگر_ختم
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, تہی, تہی, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, تہی, تہی, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, تہی, تہی, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, تہی, تہی, FALSE, curbuf);
	واپس OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    واپس FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    ساکن صحیح
may_open_tabpage(خالی)
{
    صحیح		n = (cmdmod.cmod_tab == 0)
				       ? postponed_بانٹو_tab : cmdmod.cmod_tab;

    اگر (n == 0)
	واپس FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_بانٹو_tab = 0;
    واپس win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    صحیح
make_tabpages(صحیح maxcount)
{
    صحیح		گنتی = maxcount;
    صحیح		todo;

    // Limit to 'tabpagemax' tabs.
    اگر (گنتی > p_tpm)
	گنتی = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    بلاک_autocmds();

    برائے (todo = گنتی - 1; todo > 0; --todo)
	اگر (win_new_tabpage(0) == FAIL)
	    توڑو;

    unblock_autocmds();

    // return actual number of tab pages
    واپس (گنتی - todo);
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
	    واپس TRUE;
    واپس FALSE;
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
	    FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
	    {
		اگر (win_valid_any_tab(wp))
		    واپس TRUE;
	    }
	    واپس FALSE;
	}
    }
    // shouldn't happen
    واپس FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    خالی
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    اگر (tab == پہلا_tabpage)
    {
	پہلا_tabpage = tab->tp_اگلا;
	ptp = پہلا_tabpage;
    }
    ورنہ
    {
	برائے (ptp = پہلا_tabpage; ptp != تہی && ptp->tp_اگلا != tab;
							    ptp = ptp->tp_اگلا)
	    ;
	أكد(ptp != تہی);
	ptp->tp_اگلا = tab->tp_اگلا;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
ڈھونڈو_tabpage(صحیح n)
{
    tabpage_T	*tp;
    صحیح		i = 1;

    اگر (n == 0)
	واپس curtab;

    برائے (tp = پہلا_tabpage; tp != تہی && i != n; tp = tp->tp_اگلا)
	++i;
    واپس tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    صحیح
tabpage_اشاریہ(tabpage_T *ftp)
{
    صحیح		i = 1;
    tabpage_T	*tp;

    برائے (tp = پہلا_tabpage; tp != تہی && tp != ftp; tp = tp->tp_اگلا)
	++i;
    واپس i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    ساکن صحیح
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    صحیح		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    leaving_کھڑکی(curwin);
#اگر_ختم
    reset_VIsual_and_resel();	// stop Visual mode
    اگر (trigger_leave_autocmds)
    {
	اگر (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, تہی, تہی, FALSE, curbuf);
	    اگر (curtab != tp)
		واپس FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, تہی, تہی, FALSE, curbuf);
	اگر (curtab != tp)
	    واپس FAIL;
	apply_autocmds(EVENT_TABLEAVE, تہی, تہی, FALSE, curbuf);
	اگر (curtab != tp)
	    واپس FAIL;
    }

    reset_dragwin();
#اگر defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    اگر (gui.اندر_use)
	gui_remove_scrollbars();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#اگر_ختم
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_صفیں = صفیں_AVAIL;
    اگر (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_چوڑائی;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = تہی;
    lastwin = تہی;
    واپس OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    ساکن خالی
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    صحیح		trigger_enter_autocmds,
    صحیح		trigger_leave_autocmds)
{
    صحیح		old_off = tp->tp_firstwin->w_winrow;
    win_T	*اگلا_prevwin = tp->tp_prevwin;
    tabpage_T	*آخری_tab = curtab;

    use_tabpage(tp);

    اگر (p_ch != curtab->tp_ch_مستعمل)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	صحیح new_ch = curtab->tp_ch_مستعمل;
	curtab->tp_ch_مستعمل = p_ch;
	command_چوکھٹا_اونچائی = FALSE;
	سیٹ_option_قدر((char_u *)"cmdheight", new_ch, تہی, 0);
	command_چوکھٹا_اونچائی = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (خالی)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = اگلا_prevwin;

    آخری_حالت(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#اگر_تعریف_شدہ FEAT_DIFF
    diff_need_scrollbind = TRUE;
#اگر_ختم

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    اگر (curtab->tp_old_صفیں != صفیں_AVAIL || (old_off != firstwin->w_winrow
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#اگر_ختم
		))
	shell_new_صفیں();
    اگر (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	اگر (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_چوڑائی;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	ورنہ
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = آخری_tab;

#اگر defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#اگر_ختم

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    اگر (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, تہی, تہی, FALSE, curbuf);
	اگر (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    خالی
goto_tabpage(صحیح n)
{
    tabpage_T	*tp = تہی;  // shut up compiler
    tabpage_T	*ttp;
    صحیح		i;

    اگر (متن_locked())
    {
	// Not allowed when editing the command line.
	متن_locked_پیغام();
	واپس;
    }

    // If there is only one it can't work.
    اگر (پہلا_tabpage->tp_اگلا == تہی)
    {
	اگر (n > 1)
	    beep_flush();
	واپس;
    }

    اگر (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	اگر (curtab->tp_اگلا == تہی)
	    tp = پہلا_tabpage;
	ورنہ
	    tp = curtab->tp_اگلا;
    }
    ورنہ اگر (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	برائے (i = n; i < 0; ++i)
	{
	    برائے (tp = پہلا_tabpage; tp->tp_اگلا != ttp && tp->tp_اگلا != تہی;
		    tp = tp->tp_اگلا)
		;
	    ttp = tp;
	}
    }
    ورنہ اگر (n == 9999)
    {
	// Go to last tab page.
	برائے (tp = پہلا_tabpage; tp->tp_اگلا != تہی; tp = tp->tp_اگلا)
	    ;
    }
    ورنہ
    {
	// Go to tab page "n".
	tp = ڈھونڈو_tabpage(n);
	اگر (tp == تہی)
	{
	    beep_flush();
	    واپس;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    اگر (gui_use_tabline())
	gui_mch_سیٹ_curtab(tabpage_اشاریہ(curtab));
#اگر_ختم
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    خالی
goto_tabpage_tp(
    tabpage_T	*tp,
    صحیح		trigger_enter_autocmds,
    صحیح		trigger_leave_autocmds)
{
    اگر (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    سیٹ_keep_پیغام(تہی, 0);

    skip_win_fix_scroll = TRUE;
    اگر (tp != curtab && leave_tabpage(tp->tp_curwin->w_بفر,
					trigger_leave_autocmds) == OK)
    {
	اگر (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	ورنہ
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
goto_tabpage_lastused(خالی)
{
    اگر (!valid_tabpage(lastused_tabpage))
	واپس FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    واپس OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    خالی
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    اگر (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	اگر (gui_use_tabline())
	    gui_mch_سیٹ_curtab(tabpage_اشاریہ(curtab));
#اگر_ختم
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    خالی
tabpage_move(صحیح nr)
{
    صحیح		n = 1;
    tabpage_T	*tp, *tp_dst;

    اگر (پہلا_tabpage->tp_اگلا == تہی)
	واپس;

    اگر (tabpage_move_disallowed)
	واپس;

    برائے (tp = پہلا_tabpage; tp->tp_اگلا != تہی && n < nr; tp = tp->tp_اگلا)
	++n;

    اگر (tp == curtab || (nr > 0 && tp->tp_اگلا != تہی
						    && tp->tp_اگلا == curtab))
	واپس;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    اگر (curtab == پہلا_tabpage)
	پہلا_tabpage = curtab->tp_اگلا;
    ورنہ
    {
	FOR_ALL_TABPAGES(tp)
	    اگر (tp->tp_اگلا == curtab)
		توڑو;
	اگر (tp == تہی)	// "cannot happen"
	    واپس;
	tp->tp_اگلا = curtab->tp_اگلا;
    }

    // Re-insert it at the specified position.
    اگر (nr <= 0)
    {
	curtab->tp_اگلا = پہلا_tabpage;
	پہلا_tabpage = curtab;
    }
    ورنہ
    {
	curtab->tp_اگلا = tp_dst->tp_اگلا;
	tp_dst->tp_اگلا = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
}


/*
 * Go to another window.
 * When jumping to another buffer, stop Visual mode.  Do this before
 * changing windows so we can yank the selection into the '*' register.
 * (note: this may trigger ModeChanged autocommand!)
 * When jumping to another window on the same buffer, adjust its cursor
 * position to keep the same Visual area.
 */
    خالی
win_goto(win_T *wp)
{
#اگر_تعریف_شدہ FEAT_CONCEAL
    win_T	*owp = curwin;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (ERROR_IF_ANY_POPUP_کھڑکی)
	واپس;
    اگر (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_مقصد_enter_popup_کھڑکی));
	واپس;
    }
#اگر_ختم
    اگر (متن_or_buf_locked())
    {
	beep_flush();
	واپس;
    }

    اگر (wp->w_بفر != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    ورنہ اگر (VIsual_فعال)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    اگر (!win_valid(wp))
	واپس;

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم
    win_enter(wp, TRUE);

#اگر_تعریف_شدہ FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    اگر (win_valid(owp) && owp->w_p_cole > 0 && !پیغام_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    اگر (curwin->w_p_cole > 0 && !پیغام_scrolled)
	need_cursor_سطر_redraw = TRUE;
#اگر_ختم
}

#اگر defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_ڈھونڈو_nr(صحیح winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (--winnr == 0)
	    توڑو;
    واپس wp;
}
#اگر_ختم

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_ڈھونڈو_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp == win)
		واپس tp;
    واپس تہی;
}
#اگر_ختم

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, صحیح اوپر, طویل گنتی)
{
    چوکھٹا_T	*fr;
    چوکھٹا_T	*nfr;
    چوکھٹا_T	*foundfr;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (popup_is_popup(wp))
	// popups don't have neighbors.
	واپس تہی;
#اگر_ختم
    foundfr = wp->w_چوکھٹا;
    جب (گنتی--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	برائے (;;)
	{
	    اگر (fr == tp->tp_topframe)
		جاؤ انتہا;
	    اگر (اوپر)
		nfr = fr->fr_پچھلا;
	    ورنہ
		nfr = fr->fr_اگلا;
	    اگر (fr->fr_والد->fr_layout == FR_ستون && nfr != تہی)
		توڑو;
	    fr = fr->fr_والد;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	برائے (;;)
	{
	    اگر (nfr->fr_layout == FR_برگ)
	    {
		foundfr = nfr;
		توڑو;
	    }
	    fr = nfr->fr_بچہ;
	    اگر (nfr->fr_layout == FR_صف)
	    {
		// Find the frame at the cursor row.
		جب (fr->fr_اگلا != تہی
			&& frame2win(fr)->w_wincol + fr->fr_چوڑائی
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_اگلا;
	    }
	    اگر (nfr->fr_layout == FR_ستون && اوپر)
		جب (fr->fr_اگلا != تہی)
		    fr = fr->fr_اگلا;
	    nfr = fr;
	}
    }
انتہا:
    واپس foundfr != تہی ? foundfr->fr_win : تہی;
}

/*
 * Move to window above or below "count" times.
 */
    ساکن خالی
win_goto_ver(
    صحیح		اوپر,		// TRUE to go to win above
    طویل	گنتی)
{
    win_T	*win;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (ERROR_IF_رکن_POPUP_کھڑکی)
	واپس;
#اگر_ختم
    win = win_vert_neighbor(curtab, curwin, اوپر, گنتی);
    اگر (win != تہی)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, صحیح بایاں, طویل گنتی)
{
    چوکھٹا_T	*fr;
    چوکھٹا_T	*nfr;
    چوکھٹا_T	*foundfr;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (popup_is_popup(wp))
	// popups don't have neighbors.
	واپس تہی;
#اگر_ختم
    foundfr = wp->w_چوکھٹا;
    جب (گنتی--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	برائے (;;)
	{
	    اگر (fr == tp->tp_topframe)
		جاؤ انتہا;
	    اگر (بایاں)
		nfr = fr->fr_پچھلا;
	    ورنہ
		nfr = fr->fr_اگلا;
	    اگر (fr->fr_والد->fr_layout == FR_صف && nfr != تہی)
		توڑو;
	    fr = fr->fr_والد;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	برائے (;;)
	{
	    اگر (nfr->fr_layout == FR_برگ)
	    {
		foundfr = nfr;
		توڑو;
	    }
	    fr = nfr->fr_بچہ;
	    اگر (nfr->fr_layout == FR_ستون)
	    {
		// Find the frame at the cursor row.
		جب (fr->fr_اگلا != تہی
			&& frame2win(fr)->w_winrow + fr->fr_اونچائی
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_اگلا;
	    }
	    اگر (nfr->fr_layout == FR_صف && بایاں)
		جب (fr->fr_اگلا != تہی)
		    fr = fr->fr_اگلا;
	    nfr = fr;
	}
    }
انتہا:
    واپس foundfr != تہی ? foundfr->fr_win : تہی;
}

/*
 * Move to left or right window.
 */
    ساکن خالی
win_goto_hor(
    صحیح		بایاں,		// TRUE to go to left win
    طویل	گنتی)
{
    win_T	*win;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (ERROR_IF_رکن_POPUP_کھڑکی)
	واپس;
#اگر_ختم
    win = win_horz_neighbor(curtab, curwin, بایاں, گنتی);
    اگر (win != تہی)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    خالی
win_enter(win_T *wp, صحیح undo_sync)
{
    (خالی)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    خالی
win_fix_current_dir(خالی)
{
    اگر (curwin->w_localdir != تہی || curtab->tp_localdir != تہی)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	اگر (globaldir == تہی)
	{
	    char_u	cwd[MAXPATHL];

	    اگر (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	اگر (curwin->w_localdir != تہی)
	    dirname = curwin->w_localdir;
	ورنہ
	    dirname = curtab->tp_localdir;

	اگر (mch_chdir((حرف *)dirname) == 0)
	{
	    آخری_chdir_reason = تہی;
	    shorten_fnames(TRUE);
	}
    }
    ورنہ اگر (globaldir != تہی)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((حرف *)globaldir);
	VIM_CLEAR(globaldir);
	آخری_chdir_reason = تہی;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    ساکن صحیح
win_enter_ext(win_T *wp, صحیح flags)
{
    صحیح		other_بفر = FALSE;
    صحیح		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    صحیح		did_decrement = FALSE;

    اگر (wp == curwin && curwin_invalid == 0)	// nothing to do
	واپس FALSE;

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (curwin_invalid == 0)
	leaving_کھڑکی(curwin);
#اگر_ختم

    اگر (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	اگر (wp->w_بفر != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, تہی, تہی, FALSE, curbuf);
	    other_بفر = TRUE;
	    اگر (!win_valid(wp))
		واپس FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, تہی, تہی, FALSE, curbuf);
	اگر (!win_valid(wp))
	    واپس FALSE;
#اگر_تعریف_شدہ FEAT_EVAL
	// autocmds may abort script processing
	اگر (aborting())
	    واپس FALSE;
#اگر_ختم
    }

    // sync undo before leaving the current buffer
    اگر ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_بفر)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    اگر (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    اگر (wp->w_بفر != curbuf)
	buf_copy_options(wp->w_بفر, BCO_ENTER | BCO_NOHELP);
    اگر (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_حالت = true;
    }
    curwin = wp;
    curbuf = wp->w_بفر;
    check_cursor();
    اگر (!virtual_فعال())
	curwin->w_cursor.coladd = 0;
    اگر (*p_spk == 'c')		// assume cursor position needs updating
	بدلا_سطر_abv_curs();
    ورنہ
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (منوال_معمول|منوال_CMDLINE|منوال_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#اگر_تعریف_شدہ پیغام_قطار
    اگر (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#اگر_ختم

    win_fix_current_dir();

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    entering_کھڑکی(curwin);
#اگر_ختم
    // Careful: autocommands may close the window and make "wp" invalid
    اگر (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, تہی, تہی, FALSE, curbuf);
    اگر (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, تہی, تہی, FALSE, curbuf);
	اگر (other_بفر)
	    apply_autocmds(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_حالت = true;
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (bt_terminal(curwin->w_بفر))
	// terminal is likely in another mode
	redraw_منوال = TRUE;
#اگر_ختم
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    اگر (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    اگر (curwin->w_اونچائی < p_wh && !curwin->w_p_wfh
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#اگر_ختم
	    )
	win_setheight((صحیح)p_wh);
    ورنہ اگر (curwin->w_اونچائی == 0)
	win_setheight(1);

    // set window width to desired minimal value
    اگر (curwin->w_چوڑائی < p_wiw && !curwin->w_p_wfw)
	win_setwidth((صحیح)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    واپس did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = تہی;

    اگر (curwin->w_بفر == buf)
	wp = curwin;
    ورنہ
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_بفر == buf)
		توڑو;
    اگر (wp != تہی)
	win_enter(wp, FALSE);
    واپس wp;
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

    اگر (wp != تہی)
	واپس wp;

    FOR_ALL_TABPAGES(tp)
	اگر (tp != curtab)
	{
	    FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
		اگر (wp->w_بفر == buf)
		    توڑو;
	    اگر (wp != تہی)
	    {
		goto_tabpage_win(tp, wp);
		اگر (curwin != wp)
		    wp = تہی;	// something went wrong
		توڑو;
	    }
	}
    واپس wp;
}

ساکن صحیح آخری_win_id = سب_سے_نیچا_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    ساکن win_T *
win_alloc(win_T *after, صحیح hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    اگر (new_wp == تہی)
	واپس تہی;

    اگر (win_alloc_سطریں(new_wp) == FAIL)
    {
	vim_free(new_wp);
	واپس تہی;
    }

    new_wp->w_id = ++آخری_win_id;

#اگر_تعریف_شدہ FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    اگر (new_wp->w_vars == تہی)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	واپس تہی;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#اگر_ختم

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    بلاک_autocmds();

    /*
     * link the window in the window list
     */
    اگر (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_چوڑائی = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#اگر_تعریف_شدہ FEAT_DIFF
    new_wp->w_topfill = 0;
#اگر_ختم
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_پچھلا_fraction_صف = -1;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_بایاں],
		SBAR_بایاں, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_دایاں],
		SBAR_دایاں, new_wp);
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
    foldInitWin(new_wp);
#اگر_ختم
    unblock_autocmds();
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    new_wp->w_اگلا_مطابقت_id = 1000;  // up to 1000 can be picked by the user
#اگر_ختم
    واپس new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    ساکن خالی
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    صحیح		i;
    buf_T	*buf;
    wininfo_T	*wip;

#اگر_تعریف_شدہ FEAT_FOLDING
    clearFolding(wp);
#اگر_ختم

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    بلاک_autocmds();

#اگر_تعریف_شدہ FEAT_LUA
    lua_کھڑکی_free(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MZSCHEME
    mzscheme_کھڑکی_free(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PERL
    perl_win_free(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON
    python_کھڑکی_free(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON3
    python3_کھڑکی_free(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TCL
    tcl_کھڑکی_free(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RUBY
    ruby_کھڑکی_free(wp);
#اگر_ختم

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    اگر (wp->w_stl_click != تہی)
    {
	برائے (i = 0; i < wp->w_stl_click_گنتی; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#اگر_تعریف_شدہ FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#اگر_ختم

    {
	tabpage_T	*ttp;

	اگر (prevwin == wp)
	    prevwin = تہی;
	FOR_ALL_TABPAGES(ttp)
	    اگر (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = تہی;
    }
    win_free_lsize(wp);

    برائے (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_اندراج(&wp->w_tagstack[i]);
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
		    اگر (wip2 != wip && wip2->wi_win == تہی)
		    {
			اگر (wip2->wi_اگلا != تہی)
			    wip2->wi_اگلا->wi_پچھلا = wip2->wi_پچھلا;
			اگر (wip2->wi_پچھلا == تہی)
			    buf->b_wininfo = wip2->wi_اگلا;
			ورنہ
			    wip2->wi_پچھلا->wi_اگلا = wip2->wi_اگلا;
			free_wininfo(wip2);
			توڑو;
		    }

		wip->wi_win = تہی;
	    }

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    clear_matches(wp);
#اگر_ختم

    free_jumplist(wp);

#اگر_تعریف_شدہ FEAT_QUICKFIX
    qf_free_all(wp);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_بایاں]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_دایاں]);
    }
#اگر_ختم // FEAT_GUI

#اگر_تعریف_شدہ FEAT_MENU
    remove_winbar(wp);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    برائے (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_نقاب);
    vim_free(wp->w_popup_نقاب_cells);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYN_HL
    vim_free(wp->w_p_cc_ستون_تعداد);
#اگر_ختم

    اگر (win_valid_any_tab(wp))
	win_remove(wp, tp);
    اگر (autocmd_busy)
    {
	wp->w_اگلا = au_pending_free_win;
	au_pending_free_win = wp;
    }
    ورنہ
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
    واپس is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    خالی
win_free_popup(win_T *win)
{
    اگر (win->w_بفر != تہی)
    {
	اگر (bt_popup(win->w_بفر))
	    win_close_بفر(win, DOBUF_WIPE_REUSE, FALSE);
	ورنہ
	    close_بفر(win, win->w_بفر, 0, FALSE, FALSE, TRUE);

	اگر (win->w_بفر != تہی)
	    --win->w_بفر->b_nwindows;
    }
# اگر defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    اگر (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_چوکھٹا);
    win_free(win, تہی);
}
#اگر_ختم

/*
 * Append window "wp" in the window list after window "after".
 */
    ساکن خالی
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    اگر (after == تہی)	    // after NULL is in front of the first
	before = firstwin;
    ورنہ
	before = after->w_اگلا;

    wp->w_اگلا = before;
    wp->w_پچھلا = after;
    اگر (after == تہی)
	firstwin = wp;
    ورنہ
	after->w_اگلا = wp;
    اگر (before == تہی)
	lastwin = wp;
    ورنہ
	before->w_پچھلا = wp;
}

/*
 * Remove a window from the window list.
 */
    خالی
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    اگر (wp->w_پچھلا != تہی)
	wp->w_پچھلا->w_اگلا = wp->w_اگلا;
    ورنہ اگر (tp == تہی)
	firstwin = curtab->tp_firstwin = wp->w_اگلا;
    ورنہ
	tp->tp_firstwin = wp->w_اگلا;

    اگر (wp->w_اگلا != تہی)
	wp->w_اگلا->w_پچھلا = wp->w_پچھلا;
    ورنہ اگر (tp == تہی)
	lastwin = curtab->tp_lastwin = wp->w_پچھلا;
    ورنہ
	tp->tp_lastwin = wp->w_پچھلا;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    ساکن خالی
چوکھٹا_append(چوکھٹا_T *after, چوکھٹا_T *frp)
{
    frp->fr_اگلا = after->fr_اگلا;
    after->fr_اگلا = frp;
    اگر (frp->fr_اگلا != تہی)
	frp->fr_اگلا->fr_پچھلا = frp;
    frp->fr_پچھلا = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    ساکن خالی
چوکھٹا_داخل_کرو(چوکھٹا_T *before, چوکھٹا_T *frp)
{
    frp->fr_اگلا = before;
    frp->fr_پچھلا = before->fr_پچھلا;
    before->fr_پچھلا = frp;
    اگر (frp->fr_پچھلا != تہی)
	frp->fr_پچھلا->fr_اگلا = frp;
    ورنہ
	frp->fr_والد->fr_بچہ = frp;
}

/*
 * Remove a frame from a frame list.
 */
    ساکن خالی
چوکھٹا_remove(چوکھٹا_T *frp)
{
    اگر (frp->fr_پچھلا != تہی)
	frp->fr_پچھلا->fr_اگلا = frp->fr_اگلا;
    ورنہ
	frp->fr_والد->fr_بچہ = frp->fr_اگلا;
    اگر (frp->fr_اگلا != تہی)
	frp->fr_اگلا->fr_پچھلا = frp->fr_پچھلا;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    صحیح
win_alloc_سطریں(win_T *wp)
{
    wp->w_سطریں_valid = 0;
    wp->w_سطریں = ALLOC_CLEAR_MULT(wline_T, صفیں);
    اگر (wp->w_سطریں == تہی)
	واپس FAIL;
    واپس OK;
}

/*
 * free lsize arrays for a window
 */
    خالی
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    اگر (wp != تہی)
	VIM_CLEAR(wp->w_سطریں);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    خالی
shell_new_صفیں(خالی)
{
    صحیح		h = (صحیح)صفیں_AVAIL;

    اگر (firstwin == تہی)	// not initialized yet
	واپس;
    اگر (h < چوکھٹا_minheight(topframe, تہی))
	h = چوکھٹا_minheight(topframe, تہی);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    چوکھٹا_new_اونچائی(topframe, h, FALSE, TRUE, FALSE);
    اگر (!چوکھٹا_check_اونچائی(topframe, h))
	چوکھٹا_new_اونچائی(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_مستعمل = p_ch;

    اگر (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
#اگر 0
    // Disabled: don't want making the screen smaller make a window larger.
    اگر (p_ea)
	win_equal(curwin, FALSE, 'v');
#اگر_ختم
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    خالی
shell_new_columns(خالی)
{
    اگر (firstwin == تہی)	// not initialized yet
	واپس;

#اگر defined(FEAT_TABPANEL)
    ساکن صحیح پچھلا_tp_چوڑائی;
    ساکن صحیح پچھلا_wincol;
    ساکن صحیح پچھلا_fr_چوڑائی;
#اگر_ختم
    صحیح w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    چوکھٹا_new_چوڑائی(topframe, w, FALSE, TRUE);
    اگر (!چوکھٹا_check_چوڑائی(topframe, w))
	چوکھٹا_new_چوڑائی(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#اگر defined(FEAT_TABPANEL)
    صحیح tp_چوڑائی = tabpanel_چوڑائی();
    صحیح tp_چوڑائی_بدلا = tp_چوڑائی != پچھلا_tp_چوڑائی;
    // tabpanel width changed
    اگر (tp_چوڑائی_بدلا && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    اگر (tp_چوڑائی_بدلا
	    || (tp_چوڑائی > 0 && (firstwin->w_wincol != پچھلا_wincol
		    || topframe->fr_چوڑائی != پچھلا_fr_چوڑائی)))
    {
	screen_fill(cmdline_صف, (صحیح)صفیں, 0, (صحیح)Columns, ' ', ' ', 0);
    }
    پچھلا_tp_چوڑائی = tabpanel_چوڑائی();
    پچھلا_wincol = firstwin->w_wincol;
    پچھلا_fr_چوڑائی = topframe->fr_چوڑائی;
#اگر_ختم
    // Adjust offset for command line start column
    cmdline_ستون_off = firstwin->w_wincol;
    cmdline_چوڑائی = topframe->fr_چوڑائی;
    comp_ستون();
    اگر (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#اگر defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#اگر_ختم
#اگر 0
    // Disabled: don't want making the screen smaller make a window larger.
    اگر (p_ea)
	win_equal(curwin, FALSE, 'h');
#اگر_ختم
}

/*
 * Save the size of all windows in "gap".
 */
    خالی
win_سائز_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, حجم(صحیح), 1);
    اگر (ga_grow(gap, win_گنتی() * 2 + 1) == FAIL)
	واپس;

    // first entry is the total lines available for windows
    ((صحیح *)gap->ga_ڈیٹا)[gap->ga_len++] = صفیں_AVAIL - آخری_stl_اونچائی(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((صحیح *)gap->ga_ڈیٹا)[gap->ga_len++] =
	    wp->w_چوڑائی + wp->w_vsep_چوڑائی;
	((صحیح *)gap->ga_ڈیٹا)[gap->ga_len++] = wp->w_اونچائی;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    خالی
win_سائز_restore(garray_T *gap)
{
    win_T	*wp;
    صحیح		i, j;

    اگر (win_گنتی() * 2 + 1 == gap->ga_len
	    && ((صحیح *)gap->ga_ڈیٹا)[0] == صفیں_AVAIL - آخری_stl_اونچائی(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	برائے (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		چوکھٹا_setwidth(wp->w_چوکھٹا, ((صحیح *)gap->ga_ڈیٹا)[i++]);
		win_setheight_win(((صحیح *)gap->ga_ڈیٹا)[i++], wp);
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
    خالی
win_comp_pos(خالی)
{
    صحیح		صف = tabline_اونچائی();
    صحیح		ستون = TPL_LCOL();

    چوکھٹا_comp_pos(topframe, &صف, &ستون);
#اگر defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اونچائی();
#اگر_ختم
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    ساکن خالی
چوکھٹا_comp_pos(چوکھٹا_T *topfrp, صحیح *صف, صحیح *ستون)
{
    win_T	*wp;
    چوکھٹا_T	*frp;
    صحیح		startcol;
    صحیح		startrow;
    صحیح		h;

    wp = topfrp->fr_win;
    اگر (wp != تہی)
    {
	اگر (wp->w_winrow != *صف || wp->w_wincol != *ستون)
	{
	    // position changed, redraw
	    wp->w_winrow = *صف;
	    wp->w_wincol = *ستون;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_حالت = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_اونچائی(wp) + wp->w_حالت_اونچائی;
	*صف += h > topfrp->fr_اونچائی ? topfrp->fr_اونچائی : h;
	*ستون += wp->w_چوڑائی + wp->w_vsep_چوڑائی;
    }
    ورنہ
    {
	startrow = *صف;
	startcol = *ستون;
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	{
	    اگر (topfrp->fr_layout == FR_صف)
		*صف = startrow;	// all frames are at the same row
	    ورنہ
		*ستون = startcol;	// all frames are at the same col
	    چوکھٹا_comp_pos(frp, صف, ستون);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    خالی
win_ensure_سائز(خالی)
{
    اگر (curwin->w_اونچائی == 0)
	win_setheight(1);
    اگر (curwin->w_چوڑائی == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    خالی
win_setheight(صحیح اونچائی)
{
    win_setheight_win(اونچائی, curwin);
#اگر defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اونچائی();
#اگر_ختم
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    خالی
win_setheight_win(صحیح اونچائی, win_T *win)
{
    اگر (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	اگر (اونچائی < p_wmh)
	    اونچائی = p_wmh;
	اگر (اونچائی == 0)
	    اونچائی = 1;
	اونچائی += WINBAR_اونچائی(curwin);
    }

    چوکھٹا_setheight(win->w_چوکھٹا, اونچائی + win->w_حالت_اونچائی);

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
    ساکن خالی
چوکھٹا_setheight(چوکھٹا_T *curfrp, صحیح اونچائی)
{
    صحیح		گنجائش;		// total number of lines available
    صحیح		take;		// number of lines taken from other windows
    صحیح		گنجائش_cmdline;	// lines available from cmdline
    صحیح		تسلسل;
    چوکھٹا_T	*frp;
    صحیح		h;
    صحیح		گنجائش_reserved;

    // If the height already is the desired value, nothing to do.
    اگر (curfrp->fr_اونچائی == اونچائی)
	واپس;

    اگر (curfrp->fr_والد == تہی)
    {
	// topframe: can only change the command line height
	اگر (اونچائی > 0)
	    چوکھٹا_new_اونچائی(curfrp, اونچائی, FALSE, FALSE, TRUE);
    }
    ورنہ اگر (curfrp->fr_والد->fr_layout == FR_صف)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = چوکھٹا_minheight(curfrp->fr_والد, تہی);
	اگر (اونچائی < h)
	    اونچائی = h;
	چوکھٹا_setheight(curfrp->fr_والد, اونچائی);
    }
    ورنہ
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
	برائے (تسلسل = 1; تسلسل <= 2; ++تسلسل)
	{
	    گنجائش = 0;
	    گنجائش_reserved = 0;
	    FOR_ALL_چوکھٹے(frp, curfrp->fr_والد->fr_بچہ)
	    {
		اگر (frp != curfrp
			&& frp->fr_win != تہی
			&& frp->fr_win->w_p_wfh)
		    گنجائش_reserved += frp->fr_اونچائی;
		گنجائش += frp->fr_اونچائی;
		اگر (frp != curfrp)
		    گنجائش -= چوکھٹا_minheight(frp, تہی);
	    }
	    اگر (curfrp->fr_چوڑائی != topframe->fr_چوڑائی)
		گنجائش_cmdline = 0;
	    ورنہ
	    {
		گنجائش_cmdline = صفیں - p_ch - (lastwin->w_winrow
						+ VISIBLE_اونچائی(lastwin)
						+ lastwin->w_حالت_اونچائی);
		اگر (گنجائش_cmdline < 0)
		    گنجائش_cmdline = 0;
	    }

	    اگر (اونچائی <= گنجائش + گنجائش_cmdline)
		توڑو;
	    اگر (تسلسل == 2 || curfrp->fr_چوڑائی == topframe->fr_چوڑائی)
	    {
		اونچائی = گنجائش + گنجائش_cmdline;
		توڑو;
	    }
	    چوکھٹا_setheight(curfrp->fr_والد, اونچائی
		+ چوکھٹا_minheight(curfrp->fr_والد, NOWIN) - (صحیح)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = اونچائی - curfrp->fr_اونچائی;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	اگر (اونچائی > گنجائش + گنجائش_cmdline - گنجائش_reserved)
	    گنجائش_reserved = گنجائش + گنجائش_cmdline - اونچائی;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	اگر (take < 0 && گنجائش - curfrp->fr_اونچائی <= گنجائش_reserved)
	    گنجائش_reserved = 0;

	اگر (take > 0 && گنجائش_cmdline > 0)
	{
	    // use lines from cmdline first
	    اگر (take < گنجائش_cmdline)
		گنجائش_cmdline = take;
	    take -= گنجائش_cmdline;
	    topframe->fr_اونچائی += گنجائش_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	چوکھٹا_new_اونچائی(curfrp, اونچائی, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	برائے (تسلسل = 0; تسلسل < 2; ++تسلسل)
	{
	    اگر (تسلسل == 0)
		frp = curfrp->fr_اگلا;	// 1st run: start with next window
	    ورنہ
		frp = curfrp->fr_پچھلا;	// 2nd run: start with prev window
	    جب (frp != تہی && take != 0)
	    {
		h = چوکھٹا_minheight(frp, تہی);
		اگر (گنجائش_reserved > 0
			&& frp->fr_win != تہی
			&& frp->fr_win->w_p_wfh)
		{
		    اگر (گنجائش_reserved >= frp->fr_اونچائی)
			گنجائش_reserved -= frp->fr_اونچائی;
		    ورنہ
		    {
			اگر (frp->fr_اونچائی - گنجائش_reserved > take)
			    گنجائش_reserved = frp->fr_اونچائی - take;
			take -= frp->fr_اونچائی - گنجائش_reserved;
			چوکھٹا_new_اونچائی(frp, گنجائش_reserved, FALSE, FALSE,
									TRUE);
			گنجائش_reserved = 0;
		    }
		}
		ورنہ
		{
		    اگر (frp->fr_اونچائی - take < h)
		    {
			take -= frp->fr_اونچائی - h;
			چوکھٹا_new_اونچائی(frp, h, FALSE, FALSE, TRUE);
		    }
		    ورنہ
		    {
			چوکھٹا_new_اونچائی(frp, frp->fr_اونچائی - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		اگر (تسلسل == 0)
		    frp = frp->fr_اگلا;
		ورنہ
		    frp = frp->fr_پچھلا;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    خالی
win_setwidth(صحیح چوڑائی)
{
    win_setwidth_win(چوڑائی, curwin);
}

    خالی
win_setwidth_win(صحیح چوڑائی, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    اگر (wp == curwin)
    {
	اگر (چوڑائی < p_wmw)
	    چوڑائی = p_wmw;
	اگر (چوڑائی == 0)
	    چوڑائی = 1;
    }
    ورنہ اگر (چوڑائی < 0)
	چوڑائی = 0;

    چوکھٹا_setwidth(wp->w_چوکھٹا, چوڑائی + wp->w_vsep_چوڑائی);

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
    ساکن خالی
چوکھٹا_setwidth(چوکھٹا_T *curfrp, صحیح چوڑائی)
{
    صحیح		گنجائش;		// total number of lines available
    صحیح		take;		// number of lines taken from other windows
    صحیح		تسلسل;
    چوکھٹا_T	*frp;
    صحیح		w;
    صحیح		گنجائش_reserved;

    // If the width already is the desired value, nothing to do.
    اگر (curfrp->fr_چوڑائی == چوڑائی)
	واپس;

    اگر (curfrp->fr_والد == تہی)
	// topframe: can't change width
	واپس;

    اگر (curfrp->fr_والد->fr_layout == FR_ستون)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = چوکھٹا_minwidth(curfrp->fr_والد, تہی);
	اگر (چوڑائی < w)
	    چوڑائی = w;
	چوکھٹا_setwidth(curfrp->fr_والد, چوڑائی);
    }
    ورنہ
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	برائے (تسلسل = 1; تسلسل <= 2; ++تسلسل)
	{
	    گنجائش = 0;
	    گنجائش_reserved = 0;
	    FOR_ALL_چوکھٹے(frp, curfrp->fr_والد->fr_بچہ)
	    {
		اگر (frp != curfrp
			&& frp->fr_win != تہی
			&& frp->fr_win->w_p_wfw)
		    گنجائش_reserved += frp->fr_چوڑائی;
		گنجائش += frp->fr_چوڑائی;
		اگر (frp != curfrp)
		    گنجائش -= چوکھٹا_minwidth(frp, تہی);
	    }

	    اگر (چوڑائی <= گنجائش)
		توڑو;
	    اگر (تسلسل == 2 || curfrp->fr_اونچائی >= صفیں_AVAIL)
	    {
		چوڑائی = گنجائش;
		توڑو;
	    }
	    چوکھٹا_setwidth(curfrp->fr_والد, چوڑائی
		 + چوکھٹا_minwidth(curfrp->fr_والد, NOWIN) - (صحیح)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = چوڑائی - curfrp->fr_چوڑائی;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	اگر (چوڑائی > گنجائش - گنجائش_reserved)
	    گنجائش_reserved = گنجائش - چوڑائی;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	اگر (take < 0 && گنجائش - curfrp->fr_چوڑائی < گنجائش_reserved)
	    گنجائش_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	چوکھٹا_new_چوڑائی(curfrp, چوڑائی, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	برائے (تسلسل = 0; تسلسل < 2; ++تسلسل)
	{
	    اگر (تسلسل == 0)
		frp = curfrp->fr_اگلا;	// 1st run: start with next window
	    ورنہ
		frp = curfrp->fr_پچھلا;	// 2nd run: start with prev window
	    جب (frp != تہی && take != 0)
	    {
		w = چوکھٹا_minwidth(frp, تہی);
		اگر (گنجائش_reserved > 0
			&& frp->fr_win != تہی
			&& frp->fr_win->w_p_wfw)
		{
		    اگر (گنجائش_reserved >= frp->fr_چوڑائی)
			گنجائش_reserved -= frp->fr_چوڑائی;
		    ورنہ
		    {
			اگر (frp->fr_چوڑائی - گنجائش_reserved > take)
			    گنجائش_reserved = frp->fr_چوڑائی - take;
			take -= frp->fr_چوڑائی - گنجائش_reserved;
			چوکھٹا_new_چوڑائی(frp, گنجائش_reserved, FALSE, FALSE);
			گنجائش_reserved = 0;
		    }
		}
		ورنہ
		{
		    اگر (frp->fr_چوڑائی - take < w)
		    {
			take -= frp->fr_چوڑائی - w;
			چوکھٹا_new_چوڑائی(frp, w, FALSE, FALSE);
		    }
		    ورنہ
		    {
			چوکھٹا_new_چوڑائی(frp, frp->fr_چوڑائی - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		اگر (تسلسل == 0)
		    frp = frp->fr_اگلا;
		ورنہ
		    frp = frp->fr_پچھلا;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    خالی
win_setminheight(خالی)
{
    صحیح		گنجائش;
    صحیح		needed;
    صحیح		پہلا = TRUE;

    // loop until there is a 'winminheight' that is possible
    جب (p_wmh > 0)
    {
	گنجائش = صفیں - p_ch;
	needed = min_صفیں_for_all_tabpages() - 1;  // 1 was added for the cmdline
	اگر (گنجائش >= needed)
	    توڑو;
	--p_wmh;
	اگر (پہلا)
	{
	    emsg(_(e_not_enough_گنجائش));
	    پہلا = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    خالی
win_setminwidth(خالی)
{
    صحیح		گنجائش;
    صحیح		needed;
    صحیح		پہلا = TRUE;

    // loop until there is a 'winminheight' that is possible
    جب (p_wmw > 0)
    {
	گنجائش = topframe->fr_چوڑائی;
	needed = چوکھٹا_minwidth(topframe, تہی);
	اگر (گنجائش >= needed)
	    توڑو;
	--p_wmw;
	اگر (پہلا)
	{
	    emsg(_(e_not_enough_گنجائش));
	    پہلا = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    خالی
win_drag_حالت_سطر(win_T *dragwin, صحیح انحراف)
{
    چوکھٹا_T	*curfr;
    چوکھٹا_T	*fr;
    صحیح		گنجائش;
    صحیح		اوپر;	// if TRUE, drag status line up, otherwise down
    صحیح		n;

    fr = dragwin->w_چوکھٹا;
    curfr = fr;
    اگر (fr != topframe)		// more than one window
    {
	fr = fr->fr_والد;
	// When the parent frame is not a column of frames, its parent should
	// be.
	اگر (fr->fr_layout != FR_ستون)
	{
	    curfr = fr;
	    اگر (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_والد;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    جب (curfr != topframe && curfr->fr_اگلا == تہی)
    {
	اگر (fr != topframe)
	    fr = fr->fr_والد;
	curfr = fr;
	اگر (fr != topframe)
	    fr = fr->fr_والد;
    }

    اگر (انحراف < 0) // drag up
    {
	اوپر = TRUE;
	انحراف = -انحراف;
	// sum up the room of the current frame and above it
	اگر (fr == curfr)
	{
	    // only one window
	    گنجائش = fr->fr_اونچائی - چوکھٹا_minheight(fr, تہی);
	}
	ورنہ
	{
	    گنجائش = 0;
	    برائے (fr = fr->fr_بچہ; ; fr = fr->fr_اگلا)
	    {
		گنجائش += fr->fr_اونچائی - چوکھٹا_minheight(fr, تہی);
		اگر (fr == curfr)
		    توڑو;
	    }
	}
	fr = curfr->fr_اگلا;		// put fr at frame that grows
    }
    ورنہ    // drag down
    {
	اوپر = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	گنجائش = صفیں - cmdline_صف;
	اگر (curfr->fr_اگلا == تہی)
	    --گنجائش;
	ورنہ
	    گنجائش -= p_ch;
	اگر (گنجائش < 0)
	    گنجائش = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_چوکھٹے(fr, curfr->fr_اگلا)
	    گنجائش += fr->fr_اونچائی - چوکھٹا_minheight(fr, تہی);
	fr = curfr;			// put fr at window that grows
    }

    اگر (گنجائش < انحراف)		// Not enough room
	انحراف = گنجائش;		// Move as far as we can
    اگر (انحراف <= 0)
	واپس;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    اگر (fr != تہی)
	چوکھٹا_new_اونچائی(fr, fr->fr_اونچائی + انحراف, اوپر, FALSE, TRUE);

    اگر (اوپر)
	fr = curfr;		// current frame gets smaller
    ورنہ
	fr = curfr->fr_اگلا;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    جب (fr != تہی && انحراف > 0)
    {
	n = چوکھٹا_minheight(fr, تہی);
	اگر (fr->fr_اونچائی - انحراف <= n)
	{
	    انحراف -= fr->fr_اونچائی - n;
	    چوکھٹا_new_اونچائی(fr, n, !اوپر, FALSE, TRUE);
	}
	ورنہ
	{
	    چوکھٹا_new_اونچائی(fr, fr->fr_اونچائی - انحراف, !اوپر, FALSE, TRUE);
	    توڑو;
	}
	اگر (اوپر)
	    fr = fr->fr_پچھلا;
	ورنہ
	    fr = fr->fr_اگلا;
    }
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_SOME_VALID);
    showmode();
}

/*
 * Separator line of dragwin is dragged "offset" lines right (negative is left).
 */
    خالی
win_drag_vsep_سطر(win_T *dragwin, صحیح انحراف)
{
    چوکھٹا_T	*curfr;
    چوکھٹا_T	*fr;
    صحیح		گنجائش;
    صحیح		بایاں;	// if TRUE, drag separator line left, otherwise right
    صحیح		n;

    fr = dragwin->w_چوکھٹا;
    اگر (fr == topframe)		// only one window (cannot happen?)
	واپس;
    curfr = fr;
    fr = fr->fr_والد;
    // When the parent frame is not a row of frames, its parent should be.
    اگر (fr->fr_layout != FR_صف)
    {
	اگر (fr == topframe)	// only a column of windows (cannot happen?)
	    واپس;
	curfr = fr;
	fr = fr->fr_والد;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    جب (curfr->fr_اگلا == تہی)
    {
	اگر (fr == topframe)
	    توڑو;
	curfr = fr;
	fr = fr->fr_والد;
	اگر (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_والد;
	}
    }

    اگر (انحراف < 0) // drag left
    {
	بایاں = TRUE;
	انحراف = -انحراف;
	// sum up the room of the current frame and left of it
	گنجائش = 0;
	برائے (fr = fr->fr_بچہ; ; fr = fr->fr_اگلا)
	{
	    گنجائش += fr->fr_چوڑائی - چوکھٹا_minwidth(fr, تہی);
	    اگر (fr == curfr)
		توڑو;
	}
	fr = curfr->fr_اگلا;		// put fr at frame that grows
    }
    ورنہ    // drag right
    {
	بایاں = FALSE;
	// sum up the room of frames right of the current one
	گنجائش = 0;
	FOR_ALL_چوکھٹے(fr, curfr->fr_اگلا)
	    گنجائش += fr->fr_چوڑائی - چوکھٹا_minwidth(fr, تہی);
	fr = curfr;			// put fr at window that grows
    }

    اگر (گنجائش < انحراف)		// Not enough room
	انحراف = گنجائش;		// Move as far as we can
    اگر (انحراف <= 0)		// No room at all, quit.
	واپس;
    اگر (fr == تہی)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	واپس;

    // grow frame fr by offset lines
    چوکھٹا_new_چوڑائی(fr, fr->fr_چوڑائی + انحراف, بایاں, FALSE);

    // shrink other frames: current and at the left or at the right
    اگر (بایاں)
	fr = curfr;		// current frame gets smaller
    ورنہ
	fr = curfr->fr_اگلا;	// next frame gets smaller

    جب (fr != تہی && انحراف > 0)
    {
	n = چوکھٹا_minwidth(fr, تہی);
	اگر (fr->fr_چوڑائی - انحراف <= n)
	{
	    انحراف -= fr->fr_چوڑائی - n;
	    چوکھٹا_new_چوڑائی(fr, n, !بایاں, FALSE);
	}
	ورنہ
	{
	    چوکھٹا_new_چوڑائی(fr, fr->fr_چوڑائی - انحراف, !بایاں, FALSE);
	    توڑو;
	}
	اگر (بایاں)
	    fr = fr->fr_پچھلا;
	ورنہ
	    fr = fr->fr_اگلا;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#تعریف_کرو FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    خالی
سیٹ_fraction(win_T *wp)
{
    اگر (wp->w_اونچائی > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((طویل)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (طویل)wp->w_اونچائی;
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
    ساکن خالی
win_fix_scroll(صحیح resize)
{
    اگر (*p_spk == 'c')
	واپس;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	اگر (wp->w_اونچائی != wp->w_پچھلا_اونچائی)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    اگر (*p_spk == 's' && wp->w_winrow != wp->w_پچھلا_winrow
		      && wp->w_botline - 1 <= wp->w_بفر->b_ml.ml_سطر_گنتی)
	    {
		صحیح diff = (wp->w_winrow - wp->w_پچھلا_winrow)
					  + (wp->w_اونچائی - wp->w_پچھلا_اونچائی);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		اگر (diff > 0)
		    cursor_نیچے_کو_inner(wp, diff);
		ورنہ
		    cursor_اوپر_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_مقصد_fraction(wp, wp->w_پچھلا_اونچائی);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		اگر (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    ورنہ اگر (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_پچھلا_اونچائی = wp->w_اونچائی;
	wp->w_پچھلا_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    اگر (!(get_real_state() & (منوال_معمول|منوال_CMDLINE|منوال_TERMINAL)))
	win_fix_cursor(FALSE);
    ورنہ اگر (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    ساکن خالی
win_fix_cursor(صحیح معمول_4)
{
    win_T	*wp = curwin;

    اگر (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_بفر->b_ml.ml_سطر_گنتی < wp->w_اونچائی)
	واپس;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    طویل so = MIN(wp->w_اونچائی / 2, get_scrolloff_قدر());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_نیچے_کو_inner(wp, so);
    linenr_T چوٹی = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_اوپر_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    اگر (lnum > bot && (wp->w_botline - wp->w_بفر->b_ml.ml_سطر_گنتی) != 1)
	nlnum = bot;
    ورنہ اگر (lnum < چوٹی && wp->w_topline != 1)
	nlnum = (so == wp->w_اونچائی / 2) ? bot : چوٹی;

    اگر (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	اگر (معمول_4)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	ورنہ  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_مقصد_fraction(wp, wp->w_پچھلا_اونچائی);
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
    خالی
win_new_اونچائی(win_T *wp, صحیح اونچائی)
{
    صحیح		پچھلا_اونچائی = wp->w_اونچائی;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    اگر (اونچائی < 0)
	اونچائی = 0;
    اگر (wp->w_اونچائی == اونچائی)
	واپس;	    // nothing to do

    اگر (wp->w_اونچائی > 0)
    {
	اگر (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	اگر (wp->w_اونچائی != پچھلا_اونچائی)
	    واپس;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	اگر (wp->w_wrow != wp->w_پچھلا_fraction_صف)
	    سیٹ_fraction(wp);
    }

    wp->w_اونچائی = اونچائی;
    wp->w_redr_حالت = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    اگر (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	اگر (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_مقصد_fraction(wp, پچھلا_اونچائی);
    }
}

    خالی
scroll_مقصد_fraction(win_T *wp, صحیح پچھلا_اونچائی)
{
    linenr_T	lnum;
    صحیح		sline, سطر_سائز;
    صحیح		اونچائی = wp->w_اونچائی;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    اگر (اونچائی > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (اونچائی < wp->w_بفر->b_ml.ml_سطر_گنتی || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	اگر (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((طویل)wp->w_fraction * (طویل)اونچائی - 1L)
							       / FRACTION_MULT;
	سطر_سائز = plines_win_ستون(wp, lnum, (طویل)(wp->w_cursor.ستون)) - 1;
	sline = wp->w_wrow - سطر_سائز;

	اگر (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    صحیح صفیں_2 = plines_win(wp, lnum, FALSE);

	    اگر (sline > wp->w_اونچائی - صفیں_2)
	    {
		sline = wp->w_اونچائی - صفیں_2;
		wp->w_wrow -= صفیں_2 - سطر_سائز;
	    }
	}

	اگر (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    صحیح		want_صف = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = سطر_سائز;
	    اگر (wp->w_wrow >= wp->w_اونچائی
				       && (wp->w_چوڑائی - win_ستون_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_چوڑائی - win_ستون_off(wp);

		--wp->w_wrow;
		جب (wp->w_wrow > want_صف)
		{
		    skipcol += wp->w_چوڑائی - win_ستون_off(wp) + win_ستون_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	ورنہ اگر (sline > 0)
	{
	    جب (sline > 0 && lnum > 1)
	    {
#اگر_تعریف_شدہ FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, تہی, TRUE, تہی);
		اگر (lnum == 1)
		{
		    // first line in buffer is folded
		    سطر_سائز = 1;
		    --sline;
		    توڑو;
		}
#اگر_ختم
		--lnum;
#اگر_تعریف_شدہ FEAT_DIFF
		اگر (lnum == wp->w_topline)
		    سطر_سائز = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		ورنہ
#اگر_ختم
		    سطر_سائز = plines_win(wp, lnum, TRUE);
		sline -= سطر_سائز;
	    }

	    اگر (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#اگر_تعریف_شدہ FEAT_FOLDING
		hasFoldingWin(wp, lnum, تہی, &lnum, TRUE, تہی);
#اگر_ختم
		lnum++;
		wp->w_wrow -= سطر_سائز + sline;
	    }
	    ورنہ اگر (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	سیٹ_topline(wp, lnum);
    }

    اگر (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    اگر (پچھلا_اونچائی > 0)
	wp->w_پچھلا_fraction_صف = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    خالی
win_new_چوڑائی(win_T *wp, صحیح چوڑائی)
{
    // Should we give an error if width < 0?
    wp->w_چوڑائی = چوڑائی < 0 ? 0 : چوڑائی;
    wp->w_سطریں_valid = 0;
    بدلا_سطر_abv_curs_win(wp);
    invalidate_botline_win(wp);

    اگر (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_حالت = true;
}

    خالی
win_comp_scroll(win_T *wp)
{
#اگر defined(FEAT_EVAL)
    صحیح old_w_p_scr = wp->w_p_scr;
#اگر_ختم

    wp->w_p_scr = ((غیرعلامتی)wp->w_اونچائی >> 1);
    اگر (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#اگر defined(FEAT_EVAL)
    اگر (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#اگر_ختم
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    خالی
command_اونچائی(خالی)
{
    صحیح		old_p_ch = curtab->tp_ch_مستعمل;

    // Find bottom frame with width of screen.
    چوکھٹا_T *frp = lastwin->w_چوکھٹا;
    جب (frp->fr_چوڑائی != topframe->fr_چوڑائی && frp->fr_والد != تہی)
	frp = frp->fr_والد;

    // Avoid changing the height of a window with 'winfixheight' set.
    جب (frp->fr_پچھلا != تہی && frp->fr_layout == FR_برگ
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_پچھلا;

    جب (p_ch > old_p_ch && command_چوکھٹا_اونچائی)
    {
	اگر (frp == تہی)
	{
	    emsg(_(e_not_enough_گنجائش));
	    p_ch = old_p_ch;
	    توڑو;
	}
	صحیح h = MIN(p_ch - old_p_ch,
			frp->fr_اونچائی - چوکھٹا_minheight(frp, تہی));
	چوکھٹا_جوڑو_اونچائی(frp, -h);
	old_p_ch += h;
	frp = frp->fr_پچھلا;
    }
    اگر (p_ch < old_p_ch && command_چوکھٹا_اونچائی && frp != تہی)
	چوکھٹا_جوڑو_اونچائی(frp, (صحیح)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_صف = صفیں - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    اگر (پیغام_scrolled == 0 && بھرا_screen)
    {
	screen_fill(cmdline_صف, (صحیح)صفیں, 0, (صحیح)Columns, ' ', ' ', 0);
	پیغام_صف = cmdline_صف;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_مستعمل = p_ch;
    min_سیٹ_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    ساکن خالی
چوکھٹا_جوڑو_اونچائی(چوکھٹا_T *frp, صحیح n)
{
    چوکھٹا_new_اونچائی(frp, frp->fr_اونچائی + n, FALSE, FALSE, FALSE);
    برائے (;;)
    {
	frp = frp->fr_والد;
	اگر (frp == تہی)
	    توڑو;
	frp->fr_اونچائی += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    خالی
آخری_حالت(
    صحیح		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    آخری_حالت_rec(topframe, آخری_stl_اونچائی(morewin) > 0);
#اگر defined(FEAT_STL_OPT)
    چوکھٹا_change_statusline_اونچائی();
#اگر_ختم
}

    ساکن خالی
آخری_حالت_rec(چوکھٹا_T *fr, صحیح statusline)
{
    چوکھٹا_T	*fp;
    win_T	*wp;

    اگر (fr->fr_layout == FR_برگ)
    {
	wp = fr->fr_win;
	اگر (wp->w_حالت_اونچائی != 0 && !statusline)
	{
	    // remove status line
	    win_new_اونچائی(wp, wp->w_اونچائی + wp->w_حالت_اونچائی);
	    wp->w_حالت_اونچائی = 0;
	    comp_ستون();
	}
	ورنہ اگر (wp->w_حالت_اونچائی == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    جب (fp->fr_اونچائی <= چوکھٹا_minheight(fp, تہی))
	    {
		اگر (fp == topframe)
		{
		    emsg(_(e_not_enough_گنجائش));
		    واپس;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		اگر (fp->fr_والد->fr_layout == FR_ستون && fp->fr_پچھلا != تہی)
		    fp = fp->fr_پچھلا;
		ورنہ
		    fp = fp->fr_والد;
	    }
	    wp->w_حالت_اونچائی = statusline_اونچائی(wp);
	    اگر (fp != fr)
	    {
		چوکھٹا_new_اونچائی(fp, fp->fr_اونچائی - wp->w_حالت_اونچائی,
			FALSE, FALSE, FALSE);
		چوکھٹا_fix_اونچائی(wp);
		win_comp_pos();
	    }
	    ورنہ
		win_new_اونچائی(wp, wp->w_اونچائی - wp->w_حالت_اونچائی);
	    comp_ستون();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	اگر (مطلق_قدر(wp->w_اونچائی - wp->w_پچھلا_اونچائی) == 1)
	    wp->w_پچھلا_اونچائی = wp->w_اونچائی;
    }
    ورنہ اگر (fr->fr_layout == FR_صف)
    {
	// vertically split windows, set status line for each one
	اگر (!statusline)
	{
	    FOR_ALL_چوکھٹے(fp, fr->fr_بچہ)
		آخری_حالت_rec(fp, statusline);
	}
	ورنہ
	{
	    چوکھٹا_T	*fp2;
	    صحیح		max_stlh = 0;
	    صحیح		new_صف_اونچائی;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_چوکھٹے(fp, fr->fr_بچہ)
		اگر (fp->fr_win != تہی && fp->fr_win->w_حالت_اونچائی == 0)
		{
		    صحیح h = statusline_اونچائی(fp->fr_win);
		    اگر (h > max_stlh)
			max_stlh = h;
		}
	    اگر (max_stlh == 0)
		واپس;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    جب (fp2->fr_اونچائی - چوکھٹا_minheight(fp2, تہی) < max_stlh)
	    {
		اگر (fp2 == topframe)
		{
		    emsg(_(e_not_enough_گنجائش));
		    واپس;
		}
		اگر (fp2->fr_والد->fr_layout == FR_ستون
						&& fp2->fr_پچھلا != تہی)
		    fp2 = fp2->fr_پچھلا;
		ورنہ
		    fp2 = fp2->fr_والد;
	    }

	    اگر (fp2 != fr)
	    {
		چوکھٹا_new_اونچائی(fp2, fp2->fr_اونچائی - max_stlh,
							FALSE, FALSE, FALSE);
		new_صف_اونچائی = fr->fr_اونچائی + max_stlh;
	    }
	    ورنہ
		new_صف_اونچائی = fr->fr_اونچائی;

	    // Set status and content heights for all leaves.
	    FOR_ALL_چوکھٹے(fp, fr->fr_بچہ)
	    {
		اگر (fp->fr_win != تہی)
		{
		    wp = fp->fr_win;
		    اگر (wp->w_حالت_اونچائی == 0)
		    {
			wp->w_حالت_اونچائی = statusline_اونچائی(wp);
			win_new_اونچائی(wp,
				  new_صف_اونچائی - wp->w_حالت_اونچائی);
			چوکھٹا_fix_اونچائی(wp);
			comp_ستون();
			redraw_all_later(UPD_SOME_VALID);
			اگر (مطلق_قدر(wp->w_اونچائی - wp->w_پچھلا_اونچائی) == 1)
			    wp->w_پچھلا_اونچائی = wp->w_اونچائی;
		    }
		}
		ورنہ
		    آخری_حالت_rec(fp, statusline); // nested frames
	    }

	    fr->fr_اونچائی = new_صف_اونچائی;
	    win_comp_pos();
	}
    }
    ورنہ
    {
	// horizontally split window, set status line for last one
	برائے (fp = fr->fr_بچہ; fp->fr_اگلا != تہی; fp = fp->fr_اگلا)
	    ;
	آخری_حالت_rec(fp, statusline);
    }
}

#اگر defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    خالی
update_stl_rendered_اونچائی(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == تہی) ? p_stl : wp->w_p_stl;
    صحیح		opt_flags = (wp == تہی) ? 0 : OPT_LOCAL;
    صحیح		*اونچائی = (wp == تہی) ? &stl_rendered_اونچائی
						: &wp->w_stl_rendered_اونچائی;

    *اونچائی = get_stl_rendered_اونچائی(wp == تہی ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    ساکن صحیح
چوکھٹا_ڈھونڈو_global_stlh_rec(چوکھٹا_T *frp, صحیح h)
{
    اگر (frp->fr_layout == FR_برگ)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	اگر (wp->w_اونچائی > 0 && wp->w_حالت_اونچائی > 0
		&& *wp->w_p_stlo == NUL)
	{
	    صحیح win_free_اونچائی = frp->fr_اونچائی - WINBAR_اونچائی(wp);

	    اگر (win_free_اونچائی - p_wmh < h)
		h = win_free_اونچائی - p_wmh;
	}
    }
    ورنہ
    {
	برائے (frp = frp->fr_بچہ; frp != تہی; frp = frp->fr_اگلا)
	    h = چوکھٹا_ڈھونڈو_global_stlh_rec(frp, h);
    }
    واپس h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    ساکن خالی
چوکھٹا_change_statusline_اونچائی_rec(چوکھٹا_T *frp, صحیح global_stlh)
{
    اگر (frp->fr_layout == FR_برگ)
    {
	win_T  *wp = frp->fr_win;

	اگر (wp->w_اونچائی > 0 && wp->w_حالت_اونچائی > 0)
	{
	    صحیح win_free_اونچائی = frp->fr_اونچائی - WINBAR_اونچائی(wp);

	    wp->w_حالت_اونچائی = statusline_اونچائی(wp);
	    اگر (wp->w_p_stlo == تہی || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		اگر (wp->w_حالت_اونچائی > global_stlh)
		    wp->w_حالت_اونچائی = global_stlh;
	    }
	    اگر (wp->w_حالت_اونچائی > win_free_اونچائی - p_wmh)
		wp->w_حالت_اونچائی = win_free_اونچائی - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    صحیح	     saved_topfill = wp->w_topfill;
# endif
	    win_new_اونچائی(wp, win_free_اونچائی - wp->w_حالت_اونچائی);
	    wp->w_topline = saved_topline;
# ifdef FEAT_DIFF
	    wp->w_topfill = saved_topfill;
# endif
	    wp->w_valid &= ~(VALID_WROW | VALID_CROW
			     | VALID_BOTLINE | VALID_BOTLINE_AP);
	}
    }
    ورنہ
    {
	برائے (frp = frp->fr_بچہ; frp != تہی; frp = frp->fr_اگلا)
	    چوکھٹا_change_statusline_اونچائی_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    صحیح
چوکھٹا_change_statusline_اونچائی(خالی)
{
    tabpage_T	*tp;
    صحیح		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = چوکھٹا_ڈھونڈو_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	چوکھٹا_change_statusline_اونچائی_rec(tp->tp_topframe, global_stlh);
    comp_ستون();
    redraw_all_later(UPD_SOME_VALID);
    واپس global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    خالی
سیٹ_stlo_mh(صحیح mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    صحیح
statuslineopt_بدلا(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    صحیح		l_stlo_fh = FALSE;
    صحیح		l_stlo_mh = 1;

    اگر (stlopt != تہی)
	p = stlopt;
    ورنہ
	p = wp->w_p_stlo;

    جب (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	اگر (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    اگر (l_stlo_mh < 1)
		واپس FAIL;
	}
	ورنہ اگر (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	اگر (*p != ',' && *p != NUL)
	    واپس FAIL;
	اگر (*p == ',')
	    ++p;
    }

    اگر (wp == تہی)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    ورنہ
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    واپس OK;
}
#اگر_ختم

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    صحیح
statusline_اونچائی(win_T *wp غیر_مستعمل)
{
    صحیح stl_اونچائی = 1;
#اگر defined(FEAT_STL_OPT)
    bool fixed;
    صحیح l_stl_rendered_اونچائی;

    اگر (wp != تہی && *wp->w_p_stlo != NUL)
    {
	stl_اونچائی = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_اونچائی = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_اونچائی
				    : stl_rendered_اونچائی;
    }
    ورنہ
    {
	stl_اونچائی = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_اونچائی = (wp != تہی && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_اونچائی
				    : stl_rendered_اونچائی;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    اگر (!fixed)
    {
	صحیح has_custom_stl = (*p_stl != NUL)
	    || (wp != تہی && *wp->w_p_stl != NUL);
	اگر (has_custom_stl)
	    stl_اونچائی = MAX(1, MIN(l_stl_rendered_اونچائی, stl_اونچائی));
	ورنہ
	    stl_اونچائی = 1;
    }
#اگر_ختم

    واپس stl_اونچائی;
}

/*
 * Return the number of lines used by the tab page line.
 */
    صحیح
tabline_اونچائی(خالی)
{
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    اگر (gui_use_tabline())
	واپس 0;
#اگر_ختم
    انتخاب (p_stal)
    {
	حالت 0: واپس 0;
	حالت 1: واپس (پہلا_tabpage->tp_اگلا == تہی) ? 0 : 1;
    }
    واپس 1;
}

/*
 * Return the height of the last window's statusline.
 */
    صحیح
آخری_stl_اونچائی(
    صحیح		morewin)	// pretend there are two or more windows
{
    واپس (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_کھڑکی)))
		? حالت_اونچائی_2 : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    صحیح
min_صفیں(خالی)
{
    اگر (firstwin == تہی)	// not initialized yet
	واپس MIN_سطریں;

    واپس چوکھٹا_minheight(curtab->tp_topframe, تہی) + tabline_اونچائی()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    صحیح
min_صفیں_for_all_tabpages(خالی)
{
    صحیح		کل;
    tabpage_T	*tp;
    صحیح		n;

    اگر (firstwin == تہی)	// not initialized yet
	واپس MIN_سطریں;

    کل = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = چوکھٹا_minheight(tp->tp_topframe, تہی);
	اگر (کل < n)
	    کل = n;
    }
    کل += tabline_اونچائی();
    کل += MIN_CMDHEIGHT;
    واپس کل;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    صحیح
only_one_کھڑکی(خالی)
{
    صحیح		گنتی = 0;
    win_T	*wp;

#اگر defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    اگر (popup_is_popup(curwin))
	واپس FALSE;
#اگر_ختم

    // If there is another tab page there always is another window.
    اگر (پہلا_tabpage->tp_اگلا != تہی)
	واپس FALSE;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_بفر != تہی
		&& (!((bt_help(wp->w_بفر) && !bt_help(curbuf))
#اگر_تعریف_شدہ FEAT_QUICKFIX
		    || wp->w_p_pvw
#اگر_ختم
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++گنتی;
    واپس (گنتی <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    ساکن خالی
check_lnums_both(صحیح do_curwin, صحیح nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر ((do_curwin || wp != curwin) && wp->w_بفر == curbuf)
	{
	    صحیح need_adjust;

	    اگر (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی;
	    اگر (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	    اگر (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_سطر_گنتی;
	    اگر (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_سطر_گنتی;
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
    خالی
check_lnums(صحیح do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    خالی
check_lnums_nested(صحیح do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    خالی
reset_lnums(خالی)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_بفر == curbuf)
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
	    اگر (wp->w_save_cursor.w_topline_save > wp->w_بفر->b_ml.ml_سطر_گنتی)
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
	واپس FAIL;
    }
    واپس OK;
}

    ساکن صحیح
make_snapshot_rec(چوکھٹا_T *fr, چوکھٹا_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(چوکھٹا_T);
    اگر (*frp == تہی)
	واپس FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_چوڑائی = fr->fr_چوڑائی;
    (*frp)->fr_اونچائی = fr->fr_اونچائی;
    اگر (fr->fr_اگلا != تہی)
    {
	اگر (make_snapshot_rec(fr->fr_اگلا, &((*frp)->fr_اگلا)) == FAIL)
	    واپس FAIL;
    }
    اگر (fr->fr_بچہ != تہی)
    {
	اگر (make_snapshot_rec(fr->fr_بچہ, &((*frp)->fr_بچہ)) == FAIL)
	    واپس FAIL;
    }
    اگر (fr->fr_layout == FR_برگ && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    واپس OK;
}

/*
 * Remove any existing snapshot.
 */
    ساکن خالی
clear_snapshot(tabpage_T *tp, صحیح idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = تہی;
}

    ساکن خالی
clear_snapshot_rec(چوکھٹا_T *fr)
{
    اگر (fr == تہی)
	واپس;
    clear_snapshot_rec(fr->fr_اگلا);
    clear_snapshot_rec(fr->fr_بچہ);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    ساکن win_T *
get_snapshot_curwin_rec(چوکھٹا_T *ft)
{
    win_T	*wp;

    اگر (ft->fr_اگلا != تہی)
    {
	اگر ((wp = get_snapshot_curwin_rec(ft->fr_اگلا)) != تہی)
	    واپس wp;
    }
    اگر (ft->fr_بچہ != تہی)
    {
	اگر ((wp = get_snapshot_curwin_rec(ft->fr_بچہ)) != تہی)
	    واپس wp;
    }

    واپس ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    ساکن win_T *
get_snapshot_curwin(صحیح idx)
{
    اگر (curtab->tp_snapshot[idx] == تہی)
	واپس تہی;

    واپس get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    خالی
restore_snapshot(
    صحیح		idx,
    صحیح		close_curwin)	    // closing current window
{
    win_T	*wp;

    اگر (curtab->tp_snapshot[idx] != تہی
	    && curtab->tp_snapshot[idx]->fr_چوڑائی == topframe->fr_چوڑائی
	    && curtab->tp_snapshot[idx]->fr_اونچائی == topframe->fr_اونچائی
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	اگر (wp != تہی && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    ساکن صحیح
check_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr)
{
    اگر (sn->fr_layout != fr->fr_layout
	    || (sn->fr_اگلا == تہی) != (fr->fr_اگلا == تہی)
	    || (sn->fr_بچہ == تہی) != (fr->fr_بچہ == تہی)
	    || (sn->fr_اگلا != تہی
		&& check_snapshot_rec(sn->fr_اگلا, fr->fr_اگلا) == FAIL)
	    || (sn->fr_بچہ != تہی
		&& check_snapshot_rec(sn->fr_بچہ, fr->fr_بچہ) == FAIL)
	    || (sn->fr_win != تہی && !win_valid(sn->fr_win)))
	واپس FAIL;
    واپس OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    ساکن win_T *
restore_snapshot_rec(چوکھٹا_T *sn, چوکھٹا_T *fr)
{
    win_T	*wp = تہی;
    win_T	*wp2;

    fr->fr_اونچائی = sn->fr_اونچائی;
    fr->fr_چوڑائی = sn->fr_چوڑائی;
    اگر (fr->fr_layout == FR_برگ)
    {
	چوکھٹا_new_اونچائی(fr, fr->fr_اونچائی, FALSE, FALSE, FALSE);
	چوکھٹا_new_چوڑائی(fr, fr->fr_چوڑائی, FALSE, FALSE);
	wp = sn->fr_win;
    }
    اگر (sn->fr_اگلا != تہی)
    {
	wp2 = restore_snapshot_rec(sn->fr_اگلا, fr->fr_اگلا);
	اگر (wp2 != تہی)
	    wp = wp2;
    }
    اگر (sn->fr_بچہ != تہی)
    {
	wp2 = restore_snapshot_rec(sn->fr_بچہ, fr->fr_بچہ);
	اگر (wp2 != تہی)
	    wp = wp2;
    }
    واپس wp;
}

#اگر defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    صحیح
win_hasvertsplit(خالی)
{
    چوکھٹا_T	*fr;

    اگر (topframe->fr_layout == FR_صف)
	واپس TRUE;

    اگر (topframe->fr_layout == FR_ستون)
	FOR_ALL_چوکھٹے(fr, topframe->fr_بچہ)
	    اگر (fr->fr_layout == FR_صف)
		واپس TRUE;

    واپس FALSE;
}
#اگر_ختم

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    صحیح
get_win_نمبر(win_T *wp, win_T *پہلا_win)
{
    صحیح		i = 1;
    win_T	*w;

    برائے (w = پہلا_win; w != تہی && w != wp; w = W_اگلا(w))
	++i;

    اگر (w == تہی)
	واپس 0;
    ورنہ
	واپس i;
}

    صحیح
get_tab_نمبر(tabpage_T *tp)
{
    صحیح		i = 1;
    tabpage_T	*t;

    برائے (t = پہلا_tabpage; t != تہی && t != tp; t = t->tp_اگلا)
	++i;

    اگر (t == تہی)
	واپس 0;
    ورنہ
	واپس i;
}
#اگر_ختم

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    ساکن صحیح
چوکھٹا_check_اونچائی(چوکھٹا_T *topfrp, صحیح اونچائی)
{
    چوکھٹا_T *frp;

    اگر (topfrp->fr_اونچائی != اونچائی)
	واپس FALSE;

    اگر (topfrp->fr_layout == FR_صف)
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    اگر (frp->fr_اونچائی != اونچائی)
		واپس FALSE;

    واپس TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    ساکن صحیح
چوکھٹا_check_چوڑائی(چوکھٹا_T *topfrp, صحیح چوڑائی)
{
    چوکھٹا_T *frp;

    اگر (topfrp->fr_چوڑائی != چوڑائی)
	واپس FALSE;

    اگر (topfrp->fr_layout == FR_ستون)
	FOR_ALL_چوکھٹے(frp, topfrp->fr_بچہ)
	    اگر (frp->fr_چوڑائی != چوڑائی)
		واپس FALSE;

    واپس TRUE;
}

#اگر defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    ساکن صحیح
int_cmp(ثابت خالی *pa, ثابت خالی *pb)
{
    ثابت صحیح a = *(ثابت صحیح *)pa;
    ثابت صحیح b = *(ثابت صحیح *)pb;
    اگر (a > b)
	واپس 1;
    اگر (a < b)
	واپس -1;
    واپس 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    حرف *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = خالی_خانے_option;
    صحیح		tw;
    صحیح		ستون;
    صحیح		گنتی = 0;
    صحیح		color_ستون_تعداد[256];
    صحیح		i;
    صحیح		j = 0;

    اگر (wp != تہی && wp->w_بفر == تہی)
	واپس تہی;  // buffer was closed

    اگر (cc != تہی)
	s = cc;
    ورنہ اگر (wp != تہی)
	s = wp->w_p_cc;

    اگر (wp != تہی)
	tw = wp->w_بفر->b_p_tw;
    ورنہ
	// buffer-local value not set, assume zero
	tw = 0;

    جب (*s != NUL && گنتی < 255)
    {
	اگر (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    ستون = (*s == '-') ? -1 : 1;
	    ++s;
	    اگر (!VIM_ISDIGIT(*s))
		واپس e_invalid_دلیل;
	    ستون = ستون * getdigits(&s);
	    اگر (tw == 0)
		جاؤ skip;  // 'textwidth' not set, skip this item
	    ستون += tw;
	    اگر (ستون < 0)
		جاؤ skip;
	}
	ورنہ اگر (VIM_ISDIGIT(*s))
	    ستون = getdigits(&s);
	ورنہ
	    واپس e_invalid_دلیل;
	color_ستون_تعداد[گنتی++] = ستون - 1;  // 1-based to 0-based
skip:
	اگر (*s == NUL)
	    توڑو;
	اگر (*s != ',')
	    واپس e_invalid_دلیل;
	اگر (*++s == NUL)
	    واپس e_invalid_دلیل;  // illegal trailing comma as in "set cc=80,"
    }

    اگر (wp == تہی)
	واپس تہی;  // only parse "cc"

    vim_free(wp->w_p_cc_ستون_تعداد);
    اگر (گنتی == 0)
	wp->w_p_cc_ستون_تعداد = تہی;
    ورنہ
    {
	wp->w_p_cc_ستون_تعداد = ALLOC_MULT(صحیح, گنتی + 1);
	اگر (wp->w_p_cc_ستون_تعداد != تہی)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_ستون_تعداد, گنتی, حجم(صحیح), int_cmp);

	    برائے (i = 0; i < گنتی; ++i)
		// skip duplicates
		اگر (j == 0 || wp->w_p_cc_ستون_تعداد[j - 1] != color_ستون_تعداد[i])
		    wp->w_p_cc_ستون_تعداد[j++] = color_ستون_تعداد[i];
	    wp->w_p_cc_ستون_تعداد[j] = -1;  // end marker
	}
    }

    واپس تہی;  // no error
}
#اگر_ختم

    صحیح
get_آخری_winid(خالی)
{
    واپس آخری_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   صحیح
win_locked(win_T *wp)
{
    واپس wp->w_locked;
}
