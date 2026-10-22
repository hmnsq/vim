/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

స్థిర శూన్య cmd_with_లెక్క(అక్షరం *cmd, char_u *bufp, size_t bufsize, దీర్ఘ Prenum);
స్థిర శూన్య win_init(win_T *newp, win_T *oldp, పూర్ణాంకం flags);
స్థిర శూన్య win_init_some(win_T *newp, win_T *oldp);
స్థిర శూన్య చట్రం_comp_pos(చట్రం_T *topfrp, పూర్ణాంకం *అడ్డువరుస, పూర్ణాంకం *నిలువు);
స్థిర శూన్య చట్రం_setheight(చట్రం_T *curfrp, పూర్ణాంకం ఎత్తు);
స్థిర శూన్య చట్రం_setwidth(చట్రం_T *curfrp, పూర్ణాంకం వెడల్పు);
స్థిర శూన్య win_exchange(దీర్ఘ);
స్థిర శూన్య win_rotate(పూర్ణాంకం, పూర్ణాంకం);
స్థిర శూన్య win_equal_rec(win_T *తదుపరి_curwin, పూర్ణాంకం current, చట్రం_T *topfr, పూర్ణాంకం dir, పూర్ణాంకం నిలువు, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు);
స్థిర శూన్య trigger_winnewpre(శూన్య);
స్థిర శూన్య trigger_winclosed(win_T *win);
స్థిర win_T *win_free_mem(win_T *win, పూర్ణాంకం *dirp, tabpage_T *tp);
స్థిర చట్రం_T *win_altframe(win_T *win, tabpage_T *tp);
స్థిర tabpage_T *alt_tabpage(శూన్య);
స్థిర win_T *frame2win(చట్రం_T *frp);
స్థిర పూర్ణాంకం చట్రం_has_win(చట్రం_T *frp, win_T *wp);
స్థిర శూన్య win_fix_scroll(పూర్ణాంకం resize);
స్థిర శూన్య win_fix_cursor(పూర్ణాంకం సాధారణ_4);
స్థిర శూన్య చట్రం_new_ఎత్తు(చట్రం_T *topfrp, పూర్ణాంకం ఎత్తు, పూర్ణాంకం topfirst, పూర్ణాంకం wfh, పూర్ణాంకం సమితి_ch);
స్థిర పూర్ణాంకం చట్రం_fixed_ఎత్తు(చట్రం_T *frp);
స్థిర పూర్ణాంకం చట్రం_fixed_వెడల్పు(చట్రం_T *frp);
స్థిర శూన్య చట్రం_కూడు_statusline(చట్రం_T *frp);
స్థిర శూన్య చట్రం_new_వెడల్పు(చట్రం_T *topfrp, పూర్ణాంకం వెడల్పు, పూర్ణాంకం leftfirst, పూర్ణాంకం wfw);
స్థిర శూన్య చట్రం_సమితి_vsep(చట్రం_T *frp, పూర్ణాంకం కూడు);
స్థిర పూర్ణాంకం చట్రం_minwidth(చట్రం_T *topfrp, win_T *తదుపరి_curwin);
స్థిర శూన్య చట్రం_fix_వెడల్పు(win_T *wp);
స్థిర పూర్ణాంకం win_alloc_firstwin(win_T *oldwin);
స్థిర శూన్య new_చట్రం(win_T *wp);
స్థిర tabpage_T *alloc_tabpage(శూన్య);
స్థిర పూర్ణాంకం leave_tabpage(buf_T *new_curbuf, పూర్ణాంకం trigger_leave_autocmds);
స్థిర శూన్య enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, పూర్ణాంకం trigger_enter_autocmds, పూర్ణాంకం trigger_leave_autocmds);
స్థిర శూన్య చట్రం_fix_ఎత్తు(win_T *wp);
స్థిర పూర్ణాంకం చట్రం_minheight(చట్రం_T *topfrp, win_T *తదుపరి_curwin);
స్థిర పూర్ణాంకం చట్రం_wincount_లోపల_ఎత్తు(చట్రం_T *topfrp);
స్థిర పూర్ణాంకం may_open_tabpage(శూన్య);
స్థిర పూర్ణాంకం win_enter_ext(win_T *wp, పూర్ణాంకం flags);
స్థిర శూన్య win_free(win_T *wp, tabpage_T *tp);
స్థిర శూన్య win_append(win_T *after, win_T *wp);
స్థిర శూన్య చట్రం_append(చట్రం_T *after, చట్రం_T *frp);
స్థిర శూన్య చట్రం_చొప్పించు(చట్రం_T *before, చట్రం_T *frp);
స్థిర శూన్య చట్రం_remove(చట్రం_T *frp);
స్థిర శూన్య win_goto_ver(పూర్ణాంకం పైకి, దీర్ఘ లెక్క);
స్థిర శూన్య win_goto_hor(పూర్ణాంకం ఎడమ_2, దీర్ఘ లెక్క);
స్థిర శూన్య చట్రం_కూడు_ఎత్తు(చట్రం_T *frp, పూర్ణాంకం n);
స్థిర శూన్య చివరి_స్థితి_rec(చట్రం_T *fr, పూర్ణాంకం statusline);
#ఒకవేళ defined(FEAT_STL_OPT)
స్థిర పూర్ణాంకం చట్రం_కనుగొను_global_stlh_rec(చట్రం_T *frp, పూర్ణాంకం h);
స్థిర శూన్య చట్రం_change_statusline_ఎత్తు_rec(చట్రం_T *frp, పూర్ణాంకం global_stlh);
#అయితే_ముగింపు
స్థిర శూన్య చట్రం_flatten(చట్రం_T *frp);
స్థిర శూన్య winframe_restore(win_T *wp, పూర్ణాంకం dir, చట్రం_T *unflat_altfr);

స్థిర పూర్ణాంకం make_snapshot_rec(చట్రం_T *fr, చట్రం_T **frp);
స్థిర శూన్య clear_snapshot(tabpage_T *tp, పూర్ణాంకం idx);
స్థిర శూన్య clear_snapshot_rec(చట్రం_T *fr);
స్థిర పూర్ణాంకం check_snapshot_rec(చట్రం_T *sn, చట్రం_T *fr);
స్థిర win_T *restore_snapshot_rec(చట్రం_T *sn, చట్రం_T *fr);
స్థిర win_T *get_snapshot_curwin(పూర్ణాంకం idx);

స్థిర పూర్ణాంకం చట్రం_check_ఎత్తు(చట్రం_T *topfrp, పూర్ణాంకం ఎత్తు);
స్థిర పూర్ణాంకం చట్రం_check_వెడల్పు(చట్రం_T *topfrp, పూర్ణాంకం వెడల్పు);

స్థిర win_T *win_alloc(win_T *after, పూర్ణాంకం hidden);

#నిర్వచించు NOWIN		((win_T *)-1)	// non-existing window

#నిర్వచించు అడ్డువరుసలు_AVAIL (అడ్డువరుసలు - p_ch - tabline_ఎత్తు())

// flags for win_enter_ext()
#నిర్వచించు WEE_UNDO_SYNC			0x01
#నిర్వచించు WEE_CURWIN_INVALID		0x02
#నిర్వచించు WEE_TRIGGER_NEW_AUTOCMDS	0x04
#నిర్వచించు WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#నిర్వచించు WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#నిర్వచించు WEE_ALLOW_PARSE_MESSAGES	0x20

స్థిర అక్షరం *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
స్థిర పూర్ణాంకం విభజించు_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
స్థిర పూర్ణాంకం close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
స్థిర పూర్ణాంకం చట్రం_locked = 0;

#ఒకవేళ defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
స్థిర పూర్ణాంకం stl_rendered_ఎత్తు = 1;
స్థిర పూర్ణాంకం stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
స్థిర పూర్ణాంకం stlo_mh = స్థితి_ఎత్తు_2; // "maxheight" value cache of global 'stlo'
#అయితే_ముగింపు

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    శూన్య
కిటికీ_layout_తాళం(శూన్య)
{
    ++విభజించు_disallowed;
    ++close_disallowed;
}

    శూన్య
కిటికీ_layout_unlock(శూన్య)
{
    --విభజించు_disallowed;
    --close_disallowed;
}

    పూర్ణాంకం
చట్రాలు_locked(శూన్య)
{
    తిరిగి చట్రం_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    పూర్ణాంకం
కిటికీ_layout_locked(గణన CMD_సూచీ cmd)
{
    ఒకవేళ (విభజించు_disallowed > 0 || close_disallowed > 0)
    {
	ఒకవేళ (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_విభజించు_కిటికీ_when_closing_బఫరు));
	లేకపోతే
	    emsg(_(e_not_allowed_గమ్యం_change_కిటికీ_layout_లోపల_this_autocmd));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

// #define WIN_DEBUG
#నిర్వచితమైతే WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    స్థిర శూన్య
log_చట్రం_layout(చట్రం_T *చట్రం)
{
    ch_log(ఖాళీ, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    చట్రం->fr_layout == FR_ఆకు ? "LEAF"
				  : చట్రం->fr_layout == FR_అడ్డువరుస ? "ROW" : "COL",
	    చట్రం->fr_వెడల్పు,
	    చట్రం->fr_ఎత్తు,
	    చట్రం->fr_win == ఖాళీ ? -1 : చట్రం->fr_win->w_వెడల్పు,
	    చట్రం->fr_win == ఖాళీ ? -1 : చట్రం->fr_win->w_ఎత్తు,
	    చట్రం->fr_win == ఖాళీ ? -1 : చట్రం->fr_win->w_id);
    ఒకవేళ (చట్రం->fr_బిడ్డ != ఖాళీ)
    {
	ch_log(ఖాళీ, "children");
	log_చట్రం_layout(చట్రం->fr_బిడ్డ);
	ఒకవేళ (చట్రం->fr_తదుపరి != ఖాళీ)
	    ch_log(ఖాళీ, "END of children");
    }
    ఒకవేళ (చట్రం->fr_తదుపరి != ఖాళీ)
	log_చట్రం_layout(చట్రం->fr_తదుపరి);
}
#అయితే_ముగింపు

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    పూర్ణాంకం
check_can_సమితి_curbuf_disabled(శూన్య)
{
    ఒకవేళ (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_గమ్యం_బఫరు));
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    పూర్ణాంకం
check_can_సమితి_curbuf_forceit(పూర్ణాంకం forceit)
{
    ఒకవేళ (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_గమ్యం_బఫరు));
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(శూన్య)
{
    // In cmdwin, the alternative buffer should be used.
    తిరిగి is_లోపల_cmdwin() && prevwin != ఖాళీ ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = ఖాళీ;

    ఒకవేళ (buf == ఖాళీ)
	తిరిగి wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    ఒకవేళ (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    ఒకవేళ (wp == ఖాళీ && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    తిరిగి wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    శూన్య
do_కిటికీ(
    పూర్ణాంకం		nchar,
    దీర్ఘ	Prenum,
    పూర్ణాంకం		xchar)	    // extra char from ":wincmd gx" or NUL
{
    దీర్ఘ	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#నిర్వచితమైతే FEAT_కనుగొను_ID
    పూర్ణాంకం		type = కనుగొను_DEFINE;
    పూర్ణాంకం		len;
#అయితే_ముగింపు
    char_u	cbuf[40];

    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;

#నిర్వచించు CHECK_CMDWIN \
    చేయి { \
	ఒకవేళ (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_లోపల_cmdline_కిటికీ)); \
	    తిరిగి; \
	} \
    } అయితే (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    ఎంపిక (nchar)
    {
// split current window in two parts, horizontally
    స్థితి 'S':
    స్థితి Ctrl_S:
    స్థితి 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ఒకవేళ (bt_quickfix(curbuf))
		    వెళ్ళు newwindow;
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		(శూన్య)win_విభజించు((పూర్ణాంకం)Prenum, 0);
		ఆపు;

// split current window in two parts, vertically
    స్థితి Ctrl_V:
    స్థితి 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		ఒకవేళ (bt_quickfix(curbuf))
		    వెళ్ళు newwindow;
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		(శూన్య)win_విభజించు((పూర్ణాంకం)Prenum, WSP_VERT);
		ఆపు;

// split current window and edit alternate file
    స్థితి Ctrl_HAT:
    స్థితి '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		ఒకవేళ (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == ఖాళీ)
		{
		    ఒకవేళ (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    లేకపోతే
			semsg(_(e_బఫరు_nr_not_దొరికింది), Prenum);
		    ఆపు;
		}

		ఒకవేళ (!curbuf_locked() && win_విభజించు(0, 0) == OK)
		    (శూన్య)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		ఆపు;

// open new window
    స్థితి Ctrl_N:
    స్థితి 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		ఒకవేళ (Prenum)
		    // window height
		    vim_snprintf((అక్షరం *)cbuf, పరిమాణం(cbuf) - 5, "%ld", Prenum);
		లేకపోతే
		    cbuf[0] = NUL;
#ఒకవేళ defined(FEAT_QUICKFIX)
		ఒకవేళ (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#అయితే_ముగింపు
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		ఆపు;

// quit current window
    స్థితి Ctrl_Q:
    స్థితి 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_లెక్క("quit", cbuf, పరిమాణం(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ఆపు;

// close current window
    స్థితి Ctrl_C:
    స్థితి 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_లెక్క("close", cbuf, పరిమాణం(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ఆపు;

#ఒకవేళ defined(FEAT_QUICKFIX)
// close preview window
    స్థితి Ctrl_Z:
    స్థితి 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		ఆపు;

// cursor to preview window
    స్థితి 'P':
		FOR_ALL_WINDOWS(wp)
		    ఒకవేళ (wp->w_p_pvw)
			ఆపు;
		ఒకవేళ (wp == ఖాళీ)
		    emsg(_(e_there_is_no_preview_కిటికీ));
		లేకపోతే
		    win_goto(wp);
		ఆపు;
#అయితే_ముగింపు

// close all but current window
    స్థితి Ctrl_O:
    స్థితి 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_లెక్క("only", cbuf, పరిమాణం(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		ఆపు;

// cursor to next window with wrap around
    స్థితి Ctrl_W:
    స్థితి 'w':
// cursor to previous window with wrap around
    స్థితి 'W':
		CHECK_CMDWIN;
		ఒకవేళ (ONE_కిటికీ && Prenum != 1)	// just one window
		    beep_flush();
		లేకపోతే
		{
		    ఒకవేళ (Prenum)			// go to specified window
		    {
			కొరకు (wp = firstwin; --Prenum > 0; )
			{
			    ఒకవేళ (wp->w_తదుపరి == ఖాళీ)
				ఆపు;
			    లేకపోతే
				wp = wp->w_తదుపరి;
			}
		    }
		    లేకపోతే
		    {
			ఒకవేళ (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    ఒకవేళ (wp == ఖాళీ)
				wp = lastwin;	    // wrap around
			}
			లేకపోతే			    // go to next window
			{
			    wp = curwin->w_తదుపరి;
			    ఒకవేళ (wp == ఖాళీ)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		ఆపు;

// cursor to window below
    స్థితి 'j':
    స్థితి K_కిందికి:
    స్థితి Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		ఆపు;

// cursor to window above
    స్థితి 'k':
    స్థితి K_పైకి:
    స్థితి Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		ఆపు;

// cursor to left window
    స్థితి 'h':
    స్థితి K_ఎడమ:
    స్థితి Ctrl_H:
    స్థితి K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		ఆపు;

// cursor to right window
    స్థితి 'l':
    స్థితి K_కుడి:
    స్థితి Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		ఆపు;

// move window to new tab page
    స్థితి 'T':
		CHECK_CMDWIN;
		ఒకవేళ (one_కిటికీ())
		    సందేశం(_(m_onlyone));
		లేకపోతే
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    ఒకవేళ (win_new_tabpage((పూర్ణాంకం)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			ఒకవేళ (curwin == wp)
			    win_close(curwin, FALSE);
			ఒకవేళ (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		ఆపు;

// cursor to top-left window
    స్థితి 't':
    స్థితి Ctrl_T:
		win_goto(firstwin);
		ఆపు;

// cursor to bottom-right window
    స్థితి 'b':
    స్థితి Ctrl_B:
		win_goto(lastwin);
		ఆపు;

// cursor to last accessed (previous) window
    స్థితి 'p':
    స్థితి Ctrl_P:
		ఒకవేళ (!win_valid(prevwin))
		    beep_flush();
		లేకపోతే
		    win_goto(prevwin);
		ఆపు;

// exchange current and next window
    స్థితి 'x':
    స్థితి Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		ఆపు;

// rotate windows downwards
    స్థితి Ctrl_R:
    స్థితి 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (పూర్ణాంకం)Prenum1);    // downwards
		ఆపు;

// rotate windows upwards
    స్థితి 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (పూర్ణాంకం)Prenum1);	    // upwards
		ఆపు;

// move window to the very top/bottom/left/right
    స్థితి 'K':
    స్థితి 'J':
    స్థితి 'H':
    స్థితి 'L':
		CHECK_CMDWIN;
		ఒకవేళ (ONE_కిటికీ)
		    beep_flush();
		లేకపోతే
		{
		    పూర్ణాంకం dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_శిఖరం : WSP_BOT);

		    (శూన్య)win_splitmove(curwin, (పూర్ణాంకం)Prenum, dir);
		}
		ఆపు;

// make all windows the same width and/or height
    స్థితి '=':
		{
		    పూర్ణాంకం mod = cmdmod.cmod_విభజించు & (WSP_VERT | WSP_HOR);
#నిర్వచితమైతే FEAT_GUI
		    need_mouse_correct = TRUE;
#అయితే_ముగింపు
		    win_equal(ఖాళీ, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		ఆపు;

// increase current window height
    స్థితి '+':
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		win_setheight(curwin->w_ఎత్తు + (పూర్ణాంకం)Prenum1);
		ఆపు;

// decrease current window height
    స్థితి '-':
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		win_setheight(curwin->w_ఎత్తు - (పూర్ణాంకం)Prenum1);
		ఆపు;

// set current window height
    స్థితి Ctrl__:
    స్థితి '_':
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		win_setheight(Prenum ? (పూర్ణాంకం)Prenum : 9999);
		ఆపు;

// increase current window width
    స్థితి '>':
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		win_setwidth(curwin->w_వెడల్పు + (పూర్ణాంకం)Prenum1);
		ఆపు;

// decrease current window width
    స్థితి '<':
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		win_setwidth(curwin->w_వెడల్పు - (పూర్ణాంకం)Prenum1);
		ఆపు;

// set current window width
    స్థితి '|':
#నిర్వచితమైతే FEAT_GUI
		need_mouse_correct = TRUE;
#అయితే_ముగింపు
		win_setwidth(Prenum != 0 ? (పూర్ణాంకం)Prenum : 9999);
		ఆపు;

// jump to tag and split window if tag exists (in preview window)
#ఒకవేళ defined(FEAT_QUICKFIX)
    స్థితి '}':
		CHECK_CMDWIN;
		ఒకవేళ (Prenum)
		    g_do_tagpreview = Prenum;
		లేకపోతే
		    g_do_tagpreview = p_pvh;
#అయితే_ముగింపు
		// FALLTHROUGH
    స్థితి ']':
    స్థితి Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		ఒకవేళ (Prenum)
		    postponed_విభజించు = Prenum;
		లేకపోతే
		    postponed_విభజించు = -1;
#నిర్వచితమైతే FEAT_QUICKFIX
		ఒకవేళ (nchar != '}')
		    g_do_tagpreview = 0;
#అయితే_ముగింపు

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_విభజించు = 0;
		ఆపు;

// edit file name under cursor in a new window
    స్థితి 'f':
    స్థితి 'F':
    స్థితి Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		ఒకవేళ (check_పాఠ్యం_or_curbuf_locked(ఖాళీ))
		    ఆపు;

		ptr = grab_file_name(Prenum1, &lnum);
		ఒకవేళ (ptr != ఖాళీ)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#నిర్వచితమైతే FEAT_GUI
		    need_mouse_correct = TRUE;
#అయితే_ముగింపు
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = ఖాళీ;
		    ఒకవేళ ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    ఒకవేళ (wp == ఖాళీ && win_విభజించు(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			ఒకవేళ (do_ecmd(0, ptr, ఖాళీ, ఖాళీ, ECMD_LASTL,
						   ECMD_HIDE, ఖాళీ) == FAIL)
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
			లేకపోతే
			    wp = curwin;
		    }

		    ఒకవేళ (wp != ఖాళీ && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		ఆపు;

#నిర్వచితమైతే FEAT_కనుగొను_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    స్థితి 'i':			    // Go to any match
    స్థితి Ctrl_I:
		type = కనుగొను_ANY;
		// FALLTHROUGH
    స్థితి 'd':			    // Go to definition, using 'define'
    స్థితి Ctrl_D:
		CHECK_CMDWIN;
		ఒకవేళ ((len = కనుగొను_ident_under_cursor(&ptr, కనుగొను_IDENT)) == 0)
		    ఆపు;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		ఒకవేళ (ptr == ఖాళీ)
		    ఆపు;

		కనుగొను_నమూనా_లోపల_మార్గం(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_విభజించు, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_సమితి_curswant = true;
		ఆపు;
#అయితే_ముగింపు

// Quickfix window only: view the result under the cursor in a new split.
#ఒకవేళ defined(FEAT_QUICKFIX)
    స్థితి K_KENTER:
    స్థితి CAR:
		ఒకవేళ (bt_quickfix(curbuf))
		    qf_view_ఫలితం(TRUE);
		ఆపు;
#అయితే_ముగింపు

// CTRL-W g  extended commands
    స్థితి 'g':
    స్థితి Ctrl_G:
		CHECK_CMDWIN;
#నిర్వచితమైతే USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#అయితే_ముగింపు
		++no_mapping;
		++allow_కీలకాలు;   // no mapping for xchar, but allow key codes
		ఒకవేళ (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_కీలకాలు;
		(శూన్య)కూడు_గమ్యం_showcmd(xchar);

		ఎంపిక (xchar)
		{
#ఒకవేళ defined(FEAT_QUICKFIX)
		    స్థితి '}':
			xchar = Ctrl_RSB;
			ఒకవేళ (Prenum)
			    g_do_tagpreview = Prenum;
			లేకపోతే
			    g_do_tagpreview = p_pvh;
#అయితే_ముగింపు
			// FALLTHROUGH
		    స్థితి ']':
		    స్థితి Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			ఒకవేళ (Prenum)
			    postponed_విభజించు = Prenum;
			లేకపోతే
			    postponed_విభజించు = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_విభజించు = 0;
			ఆపు;

		    స్థితి 'f':	    // CTRL-W gf: "gf" in a new tab page
		    స్థితి 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_సూచీ(curtab) + 1;
			nchar = xchar;
			వెళ్ళు wingotofile;

		    స్థితి 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((పూర్ణాంకం)Prenum);
			ఆపు;

		    స్థితి 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(పూర్ణాంకం)Prenum1);
			ఆపు;

		    స్థితి TAB:	    // CTRL-W g<Tab>: go to last used tab page
			ఒకవేళ (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			ఆపు;

		    అప్రమేయ:
			beep_flush();
			ఆపు;
		}
		ఆపు;

    అప్రమేయ:	beep_flush();
		ఆపు;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    శూన్య
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    ఎంపిక (*arg)
    {
    స్థితి 'S':
    స్థితి Ctrl_S:
    స్థితి 's':
    స్థితి Ctrl_N:
    స్థితి 'n':
    స్థితి 'j':
    స్థితి Ctrl_J:
    స్థితి 'k':
    స్థితి Ctrl_K:
    స్థితి 'T':
    స్థితి Ctrl_R:
    స్థితి 'r':
    స్థితి 'R':
    స్థితి 'K':
    స్థితి 'J':
    స్థితి '+':
    స్థితి '-':
    స్థితి Ctrl__:
    స్థితి '_':
    స్థితి '|':
    స్థితి ']':
    స్థితి Ctrl_RSB:
    స్థితి 'g':
    స్థితి Ctrl_G:
    స్థితి Ctrl_V:
    స్థితి 'v':
    స్థితి 'h':
    స్థితి Ctrl_H:
    స్థితి 'l':
    స్థితి Ctrl_L:
    స్థితి 'H':
    స్థితి 'L':
    స్థితి '>':
    స్థితి '<':
#ఒకవేళ defined(FEAT_QUICKFIX)
    స్థితి '}':
#అయితే_ముగింపు
    స్థితి 'f':
    స్థితి 'F':
    స్థితి Ctrl_F:
#నిర్వచితమైతే FEAT_కనుగొను_ID
    స్థితి 'i':
    స్థితి Ctrl_I:
    స్థితి 'd':
    స్థితి Ctrl_D:
#అయితే_ముగింపు
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		ఆపు;

    స్థితి Ctrl_HAT:
    స్థితి '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		ఆపు;

    స్థితి Ctrl_Q:
    స్థితి 'q':
    స్థితి Ctrl_C:
    స్థితి 'c':
    స్థితి Ctrl_O:
    స్థితి 'o':
    స్థితి Ctrl_W:
    స్థితి 'w':
    స్థితి 'W':
    స్థితి 'x':
    స్థితి Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		ఆపు;

#ఒకవేళ defined(FEAT_QUICKFIX)
    స్థితి Ctrl_Z:
    స్థితి 'z':
    స్థితి 'P':
#అయితే_ముగింపు
    స్థితి 't':
    స్థితి Ctrl_T:
    స్థితి 'b':
    స్థితి Ctrl_B:
    స్థితి 'p':
    స్థితి Ctrl_P:
    స్థితి '=':
    స్థితి CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		ఆపు;
    }
}

    స్థిర శూన్య
cmd_with_లెక్క(
    అక్షరం	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    దీర్ఘ	Prenum)
{
    ఒకవేళ (Prenum > 0)
	vim_snprintf((అక్షరం *)bufp, bufsize, "%s %ld", cmd, Prenum);
    లేకపోతే
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    పూర్ణాంకం
check_విభజించు_disallowed(win_T *wp)
{
    ఒకవేళ (విభజించు_disallowed > 0)
    {
	emsg(_(e_cant_విభజించు_కిటికీ_while_closing_another));
	తిరిగి FAIL;
    }
    ఒకవేళ (wp->w_బఫరు->b_locked_విభజించు)
    {
	emsg(_(e_cannot_విభజించు_కిటికీ_when_closing_బఫరు));
	తిరిగి FAIL;
    }
    తిరిగి OK;
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
    పూర్ణాంకం
win_విభజించు(పూర్ణాంకం కొలత, పూర్ణాంకం flags)
{
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి FAIL;

    ఒకవేళ (check_విభజించు_disallowed(curwin) == FAIL)
	తిరిగి FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    ఒకవేళ (may_open_tabpage() == OK)
	తిరిగి OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_విభజించు;
    ఒకవేళ ((flags & WSP_శిఖరం) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_విభజించు_topleft_and_botright_at_the_same_సమయం));
	తిరిగి FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    ఒకవేళ (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    లేకపోతే
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ఒకవేళ (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    లేకపోతే
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    తిరిగి win_విభజించు_ins(కొలత, flags, ఖాళీ, 0, ఖాళీ);
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
    పూర్ణాంకం
win_విభజించు_ins(
    పూర్ణాంకం		కొలత,
    పూర్ణాంకం		flags,
    win_T	*new_wp,
    పూర్ణాంకం		dir,
    చట్రం_T	*గమ్యం_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    పూర్ణాంకం		new_కొలత = కొలత;
    పూర్ణాంకం		i;
    పూర్ణాంకం		need_స్థితి = 0;
    పూర్ణాంకం		do_equal = FALSE;
    పూర్ణాంకం		needed;
    పూర్ణాంకం		అందుబాటు;
    పూర్ణాంకం		oldwin_ఎత్తు = 0;
    పూర్ణాంకం		layout;
    చట్రం_T	*frp, *curfrp, *frp2, *prevfrp;
    పూర్ణాంకం		before;
    పూర్ణాంకం		minheight;
    పూర్ణాంకం		wmh1;
    పూర్ణాంకం		did_సమితి_fraction = FALSE;
    పూర్ణాంకం		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    ఒకవేళ (new_wp == ఖాళీ)
	trigger_winnewpre();

    ఒకవేళ (flags & WSP_శిఖరం)
	oldwin = firstwin;
    లేకపోతే ఒకవేళ (flags & WSP_BOT)
	oldwin = lastwin;
    లేకపోతే
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    ఒకవేళ (ONE_కిటికీ && p_ls == 1 && oldwin->w_స్థితి_ఎత్తు == 0)
    {
	ఒకవేళ (!(flags & WSP_FORCE_స్థలం) && VISIBLE_ఎత్తు(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_స్థలం));
	    వెళ్ళు theend;
	}
	need_స్థితి = statusline_ఎత్తు(oldwin);
    }

#నిర్వచితమైతే FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    ఒకవేళ (gui.లోపల_use)
	బహిర్గతం_flush();
#అయితే_ముగింపు

    ఒకవేళ (flags & WSP_VERT)
    {
	పూర్ణాంకం	wmw1;
	పూర్ణాంకం	minwidth;

	layout = FR_అడ్డువరుస;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	ఒకవేళ (flags & WSP_స్థలం)
	    needed += p_wiw - wmw1;
	ఒకవేళ (flags & (WSP_BOT | WSP_శిఖరం))
	{
	    minwidth = చట్రం_minwidth(topframe, NOWIN);
	    అందుబాటు = topframe->fr_వెడల్పు;
	    needed += minwidth;
	}
	లేకపోతే ఒకవేళ (p_ea)
	{
	    minwidth = చట్రం_minwidth(oldwin->w_చట్రం, NOWIN);
	    prevfrp = oldwin->w_చట్రం;
	    కొరకు (frp = oldwin->w_చట్రం->fr_మాతృ; frp != ఖాళీ;
							frp = frp->fr_మాతృ)
	    {
		ఒకవేళ (frp->fr_layout == FR_అడ్డువరుస)
		    FOR_ALL_చట్రాలు(frp2, frp->fr_బిడ్డ)
			ఒకవేళ (frp2 != prevfrp)
			    minwidth += చట్రం_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    అందుబాటు = topframe->fr_వెడల్పు;
	    needed += minwidth;
	}
	లేకపోతే
	{
	    minwidth = చట్రం_minwidth(oldwin->w_చట్రం, NOWIN);
	    అందుబాటు = oldwin->w_చట్రం->fr_వెడల్పు;
	    needed += minwidth;
	}
	ఒకవేళ (!(flags & WSP_FORCE_స్థలం) && అందుబాటు < needed)
	{
	    emsg(_(e_not_enough_స్థలం));
	    వెళ్ళు theend;
	}
	ఒకవేళ (new_కొలత == 0)
	    new_కొలత = oldwin->w_వెడల్పు / 2;
	ఒకవేళ (new_కొలత > అందుబాటు - minwidth - 1)
	    new_కొలత = అందుబాటు - minwidth - 1;
	ఒకవేళ (new_కొలత < wmw1)
	    new_కొలత = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	ఒకవేళ (oldwin->w_వెడల్పు - new_కొలత - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	ఒకవేళ (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_వెడల్పు + new_కొలత + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	ఒకవేళ (!do_equal && p_ea && కొలత == 0 && *p_ead != 'v'
	    && oldwin->w_చట్రం->fr_మాతృ != ఖాళీ)
	    do_equal = TRUE;
    }
    లేకపోతే
    {
	layout = FR_నిలువు;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_ఎత్తు(curwin);
	needed = wmh1 + statusline_ఎత్తు(oldwin);
	ఒకవేళ (flags & WSP_స్థలం)
	    needed += p_wh - wmh1;
	ఒకవేళ (flags & (WSP_BOT | WSP_శిఖరం))
	{
	    minheight = చట్రం_minheight(topframe, NOWIN) + need_స్థితి;
	    అందుబాటు = topframe->fr_ఎత్తు;
	    needed += minheight;
	}
	లేకపోతే ఒకవేళ (p_ea)
	{
	    minheight = చట్రం_minheight(oldwin->w_చట్రం, NOWIN) + need_స్థితి;
	    prevfrp = oldwin->w_చట్రం;
	    కొరకు (frp = oldwin->w_చట్రం->fr_మాతృ; frp != ఖాళీ;
							frp = frp->fr_మాతృ)
	    {
		ఒకవేళ (frp->fr_layout == FR_నిలువు)
		    FOR_ALL_చట్రాలు(frp2, frp->fr_బిడ్డ)
			ఒకవేళ (frp2 != prevfrp)
			    minheight += చట్రం_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    అందుబాటు = topframe->fr_ఎత్తు;
	    needed += minheight;
	}
	లేకపోతే
	{
	    minheight = చట్రం_minheight(oldwin->w_చట్రం, NOWIN) + need_స్థితి;
	    అందుబాటు = oldwin->w_చట్రం->fr_ఎత్తు;
	    needed += minheight;
	}
	ఒకవేళ (!(flags & WSP_FORCE_స్థలం) && అందుబాటు < needed)
	{
	    emsg(_(e_not_enough_స్థలం));
	    వెళ్ళు theend;
	}
	oldwin_ఎత్తు = oldwin->w_ఎత్తు;
	ఒకవేళ (need_స్థితి)
	{
	    oldwin->w_స్థితి_ఎత్తు = statusline_ఎత్తు(oldwin);
	    oldwin_ఎత్తు -= oldwin->w_స్థితి_ఎత్తు;
	}
	// Estimated stl height for the new window.
	పూర్ణాంకం new_win_stlh = need_స్థితి != 0 ? need_స్థితి
			: statusline_ఎత్తు(new_wp != ఖాళీ ? new_wp : curwin);
	ఒకవేళ (new_కొలత == 0)
	    new_కొలత = (oldwin_ఎత్తు - new_win_stlh + 1) / 2;
	ఒకవేళ (new_కొలత > అందుబాటు - minheight - new_win_stlh)
	    new_కొలత = అందుబాటు - minheight - new_win_stlh;
	ఒకవేళ (new_కొలత < wmh1)
	    new_కొలత = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	ఒకవేళ (oldwin_ఎత్తు - new_కొలత - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	ఒకవేళ (oldwin->w_p_wfh)
	{
	    // Set w_fraction now so that the cursor keeps the same relative
	    // vertical position using the old height.
	    సమితి_fraction(oldwin);
	    did_సమితి_fraction = TRUE;

	    win_setheight_win(oldwin->w_ఎత్తు + new_కొలత
		    + statusline_ఎత్తు(oldwin), oldwin);
	    // w_height now excludes the status line
	    oldwin_ఎత్తు = oldwin->w_ఎత్తు;
	}

	// Make all windows the same height if 'equalalways' is set, 'ead' is
	// not set to 'h', a size was not provided and there is a parent frame.
	ఒకవేళ (!do_equal && p_ea && కొలత == 0 && *p_ead != 'h'
	    && oldwin->w_చట్రం->fr_మాతృ != ఖాళీ)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    ఒకవేళ ((flags & WSP_శిఖరం) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_క్రింద)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	ఒకవేళ (new_wp == ఖాళీ)
	    wp = win_alloc(oldwin, FALSE);
	లేకపోతే
	    win_append(oldwin, wp);
    }
    లేకపోతే
    {
	ఒకవేళ (new_wp == ఖాళీ)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	లేకపోతే
	    win_append(oldwin->w_prev, wp);
    }

    ఒకవేళ (new_wp == ఖాళీ)
    {
	ఒకవేళ (wp == ఖాళీ)
	    వెళ్ళు theend;

	new_చట్రం(wp);
	ఒకవేళ (wp->w_చట్రం == ఖాళీ)
	{
	    win_free(wp, ఖాళీ);
	    వెళ్ళు theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    ఒకవేళ (గమ్యం_flatten != ఖాళీ)
	చట్రం_flatten(గమ్యం_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    ఒకవేళ (flags & (WSP_శిఖరం | WSP_BOT))
    {
	ఒకవేళ ((topframe->fr_layout == FR_నిలువు && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_అడ్డువరుస && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_బిడ్డ;
	    ఒకవేళ (flags & WSP_BOT)
		అయితే (curfrp->fr_తదుపరి != ఖాళీ)
		    curfrp = curfrp->fr_తదుపరి;
	}
	లేకపోతే
	    curfrp = topframe;
	before = (flags & WSP_శిఖరం);
    }
    లేకపోతే
    {
	curfrp = oldwin->w_చట్రం;
	ఒకవేళ (flags & WSP_క్రింద)
	    before = FALSE;
	లేకపోతే ఒకవేళ (flags & WSP_ABOVE)
	    before = TRUE;
	లేకపోతే ఒకవేళ (flags & WSP_VERT)
	    before = !p_spr;
	లేకపోతే
	    before = !p_sb;
    }
    ఒకవేళ (curfrp->fr_మాతృ == ఖాళీ || curfrp->fr_మాతృ->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(చట్రం_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_మాతృ = curfrp;
	frp->fr_తదుపరి = ఖాళీ;
	frp->fr_prev = ఖాళీ;
	curfrp->fr_బిడ్డ = frp;
	curfrp->fr_win = ఖాళీ;
	curfrp = frp;
	ఒకవేళ (frp->fr_win != ఖాళీ)
	    oldwin->w_చట్రం = frp;
	లేకపోతే
	    FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
		frp->fr_మాతృ = curfrp;
    }

    ఒకవేళ (new_wp == ఖాళీ)
	frp = wp->w_చట్రం;
    లేకపోతే
	frp = new_wp->w_చట్రం;
    frp->fr_మాతృ = curfrp->fr_మాతృ;

    // Insert the new frame at the right place in the frame list.
    ఒకవేళ (before)
	చట్రం_చొప్పించు(curfrp, frp);
    లేకపోతే
	చట్రం_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    ఒకవేళ (!did_సమితి_fraction)
	సమితి_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    ఒకవేళ (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	ఒకవేళ (need_స్థితి)
	{
	    win_new_ఎత్తు(oldwin, oldwin->w_ఎత్తు - need_స్థితి);
	    oldwin->w_స్థితి_ఎత్తు = need_స్థితి;
	}
	ఒకవేళ (flags & (WSP_శిఖరం | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    పూర్ణాంకం stl_ఎత్తు = p_ls > 0 ? statusline_ఎత్తు(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_ఎత్తు();
	    win_new_ఎత్తు(wp, curfrp->fr_ఎత్తు - stl_ఎత్తు
							  - WINBAR_ఎత్తు(wp));
	    wp->w_స్థితి_ఎత్తు = stl_ఎత్తు;
	}
	లేకపోతే
	{
	    // height and row of new window is same as current window
	    wp->w_winrow = oldwin->w_winrow;
	    win_new_ఎత్తు(wp, VISIBLE_ఎత్తు(oldwin));
	    wp->w_స్థితి_ఎత్తు = oldwin->w_స్థితి_ఎత్తు;
	}
	frp->fr_ఎత్తు = curfrp->fr_ఎత్తు;

	// "new_size" of the current window goes to the new window, use
	// one column for the vertical separator
	win_new_వెడల్పు(wp, new_కొలత);
	ఒకవేళ (before)
	    wp->w_vsep_వెడల్పు = 1;
	లేకపోతే
	{
	    wp->w_vsep_వెడల్పు = oldwin->w_vsep_వెడల్పు;
	    oldwin->w_vsep_వెడల్పు = 1;
	}
	ఒకవేళ (flags & (WSP_శిఖరం | WSP_BOT))
	{
	    ఒకవేళ (flags & WSP_BOT)
		చట్రం_సమితి_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    చట్రం_new_వెడల్పు(curfrp, curfrp->fr_వెడల్పు
		     - (new_కొలత + ((flags & WSP_శిఖరం) != 0)), flags & WSP_శిఖరం,
								       FALSE);
	}
	లేకపోతే
	    win_new_వెడల్పు(oldwin, oldwin->w_వెడల్పు - (new_కొలత + 1));
	ఒకవేళ (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_కొలత + 1;
	}
	లేకపోతే		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_వెడల్పు + 1;
	చట్రం_fix_వెడల్పు(oldwin);
	చట్రం_fix_వెడల్పు(wp);
    }
    లేకపోతే
    {
	// width and column of new window is same as current window
	ఒకవేళ (flags & (WSP_శిఖరం | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_వెడల్పు(wp, topframe->fr_వెడల్పు);
	    wp->w_vsep_వెడల్పు = 0;
	}
	లేకపోతే
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_వెడల్పు(wp, oldwin->w_వెడల్పు);
	    wp->w_vsep_వెడల్పు = oldwin->w_vsep_వెడల్పు;
	}
	frp->fr_వెడల్పు = curfrp->fr_వెడల్పు;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_ఎత్తు(wp, new_కొలత);
	పూర్ణాంకం old_స్థితి_ఎత్తు = oldwin->w_స్థితి_ఎత్తు;
	ఒకవేళ (flags & (WSP_శిఖరం | WSP_BOT))
	{
	    పూర్ణాంకం new_fr_ఎత్తు = curfrp->fr_ఎత్తు - new_కొలత
							  + WINBAR_ఎత్తు(wp);

	    ఒకవేళ (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_ఎత్తు -= statusline_ఎత్తు(curfrp->fr_win);
	    ఒకవేళ (flags & WSP_BOT)
		చట్రం_కూడు_statusline(curfrp);
	    చట్రం_new_ఎత్తు(curfrp, new_fr_ఎత్తు, flags & WSP_శిఖరం, FALSE,
									FALSE);
	}
	లేకపోతే
	    win_new_ఎత్తు(oldwin, oldwin_ఎత్తు - (new_కొలత
			+ statusline_ఎత్తు(wp)));
	ఒకవేళ (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_స్థితి_ఎత్తు = statusline_ఎత్తు(wp);
	    oldwin->w_winrow += wp->w_ఎత్తు + statusline_ఎత్తు(wp);
	}
	లేకపోతే		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_ఎత్తు(oldwin)
						    + statusline_ఎత్తు(oldwin);
	    wp->w_స్థితి_ఎత్తు = old_స్థితి_ఎత్తు;
	    ఒకవేళ (!(flags & WSP_BOT))
		oldwin->w_స్థితి_ఎత్తు = statusline_ఎత్తు(oldwin);
	}
	చట్రం_fix_ఎత్తు(wp);
	చట్రం_fix_ఎత్తు(oldwin);
    }

    ఒకవేళ (flags & (WSP_శిఖరం | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    స్థితి_redraw_all();

    ఒకవేళ (need_స్థితి)
    {
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
	సందేశం_నిలువు = sc_నిలువు;
	సందేశం_clr_eos_force();	// Old command/ruler may still be there
	comp_నిలువు();
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
	సందేశం_నిలువు = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    ఒకవేళ (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    లేకపోతే ఒకవేళ (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    ఒకవేళ (flags & WSP_VERT)
    {
	i = p_wiw;
	ఒకవేళ (కొలత != 0)
	    p_wiw = కొలత;

#నిర్వచితమైతే FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	ఒకవేళ (gui.లోపల_use)
	    gui_init_which_components(ఖాళీ);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	i = p_wh;
	ఒకవేళ (కొలత != 0)
	    p_wh = కొలత;
    }

    /*
     * make the new window the current window
     */
    (శూన్య)win_enter_ext(wp, (new_wp == ఖాళీ ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    ఒకవేళ (flags & WSP_VERT)
	p_wiw = i;
    లేకపోతే
	p_wh = i;
    retval = OK;

theend:
    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    తిరిగి retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    స్థిర శూన్య
win_init(win_T *newp, win_T *oldp, పూర్ణాంకం flags వాడని)
{
    పూర్ణాంకం		i;

    newp->w_బఫరు = oldp->w_బఫరు;
#నిర్వచితమైతే FEAT_SYN_HL
    newp->w_s = &(oldp->w_బఫరు->b_s);
#అయితే_ముగింపు
    oldp->w_బఫరు->b_nwindows++;
    newp->w_cursor = oldp->w_cursor;
    newp->w_valid = 0;
    newp->w_curswant = oldp->w_curswant;
    newp->w_సమితి_curswant = oldp->w_సమితి_curswant;
    newp->w_topline = oldp->w_topline;
#నిర్వచితమైతే FEAT_DIFF
    newp->w_topfill = oldp->w_topfill;
#అయితే_ముగింపు
    newp->w_leftcol = oldp->w_leftcol;
    newp->w_pcmark = oldp->w_pcmark;
    newp->w_prev_pcmark = oldp->w_prev_pcmark;
    newp->w_alt_fnum = oldp->w_alt_fnum;
    newp->w_wrow = oldp->w_wrow;
    newp->w_fraction = oldp->w_fraction;
    newp->w_prev_fraction_అడ్డువరుస = oldp->w_prev_fraction_అడ్డువరుస;

    // Not sure if this is needed, but be safe
    remove_highlight_overrides(newp->w_hl);
    VIM_CLEAR(newp->w_hl);

    copy_jumplist(oldp, newp);
#నిర్వచితమైతే FEAT_QUICKFIX
    ఒకవేళ (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = ఖాళీ;
	newp->w_llist_ref = ఖాళీ;
    }
    లేకపోతే
	copy_loclist_దొంతర(oldp, newp);
#అయితే_ముగింపు
    newp->w_localdir = (oldp->w_localdir == ఖాళీ)
				    ? ఖాళీ : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == ఖాళీ)
				    ? ఖాళీ : vim_strsave(oldp->w_prevdir);

    ఒకవేళ (*p_spk != 'c')
    {
	ఒకవేళ (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_ఎత్తు = oldp->w_ఎత్తు - WINBAR_ఎత్తు(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_ఎత్తు(oldp);
    }

    // copy tagstack and folds
    కొరకు (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*ట్యాగ్ = &newp->w_tagstack[i];
	*ట్యాగ్ = oldp->w_tagstack[i];
	ఒకవేళ (ట్యాగ్->tagname != ఖాళీ)
	    ట్యాగ్->tagname = vim_strsave(ట్యాగ్->tagname);
	ఒకవేళ (ట్యాగ్->user_దత్తాంశం != ఖాళీ)
	    ట్యాగ్->user_దత్తాంశం = vim_strsave(ట్యాగ్->user_దత్తాంశం);
    }
    newp->w_tagstackidx = oldp->w_tagstackidx;
    newp->w_tagstacklen = oldp->w_tagstacklen;

    // Keep same changelist position in new window.
    newp->w_changelistidx = oldp->w_changelistidx;

#నిర్వచితమైతే FEAT_FOLDING
    copyFoldingState(oldp, newp);
#అయితే_ముగింపు

    win_init_some(newp, oldp);

#నిర్వచితమైతే FEAT_TERMINAL
    {
	// Make sure to also handle highlight overrides copied over from oldp.
	bool pushed = నెట్టు_highlight_overrides(newp->w_hl, newp->w_hl_len);
	ఒకవేళ (newp->w_బఫరు->b_పదం != ఖాళీ)
	    పదం_init_default_colors(newp->w_బఫరు->b_పదం);
	ఒకవేళ (pushed)
	    తీయి_highlight_overrides();
    }
#అయితే_ముగింపు
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    స్థిర శూన్య
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
    పూర్ణాంకం
win_valid_popup(win_T *win వాడని)
{
#నిర్వచితమైతే FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (wp == win)
	    తిరిగి TRUE;
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ (wp == win)
	    తిరిగి TRUE;
#అయితే_ముగింపు
    తిరిగి FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    పూర్ణాంకం
win_valid(win_T *win)
{
    win_T	*wp;

    ఒకవేళ (win == ఖాళీ)
	తిరిగి FALSE;
    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp == win)
	    తిరిగి TRUE;
    తిరిగి win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_కనుగొను_by_id(పూర్ణాంకం id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_id == id)
	    తిరిగి wp;
#నిర్వచితమైతే FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (wp->w_id == id)
	    తిరిగి wp;
    FOR_ALL_POPUPWINS_లోపల_TAB(curtab, wp)
	ఒకవేళ (wp->w_id == id)
	    తిరిగి wp;
#అయితే_ముగింపు
    తిరిగి ఖాళీ;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    పూర్ణాంకం
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    ఒకవేళ (win == ఖాళీ)
	తిరిగి FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
	{
	    ఒకవేళ (wp == win)
		తిరిగి TRUE;
	}
#నిర్వచితమైతే FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
	    ఒకవేళ (wp == win)
		తిరిగి TRUE;
#అయితే_ముగింపు
    }
    తిరిగి win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    పూర్ణాంకం
win_లెక్క(శూన్య)
{
    win_T	*wp;
    పూర్ణాంకం		లెక్క = 0;

    FOR_ALL_WINDOWS(wp)
	++లెక్క;
    తిరిగి లెక్క;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    పూర్ణాంకం
make_windows(
    పూర్ణాంకం		లెక్క,
    పూర్ణాంకం		vertical)  // split windows vertically if TRUE
{
    పూర్ణాంకం		maxcount;
    పూర్ణాంకం		todo;

    ఒకవేళ (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_వెడల్పు + curwin->w_vsep_వెడల్పు
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    లేకపోతే
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_ఎత్తు(curwin) + curwin->w_స్థితి_ఎత్తు
			- (p_wh - p_wmh)) / (p_wmh + statusline_ఎత్తు(curwin));
    }

    ఒకవేళ (maxcount < 2)
	maxcount = 2;
    ఒకవేళ (లెక్క > maxcount)
	లెక్క = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    ఒకవేళ (లెక్క > 1)
	చివరి_స్థితి(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    ఖండం_autocmds();

    // todo is number of windows left to create
    కొరకు (todo = లెక్క - 1; todo > 0; --todo)
	ఒకవేళ (vertical)
	{
	    ఒకవేళ (win_విభజించు(curwin->w_వెడల్పు - (curwin->w_వెడల్పు - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		ఆపు;
	}
	లేకపోతే
	{
	    ఒకవేళ (win_విభజించు(curwin->w_ఎత్తు - (curwin->w_ఎత్తు - todo
			    * statusline_ఎత్తు(curwin)) / (todo + 1)
			- statusline_ఎత్తు(curwin), WSP_ABOVE) == FAIL)
		ఆపు;
	}

    unblock_autocmds();

    // return actual number of windows
    తిరిగి (లెక్క - todo);
}

/*
 * Exchange current and next window
 */
    స్థిర శూన్య
win_exchange(దీర్ఘ Prenum)
{
    చట్రం_T	*frp;
    చట్రం_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    పూర్ణాంకం		తాత్కాలిక;

    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;
    ఒకవేళ (ONE_కిటికీ)	    // just one window
    {
	beep_flush();
	తిరిగి;
    }
    ఒకవేళ (పాఠ్యం_or_buf_locked())
    {
	beep_flush();
	తిరిగి;
    }

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    /*
     * find window to exchange with
     */
    ఒకవేళ (Prenum)
    {
	frp = curwin->w_చట్రం->fr_మాతృ->fr_బిడ్డ;
	అయితే (frp != ఖాళీ && --Prenum > 0)
	    frp = frp->fr_తదుపరి;
    }
    లేకపోతే ఒకవేళ (curwin->w_చట్రం->fr_తదుపరి != ఖాళీ)	// Swap with next
	frp = curwin->w_చట్రం->fr_తదుపరి;
    లేకపోతే    // Swap last window in row/col with previous
	frp = curwin->w_చట్రం->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    ఒకవేళ (frp == ఖాళీ || frp->fr_win == ఖాళీ || frp->fr_win == curwin)
	తిరిగి;
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
    frp2 = curwin->w_చట్రం->fr_prev;
    ఒకవేళ (wp->w_prev != curwin)
    {
	win_remove(curwin, ఖాళీ);
	చట్రం_remove(curwin->w_చట్రం);
	win_append(wp->w_prev, curwin);
	చట్రం_చొప్పించు(frp, curwin->w_చట్రం);
    }
    ఒకవేళ (wp != wp2)
    {
	win_remove(wp, ఖాళీ);
	చట్రం_remove(wp->w_చట్రం);
	win_append(wp2, wp);
	ఒకవేళ (frp2 == ఖాళీ)
	    చట్రం_చొప్పించు(wp->w_చట్రం->fr_మాతృ->fr_బిడ్డ, wp->w_చట్రం);
	లేకపోతే
	    చట్రం_append(frp2, wp->w_చట్రం);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    తాత్కాలిక = curwin->w_స్థితి_ఎత్తు;
    ఒకవేళ (తాత్కాలిక != wp->w_స్థితి_ఎత్తు)
    {
	win_new_ఎత్తు(curwin, curwin->w_ఎత్తు + తాత్కాలిక - wp->w_స్థితి_ఎత్తు);
	win_new_ఎత్తు(wp, wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు - తాత్కాలిక);
    }
    curwin->w_స్థితి_ఎత్తు = wp->w_స్థితి_ఎత్తు;
    wp->w_స్థితి_ఎత్తు = తాత్కాలిక;
    తాత్కాలిక = curwin->w_vsep_వెడల్పు;
    curwin->w_vsep_వెడల్పు = wp->w_vsep_వెడల్పు;
    wp->w_vsep_వెడల్పు = తాత్కాలిక;

    చట్రం_fix_ఎత్తు(curwin);
    చట్రం_fix_ఎత్తు(wp);
    చట్రం_fix_వెడల్పు(curwin);
    చట్రం_fix_వెడల్పు(wp);

    win_comp_pos();		// recompute window positions
#ఒకవేళ defined(FEAT_STL_OPT)
    చట్రం_change_statusline_ఎత్తు();
#అయితే_ముగింపు

    ఒకవేళ (wp->w_బఫరు != curbuf)
	reset_VIsual_and_resel();
    లేకపోతే ఒకవేళ (VIsual_చురుకైన)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    స్థిర శూన్య
win_rotate(పూర్ణాంకం upwards, పూర్ణాంకం లెక్క)
{
    win_T	*wp1;
    win_T	*wp2;
    చట్రం_T	*frp;
    పూర్ణాంకం		n;

    ఒకవేళ (ONE_కిటికీ)		// nothing to do
    {
	beep_flush();
	తిరిగి;
    }

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    // Check if all frames in this row/col have one window.
    FOR_ALL_చట్రాలు(frp, curwin->w_చట్రం->fr_మాతృ->fr_బిడ్డ)
	ఒకవేళ (frp->fr_win == ఖాళీ)
	{
	    emsg(_(e_cannot_rotate_when_another_కిటికీ_is_విభజించు));
	    తిరిగి;
	}

    అయితే (లెక్క--)
    {
	ఒకవేళ (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_చట్రం->fr_మాతృ->fr_బిడ్డ;
	    wp1 = frp->fr_win;
	    win_remove(wp1, ఖాళీ);
	    చట్రం_remove(frp);

	    // find last frame and append removed window/frame after it
	    కొరకు ( ; frp->fr_తదుపరి != ఖాళీ; frp = frp->fr_తదుపరి)
		;
	    win_append(frp->fr_win, wp1);
	    చట్రం_append(frp, wp1->w_చట్రం);

	    wp2 = frp->fr_win;		// previously last window
	}
	లేకపోతే			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    కొరకు (frp = curwin->w_చట్రం; frp->fr_తదుపరి != ఖాళీ;
							   frp = frp->fr_తదుపరి)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, ఖాళీ);
	    చట్రం_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_మాతృ->fr_బిడ్డ->fr_win->w_prev, wp1);
	    చట్రం_చొప్పించు(frp->fr_మాతృ->fr_బిడ్డ, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_స్థితి_ఎత్తు;
	ఒకవేళ (n != wp1->w_స్థితి_ఎత్తు)
	{
	    win_new_ఎత్తు(wp2, wp2->w_ఎత్తు + n - wp1->w_స్థితి_ఎత్తు);
	    win_new_ఎత్తు(wp1, wp1->w_ఎత్తు + wp1->w_స్థితి_ఎత్తు - n);
	}
	wp2->w_స్థితి_ఎత్తు = wp1->w_స్థితి_ఎత్తు;
	wp1->w_స్థితి_ఎత్తు = n;
	చట్రం_fix_ఎత్తు(wp1);
	చట్రం_fix_ఎత్తు(wp2);
	n = wp2->w_vsep_వెడల్పు;
	wp2->w_vsep_వెడల్పు = wp1->w_vsep_వెడల్పు;
	wp1->w_vsep_వెడల్పు = n;
	చట్రం_fix_వెడల్పు(wp1);
	చట్రం_fix_వెడల్పు(wp2);

	// recompute w_winrow and w_wincol for all windows
	win_comp_pos();
    }
#ఒకవేళ defined(FEAT_STL_OPT)
    చట్రం_change_statusline_ఎత్తు();
#అయితే_ముగింపు

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Move "wp" into a new split in a given direction, possibly relative to the
 * current window.
 * "wp" must be valid in the current tabpage.
 * Returns FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
win_splitmove(win_T *wp, పూర్ణాంకం కొలత, పూర్ణాంకం flags)
{
    పూర్ణాంకం		dir;
    పూర్ణాంకం		ఎత్తు = wp->w_ఎత్తు;
    చట్రం_T	*unflat_altfr;

    ఒకవేళ (ONE_కిటికీ)
	తిరిగి OK;	// nothing to do
    ఒకవేళ (check_విభజించు_disallowed(wp) == FAIL)
	తిరిగి FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, ఖాళీ, &unflat_altfr);
    win_remove(wp, ఖాళీ);
    చివరి_స్థితి(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    ఒకవేళ (win_విభజించు_ins(కొలత, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	తిరిగి FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    ఒకవేళ (కొలత == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(ఎత్తు, wp);
	ఒకవేళ (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#ఒకవేళ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#అయితే_ముగింపు
    తిరిగి OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    శూన్య
win_move_after(win_T *win1, win_T *win2)
{
    పూర్ణాంకం		ఎత్తు;

    // check if the arguments are reasonable
    ఒకవేళ (win1 == win2)
	తిరిగి;

    // check if there is something to do
    ఒకవేళ (win2->w_తదుపరి != win1)
    {
	ఒకవేళ (win1->w_చట్రం->fr_మాతృ != win2->w_చట్రం->fr_మాతృ)
	{
	    iemsg("Trying to move a window into another frame");
	    తిరిగి;
	}

	// may need to move the status line/vertical separator of the last
	//కిటికీ
	ఒకవేళ (win1 == lastwin)
	{
	    ఎత్తు = win1->w_prev->w_స్థితి_ఎత్తు;
	    win1->w_prev->w_స్థితి_ఎత్తు = win1->w_స్థితి_ఎత్తు;
	    win1->w_స్థితి_ఎత్తు = ఎత్తు;
	    ఒకవేళ (win1->w_prev->w_vsep_వెడల్పు == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_వెడల్పు = 0;
		win1->w_prev->w_చట్రం->fr_వెడల్పు -= 1;
		win1->w_vsep_వెడల్పు = 1;
		win1->w_చట్రం->fr_వెడల్పు += 1;
	    }
	}
	లేకపోతే ఒకవేళ (win2 == lastwin)
	{
	    ఎత్తు = win1->w_స్థితి_ఎత్తు;
	    win1->w_స్థితి_ఎత్తు = win2->w_స్థితి_ఎత్తు;
	    win2->w_స్థితి_ఎత్తు = ఎత్తు;
	    ఒకవేళ (win1->w_vsep_వెడల్పు == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_వెడల్పు = 1;
		win2->w_చట్రం->fr_వెడల్పు += 1;
		win1->w_vsep_వెడల్పు = 0;
		win1->w_చట్రం->fr_వెడల్పు -= 1;
	    }
	}
	win_remove(win1, ఖాళీ);
	చట్రం_remove(win1->w_చట్రం);
	win_append(win2, win1);
	చట్రం_append(win2->w_చట్రం, win1->w_చట్రం);

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
    శూన్య
win_equal(
    win_T	*తదుపరి_curwin,	// pointer to current window to be or NULL
    పూర్ణాంకం		current,	// do only frame with current window
    పూర్ణాంకం		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    ఒకవేళ (dir == 0)
	dir = *p_ead;
    win_equal_rec(తదుపరి_curwin == ఖాళీ ? curwin : తదుపరి_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_ఎత్తు(),
		      topframe->fr_వెడల్పు, topframe->fr_ఎత్తు);
#ఒకవేళ defined(FEAT_STL_OPT)
    చట్రం_change_statusline_ఎత్తు();
#అయితే_ముగింపు
    ఒకవేళ (!is_aucmd_win(తదుపరి_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    స్థిర శూన్య
win_equal_rec(
    win_T	*తదుపరి_curwin,	// pointer to current window to be or NULL
    పూర్ణాంకం		current,	// do only frame with current window
    చట్రం_T	*topfr,		// frame to set size off
    పూర్ణాంకం		dir,		// 'v', 'h' or 'b', see win_equal()
    పూర్ణాంకం		నిలువు,		// horizontal position for frame
    పూర్ణాంకం		అడ్డువరుస,		// vertical position for frame
    పూర్ణాంకం		వెడల్పు,		// new width of frame
    పూర్ణాంకం		ఎత్తు)		// new height of frame
{
    పూర్ణాంకం		n, m;
    పూర్ణాంకం		extra_sep = 0;
    పూర్ణాంకం		wincount, totwincount = 0;
    చట్రం_T	*fr;
    పూర్ణాంకం		తదుపరి_curwin_కొలత = 0;
    పూర్ణాంకం		స్థలం = 0;
    పూర్ణాంకం		new_కొలత;
    పూర్ణాంకం		has_తదుపరి_curwin = 0;
    పూర్ణాంకం		hnc;

    ఒకవేళ (topfr->fr_layout == FR_ఆకు)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	ఒకవేళ (topfr->fr_ఎత్తు != ఎత్తు || topfr->fr_win->w_winrow != అడ్డువరుస
		|| topfr->fr_వెడల్పు != వెడల్పు || topfr->fr_win->w_wincol != నిలువు
	   )
	{
	    topfr->fr_win->w_winrow = అడ్డువరుస;
	    చట్రం_new_ఎత్తు(topfr, ఎత్తు, FALSE, FALSE, FALSE);
	    topfr->fr_win->w_wincol = నిలువు;
	    చట్రం_new_వెడల్పు(topfr, వెడల్పు, FALSE, FALSE);
	    redraw_all_later(UPD_NOT_VALID);
	}
    }
    లేకపోతే ఒకవేళ (topfr->fr_layout == FR_అడ్డువరుస)
    {
	topfr->fr_వెడల్పు = వెడల్పు;
	topfr->fr_ఎత్తు = ఎత్తు;

	ఒకవేళ (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = చట్రం_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    ఒకవేళ (నిలువు + వెడల్పు >= firstwin->w_wincol + topframe->fr_వెడల్పు)
		extra_sep = 1;
	    లేకపోతే
		extra_sep = 0;
	    totwincount = (n + extra_sep) / (p_wmw + 1);
	    has_తదుపరి_curwin = చట్రం_has_win(topfr, తదుపరి_curwin);

	    /*
	     * Compute width for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal width when counting p_wiw for "next_curwin".
	     */
	    m = చట్రం_minwidth(topfr, తదుపరి_curwin);
	    స్థలం = వెడల్పు - m;
	    ఒకవేళ (స్థలం < 0)
	    {
		తదుపరి_curwin_కొలత = p_wiw + స్థలం;
		స్థలం = 0;
	    }
	    లేకపోతే
	    {
		తదుపరి_curwin_కొలత = -1;
		FOR_ALL_చట్రాలు(fr, topfr->fr_బిడ్డ)
		{
		    ఒకవేళ (!చట్రం_fixed_వెడల్పు(fr))
			కొనసాగు;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = చట్రం_minwidth(fr, NOWIN);
		    new_కొలత = fr->fr_వెడల్పు;
		    ఒకవేళ (చట్రం_has_win(fr, తదుపరి_curwin))
		    {
			స్థలం += p_wiw - p_wmw;
			తదుపరి_curwin_కొలత = 0;
			ఒకవేళ (new_కొలత < p_wiw)
			    new_కొలత = p_wiw;
		    }
		    లేకపోతే
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_తదుపరి == ఖాళీ
					? extra_sep : 0)) / (p_wmw + 1);
		    స్థలం -= new_కొలత - n;
		    ఒకవేళ (స్థలం < 0)
		    {
			new_కొలత += స్థలం;
			స్థలం = 0;
		    }
		    fr->fr_newwidth = new_కొలత;
		}
		ఒకవేళ (తదుపరి_curwin_కొలత == -1)
		{
		    ఒకవేళ (!has_తదుపరి_curwin)
			తదుపరి_curwin_కొలత = 0;
		    లేకపోతే ఒకవేళ (totwincount > 1
			    && (స్థలం + (totwincount - 2))
						  / (totwincount - 1) > p_wiw)
		    {
			// Can make all windows wider than 'winwidth', spread
			// the room equally.
			తదుపరి_curwin_కొలత = (స్థలం + p_wiw
					    + (totwincount - 1) * p_wmw
					    + (totwincount - 1)) / totwincount;
			స్థలం -= తదుపరి_curwin_కొలత - p_wiw;
		    }
		    లేకపోతే
			తదుపరి_curwin_కొలత = p_wiw;
		}
	    }

	    ఒకవేళ (has_తదుపరి_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_చట్రాలు(fr, topfr->fr_బిడ్డ)
	{
	    wincount = 1;
	    ఒకవేళ (fr->fr_తదుపరి == ఖాళీ)
		// last frame gets all that remains (avoid roundoff error)
		new_కొలత = వెడల్పు;
	    లేకపోతే ఒకవేళ (dir == 'v')
		new_కొలత = fr->fr_వెడల్పు;
	    లేకపోతే ఒకవేళ (చట్రం_fixed_వెడల్పు(fr))
	    {
		new_కొలత = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    లేకపోతే
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = చట్రం_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_తదుపరి == ఖాళీ ? extra_sep : 0))
								/ (p_wmw + 1);
		m = చట్రం_minwidth(fr, తదుపరి_curwin);
		ఒకవేళ (has_తదుపరి_curwin)
		    hnc = చట్రం_has_win(fr, తదుపరి_curwin);
		లేకపోతే
		    hnc = FALSE;
		ఒకవేళ (hnc)	    // don't count next_curwin
		    --wincount;
		ఒకవేళ (totwincount == 0)
		    new_కొలత = స్థలం;
		లేకపోతే
		    new_కొలత = (wincount * స్థలం + ((గుర్తులేని)totwincount >> 1))
								/ totwincount;
		ఒకవేళ (hnc)	    // add next_curwin size
		{
		    తదుపరి_curwin_కొలత -= p_wiw - (m - n);
		    ఒకవేళ (తదుపరి_curwin_కొలత < 0)
			తదుపరి_curwin_కొలత = 0;
		    new_కొలత += తదుపరి_curwin_కొలత;
		    స్థలం -= new_కొలత - తదుపరి_curwin_కొలత;
		}
		లేకపోతే
		    స్థలం -= new_కొలత;
		new_కొలత += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ఒకవేళ (!current || dir != 'v' || topfr->fr_మాతృ != ఖాళీ
		    || (new_కొలత != fr->fr_వెడల్పు)
		    || చట్రం_has_win(fr, తదుపరి_curwin))
		win_equal_rec(తదుపరి_curwin, current, fr, dir, నిలువు, అడ్డువరుస,
							    new_కొలత, ఎత్తు);
	    నిలువు += new_కొలత;
	    వెడల్పు -= new_కొలత;
	    totwincount -= wincount;
	}
    }
    లేకపోతే // topfr->fr_layout == FR_COL
    {
	topfr->fr_వెడల్పు = వెడల్పు;
	topfr->fr_ఎత్తు = ఎత్తు;

	ఒకవేళ (dir != 'h')			// equalize frame heights
	{
	    totwincount = చట్రం_wincount_లోపల_ఎత్తు(topfr);
	    has_తదుపరి_curwin = చట్రం_has_win(topfr, తదుపరి_curwin);

	    /*
	     * Compute height for "next_curwin" window and room available for
	     * other windows.
	     * "m" is the minimal height when counting p_wh for "next_curwin".
	     */
	    m = చట్రం_minheight(topfr, తదుపరి_curwin);
	    స్థలం = ఎత్తు - m;
	    ఒకవేళ (స్థలం < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		తదుపరి_curwin_కొలత = p_wh + స్థలం;
		స్థలం = 0;
	    }
	    లేకపోతే
	    {
		తదుపరి_curwin_కొలత = -1;
		FOR_ALL_చట్రాలు(fr, topfr->fr_బిడ్డ)
		{
		    ఒకవేళ (!చట్రం_fixed_ఎత్తు(fr))
			కొనసాగు;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = చట్రం_minheight(fr, NOWIN);
		    new_కొలత = fr->fr_ఎత్తు;
		    ఒకవేళ (చట్రం_has_win(fr, తదుపరి_curwin))
		    {
			స్థలం += p_wh - p_wmh;
			తదుపరి_curwin_కొలత = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			ఒకవేళ (new_కొలత < n + (p_wh - p_wmh))
			    new_కొలత = n + (p_wh - p_wmh);
		    }
		    లేకపోతే
			// These windows don't use up room.
			totwincount -= చట్రం_wincount_లోపల_ఎత్తు(fr);
		    స్థలం -= new_కొలత - n;
		    ఒకవేళ (స్థలం < 0)
		    {
			new_కొలత += స్థలం;
			స్థలం = 0;
		    }
		    fr->fr_newheight = new_కొలత;
		}
		ఒకవేళ (తదుపరి_curwin_కొలత == -1)
		{
		    ఒకవేళ (!has_తదుపరి_curwin)
			తదుపరి_curwin_కొలత = 0;
		    లేకపోతే ఒకవేళ (totwincount > 1
			    && (స్థలం + (totwincount - 2))
						   / (totwincount - 1) > p_wh)
		    {
			// can make all windows higher than 'winheight',
			// spread the room equally.
			తదుపరి_curwin_కొలత = (స్థలం + p_wh
					   + (totwincount - 1) * p_wmh
					   + (totwincount - 1)) / totwincount;
			స్థలం -= తదుపరి_curwin_కొలత - p_wh;
		    }
		    లేకపోతే
			తదుపరి_curwin_కొలత = p_wh;
		}
	    }

	    ఒకవేళ (has_తదుపరి_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_చట్రాలు(fr, topfr->fr_బిడ్డ)
	{
	    wincount = 1;
	    ఒకవేళ (fr->fr_తదుపరి == ఖాళీ)
		// last frame gets all that remains (avoid roundoff error)
		new_కొలత = ఎత్తు;
	    లేకపోతే ఒకవేళ (dir == 'h')
		new_కొలత = fr->fr_ఎత్తు;
	    లేకపోతే ఒకవేళ (చట్రం_fixed_ఎత్తు(fr))
	    {
		new_కొలత = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    లేకపోతే
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = చట్రం_minheight(fr, NOWIN);
		wincount = చట్రం_wincount_లోపల_ఎత్తు(fr);
		m = చట్రం_minheight(fr, తదుపరి_curwin);
		ఒకవేళ (has_తదుపరి_curwin)
		    hnc = చట్రం_has_win(fr, తదుపరి_curwin);
		లేకపోతే
		    hnc = FALSE;
		ఒకవేళ (hnc)	    // don't count next_curwin
		    --wincount;
		ఒకవేళ (totwincount == 0)
		    new_కొలత = స్థలం;
		లేకపోతే
		    new_కొలత = (wincount * స్థలం + ((గుర్తులేని)totwincount >> 1))
								/ totwincount;
		ఒకవేళ (hnc)	    // add next_curwin size
		{
		    తదుపరి_curwin_కొలత -= p_wh - (m - n);
		    new_కొలత += తదుపరి_curwin_కొలత;
		    స్థలం -= new_కొలత - తదుపరి_curwin_కొలత;
		}
		లేకపోతే
		    స్థలం -= new_కొలత;
		new_కొలత += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    ఒకవేళ (!current || dir != 'h' || topfr->fr_మాతృ != ఖాళీ
		    || (new_కొలత != fr->fr_ఎత్తు)
		    || చట్రం_has_win(fr, తదుపరి_curwin))
		win_equal_rec(తదుపరి_curwin, current, fr, dir, నిలువు, అడ్డువరుస,
							     వెడల్పు, new_కొలత);
	    అడ్డువరుస += new_కొలత;
	    ఎత్తు -= new_కొలత;
	    totwincount -= wincount;
	}
    }
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
    శూన్య
leaving_కిటికీ(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ఒకవేళ (!bt_prompt(win->w_బఫరు) || is_aucmd_win(win))
	తిరిగి;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_బఫరు->b_prompt_చొప్పించు = restart_edit;
    ఒకవేళ (restart_edit != 0 && బహుళకం_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    ఒకవేళ ((State & బహుళకం_చొప్పించు) && !stop_చొప్పించు_బహుళకం)
    {
	stop_చొప్పించు_బహుళకం = TRUE;
	ఒకవేళ (win->w_బఫరు->b_prompt_చొప్పించు == NUL)
	    win->w_బఫరు->b_prompt_చొప్పించు = 'A';
    }
}

    శూన్య
entering_కిటికీ(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    ఒకవేళ (!bt_prompt(win->w_బఫరు) || is_aucmd_win(win))
	తిరిగి;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    ఒకవేళ (win->w_బఫరు->b_prompt_చొప్పించు != NUL)
	stop_చొప్పించు_బహుళకం = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    ఒకవేళ ((State & బహుళకం_చొప్పించు) == 0)
	restart_edit = win->w_బఫరు->b_prompt_చొప్పించు;
}
#అయితే_ముగింపు

    స్థిర శూన్య
win_init_ఖాళీలు(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_పంక్తులు_valid = 0;
    wp->w_cursor.lnum = 1;
    wp->w_curswant = wp->w_cursor.నిలువు = 0;
    wp->w_cursor.coladd = 0;
    wp->w_pcmark.lnum = 1;	// pcmark not cleared but set to line 1
    wp->w_pcmark.నిలువు = 0;
    wp->w_prev_pcmark.lnum = 0;
    wp->w_prev_pcmark.నిలువు = 0;
    wp->w_topline = 1;
#నిర్వచితమైతే FEAT_DIFF
    wp->w_topfill = 0;
#అయితే_ముగింపు
    wp->w_botline = 2;
    wp->w_valid = 0;
#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    wp->w_s = &wp->w_బఫరు->b_s;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    పదం_reset_hlfwin(wp);
#అయితే_ముగింపు
}

/*
 * Init the current window "curwin".
 * Called when a new file is being edited.
 */
    శూన్య
curwin_init(శూన్య)
{
    win_init_ఖాళీలు(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    శూన్య
close_windows(
    buf_T	*buf,
    పూర్ణాంకం		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    పూర్ణాంకం		లెక్క = tabpage_సూచీ(ఖాళీ);

    ++RedrawingDisabled;

    కొరకు (wp = firstwin; wp != ఖాళీ && !ONE_కిటికీ; )
    {
	ఒకవేళ (wp->w_బఫరు == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_బఫరు->b_locked > 0))
	{
	    ఒకవేళ (కిటికీ_layout_locked(CMD_కొలత))
		వెళ్ళు theend;  // Only give one error message.
	    ఒకవేళ (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		ఆపు;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	లేకపోతే
	    wp = wp->w_తదుపరి;
    }

    // Also check windows in other tab pages.
    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = nexttp)
    {
	nexttp = tp->tp_తదుపరి;
	ఒకవేళ (tp != curtab)
	    FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
		ఒకవేళ (wp->w_బఫరు == buf
		    && !(win_locked(wp) || wp->w_బఫరు->b_locked > 0))
		{
		    ఒకవేళ (కిటికీ_layout_locked(CMD_కొలత))
			వెళ్ళు theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = మొదటి_tabpage;
		    ఆపు;
		}
    }

theend:
    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    ఒకవేళ (లెక్క != tabpage_సూచీ(ఖాళీ))
	apply_autocmds(EVENT_TABCLOSED, ఖాళీ, ఖాళీ, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    పూర్ణాంకం
చివరి_కిటికీ(శూన్య)
{
    తిరిగి (one_కిటికీ() && మొదటి_tabpage->tp_తదుపరి == ఖాళీ);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    పూర్ణాంకం
one_కిటికీ(శూన్య)
{
    win_T	*wp;
    పూర్ణాంకం		చూసిన_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (!is_aucmd_win(wp))
	{
	    ఒకవేళ (చూసిన_one)
		తిరిగి FALSE;
	    చూసిన_one = TRUE;
	}
    }
    తిరిగి TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    స్థిర పూర్ణాంకం
close_చివరి_కిటికీ_tabpage(
    win_T	*win,
    పూర్ణాంకం		free_buf,
    tabpage_T   *prev_curtab)
{
    ఒకవేళ (!ONE_కిటికీ)
	తిరిగి FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_బఫరు != ఖాళీ);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    ఒకవేళ (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    ఒకవేళ (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#నిర్వచితమైతే FEAT_పని_CHANNEL
    entering_కిటికీ(curwin);
#అయితే_ముగింపు
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, ఖాళీ, ఖాళీ, FALSE, curbuf);
#ఒకవేళ defined(FEAT_TABPANEL)
    ఒకవేళ (p_stpl > 0)
	shell_new_columns();
#అయితే_ముగింపు
    apply_autocmds(EVENT_WINENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    ఒకవేళ (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    తిరిగి TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    స్థిర శూన్య
win_close_బఫరు(win_T *win, పూర్ణాంకం action, పూర్ణాంకం abort_if_చివరి)
{
#నిర్వచితమైతే FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    ఒకవేళ (win->w_బఫరు != ఖాళీ)
	reset_synblock(win);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    ఒకవేళ (win->w_బఫరు != ఖాళీ && bt_quickfix(win->w_బఫరు)
					&& win->w_బఫరు->b_nwindows == 1)
	win->w_బఫరు->b_p_bl = FALSE;
#అయితే_ముగింపు

    // Close the link to the buffer.
    ఒకవేళ (win->w_బఫరు != ఖాళీ)
    {
	bufref_T    bufref;

	సమితి_bufref(&bufref, curbuf);
	++win->w_locked;
	close_బఫరు(win, win->w_బఫరు, action, abort_if_చివరి, TRUE, TRUE);
	ఒకవేళ (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	ఒకవేళ (!bufref_valid(&bufref))
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
    పూర్ణాంకం
win_close(win_T *win, పూర్ణాంకం free_buf)
{
    win_T	*wp;
    పూర్ణాంకం		other_బఫరు = FALSE;
    పూర్ణాంకం		close_curwin = FALSE;
    పూర్ణాంకం		dir;
    పూర్ణాంకం		help_కిటికీ = FALSE;
    పూర్ణాంకం		quickfix_కిటికీ = FALSE;
    tabpage_T   *prev_curtab = curtab;
    చట్రం_T	*win_చట్రం = win->w_చట్రం->fr_మాతృ;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		had_diffmode = win->w_p_diff;
#అయితే_ముగింపు
#నిర్వచితమైతే సందేశం_వరుస
    పూర్ణాంకం		did_decrement = FALSE;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    ఒకవేళ (may_close_పదం_popup() == OK)
	తిరిగి OK;
#అయితే_ముగింపు
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి FAIL;

    ఒకవేళ (చివరి_కిటికీ())
    {
	emsg(_(e_cannot_close_చివరి_కిటికీ));
	తిరిగి FAIL;
    }
    ఒకవేళ (కిటికీ_layout_locked(CMD_close))
	తిరిగి FAIL;

    ఒకవేళ (win_locked(win) || (win->w_బఫరు != ఖాళీ
					       && win->w_బఫరు->b_locked > 0))
	తిరిగి FAIL; // window is already being closed
    ఒకవేళ (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_కిటికీ));
	తిరిగి FAIL;
    }
    ఒకవేళ ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_కిటికీ())
    {
	emsg(_(e_cannot_close_కిటికీ_only_autocmd_కిటికీ_would_remain));
	తిరిగి FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    ఒకవేళ (close_చివరి_కిటికీ_tabpage(win, free_buf, prev_curtab))
	తిరిగి FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    ఒకవేళ (bt_help(win->w_బఫరు))
	help_కిటికీ = TRUE;
    లేకపోతే
	clear_snapshot(curtab, SNAP_HELP_IDX);

    ఒకవేళ (bt_quickfix(win->w_బఫరు))
	quickfix_కిటికీ = TRUE;
    లేకపోతే
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ఒకవేళ (win == curwin)
    {
#నిర్వచితమైతే FEAT_పని_CHANNEL
	leaving_కిటికీ(curwin);
#అయితే_ముగింపు
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, ఖాళీ));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	ఒకవేళ (wp->w_బఫరు != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_బఫరు = TRUE;
	    ఒకవేళ (!win_valid(win))
		తిరిగి FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	    ఒకవేళ (!win_valid(win))
		తిరిగి FAIL;
	    --win->w_locked;
	    ఒకవేళ (చివరి_కిటికీ())
		తిరిగి FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (!win_valid(win))
	    తిరిగి FAIL;
	--win->w_locked;
	ఒకవేళ (చివరి_కిటికీ())
	    తిరిగి FAIL;
#నిర్వచితమైతే FEAT_EVAL
	// autocmds may abort script processing
	ఒకవేళ (aborting())
	    తిరిగి FAIL;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    ఒకవేళ (gui.లోపల_use)
	బహిర్గతం_flush();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (popup_win_closed(win) && !win_valid(win))
	తిరిగి FAIL;
#అయితే_ముగింపు

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    ఒకవేళ (!win_valid_any_tab(win))
	తిరిగి OK;

    win_close_బఫరు(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    ఒకవేళ (win_valid(win) && win->w_బఫరు == ఖాళీ
#ఒకవేళ defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#అయితే_ముగింపు
	    && చివరి_కిటికీ())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	ఒకవేళ (curwin->w_బఫరు == ఖాళీ)
	    curwin->w_బఫరు = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    ఒకవేళ (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_బఫరు == ఖాళీ)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	తిరిగి FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    ఒకవేళ (!win_valid(win) || చివరి_కిటికీ()
	    || close_చివరి_కిటికీ_tabpage(win, free_buf, prev_curtab))
	తిరిగి FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++విభజించు_disallowed;
#నిర్వచితమైతే సందేశం_వరుస
    ++dont_parse_messages;
#అయితే_ముగింపు

    ఒకవేళ (win->w_బఫరు != ఖాళీ)
	--win->w_బఫరు->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, ఖాళీ);

    ఒకవేళ (help_కిటికీ || quickfix_కిటికీ)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_కిటికీ ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	ఒకవేళ (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    ఒకవేళ (win == curwin)
    {
	curwin = wp;
#నిర్వచితమైతే FEAT_QUICKFIX
	ఒకవేళ (wp->w_p_pvw || bt_quickfix(wp->w_బఫరు))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    కొరకు (;;)
	    {
		ఒకవేళ (wp->w_తదుపరి == ఖాళీ)
		    wp = firstwin;
		లేకపోతే
		    wp = wp->w_తదుపరి;
		ఒకవేళ (wp == curwin)
		    ఆపు;
		ఒకవేళ (!wp->w_p_pvw && !bt_quickfix(wp->w_బఫరు))
		{
		    curwin = wp;
		    ఆపు;
		}
	    }
	}
#అయితే_ముగింపు
	curbuf = curwin->w_బఫరు;
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
    చివరి_స్థితి(FALSE);

    ఒకవేళ (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_చట్రం->fr_మాతృ == win_చట్రం, dir);
    లేకపోతే
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    ఒకవేళ (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#నిర్వచితమైతే సందేశం_వరుస
	did_decrement =
#లేకపోతే
	(శూన్య)
#అయితే_ముగింపు
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	ఒకవేళ (other_బఫరు)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    }

    ఒకవేళ (ONE_కిటికీ && curwin->w_locked && curbuf->b_locked_విభజించు
	    && మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);

    --విభజించు_disallowed;
#నిర్వచితమైతే సందేశం_వరుస
    ఒకవేళ (!did_decrement)
	--dont_parse_messages;
#అయితే_ముగింపు

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    ఒకవేళ (help_కిటికీ || quickfix_కిటికీ)
	restore_snapshot(help_కిటికీ ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#నిర్వచితమైతే FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    ఒకవేళ (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	పూర్ణాంకం	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    ఒకవేళ (dwin->w_p_diff)
		++diffcount;
	ఒకవేళ (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    ఒకవేళ (gui.లోపల_use && !win_hasvertsplit())
	gui_init_which_components(ఖాళీ);
#అయితే_ముగింపు

    redraw_all_later(UPD_NOT_VALID);
    తిరిగి OK;
}

    స్థిర శూన్య
trigger_winnewpre(శూన్య)
{
    కిటికీ_layout_తాళం();
    apply_autocmds(EVENT_WINNEWPRE, ఖాళీ, ఖాళీ, FALSE, ఖాళీ);
    కిటికీ_layout_unlock();
}

    స్థిర శూన్య
trigger_winclosed(win_T *win)
{
    స్థిర పూర్ణాంకం	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    ఒకవేళ (recursive)
	తిరిగి;
    recursive = TRUE;
    vim_snprintf((అక్షరం *)winid, పరిమాణం(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_బఫరు);
    recursive = FALSE;
}

    శూన్య
trigger_tabclosedpre(tabpage_T *tp)
{
    స్థిర పూర్ణాంకం	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    ఒకవేళ (!has_tabclosedpre() || recursive)
	తిరిగి;

    ఒకవేళ (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    కిటికీ_layout_తాళం();
    apply_autocmds(EVENT_TABCLOSEDPRE, ఖాళీ, ఖాళీ, FALSE, ఖాళీ);
    కిటికీ_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    ఒకవేళ (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    లేకపోతే
	// fall back to the first tabpage
	goto_tabpage_tp(మొదటి_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    శూన్య
snapshot_windows_scroll_కొలత(శూన్య)
{
    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	wp->w_చివరి_topline = wp->w_topline;
#నిర్వచితమైతే FEAT_DIFF
	wp->w_చివరి_topfill = wp->w_topfill;
#అయితే_ముగింపు
	wp->w_చివరి_leftcol = wp->w_leftcol;
	wp->w_చివరి_skipcol = wp->w_skipcol;
	wp->w_చివరి_వెడల్పు = wp->w_వెడల్పు;
	wp->w_చివరి_ఎత్తు = wp->w_ఎత్తు;
    }
}

స్థిర పూర్ణాంకం did_initial_scroll_కొలత_snapshot = FALSE;

    శూన్య
may_make_initial_scroll_కొలత_snapshot(శూన్య)
{
    ఒకవేళ (!did_initial_scroll_కొలత_snapshot)
    {
	did_initial_scroll_కొలత_snapshot = TRUE;
	snapshot_windows_scroll_కొలత();
    }
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Create a dictionary with information about size and scroll changes in a
 * window.
 * Returns the dictionary with refcount set to one.
 * Returns NULL when out of memory.
 */
    స్థిర dict_T *
make_win_info_dict(
	పూర్ణాంకం వెడల్పు,
	పూర్ణాంకం ఎత్తు,
	పూర్ణాంకం topline,
# ifdef FEAT_DIFF
	పూర్ణాంకం topfill,
# endif
	పూర్ణాంకం leftcol,
	పూర్ణాంకం skipcol)
{
    dict_T *d = dict_alloc();
    ఒకవేళ (d == ఖాళీ)
	తిరిగి ఖాళీ;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    అయితే (1)
    {
	typval_T tv;
	tv.v_తాళం = 0;
	tv.v_type = VAR_సంఖ్య;

	tv.vval.v_సంఖ్య = వెడల్పు;
	ఒకవేళ (dict_కూడు_tv(d, "width", &tv) == FAIL)
	    ఆపు;
	tv.vval.v_సంఖ్య = ఎత్తు;
	ఒకవేళ (dict_కూడు_tv(d, "height", &tv) == FAIL)
	    ఆపు;
	tv.vval.v_సంఖ్య = topline;
	ఒకవేళ (dict_కూడు_tv(d, "topline", &tv) == FAIL)
	    ఆపు;
# ifdef FEAT_DIFF
	tv.vval.v_సంఖ్య = topfill;
# లేకపోతే
	tv.vval.v_సంఖ్య = 0;
# endif
	ఒకవేళ (dict_కూడు_tv(d, "topfill", &tv) == FAIL)
	    ఆపు;
	tv.vval.v_సంఖ్య = leftcol;
	ఒకవేళ (dict_కూడు_tv(d, "leftcol", &tv) == FAIL)
	    ఆపు;
	tv.vval.v_సంఖ్య = skipcol;
	ఒకవేళ (dict_కూడు_tv(d, "skipcol", &tv) == FAIL)
	    ఆపు;
	తిరిగి d;
    }
    dict_unref(d);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

// Return values of check_window_scroll_resize():
#నిర్వచించు CWSR_SCROLLED	1  // at least one window scrolled
#నిర్వచించు CWSR_RESIZED	2  // at least one window size changed

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
    స్థిర శూన్య
check_కిటికీ_scroll_resize(
	పూర్ణాంకం	*కొలత_లెక్క,
	win_T	**మొదటి_scroll_win,
	win_T	**మొదటి_కొలత_win,
	list_T	*winlist వాడని,
	dict_T	*v_event వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం listidx = 0;
    పూర్ణాంకం tot_వెడల్పు = 0;
    పూర్ణాంకం tot_ఎత్తు = 0;
    పూర్ణాంకం tot_topline = 0;
# ifdef FEAT_DIFF
    పూర్ణాంకం tot_topfill = 0;
# endif
    పూర్ణాంకం tot_leftcol = 0;
    పూర్ణాంకం tot_skipcol = 0;
#అయితే_ముగింపు

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	పూర్ణాంకం ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	పూర్ణాంకం కొలత_మారింది = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_చివరి_వెడల్పు != wp->w_వెడల్పు
			    || wp->w_చివరి_ఎత్తు != wp->w_ఎత్తు);
	ఒకవేళ (కొలత_మారింది)
	{
#నిర్వచితమైతే FEAT_EVAL
	    ఒకవేళ (winlist != ఖాళీ)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_తాళం = 0;
		tv.v_type = VAR_సంఖ్య;
		tv.vval.v_సంఖ్య = wp->w_id;
		list_సమితి_అంశం(winlist, listidx++, &tv);
	    }
	    లేకపోతే
#అయితే_ముగింపు
		ఒకవేళ (కొలత_లెక్క != ఖాళీ)
	    {
		++*కొలత_లెక్క;
		ఒకవేళ (*మొదటి_కొలత_win == ఖాళీ)
		    *మొదటి_కొలత_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		ఒకవేళ (*మొదటి_scroll_win == ఖాళీ && !ignore_scroll)
		    *మొదటి_scroll_win = wp;
	    }
	}

	పూర్ణాంకం scroll_మారింది = !ignore_scroll
				&& (wp->w_చివరి_topline != wp->w_topline
#నిర్వచితమైతే FEAT_DIFF
				|| wp->w_చివరి_topfill != wp->w_topfill
#అయితే_ముగింపు
				|| wp->w_చివరి_leftcol != wp->w_leftcol
				|| wp->w_చివరి_skipcol != wp->w_skipcol);
	ఒకవేళ (scroll_మారింది
	    && మొదటి_scroll_win != ఖాళీ && *మొదటి_scroll_win == ఖాళీ)
	    *మొదటి_scroll_win = wp;

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ ((కొలత_మారింది || scroll_మారింది) && v_event != ఖాళీ)
	{
	    // Add info about this window to the v:event dictionary.
	    పూర్ణాంకం వెడల్పు = wp->w_వెడల్పు - wp->w_చివరి_వెడల్పు;
	    పూర్ణాంకం ఎత్తు = wp->w_ఎత్తు - wp->w_చివరి_ఎత్తు;
	    పూర్ణాంకం topline = wp->w_topline - wp->w_చివరి_topline;
# ifdef FEAT_DIFF
	    పూర్ణాంకం topfill = wp->w_topfill - wp->w_చివరి_topfill;
# endif
	    పూర్ణాంకం leftcol = wp->w_leftcol - wp->w_చివరి_leftcol;
	    పూర్ణాంకం skipcol = wp->w_skipcol - wp->w_చివరి_skipcol;
	    dict_T *d = make_win_info_dict(వెడల్పు, ఎత్తు, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    ఒకవేళ (d == ఖాళీ)
		ఆపు;
	    అక్షరం winid[NUMBUFLEN];
	    vim_snprintf(winid, పరిమాణం(winid), "%d", wp->w_id);
	    ఒకవేళ (dict_కూడు_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		ఆపు;
	    }
	    --d->dv_refcount;

	    tot_వెడల్పు += నిరపేక్ష(వెడల్పు);
	    tot_ఎత్తు += నిరపేక్ష(ఎత్తు);
	    tot_topline += నిరపేక్ష(topline);
# ifdef FEAT_DIFF
	    tot_topfill += నిరపేక్ష(topfill);
# endif
	    tot_leftcol += నిరపేక్ష(leftcol);
	    tot_skipcol += నిరపేక్ష(skipcol);
	}
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (v_event != ఖాళీ)
    {
	dict_T *alldict = make_win_info_dict(tot_వెడల్పు, tot_ఎత్తు, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	ఒకవేళ (alldict != ఖాళీ)
	{
	    ఒకవేళ (dict_కూడు_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    లేకపోతే
		--alldict->dv_refcount;
	}
    }
#అయితే_ముగింపు
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    శూన్య
may_trigger_win_scrolled_resized(శూన్య)
{
    స్థిర పూర్ణాంకం	    recursive = FALSE;
    పూర్ణాంకం		    do_resize = has_winresized();
    పూర్ణాంకం		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    ఒకవేళ (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_కొలత_snapshot)
	తిరిగి;

    పూర్ణాంకం కొలత_లెక్క = 0;
    win_T *మొదటి_scroll_win = ఖాళీ, *మొదటి_కొలత_win = ఖాళీ;
    check_కిటికీ_scroll_resize(&కొలత_లెక్క, &మొదటి_scroll_win, &మొదటి_కొలత_win,
								    ఖాళీ, ఖాళీ);
    పూర్ణాంకం trigger_resize = do_resize && కొలత_లెక్క > 0;
    పూర్ణాంకం trigger_scroll = do_scroll && మొదటి_scroll_win != ఖాళీ;
    ఒకవేళ (!trigger_resize && !trigger_scroll)
	తిరిగి;  // no relevant changes
#నిర్వచితమైతే FEAT_EVAL
    list_T *windows_list = ఖాళీ;
    ఒకవేళ (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_వస్తువులు(కొలత_లెక్క);
	ఒకవేళ (windows_list != ఖాళీ)
	    check_కిటికీ_scroll_resize(ఖాళీ, ఖాళీ, ఖాళీ, windows_list, ఖాళీ);
    }

    dict_T *scroll_dict = ఖాళీ;
    ఒకవేళ (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	ఒకవేళ (scroll_dict != ఖాళీ)
	{
	    scroll_dict->dv_refcount = 1;
	    check_కిటికీ_scroll_resize(ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, scroll_dict);
	}
    }
#అయితే_ముగింపు

    // WinScrolled/WinResized are triggered only once, even when multiple
    // windows scrolled or changed size.  Store the current values before
    // triggering the event, if a scroll or resize happens as a side effect
    // then WinScrolled/WinResized is triggered for that later.
    snapshot_windows_scroll_కొలత();

    // "curwin" may be different from the actual current window, make
    // sure it can be restored.
    కిటికీ_layout_తాళం();
    recursive = TRUE;

    // If both are to be triggered do WinResized first.
    ఒకవేళ (trigger_resize
#నిర్వచితమైతే FEAT_EVAL
	    && windows_list != ఖాళీ
#అయితే_ముగింపు
	    )
    {
#నిర్వచితమైతే FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	ఒకవేళ (dict_కూడు_list(v_event, "windows", windows_list) == OK)
	{
	    dict_సమితి_వస్తువులు_ro(v_event);
#అయితే_ముగింపు
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((అక్షరం *)winid, పరిమాణం(winid), "%d",
							 మొదటి_కొలత_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     మొదటి_కొలత_win->w_బఫరు);
#నిర్వచితమైతే FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#అయితే_ముగింపు
    }

    ఒకవేళ (trigger_scroll
#నిర్వచితమైతే FEAT_EVAL
	    && scroll_dict != ఖాళీ
#అయితే_ముగింపు
	    )
    {
#నిర్వచితమైతే FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", ఖాళీ);
	dict_సమితి_వస్తువులు_ro(v_event);
	dict_unref(scroll_dict);
#అయితే_ముగింపు
	char_u winid[NUMBUFLEN];
	vim_snprintf((అక్షరం *)winid, పరిమాణం(winid), "%d",
						       మొదటి_scroll_win->w_id);
	apply_autocmds(EVENT_WINSCROLLED, winid, winid, FALSE,
						   మొదటి_scroll_win->w_బఫరు);
#నిర్వచితమైతే FEAT_EVAL
	restore_v_event(v_event, &save_v_event);
#అయితే_ముగింపు
    }

    recursive = FALSE;
    కిటికీ_layout_unlock();
}

/*
 * Close window "win" in tab page "tp", which is not the current tab page.
 * This may be the last window in that tab page and result in closing the tab,
 * thus "tp" may become invalid!
 * Caller must check if buffer is hidden and whether the tabline needs to be
 * updated.
 */
    శూన్య
win_close_othertab(win_T *win, పూర్ణాంకం free_buf, tabpage_T *tp)
{
    win_T	*wp;
    పూర్ణాంకం		dir;
    tabpage_T   *ptp = ఖాళీ;
    పూర్ణాంకం		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    ఒకవేళ (కిటికీ_layout_locked(CMD_కొలత))
	తిరిగి;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    ఒకవేళ (win_locked(win) || (win->w_బఫరు != ఖాళీ
					       && win->w_బఫరు->b_locked > 0))
	తిరిగి; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    ఒకవేళ (win->w_బఫరు != ఖాళీ)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	ఒకవేళ (!win_valid_any_tab(win))
	    తిరిగి;
    }

    ఒకవేళ (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	ఒకవేళ (!win_valid_any_tab(win))
	    తిరిగి;
    }

    ఒకవేళ (win->w_బఫరు != ఖాళీ)
	// Close the link to the buffer.
	close_బఫరు(win, win->w_బఫరు, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    కొరకు (ptp = మొదటి_tabpage; ptp != ఖాళీ && ptp != tp; ptp = ptp->tp_తదుపరి)
	;
    ఒకవేళ (ptp == ఖాళీ || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	ఒకవేళ (win_valid_any_tab(win) && win->w_బఫరు == ఖాళీ)
	{
	    win->w_బఫరు = firstbuf;
	    ++firstbuf->b_nwindows;
	    ఒకవేళ (win == curwin)
		curbuf = curwin->w_బఫరు;
	    win_init_ఖాళీలు(win);
	}
	తిరిగి;
    }

    // Autocommands may have closed the window already.
    కొరకు (wp = tp->tp_firstwin; wp != ఖాళీ && wp != win; wp = wp->w_తదుపరి)
	;
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి;

    // When closing the last window in a tab page remove the tab page.
    ఒకవేళ (tp->tp_firstwin == tp->tp_lastwin)
    {
	పూర్ణాంకం	h = tabline_ఎత్తు();

	ఒకవేళ (tp == మొదటి_tabpage)
	    మొదటి_tabpage = tp->tp_తదుపరి;
	లేకపోతే
	{
	    కొరకు (ptp = మొదటి_tabpage; ptp != ఖాళీ && ptp->tp_తదుపరి != tp;
							   ptp = ptp->tp_తదుపరి)
		;
	    ఒకవేళ (ptp == ఖాళీ)
	    {
		అంతర్గత_error("win_close_othertab()");
		తిరిగి;
	    }
	    ptp->tp_తదుపరి = tp->tp_తదుపరి;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	ఒకవేళ (h != tabline_ఎత్తు())
	    shell_new_అడ్డువరుసలు();
    }

    ఒకవేళ (win->w_బఫరు != ఖాళీ)
	--win->w_బఫరు->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    ఒకవేళ (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    స్థిర win_T *
win_free_mem(
    win_T	*win,
    పూర్ణాంకం		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    చట్రం_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == ఖాళీ ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_చట్రం;
    wp = winframe_remove(win, dirp, tp, ఖాళీ);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    ఒకవేళ (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    తిరిగి wp;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
win_free_all(శూన్య)
{
    పూర్ణాంకం		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = ఖాళీ;
    cmdwin_win = ఖాళీ;

    అయితే (మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
	tabpage_close(TRUE);

    కొరకు (పూర్ణాంకం i = 0; i < AUCMD_WIN_లెక్క; ++i)
	ఒకవేళ (aucmd_win[i].auc_win != ఖాళీ)
	{
	    (శూన్య)win_free_mem(aucmd_win[i].auc_win, &dummy, ఖాళీ);
	    aucmd_win[i].auc_win = ఖాళీ;
	}

    అయితే (firstwin != ఖాళీ)
	(శూన్య)win_free_mem(firstwin, &dummy, ఖాళీ);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    పూర్ణాంకం		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    చట్రం_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    చట్రం_T	*frp, *frp2, *frp3;
    చట్రం_T	*frp_close = win->w_చట్రం;
    win_T	*wp;
    పూర్ణాంకం		అడ్డువరుస, నిలువు;

    /*
     * If there is only one window there is nothing to remove.
     */
    ఒకవేళ (tp == ఖాళీ ? ONE_కిటికీ : tp->tp_firstwin == tp->tp_lastwin)
	తిరిగి ఖాళీ;

    చట్రం_locked++;

    // Save the position of the containing frame (which will also contain the
    // altframe) before we remove anything, to recompute window positions later.
    wp = frame2win(frp_close->fr_మాతృ);
    అడ్డువరుస = wp->w_winrow;
    నిలువు = wp->w_wincol;

    /*
     * Remove the window from its frame.
     */
    frp2 = win_altframe(win, tp);
    wp = frame2win(frp2);

    // If this is a rightmost window, remove vertical separators to the left.
    ఒకవేళ (win->w_vsep_వెడల్పు == 0 && frp_close->fr_మాతృ->fr_layout == FR_అడ్డువరుస &&
	    frp_close->fr_prev != ఖాళీ)
	చట్రం_సమితి_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    చట్రం_remove(frp_close);

    ఒకవేళ (frp_close->fr_మాతృ->fr_layout == FR_నిలువు)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	ఒకవేళ (frp2->fr_win != ఖాళీ && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_తదుపరి;
	    అయితే (frp != ఖాళీ || frp3 != ఖాళీ)
	    {
		ఒకవేళ (frp != ఖాళీ)
		{
		    ఒకవేళ (!చట్రం_fixed_ఎత్తు(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ఆపు;
		    }
		    frp = frp->fr_prev;
		}
		ఒకవేళ (frp3 != ఖాళీ)
		{
		    ఒకవేళ (frp3->fr_win != ఖాళీ && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ఆపు;
		    }
		    frp3 = frp3->fr_తదుపరి;
		}
	    }
	}
	చట్రం_new_ఎత్తు(frp2, frp2->fr_ఎత్తు + frp_close->fr_ఎత్తు,
			    frp2 == frp_close->fr_తదుపరి, !చట్రం_fixed_ఎత్తు(frp2), FALSE);
	*dirp = 'v';
    }
    లేకపోతే
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	ఒకవేళ (frp2->fr_win != ఖాళీ && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_తదుపరి;
	    అయితే (frp != ఖాళీ || frp3 != ఖాళీ)
	    {
		ఒకవేళ (frp != ఖాళీ)
		{
		    ఒకవేళ (!చట్రం_fixed_వెడల్పు(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			ఆపు;
		    }
		    frp = frp->fr_prev;
		}
		ఒకవేళ (frp3 != ఖాళీ)
		{
		    ఒకవేళ (frp3->fr_win != ఖాళీ && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			ఆపు;
		    }
		    frp3 = frp3->fr_తదుపరి;
		}
	    }
	}
	చట్రం_new_వెడల్పు(frp2, frp2->fr_వెడల్పు + frp_close->fr_వెడల్పు,
			    frp2 == frp_close->fr_తదుపరి, !చట్రం_fixed_వెడల్పు(frp2));
	*dirp = 'h';
    }

    // If the altframe wasn't adjacent and left/above, resizing it will have
    // changed window positions within the parent frame.  Recompute them.
    ఒకవేళ (frp2 != frp_close->fr_prev)
	చట్రం_comp_pos(frp_close->fr_మాతృ, &అడ్డువరుస, &నిలువు);

    ఒకవేళ (unflat_altfr == ఖాళీ)
	చట్రం_flatten(frp2);
    లేకపోతే
	*unflat_altfr = frp2;

    చట్రం_locked--;

    తిరిగి wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    స్థిర శూన్య
చట్రం_flatten(చట్రం_T *frp)
{
    చట్రం_T	*frp2, *frp3;

    ఒకవేళ (frp->fr_తదుపరి != ఖాళీ || frp->fr_prev != ఖాళీ)
	తిరిగి;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_మాతృ->fr_layout = frp->fr_layout;
    frp->fr_మాతృ->fr_బిడ్డ = frp->fr_బిడ్డ;
    FOR_ALL_చట్రాలు(frp2, frp->fr_బిడ్డ)
	frp2->fr_మాతృ = frp->fr_మాతృ;
    frp->fr_మాతృ->fr_win = frp->fr_win;
    ఒకవేళ (frp->fr_win != ఖాళీ)
	frp->fr_win->w_చట్రం = frp->fr_మాతృ;
    frp2 = frp->fr_మాతృ;
    ఒకవేళ (topframe->fr_బిడ్డ == frp)
	topframe->fr_బిడ్డ = frp2;
    vim_free(frp);

    frp = frp2->fr_మాతృ;
    ఒకవేళ (frp != ఖాళీ && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	ఒకవేళ (frp->fr_బిడ్డ == frp2)
	    frp->fr_బిడ్డ = frp2->fr_బిడ్డ;
	frp2->fr_బిడ్డ->fr_prev = frp2->fr_prev;
	ఒకవేళ (frp2->fr_prev != ఖాళీ)
	    frp2->fr_prev->fr_తదుపరి = frp2->fr_బిడ్డ;
	కొరకు (frp3 = frp2->fr_బిడ్డ; ; frp3 = frp3->fr_తదుపరి)
	{
	    frp3->fr_మాతృ = frp;
	    ఒకవేళ (frp3->fr_తదుపరి == ఖాళీ)
	    {
		frp3->fr_తదుపరి = frp2->fr_తదుపరి;
		ఒకవేళ (frp2->fr_తదుపరి != ఖాళీ)
		    frp2->fr_తదుపరి->fr_prev = frp3;
		ఆపు;
	    }
	}
	ఒకవేళ (topframe->fr_బిడ్డ == frp2)
	    topframe->fr_బిడ్డ = frp;
	vim_free(frp2);
    }
}

/*
 * Undo changes from a prior call to winframe_remove, also restoring lost
 * vertical separators and statuslines, and changed window positions for
 * windows within "unflat_altfr".
 * Caller must ensure no other changes were made to the layout or window sizes!
 */
    స్థిర శూన్య
winframe_restore(win_T *wp, పూర్ణాంకం dir, చట్రం_T *unflat_altfr)
{
    చట్రం_T	*frp = wp->w_చట్రం;

    // Put "wp"'s frame back where it was.
    ఒకవేళ (frp->fr_prev != ఖాళీ)
	చట్రం_append(frp->fr_prev, frp);
    లేకపోతే
	చట్రం_చొప్పించు(frp->fr_తదుపరి, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    ఒకవేళ (wp->w_vsep_వెడల్పు == 0
	    && frp->fr_మాతృ->fr_layout == FR_అడ్డువరుస && frp->fr_prev != ఖాళీ)
	చట్రం_సమితి_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    ఒకవేళ (wp->w_స్థితి_ఎత్తు == 0
	    && frp->fr_మాతృ->fr_layout == FR_నిలువు && frp->fr_prev != ఖాళీ)
	చట్రం_కూడు_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    ఒకవేళ (dir == 'v')
    {
	చట్రం_new_ఎత్తు(unflat_altfr, unflat_altfr->fr_ఎత్తు - frp->fr_ఎత్తు,
		unflat_altfr == frp->fr_తదుపరి, FALSE, FALSE);
    }
    లేకపోతే ఒకవేళ (dir == 'h')
    {
	చట్రం_new_వెడల్పు(unflat_altfr, unflat_altfr->fr_వెడల్పు - frp->fr_వెడల్పు,
		unflat_altfr == frp->fr_తదుపరి, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    ఒకవేళ (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_మాతృ);
	పూర్ణాంకం	అడ్డువరుస = topleft->w_winrow;
	పూర్ణాంకం	నిలువు = topleft->w_wincol;

	చట్రం_comp_pos(frp->fr_మాతృ, &అడ్డువరుస, &నిలువు);
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
    స్థిర చట్రం_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    చట్రం_T	*frp;
    చట్రం_T	*other_fr, *లక్ష్యం_fr;

    ఒకవేళ (tp == ఖాళీ ? ONE_కిటికీ : tp->tp_firstwin == tp->tp_lastwin)
	తిరిగి alt_tabpage()->tp_curwin->w_చట్రం;

    frp = win->w_చట్రం;

    ఒకవేళ (frp->fr_prev == ఖాళీ)
	తిరిగి frp->fr_తదుపరి;
    ఒకవేళ (frp->fr_తదుపరి == ఖాళీ)
	తిరిగి frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    //కిటికీ
    లక్ష్యం_fr = frp->fr_తదుపరి;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    ఒకవేళ (frp->fr_మాతృ != ఖాళీ && frp->fr_మాతృ->fr_layout == FR_నిలువు && p_sb)
    {
	లక్ష్యం_fr = frp->fr_prev;
	other_fr  = frp->fr_తదుపరి;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    ఒకవేళ (frp->fr_మాతృ != ఖాళీ && frp->fr_మాతృ->fr_layout == FR_అడ్డువరుస && p_spr)
    {
	లక్ష్యం_fr = frp->fr_prev;
	other_fr  = frp->fr_తదుపరి;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    ఒకవేళ (frp->fr_మాతృ != ఖాళీ && frp->fr_మాతృ->fr_layout == FR_అడ్డువరుస)
    {
	ఒకవేళ (చట్రం_fixed_వెడల్పు(లక్ష్యం_fr) && !చట్రం_fixed_వెడల్పు(other_fr))
	    లక్ష్యం_fr = other_fr;
    }
    లేకపోతే
    {
	ఒకవేళ (చట్రం_fixed_ఎత్తు(లక్ష్యం_fr) && !చట్రం_fixed_ఎత్తు(other_fr))
	    లక్ష్యం_fr = other_fr;
    }

    తిరిగి లక్ష్యం_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    స్థిర tabpage_T *
alt_tabpage(శూన్య)
{
    tabpage_T	*tp = ఖాళీ;
    పూర్ణాంకం		forward;

    // Use the last accessed tab page, if possible.
    ఒకవేళ ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	తిరిగి lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_తదుపరి != ఖాళీ &&
	    ((tcl_flags & TCL_ఎడమ) == 0 || curtab == మొదటి_tabpage);

    ఒకవేళ (forward)
	tp = curtab->tp_తదుపరి;
    లేకపోతే
	// Use the previous tab page.
	కొరకు (tp = మొదటి_tabpage; tp->tp_తదుపరి != curtab; tp = tp->tp_తదుపరి)
	    ;

    తిరిగి tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    స్థిర win_T *
frame2win(చట్రం_T *frp)
{
    అయితే (frp->fr_win == ఖాళీ)
	frp = frp->fr_బిడ్డ;
    తిరిగి frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    స్థిర పూర్ణాంకం
చట్రం_has_win(చట్రం_T *frp, win_T *wp)
{
    చట్రం_T	*p;

    ఒకవేళ (frp->fr_layout == FR_ఆకు)
	తిరిగి frp->fr_win == wp;

    FOR_ALL_చట్రాలు(p, frp->fr_బిడ్డ)
	ఒకవేళ (చట్రం_has_win(p, wp))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
స్థిర పూర్ణాంకం min_సమితి_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    స్థిర శూన్య
చట్రం_new_ఎత్తు(
    చట్రం_T	*topfrp,
    పూర్ణాంకం		ఎత్తు,
    పూర్ణాంకం		topfirst,	// resize topmost contained frame first
    పూర్ణాంకం		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    పూర్ణాంకం		సమితి_ch)		// set 'cmdheight' to resize topframe
{
    చట్రం_T	*frp;
    పూర్ణాంకం		extra_పంక్తులు;
    పూర్ణాంకం		h;

    ఒకవేళ (topfrp->fr_మాతృ == ఖాళీ && సమితి_ch)
    {
	// topframe: update the command line height, with side effects.
	పూర్ణాంకం new_ch = MAX(min_సమితి_ch, p_ch + topfrp->fr_ఎత్తు - ఎత్తు);
	పూర్ణాంకం save_ch = min_సమితి_ch;
	ఒకవేళ (new_ch != p_ch)
	    సమితి_option_విలువ((char_u *)"cmdheight", new_ch, ఖాళీ, 0);
	min_సమితి_ch = save_ch;
	ఎత్తు = MIN(ఎత్తు, అడ్డువరుసలు_AVAIL);
    }
    ఒకవేళ (topfrp->fr_win != ఖాళీ)
    {
	// Simple case: just one window.
	win_new_ఎత్తు(topfrp->fr_win,
				    ఎత్తు - topfrp->fr_win->w_స్థితి_ఎత్తు
					      - WINBAR_ఎత్తు(topfrp->fr_win));
    }
    లేకపోతే ఒకవేళ (topfrp->fr_layout == FR_అడ్డువరుస)
    {
	చేయి
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	    {
		చట్రం_new_ఎత్తు(frp, ఎత్తు, topfirst, wfh, సమితి_ch);
		ఒకవేళ (frp->fr_ఎత్తు > ఎత్తు)
		{
		    // Could not fit the windows, make the whole row higher.
		    ఎత్తు = frp->fr_ఎత్తు;
		    ఆపు;
		}
	    }
	}
	అయితే (frp != ఖాళీ);
    }
    లేకపోతే    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_బిడ్డ;
	ఒకవేళ (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    అయితే (చట్రం_fixed_ఎత్తు(frp))
	    {
		frp = frp->fr_తదుపరి;
		ఒకవేళ (frp == ఖాళీ)
		    తిరిగి;	    // no frame without 'wfh', give up
	    }
	ఒకవేళ (!topfirst)
	{
	    // Find the bottom frame of this column
	    అయితే (frp->fr_తదుపరి != ఖాళీ)
		frp = frp->fr_తదుపరి;
	    ఒకవేళ (wfh)
		// Advance back for frames with one window with 'wfh' set.
		అయితే (చట్రం_fixed_ఎత్తు(frp))
		    frp = frp->fr_prev;
	}

	extra_పంక్తులు = ఎత్తు - topfrp->fr_ఎత్తు;
	ఒకవేళ (extra_పంక్తులు < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    అయితే (frp != ఖాళీ)
	    {
		h = చట్రం_minheight(frp, ఖాళీ);
		ఒకవేళ (frp->fr_ఎత్తు + extra_పంక్తులు < h)
		{
		    extra_పంక్తులు += frp->fr_ఎత్తు - h;
		    చట్రం_new_ఎత్తు(frp, h, topfirst, wfh, సమితి_ch);
		}
		లేకపోతే
		{
		    చట్రం_new_ఎత్తు(frp, frp->fr_ఎత్తు + extra_పంక్తులు,
							topfirst, wfh, సమితి_ch);
		    ఆపు;
		}
		ఒకవేళ (topfirst)
		{
		    చేయి
			frp = frp->fr_తదుపరి;
		    అయితే (wfh && frp != ఖాళీ && చట్రం_fixed_ఎత్తు(frp));
		}
		లేకపోతే
		{
		    చేయి
			frp = frp->fr_prev;
		    అయితే (wfh && frp != ఖాళీ && చట్రం_fixed_ఎత్తు(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		ఒకవేళ (frp == ఖాళీ)
		    ఎత్తు -= extra_పంక్తులు;
	    }
	}
	లేకపోతే ఒకవేళ (extra_పంక్తులు > 0)
	{
	    // increase height of bottom or top frame
	    చట్రం_new_ఎత్తు(frp, frp->fr_ఎత్తు + extra_పంక్తులు, topfirst, wfh,
									సమితి_ch);
	}
    }
    topfrp->fr_ఎత్తు = ఎత్తు;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    స్థిర పూర్ణాంకం
చట్రం_fixed_ఎత్తు(చట్రం_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    ఒకవేళ (frp->fr_win != ఖాళీ)
	తిరిగి frp->fr_win->w_p_wfh;

    ఒకవేళ (frp->fr_layout == FR_అడ్డువరుస)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
	    ఒకవేళ (చట్రం_fixed_ఎత్తు(frp))
		తిరిగి TRUE;
	తిరిగి FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
	ఒకవేళ (!చట్రం_fixed_ఎత్తు(frp))
	    తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    స్థిర పూర్ణాంకం
చట్రం_fixed_వెడల్పు(చట్రం_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    ఒకవేళ (frp->fr_win != ఖాళీ)
	తిరిగి frp->fr_win->w_p_wfw;

    ఒకవేళ (frp->fr_layout == FR_నిలువు)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
	    ఒకవేళ (చట్రం_fixed_వెడల్పు(frp))
		తిరిగి TRUE;
	తిరిగి FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
	ఒకవేళ (!చట్రం_fixed_వెడల్పు(frp))
	    తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    స్థిర శూన్య
చట్రం_కూడు_statusline(చట్రం_T *frp)
{
    win_T	*wp;

    ఒకవేళ (frp->fr_layout == FR_ఆకు)
    {
	wp = frp->fr_win;
	wp->w_స్థితి_ఎత్తు = statusline_ఎత్తు(wp);
    }
    లేకపోతే ఒకవేళ (frp->fr_layout == FR_అడ్డువరుస)
    {
	// Handle all the frames in the row.
	FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
	    చట్రం_కూడు_statusline(frp);
    }
    లేకపోతే // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	కొరకు (frp = frp->fr_బిడ్డ; frp->fr_తదుపరి != ఖాళీ; frp = frp->fr_తదుపరి)
	    ;
	చట్రం_కూడు_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    స్థిర శూన్య
చట్రం_new_వెడల్పు(
    చట్రం_T	*topfrp,
    పూర్ణాంకం		వెడల్పు,
    పూర్ణాంకం		leftfirst,	// resize leftmost contained frame first
    పూర్ణాంకం		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    చట్రం_T	*frp;
    పూర్ణాంకం		extra_నిలువుల_సంఖ్య;
    పూర్ణాంకం		w;
    win_T	*wp;

    ఒకవేళ (topfrp->fr_layout == FR_ఆకు)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	కొరకు (frp = topfrp; frp->fr_మాతృ != ఖాళీ; frp = frp->fr_మాతృ)
	    ఒకవేళ (frp->fr_మాతృ->fr_layout == FR_అడ్డువరుస && frp->fr_తదుపరి != ఖాళీ)
		ఆపు;
	ఒకవేళ (frp->fr_మాతృ == ఖాళీ)
	    wp->w_vsep_వెడల్పు = 0;
	win_new_వెడల్పు(wp, వెడల్పు - wp->w_vsep_వెడల్పు);
    }
    లేకపోతే ఒకవేళ (topfrp->fr_layout == FR_నిలువు)
    {
	చేయి
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	    {
		చట్రం_new_వెడల్పు(frp, వెడల్పు, leftfirst, wfw);
		ఒకవేళ (frp->fr_వెడల్పు > వెడల్పు)
		{
		    // Could not fit the windows, make whole column wider.
		    వెడల్పు = frp->fr_వెడల్పు;
		    ఆపు;
		}
	    }
	} అయితే (frp != ఖాళీ);
    }
    లేకపోతే    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_బిడ్డ;
	ఒకవేళ (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    అయితే (చట్రం_fixed_వెడల్పు(frp))
	    {
		frp = frp->fr_తదుపరి;
		ఒకవేళ (frp == ఖాళీ)
		    తిరిగి;	    // no frame without 'wfw', give up
	    }
	ఒకవేళ (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    అయితే (frp->fr_తదుపరి != ఖాళీ)
		frp = frp->fr_తదుపరి;
	    ఒకవేళ (wfw)
		// Advance back for frames with one window with 'wfw' set.
		అయితే (చట్రం_fixed_వెడల్పు(frp))
		    frp = frp->fr_prev;
	}

	extra_నిలువుల_సంఖ్య = వెడల్పు - topfrp->fr_వెడల్పు;
	ఒకవేళ (extra_నిలువుల_సంఖ్య < 0)
	{
	    // reduce frame width, rightmost frame first
	    అయితే (frp != ఖాళీ)
	    {
		w = చట్రం_minwidth(frp, ఖాళీ);
		ఒకవేళ (frp->fr_వెడల్పు + extra_నిలువుల_సంఖ్య < w)
		{
		    extra_నిలువుల_సంఖ్య += frp->fr_వెడల్పు - w;
		    చట్రం_new_వెడల్పు(frp, w, leftfirst, wfw);
		}
		లేకపోతే
		{
		    చట్రం_new_వెడల్పు(frp, frp->fr_వెడల్పు + extra_నిలువుల_సంఖ్య,
							      leftfirst, wfw);
		    ఆపు;
		}
		ఒకవేళ (leftfirst)
		{
		    చేయి
			frp = frp->fr_తదుపరి;
		    అయితే (wfw && frp != ఖాళీ && చట్రం_fixed_వెడల్పు(frp));
		}
		లేకపోతే
		{
		    చేయి
			frp = frp->fr_prev;
		    అయితే (wfw && frp != ఖాళీ && చట్రం_fixed_వెడల్పు(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		ఒకవేళ (frp == ఖాళీ)
		    వెడల్పు -= extra_నిలువుల_సంఖ్య;
	    }
	}
	లేకపోతే ఒకవేళ (extra_నిలువుల_సంఖ్య > 0)
	{
	    // increase width of rightmost frame
	    చట్రం_new_వెడల్పు(frp, frp->fr_వెడల్పు + extra_నిలువుల_సంఖ్య, leftfirst, wfw);
	}
    }
    topfrp->fr_వెడల్పు = వెడల్పు;
}

/*
 * Add or remove the vertical separator of windows to the right side of "frp".
 * Note: Does not check if there is room!
 */
    స్థిర శూన్య
చట్రం_సమితి_vsep(చట్రం_T *frp, పూర్ణాంకం కూడు)
{
    win_T	*wp;

    ఒకవేళ (frp->fr_layout == FR_ఆకు)
    {
	wp = frp->fr_win;
	ఒకవేళ (కూడు && wp->w_vsep_వెడల్పు == 0)
	{
	    ఒకవేళ (wp->w_వెడల్పు > 0)	// don't make it negative
		win_new_వెడల్పు(wp, wp->w_వెడల్పు - 1);
	    wp->w_vsep_వెడల్పు = 1;
	}
	లేకపోతే ఒకవేళ (!కూడు && wp->w_vsep_వెడల్పు == 1)
	{
	    win_new_వెడల్పు(wp, wp->w_వెడల్పు + 1);
	    wp->w_vsep_వెడల్పు = 0;
	}
    }
    లేకపోతే ఒకవేళ (frp->fr_layout == FR_నిలువు)
    {
	// Handle all the frames in the column.
	FOR_ALL_చట్రాలు(frp, frp->fr_బిడ్డ)
	    చట్రం_సమితి_vsep(frp, కూడు);
    }
    లేకపోతే // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_బిడ్డ;
	అయితే (frp->fr_తదుపరి != ఖాళీ)
	    frp = frp->fr_తదుపరి;
	చట్రం_సమితి_vsep(frp, కూడు);
    }
}

/*
 * Set frame width from the window it contains.
 */
    స్థిర శూన్య
చట్రం_fix_వెడల్పు(win_T *wp)
{
    wp->w_చట్రం->fr_వెడల్పు = wp->w_వెడల్పు + wp->w_vsep_వెడల్పు;
}

/*
 * Set frame height from the window it contains.
 */
    స్థిర శూన్య
చట్రం_fix_ఎత్తు(win_T *wp)
{
    wp->w_చట్రం->fr_ఎత్తు = VISIBLE_ఎత్తు(wp) + wp->w_స్థితి_ఎత్తు;
}

/*
 * Compute the minimal height for frame "topfrp".
 * Uses the 'winminheight' option.
 * When "next_curwin" isn't NULL, use p_wh for this window.
 * When "next_curwin" is NOWIN, don't use at least one line for the current
 * window.
 */
    స్థిర పూర్ణాంకం
చట్రం_minheight(చట్రం_T *topfrp, win_T *తదుపరి_curwin)
{
    చట్రం_T	*frp;
    పూర్ణాంకం		m;
    పూర్ణాంకం		n;

    ఒకవేళ (topfrp->fr_win != ఖాళీ)
    {
	ఒకవేళ (topfrp->fr_win == తదుపరి_curwin)
	    m = p_wh + topfrp->fr_win->w_స్థితి_ఎత్తు;
	లేకపోతే
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_స్థితి_ఎత్తు;
	    ఒకవేళ (topfrp->fr_win == curwin && తదుపరి_curwin == ఖాళీ)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		ఒకవేళ (p_wmh == 0)
		    ++m;
		m += WINBAR_ఎత్తు(curwin);
	    }
	}
    }
    లేకపోతే ఒకవేళ (topfrp->fr_layout == FR_అడ్డువరుస)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	{
	    n = చట్రం_minheight(frp, తదుపరి_curwin);
	    ఒకవేళ (n > m)
		m = n;
	}
    }
    లేకపోతే
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	    m += చట్రం_minheight(frp, తదుపరి_curwin);
    }

    తిరిగి m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
స్థిర పూర్ణాంకం
చట్రం_wincount_లోపల_ఎత్తు(చట్రం_T *topfrp)
{
    చట్రం_T	*frp;
    పూర్ణాంకం		c = 0;
    పూర్ణాంకం		n;

    ఒకవేళ (topfrp->fr_win != ఖాళీ)
	c = 1;
    లేకపోతే ఒకవేళ (topfrp->fr_layout == FR_అడ్డువరుస)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	{
	    n = చట్రం_wincount_లోపల_ఎత్తు(frp);
	    ఒకవేళ (n > c)
		c = n;
	}
    }
    లేకపోతే
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	    c += చట్రం_wincount_లోపల_ఎత్తు(frp);
    }

    తిరిగి c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    స్థిర పూర్ణాంకం
చట్రం_minwidth(
    చట్రం_T	*topfrp,
    win_T	*తదుపరి_curwin)	// use p_wh and p_wiw for next_curwin
{
    చట్రం_T	*frp;
    పూర్ణాంకం		m, n;

    ఒకవేళ (topfrp->fr_win != ఖాళీ)
    {
	ఒకవేళ (topfrp->fr_win == తదుపరి_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_వెడల్పు;
	లేకపోతే
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_వెడల్పు;
	    // Current window is minimal one column wide
	    ఒకవేళ (p_wmw == 0 && topfrp->fr_win == curwin && తదుపరి_curwin == ఖాళీ)
		++m;
	}
    }
    లేకపోతే ఒకవేళ (topfrp->fr_layout == FR_నిలువు)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	{
	    n = చట్రం_minwidth(frp, తదుపరి_curwin);
	    ఒకవేళ (n > m)
		m = n;
	}
    }
    లేకపోతే
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	    m += చట్రం_minwidth(frp, తదుపరి_curwin);
    }

    తిరిగి m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    శూన్య
close_others(
    పూర్ణాంకం		సందేశం_2,
    పూర్ణాంకం		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    పూర్ణాంకం		r;

    ఒకవేళ (one_కిటికీ())
    {
	ఒకవేళ (సందేశం_2 && !autocmd_busy)
	    సందేశం(_(m_onlyone));
	తిరిగి;
    }

    // Be very careful here: autocommands may change the window layout.
    కొరకు (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_తదుపరి;

	// autocommands messed this one up
	ఒకవేళ (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_బఫరు;
	}

	ఒకవేళ (wp == curwin)		// don't close current window
	    కొనసాగు;

	// autoccommands messed this one up
	ఒకవేళ (!buf_valid(wp->w_బఫరు) && win_valid(wp))
	{
	    wp->w_బఫరు = ఖాళీ;
	    win_close(wp, 0);
	    కొనసాగు;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_బఫరు, forceit);
	ఒకవేళ (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    కొనసాగు;
	}
	ఒకవేళ (!r)
	{
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ఒకవేళ (సందేశం_2 && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_మారింది(wp->w_బఫరు, FALSE);
		ఒకవేళ (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    కొనసాగు;
		}
	    }
	    ఒకవేళ (buf_Is_మారింది(wp->w_బఫరు))
#అయితే_ముగింపు
		కొనసాగు;
	}
	win_close(wp, !buf_hide(wp->w_బఫరు) && !buf_Is_మారింది(wp->w_బఫరు));
    }

    ఒకవేళ (సందేశం_2 && !ONE_కిటికీ)
	emsg(_(e_other_కిటికీ_కలిగియుంది_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    శూన్య
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
స్థిర పూర్ణాంకం command_చట్రం_ఎత్తు = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    శూన్య
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
    పూర్ణాంకం
win_alloc_మొదటి(శూన్య)
{
    ఒకవేళ (win_alloc_firstwin(ఖాళీ) == FAIL)
	తిరిగి FAIL;

    మొదటి_tabpage = alloc_tabpage();
    ఒకవేళ (మొదటి_tabpage == ఖాళీ)
	తిరిగి FAIL;
    curtab = మొదటి_tabpage;
    unuse_tabpage(మొదటి_tabpage);

    తిరిగి OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(శూన్య)
{
    win_T *wp;

    wp = win_alloc(ఖాళీ, TRUE);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి ఖాళీ;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_చట్రం(wp);
    తిరిగి wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    శూన్య
win_init_popup_win(win_T *wp, buf_T *buf)
{
    wp->w_బఫరు = buf;
    ++buf->b_nwindows;
    win_init_ఖాళీలు(wp); // set cursor and topline to safe values

    // Make sure w_localdir is NULL to avoid a chdir() in win_enter_ext().
    VIM_CLEAR(wp->w_localdir);
}

/*
 * Allocate the first window or the first window in a new tab page.
 * When "oldwin" is NULL create an empty buffer for it.
 * When "oldwin" is not NULL copy info from it to the new window.
 * Return FAIL when something goes wrong (out of memory).
 */
    స్థిర పూర్ణాంకం
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(ఖాళీ, FALSE);
    ఒకవేళ (curwin == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (oldwin == ఖాళీ)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(ఖాళీ, ఖాళీ, 1L, BLN_LISTED);
	ఒకవేళ (curwin == ఖాళీ || curbuf == ఖాళీ)
	    తిరిగి FAIL;
	curwin->w_బఫరు = curbuf;
#నిర్వచితమైతే FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#అయితే_ముగింపు
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    లేకపోతే
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_చట్రం(curwin);
    ఒకవేళ (curwin->w_చట్రం == ఖాళీ)
	తిరిగి FAIL;
    topframe = curwin->w_చట్రం;
    topframe->fr_వెడల్పు = COLUMNS_WITHOUT_TPL();
    topframe->fr_ఎత్తు = అడ్డువరుసలు - p_ch;

    తిరిగి OK;
}

/*
 * Create a frame for window "wp".
 */
    స్థిర శూన్య
new_చట్రం(win_T *wp)
{
    చట్రం_T *frp = ALLOC_CLEAR_ONE(చట్రం_T);

    wp->w_చట్రం = frp;
    ఒకవేళ (frp == ఖాళీ)
	తిరిగి;
    frp->fr_layout = FR_ఆకు;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    శూన్య
win_init_కొలత(శూన్య)
{
    firstwin->w_ఎత్తు = అడ్డువరుసలు_AVAIL;
    firstwin->w_prev_ఎత్తు = అడ్డువరుసలు_AVAIL;
    topframe->fr_ఎత్తు = అడ్డువరుసలు_AVAIL;
    firstwin->w_వెడల్పు = topframe->fr_వెడల్పు;
}

/*
 * Allocate a new tabpage_T and init the values.
 * Returns NULL when out of memory.
 */
    స్థిర tabpage_T *
alloc_tabpage(శూన్య)
{
    tabpage_T	*tp;
#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం		i;
#అయితే_ముగింపు


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    ఒకవేళ (tp == ఖాళీ)
	తిరిగి ఖాళీ;

#నిర్వచితమైతే FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    ఒకవేళ (tp->tp_vars == ఖాళీ)
    {
	vim_free(tp);
	తిరిగి ఖాళీ;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    కొరకు (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#అయితే_ముగింపు
    tp->tp_ch_వాడిన = p_ch;

    తిరిగి tp;
}

    శూన్య
free_tabpage(tabpage_T *tp)
{
    పూర్ణాంకం idx;

#నిర్వచితమైతే FEAT_DIFF
    diff_clear(tp);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    అయితే (tp->tp_మొదటి_popupwin != ఖాళీ)
	popup_close_tabpage(tp, tp->tp_మొదటి_popupwin->w_id, TRUE);
#అయితే_ముగింపు
    కొరకు (idx = 0; idx < SNAP_లెక్క; ++idx)
	clear_snapshot(tp, idx);
#నిర్వచితమైతే FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#అయితే_ముగింపు

    ఒకవేళ (tp == lastused_tabpage)
	lastused_tabpage = ఖాళీ;
#నిర్వచితమైతే FEAT_TABPANEL
    tabpanel_forget_tabpage(tp);
#అయితే_ముగింపు

    vim_free(tp->tp_localdir);
    vim_free(tp->tp_prevdir);

#నిర్వచితమైతే FEAT_PYTHON
    python_tabpage_free(tp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON3
    python3_tabpage_free(tp);
#అయితే_ముగింపు

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
    పూర్ణాంకం
win_new_tabpage(పూర్ణాంకం after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    పూర్ణాంకం		n;
#ఒకవేళ defined(FEAT_TABPANEL)
    పూర్ణాంకం prev_columns = COLUMNS_WITHOUT_TPL();
#అయితే_ముగింపు

    ఒకవేళ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_లోపల_cmdline_కిటికీ));
	తిరిగి FAIL;
    }
    ఒకవేళ (కిటికీ_layout_locked(CMD_tabnew))
	తిరిగి FAIL;

    newtp = alloc_tabpage();
    ఒకవేళ (newtp == ఖాళీ)
	తిరిగి FAIL;

    // Remember the current windows in this Tab page.
    ఒకవేళ (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	తిరిగి FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == ఖాళీ)
				    ? ఖాళీ : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    ఒకవేళ (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	ఒకవేళ (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_తదుపరి = మొదటి_tabpage;
	    మొదటి_tabpage = newtp;
	}
	లేకపోతే
	{
	    ఒకవేళ (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		కొరకు (tp = మొదటి_tabpage; tp->tp_తదుపరి != ఖాళీ
					       && n < after; tp = tp->tp_తదుపరి)
		    ++n;
	    }
	    newtp->tp_తదుపరి = tp->tp_తదుపరి;
	    tp->tp_తదుపరి = newtp;
	}
	newtp->tp_firstwin = newtp->tp_lastwin = newtp->tp_curwin = curwin;

	win_init_కొలత();
	firstwin->w_winrow = tabline_ఎత్తు();
	firstwin->w_prev_winrow = firstwin->w_winrow;
	win_comp_scroll(curwin);

	newtp->tp_topframe = topframe;
	చివరి_స్థితి(FALSE);

	lastused_tabpage = prev_tp;

#ఒకవేళ defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
	entering_కిటికీ(curwin);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TABPANEL)
	ఒకవేళ (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    పూర్ణాంకం		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		ఒకవేళ (tp2 != curtab)
		{
		    చట్రం_new_వెడల్పు(tp2->tp_topframe, w, FALSE, TRUE);
		    ఒకవేళ (!చట్రం_check_వెడల్పు(tp2->tp_topframe, w))
			చట్రం_new_వెడల్పు(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#అయితే_ముగింపు
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, ఖాళీ, ఖాళీ, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, ఖాళీ, ఖాళీ, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
	తిరిగి OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    తిరిగి FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
may_open_tabpage(శూన్య)
{
    పూర్ణాంకం		n = (cmdmod.cmod_tab == 0)
				       ? postponed_విభజించు_tab : cmdmod.cmod_tab;

    ఒకవేళ (n == 0)
	తిరిగి FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_విభజించు_tab = 0;
    తిరిగి win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    పూర్ణాంకం
make_tabpages(పూర్ణాంకం maxcount)
{
    పూర్ణాంకం		లెక్క = maxcount;
    పూర్ణాంకం		todo;

    // Limit to 'tabpagemax' tabs.
    ఒకవేళ (లెక్క > p_tpm)
	లెక్క = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    ఖండం_autocmds();

    కొరకు (todo = లెక్క - 1; todo > 0; --todo)
	ఒకవేళ (win_new_tabpage(0) == FAIL)
	    ఆపు;

    unblock_autocmds();

    // return actual number of tab pages
    తిరిగి (లెక్క - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    పూర్ణాంకం
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	ఒకవేళ (tp == tpc)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    పూర్ణాంకం
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	ఒకవేళ (tp == tpc)
	{
	    FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
	    {
		ఒకవేళ (win_valid_any_tab(wp))
		    తిరిగి TRUE;
	    }
	    తిరిగి FALSE;
	}
    }
    // shouldn't happen
    తిరిగి FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    శూన్య
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    ఒకవేళ (tab == మొదటి_tabpage)
    {
	మొదటి_tabpage = tab->tp_తదుపరి;
	ptp = మొదటి_tabpage;
    }
    లేకపోతే
    {
	కొరకు (ptp = మొదటి_tabpage; ptp != ఖాళీ && ptp->tp_తదుపరి != tab;
							    ptp = ptp->tp_తదుపరి)
	    ;
	ప్రతిజ్ఞా(ptp != ఖాళీ);
	ptp->tp_తదుపరి = tab->tp_తదుపరి;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
కనుగొను_tabpage(పూర్ణాంకం n)
{
    tabpage_T	*tp;
    పూర్ణాంకం		i = 1;

    ఒకవేళ (n == 0)
	తిరిగి curtab;

    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ && i != n; tp = tp->tp_తదుపరి)
	++i;
    తిరిగి tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    పూర్ణాంకం
tabpage_సూచీ(tabpage_T *ftp)
{
    పూర్ణాంకం		i = 1;
    tabpage_T	*tp;

    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ && tp != ftp; tp = tp->tp_తదుపరి)
	++i;
    తిరిగి i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    స్థిర పూర్ణాంకం
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    పూర్ణాంకం		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#నిర్వచితమైతే FEAT_పని_CHANNEL
    leaving_కిటికీ(curwin);
#అయితే_ముగింపు
    reset_VIsual_and_resel();	// stop Visual mode
    ఒకవేళ (trigger_leave_autocmds)
    {
	ఒకవేళ (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	    ఒకవేళ (curtab != tp)
		తిరిగి FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (curtab != tp)
	    తిరిగి FAIL;
	apply_autocmds(EVENT_TABLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (curtab != tp)
	    తిరిగి FAIL;
    }

    reset_dragwin();
#ఒకవేళ defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    ఒకవేళ (gui.లోపల_use)
	gui_remove_scrollbars();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
    // Clear all image placements, they will be redrawn again if needed.
    clear_all_image_placements();
#అయితే_ముగింపు
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_అడ్డువరుసలు = అడ్డువరుసలు_AVAIL;
    ఒకవేళ (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_వెడల్పు;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = ఖాళీ;
    lastwin = ఖాళీ;
    తిరిగి OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    స్థిర శూన్య
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    పూర్ణాంకం		trigger_enter_autocmds,
    పూర్ణాంకం		trigger_leave_autocmds)
{
    పూర్ణాంకం		old_off = tp->tp_firstwin->w_winrow;
    win_T	*తదుపరి_prevwin = tp->tp_prevwin;
    tabpage_T	*చివరి_tab = curtab;

    use_tabpage(tp);

    ఒకవేళ (p_ch != curtab->tp_ch_వాడిన)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	పూర్ణాంకం new_ch = curtab->tp_ch_వాడిన;
	curtab->tp_ch_వాడిన = p_ch;
	command_చట్రం_ఎత్తు = FALSE;
	సమితి_option_విలువ((char_u *)"cmdheight", new_ch, ఖాళీ, 0);
	command_చట్రం_ఎత్తు = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (శూన్య)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
		  | (trigger_enter_autocmds ? WEE_TRIGGER_ENTER_AUTOCMDS : 0)
		  | (trigger_leave_autocmds ? WEE_TRIGGER_LEAVE_AUTOCMDS : 0));
    prevwin = తదుపరి_prevwin;

    చివరి_స్థితి(FALSE);		// status line may appear or disappear
    win_comp_pos();		// recompute w_winrow for all windows
#నిర్వచితమైతే FEAT_DIFF
    diff_need_scrollbind = TRUE;
#అయితే_ముగింపు

    // If there was a click in a window, it won't be usable for a following
    // drag.
    reset_dragwin();

    // The tabpage line may have appeared or disappeared, may need to resize
    // the frames for that.  When the Vim window was resized need to update
    // frame sizes too.
    ఒకవేళ (curtab->tp_old_అడ్డువరుసలు != అడ్డువరుసలు_AVAIL || (old_off != firstwin->w_winrow
#నిర్వచితమైతే FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#అయితే_ముగింపు
		))
	shell_new_అడ్డువరుసలు();
    ఒకవేళ (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	ఒకవేళ (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_వెడల్పు;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	లేకపోతే
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = చివరి_tab;

#ఒకవేళ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#అయితే_ముగింపు

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    ఒకవేళ (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    శూన్య
goto_tabpage(పూర్ణాంకం n)
{
    tabpage_T	*tp = ఖాళీ;  // shut up compiler
    tabpage_T	*ttp;
    పూర్ణాంకం		i;

    ఒకవేళ (పాఠ్యం_locked())
    {
	// Not allowed when editing the command line.
	పాఠ్యం_locked_సందేశం();
	తిరిగి;
    }

    // If there is only one it can't work.
    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి == ఖాళీ)
    {
	ఒకవేళ (n > 1)
	    beep_flush();
	తిరిగి;
    }

    ఒకవేళ (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	ఒకవేళ (curtab->tp_తదుపరి == ఖాళీ)
	    tp = మొదటి_tabpage;
	లేకపోతే
	    tp = curtab->tp_తదుపరి;
    }
    లేకపోతే ఒకవేళ (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	కొరకు (i = n; i < 0; ++i)
	{
	    కొరకు (tp = మొదటి_tabpage; tp->tp_తదుపరి != ttp && tp->tp_తదుపరి != ఖాళీ;
		    tp = tp->tp_తదుపరి)
		;
	    ttp = tp;
	}
    }
    లేకపోతే ఒకవేళ (n == 9999)
    {
	// Go to last tab page.
	కొరకు (tp = మొదటి_tabpage; tp->tp_తదుపరి != ఖాళీ; tp = tp->tp_తదుపరి)
	    ;
    }
    లేకపోతే
    {
	// Go to tab page "n".
	tp = కనుగొను_tabpage(n);
	ఒకవేళ (tp == ఖాళీ)
	{
	    beep_flush();
	    తిరిగి;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#నిర్వచితమైతే FEAT_GUI_TABLINE
    ఒకవేళ (gui_use_tabline())
	gui_mch_సమితి_curtab(tabpage_సూచీ(curtab));
#అయితే_ముగింపు
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    శూన్య
goto_tabpage_tp(
    tabpage_T	*tp,
    పూర్ణాంకం		trigger_enter_autocmds,
    పూర్ణాంకం		trigger_leave_autocmds)
{
    ఒకవేళ (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    సమితి_keep_సందేశం(ఖాళీ, 0);

    skip_win_fix_scroll = TRUE;
    ఒకవేళ (tp != curtab && leave_tabpage(tp->tp_curwin->w_బఫరు,
					trigger_leave_autocmds) == OK)
    {
	ఒకవేళ (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	లేకపోతే
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    పూర్ణాంకం
goto_tabpage_lastused(శూన్య)
{
    ఒకవేళ (!valid_tabpage(lastused_tabpage))
	తిరిగి FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    తిరిగి OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    శూన్య
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    ఒకవేళ (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#నిర్వచితమైతే FEAT_GUI_TABLINE
	ఒకవేళ (gui_use_tabline())
	    gui_mch_సమితి_curtab(tabpage_సూచీ(curtab));
#అయితే_ముగింపు
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    శూన్య
tabpage_move(పూర్ణాంకం nr)
{
    పూర్ణాంకం		n = 1;
    tabpage_T	*tp, *tp_dst;

    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి == ఖాళీ)
	తిరిగి;

    ఒకవేళ (tabpage_move_disallowed)
	తిరిగి;

    కొరకు (tp = మొదటి_tabpage; tp->tp_తదుపరి != ఖాళీ && n < nr; tp = tp->tp_తదుపరి)
	++n;

    ఒకవేళ (tp == curtab || (nr > 0 && tp->tp_తదుపరి != ఖాళీ
						    && tp->tp_తదుపరి == curtab))
	తిరిగి;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    ఒకవేళ (curtab == మొదటి_tabpage)
	మొదటి_tabpage = curtab->tp_తదుపరి;
    లేకపోతే
    {
	FOR_ALL_TABPAGES(tp)
	    ఒకవేళ (tp->tp_తదుపరి == curtab)
		ఆపు;
	ఒకవేళ (tp == ఖాళీ)	// "cannot happen"
	    తిరిగి;
	tp->tp_తదుపరి = curtab->tp_తదుపరి;
    }

    // Re-insert it at the specified position.
    ఒకవేళ (nr <= 0)
    {
	curtab->tp_తదుపరి = మొదటి_tabpage;
	మొదటి_tabpage = curtab;
    }
    లేకపోతే
    {
	curtab->tp_తదుపరి = tp_dst->tp_తదుపరి;
	tp_dst->tp_తదుపరి = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
}


/*
 * Go to another window.
 * When jumping to another buffer, stop Visual mode.  Do this before
 * changing windows so we can yank the selection into the '*' register.
 * (note: this may trigger ModeChanged autocommand!)
 * When jumping to another window on the same buffer, adjust its cursor
 * position to keep the same Visual area.
 */
    శూన్య
win_goto(win_T *wp)
{
#నిర్వచితమైతే FEAT_CONCEAL
    win_T	*owp = curwin;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (ERROR_IF_ANY_POPUP_కిటికీ)
	తిరిగి;
    ఒకవేళ (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_గమ్యం_enter_popup_కిటికీ));
	తిరిగి;
    }
#అయితే_ముగింపు
    ఒకవేళ (పాఠ్యం_or_buf_locked())
    {
	beep_flush();
	తిరిగి;
    }

    ఒకవేళ (wp->w_బఫరు != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    లేకపోతే ఒకవేళ (VIsual_చురుకైన)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    ఒకవేళ (!win_valid(wp))
	తిరిగి;

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు
    win_enter(wp, TRUE);

#నిర్వచితమైతే FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    ఒకవేళ (win_valid(owp) && owp->w_p_cole > 0 && !సందేశం_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    ఒకవేళ (curwin->w_p_cole > 0 && !సందేశం_scrolled)
	need_cursor_పంక్తి_redraw = TRUE;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_కనుగొను_nr(పూర్ణాంకం winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (--winnr == 0)
	    ఆపు;
    తిరిగి wp;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_కనుగొను_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ఒకవేళ (wp == win)
		తిరిగి tp;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, పూర్ణాంకం పైకి, దీర్ఘ లెక్క)
{
    చట్రం_T	*fr;
    చట్రం_T	*nfr;
    చట్రం_T	*foundfr;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (popup_is_popup(wp))
	// popups don't have neighbors.
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
    foundfr = wp->w_చట్రం;
    అయితే (లెక్క--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	కొరకు (;;)
	{
	    ఒకవేళ (fr == tp->tp_topframe)
		వెళ్ళు చివర;
	    ఒకవేళ (పైకి)
		nfr = fr->fr_prev;
	    లేకపోతే
		nfr = fr->fr_తదుపరి;
	    ఒకవేళ (fr->fr_మాతృ->fr_layout == FR_నిలువు && nfr != ఖాళీ)
		ఆపు;
	    fr = fr->fr_మాతృ;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	కొరకు (;;)
	{
	    ఒకవేళ (nfr->fr_layout == FR_ఆకు)
	    {
		foundfr = nfr;
		ఆపు;
	    }
	    fr = nfr->fr_బిడ్డ;
	    ఒకవేళ (nfr->fr_layout == FR_అడ్డువరుస)
	    {
		// Find the frame at the cursor row.
		అయితే (fr->fr_తదుపరి != ఖాళీ
			&& frame2win(fr)->w_wincol + fr->fr_వెడల్పు
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_తదుపరి;
	    }
	    ఒకవేళ (nfr->fr_layout == FR_నిలువు && పైకి)
		అయితే (fr->fr_తదుపరి != ఖాళీ)
		    fr = fr->fr_తదుపరి;
	    nfr = fr;
	}
    }
చివర:
    తిరిగి foundfr != ఖాళీ ? foundfr->fr_win : ఖాళీ;
}

/*
 * Move to window above or below "count" times.
 */
    స్థిర శూన్య
win_goto_ver(
    పూర్ణాంకం		పైకి,		// TRUE to go to win above
    దీర్ఘ	లెక్క)
{
    win_T	*win;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (ERROR_IF_పదం_POPUP_కిటికీ)
	తిరిగి;
#అయితే_ముగింపు
    win = win_vert_neighbor(curtab, curwin, పైకి, లెక్క);
    ఒకవేళ (win != ఖాళీ)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, పూర్ణాంకం ఎడమ_2, దీర్ఘ లెక్క)
{
    చట్రం_T	*fr;
    చట్రం_T	*nfr;
    చట్రం_T	*foundfr;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (popup_is_popup(wp))
	// popups don't have neighbors.
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
    foundfr = wp->w_చట్రం;
    అయితే (లెక్క--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	కొరకు (;;)
	{
	    ఒకవేళ (fr == tp->tp_topframe)
		వెళ్ళు చివర;
	    ఒకవేళ (ఎడమ_2)
		nfr = fr->fr_prev;
	    లేకపోతే
		nfr = fr->fr_తదుపరి;
	    ఒకవేళ (fr->fr_మాతృ->fr_layout == FR_అడ్డువరుస && nfr != ఖాళీ)
		ఆపు;
	    fr = fr->fr_మాతృ;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	కొరకు (;;)
	{
	    ఒకవేళ (nfr->fr_layout == FR_ఆకు)
	    {
		foundfr = nfr;
		ఆపు;
	    }
	    fr = nfr->fr_బిడ్డ;
	    ఒకవేళ (nfr->fr_layout == FR_నిలువు)
	    {
		// Find the frame at the cursor row.
		అయితే (fr->fr_తదుపరి != ఖాళీ
			&& frame2win(fr)->w_winrow + fr->fr_ఎత్తు
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_తదుపరి;
	    }
	    ఒకవేళ (nfr->fr_layout == FR_అడ్డువరుస && ఎడమ_2)
		అయితే (fr->fr_తదుపరి != ఖాళీ)
		    fr = fr->fr_తదుపరి;
	    nfr = fr;
	}
    }
చివర:
    తిరిగి foundfr != ఖాళీ ? foundfr->fr_win : ఖాళీ;
}

/*
 * Move to left or right window.
 */
    స్థిర శూన్య
win_goto_hor(
    పూర్ణాంకం		ఎడమ_2,		// TRUE to go to left win
    దీర్ఘ	లెక్క)
{
    win_T	*win;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (ERROR_IF_పదం_POPUP_కిటికీ)
	తిరిగి;
#అయితే_ముగింపు
    win = win_horz_neighbor(curtab, curwin, ఎడమ_2, లెక్క);
    ఒకవేళ (win != ఖాళీ)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    శూన్య
win_enter(win_T *wp, పూర్ణాంకం undo_sync)
{
    (శూన్య)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    శూన్య
win_fix_current_dir(శూన్య)
{
    ఒకవేళ (curwin->w_localdir != ఖాళీ || curtab->tp_localdir != ఖాళీ)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	ఒకవేళ (globaldir == ఖాళీ)
	{
	    char_u	cwd[MAXPATHL];

	    ఒకవేళ (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	ఒకవేళ (curwin->w_localdir != ఖాళీ)
	    dirname = curwin->w_localdir;
	లేకపోతే
	    dirname = curtab->tp_localdir;

	ఒకవేళ (mch_chdir((అక్షరం *)dirname) == 0)
	{
	    చివరి_chdir_reason = ఖాళీ;
	    shorten_fnames(TRUE);
	}
    }
    లేకపోతే ఒకవేళ (globaldir != ఖాళీ)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((అక్షరం *)globaldir);
	VIM_CLEAR(globaldir);
	చివరి_chdir_reason = ఖాళీ;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    స్థిర పూర్ణాంకం
win_enter_ext(win_T *wp, పూర్ణాంకం flags)
{
    పూర్ణాంకం		other_బఫరు = FALSE;
    పూర్ణాంకం		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    పూర్ణాంకం		did_decrement = FALSE;

    ఒకవేళ (wp == curwin && curwin_invalid == 0)	// nothing to do
	తిరిగి FALSE;

#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (curwin_invalid == 0)
	leaving_కిటికీ(curwin);
#అయితే_ముగింపు

    ఒకవేళ (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	ఒకవేళ (wp->w_బఫరు != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	    other_బఫరు = TRUE;
	    ఒకవేళ (!win_valid(wp))
		తిరిగి FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (!win_valid(wp))
	    తిరిగి FALSE;
#నిర్వచితమైతే FEAT_EVAL
	// autocmds may abort script processing
	ఒకవేళ (aborting())
	    తిరిగి FALSE;
#అయితే_ముగింపు
    }

    // sync undo before leaving the current buffer
    ఒకవేళ ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_బఫరు)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    ఒకవేళ (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    ఒకవేళ (wp->w_బఫరు != curbuf)
	buf_copy_options(wp->w_బఫరు, BCO_ENTER | BCO_NOHELP);
    ఒకవేళ (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_స్థితి = true;
    }
    curwin = wp;
    curbuf = wp->w_బఫరు;
    check_cursor();
    ఒకవేళ (!virtual_చురుకైన())
	curwin->w_cursor.coladd = 0;
    ఒకవేళ (*p_spk == 'c')		// assume cursor position needs updating
	మారింది_పంక్తి_abv_curs();
    లేకపోతే
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (బహుళకం_సాధారణ|బహుళకం_CMDLINE|బహుళకం_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#నిర్వచితమైతే సందేశం_వరుస
    ఒకవేళ (flags & WEE_ALLOW_PARSE_MESSAGES)
    {
	--dont_parse_messages;
	did_decrement = TRUE;
    }
#అయితే_ముగింపు

    win_fix_current_dir();

#నిర్వచితమైతే FEAT_పని_CHANNEL
    entering_కిటికీ(curwin);
#అయితే_ముగింపు
    // Careful: autocommands may close the window and make "wp" invalid
    ఒకవేళ (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, ఖాళీ, ఖాళీ, FALSE, curbuf);
    ఒకవేళ (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
	ఒకవేళ (other_బఫరు)
	    apply_autocmds(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_స్థితి = true;
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (bt_terminal(curwin->w_బఫరు))
	// terminal is likely in another mode
	redraw_బహుళకం = TRUE;
#అయితే_ముగింపు
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    ఒకవేళ (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    ఒకవేళ (curwin->w_ఎత్తు < p_wh && !curwin->w_p_wfh
#నిర్వచితమైతే FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#అయితే_ముగింపు
	    )
	win_setheight((పూర్ణాంకం)p_wh);
    లేకపోతే ఒకవేళ (curwin->w_ఎత్తు == 0)
	win_setheight(1);

    // set window width to desired minimal value
    ఒకవేళ (curwin->w_వెడల్పు < p_wiw && !curwin->w_p_wfw)
	win_setwidth((పూర్ణాంకం)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    తిరిగి did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = ఖాళీ;

    ఒకవేళ (curwin->w_బఫరు == buf)
	wp = curwin;
    లేకపోతే
	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_బఫరు == buf)
		ఆపు;
    ఒకవేళ (wp != ఖాళీ)
	win_enter(wp, FALSE);
    తిరిగి wp;
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

    ఒకవేళ (wp != ఖాళీ)
	తిరిగి wp;

    FOR_ALL_TABPAGES(tp)
	ఒకవేళ (tp != curtab)
	{
	    FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
		ఒకవేళ (wp->w_బఫరు == buf)
		    ఆపు;
	    ఒకవేళ (wp != ఖాళీ)
	    {
		goto_tabpage_win(tp, wp);
		ఒకవేళ (curwin != wp)
		    wp = ఖాళీ;	// something went wrong
		ఆపు;
	    }
	}
    తిరిగి wp;
}

స్థిర పూర్ణాంకం చివరి_win_id = అత్యల్ప_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    స్థిర win_T *
win_alloc(win_T *after, పూర్ణాంకం hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    ఒకవేళ (new_wp == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (win_alloc_పంక్తులు(new_wp) == FAIL)
    {
	vim_free(new_wp);
	తిరిగి ఖాళీ;
    }

    new_wp->w_id = ++చివరి_win_id;

#నిర్వచితమైతే FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    ఒకవేళ (new_wp->w_vars == ఖాళీ)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	తిరిగి ఖాళీ;
    }
    init_var_dict(new_wp->w_vars, &new_wp->w_winvar, VAR_SCOPE);
#అయితే_ముగింపు

    // Don't execute autocommands while the window is not properly
    // initialized yet.  gui_create_scrollbar() may trigger a FocusGained
    // event.
    ఖండం_autocmds();

    /*
     * link the window in the window list
     */
    ఒకవేళ (!hidden)
	win_append(after, new_wp);
    new_wp->w_wincol = TPL_LCOL();
    new_wp->w_వెడల్పు = COLUMNS_WITHOUT_TPL();

    // position the display and the cursor at the top of the file.
    new_wp->w_topline = 1;
#నిర్వచితమైతే FEAT_DIFF
    new_wp->w_topfill = 0;
#అయితే_ముగింపు
    new_wp->w_botline = 2;
    new_wp->w_cursor.lnum = 1;
    new_wp->w_scbind_pos = 1;

    // use global option value for global-local options
    new_wp->w_allbuf_opt.wo_so = new_wp->w_p_so = -1;
    new_wp->w_allbuf_opt.wo_sop = new_wp->w_p_sop = -1;
    new_wp->w_allbuf_opt.wo_siso = new_wp->w_p_siso = -1;

    // We won't calculate w_fraction until resizing the window
    new_wp->w_fraction = 0;
    new_wp->w_prev_fraction_అడ్డువరుస = -1;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_ఎడమ],
		SBAR_ఎడమ, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_కుడి],
		SBAR_కుడి, new_wp);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    foldInitWin(new_wp);
#అయితే_ముగింపు
    unblock_autocmds();
#నిర్వచితమైతే FEAT_శోధన_EXTRA
    new_wp->w_తదుపరి_సరిపోలిక_id = 1000;  // up to 1000 can be picked by the user
#అయితే_ముగింపు
    తిరిగి new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    స్థిర శూన్య
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    పూర్ణాంకం		i;
    buf_T	*buf;
    wininfo_T	*wip;

#నిర్వచితమైతే FEAT_FOLDING
    clearFolding(wp);
#అయితే_ముగింపు

    // reduce the reference count to the argument list.
    alist_unlink(wp->w_alist);

    // Don't execute autocommands while the window is halfway being deleted.
    // gui_mch_destroy_scrollbar() may trigger a FocusGained event.
    ఖండం_autocmds();

#నిర్వచితమైతే FEAT_LUA
    lua_కిటికీ_free(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MZSCHEME
    mzscheme_కిటికీ_free(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PERL
    perl_win_free(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON
    python_కిటికీ_free(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON3
    python3_కిటికీ_free(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TCL
    tcl_కిటికీ_free(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RUBY
    ruby_కిటికీ_free(wp);
#అయితే_ముగింపు

    remove_highlight_overrides(wp->w_hl);
    vim_free(wp->w_hl);

    // Free statusline click regions.
    ఒకవేళ (wp->w_stl_click != ఖాళీ)
    {
	కొరకు (i = 0; i < wp->w_stl_click_లెక్క; i++)
	    vim_free(wp->w_stl_click[i].funcname);
	vim_free(wp->w_stl_click);
    }

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

    vim_free(wp->w_lcs_chars.multispace);
    vim_free(wp->w_lcs_chars.leadmultispace);

#నిర్వచితమైతే FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	// free all w: variables
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#అయితే_ముగింపు

    {
	tabpage_T	*ttp;

	ఒకవేళ (prevwin == wp)
	    prevwin = ఖాళీ;
	FOR_ALL_TABPAGES(ttp)
	    ఒకవేళ (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = ఖాళీ;
    }
    win_free_lsize(wp);

    కొరకు (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_నమోదు(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    ఒకవేళ (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    ఒకవేళ (wip2 != wip && wip2->wi_win == ఖాళీ)
		    {
			ఒకవేళ (wip2->wi_తదుపరి != ఖాళీ)
			    wip2->wi_తదుపరి->wi_prev = wip2->wi_prev;
			ఒకవేళ (wip2->wi_prev == ఖాళీ)
			    buf->b_wininfo = wip2->wi_తదుపరి;
			లేకపోతే
			    wip2->wi_prev->wi_తదుపరి = wip2->wi_తదుపరి;
			free_wininfo(wip2);
			ఆపు;
		    }

		wip->wi_win = ఖాళీ;
	    }

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    clear_matches(wp);
#అయితే_ముగింపు

    free_jumplist(wp);

#నిర్వచితమైతే FEAT_QUICKFIX
    qf_free_all(wp);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_ఎడమ]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_కుడి]);
    }
#అయితే_ముగింపు // FEAT_GUI

#నిర్వచితమైతే FEAT_MENU
    remove_winbar(wp);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    కొరకు (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_ముసుగు);
    vim_free(wp->w_popup_ముసుగు_cells);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
    vim_free(wp->w_p_cc_నిలువుల_సంఖ్య);
#అయితే_ముగింపు

    ఒకవేళ (win_valid_any_tab(wp))
	win_remove(wp, tp);
    ఒకవేళ (autocmd_busy)
    {
	wp->w_తదుపరి = au_pending_free_win;
	au_pending_free_win = wp;
    }
    లేకపోతే
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    పూర్ణాంకం
win_unlisted(win_T *wp)
{
    తిరిగి is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    శూన్య
win_free_popup(win_T *win)
{
    ఒకవేళ (win->w_బఫరు != ఖాళీ)
    {
	ఒకవేళ (bt_popup(win->w_బఫరు))
	    win_close_బఫరు(win, DOBUF_WIPE_REUSE, FALSE);
	లేకపోతే
	    close_బఫరు(win, win->w_బఫరు, 0, FALSE, FALSE, TRUE);

	ఒకవేళ (win->w_బఫరు != ఖాళీ)
	    --win->w_బఫరు->b_nwindows;
    }
# ఒకవేళ defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    ఒకవేళ (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_చట్రం);
    win_free(win, ఖాళీ);
}
#అయితే_ముగింపు

/*
 * Append window "wp" in the window list after window "after".
 */
    స్థిర శూన్య
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    ఒకవేళ (after == ఖాళీ)	    // after NULL is in front of the first
	before = firstwin;
    లేకపోతే
	before = after->w_తదుపరి;

    wp->w_తదుపరి = before;
    wp->w_prev = after;
    ఒకవేళ (after == ఖాళీ)
	firstwin = wp;
    లేకపోతే
	after->w_తదుపరి = wp;
    ఒకవేళ (before == ఖాళీ)
	lastwin = wp;
    లేకపోతే
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    శూన్య
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ఒకవేళ (wp->w_prev != ఖాళీ)
	wp->w_prev->w_తదుపరి = wp->w_తదుపరి;
    లేకపోతే ఒకవేళ (tp == ఖాళీ)
	firstwin = curtab->tp_firstwin = wp->w_తదుపరి;
    లేకపోతే
	tp->tp_firstwin = wp->w_తదుపరి;

    ఒకవేళ (wp->w_తదుపరి != ఖాళీ)
	wp->w_తదుపరి->w_prev = wp->w_prev;
    లేకపోతే ఒకవేళ (tp == ఖాళీ)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    లేకపోతే
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    స్థిర శూన్య
చట్రం_append(చట్రం_T *after, చట్రం_T *frp)
{
    frp->fr_తదుపరి = after->fr_తదుపరి;
    after->fr_తదుపరి = frp;
    ఒకవేళ (frp->fr_తదుపరి != ఖాళీ)
	frp->fr_తదుపరి->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    స్థిర శూన్య
చట్రం_చొప్పించు(చట్రం_T *before, చట్రం_T *frp)
{
    frp->fr_తదుపరి = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    ఒకవేళ (frp->fr_prev != ఖాళీ)
	frp->fr_prev->fr_తదుపరి = frp;
    లేకపోతే
	frp->fr_మాతృ->fr_బిడ్డ = frp;
}

/*
 * Remove a frame from a frame list.
 */
    స్థిర శూన్య
చట్రం_remove(చట్రం_T *frp)
{
    ఒకవేళ (frp->fr_prev != ఖాళీ)
	frp->fr_prev->fr_తదుపరి = frp->fr_తదుపరి;
    లేకపోతే
	frp->fr_మాతృ->fr_బిడ్డ = frp->fr_తదుపరి;
    ఒకవేళ (frp->fr_తదుపరి != ఖాళీ)
	frp->fr_తదుపరి->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    పూర్ణాంకం
win_alloc_పంక్తులు(win_T *wp)
{
    wp->w_పంక్తులు_valid = 0;
    wp->w_పంక్తులు = ALLOC_CLEAR_MULT(wline_T, అడ్డువరుసలు);
    ఒకవేళ (wp->w_పంక్తులు == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * free lsize arrays for a window
 */
    శూన్య
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    ఒకవేళ (wp != ఖాళీ)
	VIM_CLEAR(wp->w_పంక్తులు);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    శూన్య
shell_new_అడ్డువరుసలు(శూన్య)
{
    పూర్ణాంకం		h = (పూర్ణాంకం)అడ్డువరుసలు_AVAIL;

    ఒకవేళ (firstwin == ఖాళీ)	// not initialized yet
	తిరిగి;
    ఒకవేళ (h < చట్రం_minheight(topframe, ఖాళీ))
	h = చట్రం_minheight(topframe, ఖాళీ);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    చట్రం_new_ఎత్తు(topframe, h, FALSE, TRUE, FALSE);
    ఒకవేళ (!చట్రం_check_ఎత్తు(topframe, h))
	చట్రం_new_ఎత్తు(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_వాడిన = p_ch;

    ఒకవేళ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
#ఒకవేళ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ఒకవేళ (p_ea)
	win_equal(curwin, FALSE, 'v');
#అయితే_ముగింపు
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    శూన్య
shell_new_columns(శూన్య)
{
    ఒకవేళ (firstwin == ఖాళీ)	// not initialized yet
	తిరిగి;

#ఒకవేళ defined(FEAT_TABPANEL)
    స్థిర పూర్ణాంకం prev_tp_వెడల్పు;
    స్థిర పూర్ణాంకం prev_wincol;
    స్థిర పూర్ణాంకం prev_fr_వెడల్పు;
#అయితే_ముగింపు
    పూర్ణాంకం w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    చట్రం_new_వెడల్పు(topframe, w, FALSE, TRUE);
    ఒకవేళ (!చట్రం_check_వెడల్పు(topframe, w))
	చట్రం_new_వెడల్పు(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#ఒకవేళ defined(FEAT_TABPANEL)
    పూర్ణాంకం tp_వెడల్పు = tabpanel_వెడల్పు();
    పూర్ణాంకం tp_వెడల్పు_మారింది = tp_వెడల్పు != prev_tp_వెడల్పు;
    // tabpanel width changed
    ఒకవేళ (tp_వెడల్పు_మారింది && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    ఒకవేళ (tp_వెడల్పు_మారింది
	    || (tp_వెడల్పు > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_వెడల్పు != prev_fr_వెడల్పు)))
    {
	screen_fill(cmdline_అడ్డువరుస, (పూర్ణాంకం)అడ్డువరుసలు, 0, (పూర్ణాంకం)Columns, ' ', ' ', 0);
    }
    prev_tp_వెడల్పు = tabpanel_వెడల్పు();
    prev_wincol = firstwin->w_wincol;
    prev_fr_వెడల్పు = topframe->fr_వెడల్పు;
#అయితే_ముగింపు
    // Adjust offset for command line start column
    cmdline_నిలువు_off = firstwin->w_wincol;
    cmdline_వెడల్పు = topframe->fr_వెడల్పు;
    comp_నిలువు();
    ఒకవేళ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#ఒకవేళ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#అయితే_ముగింపు
#ఒకవేళ 0
    // Disabled: don't want making the screen smaller make a window larger.
    ఒకవేళ (p_ea)
	win_equal(curwin, FALSE, 'h');
#అయితే_ముగింపు
}

/*
 * Save the size of all windows in "gap".
 */
    శూన్య
win_కొలత_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, పరిమాణం(పూర్ణాంకం), 1);
    ఒకవేళ (ga_grow(gap, win_లెక్క() * 2 + 1) == FAIL)
	తిరిగి;

    // first entry is the total lines available for windows
    ((పూర్ణాంకం *)gap->ga_దత్తాంశం)[gap->ga_len++] = అడ్డువరుసలు_AVAIL - చివరి_stl_ఎత్తు(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((పూర్ణాంకం *)gap->ga_దత్తాంశం)[gap->ga_len++] =
	    wp->w_వెడల్పు + wp->w_vsep_వెడల్పు;
	((పూర్ణాంకం *)gap->ga_దత్తాంశం)[gap->ga_len++] = wp->w_ఎత్తు;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    శూన్య
win_కొలత_restore(garray_T *gap)
{
    win_T	*wp;
    పూర్ణాంకం		i, j;

    ఒకవేళ (win_లెక్క() * 2 + 1 == gap->ga_len
	    && ((పూర్ణాంకం *)gap->ga_దత్తాంశం)[0] == అడ్డువరుసలు_AVAIL - చివరి_stl_ఎత్తు(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	కొరకు (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		చట్రం_setwidth(wp->w_చట్రం, ((పూర్ణాంకం *)gap->ga_దత్తాంశం)[i++]);
		win_setheight_win(((పూర్ణాంకం *)gap->ga_దత్తాంశం)[i++], wp);
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
    శూన్య
win_comp_pos(శూన్య)
{
    పూర్ణాంకం		అడ్డువరుస = tabline_ఎత్తు();
    పూర్ణాంకం		నిలువు = TPL_LCOL();

    చట్రం_comp_pos(topframe, &అడ్డువరుస, &నిలువు);
#ఒకవేళ defined(FEAT_STL_OPT)
    చట్రం_change_statusline_ఎత్తు();
#అయితే_ముగింపు
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    స్థిర శూన్య
చట్రం_comp_pos(చట్రం_T *topfrp, పూర్ణాంకం *అడ్డువరుస, పూర్ణాంకం *నిలువు)
{
    win_T	*wp;
    చట్రం_T	*frp;
    పూర్ణాంకం		startcol;
    పూర్ణాంకం		startrow;
    పూర్ణాంకం		h;

    wp = topfrp->fr_win;
    ఒకవేళ (wp != ఖాళీ)
    {
	ఒకవేళ (wp->w_winrow != *అడ్డువరుస || wp->w_wincol != *నిలువు)
	{
	    // position changed, redraw
	    wp->w_winrow = *అడ్డువరుస;
	    wp->w_wincol = *నిలువు;
	    redraw_win_later(wp, UPD_NOT_VALID);
	    wp->w_redr_స్థితి = true;
	}
	// WinBar will not show if the window height is zero
	h = VISIBLE_ఎత్తు(wp) + wp->w_స్థితి_ఎత్తు;
	*అడ్డువరుస += h > topfrp->fr_ఎత్తు ? topfrp->fr_ఎత్తు : h;
	*నిలువు += wp->w_వెడల్పు + wp->w_vsep_వెడల్పు;
    }
    లేకపోతే
    {
	startrow = *అడ్డువరుస;
	startcol = *నిలువు;
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	{
	    ఒకవేళ (topfrp->fr_layout == FR_అడ్డువరుస)
		*అడ్డువరుస = startrow;	// all frames are at the same row
	    లేకపోతే
		*నిలువు = startcol;	// all frames are at the same col
	    చట్రం_comp_pos(frp, అడ్డువరుస, నిలువు);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    శూన్య
win_ensure_కొలత(శూన్య)
{
    ఒకవేళ (curwin->w_ఎత్తు == 0)
	win_setheight(1);
    ఒకవేళ (curwin->w_వెడల్పు == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    శూన్య
win_setheight(పూర్ణాంకం ఎత్తు)
{
    win_setheight_win(ఎత్తు, curwin);
#ఒకవేళ defined(FEAT_STL_OPT)
    చట్రం_change_statusline_ఎత్తు();
#అయితే_ముగింపు
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    శూన్య
win_setheight_win(పూర్ణాంకం ఎత్తు, win_T *win)
{
    ఒకవేళ (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	ఒకవేళ (ఎత్తు < p_wmh)
	    ఎత్తు = p_wmh;
	ఒకవేళ (ఎత్తు == 0)
	    ఎత్తు = 1;
	ఎత్తు += WINBAR_ఎత్తు(curwin);
    }

    చట్రం_setheight(win->w_చట్రం, ఎత్తు + win->w_స్థితి_ఎత్తు);

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
    స్థిర శూన్య
చట్రం_setheight(చట్రం_T *curfrp, పూర్ణాంకం ఎత్తు)
{
    పూర్ణాంకం		స్థలం;		// total number of lines available
    పూర్ణాంకం		take;		// number of lines taken from other windows
    పూర్ణాంకం		స్థలం_cmdline;	// lines available from cmdline
    పూర్ణాంకం		నిరంతరం;
    చట్రం_T	*frp;
    పూర్ణాంకం		h;
    పూర్ణాంకం		స్థలం_reserved;

    // If the height already is the desired value, nothing to do.
    ఒకవేళ (curfrp->fr_ఎత్తు == ఎత్తు)
	తిరిగి;

    ఒకవేళ (curfrp->fr_మాతృ == ఖాళీ)
    {
	// topframe: can only change the command line height
	ఒకవేళ (ఎత్తు > 0)
	    చట్రం_new_ఎత్తు(curfrp, ఎత్తు, FALSE, FALSE, TRUE);
    }
    లేకపోతే ఒకవేళ (curfrp->fr_మాతృ->fr_layout == FR_అడ్డువరుస)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = చట్రం_minheight(curfrp->fr_మాతృ, ఖాళీ);
	ఒకవేళ (ఎత్తు < h)
	    ఎత్తు = h;
	చట్రం_setheight(curfrp->fr_మాతృ, ఎత్తు);
    }
    లేకపోతే
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
	కొరకు (నిరంతరం = 1; నిరంతరం <= 2; ++నిరంతరం)
	{
	    స్థలం = 0;
	    స్థలం_reserved = 0;
	    FOR_ALL_చట్రాలు(frp, curfrp->fr_మాతృ->fr_బిడ్డ)
	    {
		ఒకవేళ (frp != curfrp
			&& frp->fr_win != ఖాళీ
			&& frp->fr_win->w_p_wfh)
		    స్థలం_reserved += frp->fr_ఎత్తు;
		స్థలం += frp->fr_ఎత్తు;
		ఒకవేళ (frp != curfrp)
		    స్థలం -= చట్రం_minheight(frp, ఖాళీ);
	    }
	    ఒకవేళ (curfrp->fr_వెడల్పు != topframe->fr_వెడల్పు)
		స్థలం_cmdline = 0;
	    లేకపోతే
	    {
		స్థలం_cmdline = అడ్డువరుసలు - p_ch - (lastwin->w_winrow
						+ VISIBLE_ఎత్తు(lastwin)
						+ lastwin->w_స్థితి_ఎత్తు);
		ఒకవేళ (స్థలం_cmdline < 0)
		    స్థలం_cmdline = 0;
	    }

	    ఒకవేళ (ఎత్తు <= స్థలం + స్థలం_cmdline)
		ఆపు;
	    ఒకవేళ (నిరంతరం == 2 || curfrp->fr_వెడల్పు == topframe->fr_వెడల్పు)
	    {
		ఎత్తు = స్థలం + స్థలం_cmdline;
		ఆపు;
	    }
	    చట్రం_setheight(curfrp->fr_మాతృ, ఎత్తు
		+ చట్రం_minheight(curfrp->fr_మాతృ, NOWIN) - (పూర్ణాంకం)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = ఎత్తు - curfrp->fr_ఎత్తు;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	ఒకవేళ (ఎత్తు > స్థలం + స్థలం_cmdline - స్థలం_reserved)
	    స్థలం_reserved = స్థలం + స్థలం_cmdline - ఎత్తు;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	ఒకవేళ (take < 0 && స్థలం - curfrp->fr_ఎత్తు <= స్థలం_reserved)
	    స్థలం_reserved = 0;

	ఒకవేళ (take > 0 && స్థలం_cmdline > 0)
	{
	    // use lines from cmdline first
	    ఒకవేళ (take < స్థలం_cmdline)
		స్థలం_cmdline = take;
	    take -= స్థలం_cmdline;
	    topframe->fr_ఎత్తు += స్థలం_cmdline;
	}

	/*
	 * set the current frame to the new height
	 */
	చట్రం_new_ఎత్తు(curfrp, ఎత్తు, FALSE, FALSE, TRUE);

	/*
	 * First take lines from the frames after the current frame.  If
	 * that is not enough, takes lines from frames above the current
	 * frame.
	 */
	కొరకు (నిరంతరం = 0; నిరంతరం < 2; ++నిరంతరం)
	{
	    ఒకవేళ (నిరంతరం == 0)
		frp = curfrp->fr_తదుపరి;	// 1st run: start with next window
	    లేకపోతే
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    అయితే (frp != ఖాళీ && take != 0)
	    {
		h = చట్రం_minheight(frp, ఖాళీ);
		ఒకవేళ (స్థలం_reserved > 0
			&& frp->fr_win != ఖాళీ
			&& frp->fr_win->w_p_wfh)
		{
		    ఒకవేళ (స్థలం_reserved >= frp->fr_ఎత్తు)
			స్థలం_reserved -= frp->fr_ఎత్తు;
		    లేకపోతే
		    {
			ఒకవేళ (frp->fr_ఎత్తు - స్థలం_reserved > take)
			    స్థలం_reserved = frp->fr_ఎత్తు - take;
			take -= frp->fr_ఎత్తు - స్థలం_reserved;
			చట్రం_new_ఎత్తు(frp, స్థలం_reserved, FALSE, FALSE,
									TRUE);
			స్థలం_reserved = 0;
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ (frp->fr_ఎత్తు - take < h)
		    {
			take -= frp->fr_ఎత్తు - h;
			చట్రం_new_ఎత్తు(frp, h, FALSE, FALSE, TRUE);
		    }
		    లేకపోతే
		    {
			చట్రం_new_ఎత్తు(frp, frp->fr_ఎత్తు - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		ఒకవేళ (నిరంతరం == 0)
		    frp = frp->fr_తదుపరి;
		లేకపోతే
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    శూన్య
win_setwidth(పూర్ణాంకం వెడల్పు)
{
    win_setwidth_win(వెడల్పు, curwin);
}

    శూన్య
win_setwidth_win(పూర్ణాంకం వెడల్పు, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    ఒకవేళ (wp == curwin)
    {
	ఒకవేళ (వెడల్పు < p_wmw)
	    వెడల్పు = p_wmw;
	ఒకవేళ (వెడల్పు == 0)
	    వెడల్పు = 1;
    }
    లేకపోతే ఒకవేళ (వెడల్పు < 0)
	వెడల్పు = 0;

    చట్రం_setwidth(wp->w_చట్రం, వెడల్పు + wp->w_vsep_వెడల్పు);

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
    స్థిర శూన్య
చట్రం_setwidth(చట్రం_T *curfrp, పూర్ణాంకం వెడల్పు)
{
    పూర్ణాంకం		స్థలం;		// total number of lines available
    పూర్ణాంకం		take;		// number of lines taken from other windows
    పూర్ణాంకం		నిరంతరం;
    చట్రం_T	*frp;
    పూర్ణాంకం		w;
    పూర్ణాంకం		స్థలం_reserved;

    // If the width already is the desired value, nothing to do.
    ఒకవేళ (curfrp->fr_వెడల్పు == వెడల్పు)
	తిరిగి;

    ఒకవేళ (curfrp->fr_మాతృ == ఖాళీ)
	// topframe: can't change width
	తిరిగి;

    ఒకవేళ (curfrp->fr_మాతృ->fr_layout == FR_నిలువు)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = చట్రం_minwidth(curfrp->fr_మాతృ, ఖాళీ);
	ఒకవేళ (వెడల్పు < w)
	    వెడల్పు = w;
	చట్రం_setwidth(curfrp->fr_మాతృ, వెడల్పు);
    }
    లేకపోతే
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	కొరకు (నిరంతరం = 1; నిరంతరం <= 2; ++నిరంతరం)
	{
	    స్థలం = 0;
	    స్థలం_reserved = 0;
	    FOR_ALL_చట్రాలు(frp, curfrp->fr_మాతృ->fr_బిడ్డ)
	    {
		ఒకవేళ (frp != curfrp
			&& frp->fr_win != ఖాళీ
			&& frp->fr_win->w_p_wfw)
		    స్థలం_reserved += frp->fr_వెడల్పు;
		స్థలం += frp->fr_వెడల్పు;
		ఒకవేళ (frp != curfrp)
		    స్థలం -= చట్రం_minwidth(frp, ఖాళీ);
	    }

	    ఒకవేళ (వెడల్పు <= స్థలం)
		ఆపు;
	    ఒకవేళ (నిరంతరం == 2 || curfrp->fr_ఎత్తు >= అడ్డువరుసలు_AVAIL)
	    {
		వెడల్పు = స్థలం;
		ఆపు;
	    }
	    చట్రం_setwidth(curfrp->fr_మాతృ, వెడల్పు
		 + చట్రం_minwidth(curfrp->fr_మాతృ, NOWIN) - (పూర్ణాంకం)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = వెడల్పు - curfrp->fr_వెడల్పు;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	ఒకవేళ (వెడల్పు > స్థలం - స్థలం_reserved)
	    స్థలం_reserved = స్థలం - వెడల్పు;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	ఒకవేళ (take < 0 && స్థలం - curfrp->fr_వెడల్పు < స్థలం_reserved)
	    స్థలం_reserved = 0;

	/*
	 * set the current frame to the new width
	 */
	చట్రం_new_వెడల్పు(curfrp, వెడల్పు, FALSE, FALSE);

	/*
	 * First take lines from the frames right of the current frame.  If
	 * that is not enough, takes lines from frames left of the current
	 * frame.
	 */
	కొరకు (నిరంతరం = 0; నిరంతరం < 2; ++నిరంతరం)
	{
	    ఒకవేళ (నిరంతరం == 0)
		frp = curfrp->fr_తదుపరి;	// 1st run: start with next window
	    లేకపోతే
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    అయితే (frp != ఖాళీ && take != 0)
	    {
		w = చట్రం_minwidth(frp, ఖాళీ);
		ఒకవేళ (స్థలం_reserved > 0
			&& frp->fr_win != ఖాళీ
			&& frp->fr_win->w_p_wfw)
		{
		    ఒకవేళ (స్థలం_reserved >= frp->fr_వెడల్పు)
			స్థలం_reserved -= frp->fr_వెడల్పు;
		    లేకపోతే
		    {
			ఒకవేళ (frp->fr_వెడల్పు - స్థలం_reserved > take)
			    స్థలం_reserved = frp->fr_వెడల్పు - take;
			take -= frp->fr_వెడల్పు - స్థలం_reserved;
			చట్రం_new_వెడల్పు(frp, స్థలం_reserved, FALSE, FALSE);
			స్థలం_reserved = 0;
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ (frp->fr_వెడల్పు - take < w)
		    {
			take -= frp->fr_వెడల్పు - w;
			చట్రం_new_వెడల్పు(frp, w, FALSE, FALSE);
		    }
		    లేకపోతే
		    {
			చట్రం_new_వెడల్పు(frp, frp->fr_వెడల్పు - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		ఒకవేళ (నిరంతరం == 0)
		    frp = frp->fr_తదుపరి;
		లేకపోతే
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    శూన్య
win_setminheight(శూన్య)
{
    పూర్ణాంకం		స్థలం;
    పూర్ణాంకం		needed;
    పూర్ణాంకం		మొదటి = TRUE;

    // loop until there is a 'winminheight' that is possible
    అయితే (p_wmh > 0)
    {
	స్థలం = అడ్డువరుసలు - p_ch;
	needed = min_అడ్డువరుసలు_for_all_tabpages() - 1;  // 1 was added for the cmdline
	ఒకవేళ (స్థలం >= needed)
	    ఆపు;
	--p_wmh;
	ఒకవేళ (మొదటి)
	{
	    emsg(_(e_not_enough_స్థలం));
	    మొదటి = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    శూన్య
win_setminwidth(శూన్య)
{
    పూర్ణాంకం		స్థలం;
    పూర్ణాంకం		needed;
    పూర్ణాంకం		మొదటి = TRUE;

    // loop until there is a 'winminheight' that is possible
    అయితే (p_wmw > 0)
    {
	స్థలం = topframe->fr_వెడల్పు;
	needed = చట్రం_minwidth(topframe, ఖాళీ);
	ఒకవేళ (స్థలం >= needed)
	    ఆపు;
	--p_wmw;
	ఒకవేళ (మొదటి)
	{
	    emsg(_(e_not_enough_స్థలం));
	    మొదటి = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    శూన్య
win_drag_స్థితి_పంక్తి(win_T *dragwin, పూర్ణాంకం స్థానభ్రంశం)
{
    చట్రం_T	*curfr;
    చట్రం_T	*fr;
    పూర్ణాంకం		స్థలం;
    పూర్ణాంకం		పైకి;	// if TRUE, drag status line up, otherwise down
    పూర్ణాంకం		n;

    fr = dragwin->w_చట్రం;
    curfr = fr;
    ఒకవేళ (fr != topframe)		// more than one window
    {
	fr = fr->fr_మాతృ;
	// When the parent frame is not a column of frames, its parent should
	// be.
	ఒకవేళ (fr->fr_layout != FR_నిలువు)
	{
	    curfr = fr;
	    ఒకవేళ (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_మాతృ;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    అయితే (curfr != topframe && curfr->fr_తదుపరి == ఖాళీ)
    {
	ఒకవేళ (fr != topframe)
	    fr = fr->fr_మాతృ;
	curfr = fr;
	ఒకవేళ (fr != topframe)
	    fr = fr->fr_మాతృ;
    }

    ఒకవేళ (స్థానభ్రంశం < 0) // drag up
    {
	పైకి = TRUE;
	స్థానభ్రంశం = -స్థానభ్రంశం;
	// sum up the room of the current frame and above it
	ఒకవేళ (fr == curfr)
	{
	    // only one window
	    స్థలం = fr->fr_ఎత్తు - చట్రం_minheight(fr, ఖాళీ);
	}
	లేకపోతే
	{
	    స్థలం = 0;
	    కొరకు (fr = fr->fr_బిడ్డ; ; fr = fr->fr_తదుపరి)
	    {
		స్థలం += fr->fr_ఎత్తు - చట్రం_minheight(fr, ఖాళీ);
		ఒకవేళ (fr == curfr)
		    ఆపు;
	    }
	}
	fr = curfr->fr_తదుపరి;		// put fr at frame that grows
    }
    లేకపోతే    // drag down
    {
	పైకి = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	స్థలం = అడ్డువరుసలు - cmdline_అడ్డువరుస;
	ఒకవేళ (curfr->fr_తదుపరి == ఖాళీ)
	    --స్థలం;
	లేకపోతే
	    స్థలం -= p_ch;
	ఒకవేళ (స్థలం < 0)
	    స్థలం = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_చట్రాలు(fr, curfr->fr_తదుపరి)
	    స్థలం += fr->fr_ఎత్తు - చట్రం_minheight(fr, ఖాళీ);
	fr = curfr;			// put fr at window that grows
    }

    ఒకవేళ (స్థలం < స్థానభ్రంశం)		// Not enough room
	స్థానభ్రంశం = స్థలం;		// Move as far as we can
    ఒకవేళ (స్థానభ్రంశం <= 0)
	తిరిగి;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    ఒకవేళ (fr != ఖాళీ)
	చట్రం_new_ఎత్తు(fr, fr->fr_ఎత్తు + స్థానభ్రంశం, పైకి, FALSE, TRUE);

    ఒకవేళ (పైకి)
	fr = curfr;		// current frame gets smaller
    లేకపోతే
	fr = curfr->fr_తదుపరి;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    అయితే (fr != ఖాళీ && స్థానభ్రంశం > 0)
    {
	n = చట్రం_minheight(fr, ఖాళీ);
	ఒకవేళ (fr->fr_ఎత్తు - స్థానభ్రంశం <= n)
	{
	    స్థానభ్రంశం -= fr->fr_ఎత్తు - n;
	    చట్రం_new_ఎత్తు(fr, n, !పైకి, FALSE, TRUE);
	}
	లేకపోతే
	{
	    చట్రం_new_ఎత్తు(fr, fr->fr_ఎత్తు - స్థానభ్రంశం, !పైకి, FALSE, TRUE);
	    ఆపు;
	}
	ఒకవేళ (పైకి)
	    fr = fr->fr_prev;
	లేకపోతే
	    fr = fr->fr_తదుపరి;
    }
    win_comp_pos();
    win_fix_scroll(TRUE);

    redraw_all_later(UPD_SOME_VALID);
    showmode();
}

/*
 * Separator line of dragwin is dragged "offset" lines right (negative is left).
 */
    శూన్య
win_drag_vsep_పంక్తి(win_T *dragwin, పూర్ణాంకం స్థానభ్రంశం)
{
    చట్రం_T	*curfr;
    చట్రం_T	*fr;
    పూర్ణాంకం		స్థలం;
    పూర్ణాంకం		ఎడమ_2;	// if TRUE, drag separator line left, otherwise right
    పూర్ణాంకం		n;

    fr = dragwin->w_చట్రం;
    ఒకవేళ (fr == topframe)		// only one window (cannot happen?)
	తిరిగి;
    curfr = fr;
    fr = fr->fr_మాతృ;
    // When the parent frame is not a row of frames, its parent should be.
    ఒకవేళ (fr->fr_layout != FR_అడ్డువరుస)
    {
	ఒకవేళ (fr == topframe)	// only a column of windows (cannot happen?)
	    తిరిగి;
	curfr = fr;
	fr = fr->fr_మాతృ;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    అయితే (curfr->fr_తదుపరి == ఖాళీ)
    {
	ఒకవేళ (fr == topframe)
	    ఆపు;
	curfr = fr;
	fr = fr->fr_మాతృ;
	ఒకవేళ (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_మాతృ;
	}
    }

    ఒకవేళ (స్థానభ్రంశం < 0) // drag left
    {
	ఎడమ_2 = TRUE;
	స్థానభ్రంశం = -స్థానభ్రంశం;
	// sum up the room of the current frame and left of it
	స్థలం = 0;
	కొరకు (fr = fr->fr_బిడ్డ; ; fr = fr->fr_తదుపరి)
	{
	    స్థలం += fr->fr_వెడల్పు - చట్రం_minwidth(fr, ఖాళీ);
	    ఒకవేళ (fr == curfr)
		ఆపు;
	}
	fr = curfr->fr_తదుపరి;		// put fr at frame that grows
    }
    లేకపోతే    // drag right
    {
	ఎడమ_2 = FALSE;
	// sum up the room of frames right of the current one
	స్థలం = 0;
	FOR_ALL_చట్రాలు(fr, curfr->fr_తదుపరి)
	    స్థలం += fr->fr_వెడల్పు - చట్రం_minwidth(fr, ఖాళీ);
	fr = curfr;			// put fr at window that grows
    }

    ఒకవేళ (స్థలం < స్థానభ్రంశం)		// Not enough room
	స్థానభ్రంశం = స్థలం;		// Move as far as we can
    ఒకవేళ (స్థానభ్రంశం <= 0)		// No room at all, quit.
	తిరిగి;
    ఒకవేళ (fr == ఖాళీ)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	తిరిగి;

    // grow frame fr by offset lines
    చట్రం_new_వెడల్పు(fr, fr->fr_వెడల్పు + స్థానభ్రంశం, ఎడమ_2, FALSE);

    // shrink other frames: current and at the left or at the right
    ఒకవేళ (ఎడమ_2)
	fr = curfr;		// current frame gets smaller
    లేకపోతే
	fr = curfr->fr_తదుపరి;	// next frame gets smaller

    అయితే (fr != ఖాళీ && స్థానభ్రంశం > 0)
    {
	n = చట్రం_minwidth(fr, ఖాళీ);
	ఒకవేళ (fr->fr_వెడల్పు - స్థానభ్రంశం <= n)
	{
	    స్థానభ్రంశం -= fr->fr_వెడల్పు - n;
	    చట్రం_new_వెడల్పు(fr, n, !ఎడమ_2, FALSE);
	}
	లేకపోతే
	{
	    చట్రం_new_వెడల్పు(fr, fr->fr_వెడల్పు - స్థానభ్రంశం, !ఎడమ_2, FALSE);
	    ఆపు;
	}
	ఒకవేళ (ఎడమ_2)
	    fr = fr->fr_prev;
	లేకపోతే
	    fr = fr->fr_తదుపరి;
    }
    win_comp_pos();
    redraw_all_later(UPD_NOT_VALID);
}

#నిర్వచించు FRACTION_MULT	16384L

/*
 * Set wp->w_fraction for the current w_wrow and w_height.
 * Has no effect when the window is less than two lines.
 */
    శూన్య
సమితి_fraction(win_T *wp)
{
    ఒకవేళ (wp->w_ఎత్తు > 1)
	// When cursor is in the first line the percentage is computed as if
	// it's halfway that line.  Thus with two lines it is 25%, with three
	// lines 17%, etc.  Similarly for the last line: 75%, 83%, etc.
	wp->w_fraction = ((దీర్ఘ)wp->w_wrow * FRACTION_MULT
				     + FRACTION_MULT / 2) / (దీర్ఘ)wp->w_ఎత్తు;
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
    స్థిర శూన్య
win_fix_scroll(పూర్ణాంకం resize)
{
    ఒకవేళ (*p_spk == 'c')
	తిరిగి;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	ఒకవేళ (wp->w_ఎత్తు != wp->w_prev_ఎత్తు)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    ఒకవేళ (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    {
		పూర్ణాంకం diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_ఎత్తు - wp->w_prev_ఎత్తు);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		ఒకవేళ (diff > 0)
		    cursor_కిందికి_inner(wp, diff);
		లేకపోతే
		    cursor_పైకి_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_గమ్యం_fraction(wp, wp->w_prev_ఎత్తు);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		ఒకవేళ (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    లేకపోతే ఒకవేళ (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_ఎత్తు = wp->w_ఎత్తు;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    ఒకవేళ (!(get_real_state() & (బహుళకం_సాధారణ|బహుళకం_CMDLINE|బహుళకం_TERMINAL)))
	win_fix_cursor(FALSE);
    లేకపోతే ఒకవేళ (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    స్థిర శూన్య
win_fix_cursor(పూర్ణాంకం సాధారణ_4)
{
    win_T	*wp = curwin;

    ఒకవేళ (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క < wp->w_ఎత్తు)
	తిరిగి;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    దీర్ఘ so = MIN(wp->w_ఎత్తు / 2, get_scrolloff_విలువ());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_కిందికి_inner(wp, so);
    linenr_T శిఖరం = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_పైకి_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    ఒకవేళ (lnum > bot && (wp->w_botline - wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క) != 1)
	nlnum = bot;
    లేకపోతే ఒకవేళ (lnum < శిఖరం && wp->w_topline != 1)
	nlnum = (so == wp->w_ఎత్తు / 2) ? bot : శిఖరం;

    ఒకవేళ (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	ఒకవేళ (సాధారణ_4)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	లేకపోతే  // Scroll instead when not in normal mode.
	{
	    wp->w_fraction = (nlnum == bot) ? FRACTION_MULT : 0;
	    scroll_గమ్యం_fraction(wp, wp->w_prev_ఎత్తు);
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
    శూన్య
win_new_ఎత్తు(win_T *wp, పూర్ణాంకం ఎత్తు)
{
    పూర్ణాంకం		prev_ఎత్తు = wp->w_ఎత్తు;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    ఒకవేళ (ఎత్తు < 0)
	ఎత్తు = 0;
    ఒకవేళ (wp->w_ఎత్తు == ఎత్తు)
	తిరిగి;	    // nothing to do

    ఒకవేళ (wp->w_ఎత్తు > 0)
    {
	ఒకవేళ (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	ఒకవేళ (wp->w_ఎత్తు != prev_ఎత్తు)
	    తిరిగి;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	ఒకవేళ (wp->w_wrow != wp->w_prev_fraction_అడ్డువరుస)
	    సమితి_fraction(wp);
    }

    wp->w_ఎత్తు = ఎత్తు;
    wp->w_redr_స్థితి = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    ఒకవేళ (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	ఒకవేళ (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_గమ్యం_fraction(wp, prev_ఎత్తు);
    }
}

    శూన్య
scroll_గమ్యం_fraction(win_T *wp, పూర్ణాంకం prev_ఎత్తు)
{
    linenr_T	lnum;
    పూర్ణాంకం		sline, పంక్తి_కొలత;
    పూర్ణాంకం		ఎత్తు = wp->w_ఎత్తు;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    ఒకవేళ (ఎత్తు > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (ఎత్తు < wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	ఒకవేళ (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((దీర్ఘ)wp->w_fraction * (దీర్ఘ)ఎత్తు - 1L)
							       / FRACTION_MULT;
	పంక్తి_కొలత = plines_win_నిలువు(wp, lnum, (దీర్ఘ)(wp->w_cursor.నిలువు)) - 1;
	sline = wp->w_wrow - పంక్తి_కొలత;

	ఒకవేళ (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    పూర్ణాంకం అడ్డువరుసలు_2 = plines_win(wp, lnum, FALSE);

	    ఒకవేళ (sline > wp->w_ఎత్తు - అడ్డువరుసలు_2)
	    {
		sline = wp->w_ఎత్తు - అడ్డువరుసలు_2;
		wp->w_wrow -= అడ్డువరుసలు_2 - పంక్తి_కొలత;
	    }
	}

	ఒకవేళ (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    పూర్ణాంకం		want_అడ్డువరుస = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = పంక్తి_కొలత;
	    ఒకవేళ (wp->w_wrow >= wp->w_ఎత్తు
				       && (wp->w_వెడల్పు - win_నిలువు_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_వెడల్పు - win_నిలువు_off(wp);

		--wp->w_wrow;
		అయితే (wp->w_wrow > want_అడ్డువరుస)
		{
		    skipcol += wp->w_వెడల్పు - win_నిలువు_off(wp) + win_నిలువు_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	లేకపోతే ఒకవేళ (sline > 0)
	{
	    అయితే (sline > 0 && lnum > 1)
	    {
#నిర్వచితమైతే FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, ఖాళీ, TRUE, ఖాళీ);
		ఒకవేళ (lnum == 1)
		{
		    // first line in buffer is folded
		    పంక్తి_కొలత = 1;
		    --sline;
		    ఆపు;
		}
#అయితే_ముగింపు
		--lnum;
#నిర్వచితమైతే FEAT_DIFF
		ఒకవేళ (lnum == wp->w_topline)
		    పంక్తి_కొలత = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		లేకపోతే
#అయితే_ముగింపు
		    పంక్తి_కొలత = plines_win(wp, lnum, TRUE);
		sline -= పంక్తి_కొలత;
	    }

	    ఒకవేళ (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#నిర్వచితమైతే FEAT_FOLDING
		hasFoldingWin(wp, lnum, ఖాళీ, &lnum, TRUE, ఖాళీ);
#అయితే_ముగింపు
		lnum++;
		wp->w_wrow -= పంక్తి_కొలత + sline;
	    }
	    లేకపోతే ఒకవేళ (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	సమితి_topline(wp, lnum);
    }

    ఒకవేళ (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    ఒకవేళ (prev_ఎత్తు > 0)
	wp->w_prev_fraction_అడ్డువరుస = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    శూన్య
win_new_వెడల్పు(win_T *wp, పూర్ణాంకం వెడల్పు)
{
    // Should we give an error if width < 0?
    wp->w_వెడల్పు = వెడల్పు < 0 ? 0 : వెడల్పు;
    wp->w_పంక్తులు_valid = 0;
    మారింది_పంక్తి_abv_curs_win(wp);
    invalidate_botline_win(wp);

    ఒకవేళ (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_స్థితి = true;
}

    శూన్య
win_comp_scroll(win_T *wp)
{
#ఒకవేళ defined(FEAT_EVAL)
    పూర్ణాంకం old_w_p_scr = wp->w_p_scr;
#అయితే_ముగింపు

    wp->w_p_scr = ((గుర్తులేని)wp->w_ఎత్తు >> 1);
    ఒకవేళ (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (wp->w_p_scr != old_w_p_scr)
    {
	// Used by "verbose set scroll".
	wp->w_p_script_ctx[WV_SCROLL].sc_sid = SID_WINLAYOUT;
	wp->w_p_script_ctx[WV_SCROLL].sc_lnum = 0;
    }
#అయితే_ముగింపు
}

/*
 * Command_height: called whenever p_ch has been changed.
 */
    శూన్య
command_ఎత్తు(శూన్య)
{
    పూర్ణాంకం		old_p_ch = curtab->tp_ch_వాడిన;

    // Find bottom frame with width of screen.
    చట్రం_T *frp = lastwin->w_చట్రం;
    అయితే (frp->fr_వెడల్పు != topframe->fr_వెడల్పు && frp->fr_మాతృ != ఖాళీ)
	frp = frp->fr_మాతృ;

    // Avoid changing the height of a window with 'winfixheight' set.
    అయితే (frp->fr_prev != ఖాళీ && frp->fr_layout == FR_ఆకు
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    అయితే (p_ch > old_p_ch && command_చట్రం_ఎత్తు)
    {
	ఒకవేళ (frp == ఖాళీ)
	{
	    emsg(_(e_not_enough_స్థలం));
	    p_ch = old_p_ch;
	    ఆపు;
	}
	పూర్ణాంకం h = MIN(p_ch - old_p_ch,
			frp->fr_ఎత్తు - చట్రం_minheight(frp, ఖాళీ));
	చట్రం_కూడు_ఎత్తు(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    ఒకవేళ (p_ch < old_p_ch && command_చట్రం_ఎత్తు && frp != ఖాళీ)
	చట్రం_కూడు_ఎత్తు(frp, (పూర్ణాంకం)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_అడ్డువరుస = అడ్డువరుసలు - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    ఒకవేళ (సందేశం_scrolled == 0 && నిండిన_screen)
    {
	screen_fill(cmdline_అడ్డువరుస, (పూర్ణాంకం)అడ్డువరుసలు, 0, (పూర్ణాంకం)Columns, ' ', ' ', 0);
	సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;
    }

    // Use the value of p_ch that we remembered.  This is needed for when the
    // GUI starts up, we can't be sure in what order things happen.  And when
    // p_ch was changed in another tab page.
    curtab->tp_ch_వాడిన = p_ch;
    min_సమితి_ch = p_ch;
}

/*
 * Resize frame "frp" to be "n" lines higher (negative for less high).
 * Also resize the frames it is contained in.
 */
    స్థిర శూన్య
చట్రం_కూడు_ఎత్తు(చట్రం_T *frp, పూర్ణాంకం n)
{
    చట్రం_new_ఎత్తు(frp, frp->fr_ఎత్తు + n, FALSE, FALSE, FALSE);
    కొరకు (;;)
    {
	frp = frp->fr_మాతృ;
	ఒకవేళ (frp == ఖాళీ)
	    ఆపు;
	frp->fr_ఎత్తు += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    శూన్య
చివరి_స్థితి(
    పూర్ణాంకం		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    చివరి_స్థితి_rec(topframe, చివరి_stl_ఎత్తు(morewin) > 0);
#ఒకవేళ defined(FEAT_STL_OPT)
    చట్రం_change_statusline_ఎత్తు();
#అయితే_ముగింపు
}

    స్థిర శూన్య
చివరి_స్థితి_rec(చట్రం_T *fr, పూర్ణాంకం statusline)
{
    చట్రం_T	*fp;
    win_T	*wp;

    ఒకవేళ (fr->fr_layout == FR_ఆకు)
    {
	wp = fr->fr_win;
	ఒకవేళ (wp->w_స్థితి_ఎత్తు != 0 && !statusline)
	{
	    // remove status line
	    win_new_ఎత్తు(wp, wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు);
	    wp->w_స్థితి_ఎత్తు = 0;
	    comp_నిలువు();
	}
	లేకపోతే ఒకవేళ (wp->w_స్థితి_ఎత్తు == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    అయితే (fp->fr_ఎత్తు <= చట్రం_minheight(fp, ఖాళీ))
	    {
		ఒకవేళ (fp == topframe)
		{
		    emsg(_(e_not_enough_స్థలం));
		    తిరిగి;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		ఒకవేళ (fp->fr_మాతృ->fr_layout == FR_నిలువు && fp->fr_prev != ఖాళీ)
		    fp = fp->fr_prev;
		లేకపోతే
		    fp = fp->fr_మాతృ;
	    }
	    wp->w_స్థితి_ఎత్తు = statusline_ఎత్తు(wp);
	    ఒకవేళ (fp != fr)
	    {
		చట్రం_new_ఎత్తు(fp, fp->fr_ఎత్తు - wp->w_స్థితి_ఎత్తు,
			FALSE, FALSE, FALSE);
		చట్రం_fix_ఎత్తు(wp);
		win_comp_pos();
	    }
	    లేకపోతే
		win_new_ఎత్తు(wp, wp->w_ఎత్తు - wp->w_స్థితి_ఎత్తు);
	    comp_నిలువు();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	ఒకవేళ (నిరపేక్ష(wp->w_ఎత్తు - wp->w_prev_ఎత్తు) == 1)
	    wp->w_prev_ఎత్తు = wp->w_ఎత్తు;
    }
    లేకపోతే ఒకవేళ (fr->fr_layout == FR_అడ్డువరుస)
    {
	// vertically split windows, set status line for each one
	ఒకవేళ (!statusline)
	{
	    FOR_ALL_చట్రాలు(fp, fr->fr_బిడ్డ)
		చివరి_స్థితి_rec(fp, statusline);
	}
	లేకపోతే
	{
	    చట్రం_T	*fp2;
	    పూర్ణాంకం		max_stlh = 0;
	    పూర్ణాంకం		new_అడ్డువరుస_ఎత్తు;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_చట్రాలు(fp, fr->fr_బిడ్డ)
		ఒకవేళ (fp->fr_win != ఖాళీ && fp->fr_win->w_స్థితి_ఎత్తు == 0)
		{
		    పూర్ణాంకం h = statusline_ఎత్తు(fp->fr_win);
		    ఒకవేళ (h > max_stlh)
			max_stlh = h;
		}
	    ఒకవేళ (max_stlh == 0)
		తిరిగి;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    అయితే (fp2->fr_ఎత్తు - చట్రం_minheight(fp2, ఖాళీ) < max_stlh)
	    {
		ఒకవేళ (fp2 == topframe)
		{
		    emsg(_(e_not_enough_స్థలం));
		    తిరిగి;
		}
		ఒకవేళ (fp2->fr_మాతృ->fr_layout == FR_నిలువు
						&& fp2->fr_prev != ఖాళీ)
		    fp2 = fp2->fr_prev;
		లేకపోతే
		    fp2 = fp2->fr_మాతృ;
	    }

	    ఒకవేళ (fp2 != fr)
	    {
		చట్రం_new_ఎత్తు(fp2, fp2->fr_ఎత్తు - max_stlh,
							FALSE, FALSE, FALSE);
		new_అడ్డువరుస_ఎత్తు = fr->fr_ఎత్తు + max_stlh;
	    }
	    లేకపోతే
		new_అడ్డువరుస_ఎత్తు = fr->fr_ఎత్తు;

	    // Set status and content heights for all leaves.
	    FOR_ALL_చట్రాలు(fp, fr->fr_బిడ్డ)
	    {
		ఒకవేళ (fp->fr_win != ఖాళీ)
		{
		    wp = fp->fr_win;
		    ఒకవేళ (wp->w_స్థితి_ఎత్తు == 0)
		    {
			wp->w_స్థితి_ఎత్తు = statusline_ఎత్తు(wp);
			win_new_ఎత్తు(wp,
				  new_అడ్డువరుస_ఎత్తు - wp->w_స్థితి_ఎత్తు);
			చట్రం_fix_ఎత్తు(wp);
			comp_నిలువు();
			redraw_all_later(UPD_SOME_VALID);
			ఒకవేళ (నిరపేక్ష(wp->w_ఎత్తు - wp->w_prev_ఎత్తు) == 1)
			    wp->w_prev_ఎత్తు = wp->w_ఎత్తు;
		    }
		}
		లేకపోతే
		    చివరి_స్థితి_rec(fp, statusline); // nested frames
	    }

	    fr->fr_ఎత్తు = new_అడ్డువరుస_ఎత్తు;
	    win_comp_pos();
	}
    }
    లేకపోతే
    {
	// horizontally split window, set status line for last one
	కొరకు (fp = fr->fr_బిడ్డ; fp->fr_తదుపరి != ఖాళీ; fp = fp->fr_తదుపరి)
	    ;
	చివరి_స్థితి_rec(fp, statusline);
    }
}

#ఒకవేళ defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    శూన్య
update_stl_rendered_ఎత్తు(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == ఖాళీ) ? p_stl : wp->w_p_stl;
    పూర్ణాంకం		opt_flags = (wp == ఖాళీ) ? 0 : OPT_LOCAL;
    పూర్ణాంకం		*ఎత్తు = (wp == ఖాళీ) ? &stl_rendered_ఎత్తు
						: &wp->w_stl_rendered_ఎత్తు;

    *ఎత్తు = get_stl_rendered_ఎత్తు(wp == ఖాళీ ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    స్థిర పూర్ణాంకం
చట్రం_కనుగొను_global_stlh_rec(చట్రం_T *frp, పూర్ణాంకం h)
{
    ఒకవేళ (frp->fr_layout == FR_ఆకు)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	ఒకవేళ (wp->w_ఎత్తు > 0 && wp->w_స్థితి_ఎత్తు > 0
		&& *wp->w_p_stlo == NUL)
	{
	    పూర్ణాంకం win_free_ఎత్తు = frp->fr_ఎత్తు - WINBAR_ఎత్తు(wp);

	    ఒకవేళ (win_free_ఎత్తు - p_wmh < h)
		h = win_free_ఎత్తు - p_wmh;
	}
    }
    లేకపోతే
    {
	కొరకు (frp = frp->fr_బిడ్డ; frp != ఖాళీ; frp = frp->fr_తదుపరి)
	    h = చట్రం_కనుగొను_global_stlh_rec(frp, h);
    }
    తిరిగి h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    స్థిర శూన్య
చట్రం_change_statusline_ఎత్తు_rec(చట్రం_T *frp, పూర్ణాంకం global_stlh)
{
    ఒకవేళ (frp->fr_layout == FR_ఆకు)
    {
	win_T  *wp = frp->fr_win;

	ఒకవేళ (wp->w_ఎత్తు > 0 && wp->w_స్థితి_ఎత్తు > 0)
	{
	    పూర్ణాంకం win_free_ఎత్తు = frp->fr_ఎత్తు - WINBAR_ఎత్తు(wp);

	    wp->w_స్థితి_ఎత్తు = statusline_ఎత్తు(wp);
	    ఒకవేళ (wp->w_p_stlo == ఖాళీ || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		ఒకవేళ (wp->w_స్థితి_ఎత్తు > global_stlh)
		    wp->w_స్థితి_ఎత్తు = global_stlh;
	    }
	    ఒకవేళ (wp->w_స్థితి_ఎత్తు > win_free_ఎత్తు - p_wmh)
		wp->w_స్థితి_ఎత్తు = win_free_ఎత్తు - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    పూర్ణాంకం	     saved_topfill = wp->w_topfill;
# endif
	    win_new_ఎత్తు(wp, win_free_ఎత్తు - wp->w_స్థితి_ఎత్తు);
	    wp->w_topline = saved_topline;
# ifdef FEAT_DIFF
	    wp->w_topfill = saved_topfill;
# endif
	    wp->w_valid &= ~(VALID_WROW | VALID_CROW
			     | VALID_BOTLINE | VALID_BOTLINE_AP);
	}
    }
    లేకపోతే
    {
	కొరకు (frp = frp->fr_బిడ్డ; frp != ఖాళీ; frp = frp->fr_తదుపరి)
	    చట్రం_change_statusline_ఎత్తు_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    పూర్ణాంకం
చట్రం_change_statusline_ఎత్తు(శూన్య)
{
    tabpage_T	*tp;
    పూర్ణాంకం		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = చట్రం_కనుగొను_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	చట్రం_change_statusline_ఎత్తు_rec(tp->tp_topframe, global_stlh);
    comp_నిలువు();
    redraw_all_later(UPD_SOME_VALID);
    తిరిగి global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    శూన్య
సమితి_stlo_mh(పూర్ణాంకం mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
statuslineopt_మారింది(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    పూర్ణాంకం		l_stlo_fh = FALSE;
    పూర్ణాంకం		l_stlo_mh = 1;

    ఒకవేళ (stlopt != ఖాళీ)
	p = stlopt;
    లేకపోతే
	p = wp->w_p_stlo;

    అయితే (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	ఒకవేళ (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    ఒకవేళ (l_stlo_mh < 1)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	ఒకవేళ (*p != ',' && *p != NUL)
	    తిరిగి FAIL;
	ఒకవేళ (*p == ',')
	    ++p;
    }

    ఒకవేళ (wp == ఖాళీ)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    లేకపోతే
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    పూర్ణాంకం
statusline_ఎత్తు(win_T *wp వాడని)
{
    పూర్ణాంకం stl_ఎత్తు = 1;
#ఒకవేళ defined(FEAT_STL_OPT)
    bool fixed;
    పూర్ణాంకం l_stl_rendered_ఎత్తు;

    ఒకవేళ (wp != ఖాళీ && *wp->w_p_stlo != NUL)
    {
	stl_ఎత్తు = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_ఎత్తు = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ఎత్తు
				    : stl_rendered_ఎత్తు;
    }
    లేకపోతే
    {
	stl_ఎత్తు = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_ఎత్తు = (wp != ఖాళీ && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_ఎత్తు
				    : stl_rendered_ఎత్తు;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    ఒకవేళ (!fixed)
    {
	పూర్ణాంకం has_custom_stl = (*p_stl != NUL)
	    || (wp != ఖాళీ && *wp->w_p_stl != NUL);
	ఒకవేళ (has_custom_stl)
	    stl_ఎత్తు = MAX(1, MIN(l_stl_rendered_ఎత్తు, stl_ఎత్తు));
	లేకపోతే
	    stl_ఎత్తు = 1;
    }
#అయితే_ముగింపు

    తిరిగి stl_ఎత్తు;
}

/*
 * Return the number of lines used by the tab page line.
 */
    పూర్ణాంకం
tabline_ఎత్తు(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    ఒకవేళ (gui_use_tabline())
	తిరిగి 0;
#అయితే_ముగింపు
    ఎంపిక (p_stal)
    {
	స్థితి 0: తిరిగి 0;
	స్థితి 1: తిరిగి (మొదటి_tabpage->tp_తదుపరి == ఖాళీ) ? 0 : 1;
    }
    తిరిగి 1;
}

/*
 * Return the height of the last window's statusline.
 */
    పూర్ణాంకం
చివరి_stl_ఎత్తు(
    పూర్ణాంకం		morewin)	// pretend there are two or more windows
{
    తిరిగి (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_కిటికీ)))
		? స్థితి_ఎత్తు_2 : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    పూర్ణాంకం
min_అడ్డువరుసలు(శూన్య)
{
    ఒకవేళ (firstwin == ఖాళీ)	// not initialized yet
	తిరిగి MIN_పంక్తులు;

    తిరిగి చట్రం_minheight(curtab->tp_topframe, ఖాళీ) + tabline_ఎత్తు()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    పూర్ణాంకం
min_అడ్డువరుసలు_for_all_tabpages(శూన్య)
{
    పూర్ణాంకం		మొత్తం;
    tabpage_T	*tp;
    పూర్ణాంకం		n;

    ఒకవేళ (firstwin == ఖాళీ)	// not initialized yet
	తిరిగి MIN_పంక్తులు;

    మొత్తం = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = చట్రం_minheight(tp->tp_topframe, ఖాళీ);
	ఒకవేళ (మొత్తం < n)
	    మొత్తం = n;
    }
    మొత్తం += tabline_ఎత్తు();
    మొత్తం += MIN_CMDHEIGHT;
    తిరిగి మొత్తం;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    పూర్ణాంకం
only_one_కిటికీ(శూన్య)
{
    పూర్ణాంకం		లెక్క = 0;
    win_T	*wp;

#ఒకవేళ defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    ఒకవేళ (popup_is_popup(curwin))
	తిరిగి FALSE;
#అయితే_ముగింపు

    // If there is another tab page there always is another window.
    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
	తిరిగి FALSE;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_బఫరు != ఖాళీ
		&& (!((bt_help(wp->w_బఫరు) && !bt_help(curbuf))
#నిర్వచితమైతే FEAT_QUICKFIX
		    || wp->w_p_pvw
#అయితే_ముగింపు
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++లెక్క;
    తిరిగి (లెక్క <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    స్థిర శూన్య
check_lnums_both(పూర్ణాంకం do_curwin, పూర్ణాంకం nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ ((do_curwin || wp != curwin) && wp->w_బఫరు == curbuf)
	{
	    పూర్ణాంకం need_adjust;

	    ఒకవేళ (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క;
	    ఒకవేళ (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    ఒకవేళ (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_పంక్తి_లెక్క;
	    ఒకవేళ (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    ఒకవేళ (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    శూన్య
check_lnums(పూర్ణాంకం do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    శూన్య
check_lnums_nested(పూర్ణాంకం do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    శూన్య
reset_lnums(శూన్య)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ (wp->w_బఫరు == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    ఒకవేళ (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    ఒకవేళ (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    ఒకవేళ (wp->w_save_cursor.w_topline_save > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
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
    పూర్ణాంకం
make_snapshot(పూర్ణాంకం idx)
{
    clear_snapshot(curtab, idx);
    ఒకవేళ (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
make_snapshot_rec(చట్రం_T *fr, చట్రం_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(చట్రం_T);
    ఒకవేళ (*frp == ఖాళీ)
	తిరిగి FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_వెడల్పు = fr->fr_వెడల్పు;
    (*frp)->fr_ఎత్తు = fr->fr_ఎత్తు;
    ఒకవేళ (fr->fr_తదుపరి != ఖాళీ)
    {
	ఒకవేళ (make_snapshot_rec(fr->fr_తదుపరి, &((*frp)->fr_తదుపరి)) == FAIL)
	    తిరిగి FAIL;
    }
    ఒకవేళ (fr->fr_బిడ్డ != ఖాళీ)
    {
	ఒకవేళ (make_snapshot_rec(fr->fr_బిడ్డ, &((*frp)->fr_బిడ్డ)) == FAIL)
	    తిరిగి FAIL;
    }
    ఒకవేళ (fr->fr_layout == FR_ఆకు && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    తిరిగి OK;
}

/*
 * Remove any existing snapshot.
 */
    స్థిర శూన్య
clear_snapshot(tabpage_T *tp, పూర్ణాంకం idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = ఖాళీ;
}

    స్థిర శూన్య
clear_snapshot_rec(చట్రం_T *fr)
{
    ఒకవేళ (fr == ఖాళీ)
	తిరిగి;
    clear_snapshot_rec(fr->fr_తదుపరి);
    clear_snapshot_rec(fr->fr_బిడ్డ);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    స్థిర win_T *
get_snapshot_curwin_rec(చట్రం_T *ft)
{
    win_T	*wp;

    ఒకవేళ (ft->fr_తదుపరి != ఖాళీ)
    {
	ఒకవేళ ((wp = get_snapshot_curwin_rec(ft->fr_తదుపరి)) != ఖాళీ)
	    తిరిగి wp;
    }
    ఒకవేళ (ft->fr_బిడ్డ != ఖాళీ)
    {
	ఒకవేళ ((wp = get_snapshot_curwin_rec(ft->fr_బిడ్డ)) != ఖాళీ)
	    తిరిగి wp;
    }

    తిరిగి ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    స్థిర win_T *
get_snapshot_curwin(పూర్ణాంకం idx)
{
    ఒకవేళ (curtab->tp_snapshot[idx] == ఖాళీ)
	తిరిగి ఖాళీ;

    తిరిగి get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    శూన్య
restore_snapshot(
    పూర్ణాంకం		idx,
    పూర్ణాంకం		close_curwin)	    // closing current window
{
    win_T	*wp;

    ఒకవేళ (curtab->tp_snapshot[idx] != ఖాళీ
	    && curtab->tp_snapshot[idx]->fr_వెడల్పు == topframe->fr_వెడల్పు
	    && curtab->tp_snapshot[idx]->fr_ఎత్తు == topframe->fr_ఎత్తు
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	ఒకవేళ (wp != ఖాళీ && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    స్థిర పూర్ణాంకం
check_snapshot_rec(చట్రం_T *sn, చట్రం_T *fr)
{
    ఒకవేళ (sn->fr_layout != fr->fr_layout
	    || (sn->fr_తదుపరి == ఖాళీ) != (fr->fr_తదుపరి == ఖాళీ)
	    || (sn->fr_బిడ్డ == ఖాళీ) != (fr->fr_బిడ్డ == ఖాళీ)
	    || (sn->fr_తదుపరి != ఖాళీ
		&& check_snapshot_rec(sn->fr_తదుపరి, fr->fr_తదుపరి) == FAIL)
	    || (sn->fr_బిడ్డ != ఖాళీ
		&& check_snapshot_rec(sn->fr_బిడ్డ, fr->fr_బిడ్డ) == FAIL)
	    || (sn->fr_win != ఖాళీ && !win_valid(sn->fr_win)))
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    స్థిర win_T *
restore_snapshot_rec(చట్రం_T *sn, చట్రం_T *fr)
{
    win_T	*wp = ఖాళీ;
    win_T	*wp2;

    fr->fr_ఎత్తు = sn->fr_ఎత్తు;
    fr->fr_వెడల్పు = sn->fr_వెడల్పు;
    ఒకవేళ (fr->fr_layout == FR_ఆకు)
    {
	చట్రం_new_ఎత్తు(fr, fr->fr_ఎత్తు, FALSE, FALSE, FALSE);
	చట్రం_new_వెడల్పు(fr, fr->fr_వెడల్పు, FALSE, FALSE);
	wp = sn->fr_win;
    }
    ఒకవేళ (sn->fr_తదుపరి != ఖాళీ)
    {
	wp2 = restore_snapshot_rec(sn->fr_తదుపరి, fr->fr_తదుపరి);
	ఒకవేళ (wp2 != ఖాళీ)
	    wp = wp2;
    }
    ఒకవేళ (sn->fr_బిడ్డ != ఖాళీ)
    {
	wp2 = restore_snapshot_rec(sn->fr_బిడ్డ, fr->fr_బిడ్డ);
	ఒకవేళ (wp2 != ఖాళీ)
	    wp = wp2;
    }
    తిరిగి wp;
}

#ఒకవేళ defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    పూర్ణాంకం
win_hasvertsplit(శూన్య)
{
    చట్రం_T	*fr;

    ఒకవేళ (topframe->fr_layout == FR_అడ్డువరుస)
	తిరిగి TRUE;

    ఒకవేళ (topframe->fr_layout == FR_నిలువు)
	FOR_ALL_చట్రాలు(fr, topframe->fr_బిడ్డ)
	    ఒకవేళ (fr->fr_layout == FR_అడ్డువరుస)
		తిరిగి TRUE;

    తిరిగి FALSE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    పూర్ణాంకం
get_win_సంఖ్య(win_T *wp, win_T *మొదటి_win)
{
    పూర్ణాంకం		i = 1;
    win_T	*w;

    కొరకు (w = మొదటి_win; w != ఖాళీ && w != wp; w = W_తదుపరి(w))
	++i;

    ఒకవేళ (w == ఖాళీ)
	తిరిగి 0;
    లేకపోతే
	తిరిగి i;
}

    పూర్ణాంకం
get_tab_సంఖ్య(tabpage_T *tp)
{
    పూర్ణాంకం		i = 1;
    tabpage_T	*t;

    కొరకు (t = మొదటి_tabpage; t != ఖాళీ && t != tp; t = t->tp_తదుపరి)
	++i;

    ఒకవేళ (t == ఖాళీ)
	తిరిగి 0;
    లేకపోతే
	తిరిగి i;
}
#అయితే_ముగింపు

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    స్థిర పూర్ణాంకం
చట్రం_check_ఎత్తు(చట్రం_T *topfrp, పూర్ణాంకం ఎత్తు)
{
    చట్రం_T *frp;

    ఒకవేళ (topfrp->fr_ఎత్తు != ఎత్తు)
	తిరిగి FALSE;

    ఒకవేళ (topfrp->fr_layout == FR_అడ్డువరుస)
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	    ఒకవేళ (frp->fr_ఎత్తు != ఎత్తు)
		తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    స్థిర పూర్ణాంకం
చట్రం_check_వెడల్పు(చట్రం_T *topfrp, పూర్ణాంకం వెడల్పు)
{
    చట్రం_T *frp;

    ఒకవేళ (topfrp->fr_వెడల్పు != వెడల్పు)
	తిరిగి FALSE;

    ఒకవేళ (topfrp->fr_layout == FR_నిలువు)
	FOR_ALL_చట్రాలు(frp, topfrp->fr_బిడ్డ)
	    ఒకవేళ (frp->fr_వెడల్పు != వెడల్పు)
		తిరిగి FALSE;

    తిరిగి TRUE;
}

#ఒకవేళ defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    స్థిర పూర్ణాంకం
int_cmp(స్థిరరాశి శూన్య *pa, స్థిరరాశి శూన్య *pb)
{
    స్థిరరాశి పూర్ణాంకం a = *(స్థిరరాశి పూర్ణాంకం *)pa;
    స్థిరరాశి పూర్ణాంకం b = *(స్థిరరాశి పూర్ణాంకం *)pb;
    ఒకవేళ (a > b)
	తిరిగి 1;
    ఒకవేళ (a < b)
	తిరిగి -1;
    తిరిగి 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    అక్షరం *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = ఖాళీలు_option;
    పూర్ణాంకం		tw;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		color_నిలువుల_సంఖ్య[256];
    పూర్ణాంకం		i;
    పూర్ణాంకం		j = 0;

    ఒకవేళ (wp != ఖాళీ && wp->w_బఫరు == ఖాళీ)
	తిరిగి ఖాళీ;  // buffer was closed

    ఒకవేళ (cc != ఖాళీ)
	s = cc;
    లేకపోతే ఒకవేళ (wp != ఖాళీ)
	s = wp->w_p_cc;

    ఒకవేళ (wp != ఖాళీ)
	tw = wp->w_బఫరు->b_p_tw;
    లేకపోతే
	// buffer-local value not set, assume zero
	tw = 0;

    అయితే (*s != NUL && లెక్క < 255)
    {
	ఒకవేళ (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    నిలువు = (*s == '-') ? -1 : 1;
	    ++s;
	    ఒకవేళ (!VIM_ISDIGIT(*s))
		తిరిగి e_invalid_వాదం;
	    నిలువు = నిలువు * getdigits(&s);
	    ఒకవేళ (tw == 0)
		వెళ్ళు skip;  // 'textwidth' not set, skip this item
	    నిలువు += tw;
	    ఒకవేళ (నిలువు < 0)
		వెళ్ళు skip;
	}
	లేకపోతే ఒకవేళ (VIM_ISDIGIT(*s))
	    నిలువు = getdigits(&s);
	లేకపోతే
	    తిరిగి e_invalid_వాదం;
	color_నిలువుల_సంఖ్య[లెక్క++] = నిలువు - 1;  // 1-based to 0-based
skip:
	ఒకవేళ (*s == NUL)
	    ఆపు;
	ఒకవేళ (*s != ',')
	    తిరిగి e_invalid_వాదం;
	ఒకవేళ (*++s == NUL)
	    తిరిగి e_invalid_వాదం;  // illegal trailing comma as in "set cc=80,"
    }

    ఒకవేళ (wp == ఖాళీ)
	తిరిగి ఖాళీ;  // only parse "cc"

    vim_free(wp->w_p_cc_నిలువుల_సంఖ్య);
    ఒకవేళ (లెక్క == 0)
	wp->w_p_cc_నిలువుల_సంఖ్య = ఖాళీ;
    లేకపోతే
    {
	wp->w_p_cc_నిలువుల_సంఖ్య = ALLOC_MULT(పూర్ణాంకం, లెక్క + 1);
	ఒకవేళ (wp->w_p_cc_నిలువుల_సంఖ్య != ఖాళీ)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_నిలువుల_సంఖ్య, లెక్క, పరిమాణం(పూర్ణాంకం), int_cmp);

	    కొరకు (i = 0; i < లెక్క; ++i)
		// skip duplicates
		ఒకవేళ (j == 0 || wp->w_p_cc_నిలువుల_సంఖ్య[j - 1] != color_నిలువుల_సంఖ్య[i])
		    wp->w_p_cc_నిలువుల_సంఖ్య[j++] = color_నిలువుల_సంఖ్య[i];
	    wp->w_p_cc_నిలువుల_సంఖ్య[j] = -1;  // end marker
	}
    }

    తిరిగి ఖాళీ;  // no error
}
#అయితే_ముగింపు

    పూర్ణాంకం
get_చివరి_winid(శూన్య)
{
    తిరిగి చివరి_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   పూర్ణాంకం
win_locked(win_T *wp)
{
    తిరిగి wp->w_locked;
}
