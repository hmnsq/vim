/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

static void cmd_with_count(var_na *cmd, char_u *bufp, size_t bufsize, long Prenum);
static void win_init(win_T *newp, win_T *oldp, ကိန်းပြည့် flags);
static void win_init_some(win_T *newp, win_T *oldp);
static void frame_comp_pos(frame_T *topfrp, ကိန်းပြည့် *row, ကိန်းပြည့် *col);
static void frame_setheight(frame_T *curfrp, ကိန်းပြည့် height);
static void frame_setwidth(frame_T *curfrp, ကိန်းပြည့် width);
static void win_exchange(long);
static void win_rotate(ကိန်းပြည့်, ကိန်းပြည့်);
static void win_equal_rec(win_T *next_curwin, ကိန်းပြည့် current, frame_T *topfr, ကိန်းပြည့် dir, ကိန်းပြည့် col, ကိန်းပြည့် row, ကိန်းပြည့် width, ကိန်းပြည့် height);
static void trigger_winnewpre(void);
static void trigger_winclosed(win_T *win);
static win_T *win_free_mem(win_T *win, ကိန်းပြည့် *dirp, tabpage_T *tp);
static frame_T *win_altframe(win_T *win, tabpage_T *tp);
static tabpage_T *alt_tabpage(void);
static win_T *frame2win(frame_T *frp);
static ကိန်းပြည့် frame_has_win(frame_T *frp, win_T *wp);
static void win_fix_scroll(ကိန်းပြည့် resize);
static void win_fix_cursor(ကိန်းပြည့် normal);
static void frame_new_height(frame_T *topfrp, ကိန်းပြည့် height, ကိန်းပြည့် topfirst, ကိန်းပြည့် wfh, ကိန်းပြည့် set_ch);
static ကိန်းပြည့် frame_fixed_height(frame_T *frp);
static ကိန်းပြည့် frame_fixed_width(frame_T *frp);
static void frame_add_statusline(frame_T *frp);
static void frame_new_width(frame_T *topfrp, ကိန်းပြည့် width, ကိန်းပြည့် leftfirst, ကိန်းပြည့် wfw);
static void frame_set_vsep(frame_T *frp, ကိန်းပြည့် add);
static ကိန်းပြည့် frame_minwidth(frame_T *topfrp, win_T *next_curwin);
static void frame_fix_width(win_T *wp);
static ကိန်းပြည့် win_alloc_firstwin(win_T *oldwin);
static void new_frame(win_T *wp);
static tabpage_T *alloc_tabpage(void);
static ကိန်းပြည့် leave_tabpage(buf_T *new_curbuf, ကိန်းပြည့် trigger_leave_autocmds);
static void enter_tabpage(tabpage_T *tp, buf_T *old_curbuf, ကိန်းပြည့် trigger_enter_autocmds, ကိန်းပြည့် trigger_leave_autocmds);
static void frame_fix_height(win_T *wp);
static ကိန်းပြည့် frame_minheight(frame_T *topfrp, win_T *next_curwin);
static ကိန်းပြည့် frame_wincount_in_height(frame_T *topfrp);
static ကိန်းပြည့် may_open_tabpage(void);
static ကိန်းပြည့် win_enter_ext(win_T *wp, ကိန်းပြည့် flags);
static void win_free(win_T *wp, tabpage_T *tp);
static void win_append(win_T *after, win_T *wp);
static void frame_append(frame_T *after, frame_T *frp);
static void frame_insert(frame_T *before, frame_T *frp);
static void frame_remove(frame_T *frp);
static void win_goto_ver(ကိန်းပြည့် up, long count);
static void win_goto_hor(ကိန်းပြည့် left, long count);
static void frame_add_height(frame_T *frp, ကိန်းပြည့် n);
static void last_status_rec(frame_T *fr, ကိန်းပြည့် statusline);
#အကယ်၍ defined(FEAT_STL_OPT)
static ကိန်းပြည့် frame_find_global_stlh_rec(frame_T *frp, ကိန်းပြည့် h);
static void frame_change_statusline_height_rec(frame_T *frp, ကိန်းပြည့် global_stlh);
#endif
static void frame_flatten(frame_T *frp);
static void winframe_restore(win_T *wp, ကိန်းပြည့် dir, frame_T *unflat_altfr);

static ကိန်းပြည့် make_snapshot_rec(frame_T *fr, frame_T **frp);
static void clear_snapshot(tabpage_T *tp, ကိန်းပြည့် idx);
static void clear_snapshot_rec(frame_T *fr);
static ကိန်းပြည့် check_snapshot_rec(frame_T *sn, frame_T *fr);
static win_T *restore_snapshot_rec(frame_T *sn, frame_T *fr);
static win_T *get_snapshot_curwin(ကိန်းပြည့် idx);

static ကိန်းပြည့် frame_check_height(frame_T *topfrp, ကိန်းပြည့် height);
static ကိန်းပြည့် frame_check_width(frame_T *topfrp, ကိန်းပြည့် width);

static win_T *win_alloc(win_T *after, ကိန်းပြည့် hidden);

#define NOWIN		((win_T *)-1)	// non-existing window

#define ROWS_AVAIL (Rows - p_ch - tabline_height())

// flags for win_enter_ext()
#define WEE_UNDO_SYNC			0x01
#define WEE_CURWIN_INVALID		0x02
#define WEE_TRIGGER_NEW_AUTOCMDS	0x04
#define WEE_TRIGGER_ENTER_AUTOCMDS	0x08
#define WEE_TRIGGER_LEAVE_AUTOCMDS	0x10
#define WEE_ALLOW_PARSE_MESSAGES	0x20

static var_na *m_onlyone = N_("Already only one window");

// When non-zero splitting a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
static ကိန်းပြည့် split_disallowed = 0;

// When non-zero closing a window is forbidden.  Used to avoid that nasty
// autocommands mess up the window structure.
static ကိန်းပြည့် close_disallowed = 0;

// When non-zero changing the window frame structure is forbidden.  Used
// to avoid that winframe_remove() is called recursively
static ကိန်းပြည့် frame_locked = 0;

#အကယ်၍ defined(FEAT_STL_OPT)
// rendered height of global 'stl' (number of "%@" + 1)
static ကိန်းပြည့် stl_rendered_height = 1;
static ကိန်းပြည့် stlo_fh = FALSE;	    // "fixedheight" flag cache of global 'stlo'
static ကိန်းပြည့် stlo_mh = STATUS_HEIGHT; // "maxheight" value cache of global 'stlo'
#endif

/*
 * Disallow changing the window layout (split window, close window, move
 * window).  Resizing is still allowed.
 * Used for autocommands that temporarily use another window and need to
 * make sure the previously selected window is still there.
 * Must be matched with exactly one call to window_layout_unlock()!
 */
    void
window_layout_lock(void)
{
    ++split_disallowed;
    ++close_disallowed;
}

    void
window_layout_unlock(void)
{
    --split_disallowed;
    --close_disallowed;
}

    ကိန်းပြည့်
frames_locked(void)
{
    ပြန်ပေး frame_locked;
}

/*
 * When the window layout cannot be changed give an error and return TRUE.
 * "cmd" indicates the action being performed and is used to pick the relevant
 * error message.  When closing window(s) and the command isn't easy to know,
 * passing CMD_SIZE will also work.
 */
    ကိန်းပြည့်
window_layout_locked(ga_nanaa CMD_index cmd)
{
    အကယ်၍ (split_disallowed > 0 || close_disallowed > 0)
    {
	အကယ်၍ (close_disallowed == 0 && cmd == CMD_tabnew)
	    emsg(_(e_cannot_split_window_when_closing_buffer));
	မဟုတ်ပါက
	    emsg(_(e_not_allowed_to_change_window_layout_in_this_autocmd));
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

// #define WIN_DEBUG
#ifdef WIN_DEBUG
/*
 * Call this method to log the current window layout.
 */
    static void
log_frame_layout(frame_T *frame)
{
    ch_log(NULL, "layout %s, wi: %d, he: %d, wwi: %d, whe: %d, id: %d",
	    frame->fr_layout == FR_LEAF ? "LEAF"
				  : frame->fr_layout == FR_ROW ? "ROW" : "COL",
	    frame->fr_width,
	    frame->fr_height,
	    frame->fr_win == NULL ? -1 : frame->fr_win->w_width,
	    frame->fr_win == NULL ? -1 : frame->fr_win->w_height,
	    frame->fr_win == NULL ? -1 : frame->fr_win->w_id);
    အကယ်၍ (frame->fr_child != NULL)
    {
	ch_log(NULL, "children");
	log_frame_layout(frame->fr_child);
	အကယ်၍ (frame->fr_next != NULL)
	    ch_log(NULL, "END of children");
    }
    အကယ်၍ (frame->fr_next != NULL)
	log_frame_layout(frame->fr_next);
}
#endif

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', this function will return FALSE.
 */
    ကိန်းပြည့်
check_can_set_curbuf_disabled(void)
{
    အကယ်၍ (curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	ပြန်ပေး FALSE;
    }
    ပြန်ပေး TRUE;
}

/*
 * Check if the current window is allowed to move to a different buffer.
 * If the window has 'winfixbuf', then forceit must be TRUE or this function
 * will return FALSE.
 */
    ကိန်းပြည့်
check_can_set_curbuf_forceit(ကိန်းပြည့် forceit)
{
    အကယ်၍ (!forceit && curwin->w_p_wfb)
    {
	emsg(_(e_winfixbuf_cannot_go_to_buffer));
	ပြန်ပေး FALSE;
    }
    ပြန်ပေး TRUE;
}

/*
 * Return the current window, unless in the cmdline window and "prevwin" is
 * set, then return "prevwin".
 */
    win_T *
prevwin_curwin(void)
{
    // In cmdwin, the alternative buffer should be used.
    ပြန်ပေး is_in_cmdwin() && prevwin != NULL ? prevwin : curwin;
}

/*
 * If the 'switchbuf' option contains "useopen" or "usetab", then try to jump
 * to a window containing "buf".
 * Returns the pointer to the window that was jumped to or NULL.
 */
    win_T *
swbuf_goto_win_with_buf(buf_T *buf)
{
    win_T   *wp = NULL;

    အကယ်၍ (buf == NULL)
	ပြန်ပေး wp;

    // If 'switchbuf' contains "useopen": jump to first window in the current
    // tab page containing "buf" if one exists.
    အကယ်၍ (swb_flags & SWB_USEOPEN)
	wp = buf_jump_open_win(buf);

    // If 'switchbuf' contains "usetab": jump to first window in any tab page
    // containing "buf" if one exists.
    အကယ်၍ (wp == NULL && (swb_flags & SWB_USETAB))
	wp = buf_jump_open_tab(buf);

    ပြန်ပေး wp;
}

/*
 * All CTRL-W window commands are handled here, called from normal_cmd().
 */
    void
do_window(
    ကိန်းပြည့်		nchar,
    long	Prenum,
    ကိန်းပြည့်		xchar)	    // extra char from ":wincmd gx" or NUL
{
    long	Prenum1;
    win_T	*wp;
    char_u	*ptr;
    linenr_T    lnum = -1;
#ifdef FEAT_FIND_ID
    ကိန်းပြည့်		type = FIND_DEFINE;
    ကိန်းပြည့်		len;
#endif
    char_u	cbuf[40];

    အကယ်၍ (ERROR_IF_ANY_POPUP_WINDOW)
	ပြန်ပေး;

#define CHECK_CMDWIN \
    do { \
	အကယ်၍ (cmdwin_type != 0) \
	{ \
	    emsg(_(e_invalid_in_cmdline_window)); \
	    ပြန်ပေး; \
	} \
    } နေစဉ် (0)

    Prenum1 = Prenum == 0 ? 1 : Prenum;

    switch (nchar)
    {
// split current window in two parts, horizontally
    pak_sa 'S':
    pak_sa Ctrl_S:
    pak_sa 's':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		အကယ်၍ (bt_quickfix(curbuf))
		    goto newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(void)win_split((ကိန်းပြည့်)Prenum, 0);
		bha_nga;

// split current window in two parts, vertically
    pak_sa Ctrl_V:
    pak_sa 'v':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		// When splitting the quickfix window open a new buffer in it,
		// don't replicate the quickfix buffer.
		အကယ်၍ (bt_quickfix(curbuf))
		    goto newwindow;
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		(void)win_split((ကိန်းပြည့်)Prenum, WSP_VERT);
		bha_nga;

// split current window and edit alternate file
    pak_sa Ctrl_HAT:
    pak_sa '^':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode

		အကယ်၍ (buflist_findnr(Prenum == 0
					? curwin->w_alt_fnum : Prenum) == NULL)
		{
		    အကယ်၍ (Prenum == 0)
			emsg(_(e_no_alternate_file));
		    မဟုတ်ပါက
			semsg(_(e_buffer_nr_not_found), Prenum);
		    bha_nga;
		}

		အကယ်၍ (!curbuf_locked() && win_split(0, 0) == OK)
		    (void)buflist_getfile(
			    Prenum == 0 ? curwin->w_alt_fnum : Prenum,
			    (linenr_T)0, GETF_ALT, FALSE);
		bha_nga;

// open new window
    pak_sa Ctrl_N:
    pak_sa 'n':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
newwindow:
		အကယ်၍ (Prenum)
		    // window height
		    vim_snprintf((var_na *)cbuf, parimaa_na(cbuf) - 5, "%ld", Prenum);
		မဟုတ်ပါက
		    cbuf[0] = NUL;
#အကယ်၍ defined(FEAT_QUICKFIX)
		အကယ်၍ (nchar == 'v' || nchar == Ctrl_V)
		    STRCAT(cbuf, "v");
#endif
		STRCAT(cbuf, "new");
		do_cmdline_cmd(cbuf);
		bha_nga;

// quit current window
    pak_sa Ctrl_Q:
    pak_sa 'q':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_count("quit", cbuf, parimaa_na(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		bha_nga;

// close current window
    pak_sa Ctrl_C:
    pak_sa 'c':
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_count("close", cbuf, parimaa_na(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		bha_nga;

#အကယ်၍ defined(FEAT_QUICKFIX)
// close preview window
    pak_sa Ctrl_Z:
    pak_sa 'z':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		do_cmdline_cmd((char_u *)"pclose");
		bha_nga;

// cursor to preview window
    pak_sa 'P':
		FOR_ALL_WINDOWS(wp)
		    အကယ်၍ (wp->w_p_pvw)
			bha_nga;
		အကယ်၍ (wp == NULL)
		    emsg(_(e_there_is_no_preview_window));
		မဟုတ်ပါက
		    win_goto(wp);
		bha_nga;
#endif

// close all but current window
    pak_sa Ctrl_O:
    pak_sa 'o':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		cmd_with_count("only", cbuf, parimaa_na(cbuf), Prenum);
		do_cmdline_cmd(cbuf);
		bha_nga;

// cursor to next window with wrap around
    pak_sa Ctrl_W:
    pak_sa 'w':
// cursor to previous window with wrap around
    pak_sa 'W':
		CHECK_CMDWIN;
		အကယ်၍ (ONE_WINDOW && Prenum != 1)	// just one window
		    beep_flush();
		မဟုတ်ပါက
		{
		    အကယ်၍ (Prenum)			// go to specified window
		    {
			အတွက် (wp = firstwin; --Prenum > 0; )
			{
			    အကယ်၍ (wp->w_next == NULL)
				bha_nga;
			    မဟုတ်ပါက
				wp = wp->w_next;
			}
		    }
		    မဟုတ်ပါက
		    {
			အကယ်၍ (nchar == 'W')	    // go to previous window
			{
			    wp = curwin->w_prev;
			    အကယ်၍ (wp == NULL)
				wp = lastwin;	    // wrap around
			}
			မဟုတ်ပါက			    // go to next window
			{
			    wp = curwin->w_next;
			    အကယ်၍ (wp == NULL)
				wp = firstwin;	    // wrap around
			}
		    }
		    win_goto(wp);
		}
		bha_nga;

// cursor to window below
    pak_sa 'j':
    pak_sa K_DOWN:
    pak_sa Ctrl_J:
		CHECK_CMDWIN;
		win_goto_ver(FALSE, Prenum1);
		bha_nga;

// cursor to window above
    pak_sa 'k':
    pak_sa K_UP:
    pak_sa Ctrl_K:
		CHECK_CMDWIN;
		win_goto_ver(TRUE, Prenum1);
		bha_nga;

// cursor to left window
    pak_sa 'h':
    pak_sa K_LEFT:
    pak_sa Ctrl_H:
    pak_sa K_BS:
		CHECK_CMDWIN;
		win_goto_hor(TRUE, Prenum1);
		bha_nga;

// cursor to right window
    pak_sa 'l':
    pak_sa K_RIGHT:
    pak_sa Ctrl_L:
		CHECK_CMDWIN;
		win_goto_hor(FALSE, Prenum1);
		bha_nga;

// move window to new tab page
    pak_sa 'T':
		CHECK_CMDWIN;
		အကယ်၍ (one_window())
		    msg(_(m_onlyone));
		မဟုတ်ပါက
		{
		    tabpage_T	*oldtab = curtab;
		    tabpage_T	*newtab;

		    // First create a new tab with the window, then go back to
		    // the old tab and close the window there.
		    wp = curwin;
		    အကယ်၍ (win_new_tabpage((ကိန်းပြည့်)Prenum) == OK
						     && valid_tabpage(oldtab))
		    {
			newtab = curtab;
			goto_tabpage_tp(oldtab, TRUE, TRUE);
			အကယ်၍ (curwin == wp)
			    win_close(curwin, FALSE);
			အကယ်၍ (valid_tabpage(newtab))
			    goto_tabpage_tp(newtab, TRUE, TRUE);
		    }
		}
		bha_nga;

// cursor to top-left window
    pak_sa 't':
    pak_sa Ctrl_T:
		win_goto(firstwin);
		bha_nga;

// cursor to bottom-right window
    pak_sa 'b':
    pak_sa Ctrl_B:
		win_goto(lastwin);
		bha_nga;

// cursor to last accessed (previous) window
    pak_sa 'p':
    pak_sa Ctrl_P:
		အကယ်၍ (!win_valid(prevwin))
		    beep_flush();
		မဟုတ်ပါက
		    win_goto(prevwin);
		bha_nga;

// exchange current and next window
    pak_sa 'x':
    pak_sa Ctrl_X:
		CHECK_CMDWIN;
		win_exchange(Prenum);
		bha_nga;

// rotate windows downwards
    pak_sa Ctrl_R:
    pak_sa 'r':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(FALSE, (ကိန်းပြည့်)Prenum1);    // downwards
		bha_nga;

// rotate windows upwards
    pak_sa 'R':
		CHECK_CMDWIN;
		reset_VIsual_and_resel();	// stop Visual mode
		win_rotate(TRUE, (ကိန်းပြည့်)Prenum1);	    // upwards
		bha_nga;

// move window to the very top/bottom/left/right
    pak_sa 'K':
    pak_sa 'J':
    pak_sa 'H':
    pak_sa 'L':
		CHECK_CMDWIN;
		အကယ်၍ (ONE_WINDOW)
		    beep_flush();
		မဟုတ်ပါက
		{
		    ကိန်းပြည့် dir = ((nchar == 'H' || nchar == 'L') ? WSP_VERT : 0)
			 | ((nchar == 'H' || nchar == 'K') ? WSP_TOP : WSP_BOT);

		    (void)win_splitmove(curwin, (ကိန်းပြည့်)Prenum, dir);
		}
		bha_nga;

// make all windows the same width and/or height
    pak_sa '=':
		{
		    ကိန်းပြည့် mod = cmdmod.cmod_split & (WSP_VERT | WSP_HOR);
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_equal(NULL, FALSE,
			   mod == WSP_VERT ? 'v' : mod == WSP_HOR ? 'h' : 'b');
		}
		bha_nga;

// increase current window height
    pak_sa '+':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height + (ကိန်းပြည့်)Prenum1);
		bha_nga;

// decrease current window height
    pak_sa '-':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(curwin->w_height - (ကိန်းပြည့်)Prenum1);
		bha_nga;

// set current window height
    pak_sa Ctrl__:
    pak_sa '_':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight(Prenum ? (ကိန်းပြည့်)Prenum : 9999);
		bha_nga;

// increase current window width
    pak_sa '>':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width + (ကိန်းပြည့်)Prenum1);
		bha_nga;

// decrease current window width
    pak_sa '<':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(curwin->w_width - (ကိန်းပြည့်)Prenum1);
		bha_nga;

// set current window width
    pak_sa '|':
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setwidth(Prenum != 0 ? (ကိန်းပြည့်)Prenum : 9999);
		bha_nga;

// jump to tag and split window if tag exists (in preview window)
#အကယ်၍ defined(FEAT_QUICKFIX)
    pak_sa '}':
		CHECK_CMDWIN;
		အကယ်၍ (Prenum)
		    g_do_tagpreview = Prenum;
		မဟုတ်ပါက
		    g_do_tagpreview = p_pvh;
#endif
		// FALLTHROUGH
    pak_sa ']':
    pak_sa Ctrl_RSB:
		CHECK_CMDWIN;
		// keep Visual mode, can select words to use as a tag
		အကယ်၍ (Prenum)
		    postponed_split = Prenum;
		မဟုတ်ပါက
		    postponed_split = -1;
#ifdef FEAT_QUICKFIX
		အကယ်၍ (nchar != '}')
		    g_do_tagpreview = 0;
#endif

		// Execute the command right here, required when "wincmd ]"
		// was used in a function.
		do_nv_ident(Ctrl_RSB, NUL);
		postponed_split = 0;
		bha_nga;

// edit file name under cursor in a new window
    pak_sa 'f':
    pak_sa 'F':
    pak_sa Ctrl_F:
wingotofile:
		CHECK_CMDWIN;
		အကယ်၍ (check_text_or_curbuf_locked(NULL))
		    bha_nga;

		ptr = grab_file_name(Prenum1, &lnum);
		အကယ်၍ (ptr != NULL)
		{
		    tabpage_T	*oldtab = curtab;
		    win_T	*oldwin = curwin;
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    setpcmark();

		    // If 'switchbuf' is set to 'useopen' or 'usetab' and the
		    // file is already opened in a window, then jump to it.
		    wp = NULL;
		    အကယ်၍ ((swb_flags & (SWB_USEOPEN | SWB_USETAB))
						&& cmdmod.cmod_tab == 0)
			wp = swbuf_goto_win_with_buf(buflist_findname_exp(ptr));

		    အကယ်၍ (wp == NULL && win_split(0, 0) == OK)
		    {
			RESET_BINDING(curwin);
			အကယ်၍ (do_ecmd(0, ptr, NULL, NULL, ECMD_LASTL,
						   ECMD_HIDE, NULL) == FAIL)
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
			မဟုတ်ပါက
			    wp = curwin;
		    }

		    အကယ်၍ (wp != NULL && nchar == 'F' && lnum >= 0)
		    {
			curwin->w_cursor.lnum = lnum;
			check_cursor_lnum();
			beginline(BL_SOL | BL_FIX);
		    }
		    vim_free(ptr);
		}
		bha_nga;

#ifdef FEAT_FIND_ID
// Go to the first occurrence of the identifier under cursor along path in a
// new window -- webb
    pak_sa 'i':			    // Go to any match
    pak_sa Ctrl_I:
		type = FIND_ANY;
		// FALLTHROUGH
    pak_sa 'd':			    // Go to definition, using 'define'
    pak_sa Ctrl_D:
		CHECK_CMDWIN;
		အကယ်၍ ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
		    bha_nga;

		// Make a copy, if the line was changed it will be freed.
		ptr = vim_strnsave(ptr, len);
		အကယ်၍ (ptr == NULL)
		    bha_nga;

		find_pattern_in_path(ptr, 0, len, TRUE,
			Prenum == 0 ? TRUE : FALSE, type,
			Prenum1, ACTION_SPLIT, (linenr_T)1, (linenr_T)MAXLNUM,
			FALSE, FALSE);
		vim_free(ptr);
		curwin->w_set_curswant = true;
		bha_nga;
#endif

// Quickfix window only: view the result under the cursor in a new split.
#အကယ်၍ defined(FEAT_QUICKFIX)
    pak_sa K_KENTER:
    pak_sa CAR:
		အကယ်၍ (bt_quickfix(curbuf))
		    qf_view_result(TRUE);
		bha_nga;
#endif

// CTRL-W g  extended commands
    pak_sa 'g':
    pak_sa Ctrl_G:
		CHECK_CMDWIN;
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;		// disallow scrolling here
#endif
		++no_mapping;
		++allow_keys;   // no mapping for xchar, but allow key codes
		အကယ်၍ (xchar == NUL)
		    xchar = plain_vgetc();
		LANGMAP_ADJUST(xchar, TRUE);
		--no_mapping;
		--allow_keys;
		(void)add_to_showcmd(xchar);

		switch (xchar)
		{
#အကယ်၍ defined(FEAT_QUICKFIX)
		    pak_sa '}':
			xchar = Ctrl_RSB;
			အကယ်၍ (Prenum)
			    g_do_tagpreview = Prenum;
			မဟုတ်ပါက
			    g_do_tagpreview = p_pvh;
#endif
			// FALLTHROUGH
		    pak_sa ']':
		    pak_sa Ctrl_RSB:
			// keep Visual mode, can select words to use as a tag
			အကယ်၍ (Prenum)
			    postponed_split = Prenum;
			မဟုတ်ပါက
			    postponed_split = -1;

			// Execute the command right here, required when
			// "wincmd g}" was used in a function.
			do_nv_ident('g', xchar);
			postponed_split = 0;
			bha_nga;

		    pak_sa 'f':	    // CTRL-W gf: "gf" in a new tab page
		    pak_sa 'F':	    // CTRL-W gF: "gF" in a new tab page
			cmdmod.cmod_tab = tabpage_index(curtab) + 1;
			nchar = xchar;
			goto wingotofile;

		    pak_sa 't':	    // CTRL-W gt: go to next tab page
			goto_tabpage((ကိန်းပြည့်)Prenum);
			bha_nga;

		    pak_sa 'T':	    // CTRL-W gT: go to previous tab page
			goto_tabpage(-(ကိန်းပြည့်)Prenum1);
			bha_nga;

		    pak_sa TAB:	    // CTRL-W g<Tab>: go to last used tab page
			အကယ်၍ (goto_tabpage_lastused() == FAIL)
			    beep_flush();
			bha_nga;

		    default:
			beep_flush();
			bha_nga;
		}
		bha_nga;

    default:	beep_flush();
		bha_nga;
    }
}

/*
 * Figure out the address type for ":wincmd".
 */
    void
get_wincmd_addr_type(char_u *arg, exarg_T *eap)
{
    switch (*arg)
    {
    pak_sa 'S':
    pak_sa Ctrl_S:
    pak_sa 's':
    pak_sa Ctrl_N:
    pak_sa 'n':
    pak_sa 'j':
    pak_sa Ctrl_J:
    pak_sa 'k':
    pak_sa Ctrl_K:
    pak_sa 'T':
    pak_sa Ctrl_R:
    pak_sa 'r':
    pak_sa 'R':
    pak_sa 'K':
    pak_sa 'J':
    pak_sa '+':
    pak_sa '-':
    pak_sa Ctrl__:
    pak_sa '_':
    pak_sa '|':
    pak_sa ']':
    pak_sa Ctrl_RSB:
    pak_sa 'g':
    pak_sa Ctrl_G:
    pak_sa Ctrl_V:
    pak_sa 'v':
    pak_sa 'h':
    pak_sa Ctrl_H:
    pak_sa 'l':
    pak_sa Ctrl_L:
    pak_sa 'H':
    pak_sa 'L':
    pak_sa '>':
    pak_sa '<':
#အကယ်၍ defined(FEAT_QUICKFIX)
    pak_sa '}':
#endif
    pak_sa 'f':
    pak_sa 'F':
    pak_sa Ctrl_F:
#ifdef FEAT_FIND_ID
    pak_sa 'i':
    pak_sa Ctrl_I:
    pak_sa 'd':
    pak_sa Ctrl_D:
#endif
		// window size or any count
		eap->addr_type = ADDR_OTHER;
		bha_nga;

    pak_sa Ctrl_HAT:
    pak_sa '^':
		// buffer number
		eap->addr_type = ADDR_BUFFERS;
		bha_nga;

    pak_sa Ctrl_Q:
    pak_sa 'q':
    pak_sa Ctrl_C:
    pak_sa 'c':
    pak_sa Ctrl_O:
    pak_sa 'o':
    pak_sa Ctrl_W:
    pak_sa 'w':
    pak_sa 'W':
    pak_sa 'x':
    pak_sa Ctrl_X:
		// window number
		eap->addr_type = ADDR_WINDOWS;
		bha_nga;

#အကယ်၍ defined(FEAT_QUICKFIX)
    pak_sa Ctrl_Z:
    pak_sa 'z':
    pak_sa 'P':
#endif
    pak_sa 't':
    pak_sa Ctrl_T:
    pak_sa 'b':
    pak_sa Ctrl_B:
    pak_sa 'p':
    pak_sa Ctrl_P:
    pak_sa '=':
    pak_sa CAR:
		// no count
		eap->addr_type = ADDR_NONE;
		bha_nga;
    }
}

    static void
cmd_with_count(
    var_na	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    အကယ်၍ (Prenum > 0)
	vim_snprintf((var_na *)bufp, bufsize, "%s %ld", cmd, Prenum);
    မဟုတ်ပါက
	STRCPY(bufp, cmd);
}

/*
 * If "split_disallowed" is set, or "wp"'s buffer is closing, give an error and
 * return FAIL.  Otherwise return OK.
 */
    ကိန်းပြည့်
check_split_disallowed(win_T *wp)
{
    အကယ်၍ (split_disallowed > 0)
    {
	emsg(_(e_cant_split_window_while_closing_another));
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (wp->w_buffer->b_locked_split)
    {
	emsg(_(e_cannot_split_window_when_closing_buffer));
	ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
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
    ကိန်းပြည့်
win_split(ကိန်းပြည့် size, ကိန်းပြည့် flags)
{
    အကယ်၍ (ERROR_IF_ANY_POPUP_WINDOW)
	ပြန်ပေး FAIL;

    အကယ်၍ (check_split_disallowed(curwin) == FAIL)
	ပြန်ပေး FAIL;

    // When the ":tab" modifier was used open a new tab page instead.
    အကယ်၍ (may_open_tabpage() == OK)
	ပြန်ပေး OK;

    // Add flags from ":vertical", ":topleft" and ":botright".
    flags |= cmdmod.cmod_split;
    အကယ်၍ ((flags & WSP_TOP) && (flags & WSP_BOT))
    {
	emsg(_(e_cant_split_topleft_and_botright_at_the_same_time));
	ပြန်ပေး FAIL;
    }

    // When creating the help window make a snapshot of the window layout.
    // Otherwise clear the snapshot, it's now invalid.
    အကယ်၍ (flags & WSP_HELP)
	make_snapshot(SNAP_HELP_IDX);
    မဟုတ်ပါက
	clear_snapshot(curtab, SNAP_HELP_IDX);

    အကယ်၍ (flags & WSP_QUICKFIX)
	make_snapshot(SNAP_QUICKFIX_IDX);
    မဟုတ်ပါက
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    ပြန်ပေး win_split_ins(size, flags, NULL, 0, NULL);
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
    ကိန်းပြည့်
win_split_ins(
    ကိန်းပြည့်		size,
    ကိန်းပြည့်		flags,
    win_T	*new_wp,
    ကိန်းပြည့်		dir,
    frame_T	*to_flatten)
{
    win_T	*wp = new_wp;
    win_T	*oldwin;
    ကိန်းပြည့်		new_size = size;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		need_status = 0;
    ကိန်းပြည့်		do_equal = FALSE;
    ကိန်းပြည့်		needed;
    ကိန်းပြည့်		available;
    ကိန်းပြည့်		oldwin_height = 0;
    ကိန်းပြည့်		layout;
    frame_T	*frp, *curfrp, *frp2, *prevfrp;
    ကိန်းပြည့်		before;
    ကိန်းပြည့်		minheight;
    ကိန်းပြည့်		wmh1;
    ကိန်းပြည့်		did_set_fraction = FALSE;
    ကိန်းပြည့်		retval = FAIL;

    // Do not redraw here, curwin->w_buffer may be invalid.
    ++RedrawingDisabled;

    အကယ်၍ (new_wp == NULL)
	trigger_winnewpre();

    အကယ်၍ (flags & WSP_TOP)
	oldwin = firstwin;
    မဟုတ်ပါက အကယ်၍ (flags & WSP_BOT)
	oldwin = lastwin;
    မဟုတ်ပါက
	oldwin = curwin;

    // add a status line when p_ls == 1 and splitting the first window
    အကယ်၍ (ONE_WINDOW && p_ls == 1 && oldwin->w_status_height == 0)
    {
	အကယ်၍ (!(flags & WSP_FORCE_ROOM) && VISIBLE_HEIGHT(oldwin) <= p_wmh)
	{
	    emsg(_(e_not_enough_room));
	    goto theend;
	}
	need_status = statusline_height(oldwin);
    }

#ifdef FEAT_GUI
    // May be needed for the scrollbars that are going to change.
    အကယ်၍ (gui.in_use)
	out_flush();
#endif

    အကယ်၍ (flags & WSP_VERT)
    {
	ကိန်းပြည့်	wmw1;
	ကိန်းပြည့်	minwidth;

	layout = FR_ROW;

	/*
	 * Check if we are able to split the current window and compute its
	 * width.
	 */
	// Current window requires at least 1 space.
	wmw1 = (p_wmw == 0 ? 1 : p_wmw);
	needed = wmw1 + 1;
	အကယ်၍ (flags & WSP_ROOM)
	    needed += p_wiw - wmw1;
	အကယ်၍ (flags & (WSP_BOT | WSP_TOP))
	{
	    minwidth = frame_minwidth(topframe, NOWIN);
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	မဟုတ်ပါက အကယ်၍ (p_ea)
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    prevfrp = oldwin->w_frame;
	    အတွက် (frp = oldwin->w_frame->fr_parent; frp != NULL;
							frp = frp->fr_parent)
	    {
		အကယ်၍ (frp->fr_layout == FR_ROW)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			အကယ်၍ (frp2 != prevfrp)
			    minwidth += frame_minwidth(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_width;
	    needed += minwidth;
	}
	မဟုတ်ပါက
	{
	    minwidth = frame_minwidth(oldwin->w_frame, NOWIN);
	    available = oldwin->w_frame->fr_width;
	    needed += minwidth;
	}
	အကယ်၍ (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    goto theend;
	}
	အကယ်၍ (new_size == 0)
	    new_size = oldwin->w_width / 2;
	အကယ်၍ (new_size > available - minwidth - 1)
	    new_size = available - minwidth - 1;
	အကယ်၍ (new_size < wmw1)
	    new_size = wmw1;

	// if it doesn't fit in the current window, need win_equal()
	အကယ်၍ (oldwin->w_width - new_size - 1 < p_wmw)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixwidth' window.  Take them from a window to the left or right
	// instead, if possible. Add one for the separator.
	အကယ်၍ (oldwin->w_p_wfw)
	    win_setwidth_win(oldwin->w_width + new_size + 1, oldwin);

	// Make all windows the same width if 'equalalways' is set, 'ead'
	// not set to 'v', a size was not provided and there is a parent frame.
	အကယ်၍ (!do_equal && p_ea && size == 0 && *p_ead != 'v'
	    && oldwin->w_frame->fr_parent != NULL)
	    do_equal = TRUE;
    }
    မဟုတ်ပါက
    {
	layout = FR_COL;

	/*
	 * Check if we are able to split the current window and compute its
	 * height.
	 */
	// Current window requires at least 1 space.
	wmh1 = (p_wmh == 0 ? 1 : p_wmh) + WINBAR_HEIGHT(curwin);
	needed = wmh1 + statusline_height(oldwin);
	အကယ်၍ (flags & WSP_ROOM)
	    needed += p_wh - wmh1;
	အကယ်၍ (flags & (WSP_BOT | WSP_TOP))
	{
	    minheight = frame_minheight(topframe, NOWIN) + need_status;
	    available = topframe->fr_height;
	    needed += minheight;
	}
	မဟုတ်ပါက အကယ်၍ (p_ea)
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    prevfrp = oldwin->w_frame;
	    အတွက် (frp = oldwin->w_frame->fr_parent; frp != NULL;
							frp = frp->fr_parent)
	    {
		အကယ်၍ (frp->fr_layout == FR_COL)
		    FOR_ALL_FRAMES(frp2, frp->fr_child)
			အကယ်၍ (frp2 != prevfrp)
			    minheight += frame_minheight(frp2, NOWIN);
		prevfrp = frp;
	    }
	    available = topframe->fr_height;
	    needed += minheight;
	}
	မဟုတ်ပါက
	{
	    minheight = frame_minheight(oldwin->w_frame, NOWIN) + need_status;
	    available = oldwin->w_frame->fr_height;
	    needed += minheight;
	}
	အကယ်၍ (!(flags & WSP_FORCE_ROOM) && available < needed)
	{
	    emsg(_(e_not_enough_room));
	    goto theend;
	}
	oldwin_height = oldwin->w_height;
	အကယ်၍ (need_status)
	{
	    oldwin->w_status_height = statusline_height(oldwin);
	    oldwin_height -= oldwin->w_status_height;
	}
	// Estimated stl height for the new window.
	ကိန်းပြည့် new_win_stlh = need_status != 0 ? need_status
			: statusline_height(new_wp != NULL ? new_wp : curwin);
	အကယ်၍ (new_size == 0)
	    new_size = (oldwin_height - new_win_stlh + 1) / 2;
	အကယ်၍ (new_size > available - minheight - new_win_stlh)
	    new_size = available - minheight - new_win_stlh;
	အကယ်၍ (new_size < wmh1)
	    new_size = wmh1;

	// if it doesn't fit in the current window, need win_equal()
	အကယ်၍ (oldwin_height - new_size - new_win_stlh < p_wmh)
	    do_equal = TRUE;

	// We don't like to take lines for the new window from a
	// 'winfixheight' window.  Take them from a window above or below
	// instead, if possible.
	အကယ်၍ (oldwin->w_p_wfh)
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
	အကယ်၍ (!do_equal && p_ea && size == 0 && *p_ead != 'h'
	    && oldwin->w_frame->fr_parent != NULL)
	    do_equal = TRUE;
    }

    /*
     * allocate new window structure and link it in the window list
     */
    အကယ်၍ ((flags & WSP_TOP) == 0
	    && ((flags & WSP_BOT)
		|| (flags & WSP_BELOW)
		|| (!(flags & WSP_ABOVE)
		    && ( (flags & WSP_VERT) ? p_spr : p_sb))))
    {
	// new window below/right of current one
	အကယ်၍ (new_wp == NULL)
	    wp = win_alloc(oldwin, FALSE);
	မဟုတ်ပါက
	    win_append(oldwin, wp);
    }
    မဟုတ်ပါက
    {
	အကယ်၍ (new_wp == NULL)
	    wp = win_alloc(oldwin->w_prev, FALSE);
	မဟုတ်ပါက
	    win_append(oldwin->w_prev, wp);
    }

    အကယ်၍ (new_wp == NULL)
    {
	အကယ်၍ (wp == NULL)
	    goto theend;

	new_frame(wp);
	အကယ်၍ (wp->w_frame == NULL)
	{
	    win_free(wp, NULL);
	    goto theend;
	}

	// make the contents of the new window the same as the current one
	win_init(wp, curwin, flags);
    }

    // Going to reorganize frames now, make sure they're flat.
    အကယ်၍ (to_flatten != NULL)
	frame_flatten(to_flatten);

    /*
     * Reorganise the tree of frames to insert the new window.
     */
    အကယ်၍ (flags & (WSP_TOP | WSP_BOT))
    {
	အကယ်၍ ((topframe->fr_layout == FR_COL && (flags & WSP_VERT) == 0)
	    || (topframe->fr_layout == FR_ROW && (flags & WSP_VERT) != 0))
	{
	    curfrp = topframe->fr_child;
	    အကယ်၍ (flags & WSP_BOT)
		နေစဉ် (curfrp->fr_next != NULL)
		    curfrp = curfrp->fr_next;
	}
	မဟုတ်ပါက
	    curfrp = topframe;
	before = (flags & WSP_TOP);
    }
    မဟုတ်ပါက
    {
	curfrp = oldwin->w_frame;
	အကယ်၍ (flags & WSP_BELOW)
	    before = FALSE;
	မဟုတ်ပါက အကယ်၍ (flags & WSP_ABOVE)
	    before = TRUE;
	မဟုတ်ပါက အကယ်၍ (flags & WSP_VERT)
	    before = !p_spr;
	မဟုတ်ပါက
	    before = !p_sb;
    }
    အကယ်၍ (curfrp->fr_parent == NULL || curfrp->fr_parent->fr_layout != layout)
    {
	// Need to create a new frame in the tree to make a branch.
	frp = ALLOC_CLEAR_ONE(frame_T);
	*frp = *curfrp;
	curfrp->fr_layout = layout;
	frp->fr_parent = curfrp;
	frp->fr_next = NULL;
	frp->fr_prev = NULL;
	curfrp->fr_child = frp;
	curfrp->fr_win = NULL;
	curfrp = frp;
	အကယ်၍ (frp->fr_win != NULL)
	    oldwin->w_frame = frp;
	မဟုတ်ပါက
	    FOR_ALL_FRAMES(frp, frp->fr_child)
		frp->fr_parent = curfrp;
    }

    အကယ်၍ (new_wp == NULL)
	frp = wp->w_frame;
    မဟုတ်ပါက
	frp = new_wp->w_frame;
    frp->fr_parent = curfrp->fr_parent;

    // Insert the new frame at the right place in the frame list.
    အကယ်၍ (before)
	frame_insert(curfrp, frp);
    မဟုတ်ပါက
	frame_append(curfrp, frp);

    // Set w_fraction now so that the cursor keeps the same relative
    // vertical position.
    အကယ်၍ (!did_set_fraction)
	set_fraction(oldwin);
    wp->w_fraction = oldwin->w_fraction;

    အကယ်၍ (flags & WSP_VERT)
    {
	wp->w_p_scr = curwin->w_p_scr;

	အကယ်၍ (need_status)
	{
	    win_new_height(oldwin, oldwin->w_height - need_status);
	    oldwin->w_status_height = need_status;
	}
	အကယ်၍ (flags & (WSP_TOP | WSP_BOT))
	{
	    // set height and row of new window to full height
	    // no status line when 'laststatus' is zero
	    ကိန်းပြည့် stl_height = p_ls > 0 ? statusline_height(curfrp->fr_win) : 0;

	    wp->w_winrow = tabline_height();
	    win_new_height(wp, curfrp->fr_height - stl_height
							  - WINBAR_HEIGHT(wp));
	    wp->w_status_height = stl_height;
	}
	မဟုတ်ပါက
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
	အကယ်၍ (before)
	    wp->w_vsep_width = 1;
	မဟုတ်ပါက
	{
	    wp->w_vsep_width = oldwin->w_vsep_width;
	    oldwin->w_vsep_width = 1;
	}
	အကယ်၍ (flags & (WSP_TOP | WSP_BOT))
	{
	    အကယ်၍ (flags & WSP_BOT)
		frame_set_vsep(curfrp, TRUE);
	    // Set width of neighbor frame
	    frame_new_width(curfrp, curfrp->fr_width
		     - (new_size + ((flags & WSP_TOP) != 0)), flags & WSP_TOP,
								       FALSE);
	}
	မဟုတ်ပါက
	    win_new_width(oldwin, oldwin->w_width - (new_size + 1));
	အကယ်၍ (before)	// new window left of current one
	{
	    wp->w_wincol = oldwin->w_wincol;
	    oldwin->w_wincol += new_size + 1;
	}
	မဟုတ်ပါက		// new window right of current one
	    wp->w_wincol = oldwin->w_wincol + oldwin->w_width + 1;
	frame_fix_width(oldwin);
	frame_fix_width(wp);
    }
    မဟုတ်ပါက
    {
	// width and column of new window is same as current window
	အကယ်၍ (flags & (WSP_TOP | WSP_BOT))
	{
	    wp->w_wincol = firstwin->w_wincol;
	    win_new_width(wp, topframe->fr_width);
	    wp->w_vsep_width = 0;
	}
	မဟုတ်ပါက
	{
	    wp->w_wincol = oldwin->w_wincol;
	    win_new_width(wp, oldwin->w_width);
	    wp->w_vsep_width = oldwin->w_vsep_width;
	}
	frp->fr_width = curfrp->fr_width;

	// "new_size" of the current window goes to the new window, use
	// one row for the status line
	win_new_height(wp, new_size);
	ကိန်းပြည့် old_status_height = oldwin->w_status_height;
	အကယ်၍ (flags & (WSP_TOP | WSP_BOT))
	{
	    ကိန်းပြည့် new_fr_height = curfrp->fr_height - new_size
							  + WINBAR_HEIGHT(wp);

	    အကယ်၍ (!((flags & WSP_BOT) && p_ls == 0))
		new_fr_height -= statusline_height(curfrp->fr_win);
	    အကယ်၍ (flags & WSP_BOT)
		frame_add_statusline(curfrp);
	    frame_new_height(curfrp, new_fr_height, flags & WSP_TOP, FALSE,
									FALSE);
	}
	မဟုတ်ပါက
	    win_new_height(oldwin, oldwin_height - (new_size
			+ statusline_height(wp)));
	အကယ်၍ (before)	// new window above current one
	{
	    wp->w_winrow = oldwin->w_winrow;
	    wp->w_status_height = statusline_height(wp);
	    oldwin->w_winrow += wp->w_height + statusline_height(wp);
	}
	မဟုတ်ပါက		// new window below current one
	{
	    wp->w_winrow = oldwin->w_winrow + VISIBLE_HEIGHT(oldwin)
						    + statusline_height(oldwin);
	    wp->w_status_height = old_status_height;
	    အကယ်၍ (!(flags & WSP_BOT))
		oldwin->w_status_height = statusline_height(oldwin);
	}
	frame_fix_height(wp);
	frame_fix_height(oldwin);
    }

    အကယ်၍ (flags & (WSP_TOP | WSP_BOT))
	win_comp_pos();

     // Both windows need redrawing.  Update all status lines, in case they
     // show something related to the window count or position.
    redraw_win_later(wp, UPD_NOT_VALID);
    redraw_win_later(oldwin, UPD_NOT_VALID);
    status_redraw_all();

    အကယ်၍ (need_status)
    {
	msg_row = Rows - 1;
	msg_col = sc_col;
	msg_clr_eos_force();	// Old command/ruler may still be there
	comp_col();
	msg_row = Rows - 1;
	msg_col = 0;	// put position back at start of line
    }

    /*
     * equalize the window sizes.
     * Note: this win_equal() runs before get_winopts() changes the new
     * window's statusline height (stlh).  If the new window's stlh differs
     * from the estimate used for new_size (e.g. because it won't inherit a
     * local 'statuslineopt' from the current window), get_winopts() will call
     * win_equal() again after adjusting the stlh.
     */
    အကယ်၍ (do_equal || dir != 0)
	win_equal(wp, TRUE,
		(flags & WSP_VERT) ? (dir == 'v' ? 'b' : 'h')
		: dir == 'h' ? 'b' : 'v');
    မဟုတ်ပါက အကယ်၍ (!is_aucmd_win(wp))
	win_fix_scroll(FALSE);

    // Don't change the window height/width to 'winheight' / 'winwidth' if a
    // size was given.
    အကယ်၍ (flags & WSP_VERT)
    {
	i = p_wiw;
	အကယ်၍ (size != 0)
	    p_wiw = size;

#ifdef FEAT_GUI
	// When 'guioptions' includes 'L' or 'R' may have to add scrollbars.
	အကယ်၍ (gui.in_use)
	    gui_init_which_components(NULL);
#endif
    }
    မဟုတ်ပါက
    {
	i = p_wh;
	အကယ်၍ (size != 0)
	    p_wh = size;
    }

    /*
     * make the new window the current window
     */
    (void)win_enter_ext(wp, (new_wp == NULL ? WEE_TRIGGER_NEW_AUTOCMDS : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
    အကယ်၍ (flags & WSP_VERT)
	p_wiw = i;
    မဟုတ်ပါက
	p_wh = i;
    retval = OK;

theend:
    အကယ်၍ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ပြန်ပေး retval;
}


/*
 * Initialize window "newp" from window "oldp".
 * Used when splitting a window and when creating a new tab page.
 * The windows will both edit the same buffer.
 * WSP_NEWLOC may be specified in flags to prevent the location list from
 * being copied.
 */
    static void
win_init(win_T *newp, win_T *oldp, ကိန်းပြည့် flags UNUSED)
{
    ကိန်းပြည့်		i;

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
    အကယ်၍ (flags & WSP_NEWLOC)
    {
	// Don't copy the location list.
	newp->w_llist = NULL;
	newp->w_llist_ref = NULL;
    }
    မဟုတ်ပါက
	copy_loclist_stack(oldp, newp);
#endif
    newp->w_localdir = (oldp->w_localdir == NULL)
				    ? NULL : vim_strsave(oldp->w_localdir);
    newp->w_prevdir = (oldp->w_prevdir == NULL)
				    ? NULL : vim_strsave(oldp->w_prevdir);

    အကယ်၍ (*p_spk != 'c')
    {
	အကယ်၍ (*p_spk == 't')
	    newp->w_skipcol = oldp->w_skipcol;
	newp->w_botline = oldp->w_botline;
	newp->w_prev_height = oldp->w_height - WINBAR_HEIGHT(oldp);
	newp->w_prev_winrow = oldp->w_winrow + 2 * WINBAR_HEIGHT(oldp);
    }

    // copy tagstack and folds
    အတွက် (i = 0; i < oldp->w_tagstacklen; i++)
    {
	taggy_T	*tag = &newp->w_tagstack[i];
	*tag = oldp->w_tagstack[i];
	အကယ်၍ (tag->tagname != NULL)
	    tag->tagname = vim_strsave(tag->tagname);
	အကယ်၍ (tag->user_data != NULL)
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
	bool pushed = push_highlight_overrides(newp->w_hl, newp->w_hl_len);
	အကယ်၍ (newp->w_buffer->b_term != NULL)
	    term_init_default_colors(newp->w_buffer->b_term);
	အကယ်၍ (pushed)
	    pop_highlight_overrides();
    }
#endif
}

/*
 * Initialize window "newp" from window "old".
 * Only the essential things are copied.
 */
    static void
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
    ကိန်းပြည့်
win_valid_popup(win_T *win UNUSED)
{
#ifdef FEAT_PROP_POPUP
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	အကယ်၍ (wp == win)
	    ပြန်ပေး TRUE;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	အကယ်၍ (wp == win)
	    ပြန်ပေး TRUE;
#endif
    ပြန်ပေး FALSE;
}

/*
 * Check if "win" is a pointer to an existing window in the current tab page.
 */
    ကိန်းပြည့်
win_valid(win_T *win)
{
    win_T	*wp;

    အကယ်၍ (win == NULL)
	ပြန်ပေး FALSE;
    FOR_ALL_WINDOWS(wp)
	အကယ်၍ (wp == win)
	    ပြန်ပေး TRUE;
    ပြန်ပေး win_valid_popup(win);
}

/*
 * Find window "id" in the current tab page.
 * Also find popup windows.
 * Return NULL if not found.
 */
    win_T *
win_find_by_id(ကိန်းပြည့် id)
{
    win_T   *wp;

    FOR_ALL_WINDOWS(wp)
	အကယ်၍ (wp->w_id == id)
	    ပြန်ပေး wp;
#ifdef FEAT_PROP_POPUP
    FOR_ALL_POPUPWINS(wp)
	အကယ်၍ (wp->w_id == id)
	    ပြန်ပေး wp;
    FOR_ALL_POPUPWINS_IN_TAB(curtab, wp)
	အကယ်၍ (wp->w_id == id)
	    ပြန်ပေး wp;
#endif
    ပြန်ပေး NULL;
}

/*
 * Check if "win" is a pointer to an existing window in any tab page.
 */
    ကိန်းပြည့်
win_valid_any_tab(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    အကယ်၍ (win == NULL)
	ပြန်ပေး FALSE;
    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    အကယ်၍ (wp == win)
		ပြန်ပေး TRUE;
	}
#ifdef FEAT_PROP_POPUP
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	    အကယ်၍ (wp == win)
		ပြန်ပေး TRUE;
#endif
    }
    ပြန်ပေး win_valid_popup(win);
}

/*
 * Return the number of windows.
 */
    ကိန်းပြည့်
win_count(void)
{
    win_T	*wp;
    ကိန်းပြည့်		count = 0;

    FOR_ALL_WINDOWS(wp)
	++count;
    ပြန်ပေး count;
}

/*
 * Make "count" windows on the screen.
 * Return actual number of windows on the screen.
 * Must be called when there is just one window, filling the whole screen
 * (excluding the command line).
 */
    ကိန်းပြည့်
make_windows(
    ကိန်းပြည့်		count,
    ကိန်းပြည့်		vertical)  // split windows vertically if TRUE
{
    ကိန်းပြည့်		maxcount;
    ကိန်းပြည့်		todo;

    အကယ်၍ (vertical)
    {
	// Each window needs at least 'winminwidth' lines and a separator
	// column.
	maxcount = (curwin->w_width + curwin->w_vsep_width
					     - (p_wiw - p_wmw)) / (p_wmw + 1);
    }
    မဟုတ်ပါက
    {
	// Each window needs at least 'winminheight' lines and a status line.
	maxcount = (VISIBLE_HEIGHT(curwin) + curwin->w_status_height
			- (p_wh - p_wmh)) / (p_wmh + statusline_height(curwin));
    }

    အကယ်၍ (maxcount < 2)
	maxcount = 2;
    အကယ်၍ (count > maxcount)
	count = maxcount;

    /*
     * add status line now, otherwise first window will be too big
     */
    အကယ်၍ (count > 1)
	last_status(TRUE);

    /*
     * Don't execute autocommands while creating the windows.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    // todo is number of windows left to create
    အတွက် (todo = count - 1; todo > 0; --todo)
	အကယ်၍ (vertical)
	{
	    အကယ်၍ (win_split(curwin->w_width - (curwin->w_width - todo)
			/ (todo + 1) - 1, WSP_VERT | WSP_ABOVE) == FAIL)
		bha_nga;
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (win_split(curwin->w_height - (curwin->w_height - todo
			    * statusline_height(curwin)) / (todo + 1)
			- statusline_height(curwin), WSP_ABOVE) == FAIL)
		bha_nga;
	}

    unblock_autocmds();

    // return actual number of windows
    ပြန်ပေး (count - todo);
}

/*
 * Exchange current and next window
 */
    static void
win_exchange(long Prenum)
{
    frame_T	*frp;
    frame_T	*frp2;
    win_T	*wp;
    win_T	*wp2;
    ကိန်းပြည့်		temp;

    အကယ်၍ (ERROR_IF_ANY_POPUP_WINDOW)
	ပြန်ပေး;
    အကယ်၍ (ONE_WINDOW)	    // just one window
    {
	beep_flush();
	ပြန်ပေး;
    }
    အကယ်၍ (text_or_buf_locked())
    {
	beep_flush();
	ပြန်ပေး;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * find window to exchange with
     */
    အကယ်၍ (Prenum)
    {
	frp = curwin->w_frame->fr_parent->fr_child;
	နေစဉ် (frp != NULL && --Prenum > 0)
	    frp = frp->fr_next;
    }
    မဟုတ်ပါက အကယ်၍ (curwin->w_frame->fr_next != NULL)	// Swap with next
	frp = curwin->w_frame->fr_next;
    မဟုတ်ပါက    // Swap last window in row/col with previous
	frp = curwin->w_frame->fr_prev;

    // We can only exchange a window with another window, not with a frame
    // containing windows.
    အကယ်၍ (frp == NULL || frp->fr_win == NULL || frp->fr_win == curwin)
	ပြန်ပေး;
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
    အကယ်၍ (wp->w_prev != curwin)
    {
	win_remove(curwin, NULL);
	frame_remove(curwin->w_frame);
	win_append(wp->w_prev, curwin);
	frame_insert(frp, curwin->w_frame);
    }
    အကယ်၍ (wp != wp2)
    {
	win_remove(wp, NULL);
	frame_remove(wp->w_frame);
	win_append(wp2, wp);
	အကယ်၍ (frp2 == NULL)
	    frame_insert(wp->w_frame->fr_parent->fr_child, wp->w_frame);
	မဟုတ်ပါက
	    frame_append(frp2, wp->w_frame);
    }
    // Keep the total height of each window the same, so that the frames keep
    // their height; the status line height is computed below.
    temp = curwin->w_status_height;
    အကယ်၍ (temp != wp->w_status_height)
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
#အကယ်၍ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif

    အကယ်၍ (wp->w_buffer != curbuf)
	reset_VIsual_and_resel();
    မဟုတ်ပါက အကယ်၍ (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    win_enter(wp, TRUE);
    redraw_all_later(UPD_NOT_VALID);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *		   if upwards FALSE the first window becomes the second one
 */
    static void
win_rotate(ကိန်းပြည့် upwards, ကိန်းပြည့် count)
{
    win_T	*wp1;
    win_T	*wp2;
    frame_T	*frp;
    ကိန်းပြည့်		n;

    အကယ်၍ (ONE_WINDOW)		// nothing to do
    {
	beep_flush();
	ပြန်ပေး;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Check if all frames in this row/col have one window.
    FOR_ALL_FRAMES(frp, curwin->w_frame->fr_parent->fr_child)
	အကယ်၍ (frp->fr_win == NULL)
	{
	    emsg(_(e_cannot_rotate_when_another_window_is_split));
	    ပြန်ပေး;
	}

    နေစဉ် (count--)
    {
	အကယ်၍ (upwards)		// first window becomes last window
	{
	    // remove first window/frame from the list
	    frp = curwin->w_frame->fr_parent->fr_child;
	    wp1 = frp->fr_win;
	    win_remove(wp1, NULL);
	    frame_remove(frp);

	    // find last frame and append removed window/frame after it
	    အတွက် ( ; frp->fr_next != NULL; frp = frp->fr_next)
		;
	    win_append(frp->fr_win, wp1);
	    frame_append(frp, wp1->w_frame);

	    wp2 = frp->fr_win;		// previously last window
	}
	မဟုတ်ပါက			// last window becomes first window
	{
	    // find last window/frame in the list and remove it
	    အတွက် (frp = curwin->w_frame; frp->fr_next != NULL;
							   frp = frp->fr_next)
		;
	    wp1 = frp->fr_win;
	    wp2 = wp1->w_prev;		    // will become last window
	    win_remove(wp1, NULL);
	    frame_remove(frp);

	    // append the removed window/frame before the first in the list
	    win_append(frp->fr_parent->fr_child->fr_win->w_prev, wp1);
	    frame_insert(frp->fr_parent->fr_child, frp);
	}

	// exchange status height and vsep width of old and new last window
	// Keep the total height of each window the same, so that the frames
	// keep their height; the status line height is computed below.
	n = wp2->w_status_height;
	အကယ်၍ (n != wp1->w_status_height)
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
#အကယ်၍ defined(FEAT_STL_OPT)
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
    ကိန်းပြည့်
win_splitmove(win_T *wp, ကိန်းပြည့် size, ကိန်းပြည့် flags)
{
    ကိန်းပြည့်		dir;
    ကိန်းပြည့်		height = wp->w_height;
    frame_T	*unflat_altfr;

    အကယ်၍ (ONE_WINDOW)
	ပြန်ပေး OK;	// nothing to do
    အကယ်၍ (check_split_disallowed(wp) == FAIL)
	ပြန်ပေး FAIL;

    // Remove the window and frame from the tree of frames.  Don't flatten any
    // frames yet so we can restore things if win_split_ins fails.
    winframe_remove(wp, &dir, NULL, &unflat_altfr);
    win_remove(wp, NULL);
    last_status(FALSE);	    // may need to remove last status line
    win_comp_pos();   // recompute window positions

    // Split a window on the desired side and put "wp" there.
    အကယ်၍ (win_split_ins(size, flags, wp, dir, unflat_altfr) == FAIL)
    {
	// win_split_ins doesn't change sizes or layout if it fails to insert an
	// existing window, so just undo winframe_remove.
	winframe_restore(wp, dir, unflat_altfr);
	win_append(wp->w_prev, wp);
	ပြန်ပေး FAIL;
    }

    // If splitting horizontally, try to preserve height.
    // Note that win_split_ins autocommands may have immediately closed "wp"!
    အကယ်၍ (size == 0 && !(flags & WSP_VERT) && win_valid(wp))
    {
	win_setheight_win(height, wp);
	အကယ်၍ (p_ea)
	{
	    // Equalize windows.  Note that win_split_ins autocommands may have
	    // made a window other than "wp" current.
	    win_equal(curwin, curwin == wp, 'v');
	}
    }

#အကယ်၍ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif
    ပြန်ပေး OK;
}

/*
 * Move window "win1" to below/right of "win2" and make "win1" the current
 * window.  Only works within the same frame!
 */
    void
win_move_after(win_T *win1, win_T *win2)
{
    ကိန်းပြည့်		height;

    // check if the arguments are reasonable
    အကယ်၍ (win1 == win2)
	ပြန်ပေး;

    // check if there is something to do
    အကယ်၍ (win2->w_next != win1)
    {
	အကယ်၍ (win1->w_frame->fr_parent != win2->w_frame->fr_parent)
	{
	    iemsg("Trying to move a window into another frame");
	    ပြန်ပေး;
	}

	// may need to move the status line/vertical separator of the last
	// window
	အကယ်၍ (win1 == lastwin)
	{
	    height = win1->w_prev->w_status_height;
	    win1->w_prev->w_status_height = win1->w_status_height;
	    win1->w_status_height = height;
	    အကယ်၍ (win1->w_prev->w_vsep_width == 1)
	    {
		// Remove the vertical separator from the last-but-one window,
		// add it to the last window.  Adjust the frame widths.
		win1->w_prev->w_vsep_width = 0;
		win1->w_prev->w_frame->fr_width -= 1;
		win1->w_vsep_width = 1;
		win1->w_frame->fr_width += 1;
	    }
	}
	မဟုတ်ပါက အကယ်၍ (win2 == lastwin)
	{
	    height = win1->w_status_height;
	    win1->w_status_height = win2->w_status_height;
	    win2->w_status_height = height;
	    အကယ်၍ (win1->w_vsep_width == 1)
	    {
		// Remove the vertical separator from win1, add it to the last
		// window, win2.  Adjust the frame widths.
		win2->w_vsep_width = 1;
		win2->w_frame->fr_width += 1;
		win1->w_vsep_width = 0;
		win1->w_frame->fr_width -= 1;
	    }
	}
	win_remove(win1, NULL);
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
    void
win_equal(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    ကိန်းပြည့်		current,	// do only frame with current window
    ကိန်းပြည့်		dir)		// 'v' for vertically, 'h' for horizontally,
				// 'b' for both, 0 for using p_ead
{
    အကယ်၍ (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == NULL ? curwin : next_curwin, current,
		      topframe, dir, firstwin->w_wincol, tabline_height(),
		      topframe->fr_width, topframe->fr_height);
#အကယ်၍ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
    အကယ်၍ (!is_aucmd_win(next_curwin))
	win_fix_scroll(TRUE);
}

/*
 * Set a frame to a new position and height, spreading the available room
 * equally over contained frames.
 * The window "next_curwin" (if not NULL) should at least get the size from
 * 'winheight' and 'winwidth' if possible.
 */
    static void
win_equal_rec(
    win_T	*next_curwin,	// pointer to current window to be or NULL
    ကိန်းပြည့်		current,	// do only frame with current window
    frame_T	*topfr,		// frame to set size off
    ကိန်းပြည့်		dir,		// 'v', 'h' or 'b', see win_equal()
    ကိန်းပြည့်		col,		// horizontal position for frame
    ကိန်းပြည့်		row,		// vertical position for frame
    ကိန်းပြည့်		width,		// new width of frame
    ကိန်းပြည့်		height)		// new height of frame
{
    ကိန်းပြည့်		n, m;
    ကိန်းပြည့်		extra_sep = 0;
    ကိန်းပြည့်		wincount, totwincount = 0;
    frame_T	*fr;
    ကိန်းပြည့်		next_curwin_size = 0;
    ကိန်းပြည့်		room = 0;
    ကိန်းပြည့်		new_size;
    ကိန်းပြည့်		has_next_curwin = 0;
    ကိန်းပြည့်		hnc;

    အကယ်၍ (topfr->fr_layout == FR_LEAF)
    {
	// Set the width/height of this frame.
	// Redraw when size or position changes
	အကယ်၍ (topfr->fr_height != height || topfr->fr_win->w_winrow != row
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
    မဟုတ်ပါက အကယ်၍ (topfr->fr_layout == FR_ROW)
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	အကယ်၍ (dir != 'v')			// equalize frame widths
	{
	    // Compute the maximum number of windows horizontally in this
	    // frame.
	    n = frame_minwidth(topfr, NOWIN);
	    // add one for the rightmost window, it doesn't have a separator
	    အကယ်၍ (col + width >= firstwin->w_wincol + topframe->fr_width)
		extra_sep = 1;
	    မဟုတ်ပါက
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
	    အကယ်၍ (room < 0)
	    {
		next_curwin_size = p_wiw + room;
		room = 0;
	    }
	    မဟုတ်ပါက
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    အကယ်၍ (!frame_fixed_width(fr))
			continue;
		    // If 'winfixwidth' set keep the window width if possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minwidth(fr, NOWIN);
		    new_size = fr->fr_width;
		    အကယ်၍ (frame_has_win(fr, next_curwin))
		    {
			room += p_wiw - p_wmw;
			next_curwin_size = 0;
			အကယ်၍ (new_size < p_wiw)
			    new_size = p_wiw;
		    }
		    မဟုတ်ပါက
			// These windows don't use up room.
			totwincount -= (n + (fr->fr_next == NULL
					? extra_sep : 0)) / (p_wmw + 1);
		    room -= new_size - n;
		    အကယ်၍ (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newwidth = new_size;
		}
		အကယ်၍ (next_curwin_size == -1)
		{
		    အကယ်၍ (!has_next_curwin)
			next_curwin_size = 0;
		    မဟုတ်ပါက အကယ်၍ (totwincount > 1
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
		    မဟုတ်ပါက
			next_curwin_size = p_wiw;
		}
	    }

	    အကယ်၍ (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    အကယ်၍ (fr->fr_next == NULL)
		// last frame gets all that remains (avoid roundoff error)
		new_size = width;
	    မဟုတ်ပါက အကယ်၍ (dir == 'v')
		new_size = fr->fr_width;
	    မဟုတ်ပါက အကယ်၍ (frame_fixed_width(fr))
	    {
		new_size = fr->fr_newwidth;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    မဟုတ်ပါက
	    {
		// Compute the maximum number of windows horiz. in "fr".
		n = frame_minwidth(fr, NOWIN);
		wincount = (n + (fr->fr_next == NULL ? extra_sep : 0))
								/ (p_wmw + 1);
		m = frame_minwidth(fr, next_curwin);
		အကယ်၍ (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		မဟုတ်ပါက
		    hnc = FALSE;
		အကယ်၍ (hnc)	    // don't count next_curwin
		    --wincount;
		အကယ်၍ (totwincount == 0)
		    new_size = room;
		မဟုတ်ပါက
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		အကယ်၍ (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wiw - (m - n);
		    အကယ်၍ (next_curwin_size < 0)
			next_curwin_size = 0;
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		မဟုတ်ပါက
		    room -= new_size;
		new_size += n;
	    }

	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    အကယ်၍ (!current || dir != 'v' || topfr->fr_parent != NULL
		    || (new_size != fr->fr_width)
		    || frame_has_win(fr, next_curwin))
		win_equal_rec(next_curwin, current, fr, dir, col, row,
							    new_size, height);
	    col += new_size;
	    width -= new_size;
	    totwincount -= wincount;
	}
    }
    မဟုတ်ပါက // topfr->fr_layout == FR_COL
    {
	topfr->fr_width = width;
	topfr->fr_height = height;

	အကယ်၍ (dir != 'h')			// equalize frame heights
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
	    အကယ်၍ (room < 0)
	    {
		// The room is less than 'winheight', use all space for the
		// current window.
		next_curwin_size = p_wh + room;
		room = 0;
	    }
	    မဟုတ်ပါက
	    {
		next_curwin_size = -1;
		FOR_ALL_FRAMES(fr, topfr->fr_child)
		{
		    အကယ်၍ (!frame_fixed_height(fr))
			continue;
		    // If 'winfixheight' set keep the window height if
		    // possible.
		    // Watch out for this window being the next_curwin.
		    n = frame_minheight(fr, NOWIN);
		    new_size = fr->fr_height;
		    အကယ်၍ (frame_has_win(fr, next_curwin))
		    {
			room += p_wh - p_wmh;
			next_curwin_size = 0;
			// n + (p_wh - p_wmh) = frame_minheight(fr,
			// next_curwin): min height for curwin.
			အကယ်၍ (new_size < n + (p_wh - p_wmh))
			    new_size = n + (p_wh - p_wmh);
		    }
		    မဟုတ်ပါက
			// These windows don't use up room.
			totwincount -= frame_wincount_in_height(fr);
		    room -= new_size - n;
		    အကယ်၍ (room < 0)
		    {
			new_size += room;
			room = 0;
		    }
		    fr->fr_newheight = new_size;
		}
		အကယ်၍ (next_curwin_size == -1)
		{
		    အကယ်၍ (!has_next_curwin)
			next_curwin_size = 0;
		    မဟုတ်ပါက အကယ်၍ (totwincount > 1
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
		    မဟုတ်ပါက
			next_curwin_size = p_wh;
		}
	    }

	    အကယ်၍ (has_next_curwin)
		--totwincount;		// don't count curwin
	}

	FOR_ALL_FRAMES(fr, topfr->fr_child)
	{
	    wincount = 1;
	    အကယ်၍ (fr->fr_next == NULL)
		// last frame gets all that remains (avoid roundoff error)
		new_size = height;
	    မဟုတ်ပါက အကယ်၍ (dir == 'h')
		new_size = fr->fr_height;
	    မဟုတ်ပါက အကယ်၍ (frame_fixed_height(fr))
	    {
		new_size = fr->fr_newheight;
		wincount = 0;	    // doesn't count as a sizeable window
	    }
	    မဟုတ်ပါက
	    {
		// Compute the maximum number of windows vert. in "fr".
		n = frame_minheight(fr, NOWIN);
		wincount = frame_wincount_in_height(fr);
		m = frame_minheight(fr, next_curwin);
		အကယ်၍ (has_next_curwin)
		    hnc = frame_has_win(fr, next_curwin);
		မဟုတ်ပါက
		    hnc = FALSE;
		အကယ်၍ (hnc)	    // don't count next_curwin
		    --wincount;
		အကယ်၍ (totwincount == 0)
		    new_size = room;
		မဟုတ်ပါက
		    new_size = (wincount * room + ((unsigned)totwincount >> 1))
								/ totwincount;
		အကယ်၍ (hnc)	    // add next_curwin size
		{
		    next_curwin_size -= p_wh - (m - n);
		    new_size += next_curwin_size;
		    room -= new_size - next_curwin_size;
		}
		မဟုတ်ပါက
		    room -= new_size;
		new_size += n;
	    }
	    // Skip frame that is full width when splitting or closing a
	    // window, unless equalizing all frames.
	    အကယ်၍ (!current || dir != 'h' || topfr->fr_parent != NULL
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

#အကယ်၍ defined(FEAT_JOB_CHANNEL)
    void
leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    အကယ်၍ (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	ပြန်ပေး;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_insert = restart_edit;
    အကယ်၍ (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		// unshow mode later
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    အကယ်၍ ((State & MODE_INSERT) && !stop_insert_mode)
    {
	stop_insert_mode = TRUE;
	အကယ်၍ (win->w_buffer->b_prompt_insert == NUL)
	    win->w_buffer->b_prompt_insert = 'A';
    }
}

    void
entering_window(win_T *win)
{
    // Only matters for a prompt window.
    // Don't do mode changes for a prompt buffer in an autocommand window, as
    // it's only used temporarily during an autocommand.
    အကယ်၍ (!bt_prompt(win->w_buffer) || is_aucmd_win(win))
	ပြန်ပေး;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    အကယ်၍ (win->w_buffer->b_prompt_insert != NUL)
	stop_insert_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it and not already in Insert mode.
    အကယ်၍ ((State & MODE_INSERT) == 0)
	restart_edit = win->w_buffer->b_prompt_insert;
}
#endif

    static void
win_init_empty(win_T *wp)
{
    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_lines_valid = 0;
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
#အကယ်၍ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
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
    void
curwin_init(void)
{
    win_init_empty(curwin);
}

/*
 * Close all windows for buffer "buf".
 */
    void
close_windows(
    buf_T	*buf,
    ကိန်းပြည့်		keep_curwin)	    // don't close "curwin"
{
    win_T	*wp;
    tabpage_T   *tp, *nexttp;
    ကိန်းပြည့်		count = tabpage_index(NULL);

    ++RedrawingDisabled;

    အတွက် (wp = firstwin; wp != NULL && !ONE_WINDOW; )
    {
	အကယ်၍ (wp->w_buffer == buf && (!keep_curwin || wp != curwin)
		&& !(win_locked(wp) || wp->w_buffer->b_locked > 0))
	{
	    အကယ်၍ (window_layout_locked(CMD_SIZE))
		goto theend;  // Only give one error message.
	    အကယ်၍ (win_close(wp, FALSE) == FAIL)
		// If closing the window fails give up, to avoid looping
		// forever.
		bha_nga;

	    // Start all over, autocommands may change the window layout.
	    wp = firstwin;
	}
	မဟုတ်ပါက
	    wp = wp->w_next;
    }

    // Also check windows in other tab pages.
    အတွက် (tp = first_tabpage; tp != NULL; tp = nexttp)
    {
	nexttp = tp->tp_next;
	အကယ်၍ (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		အကယ်၍ (wp->w_buffer == buf
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0))
		{
		    အကယ်၍ (window_layout_locked(CMD_SIZE))
			goto theend;  // Only give one error message.
		    win_close_othertab(wp, FALSE, tp);

		    // Start all over, the tab page may be closed and
		    // autocommands may change the window layout.
		    nexttp = first_tabpage;
		    bha_nga;
		}
    }

theend:
    အကယ်၍ (RedrawingDisabled > 0)
	--RedrawingDisabled;

    အကယ်၍ (count != tabpage_index(NULL))
	apply_autocmds(EVENT_TABCLOSED, NULL, NULL, FALSE, curbuf);
}

/*
 * Return TRUE if the current window is the only window that exists (ignoring
 * "aucmd_win[]").
 * Returns FALSE if there is a window, possibly in another tab page.
 */
    ကိန်းပြည့်
last_window(void)
{
    ပြန်ပေး (one_window() && first_tabpage->tp_next == NULL);
}

/*
 * Return TRUE if there is only one window other than "aucmd_win[]" in the
 * current tab page.
 */
    ကိန်းပြည့်
one_window(void)
{
    win_T	*wp;
    ကိန်းပြည့်		seen_one = FALSE;

    FOR_ALL_WINDOWS(wp)
    {
	အကယ်၍ (!is_aucmd_win(wp))
	{
	    အကယ်၍ (seen_one)
		ပြန်ပေး FALSE;
	    seen_one = TRUE;
	}
    }
    ပြန်ပေး TRUE;
}

/*
 * Close the possibly last window in a tab page.
 * Return FALSE if there are other windows and nothing is done, TRUE otherwise.
 */
    static ကိန်းပြည့်
close_last_window_tabpage(
    win_T	*win,
    ကိန်းပြည့်		free_buf,
    tabpage_T   *prev_curtab)
{
    အကယ်၍ (!ONE_WINDOW)
	ပြန်ပေး FALSE;

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
    goto_tabpage_tp(alt_tabpage(), FALSE, win->w_buffer != NULL);

    // Safety check: Autocommands may have closed the window when jumping
    // to the other tab page.
    အကယ်၍ (valid_tabpage(prev_curtab) && prev_curtab->tp_firstwin == win)
	win_close_othertab(win, free_buf, prev_curtab);

    // Entering the other tab page made the closed one the last used tab page.
    // Restore the previous one when it is still there.
    အကယ်၍ (valid_tabpage(save_lastused) && save_lastused != curtab)
	lastused_tabpage = save_lastused;
#ifdef FEAT_JOB_CHANNEL
    entering_window(curwin);
#endif
    // Since goto_tabpage_tp above did not trigger *Enter autocommands, do
    // that now.
    apply_autocmds(EVENT_TABCLOSED, NULL, NULL, FALSE, curbuf);
#အကယ်၍ defined(FEAT_TABPANEL)
    အကယ်၍ (p_stpl > 0)
	shell_new_columns();
#endif
    apply_autocmds(EVENT_WINENTER, NULL, NULL, FALSE, curbuf);
    apply_autocmds(EVENT_TABENTER, NULL, NULL, FALSE, curbuf);
    အကယ်၍ (old_curbuf != curbuf)
	apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
    ပြန်ပေး TRUE;
}

/*
 * Close the buffer of "win" and unload it if "action" is DOBUF_UNLOAD.
 * "action" can also be zero (do nothing) or DOBUF_WIPE.
 * "abort_if_last" is passed to close_buffer(): abort closing if all other
 * windows are closed.
 */
    static void
win_close_buffer(win_T *win, ကိန်းပြည့် action, ကိန်းပြည့် abort_if_last)
{
#ifdef FEAT_SYN_HL
    // Free independent synblock before the buffer is freed.
    အကယ်၍ (win->w_buffer != NULL)
	reset_synblock(win);
#endif

#ifdef FEAT_QUICKFIX
    // When a quickfix/location list window is closed and the buffer is
    // displayed in only one window, then unlist the buffer.
    အကယ်၍ (win->w_buffer != NULL && bt_quickfix(win->w_buffer)
					&& win->w_buffer->b_nwindows == 1)
	win->w_buffer->b_p_bl = FALSE;
#endif

    // Close the link to the buffer.
    အကယ်၍ (win->w_buffer != NULL)
    {
	bufref_T    bufref;

	set_bufref(&bufref, curbuf);
	++win->w_locked;
	close_buffer(win, win->w_buffer, action, abort_if_last, TRUE, TRUE);
	အကယ်၍ (win_valid_any_tab(win))
	    --win->w_locked;
	// Make sure curbuf is valid. It can become invalid if 'bufhidden' is
	// "wipe".
	အကယ်၍ (!bufref_valid(&bufref))
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
    ကိန်းပြည့်
win_close(win_T *win, ကိန်းပြည့် free_buf)
{
    win_T	*wp;
    ကိန်းပြည့်		other_buffer = FALSE;
    ကိန်းပြည့်		close_curwin = FALSE;
    ကိန်းပြည့်		dir;
    ကိန်းပြည့်		help_window = FALSE;
    ကိန်းပြည့်		quickfix_window = FALSE;
    tabpage_T   *prev_curtab = curtab;
    frame_T	*win_frame = win->w_frame->fr_parent;
#ifdef FEAT_DIFF
    ကိန်းပြည့်		had_diffmode = win->w_p_diff;
#endif
#ifdef MESSAGE_QUEUE
    ကိန်းပြည့်		did_decrement = FALSE;
#endif

#အကယ်၍ defined(FEAT_TERMINAL) && defined(FEAT_PROP_POPUP)
    // Can close a popup window with a terminal if the job has finished.
    အကယ်၍ (may_close_term_popup() == OK)
	ပြန်ပေး OK;
#endif
    အကယ်၍ (ERROR_IF_ANY_POPUP_WINDOW)
	ပြန်ပေး FAIL;

    အကယ်၍ (last_window())
    {
	emsg(_(e_cannot_close_last_window));
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (window_layout_locked(CMD_close))
	ပြန်ပေး FAIL;

    အကယ်၍ (win_locked(win) || (win->w_buffer != NULL
					       && win->w_buffer->b_locked > 0))
	ပြန်ပေး FAIL; // window is already being closed
    အကယ်၍ (win_unlisted(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ပြန်ပေး FAIL;
    }
    အကယ်၍ ((is_aucmd_win(firstwin) || is_aucmd_win(lastwin)) && one_window())
    {
	emsg(_(e_cannot_close_window_only_autocmd_window_would_remain));
	ပြန်ပေး FAIL;
    }

    // When closing the last window in a tab page first go to another tab page
    // and then close the window and the tab page to avoid that curwin and
    // curtab are invalid while we are freeing memory.
    အကယ်၍ (close_last_window_tabpage(win, free_buf, prev_curtab))
	ပြန်ပေး FAIL;

    // When closing the help window, try restoring a snapshot after closing
    // the window.  Otherwise clear the snapshot, it's now invalid.
    အကယ်၍ (bt_help(win->w_buffer))
	help_window = TRUE;
    မဟုတ်ပါက
	clear_snapshot(curtab, SNAP_HELP_IDX);

    အကယ်၍ (bt_quickfix(win->w_buffer))
	quickfix_window = TRUE;
    မဟုတ်ပါက
	clear_snapshot(curtab, SNAP_QUICKFIX_IDX);

    အကယ်၍ (win == curwin)
    {
#ifdef FEAT_JOB_CHANNEL
	leaving_window(curwin);
#endif
	/*
	 * Guess which window is going to be the new current window.
	 * This may change because of the autocommands (sigh).
	 */
	wp = frame2win(win_altframe(win, NULL));

	/*
	 * Be careful: If autocommands delete the window or cause this window
	 * to be the last one left, return now.
	 */
	အကယ်၍ (wp->w_buffer != curbuf)
	{
	    reset_VIsual_and_resel();	// stop Visual mode

	    other_buffer = TRUE;
	    အကယ်၍ (!win_valid(win))
		ပြန်ပေး FAIL;
	    ++win->w_locked;
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf);
	    အကယ်၍ (!win_valid(win))
		ပြန်ပေး FAIL;
	    --win->w_locked;
	    အကယ်၍ (last_window())
		ပြန်ပေး FAIL;
	}
	++win->w_locked;
	apply_autocmds(EVENT_WINLEAVE, NULL, NULL, FALSE, curbuf);
	အကယ်၍ (!win_valid(win))
	    ပြန်ပေး FAIL;
	--win->w_locked;
	အကယ်၍ (last_window())
	    ပြန်ပေး FAIL;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	အကယ်၍ (aborting())
	    ပြန်ပေး FAIL;
#endif
    }

#ifdef FEAT_GUI
    // Avoid trouble with scrollbars that are going to be deleted in
    // win_free().
    အကယ်၍ (gui.in_use)
	out_flush();
#endif

#ifdef FEAT_PROP_POPUP
    အကယ်၍ (popup_win_closed(win) && !win_valid(win))
	ပြန်ပေး FAIL;
#endif

    // Trigger WinClosed just before starting to free window-related resources.
    trigger_winclosed(win);
    // autocmd may have freed the window already.
    အကယ်၍ (!win_valid_any_tab(win))
	ပြန်ပေး OK;

    win_close_buffer(win, free_buf ? DOBUF_UNLOAD : 0, TRUE);

    အကယ်၍ (win_valid(win) && win->w_buffer == NULL
#အကယ်၍ defined(FEAT_PROP_POPUP)
	    && !popup_is_popup(win)
#endif
	    && last_window())
    {
	// Autocommands have closed all windows, quit now.  Restore
	// curwin->w_buffer, otherwise writing viminfo may fail.
	အကယ်၍ (curwin->w_buffer == NULL)
	    curwin->w_buffer = curbuf;
	getout(0);
    }

    // Autocommands may have moved to another tab page.
    အကယ်၍ (curtab != prev_curtab && win_valid_any_tab(win)
						      && win->w_buffer == NULL)
    {
	// Need to close the window anyway, since the buffer is NULL.
	win_close_othertab(win, FALSE, prev_curtab);
	ပြန်ပေး FAIL;
    }

    // Autocommands may have closed the window already or closed the only
    // other window.
    အကယ်၍ (!win_valid(win) || last_window()
	    || close_last_window_tabpage(win, free_buf, prev_curtab))
	ပြန်ပေး FAIL;

    // Now we are really going to close the window.  Disallow any autocommand
    // to split a window to avoid trouble.
    // Also bail out of parse_queued_messages() to avoid it tries to update the
    // screen.
    ++split_disallowed;
#ifdef MESSAGE_QUEUE
    ++dont_parse_messages;
#endif

    အကယ်၍ (win->w_buffer != NULL)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window and get the window that received
    // the screen space.
    wp = win_free_mem(win, &dir, NULL);

    အကယ်၍ (help_window || quickfix_window)
    {
	// Closing the help window moves the cursor back to the current window
	// of the snapshot.
	win_T *prev_win = get_snapshot_curwin(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX);

	အကယ်၍ (win_valid(prev_win))
	    wp = prev_win;
    }

    // Make sure curwin isn't invalid.  It can cause severe trouble when
    // printing an error message.  For win_equal() curbuf needs to be valid
    // too.
    အကယ်၍ (win == curwin)
    {
	curwin = wp;
#ifdef FEAT_QUICKFIX
	အကယ်၍ (wp->w_p_pvw || bt_quickfix(wp->w_buffer))
	{
	    /*
	     * If the cursor goes to the preview or the quickfix window, try
	     * finding another window to go to.
	     */
	    အတွက် (;;)
	    {
		အကယ်၍ (wp->w_next == NULL)
		    wp = firstwin;
		မဟုတ်ပါက
		    wp = wp->w_next;
		အကယ်၍ (wp == curwin)
		    bha_nga;
		အကယ်၍ (!wp->w_p_pvw && !bt_quickfix(wp->w_buffer))
		{
		    curwin = wp;
		    bha_nga;
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

    အကယ်၍ (p_ea && (*p_ead == 'b' || *p_ead == dir))
	// If the frame of the closed window contains the new current window,
	// only resize that frame.  Otherwise resize all windows.
	win_equal(curwin, curwin->w_frame->fr_parent == win_frame, dir);
    မဟုတ်ပါက
    {
	win_comp_pos();
	win_fix_scroll(FALSE);
    }

    အကယ်၍ (close_curwin)
    {
	// Pass WEE_ALLOW_PARSE_MESSAGES to decrement dont_parse_messages
	// before autocommands.
#ifdef MESSAGE_QUEUE
	did_decrement =
#မဟုတ်ပါက
	(void)
#endif
	    win_enter_ext(wp,
		WEE_CURWIN_INVALID | WEE_TRIGGER_ENTER_AUTOCMDS
		      | WEE_TRIGGER_LEAVE_AUTOCMDS | WEE_ALLOW_PARSE_MESSAGES);
	အကယ်၍ (other_buffer)
	    // careful: after this wp and win may be invalid!
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
    }

    အကယ်၍ (ONE_WINDOW && curwin->w_locked && curbuf->b_locked_split
	    && first_tabpage->tp_next != NULL)
	// The new curwin is the last window in the current tab page, and it is
	// already being closed.  Trigger TabLeave now, as after its buffer is
	// removed it's no longer safe to do that.
	apply_autocmds(EVENT_TABLEAVE, NULL, NULL, FALSE, curbuf);

    --split_disallowed;
#ifdef MESSAGE_QUEUE
    အကယ်၍ (!did_decrement)
	--dont_parse_messages;
#endif

    // After closing the help or quickfix window, try restoring the window
    // layout from before it was opened.
    အကယ်၍ (help_window || quickfix_window)
	restore_snapshot(help_window ? SNAP_HELP_IDX : SNAP_QUICKFIX_IDX,
			close_curwin);

#ifdef FEAT_DIFF
    // If the window had 'diff' set and now there is only one window left in
    // the tab page with 'diff' set, and "closeoff" is in 'diffopt', then
    // execute ":diffoff!".
    အကယ်၍ (diffopt_closeoff() && had_diffmode && curtab == prev_curtab)
    {
	ကိန်းပြည့်	diffcount = 0;
	win_T	*dwin;

	FOR_ALL_WINDOWS(dwin)
	    အကယ်၍ (dwin->w_p_diff)
		++diffcount;
	အကယ်၍ (diffcount == 1)
	    do_cmdline_cmd((char_u *)"diffoff!");
    }
#endif

#အကယ်၍ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove scrollbars.
    အကယ်၍ (gui.in_use && !win_hasvertsplit())
	gui_init_which_components(NULL);
#endif

    redraw_all_later(UPD_NOT_VALID);
    ပြန်ပေး OK;
}

    static void
trigger_winnewpre(void)
{
    window_layout_lock();
    apply_autocmds(EVENT_WINNEWPRE, NULL, NULL, FALSE, NULL);
    window_layout_unlock();
}

    static void
trigger_winclosed(win_T *win)
{
    static ကိန်းပြည့်	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    အကယ်၍ (recursive)
	ပြန်ပေး;
    recursive = TRUE;
    vim_snprintf((var_na *)winid, parimaa_na(winid), "%d", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}

    void
trigger_tabclosedpre(tabpage_T *tp)
{
    static ကိန်းပြည့်	recursive = FALSE;
    tabpage_T	*ptp = curtab;

    // Quickly return when no TabClosedPre autocommands to be executed or
    // already executing
    အကယ်၍ (!has_tabclosedpre() || recursive)
	ပြန်ပေး;

    အကယ်၍ (valid_tabpage(tp))
	goto_tabpage_tp(tp, FALSE, FALSE);
    recursive = TRUE;
    window_layout_lock();
    apply_autocmds(EVENT_TABCLOSEDPRE, NULL, NULL, FALSE, NULL);
    window_layout_unlock();
    recursive = FALSE;
    // tabpage may have been modified or deleted by autocmds
    အကယ်၍ (valid_tabpage(ptp))
	// try to recover the tabpage first
	goto_tabpage_tp(ptp, FALSE, FALSE);
    မဟုတ်ပါက
	// fall back to the first tabpage
	goto_tabpage_tp(first_tabpage, FALSE, FALSE);
}

/*
 * Make a snapshot of all the window scroll positions and sizes of the current
 * tab page.
 */
    void
snapshot_windows_scroll_size(void)
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

static ကိန်းပြည့် did_initial_scroll_size_snapshot = FALSE;

    void
may_make_initial_scroll_size_snapshot(void)
{
    အကယ်၍ (!did_initial_scroll_size_snapshot)
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
    static dict_T *
make_win_info_dict(
	ကိန်းပြည့် width,
	ကိန်းပြည့် height,
	ကိန်းပြည့် topline,
# ifdef FEAT_DIFF
	ကိန်းပြည့် topfill,
# endif
	ကိန်းပြည့် leftcol,
	ကိန်းပြည့် skipcol)
{
    dict_T *d = dict_alloc();
    အကယ်၍ (d == NULL)
	ပြန်ပေး NULL;
    d->dv_refcount = 1;

    // not actually looping, for breaking out on error
    နေစဉ် (1)
    {
	typval_T tv;
	tv.v_lock = 0;
	tv.v_type = VAR_NUMBER;

	tv.vval.v_number = width;
	အကယ်၍ (dict_add_tv(d, "width", &tv) == FAIL)
	    bha_nga;
	tv.vval.v_number = height;
	အကယ်၍ (dict_add_tv(d, "height", &tv) == FAIL)
	    bha_nga;
	tv.vval.v_number = topline;
	အကယ်၍ (dict_add_tv(d, "topline", &tv) == FAIL)
	    bha_nga;
# ifdef FEAT_DIFF
	tv.vval.v_number = topfill;
# မဟုတ်ပါက
	tv.vval.v_number = 0;
# endif
	အကယ်၍ (dict_add_tv(d, "topfill", &tv) == FAIL)
	    bha_nga;
	tv.vval.v_number = leftcol;
	အကယ်၍ (dict_add_tv(d, "leftcol", &tv) == FAIL)
	    bha_nga;
	tv.vval.v_number = skipcol;
	အကယ်၍ (dict_add_tv(d, "skipcol", &tv) == FAIL)
	    bha_nga;
	ပြန်ပေး d;
    }
    dict_unref(d);
    ပြန်ပေး NULL;
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
    static void
check_window_scroll_resize(
	ကိန်းပြည့်	*size_count,
	win_T	**first_scroll_win,
	win_T	**first_size_win,
	list_T	*winlist UNUSED,
	dict_T	*v_event UNUSED)
{
#ifdef FEAT_EVAL
    ကိန်းပြည့် listidx = 0;
    ကိန်းပြည့် tot_width = 0;
    ကိန်းပြည့် tot_height = 0;
    ကိန်းပြည့် tot_topline = 0;
# ifdef FEAT_DIFF
    ကိန်းပြည့် tot_topfill = 0;
# endif
    ကိန်းပြည့် tot_leftcol = 0;
    ကိန်းပြည့် tot_skipcol = 0;
#endif

    win_T *wp;
    FOR_ALL_WINDOWS(wp)
    {
	ကိန်းပြည့် ignore_scroll = event_ignored(EVENT_WINSCROLLED, wp->w_p_eiw);
	ကိန်းပြည့် size_changed = !event_ignored(EVENT_WINRESIZED, wp->w_p_eiw)
			    && (wp->w_last_width != wp->w_width
			    || wp->w_last_height != wp->w_height);
	အကယ်၍ (size_changed)
	{
#ifdef FEAT_EVAL
	    အကယ်၍ (winlist != NULL)
	    {
		// Add this window to the list of changed windows.
		typval_T tv;
		tv.v_lock = 0;
		tv.v_type = VAR_NUMBER;
		tv.vval.v_number = wp->w_id;
		list_set_item(winlist, listidx++, &tv);
	    }
	    မဟုတ်ပါက
#endif
		အကယ်၍ (size_count != NULL)
	    {
		++*size_count;
		အကယ်၍ (*first_size_win == NULL)
		    *first_size_win = wp;
		// For WinScrolled the first window with a size change is used
		// even when it didn't scroll.
		အကယ်၍ (*first_scroll_win == NULL && !ignore_scroll)
		    *first_scroll_win = wp;
	    }
	}

	ကိန်းပြည့် scroll_changed = !ignore_scroll
				&& (wp->w_last_topline != wp->w_topline
#ifdef FEAT_DIFF
				|| wp->w_last_topfill != wp->w_topfill
#endif
				|| wp->w_last_leftcol != wp->w_leftcol
				|| wp->w_last_skipcol != wp->w_skipcol);
	အကယ်၍ (scroll_changed
	    && first_scroll_win != NULL && *first_scroll_win == NULL)
	    *first_scroll_win = wp;

#ifdef FEAT_EVAL
	အကယ်၍ ((size_changed || scroll_changed) && v_event != NULL)
	{
	    // Add info about this window to the v:event dictionary.
	    ကိန်းပြည့် width = wp->w_width - wp->w_last_width;
	    ကိန်းပြည့် height = wp->w_height - wp->w_last_height;
	    ကိန်းပြည့် topline = wp->w_topline - wp->w_last_topline;
# ifdef FEAT_DIFF
	    ကိန်းပြည့် topfill = wp->w_topfill - wp->w_last_topfill;
# endif
	    ကိန်းပြည့် leftcol = wp->w_leftcol - wp->w_last_leftcol;
	    ကိန်းပြည့် skipcol = wp->w_skipcol - wp->w_last_skipcol;
	    dict_T *d = make_win_info_dict(width, height, topline,
# ifdef FEAT_DIFF
							    topfill,
# endif
							    leftcol, skipcol);
	    အကယ်၍ (d == NULL)
		bha_nga;
	    var_na winid[NUMBUFLEN];
	    vim_snprintf(winid, parimaa_na(winid), "%d", wp->w_id);
	    အကယ်၍ (dict_add_dict(v_event, winid, d) == FAIL)
	    {
		dict_unref(d);
		bha_nga;
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
    အကယ်၍ (v_event != NULL)
    {
	dict_T *alldict = make_win_info_dict(tot_width, tot_height, tot_topline,
# ifdef FEAT_DIFF
						    tot_topfill,
# endif
						    tot_leftcol, tot_skipcol);
	အကယ်၍ (alldict != NULL)
	{
	    အကယ်၍ (dict_add_dict(v_event, "all", alldict) == FAIL)
		dict_unref(alldict);
	    မဟုတ်ပါက
		--alldict->dv_refcount;
	}
    }
#endif
}

/*
 * Trigger WinScrolled and/or WinResized if any window in the current tab page
 * scrolled or changed size.
 */
    void
may_trigger_win_scrolled_resized(void)
{
    static ကိန်းပြည့်	    recursive = FALSE;
    ကိန်းပြည့်		    do_resize = has_winresized();
    ကိန်းပြည့်		    do_scroll = has_winscrolled();

    // Do not trigger WinScrolled or WinResized recursively.  Do not trigger
    // before the initial snapshot of the w_last_ values was made.
    အကယ်၍ (recursive
	    || !(do_scroll || do_resize)
	    || !did_initial_scroll_size_snapshot)
	ပြန်ပေး;

    ကိန်းပြည့် size_count = 0;
    win_T *first_scroll_win = NULL, *first_size_win = NULL;
    check_window_scroll_resize(&size_count, &first_scroll_win, &first_size_win,
								    NULL, NULL);
    ကိန်းပြည့် trigger_resize = do_resize && size_count > 0;
    ကိန်းပြည့် trigger_scroll = do_scroll && first_scroll_win != NULL;
    အကယ်၍ (!trigger_resize && !trigger_scroll)
	ပြန်ပေး;  // no relevant changes
#ifdef FEAT_EVAL
    list_T *windows_list = NULL;
    အကယ်၍ (trigger_resize)
    {
	// Create the list for v:event.windows before making the snapshot.
	windows_list = list_alloc_with_items(size_count);
	အကယ်၍ (windows_list != NULL)
	    check_window_scroll_resize(NULL, NULL, NULL, windows_list, NULL);
    }

    dict_T *scroll_dict = NULL;
    အကယ်၍ (trigger_scroll)
    {
	// Create the dict with entries for v:event before making the snapshot.
	scroll_dict = dict_alloc();
	အကယ်၍ (scroll_dict != NULL)
	{
	    scroll_dict->dv_refcount = 1;
	    check_window_scroll_resize(NULL, NULL, NULL, NULL, scroll_dict);
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
    အကယ်၍ (trigger_resize
#ifdef FEAT_EVAL
	    && windows_list != NULL
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	အကယ်၍ (dict_add_list(v_event, "windows", windows_list) == OK)
	{
	    dict_set_items_ro(v_event);
#endif
	    char_u winid[NUMBUFLEN];
	    vim_snprintf((var_na *)winid, parimaa_na(winid), "%d",
							 first_size_win->w_id);
	    apply_autocmds(EVENT_WINRESIZED, winid, winid, FALSE,
						     first_size_win->w_buffer);
#ifdef FEAT_EVAL
	}
	restore_v_event(v_event, &save_v_event);
#endif
    }

    အကယ်၍ (trigger_scroll
#ifdef FEAT_EVAL
	    && scroll_dict != NULL
#endif
	    )
    {
#ifdef FEAT_EVAL
	save_v_event_T  save_v_event;
	dict_T		*v_event = get_v_event(&save_v_event);

	// Move the entries from scroll_dict to v_event.
	dict_extend(v_event, scroll_dict, (char_u *)"move", NULL);
	dict_set_items_ro(v_event);
	dict_unref(scroll_dict);
#endif
	char_u winid[NUMBUFLEN];
	vim_snprintf((var_na *)winid, parimaa_na(winid), "%d",
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
    void
win_close_othertab(win_T *win, ကိန်းပြည့် free_buf, tabpage_T *tp)
{
    win_T	*wp;
    ကိန်းပြည့်		dir;
    tabpage_T   *ptp = NULL;
    ကိန်းပြည့်		free_tp = FALSE;

    // Commands that may call win_close_othertab() already check this, but
    // check here again just in case.
    အကယ်၍ (window_layout_locked(CMD_SIZE))
	ပြန်ပေး;
    // Get here with win->w_buffer == NULL when win_close() detects the tab
    // page changed.
    အကယ်၍ (win_locked(win) || (win->w_buffer != NULL
					       && win->w_buffer->b_locked > 0))
	ပြန်ပေး; // window is already being closed

    // Trigger WinClosed just before starting to free window-related resources.
    // If the buffer is NULL, it isn't safe to trigger autocommands,
    // and win_close() should have already triggered WinClosed.
    အကယ်၍ (win->w_buffer != NULL)
    {
	trigger_winclosed(win);
	// autocmd may have freed the window already.
	အကယ်၍ (!win_valid_any_tab(win))
	    ပြန်ပေး;
    }

    အကယ်၍ (tp->tp_firstwin == tp->tp_lastwin && !tp->tp_did_tabclosedpre)
    {
	trigger_tabclosedpre(tp);
	// autocmd may have freed the window already.
	အကယ်၍ (!win_valid_any_tab(win))
	    ပြန်ပေး;
    }

    အကယ်၍ (win->w_buffer != NULL)
	// Close the link to the buffer.
	close_buffer(win, win->w_buffer, free_buf ? DOBUF_UNLOAD : 0,
							    FALSE, TRUE, TRUE);

    // Careful: Autocommands may have closed the tab page or made it the
    // current tab page.
    အတွက် (ptp = first_tabpage; ptp != NULL && ptp != tp; ptp = ptp->tp_next)
	;
    အကယ်၍ (ptp == NULL || tp == curtab)
    {
	// If the buffer was removed from the window we have to give it any
	// buffer.
	အကယ်၍ (win_valid_any_tab(win) && win->w_buffer == NULL)
	{
	    win->w_buffer = firstbuf;
	    ++firstbuf->b_nwindows;
	    အကယ်၍ (win == curwin)
		curbuf = curwin->w_buffer;
	    win_init_empty(win);
	}
	ပြန်ပေး;
    }

    // Autocommands may have closed the window already.
    အတွက် (wp = tp->tp_firstwin; wp != NULL && wp != win; wp = wp->w_next)
	;
    အကယ်၍ (wp == NULL)
	ပြန်ပေး;

    // When closing the last window in a tab page remove the tab page.
    အကယ်၍ (tp->tp_firstwin == tp->tp_lastwin)
    {
	ကိန်းပြည့်	h = tabline_height();

	အကယ်၍ (tp == first_tabpage)
	    first_tabpage = tp->tp_next;
	မဟုတ်ပါက
	{
	    အတွက် (ptp = first_tabpage; ptp != NULL && ptp->tp_next != tp;
							   ptp = ptp->tp_next)
		;
	    အကယ်၍ (ptp == NULL)
	    {
		internal_error("win_close_othertab()");
		ပြန်ပေး;
	    }
	    ptp->tp_next = tp->tp_next;
	}
	free_tp = TRUE;
	redraw_tabline = TRUE;
	shell_new_columns();
	အကယ်၍ (h != tabline_height())
	    shell_new_rows();
    }

    အကယ်၍ (win->w_buffer != NULL)
	--win->w_buffer->b_nwindows;

    // Free the memory used for the window.
    win_free_mem(win, &dir, tp);

    အကယ်၍ (free_tp)
	free_tabpage(tp);
}

/*
 * Free the memory used for a window.
 * Returns a pointer to the window that got the freed up space.
 */
    static win_T *
win_free_mem(
    win_T	*win,
    ကိန်းပြည့်		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    win_T	*wp;
    tabpage_T	*win_tp = tp == NULL ? curtab : tp;

    // Remove the window and its frame from the tree of frames.
    frp = win->w_frame;
    wp = winframe_remove(win, dirp, tp, NULL);
    vim_free(frp);
    win_free(win, tp);

    // When deleting the current window in the tab, select a new current
    // window.
    အကယ်၍ (win == win_tp->tp_curwin)
	win_tp->tp_curwin = wp;

    ပြန်ပေး wp;
}

#အကယ်၍ defined(EXITFREE)
    void
win_free_all(void)
{
    ကိန်းပြည့်		dummy;

    // avoid an error for switching tabpage with the cmdline window open
    cmdwin_type = 0;
    cmdwin_buf = NULL;
    cmdwin_win = NULL;

    နေစဉ် (first_tabpage->tp_next != NULL)
	tabpage_close(TRUE);

    အတွက် (ကိန်းပြည့် i = 0; i < AUCMD_WIN_COUNT; ++i)
	အကယ်၍ (aucmd_win[i].auc_win != NULL)
	{
	    (void)win_free_mem(aucmd_win[i].auc_win, &dummy, NULL);
	    aucmd_win[i].auc_win = NULL;
	}

    နေစဉ် (firstwin != NULL)
	(void)win_free_mem(firstwin, &dummy, NULL);

    // No window should be used after this. Set curwin to NULL to crash
    // instead of using freed memory.
    curwin = NULL;
}
#endif

/*
 * Remove a window and its frame from the tree of frames.
 * Returns a pointer to the window that got the freed up space.
 */
    win_T *
winframe_remove(
    win_T	*win,
    ကိန်းပြည့်		*dirp,		// set to 'v' or 'h' for direction if 'ea'
    tabpage_T	*tp,		// tab page "win" is in, NULL for current
    frame_T	**unflat_altfr)	// if not NULL, set to pointer of frame that got
				// the space, and it is not flattened
{
    frame_T	*frp, *frp2, *frp3;
    frame_T	*frp_close = win->w_frame;
    win_T	*wp;
    ကိန်းပြည့်		row, col;

    /*
     * If there is only one window there is nothing to remove.
     */
    အကယ်၍ (tp == NULL ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ပြန်ပေး NULL;

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
    အကယ်၍ (win->w_vsep_width == 0 && frp_close->fr_parent->fr_layout == FR_ROW &&
	    frp_close->fr_prev != NULL)
	frame_set_vsep(frp_close->fr_prev, FALSE);

    // Remove this frame from the list of frames.
    frame_remove(frp_close);

    အကယ်၍ (frp_close->fr_parent->fr_layout == FR_COL)
    {
	// When 'winfixheight' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the height
	// to.
	အကယ်၍ (frp2->fr_win != NULL && frp2->fr_win->w_p_wfh)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    နေစဉ် (frp != NULL || frp3 != NULL)
	    {
		အကယ်၍ (frp != NULL)
		{
		    အကယ်၍ (!frame_fixed_height(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			bha_nga;
		    }
		    frp = frp->fr_prev;
		}
		အကယ်၍ (frp3 != NULL)
		{
		    အကယ်၍ (frp3->fr_win != NULL && !frp3->fr_win->w_p_wfh)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			bha_nga;
		    }
		    frp3 = frp3->fr_next;
		}
	    }
	}
	frame_new_height(frp2, frp2->fr_height + frp_close->fr_height,
			    frp2 == frp_close->fr_next, !frame_fixed_height(frp2), FALSE);
	*dirp = 'v';
    }
    မဟုတ်ပါက
    {
	// When 'winfixwidth' is set, try to find another frame in the column
	// (as close to the closed frame as possible) to distribute the width
	// to.
	အကယ်၍ (frp2->fr_win != NULL && frp2->fr_win->w_p_wfw)
	{
	    frp = frp_close->fr_prev;
	    frp3 = frp_close->fr_next;
	    နေစဉ် (frp != NULL || frp3 != NULL)
	    {
		အကယ်၍ (frp != NULL)
		{
		    အကယ်၍ (!frame_fixed_width(frp))
		    {
			frp2 = frp;
			wp = frame2win(frp2);
			bha_nga;
		    }
		    frp = frp->fr_prev;
		}
		အကယ်၍ (frp3 != NULL)
		{
		    အကယ်၍ (frp3->fr_win != NULL && !frp3->fr_win->w_p_wfw)
		    {
			frp2 = frp3;
			wp = frp3->fr_win;
			bha_nga;
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
    အကယ်၍ (frp2 != frp_close->fr_prev)
	frame_comp_pos(frp_close->fr_parent, &row, &col);

    အကယ်၍ (unflat_altfr == NULL)
	frame_flatten(frp2);
    မဟုတ်ပါက
	*unflat_altfr = frp2;

    frame_locked--;

    ပြန်ပေး wp;
}

/*
 * Flatten "frp" into its parent frame if it's the only child, also merging its
 * list with the grandparent if they share the same layout.
 * Frees "frp" if flattened; also "frp->fr_parent" if it has the same layout.
 */
    static void
frame_flatten(frame_T *frp)
{
    frame_T	*frp2, *frp3;

    အကယ်၍ (frp->fr_next != NULL || frp->fr_prev != NULL)
	ပြန်ပေး;

    // There is no other frame in this list, move its info to the parent
    // and remove it.
    frp->fr_parent->fr_layout = frp->fr_layout;
    frp->fr_parent->fr_child = frp->fr_child;
    FOR_ALL_FRAMES(frp2, frp->fr_child)
	frp2->fr_parent = frp->fr_parent;
    frp->fr_parent->fr_win = frp->fr_win;
    အကယ်၍ (frp->fr_win != NULL)
	frp->fr_win->w_frame = frp->fr_parent;
    frp2 = frp->fr_parent;
    အကယ်၍ (topframe->fr_child == frp)
	topframe->fr_child = frp2;
    vim_free(frp);

    frp = frp2->fr_parent;
    အကယ်၍ (frp != NULL && frp->fr_layout == frp2->fr_layout)
    {
	// The frame above the parent has the same layout, have to merge
	// the frames into this list.
	အကယ်၍ (frp->fr_child == frp2)
	    frp->fr_child = frp2->fr_child;
	frp2->fr_child->fr_prev = frp2->fr_prev;
	အကယ်၍ (frp2->fr_prev != NULL)
	    frp2->fr_prev->fr_next = frp2->fr_child;
	အတွက် (frp3 = frp2->fr_child; ; frp3 = frp3->fr_next)
	{
	    frp3->fr_parent = frp;
	    အကယ်၍ (frp3->fr_next == NULL)
	    {
		frp3->fr_next = frp2->fr_next;
		အကယ်၍ (frp2->fr_next != NULL)
		    frp2->fr_next->fr_prev = frp3;
		bha_nga;
	    }
	}
	အကယ်၍ (topframe->fr_child == frp2)
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
    static void
winframe_restore(win_T *wp, ကိန်းပြည့် dir, frame_T *unflat_altfr)
{
    frame_T	*frp = wp->w_frame;

    // Put "wp"'s frame back where it was.
    အကယ်၍ (frp->fr_prev != NULL)
	frame_append(frp->fr_prev, frp);
    မဟုတ်ပါက
	frame_insert(frp->fr_next, frp);

    // Vertical separators to the left may have been lost.  Restore them.
    အကယ်၍ (wp->w_vsep_width == 0
	    && frp->fr_parent->fr_layout == FR_ROW && frp->fr_prev != NULL)
	frame_set_vsep(frp->fr_prev, TRUE);

    // Statuslines above may have been lost.  Restore them.
    အကယ်၍ (wp->w_status_height == 0
	    && frp->fr_parent->fr_layout == FR_COL && frp->fr_prev != NULL)
	frame_add_statusline(frp->fr_prev);

    // Restore the lost room that was redistributed to the altframe.  Also
    // adjusts window sizes to fit restored statuslines/separators, if needed.
    အကယ်၍ (dir == 'v')
    {
	frame_new_height(unflat_altfr, unflat_altfr->fr_height - frp->fr_height,
		unflat_altfr == frp->fr_next, FALSE, FALSE);
    }
    မဟုတ်ပါက အကယ်၍ (dir == 'h')
    {
	frame_new_width(unflat_altfr, unflat_altfr->fr_width - frp->fr_width,
		unflat_altfr == frp->fr_next, FALSE);
    }

    // Recompute window positions within the parent frame to restore them.
    // Positions were unchanged if the altframe was adjacent and left/above.
    အကယ်၍ (unflat_altfr != frp->fr_prev)
    {
	win_T	*topleft = frame2win(frp->fr_parent);
	ကိန်းပြည့်	row = topleft->w_winrow;
	ကိန်းပြည့်	col = topleft->w_wincol;

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
    static frame_T *
win_altframe(
    win_T	*win,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    frame_T	*frp;
    frame_T	*other_fr, *target_fr;

    အကယ်၍ (tp == NULL ? ONE_WINDOW : tp->tp_firstwin == tp->tp_lastwin)
	ပြန်ပေး alt_tabpage()->tp_curwin->w_frame;

    frp = win->w_frame;

    အကယ်၍ (frp->fr_prev == NULL)
	ပြန်ပေး frp->fr_next;
    အကယ်၍ (frp->fr_next == NULL)
	ပြန်ပေး frp->fr_prev;

    // By default the next window will get the space that was abandoned by this
    // window
    target_fr = frp->fr_next;
    other_fr  = frp->fr_prev;

    // If this is part of a column of windows and 'splitbelow' is true then the
    // previous window will get the space.
    အကယ်၍ (frp->fr_parent != NULL && frp->fr_parent->fr_layout == FR_COL && p_sb)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If this is part of a row of windows, and 'splitright' is true then the
    // previous window will get the space.
    အကယ်၍ (frp->fr_parent != NULL && frp->fr_parent->fr_layout == FR_ROW && p_spr)
    {
	target_fr = frp->fr_prev;
	other_fr  = frp->fr_next;
    }

    // If 'wfh' or 'wfw' is set for the target and not for the alternate
    // window, reverse the selection.
    အကယ်၍ (frp->fr_parent != NULL && frp->fr_parent->fr_layout == FR_ROW)
    {
	အကယ်၍ (frame_fixed_width(target_fr) && !frame_fixed_width(other_fr))
	    target_fr = other_fr;
    }
    မဟုတ်ပါက
    {
	အကယ်၍ (frame_fixed_height(target_fr) && !frame_fixed_height(other_fr))
	    target_fr = other_fr;
    }

    ပြန်ပေး target_fr;
}

/*
 * Return the tabpage that will be used if the current one is closed.
 */
    static tabpage_T *
alt_tabpage(void)
{
    tabpage_T	*tp = NULL;
    ကိန်းပြည့်		forward;

    // Use the last accessed tab page, if possible.
    အကယ်၍ ((tcl_flags & TCL_USELAST) && valid_tabpage(lastused_tabpage))
	ပြန်ပေး lastused_tabpage;

    // Use the next tab page, if possible.
    forward = curtab->tp_next != NULL &&
	    ((tcl_flags & TCL_LEFT) == 0 || curtab == first_tabpage);

    အကယ်၍ (forward)
	tp = curtab->tp_next;
    မဟုတ်ပါက
	// Use the previous tab page.
	အတွက် (tp = first_tabpage; tp->tp_next != curtab; tp = tp->tp_next)
	    ;

    ပြန်ပေး tp;
}

/*
 * Find the left-upper window in frame "frp".
 */
    static win_T *
frame2win(frame_T *frp)
{
    နေစဉ် (frp->fr_win == NULL)
	frp = frp->fr_child;
    ပြန်ပေး frp->fr_win;
}

/*
 * Return TRUE if frame "frp" contains window "wp".
 */
    static ကိန်းပြည့်
frame_has_win(frame_T *frp, win_T *wp)
{
    frame_T	*p;

    အကယ်၍ (frp->fr_layout == FR_LEAF)
	ပြန်ပေး frp->fr_win == wp;

    FOR_ALL_FRAMES(p, frp->fr_child)
	အကယ်၍ (frame_has_win(p, wp))
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

// 'cmdheight' value explicitly set by the user: window commands are allowed to
// resize the topframe to values higher than this minimum, but not lower.
static ကိန်းပြည့် min_set_ch = 1;

/*
 * Set a new height for a frame.  Recursively sets the height for contained
 * frames and windows.  Caller must take care of positions.
 */
    static void
frame_new_height(
    frame_T	*topfrp,
    ကိန်းပြည့်		height,
    ကိန်းပြည့်		topfirst,	// resize topmost contained frame first
    ကိန်းပြည့်		wfh,		// obey 'winfixheight' when there is a choice;
				// may cause the height not to be set
    ကိန်းပြည့်		set_ch)		// set 'cmdheight' to resize topframe
{
    frame_T	*frp;
    ကိန်းပြည့်		extra_lines;
    ကိန်းပြည့်		h;

    အကယ်၍ (topfrp->fr_parent == NULL && set_ch)
    {
	// topframe: update the command line height, with side effects.
	ကိန်းပြည့် new_ch = MAX(min_set_ch, p_ch + topfrp->fr_height - height);
	ကိန်းပြည့် save_ch = min_set_ch;
	အကယ်၍ (new_ch != p_ch)
	    set_option_value((char_u *)"cmdheight", new_ch, NULL, 0);
	min_set_ch = save_ch;
	height = MIN(height, ROWS_AVAIL);
    }
    အကယ်၍ (topfrp->fr_win != NULL)
    {
	// Simple case: just one window.
	win_new_height(topfrp->fr_win,
				    height - topfrp->fr_win->w_status_height
					      - WINBAR_HEIGHT(topfrp->fr_win));
    }
    မဟုတ်ပါက အကယ်၍ (topfrp->fr_layout == FR_ROW)
    {
	do
	{
	    // All frames in this row get the same new height.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_height(frp, height, topfirst, wfh, set_ch);
		အကယ်၍ (frp->fr_height > height)
		{
		    // Could not fit the windows, make the whole row higher.
		    height = frp->fr_height;
		    bha_nga;
		}
	    }
	}
	နေစဉ် (frp != NULL);
    }
    မဟုတ်ပါက    // fr_layout == FR_COL
    {
	// Complicated case: Resize a column of frames.  Resize the bottom
	// frame first, frames above that when needed.

	frp = topfrp->fr_child;
	အကယ်၍ (wfh)
	    // Advance past frames with one window with 'wfh' set.
	    နေစဉ် (frame_fixed_height(frp))
	    {
		frp = frp->fr_next;
		အကယ်၍ (frp == NULL)
		    ပြန်ပေး;	    // no frame without 'wfh', give up
	    }
	အကယ်၍ (!topfirst)
	{
	    // Find the bottom frame of this column
	    နေစဉ် (frp->fr_next != NULL)
		frp = frp->fr_next;
	    အကယ်၍ (wfh)
		// Advance back for frames with one window with 'wfh' set.
		နေစဉ် (frame_fixed_height(frp))
		    frp = frp->fr_prev;
	}

	extra_lines = height - topfrp->fr_height;
	အကယ်၍ (extra_lines < 0)
	{
	    // reduce height of contained frames, bottom or top frame first
	    နေစဉ် (frp != NULL)
	    {
		h = frame_minheight(frp, NULL);
		အကယ်၍ (frp->fr_height + extra_lines < h)
		{
		    extra_lines += frp->fr_height - h;
		    frame_new_height(frp, h, topfirst, wfh, set_ch);
		}
		မဟုတ်ပါက
		{
		    frame_new_height(frp, frp->fr_height + extra_lines,
							topfirst, wfh, set_ch);
		    bha_nga;
		}
		အကယ်၍ (topfirst)
		{
		    do
			frp = frp->fr_next;
		    နေစဉ် (wfh && frp != NULL && frame_fixed_height(frp));
		}
		မဟုတ်ပါက
		{
		    do
			frp = frp->fr_prev;
		    နေစဉ် (wfh && frp != NULL && frame_fixed_height(frp));
		}
		// Increase "height" if we could not reduce enough frames.
		အကယ်၍ (frp == NULL)
		    height -= extra_lines;
	    }
	}
	မဟုတ်ပါက အကယ်၍ (extra_lines > 0)
	{
	    // increase height of bottom or top frame
	    frame_new_height(frp, frp->fr_height + extra_lines, topfirst, wfh,
									set_ch);
	}
    }
    topfrp->fr_height = height;
}

/*
 * Return TRUE if height of frame "frp" should not be changed because of
 * the 'winfixheight' option.
 */
    static ကိန်းပြည့်
frame_fixed_height(frame_T *frp)
{
    // frame with one window: fixed height if 'winfixheight' set.
    အကယ်၍ (frp->fr_win != NULL)
	ပြန်ပေး frp->fr_win->w_p_wfh;

    အကယ်၍ (frp->fr_layout == FR_ROW)
    {
	// The frame is fixed height if one of the frames in the row is fixed
	// height.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    အကယ်၍ (frame_fixed_height(frp))
		ပြန်ပေး TRUE;
	ပြန်ပေး FALSE;
    }

    // frp->fr_layout == FR_COL: The frame is fixed height if all of the
    // frames in the row are fixed height.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	အကယ်၍ (!frame_fixed_height(frp))
	    ပြန်ပေး FALSE;
    ပြန်ပေး TRUE;
}

/*
 * Return TRUE if width of frame "frp" should not be changed because of
 * the 'winfixwidth' option.
 */
    static ကိန်းပြည့်
frame_fixed_width(frame_T *frp)
{
    // frame with one window: fixed width if 'winfixwidth' set.
    အကယ်၍ (frp->fr_win != NULL)
	ပြန်ပေး frp->fr_win->w_p_wfw;

    အကယ်၍ (frp->fr_layout == FR_COL)
    {
	// The frame is fixed width if one of the frames in the row is fixed
	// width.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    အကယ်၍ (frame_fixed_width(frp))
		ပြန်ပေး TRUE;
	ပြန်ပေး FALSE;
    }

    // frp->fr_layout == FR_ROW: The frame is fixed width if all of the
    // frames in the row are fixed width.
    FOR_ALL_FRAMES(frp, frp->fr_child)
	အကယ်၍ (!frame_fixed_width(frp))
	    ပြန်ပေး FALSE;
    ပြန်ပေး TRUE;
}

/*
 * Add a status line to windows at the bottom of "frp".
 * Note: Does not check if there is room!
 */
    static void
frame_add_statusline(frame_T *frp)
{
    win_T	*wp;

    အကယ်၍ (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	wp->w_status_height = statusline_height(wp);
    }
    မဟုတ်ပါက အကယ်၍ (frp->fr_layout == FR_ROW)
    {
	// Handle all the frames in the row.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_add_statusline(frp);
    }
    မဟုတ်ပါက // frp->fr_layout == FR_COL
    {
	// Only need to handle the last frame in the column.
	အတွက် (frp = frp->fr_child; frp->fr_next != NULL; frp = frp->fr_next)
	    ;
	frame_add_statusline(frp);
    }
}

/*
 * Set width of a frame.  Handles recursively going through contained frames.
 * May remove separator line for windows at the right side (for win_close()).
 */
    static void
frame_new_width(
    frame_T	*topfrp,
    ကိန်းပြည့်		width,
    ကိန်းပြည့်		leftfirst,	// resize leftmost contained frame first
    ကိန်းပြည့်		wfw)		// obey 'winfixwidth' when there is a choice;
				// may cause the width not to be set
{
    frame_T	*frp;
    ကိန်းပြည့်		extra_cols;
    ကိန်းပြည့်		w;
    win_T	*wp;

    အကယ်၍ (topfrp->fr_layout == FR_LEAF)
    {
	// Simple case: just one window.
	wp = topfrp->fr_win;
	// Find out if there are any windows right of this one.
	အတွက် (frp = topfrp; frp->fr_parent != NULL; frp = frp->fr_parent)
	    အကယ်၍ (frp->fr_parent->fr_layout == FR_ROW && frp->fr_next != NULL)
		bha_nga;
	အကယ်၍ (frp->fr_parent == NULL)
	    wp->w_vsep_width = 0;
	win_new_width(wp, width - wp->w_vsep_width);
    }
    မဟုတ်ပါက အကယ်၍ (topfrp->fr_layout == FR_COL)
    {
	do
	{
	    // All frames in this column get the same new width.
	    FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    {
		frame_new_width(frp, width, leftfirst, wfw);
		အကယ်၍ (frp->fr_width > width)
		{
		    // Could not fit the windows, make whole column wider.
		    width = frp->fr_width;
		    bha_nga;
		}
	    }
	} နေစဉ် (frp != NULL);
    }
    မဟုတ်ပါက    // fr_layout == FR_ROW
    {
	// Complicated case: Resize a row of frames.  Resize the rightmost
	// frame first, frames left of it when needed.

	frp = topfrp->fr_child;
	အကယ်၍ (wfw)
	    // Advance past frames with one window with 'wfw' set.
	    နေစဉ် (frame_fixed_width(frp))
	    {
		frp = frp->fr_next;
		အကယ်၍ (frp == NULL)
		    ပြန်ပေး;	    // no frame without 'wfw', give up
	    }
	အကယ်၍ (!leftfirst)
	{
	    // Find the rightmost frame of this row
	    နေစဉ် (frp->fr_next != NULL)
		frp = frp->fr_next;
	    အကယ်၍ (wfw)
		// Advance back for frames with one window with 'wfw' set.
		နေစဉ် (frame_fixed_width(frp))
		    frp = frp->fr_prev;
	}

	extra_cols = width - topfrp->fr_width;
	အကယ်၍ (extra_cols < 0)
	{
	    // reduce frame width, rightmost frame first
	    နေစဉ် (frp != NULL)
	    {
		w = frame_minwidth(frp, NULL);
		အကယ်၍ (frp->fr_width + extra_cols < w)
		{
		    extra_cols += frp->fr_width - w;
		    frame_new_width(frp, w, leftfirst, wfw);
		}
		မဟုတ်ပါက
		{
		    frame_new_width(frp, frp->fr_width + extra_cols,
							      leftfirst, wfw);
		    bha_nga;
		}
		အကယ်၍ (leftfirst)
		{
		    do
			frp = frp->fr_next;
		    နေစဉ် (wfw && frp != NULL && frame_fixed_width(frp));
		}
		မဟုတ်ပါက
		{
		    do
			frp = frp->fr_prev;
		    နေစဉ် (wfw && frp != NULL && frame_fixed_width(frp));
		}
		// Increase "width" if we could not reduce enough frames.
		အကယ်၍ (frp == NULL)
		    width -= extra_cols;
	    }
	}
	မဟုတ်ပါက အကယ်၍ (extra_cols > 0)
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
    static void
frame_set_vsep(frame_T *frp, ကိန်းပြည့် add)
{
    win_T	*wp;

    အကယ်၍ (frp->fr_layout == FR_LEAF)
    {
	wp = frp->fr_win;
	အကယ်၍ (add && wp->w_vsep_width == 0)
	{
	    အကယ်၍ (wp->w_width > 0)	// don't make it negative
		win_new_width(wp, wp->w_width - 1);
	    wp->w_vsep_width = 1;
	}
	မဟုတ်ပါက အကယ်၍ (!add && wp->w_vsep_width == 1)
	{
	    win_new_width(wp, wp->w_width + 1);
	    wp->w_vsep_width = 0;
	}
    }
    မဟုတ်ပါက အကယ်၍ (frp->fr_layout == FR_COL)
    {
	// Handle all the frames in the column.
	FOR_ALL_FRAMES(frp, frp->fr_child)
	    frame_set_vsep(frp, add);
    }
    မဟုတ်ပါက // frp->fr_layout == FR_ROW
    {
	// Only need to handle the last frame in the row.
	frp = frp->fr_child;
	နေစဉ် (frp->fr_next != NULL)
	    frp = frp->fr_next;
	frame_set_vsep(frp, add);
    }
}

/*
 * Set frame width from the window it contains.
 */
    static void
frame_fix_width(win_T *wp)
{
    wp->w_frame->fr_width = wp->w_width + wp->w_vsep_width;
}

/*
 * Set frame height from the window it contains.
 */
    static void
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
    static ကိန်းပြည့်
frame_minheight(frame_T *topfrp, win_T *next_curwin)
{
    frame_T	*frp;
    ကိန်းပြည့်		m;
    ကိန်းပြည့်		n;

    အကယ်၍ (topfrp->fr_win != NULL)
    {
	အကယ်၍ (topfrp->fr_win == next_curwin)
	    m = p_wh + topfrp->fr_win->w_status_height;
	မဟုတ်ပါက
	{
	    // window: minimal height of the window plus status line
	    m = p_wmh + topfrp->fr_win->w_status_height;
	    အကယ်၍ (topfrp->fr_win == curwin && next_curwin == NULL)
	    {
		// Current window is minimal one line high and WinBar is
		// visible.
		အကယ်၍ (p_wmh == 0)
		    ++m;
		m += WINBAR_HEIGHT(curwin);
	    }
	}
    }
    မဟုတ်ပါက အကယ်၍ (topfrp->fr_layout == FR_ROW)
    {
	// get the minimal height from each frame in this row
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minheight(frp, next_curwin);
	    အကယ်၍ (n > m)
		m = n;
	}
    }
    မဟုတ်ပါက
    {
	// Add up the minimal heights for all frames in this column.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minheight(frp, next_curwin);
    }

    ပြန်ပေး m;
}

/*
 * Compute the window count in height for frame "topfrp".
 */
static ကိန်းပြည့်
frame_wincount_in_height(frame_T *topfrp)
{
    frame_T	*frp;
    ကိန်းပြည့်		c = 0;
    ကိန်းပြည့်		n;

    အကယ်၍ (topfrp->fr_win != NULL)
	c = 1;
    မဟုတ်ပါက အကယ်၍ (topfrp->fr_layout == FR_ROW)
    {
	// get the max window count across all frames in this row
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_wincount_in_height(frp);
	    အကယ်၍ (n > c)
		c = n;
	}
    }
    မဟုတ်ပါက
    {
	// Sum the window counts for all frames in this column.
	c = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    c += frame_wincount_in_height(frp);
    }

    ပြန်ပေး c;
}

/*
 * Compute the minimal width for frame "topfrp".
 * When "next_curwin" isn't NULL, use p_wiw for this window.
 * When "next_curwin" is NOWIN, don't use at least one column for the current
 * window.
 */
    static ကိန်းပြည့်
frame_minwidth(
    frame_T	*topfrp,
    win_T	*next_curwin)	// use p_wh and p_wiw for next_curwin
{
    frame_T	*frp;
    ကိန်းပြည့်		m, n;

    အကယ်၍ (topfrp->fr_win != NULL)
    {
	အကယ်၍ (topfrp->fr_win == next_curwin)
	    m = p_wiw + topfrp->fr_win->w_vsep_width;
	မဟုတ်ပါက
	{
	    // window: minimal width of the window plus separator column
	    m = p_wmw + topfrp->fr_win->w_vsep_width;
	    // Current window is minimal one column wide
	    အကယ်၍ (p_wmw == 0 && topfrp->fr_win == curwin && next_curwin == NULL)
		++m;
	}
    }
    မဟုတ်ပါက အကယ်၍ (topfrp->fr_layout == FR_COL)
    {
	// get the minimal width from each frame in this column
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    n = frame_minwidth(frp, next_curwin);
	    အကယ်၍ (n > m)
		m = n;
	}
    }
    မဟုတ်ပါက
    {
	// Add up the minimal widths for all frames in this row.
	m = 0;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    m += frame_minwidth(frp, next_curwin);
    }

    ပြန်ပေး m;
}


/*
 * Try to close all windows except current one.
 * Buffers in the other windows become hidden if 'hidden' is set, or '!' is
 * used and the buffer was modified.
 *
 * Used by ":bdel" and ":only".
 */
    void
close_others(
    ကိန်းပြည့်		message,
    ကိန်းပြည့်		forceit)	    // always hide all other windows
{
    win_T	*wp;
    win_T	*nextwp;
    win_T	*old_curwin = curwin;
    ကိန်းပြည့်		r;

    အကယ်၍ (one_window())
    {
	အကယ်၍ (message && !autocmd_busy)
	    msg(_(m_onlyone));
	ပြန်ပေး;
    }

    // Be very careful here: autocommands may change the window layout.
    အတွက် (wp = firstwin; win_valid(wp); wp = nextwp)
    {
	nextwp = wp->w_next;

	// autocommands messed this one up
	အကယ်၍ (old_curwin != curwin && win_valid(old_curwin))
	{
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
	}

	အကယ်၍ (wp == curwin)		// don't close current window
	    continue;

	// autoccommands messed this one up
	အကယ်၍ (!buf_valid(wp->w_buffer) && win_valid(wp))
	{
	    wp->w_buffer = NULL;
	    win_close(wp, 0);
	    continue;
	}
	// Check if it's allowed to abandon this window
	r = can_abandon(wp->w_buffer, forceit);
	အကယ်၍ (!win_valid(wp))		// autocommands messed wp up
	{
	    nextwp = firstwin;
	    continue;
	}
	အကယ်၍ (!r)
	{
#အကယ်၍ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    အကယ်၍ (message && (p_confirm
			 || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
		dialog_changed(wp->w_buffer, FALSE);
		အကယ်၍ (!win_valid(wp))		// autocommands messed wp up
		{
		    nextwp = firstwin;
		    continue;
		}
	    }
	    အကယ်၍ (bufIsChanged(wp->w_buffer))
#endif
		continue;
	}
	win_close(wp, !buf_hide(wp->w_buffer) && !bufIsChanged(wp->w_buffer));
    }

    အကယ်၍ (message && !ONE_WINDOW)
	emsg(_(e_other_window_contains_changes));
}

/*
 * Store the relevant window pointers for tab page "tp".  To be used before
 * use_tabpage().
 */
    void
unuse_tabpage(tabpage_T *tp)
{
    tp->tp_topframe = topframe;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_curwin = curwin;
}

// When switching tabpage, handle other side-effects in command_height(), but
// avoid setting frame sizes which are still correct.
static ကိန်းပြည့် command_frame_height = TRUE;

/*
 * Set the relevant pointers to use tab page "tp".  May want to call
 * unuse_tabpage() first.
 */
    void
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
    ကိန်းပြည့်
win_alloc_first(void)
{
    အကယ်၍ (win_alloc_firstwin(NULL) == FAIL)
	ပြန်ပေး FAIL;

    first_tabpage = alloc_tabpage();
    အကယ်၍ (first_tabpage == NULL)
	ပြန်ပေး FAIL;
    curtab = first_tabpage;
    unuse_tabpage(first_tabpage);

    ပြန်ပေး OK;
}

/*
 * Allocate and init a window that is not a regular window.
 * This can only be done after the first window is fully initialized, thus it
 * can't be in win_alloc_first().
 */
    win_T *
win_alloc_popup_win(void)
{
    win_T *wp;

    wp = win_alloc(NULL, TRUE);
    အကယ်၍ (wp == NULL)
	ပြန်ပေး NULL;
    // We need to initialize options with something, using the current
    // window makes most sense.
    win_init_some(wp, curwin);

    RESET_BINDING(wp);
    new_frame(wp);
    ပြန်ပေး wp;
}

/*
 * Initialize window "wp" to display buffer "buf".
 */
    void
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
    static ကိန်းပြည့်
win_alloc_firstwin(win_T *oldwin)
{
    curwin = win_alloc(NULL, FALSE);
    အကယ်၍ (curwin == NULL)
	ပြန်ပေး FAIL;
    အကယ်၍ (oldwin == NULL)
    {
	// Very first window, need to create an empty buffer for it and
	// initialize from scratch.
	curbuf = buflist_new(NULL, NULL, 1L, BLN_LISTED);
	အကယ်၍ (curwin == NULL || curbuf == NULL)
	    ပြန်ပေး FAIL;
	curwin->w_buffer = curbuf;
#ifdef FEAT_SYN_HL
	curwin->w_s = &(curbuf->b_s);
#endif
	curbuf->b_nwindows = 1;	// there is one window
	curwin->w_alist = &global_alist;
	curwin_init();		// init current window
    }
    မဟုတ်ပါက
    {
	// First window in new tab page, initialize it from "oldwin".
	win_init(curwin, oldwin, 0);

	// We don't want cursor- and scroll-binding in the first window.
	RESET_BINDING(curwin);
    }

    new_frame(curwin);
    အကယ်၍ (curwin->w_frame == NULL)
	ပြန်ပေး FAIL;
    topframe = curwin->w_frame;
    topframe->fr_width = COLUMNS_WITHOUT_TPL();
    topframe->fr_height = Rows - p_ch;

    ပြန်ပေး OK;
}

/*
 * Create a frame for window "wp".
 */
    static void
new_frame(win_T *wp)
{
    frame_T *frp = ALLOC_CLEAR_ONE(frame_T);

    wp->w_frame = frp;
    အကယ်၍ (frp == NULL)
	ပြန်ပေး;
    frp->fr_layout = FR_LEAF;
    frp->fr_win = wp;
}

/*
 * Initialize the window and frame size to the maximum.
 */
    void
win_init_size(void)
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
    static tabpage_T *
alloc_tabpage(void)
{
    tabpage_T	*tp;
#ifdef FEAT_GUI
    ကိန်းပြည့်		i;
#endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    အကယ်၍ (tp == NULL)
	ပြန်ပေး NULL;

#ifdef FEAT_EVAL
    // init t: variables
    tp->tp_vars = dict_alloc_id(aid_newtabpage_tvars);
    အကယ်၍ (tp->tp_vars == NULL)
    {
	vim_free(tp);
	ပြန်ပေး NULL;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
#endif

#ifdef FEAT_GUI
    အတွက် (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
#endif
#ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
#endif
    tp->tp_ch_used = p_ch;

    ပြန်ပေး tp;
}

    void
free_tabpage(tabpage_T *tp)
{
    ကိန်းပြည့် idx;

#ifdef FEAT_DIFF
    diff_clear(tp);
#endif
#ifdef FEAT_PROP_POPUP
    နေစဉ် (tp->tp_first_popupwin != NULL)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id, TRUE);
#endif
    အတွက် (idx = 0; idx < SNAP_COUNT; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	// free all t: variables
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    အကယ်၍ (tp == lastused_tabpage)
	lastused_tabpage = NULL;
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
    ကိန်းပြည့်
win_new_tabpage(ကိန်းပြည့် after)
{
    tabpage_T	*tp = curtab;
    tabpage_T	*prev_tp = curtab;
    tabpage_T	*newtp;
    ကိန်းပြည့်		n;
#အကယ်၍ defined(FEAT_TABPANEL)
    ကိန်းပြည့် prev_columns = COLUMNS_WITHOUT_TPL();
#endif

    အကယ်၍ (cmdwin_type != 0)
    {
	emsg(_(e_invalid_in_cmdline_window));
	ပြန်ပေး FAIL;
    }
    အကယ်၍ (window_layout_locked(CMD_tabnew))
	ပြန်ပေး FAIL;

    newtp = alloc_tabpage();
    အကယ်၍ (newtp == NULL)
	ပြန်ပေး FAIL;

    // Remember the current windows in this Tab page.
    အကယ်၍ (leave_tabpage(curbuf, TRUE) == FAIL)
    {
	vim_free(newtp);
	ပြန်ပေး FAIL;
    }
    curtab = newtp;

    newtp->tp_localdir = (tp->tp_localdir == NULL)
				    ? NULL : vim_strsave(tp->tp_localdir);

    // Create a new empty window.
    အကယ်၍ (win_alloc_firstwin(tp->tp_curwin) == OK)
    {
	// Make the new Tab page the new topframe.
	အကယ်၍ (after == 1)
	{
	    // New tab page becomes the first one.
	    newtp->tp_next = first_tabpage;
	    first_tabpage = newtp;
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (after > 0)
	    {
		// Put new tab page before tab page "after".
		n = 2;
		အတွက် (tp = first_tabpage; tp->tp_next != NULL
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

#အကယ်၍ defined(FEAT_GUI)
	// When 'guioptions' includes 'L' or 'R' may have to remove or add
	// scrollbars.  Have to update them anyway.
	gui_may_update_scrollbars();
#endif
#ifdef FEAT_JOB_CHANNEL
	entering_window(curwin);
#endif
#အကယ်၍ defined(FEAT_TABPANEL)
	အကယ်၍ (prev_columns != COLUMNS_WITHOUT_TPL())
	{
	    tabpage_T	*tp2;
	    ကိန်းပြည့်		w = COLUMNS_WITHOUT_TPL();

	    shell_new_columns();
	    // shell_new_columns() only updates the current tab page; fix up
	    // all others.
	    FOR_ALL_TABPAGES(tp2)
		အကယ်၍ (tp2 != curtab)
		{
		    frame_new_width(tp2->tp_topframe, w, FALSE, TRUE);
		    အကယ်၍ (!frame_check_width(tp2->tp_topframe, w))
			frame_new_width(tp2->tp_topframe, w, FALSE, FALSE);
		}
	}
#endif
	redraw_all_later(UPD_NOT_VALID);
	apply_autocmds(EVENT_WINNEW, NULL, NULL, FALSE, curbuf);
	apply_autocmds(EVENT_WINENTER, NULL, NULL, FALSE, curbuf);
	apply_autocmds(EVENT_TABNEW, NULL, NULL, FALSE, curbuf);
	apply_autocmds(EVENT_TABENTER, NULL, NULL, FALSE, curbuf);
	ပြန်ပေး OK;
    }

    // Failed, get back the previous Tab page
    enter_tabpage(curtab, curbuf, TRUE, TRUE);
    ပြန်ပေး FAIL;
}

/*
 * Open a new tab page if ":tab cmd" was used.  It will edit the same buffer,
 * like with ":split".
 * Returns OK if a new tab page was created, FAIL otherwise.
 */
    static ကိန်းပြည့်
may_open_tabpage(void)
{
    ကိန်းပြည့်		n = (cmdmod.cmod_tab == 0)
				       ? postponed_split_tab : cmdmod.cmod_tab;

    အကယ်၍ (n == 0)
	ပြန်ပေး FAIL;

    cmdmod.cmod_tab = 0;	    // reset it to avoid doing it twice
    postponed_split_tab = 0;
    ပြန်ပေး win_new_tabpage(n);
}

/*
 * Create up to "maxcount" tabpages with empty windows.
 * Returns the number of resulting tab pages.
 */
    ကိန်းပြည့်
make_tabpages(ကိန်းပြည့် maxcount)
{
    ကိန်းပြည့်		count = maxcount;
    ကိန်းပြည့်		todo;

    // Limit to 'tabpagemax' tabs.
    အကယ်၍ (count > p_tpm)
	count = p_tpm;

    /*
     * Don't execute autocommands while creating the tab pages.  Must do that
     * when putting the buffers in the windows.
     */
    block_autocmds();

    အတွက် (todo = count - 1; todo > 0; --todo)
	အကယ်၍ (win_new_tabpage(0) == FAIL)
	    bha_nga;

    unblock_autocmds();

    // return actual number of tab pages
    ပြန်ပေး (count - todo);
}

/*
 * Return TRUE when "tpc" points to a valid tab page.
 */
    ကိန်းပြည့်
valid_tabpage(tabpage_T *tpc)
{
    tabpage_T	*tp;

    FOR_ALL_TABPAGES(tp)
	အကယ်၍ (tp == tpc)
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

/*
 * Return TRUE when "tpc" points to a valid tab page and at least one window is
 * valid.
 */
    ကိန်းပြည့်
valid_tabpage_win(tabpage_T *tpc)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TABPAGES(tp)
    {
	အကယ်၍ (tp == tpc)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    {
		အကယ်၍ (win_valid_any_tab(wp))
		    ပြန်ပေး TRUE;
	    }
	    ပြန်ပေး FALSE;
	}
    }
    // shouldn't happen
    ပြန်ပေး FALSE;
}

/*
 * Close tabpage "tab", assuming it has no windows in it.
 * There must be another tabpage or this will crash.
 */
    void
close_tabpage(tabpage_T *tab)
{
    tabpage_T	*ptp;

    အကယ်၍ (tab == first_tabpage)
    {
	first_tabpage = tab->tp_next;
	ptp = first_tabpage;
    }
    မဟုတ်ပါက
    {
	အတွက် (ptp = first_tabpage; ptp != NULL && ptp->tp_next != tab;
							    ptp = ptp->tp_next)
	    ;
	assert(ptp != NULL);
	ptp->tp_next = tab->tp_next;
    }

    goto_tabpage_tp(ptp, FALSE, FALSE);
    free_tabpage(tab);
}

/*
 * Find tab page "n" (first one is 1).  Returns NULL when not found.
 */
    tabpage_T *
find_tabpage(ကိန်းပြည့် n)
{
    tabpage_T	*tp;
    ကိန်းပြည့်		i = 1;

    အကယ်၍ (n == 0)
	ပြန်ပေး curtab;

    အတွက် (tp = first_tabpage; tp != NULL && i != n; tp = tp->tp_next)
	++i;
    ပြန်ပေး tp;
}

/*
 * Get index of tab page "tp".  First one has index 1.
 * When not found returns number of tab pages plus one.
 */
    ကိန်းပြည့်
tabpage_index(tabpage_T *ftp)
{
    ကိန်းပြည့်		i = 1;
    tabpage_T	*tp;

    အတွက် (tp = first_tabpage; tp != NULL && tp != ftp; tp = tp->tp_next)
	++i;
    ပြန်ပေး i;
}

/*
 * Prepare for leaving the current tab page.
 * When autocommands change "curtab" we don't leave the tab page and return
 * FAIL.
 * Careful: When OK is returned need to get a new tab page very very soon!
 */
    static ကိန်းပြည့်
leave_tabpage(
    buf_T	*new_curbuf,		// what is going to be the new curbuf,
					// NULL if unknown
    ကိန်းပြည့်		trigger_leave_autocmds)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	// stop Visual mode
    အကယ်၍ (trigger_leave_autocmds)
    {
	အကယ်၍ (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf);
	    အကယ်၍ (curtab != tp)
		ပြန်ပေး FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, NULL, NULL, FALSE, curbuf);
	အကယ်၍ (curtab != tp)
	    ပြန်ပေး FAIL;
	apply_autocmds(EVENT_TABLEAVE, NULL, NULL, FALSE, curbuf);
	အကယ်၍ (curtab != tp)
	    ပြန်ပေး FAIL;
    }

    reset_dragwin();
#အကယ်၍ defined(FEAT_GUI)
    // Remove the scrollbars.  They may be added back later.
    အကယ်၍ (gui.in_use)
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
    အကယ်၍ (tp->tp_old_Columns != -1)
    {
	tp->tp_old_Columns = topframe->fr_width;
	tp->tp_old_coloff = firstwin->w_wincol;
    }
    firstwin = NULL;
    lastwin = NULL;
    ပြန်ပေး OK;
}

/*
 * Start using tab page "tp".
 * Only to be used after leave_tabpage() or freeing the current tab page.
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 */
    static void
enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf,
    ကိန်းပြည့်		trigger_enter_autocmds,
    ကိန်းပြည့်		trigger_leave_autocmds)
{
    ကိန်းပြည့်		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;
    tabpage_T	*last_tab = curtab;

    use_tabpage(tp);

    အကယ်၍ (p_ch != curtab->tp_ch_used)
    {
	// Use the stored value of p_ch, so that it can be different for each
	// tab page.
	ကိန်းပြည့် new_ch = curtab->tp_ch_used;
	curtab->tp_ch_used = p_ch;
	command_frame_height = FALSE;
	set_option_value((char_u *)"cmdheight", new_ch, NULL, 0);
	command_frame_height = TRUE;
    }

    // We would like doing the TabEnter event first, but we don't have a
    // valid current window yet, which may break some commands.
    // This triggers autocommands, thus may make "tp" invalid.
    (void)win_enter_ext(tp->tp_curwin, WEE_CURWIN_INVALID
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
    အကယ်၍ (curtab->tp_old_Rows != ROWS_AVAIL || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    အကယ်၍ (curtab->tp_old_Columns != COLUMNS_WITHOUT_TPL()
	    || curtab->tp_old_coloff != TPL_LCOL())
    {
	အကယ်၍ (starting == 0)
	{
	    shell_new_columns();	// update window widths
	    curtab->tp_old_Columns = topframe->fr_width;
	    curtab->tp_old_coloff = firstwin->w_wincol;
	}
	မဟုတ်ပါက
	    curtab->tp_old_Columns = -1;  // update window widths later
    }

    lastused_tabpage = last_tab;

#အကယ်၍ defined(FEAT_GUI)
    // When 'guioptions' includes 'L' or 'R' may have to remove or add
    // scrollbars.  Have to update them anyway.
    gui_may_update_scrollbars();
#endif

    // Apply autocommands after updating the display, when 'rows' and
    // 'columns' have been set correctly.
    အကယ်၍ (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, NULL, NULL, FALSE, curbuf);
	အကယ်၍ (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
    }

    redraw_all_later(UPD_NOT_VALID);
}

/*
 * Go to tab page "n".  For ":tab N" and "Ngt".
 * When "n" is 9999 go to the last tab page.
 */
    void
goto_tabpage(ကိန်းပြည့် n)
{
    tabpage_T	*tp = NULL;  // shut up compiler
    tabpage_T	*ttp;
    ကိန်းပြည့်		i;

    အကယ်၍ (text_locked())
    {
	// Not allowed when editing the command line.
	text_locked_msg();
	ပြန်ပေး;
    }

    // If there is only one it can't work.
    အကယ်၍ (first_tabpage->tp_next == NULL)
    {
	အကယ်၍ (n > 1)
	    beep_flush();
	ပြန်ပေး;
    }

    အကယ်၍ (n == 0)
    {
	// No count, go to next tab page, wrap around end.
	အကယ်၍ (curtab->tp_next == NULL)
	    tp = first_tabpage;
	မဟုတ်ပါက
	    tp = curtab->tp_next;
    }
    မဟုတ်ပါက အကယ်၍ (n < 0)
    {
	// "gT": go to previous tab page, wrap around end.  "N gT" repeats
	// this N times.
	ttp = curtab;
	အတွက် (i = n; i < 0; ++i)
	{
	    အတွက် (tp = first_tabpage; tp->tp_next != ttp && tp->tp_next != NULL;
		    tp = tp->tp_next)
		;
	    ttp = tp;
	}
    }
    မဟုတ်ပါက အကယ်၍ (n == 9999)
    {
	// Go to last tab page.
	အတွက် (tp = first_tabpage; tp->tp_next != NULL; tp = tp->tp_next)
	    ;
    }
    မဟုတ်ပါက
    {
	// Go to tab page "n".
	tp = find_tabpage(n);
	အကယ်၍ (tp == NULL)
	{
	    beep_flush();
	    ပြန်ပေး;
	}
    }

    goto_tabpage_tp(tp, TRUE, TRUE);

#ifdef FEAT_GUI_TABLINE
    အကယ်၍ (gui_use_tabline())
	gui_mch_set_curtab(tabpage_index(curtab));
#endif
}

/*
 * Go to tabpage "tp".
 * Only trigger *Enter autocommands when trigger_enter_autocmds is TRUE.
 * Only trigger *Leave autocommands when trigger_leave_autocmds is TRUE.
 * Note: doesn't update the GUI tab.
 */
    void
goto_tabpage_tp(
    tabpage_T	*tp,
    ကိန်းပြည့်		trigger_enter_autocmds,
    ကိန်းပြည့်		trigger_leave_autocmds)
{
    အကယ်၍ (trigger_enter_autocmds || trigger_leave_autocmds)
	CHECK_CMDWIN;

    // Don't repeat a message in another tab page.
    set_keep_msg(NULL, 0);

    skip_win_fix_scroll = TRUE;
    အကယ်၍ (tp != curtab && leave_tabpage(tp->tp_curwin->w_buffer,
					trigger_leave_autocmds) == OK)
    {
	အကယ်၍ (valid_tabpage(tp))
	    enter_tabpage(tp, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
	မဟုတ်ပါက
	    enter_tabpage(curtab, curbuf, trigger_enter_autocmds,
		    trigger_leave_autocmds);
    }
    skip_win_fix_scroll = FALSE;
}

/*
 * Go to the last accessed tab page, if there is one.
 * Return OK or FAIL
 */
    ကိန်းပြည့်
goto_tabpage_lastused(void)
{
    အကယ်၍ (!valid_tabpage(lastused_tabpage))
	ပြန်ပေး FAIL;

    goto_tabpage_tp(lastused_tabpage, TRUE, TRUE);
    ပြန်ပေး OK;
}

/*
 * Enter window "wp" in tab page "tp".
 * Also updates the GUI tab.
 */
    void
goto_tabpage_win(tabpage_T *tp, win_T *wp)
{
    goto_tabpage_tp(tp, TRUE, TRUE);
    အကယ်၍ (curtab == tp && win_valid(wp))
    {
	win_enter(wp, TRUE);
#ifdef FEAT_GUI_TABLINE
	အကယ်၍ (gui_use_tabline())
	    gui_mch_set_curtab(tabpage_index(curtab));
#endif
    }
}

/*
 * Move the current tab page to after tab page "nr".
 */
    void
tabpage_move(ကိန်းပြည့် nr)
{
    ကိန်းပြည့်		n = 1;
    tabpage_T	*tp, *tp_dst;

    အကယ်၍ (first_tabpage->tp_next == NULL)
	ပြန်ပေး;

    အကယ်၍ (tabpage_move_disallowed)
	ပြန်ပေး;

    အတွက် (tp = first_tabpage; tp->tp_next != NULL && n < nr; tp = tp->tp_next)
	++n;

    အကယ်၍ (tp == curtab || (nr > 0 && tp->tp_next != NULL
						    && tp->tp_next == curtab))
	ပြန်ပေး;

    tp_dst = tp;

    // Remove the current tab page from the list of tab pages.
    အကယ်၍ (curtab == first_tabpage)
	first_tabpage = curtab->tp_next;
    မဟုတ်ပါက
    {
	FOR_ALL_TABPAGES(tp)
	    အကယ်၍ (tp->tp_next == curtab)
		bha_nga;
	အကယ်၍ (tp == NULL)	// "cannot happen"
	    ပြန်ပေး;
	tp->tp_next = curtab->tp_next;
    }

    // Re-insert it at the specified position.
    အကယ်၍ (nr <= 0)
    {
	curtab->tp_next = first_tabpage;
	first_tabpage = curtab;
    }
    မဟုတ်ပါက
    {
	curtab->tp_next = tp_dst->tp_next;
	tp_dst->tp_next = curtab;
    }

    // Need to redraw the tabline.  Tab page contents doesn't change.
    redraw_tabline = TRUE;
#အကယ်၍ defined(FEAT_TABPANEL)
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
    void
win_goto(win_T *wp)
{
#ifdef FEAT_CONCEAL
    win_T	*owp = curwin;
#endif

#ifdef FEAT_PROP_POPUP
    အကယ်၍ (ERROR_IF_ANY_POPUP_WINDOW)
	ပြန်ပေး;
    အကယ်၍ (popup_is_popup(wp))
    {
	emsg(_(e_not_allowed_to_enter_popup_window));
	ပြန်ပေး;
    }
#endif
    အကယ်၍ (text_or_buf_locked())
    {
	beep_flush();
	ပြန်ပေး;
    }

    အကယ်၍ (wp->w_buffer != curbuf)
	// careful: triggers ModeChanged autocommand
	reset_VIsual_and_resel();
    မဟုတ်ပါက အကယ်၍ (VIsual_active)
	wp->w_cursor = curwin->w_cursor;

    // autocommand may have made wp invalid
    အကယ်၍ (!win_valid(wp))
	ပြန်ပေး;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    win_enter(wp, TRUE);

#ifdef FEAT_CONCEAL
    // Conceal cursor line in previous window, unconceal in current window.
    အကယ်၍ (win_valid(owp) && owp->w_p_cole > 0 && !msg_scrolled)
	redrawWinline(owp, owp->w_cursor.lnum);
    အကယ်၍ (curwin->w_p_cole > 0 && !msg_scrolled)
	need_cursor_line_redraw = TRUE;
#endif
}

#အကယ်၍ defined(FEAT_PERL)
/*
 * Find window number "winnr" (counting top to bottom).
 */
    win_T *
win_find_nr(ကိန်းပြည့် winnr)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	အကယ်၍ (--winnr == 0)
	    bha_nga;
    ပြန်ပေး wp;
}
#endif

#အကယ်၍ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Find the tabpage for window "win".
 */
    tabpage_T *
win_find_tabpage(win_T *win)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    အကယ်၍ (wp == win)
		ပြန်ပေး tp;
    ပြန်ပေး NULL;
}
#endif

/*
 * Get the above or below neighbor window of the specified window.
 *   up - TRUE for the above neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_vert_neighbor(tabpage_T *tp, win_T *wp, ကိန်းပြည့် up, long count)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    အကယ်၍ (popup_is_popup(wp))
	// popups don't have neighbors.
	ပြန်ပေး NULL;
#endif
    foundfr = wp->w_frame;
    နေစဉ် (count--)
    {
	/*
	 * First go upwards in the tree of frames until we find a upwards or
	 * downwards neighbor.
	 */
	fr = foundfr;
	အတွက် (;;)
	{
	    အကယ်၍ (fr == tp->tp_topframe)
		goto end;
	    အကယ်၍ (up)
		nfr = fr->fr_prev;
	    မဟုတ်ပါက
		nfr = fr->fr_next;
	    အကယ်၍ (fr->fr_parent->fr_layout == FR_COL && nfr != NULL)
		bha_nga;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the bottom or top frame in it.
	 */
	အတွက် (;;)
	{
	    အကယ်၍ (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		bha_nga;
	    }
	    fr = nfr->fr_child;
	    အကယ်၍ (nfr->fr_layout == FR_ROW)
	    {
		// Find the frame at the cursor row.
		နေစဉ် (fr->fr_next != NULL
			&& frame2win(fr)->w_wincol + fr->fr_width
					 <= wp->w_wincol + wp->w_wcol)
		    fr = fr->fr_next;
	    }
	    အကယ်၍ (nfr->fr_layout == FR_COL && up)
		နေစဉ် (fr->fr_next != NULL)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    ပြန်ပေး foundfr != NULL ? foundfr->fr_win : NULL;
}

/*
 * Move to window above or below "count" times.
 */
    static void
win_goto_ver(
    ကိန်းပြည့်		up,		// TRUE to go to win above
    long	count)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    အကယ်၍ (ERROR_IF_TERM_POPUP_WINDOW)
	ပြန်ပေး;
#endif
    win = win_vert_neighbor(curtab, curwin, up, count);
    အကယ်၍ (win != NULL)
	win_goto(win);
}

/*
 * Get the left or right neighbor window of the specified window.
 *   left - TRUE for the left neighbor
 *   count - nth neighbor window
 * Returns the specified window if the neighbor is not found.
 */
    win_T *
win_horz_neighbor(tabpage_T *tp, win_T *wp, ကိန်းပြည့် left, long count)
{
    frame_T	*fr;
    frame_T	*nfr;
    frame_T	*foundfr;

#ifdef FEAT_PROP_POPUP
    အကယ်၍ (popup_is_popup(wp))
	// popups don't have neighbors.
	ပြန်ပေး NULL;
#endif
    foundfr = wp->w_frame;
    နေစဉ် (count--)
    {
	/*
	 * First go upwards in the tree of frames until we find a left or
	 * right neighbor.
	 */
	fr = foundfr;
	အတွက် (;;)
	{
	    အကယ်၍ (fr == tp->tp_topframe)
		goto end;
	    အကယ်၍ (left)
		nfr = fr->fr_prev;
	    မဟုတ်ပါက
		nfr = fr->fr_next;
	    အကယ်၍ (fr->fr_parent->fr_layout == FR_ROW && nfr != NULL)
		bha_nga;
	    fr = fr->fr_parent;
	}

	/*
	 * Now go downwards to find the leftmost or rightmost frame in it.
	 */
	အတွက် (;;)
	{
	    အကယ်၍ (nfr->fr_layout == FR_LEAF)
	    {
		foundfr = nfr;
		bha_nga;
	    }
	    fr = nfr->fr_child;
	    အကယ်၍ (nfr->fr_layout == FR_COL)
	    {
		// Find the frame at the cursor row.
		နေစဉ် (fr->fr_next != NULL
			&& frame2win(fr)->w_winrow + fr->fr_height
					 <= wp->w_winrow + wp->w_wrow)
		    fr = fr->fr_next;
	    }
	    အကယ်၍ (nfr->fr_layout == FR_ROW && left)
		နေစဉ် (fr->fr_next != NULL)
		    fr = fr->fr_next;
	    nfr = fr;
	}
    }
end:
    ပြန်ပေး foundfr != NULL ? foundfr->fr_win : NULL;
}

/*
 * Move to left or right window.
 */
    static void
win_goto_hor(
    ကိန်းပြည့်		left,		// TRUE to go to left win
    long	count)
{
    win_T	*win;

#ifdef FEAT_PROP_POPUP
    အကယ်၍ (ERROR_IF_TERM_POPUP_WINDOW)
	ပြန်ပေး;
#endif
    win = win_horz_neighbor(curtab, curwin, left, count);
    အကယ်၍ (win != NULL)
	win_goto(win);
}

/*
 * Make window "wp" the current window.
 */
    void
win_enter(win_T *wp, ကိန်းပြည့် undo_sync)
{
    (void)win_enter_ext(wp, (undo_sync ? WEE_UNDO_SYNC : 0)
		    | WEE_TRIGGER_ENTER_AUTOCMDS | WEE_TRIGGER_LEAVE_AUTOCMDS);
}

/*
 * Used after making another window the current one: change directory if
 * needed.
 */
    void
win_fix_current_dir(void)
{
    အကယ်၍ (curwin->w_localdir != NULL || curtab->tp_localdir != NULL)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	အကယ်၍ (globaldir == NULL)
	{
	    char_u	cwd[MAXPATHL];

	    အကယ်၍ (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	အကယ်၍ (curwin->w_localdir != NULL)
	    dirname = curwin->w_localdir;
	မဟုတ်ပါက
	    dirname = curtab->tp_localdir;

	အကယ်၍ (mch_chdir((var_na *)dirname) == 0)
	{
	    last_chdir_reason = NULL;
	    shorten_fnames(TRUE);
	}
    }
    မဟုတ်ပါက အကယ်၍ (globaldir != NULL)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((var_na *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = NULL;
	shorten_fnames(TRUE);
    }
}

/*
 * Make window "wp" the current window.
 * Can be called with "flags" containing WEE_CURWIN_INVALID, which means that
 * curwin has just been closed and isn't valid.
 * Returns TRUE when dont_parse_messages was decremented.
 */
    static ကိန်းပြည့်
win_enter_ext(win_T *wp, ကိန်းပြည့် flags)
{
    ကိန်းပြည့်		other_buffer = FALSE;
    ကိန်းပြည့်		curwin_invalid = (flags & WEE_CURWIN_INVALID);
    ကိန်းပြည့်		did_decrement = FALSE;

    အကယ်၍ (wp == curwin && curwin_invalid == 0)	// nothing to do
	ပြန်ပေး FALSE;

#ifdef FEAT_JOB_CHANNEL
    အကယ်၍ (curwin_invalid == 0)
	leaving_window(curwin);
#endif

    အကယ်၍ (curwin_invalid == 0 && (flags & WEE_TRIGGER_LEAVE_AUTOCMDS))
    {
	/*
	 * Be careful: If autocommands delete the window, return now.
	 */
	အကယ်၍ (wp->w_buffer != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf);
	    other_buffer = TRUE;
	    အကယ်၍ (!win_valid(wp))
		ပြန်ပေး FALSE;
	}
	apply_autocmds(EVENT_WINLEAVE, NULL, NULL, FALSE, curbuf);
	အကယ်၍ (!win_valid(wp))
	    ပြန်ပေး FALSE;
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	အကယ်၍ (aborting())
	    ပြန်ပေး FALSE;
#endif
    }

    // sync undo before leaving the current buffer
    အကယ်၍ ((flags & WEE_UNDO_SYNC) && curbuf != wp->w_buffer)
	u_sync(FALSE);

    // Might need to scroll the old window before switching, e.g., when the
    // cursor was moved.
    အကယ်၍ (*p_spk == 'c' && curwin_invalid == 0)
	update_topline();

    // may have to copy the buffer options when 'cpo' contains 'S'
    အကယ်၍ (wp->w_buffer != curbuf)
	buf_copy_options(wp->w_buffer, BCO_ENTER | BCO_NOHELP);
    အကယ်၍ (curwin_invalid == 0)
    {
	prevwin = curwin;	// remember for CTRL-W p
	curwin->w_redr_status = true;
    }
    curwin = wp;
    curbuf = wp->w_buffer;
    check_cursor();
    အကယ်၍ (!virtual_active())
	curwin->w_cursor.coladd = 0;
    အကယ်၍ (*p_spk == 'c')		// assume cursor position needs updating
	changed_line_abv_curs();
    မဟုတ်ပါက
	// Make sure the cursor position is valid, either by moving the cursor
	// or by scrolling the text.
	win_fix_cursor(
		get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL));

    // Now it is OK to parse messages again, which may be needed in
    // autocommands.
#ifdef MESSAGE_QUEUE
    အကယ်၍ (flags & WEE_ALLOW_PARSE_MESSAGES)
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
    အကယ်၍ (flags & WEE_TRIGGER_NEW_AUTOCMDS)
	apply_autocmds(EVENT_WINNEW, NULL, NULL, FALSE, curbuf);
    အကယ်၍ (flags & WEE_TRIGGER_ENTER_AUTOCMDS)
    {
	apply_autocmds(EVENT_WINENTER, NULL, NULL, FALSE, curbuf);
	အကယ်၍ (other_buffer)
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
    }

    maketitle();
    curwin->w_redr_status = true;
#ifdef FEAT_TERMINAL
    အကယ်၍ (bt_terminal(curwin->w_buffer))
	// terminal is likely in another mode
	redraw_mode = TRUE;
#endif
    redraw_tabline = TRUE;
    redraw_vseps = TRUE;
#အကယ်၍ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    // Need to schedule a redraw so that the vertical separator highlight is
    // updated for the new current window.  The status line redraw of curwin
    // is already requested via "curwin->w_redr_status".
    redraw_later(UPD_VALID);
    အကယ်၍ (restart_edit)
	redraw_later(UPD_VALID);	// causes status line redraw

    // set window height to desired minimal value
    အကယ်၍ (curwin->w_height < p_wh && !curwin->w_p_wfh
#ifdef FEAT_PROP_POPUP
	    && !popup_is_popup(curwin)
#endif
	    )
	win_setheight((ကိန်းပြည့်)p_wh);
    မဟုတ်ပါက အကယ်၍ (curwin->w_height == 0)
	win_setheight(1);

    // set window width to desired minimal value
    အကယ်၍ (curwin->w_width < p_wiw && !curwin->w_p_wfw)
	win_setwidth((ကိန်းပြည့်)p_wiw);

    setmouse();			// in case jumped to/from help buffer

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

    ပြန်ပေး did_decrement;
}

/*
 * Jump to the first open window that contains buffer "buf", if one exists.
 * Returns a pointer to the window found, otherwise NULL.
 */
    win_T *
buf_jump_open_win(buf_T *buf)
{
    win_T	*wp = NULL;

    အကယ်၍ (curwin->w_buffer == buf)
	wp = curwin;
    မဟုတ်ပါက
	FOR_ALL_WINDOWS(wp)
	    အကယ်၍ (wp->w_buffer == buf)
		bha_nga;
    အကယ်၍ (wp != NULL)
	win_enter(wp, FALSE);
    ပြန်ပေး wp;
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

    အကယ်၍ (wp != NULL)
	ပြန်ပေး wp;

    FOR_ALL_TABPAGES(tp)
	အကယ်၍ (tp != curtab)
	{
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		အကယ်၍ (wp->w_buffer == buf)
		    bha_nga;
	    အကယ်၍ (wp != NULL)
	    {
		goto_tabpage_win(tp, wp);
		အကယ်၍ (curwin != wp)
		    wp = NULL;	// something went wrong
		bha_nga;
	    }
	}
    ပြန်ပေး wp;
}

static ကိန်းပြည့် last_win_id = LOWEST_WIN_ID - 1;

/*
 * Allocate a window structure and link it in the window list when "hidden" is
 * FALSE.
 */
    static win_T *
win_alloc(win_T *after, ကိန်းပြည့် hidden)
{
    win_T	*new_wp;

    /*
     * allocate window structure and linesizes arrays
     */
    new_wp = ALLOC_CLEAR_ONE(win_T);
    အကယ်၍ (new_wp == NULL)
	ပြန်ပေး NULL;

    အကယ်၍ (win_alloc_lines(new_wp) == FAIL)
    {
	vim_free(new_wp);
	ပြန်ပေး NULL;
    }

    new_wp->w_id = ++last_win_id;

#ifdef FEAT_EVAL
    // init w: variables
    new_wp->w_vars = dict_alloc_id(aid_newwin_wvars);
    အကယ်၍ (new_wp->w_vars == NULL)
    {
	win_free_lsize(new_wp);
	vim_free(new_wp);
	ပြန်ပေး NULL;
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
    အကယ်၍ (!hidden)
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
    အကယ်၍ (gui.in_use)
    {
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_LEFT],
		SBAR_LEFT, new_wp);
	gui_create_scrollbar(&new_wp->w_scrollbars[SBAR_RIGHT],
		SBAR_RIGHT, new_wp);
    }
#endif
#ifdef FEAT_FOLDING
    foldInitWin(new_wp);
#endif
    unblock_autocmds();
#ifdef FEAT_SEARCH_EXTRA
    new_wp->w_next_match_id = 1000;  // up to 1000 can be picked by the user
#endif
    ပြန်ပေး new_wp;
}

/*
 * Remove window 'wp' from the window list and free the structure.
 */
    static void
win_free(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    ကိန်းပြည့်		i;
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
    အကယ်၍ (wp->w_stl_click != NULL)
    {
	အတွက် (i = 0; i < wp->w_stl_click_count; i++)
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

	အကယ်၍ (prevwin == wp)
	    prevwin = NULL;
	FOR_ALL_TABPAGES(ttp)
	    အကယ်၍ (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = NULL;
    }
    win_free_lsize(wp);

    အတွက် (i = 0; i < wp->w_tagstacklen; ++i)
	tagstack_clear_entry(&wp->w_tagstack[i]);
    vim_free(wp->w_localdir);
    vim_free(wp->w_prevdir);

    // Remove the window from the b_wininfo lists, it may happen that the
    // freed memory is re-used for another window.
    FOR_ALL_BUFFERS(buf)
	FOR_ALL_BUF_WININFO(buf, wip)
	    အကယ်၍ (wip->wi_win == wp)
	    {
		wininfo_T	*wip2;

		// If there already is an entry with "wi_win" set to NULL it
		// must be removed, it would never be used.
		// Skip "wip" itself, otherwise Coverity complains.
		FOR_ALL_BUF_WININFO(buf, wip2)
		    အကယ်၍ (wip2 != wip && wip2->wi_win == NULL)
		    {
			အကယ်၍ (wip2->wi_next != NULL)
			    wip2->wi_next->wi_prev = wip2->wi_prev;
			အကယ်၍ (wip2->wi_prev == NULL)
			    buf->b_wininfo = wip2->wi_next;
			မဟုတ်ပါက
			    wip2->wi_prev->wi_next = wip2->wi_next;
			free_wininfo(wip2);
			bha_nga;
		    }

		wip->wi_win = NULL;
	    }

#ifdef FEAT_SEARCH_EXTRA
    clear_matches(wp);
#endif

    free_jumplist(wp);

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    အကယ်၍ (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_LEFT]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_RIGHT]);
    }
#endif // FEAT_GUI

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_PROP_POPUP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    အတွက် (i = 0; i < 4; ++i)
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

    အကယ်၍ (win_valid_any_tab(wp))
	win_remove(wp, tp);
    အကယ်၍ (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    မဟုတ်ပါက
	vim_free(wp);

    unblock_autocmds();
}

/*
 * Return TRUE if "wp" is not in the list of windows: the autocmd window or a
 * popup window.
 */
    ကိန်းပြည့်
win_unlisted(win_T *wp)
{
    ပြန်ပေး is_aucmd_win(wp) || WIN_IS_POPUP(wp);
}

#အကယ်၍ defined(FEAT_PROP_POPUP)
/*
 * Free a popup window.  This does not take the window out of the window list
 * and assumes there is only one toplevel frame, no split.
 */
    void
win_free_popup(win_T *win)
{
    အကယ်၍ (win->w_buffer != NULL)
    {
	အကယ်၍ (bt_popup(win->w_buffer))
	    win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
	မဟုတ်ပါက
	    close_buffer(win, win->w_buffer, 0, FALSE, FALSE, TRUE);

	အကယ်၍ (win->w_buffer != NULL)
	    --win->w_buffer->b_nwindows;
    }
# အကယ်၍ defined(FEAT_TIMERS)
    // the timer may have been cleared, making the pointer invalid
    အကယ်၍ (timer_valid(win->w_popup_timer))
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, NULL);
}
#endif

/*
 * Append window "wp" in the window list after window "after".
 */
    static void
win_append(win_T *after, win_T *wp)
{
    win_T	*before;

    အကယ်၍ (after == NULL)	    // after NULL is in front of the first
	before = firstwin;
    မဟုတ်ပါက
	before = after->w_next;

    wp->w_next = before;
    wp->w_prev = after;
    အကယ်၍ (after == NULL)
	firstwin = wp;
    မဟုတ်ပါက
	after->w_next = wp;
    အကယ်၍ (before == NULL)
	lastwin = wp;
    မဟုတ်ပါက
	before->w_prev = wp;
}

/*
 * Remove a window from the window list.
 */
    void
win_remove(
    win_T	*wp,
    tabpage_T	*tp)		// tab page "win" is in, NULL for current
{
    အကယ်၍ (wp->w_prev != NULL)
	wp->w_prev->w_next = wp->w_next;
    မဟုတ်ပါက အကယ်၍ (tp == NULL)
	firstwin = curtab->tp_firstwin = wp->w_next;
    မဟုတ်ပါက
	tp->tp_firstwin = wp->w_next;

    အကယ်၍ (wp->w_next != NULL)
	wp->w_next->w_prev = wp->w_prev;
    မဟုတ်ပါက အကယ်၍ (tp == NULL)
	lastwin = curtab->tp_lastwin = wp->w_prev;
    မဟုတ်ပါက
	tp->tp_lastwin = wp->w_prev;
}

/*
 * Append frame "frp" in a frame list after frame "after".
 */
    static void
frame_append(frame_T *after, frame_T *frp)
{
    frp->fr_next = after->fr_next;
    after->fr_next = frp;
    အကယ်၍ (frp->fr_next != NULL)
	frp->fr_next->fr_prev = frp;
    frp->fr_prev = after;
}

/*
 * Insert frame "frp" in a frame list before frame "before".
 */
    static void
frame_insert(frame_T *before, frame_T *frp)
{
    frp->fr_next = before;
    frp->fr_prev = before->fr_prev;
    before->fr_prev = frp;
    အကယ်၍ (frp->fr_prev != NULL)
	frp->fr_prev->fr_next = frp;
    မဟုတ်ပါက
	frp->fr_parent->fr_child = frp;
}

/*
 * Remove a frame from a frame list.
 */
    static void
frame_remove(frame_T *frp)
{
    အကယ်၍ (frp->fr_prev != NULL)
	frp->fr_prev->fr_next = frp->fr_next;
    မဟုတ်ပါက
	frp->fr_parent->fr_child = frp->fr_next;
    အကယ်၍ (frp->fr_next != NULL)
	frp->fr_next->fr_prev = frp->fr_prev;
}

/*
 * Allocate w_lines[] for window "wp".
 * Return FAIL for failure, OK for success.
 */
    ကိန်းပြည့်
win_alloc_lines(win_T *wp)
{
    wp->w_lines_valid = 0;
    wp->w_lines = ALLOC_CLEAR_MULT(wline_T, Rows);
    အကယ်၍ (wp->w_lines == NULL)
	ပြန်ပေး FAIL;
    ပြန်ပေး OK;
}

/*
 * free lsize arrays for a window
 */
    void
win_free_lsize(win_T *wp)
{
    // TODO: why would wp be NULL here?
    အကယ်၍ (wp != NULL)
	VIM_CLEAR(wp->w_lines);
}

/*
 * Called from win_new_shellsize() after Rows changed.
 * This only does the current tab page, others must be done when made active.
 * Note: When called together with shell_new_columns(), call shell_new_columns()
 * first to avoid this function updating firstwin->w_wincol first.
 */
    void
shell_new_rows(void)
{
    ကိန်းပြည့်		h = (ကိန်းပြည့်)ROWS_AVAIL;

    အကယ်၍ (firstwin == NULL)	// not initialized yet
	ပြန်ပေး;
    အကယ်၍ (h < frame_minheight(topframe, NULL))
	h = frame_minheight(topframe, NULL);

    // First try setting the heights of windows with 'winfixheight'.  If that
    // doesn't result in the right height, forget about that option.
    frame_new_height(topframe, h, FALSE, TRUE, FALSE);
    အကယ်၍ (!frame_check_height(topframe, h))
	frame_new_height(topframe, h, FALSE, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol
    compute_cmdrow();
    curtab->tp_ch_used = p_ch;

    အကယ်၍ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#အကယ်၍ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#အကယ်၍ 0
    // Disabled: don't want making the screen smaller make a window larger.
    အကယ်၍ (p_ea)
	win_equal(curwin, FALSE, 'v');
#endif
}

/*
 * Called from win_new_shellsize() after Columns changed.
 */
    void
shell_new_columns(void)
{
    အကယ်၍ (firstwin == NULL)	// not initialized yet
	ပြန်ပေး;

#အကယ်၍ defined(FEAT_TABPANEL)
    static ကိန်းပြည့် prev_tp_width;
    static ကိန်းပြည့် prev_wincol;
    static ကိန်းပြည့် prev_fr_width;
#endif
    ကိန်းပြည့် w = COLUMNS_WITHOUT_TPL();

    // First try setting the widths of windows with 'winfixwidth'.  If that
    // doesn't result in the right width, forget about that option.
    frame_new_width(topframe, w, FALSE, TRUE);
    အကယ်၍ (!frame_check_width(topframe, w))
	frame_new_width(topframe, w, FALSE, FALSE);

    win_comp_pos();		// recompute w_winrow and w_wincol

#အကယ်၍ defined(FEAT_TABPANEL)
    ကိန်းပြည့် tp_width = tabpanel_width();
    ကိန်းပြည့် tp_width_changed = tp_width != prev_tp_width;
    // tabpanel width changed
    အကယ်၍ (tp_width_changed && p_ea)
	win_equal(curwin, FALSE, 0);
    // tabpanel layout changed
    အကယ်၍ (tp_width_changed
	    || (tp_width > 0 && (firstwin->w_wincol != prev_wincol
		    || topframe->fr_width != prev_fr_width)))
    {
	screen_fill(cmdline_row, (ကိန်းပြည့်)Rows, 0, (ကိန်းပြည့်)Columns, ' ', ' ', 0);
    }
    prev_tp_width = tabpanel_width();
    prev_wincol = firstwin->w_wincol;
    prev_fr_width = topframe->fr_width;
#endif
    // Adjust offset for command line start column
    cmdline_col_off = firstwin->w_wincol;
    cmdline_width = topframe->fr_width;
    comp_col();
    အကယ်၍ (!skip_win_fix_scroll)
	win_fix_scroll(TRUE);

    redraw_tabline = TRUE;
#အကယ်၍ defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
#အကယ်၍ 0
    // Disabled: don't want making the screen smaller make a window larger.
    အကယ်၍ (p_ea)
	win_equal(curwin, FALSE, 'h');
#endif
}

/*
 * Save the size of all windows in "gap".
 */
    void
win_size_save(garray_T *gap)
{
    win_T	*wp;

    ga_init2(gap, parimaa_na(ကိန်းပြည့်), 1);
    အကယ်၍ (ga_grow(gap, win_count() * 2 + 1) == FAIL)
	ပြန်ပေး;

    // first entry is the total lines available for windows
    ((ကိန်းပြည့် *)gap->ga_data)[gap->ga_len++] = ROWS_AVAIL - last_stl_height(FALSE);

    FOR_ALL_WINDOWS(wp)
    {
	((ကိန်းပြည့် *)gap->ga_data)[gap->ga_len++] =
	    wp->w_width + wp->w_vsep_width;
	((ကိန်းပြည့် *)gap->ga_data)[gap->ga_len++] = wp->w_height;
    }
}

/*
 * Restore window sizes, but only if the number of windows is still the same
 * and total lines available for windows didn't change.
 * Does not free the growarray.
 */
    void
win_size_restore(garray_T *gap)
{
    win_T	*wp;
    ကိန်းပြည့်		i, j;

    အကယ်၍ (win_count() * 2 + 1 == gap->ga_len
	    && ((ကိန်းပြည့် *)gap->ga_data)[0] == ROWS_AVAIL - last_stl_height(FALSE))
    {
	// The order matters, because frames contain other frames, but it's
	// difficult to get right. The easy way out is to do it twice.
	အတွက် (j = 0; j < 2; ++j)
	{
	    i = 1;
	    FOR_ALL_WINDOWS(wp)
	    {
		frame_setwidth(wp->w_frame, ((ကိန်းပြည့် *)gap->ga_data)[i++]);
		win_setheight_win(((ကိန်းပြည့် *)gap->ga_data)[i++], wp);
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
    void
win_comp_pos(void)
{
    ကိန်းပြည့်		row = tabline_height();
    ကိန်းပြည့်		col = TPL_LCOL();

    frame_comp_pos(topframe, &row, &col);
#အကယ်၍ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Update the position of the windows in frame "topfrp", using the width and
 * height of the frames.
 * "*row" and "*col" are the top-left position of the frame.  They are updated
 * to the bottom-right position plus one.
 */
    static void
frame_comp_pos(frame_T *topfrp, ကိန်းပြည့် *row, ကိန်းပြည့် *col)
{
    win_T	*wp;
    frame_T	*frp;
    ကိန်းပြည့်		startcol;
    ကိန်းပြည့်		startrow;
    ကိန်းပြည့်		h;

    wp = topfrp->fr_win;
    အကယ်၍ (wp != NULL)
    {
	အကယ်၍ (wp->w_winrow != *row || wp->w_wincol != *col)
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
    မဟုတ်ပါက
    {
	startrow = *row;
	startcol = *col;
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	{
	    အကယ်၍ (topfrp->fr_layout == FR_ROW)
		*row = startrow;	// all frames are at the same row
	    မဟုတ်ပါက
		*col = startcol;	// all frames are at the same col
	    frame_comp_pos(frp, row, col);
	}
    }
}

/*
 * Make the current window show at least one line and one column.
 */
    void
win_ensure_size(void)
{
    အကယ်၍ (curwin->w_height == 0)
	win_setheight(1);
    အကယ်၍ (curwin->w_width == 0)
	win_setwidth(1);
}

/*
 * Set current window height and take care of repositioning other windows to
 * fit around it.
 */
    void
win_setheight(ကိန်းပြည့် height)
{
    win_setheight_win(height, curwin);
#အကယ်၍ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

/*
 * Set the window height of window "win" and take care of repositioning other
 * windows to fit around it.
 */
    void
win_setheight_win(ကိန်းပြည့် height, win_T *win)
{
    အကယ်၍ (win == curwin)
    {
	// Always keep current window at least one line high, even when
	// 'winminheight' is zero.
	အကယ်၍ (height < p_wmh)
	    height = p_wmh;
	အကယ်၍ (height == 0)
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
    static void
frame_setheight(frame_T *curfrp, ကိန်းပြည့် height)
{
    ကိန်းပြည့်		room;		// total number of lines available
    ကိန်းပြည့်		take;		// number of lines taken from other windows
    ကိန်းပြည့်		room_cmdline;	// lines available from cmdline
    ကိန်းပြည့်		run;
    frame_T	*frp;
    ကိန်းပြည့်		h;
    ကိန်းပြည့်		room_reserved;

    // If the height already is the desired value, nothing to do.
    အကယ်၍ (curfrp->fr_height == height)
	ပြန်ပေး;

    အကယ်၍ (curfrp->fr_parent == NULL)
    {
	// topframe: can only change the command line height
	အကယ်၍ (height > 0)
	    frame_new_height(curfrp, height, FALSE, FALSE, TRUE);
    }
    မဟုတ်ပါက အကယ်၍ (curfrp->fr_parent->fr_layout == FR_ROW)
    {
	// Row of frames: Also need to resize frames left and right of this
	// one.  First check for the minimal height of these.
	h = frame_minheight(curfrp->fr_parent, NULL);
	အကယ်၍ (height < h)
	    height = h;
	frame_setheight(curfrp->fr_parent, height);
    }
    မဟုတ်ပါက
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
	အတွက် (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		အကယ်၍ (frp != curfrp
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfh)
		    room_reserved += frp->fr_height;
		room += frp->fr_height;
		အကယ်၍ (frp != curfrp)
		    room -= frame_minheight(frp, NULL);
	    }
	    အကယ်၍ (curfrp->fr_width != topframe->fr_width)
		room_cmdline = 0;
	    မဟုတ်ပါက
	    {
		room_cmdline = Rows - p_ch - (lastwin->w_winrow
						+ VISIBLE_HEIGHT(lastwin)
						+ lastwin->w_status_height);
		အကယ်၍ (room_cmdline < 0)
		    room_cmdline = 0;
	    }

	    အကယ်၍ (height <= room + room_cmdline)
		bha_nga;
	    အကယ်၍ (run == 2 || curfrp->fr_width == topframe->fr_width)
	    {
		height = room + room_cmdline;
		bha_nga;
	    }
	    frame_setheight(curfrp->fr_parent, height
		+ frame_minheight(curfrp->fr_parent, NOWIN) - (ကိန်းပြည့်)p_wmh - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = height - curfrp->fr_height;

	// If there is not enough room, also reduce the height of a window
	// with 'winfixheight' set.
	အကယ်၍ (height > room + room_cmdline - room_reserved)
	    room_reserved = room + room_cmdline - height;
	// If there is only a 'winfixheight' window and making the
	// window smaller, need to make the other window taller.
	အကယ်၍ (take < 0 && room - curfrp->fr_height <= room_reserved)
	    room_reserved = 0;

	အကယ်၍ (take > 0 && room_cmdline > 0)
	{
	    // use lines from cmdline first
	    အကယ်၍ (take < room_cmdline)
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
	အတွက် (run = 0; run < 2; ++run)
	{
	    အကယ်၍ (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    မဟုတ်ပါက
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    နေစဉ် (frp != NULL && take != 0)
	    {
		h = frame_minheight(frp, NULL);
		အကယ်၍ (room_reserved > 0
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfh)
		{
		    အကယ်၍ (room_reserved >= frp->fr_height)
			room_reserved -= frp->fr_height;
		    မဟုတ်ပါက
		    {
			အကယ်၍ (frp->fr_height - room_reserved > take)
			    room_reserved = frp->fr_height - take;
			take -= frp->fr_height - room_reserved;
			frame_new_height(frp, room_reserved, FALSE, FALSE,
									TRUE);
			room_reserved = 0;
		    }
		}
		မဟုတ်ပါက
		{
		    အကယ်၍ (frp->fr_height - take < h)
		    {
			take -= frp->fr_height - h;
			frame_new_height(frp, h, FALSE, FALSE, TRUE);
		    }
		    မဟုတ်ပါက
		    {
			frame_new_height(frp, frp->fr_height - take,
							    FALSE, FALSE, TRUE);
			take = 0;
		    }
		}
		အကယ်၍ (run == 0)
		    frp = frp->fr_next;
		မဟုတ်ပါက
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Set current window width and take care of repositioning other windows to
 * fit around it.
 */
    void
win_setwidth(ကိန်းပြည့် width)
{
    win_setwidth_win(width, curwin);
}

    void
win_setwidth_win(ကိန်းပြည့် width, win_T *wp)
{
    // Always keep current window at least one column wide, even when
    // 'winminwidth' is zero.
    အကယ်၍ (wp == curwin)
    {
	အကယ်၍ (width < p_wmw)
	    width = p_wmw;
	အကယ်၍ (width == 0)
	    width = 1;
    }
    မဟုတ်ပါက အကယ်၍ (width < 0)
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
    static void
frame_setwidth(frame_T *curfrp, ကိန်းပြည့် width)
{
    ကိန်းပြည့်		room;		// total number of lines available
    ကိန်းပြည့်		take;		// number of lines taken from other windows
    ကိန်းပြည့်		run;
    frame_T	*frp;
    ကိန်းပြည့်		w;
    ကိန်းပြည့်		room_reserved;

    // If the width already is the desired value, nothing to do.
    အကယ်၍ (curfrp->fr_width == width)
	ပြန်ပေး;

    အကယ်၍ (curfrp->fr_parent == NULL)
	// topframe: can't change width
	ပြန်ပေး;

    အကယ်၍ (curfrp->fr_parent->fr_layout == FR_COL)
    {
	// Column of frames: Also need to resize frames above and below of
	// this one.  First check for the minimal width of these.
	w = frame_minwidth(curfrp->fr_parent, NULL);
	အကယ်၍ (width < w)
	    width = w;
	frame_setwidth(curfrp->fr_parent, width);
    }
    မဟုတ်ပါက
    {
	/*
	 * Row of frames: try to change only frames in this row.
	 *
	 * Do this twice:
	 * 1: compute room available, if it's not enough try resizing the
	 *    containing frame.
	 * 2: compute the room available and adjust the width to it.
	 */
	အတွက် (run = 1; run <= 2; ++run)
	{
	    room = 0;
	    room_reserved = 0;
	    FOR_ALL_FRAMES(frp, curfrp->fr_parent->fr_child)
	    {
		အကယ်၍ (frp != curfrp
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfw)
		    room_reserved += frp->fr_width;
		room += frp->fr_width;
		အကယ်၍ (frp != curfrp)
		    room -= frame_minwidth(frp, NULL);
	    }

	    အကယ်၍ (width <= room)
		bha_nga;
	    အကယ်၍ (run == 2 || curfrp->fr_height >= ROWS_AVAIL)
	    {
		width = room;
		bha_nga;
	    }
	    frame_setwidth(curfrp->fr_parent, width
		 + frame_minwidth(curfrp->fr_parent, NOWIN) - (ကိန်းပြည့်)p_wmw - 1);
	}

	/*
	 * Compute the number of lines we will take from others frames (can be
	 * negative!).
	 */
	take = width - curfrp->fr_width;

	// If there is not enough room, also reduce the width of a window
	// with 'winfixwidth' set.
	အကယ်၍ (width > room - room_reserved)
	    room_reserved = room - width;
	// If there is only a 'winfixwidth' window and making the
	// window smaller, need to make the other window narrower.
	အကယ်၍ (take < 0 && room - curfrp->fr_width < room_reserved)
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
	အတွက် (run = 0; run < 2; ++run)
	{
	    အကယ်၍ (run == 0)
		frp = curfrp->fr_next;	// 1st run: start with next window
	    မဟုတ်ပါက
		frp = curfrp->fr_prev;	// 2nd run: start with prev window
	    နေစဉ် (frp != NULL && take != 0)
	    {
		w = frame_minwidth(frp, NULL);
		အကယ်၍ (room_reserved > 0
			&& frp->fr_win != NULL
			&& frp->fr_win->w_p_wfw)
		{
		    အကယ်၍ (room_reserved >= frp->fr_width)
			room_reserved -= frp->fr_width;
		    မဟုတ်ပါက
		    {
			အကယ်၍ (frp->fr_width - room_reserved > take)
			    room_reserved = frp->fr_width - take;
			take -= frp->fr_width - room_reserved;
			frame_new_width(frp, room_reserved, FALSE, FALSE);
			room_reserved = 0;
		    }
		}
		မဟုတ်ပါက
		{
		    အကယ်၍ (frp->fr_width - take < w)
		    {
			take -= frp->fr_width - w;
			frame_new_width(frp, w, FALSE, FALSE);
		    }
		    မဟုတ်ပါက
		    {
			frame_new_width(frp, frp->fr_width - take,
								FALSE, FALSE);
			take = 0;
		    }
		}
		အကယ်၍ (run == 0)
		    frp = frp->fr_next;
		မဟုတ်ပါက
		    frp = frp->fr_prev;
	    }
	}
    }
}

/*
 * Check 'winminheight' for a valid value and reduce it if needed.
 */
    void
win_setminheight(void)
{
    ကိန်းပြည့်		room;
    ကိန်းပြည့်		needed;
    ကိန်းပြည့်		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    နေစဉ် (p_wmh > 0)
    {
	room = Rows - p_ch;
	needed = min_rows_for_all_tabpages() - 1;  // 1 was added for the cmdline
	အကယ်၍ (room >= needed)
	    bha_nga;
	--p_wmh;
	အကယ်၍ (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Check 'winminwidth' for a valid value and reduce it if needed.
 */
    void
win_setminwidth(void)
{
    ကိန်းပြည့်		room;
    ကိန်းပြည့်		needed;
    ကိန်းပြည့်		first = TRUE;

    // loop until there is a 'winminheight' that is possible
    နေစဉ် (p_wmw > 0)
    {
	room = topframe->fr_width;
	needed = frame_minwidth(topframe, NULL);
	အကယ်၍ (room >= needed)
	    bha_nga;
	--p_wmw;
	အကယ်၍ (first)
	{
	    emsg(_(e_not_enough_room));
	    first = FALSE;
	}
    }
}

/*
 * Status line of dragwin is dragged "offset" lines down (negative is up).
 */
    void
win_drag_status_line(win_T *dragwin, ကိန်းပြည့် offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    ကိန်းပြည့်		room;
    ကိန်းပြည့်		up;	// if TRUE, drag status line up, otherwise down
    ကိန်းပြည့်		n;

    fr = dragwin->w_frame;
    curfr = fr;
    အကယ်၍ (fr != topframe)		// more than one window
    {
	fr = fr->fr_parent;
	// When the parent frame is not a column of frames, its parent should
	// be.
	အကယ်၍ (fr->fr_layout != FR_COL)
	{
	    curfr = fr;
	    အကယ်၍ (fr != topframe)	// only a row of windows, may drag statusline
		fr = fr->fr_parent;
	}
    }

    // If this is the last frame in a column, may want to resize the parent
    // frame instead (go two up to skip a row of frames).
    နေစဉ် (curfr != topframe && curfr->fr_next == NULL)
    {
	အကယ်၍ (fr != topframe)
	    fr = fr->fr_parent;
	curfr = fr;
	အကယ်၍ (fr != topframe)
	    fr = fr->fr_parent;
    }

    အကယ်၍ (offset < 0) // drag up
    {
	up = TRUE;
	offset = -offset;
	// sum up the room of the current frame and above it
	အကယ်၍ (fr == curfr)
	{
	    // only one window
	    room = fr->fr_height - frame_minheight(fr, NULL);
	}
	မဟုတ်ပါက
	{
	    room = 0;
	    အတွက် (fr = fr->fr_child; ; fr = fr->fr_next)
	    {
		room += fr->fr_height - frame_minheight(fr, NULL);
		အကယ်၍ (fr == curfr)
		    bha_nga;
	    }
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    မဟုတ်ပါက    // drag down
    {
	up = FALSE;
	/*
	 * Only dragging the last status line can reduce p_ch.
	 */
	room = Rows - cmdline_row;
	အကယ်၍ (curfr->fr_next == NULL)
	    --room;
	မဟုတ်ပါက
	    room -= p_ch;
	အကယ်၍ (room < 0)
	    room = 0;
	// sum up the room of frames below of the current one
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_height - frame_minheight(fr, NULL);
	fr = curfr;			// put fr at window that grows
    }

    အကယ်၍ (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    အကယ်၍ (offset <= 0)
	ပြန်ပေး;

    /*
     * Grow frame fr by "offset" lines.
     * Doesn't happen when dragging the last status line up.
     */
    အကယ်၍ (fr != NULL)
	frame_new_height(fr, fr->fr_height + offset, up, FALSE, TRUE);

    အကယ်၍ (up)
	fr = curfr;		// current frame gets smaller
    မဟုတ်ပါက
	fr = curfr->fr_next;	// next frame gets smaller

    /*
     * Now make the other frames smaller.
     */
    နေစဉ် (fr != NULL && offset > 0)
    {
	n = frame_minheight(fr, NULL);
	အကယ်၍ (fr->fr_height - offset <= n)
	{
	    offset -= fr->fr_height - n;
	    frame_new_height(fr, n, !up, FALSE, TRUE);
	}
	မဟုတ်ပါက
	{
	    frame_new_height(fr, fr->fr_height - offset, !up, FALSE, TRUE);
	    bha_nga;
	}
	အကယ်၍ (up)
	    fr = fr->fr_prev;
	မဟုတ်ပါက
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
    void
win_drag_vsep_line(win_T *dragwin, ကိန်းပြည့် offset)
{
    frame_T	*curfr;
    frame_T	*fr;
    ကိန်းပြည့်		room;
    ကိန်းပြည့်		left;	// if TRUE, drag separator line left, otherwise right
    ကိန်းပြည့်		n;

    fr = dragwin->w_frame;
    အကယ်၍ (fr == topframe)		// only one window (cannot happen?)
	ပြန်ပေး;
    curfr = fr;
    fr = fr->fr_parent;
    // When the parent frame is not a row of frames, its parent should be.
    အကယ်၍ (fr->fr_layout != FR_ROW)
    {
	အကယ်၍ (fr == topframe)	// only a column of windows (cannot happen?)
	    ပြန်ပေး;
	curfr = fr;
	fr = fr->fr_parent;
    }

    // If this is the last frame in a row, may want to resize a parent
    // frame instead.
    နေစဉ် (curfr->fr_next == NULL)
    {
	အကယ်၍ (fr == topframe)
	    bha_nga;
	curfr = fr;
	fr = fr->fr_parent;
	အကယ်၍ (fr != topframe)
	{
	    curfr = fr;
	    fr = fr->fr_parent;
	}
    }

    အကယ်၍ (offset < 0) // drag left
    {
	left = TRUE;
	offset = -offset;
	// sum up the room of the current frame and left of it
	room = 0;
	အတွက် (fr = fr->fr_child; ; fr = fr->fr_next)
	{
	    room += fr->fr_width - frame_minwidth(fr, NULL);
	    အကယ်၍ (fr == curfr)
		bha_nga;
	}
	fr = curfr->fr_next;		// put fr at frame that grows
    }
    မဟုတ်ပါက    // drag right
    {
	left = FALSE;
	// sum up the room of frames right of the current one
	room = 0;
	FOR_ALL_FRAMES(fr, curfr->fr_next)
	    room += fr->fr_width - frame_minwidth(fr, NULL);
	fr = curfr;			// put fr at window that grows
    }

    အကယ်၍ (room < offset)		// Not enough room
	offset = room;		// Move as far as we can
    အကယ်၍ (offset <= 0)		// No room at all, quit.
	ပြန်ပေး;
    အကယ်၍ (fr == NULL)
	// This can happen when calling win_move_separator() on the rightmost
	// window.  Just don't do anything.
	ပြန်ပေး;

    // grow frame fr by offset lines
    frame_new_width(fr, fr->fr_width + offset, left, FALSE);

    // shrink other frames: current and at the left or at the right
    အကယ်၍ (left)
	fr = curfr;		// current frame gets smaller
    မဟုတ်ပါက
	fr = curfr->fr_next;	// next frame gets smaller

    နေစဉ် (fr != NULL && offset > 0)
    {
	n = frame_minwidth(fr, NULL);
	အကယ်၍ (fr->fr_width - offset <= n)
	{
	    offset -= fr->fr_width - n;
	    frame_new_width(fr, n, !left, FALSE);
	}
	မဟုတ်ပါက
	{
	    frame_new_width(fr, fr->fr_width - offset, !left, FALSE);
	    bha_nga;
	}
	အကယ်၍ (left)
	    fr = fr->fr_prev;
	မဟုတ်ပါက
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
    void
set_fraction(win_T *wp)
{
    အကယ်၍ (wp->w_height > 1)
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
    static void
win_fix_scroll(ကိန်းပြည့် resize)
{
    အကယ်၍ (*p_spk == 'c')
	ပြန်ပေး;  // 'splitkeep' is "cursor"

    skip_update_topline = TRUE;
    win_T	*wp;
    FOR_ALL_WINDOWS(wp)
    {
	// Skip when window height has not changed.
	အကယ်၍ (wp->w_height != wp->w_prev_height)
	{
	    // Cursor position in this window may now be invalid.  It is kept
	    // potentially invalid until the window is made the current window.
	    wp->w_do_win_fix_cursor = true;

	    // If window has moved update botline to keep the same screenlines.
	    အကယ်၍ (*p_spk == 's' && wp->w_winrow != wp->w_prev_winrow
		      && wp->w_botline - 1 <= wp->w_buffer->b_ml.ml_line_count)
	    {
		ကိန်းပြည့် diff = (wp->w_winrow - wp->w_prev_winrow)
					  + (wp->w_height - wp->w_prev_height);
		pos_T cursor = wp->w_cursor;
		linenr_T topline = wp->w_topline;
		colnr_T skipcol = wp->w_skipcol;

		wp->w_cursor.lnum = wp->w_botline - 1;

		//  Add difference in height and row to botline.
		အကယ်၍ (diff > 0)
		    cursor_down_inner(wp, diff);
		မဟုတ်ပါက
		    cursor_up_inner(wp, -diff);

		// Scroll to put the new cursor position at the bottom of the
		// screen.
		wp->w_fraction = FRACTION_MULT;
		scroll_to_fraction(wp, wp->w_prev_height);

		wp->w_cursor = cursor;
		// Keeping the same screen lines includes the skipped columns.
		အကယ်၍ (wp->w_topline == topline)
		    wp->w_skipcol = skipcol;
		wp->w_valid &= ~VALID_WCOL;
	    }
	    မဟုတ်ပါက အကယ်၍ (wp == curwin)
		wp->w_valid &= ~VALID_CROW;

	    invalidate_botline_win(wp);
	    validate_botline_win(wp);
	}
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    skip_update_topline = FALSE;
    // Ensure cursor is valid when not in normal mode or when resized.
    အကယ်၍ (!(get_real_state() & (MODE_NORMAL|MODE_CMDLINE|MODE_TERMINAL)))
	win_fix_cursor(FALSE);
    မဟုတ်ပါက အကယ်၍ (resize)
	win_fix_cursor(TRUE);
}

/*
 * Make sure the cursor position is valid for 'splitkeep'.
 * If it is not, put the cursor position in the jumplist and move it.
 * If we are not in normal mode ("normal" is FALSE), make it valid by scrolling
 * instead.
 */
    static void
win_fix_cursor(ကိန်းပြည့် normal)
{
    win_T	*wp = curwin;

    အကယ်၍ (skip_win_fix_cursor
	    || !wp->w_do_win_fix_cursor
	    || wp->w_buffer->b_ml.ml_line_count < wp->w_height)
	ပြန်ပေး;

    wp->w_do_win_fix_cursor = false;
    // Determine valid cursor range.
    long so = MIN(wp->w_height / 2, get_scrolloff_value());
    linenr_T lnum = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_topline;
    cursor_down_inner(wp, so);
    linenr_T top = wp->w_cursor.lnum;

    wp->w_cursor.lnum = wp->w_botline - 1;
    cursor_up_inner(wp, so);
    linenr_T bot = wp->w_cursor.lnum;

    wp->w_cursor.lnum = lnum;

    // Check if cursor position is above or below valid cursor range.
    linenr_T nlnum = 0;
    အကယ်၍ (lnum > bot && (wp->w_botline - wp->w_buffer->b_ml.ml_line_count) != 1)
	nlnum = bot;
    မဟုတ်ပါက အကယ်၍ (lnum < top && wp->w_topline != 1)
	nlnum = (so == wp->w_height / 2) ? bot : top;

    အကယ်၍ (nlnum != 0)  // Cursor is invalid for current scroll position.
    {
	အကယ်၍ (normal)  // Save to jumplist and set cursor to avoid scrolling.
	{
	    setmark('\'');
	    wp->w_cursor.lnum = nlnum;
	}
	မဟုတ်ပါက  // Scroll instead when not in normal mode.
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
    void
win_new_height(win_T *wp, ကိန်းပြည့် height)
{
    ကိန်းပြည့်		prev_height = wp->w_height;

    // Don't want a negative height.  Happens when splitting a tiny window.
    // Will equalize heights soon to fix it.
    အကယ်၍ (height < 0)
	height = 0;
    အကယ်၍ (wp->w_height == height)
	ပြန်ပေး;	    // nothing to do

    အကယ်၍ (wp->w_height > 0)
    {
	အကယ်၍ (wp == curwin && *p_spk == 'c')
	    // w_wrow needs to be valid. When setting 'laststatus' this may
	    // call win_new_height() recursively.
	    validate_cursor();
	အကယ်၍ (wp->w_height != prev_height)
	    ပြန်ပေး;  // Recursive call already changed the size, bail out here
		     //	to avoid the following to mess things up.
	အကယ်၍ (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_redr_status = true;
    win_comp_scroll(wp);

    // There is no point in adjusting the scroll position when exiting.  Some
    // values might be invalid.
    အကယ်၍ (!exiting && *p_spk == 'c')
    {
	// With 'smoothscroll' w_skipcol is the scroll position, keep it.
	// Otherwise it only keeps the cursor visible and is computed again.
	အကယ်၍ (!wp->w_p_sms)
	    wp->w_skipcol = 0;
	scroll_to_fraction(wp, prev_height);
    }
}

    void
scroll_to_fraction(win_T *wp, ကိန်းပြည့် prev_height)
{
    linenr_T	lnum;
    ကိန်းပြည့်		sline, line_size;
    ကိန်းပြည့်		height = wp->w_height;

    // Don't change w_topline in any of these cases:
    // - window height is 0
    // - 'scrollbind' is set and this isn't the current window
    // - window height is sufficient to display the whole buffer and first line
    //   is visible.
    အကယ်၍ (height > 0
	   && (!wp->w_p_scb || wp == curwin)
	   && (height < wp->w_buffer->b_ml.ml_line_count || wp->w_topline > 1))
    {
	/*
	 * Find a value for w_topline that shows the cursor at the same
	 * relative position in the window as before (more or less).
	 */
	lnum = wp->w_cursor.lnum;
	အကယ်၍ (lnum < 1)		// can happen when starting up
	    lnum = 1;
	wp->w_wrow = ((long)wp->w_fraction * (long)height - 1L)
							       / FRACTION_MULT;
	line_size = plines_win_col(wp, lnum, (long)(wp->w_cursor.col)) - 1;
	sline = wp->w_wrow - line_size;

	အကယ်၍ (sline >= 0)
	{
	    // Make sure the whole cursor line is visible, if possible.
	    ကိန်းပြည့် rows = plines_win(wp, lnum, FALSE);

	    အကယ်၍ (sline > wp->w_height - rows)
	    {
		sline = wp->w_height - rows;
		wp->w_wrow -= rows - line_size;
	    }
	}

	အကယ်၍ (sline < 0)
	{
	    /*
	     * Cursor line would go off top of screen if w_wrow was this high.
	     * Make cursor line the first line in the window.  If not enough
	     * room use w_skipcol;
	     */
	    ကိန်းပြည့်		want_row = wp->w_wrow;	// where the cursor should be

	    wp->w_wrow = line_size;
	    အကယ်၍ (wp->w_wrow >= wp->w_height
				       && (wp->w_width - win_col_off(wp)) > 0)
	    {
		// Skip columns to get the cursor in the wanted row.
		colnr_T	skipcol = wp->w_width - win_col_off(wp);

		--wp->w_wrow;
		နေစဉ် (wp->w_wrow > want_row)
		{
		    skipcol += wp->w_width - win_col_off(wp) + win_col_off2(wp);
		    --wp->w_wrow;
		}
		wp->w_skipcol = skipcol;
	    }
	}
	မဟုတ်ပါက အကယ်၍ (sline > 0)
	{
	    နေစဉ် (sline > 0 && lnum > 1)
	    {
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
		အကယ်၍ (lnum == 1)
		{
		    // first line in buffer is folded
		    line_size = 1;
		    --sline;
		    bha_nga;
		}
#endif
		--lnum;
#ifdef FEAT_DIFF
		အကယ်၍ (lnum == wp->w_topline)
		    line_size = plines_win_nofill(wp, lnum, TRUE)
							      + wp->w_topfill;
		မဟုတ်ပါက
#endif
		    line_size = plines_win(wp, lnum, TRUE);
		sline -= line_size;
	    }

	    အကယ်၍ (sline < 0)
	    {
		/*
		 * Line we want at top would go off top of screen.  Use next
		 * line instead.
		 */
#ifdef FEAT_FOLDING
		hasFoldingWin(wp, lnum, NULL, &lnum, TRUE, NULL);
#endif
		lnum++;
		wp->w_wrow -= line_size + sline;
	    }
	    မဟုတ်ပါက အကယ်၍ (sline > 0)
	    {
		// First line of file reached, use that as topline.
		lnum = 1;
		wp->w_wrow -= sline;
	    }
	}
	set_topline(wp, lnum);
    }

    အကယ်၍ (wp == curwin)
	curs_columns(FALSE);	// validate w_wrow

    အကယ်၍ (prev_height > 0)
	wp->w_prev_fraction_row = wp->w_wrow;

    redraw_win_later(wp, UPD_SOME_VALID);
    invalidate_botline_win(wp);
}

/*
 * Set the width of a window.
 */
    void
win_new_width(win_T *wp, ကိန်းပြည့် width)
{
    // Should we give an error if width < 0?
    wp->w_width = width < 0 ? 0 : width;
    wp->w_lines_valid = 0;
    changed_line_abv_curs_win(wp);
    invalidate_botline_win(wp);

    အကယ်၍ (wp == curwin && *p_spk == 'c')
	curs_columns(TRUE);	// validate w_wrow

    redraw_win_later(wp, UPD_NOT_VALID);
    wp->w_redr_status = true;
}

    void
win_comp_scroll(win_T *wp)
{
#အကယ်၍ defined(FEAT_EVAL)
    ကိန်းပြည့် old_w_p_scr = wp->w_p_scr;
#endif

    wp->w_p_scr = ((unsigned)wp->w_height >> 1);
    အကယ်၍ (wp->w_p_scr == 0)
	wp->w_p_scr = 1;
#အကယ်၍ defined(FEAT_EVAL)
    အကယ်၍ (wp->w_p_scr != old_w_p_scr)
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
    void
command_height(void)
{
    ကိန်းပြည့်		old_p_ch = curtab->tp_ch_used;

    // Find bottom frame with width of screen.
    frame_T *frp = lastwin->w_frame;
    နေစဉ် (frp->fr_width != topframe->fr_width && frp->fr_parent != NULL)
	frp = frp->fr_parent;

    // Avoid changing the height of a window with 'winfixheight' set.
    နေစဉ် (frp->fr_prev != NULL && frp->fr_layout == FR_LEAF
						      && frp->fr_win->w_p_wfh)
	frp = frp->fr_prev;

    နေစဉ် (p_ch > old_p_ch && command_frame_height)
    {
	အကယ်၍ (frp == NULL)
	{
	    emsg(_(e_not_enough_room));
	    p_ch = old_p_ch;
	    bha_nga;
	}
	ကိန်းပြည့် h = MIN(p_ch - old_p_ch,
			frp->fr_height - frame_minheight(frp, NULL));
	frame_add_height(frp, -h);
	old_p_ch += h;
	frp = frp->fr_prev;
    }
    အကယ်၍ (p_ch < old_p_ch && command_frame_height && frp != NULL)
	frame_add_height(frp, (ကိန်းပြည့်)(old_p_ch - p_ch));

    // Recompute window positions.
    win_comp_pos();
    win_fix_scroll(true);
    cmdline_row = Rows - p_ch;
    redraw_cmdline = TRUE;

    // Clear the cmdheight area.
    အကယ်၍ (msg_scrolled == 0 && full_screen)
    {
	screen_fill(cmdline_row, (ကိန်းပြည့်)Rows, 0, (ကိန်းပြည့်)Columns, ' ', ' ', 0);
	msg_row = cmdline_row;
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
    static void
frame_add_height(frame_T *frp, ကိန်းပြည့် n)
{
    frame_new_height(frp, frp->fr_height + n, FALSE, FALSE, FALSE);
    အတွက် (;;)
    {
	frp = frp->fr_parent;
	အကယ်၍ (frp == NULL)
	    bha_nga;
	frp->fr_height += n;
    }
}

/*
 * Add or remove a status line for the bottom window(s), according to the
 * value of 'laststatus'.
 */
    void
last_status(
    ကိန်းပြည့်		morewin)	// pretend there are two or more windows
{
    // Don't make a difference between horizontal or vertical split.
    last_status_rec(topframe, last_stl_height(morewin) > 0);
#အကယ်၍ defined(FEAT_STL_OPT)
    frame_change_statusline_height();
#endif
}

    static void
last_status_rec(frame_T *fr, ကိန်းပြည့် statusline)
{
    frame_T	*fp;
    win_T	*wp;

    အကယ်၍ (fr->fr_layout == FR_LEAF)
    {
	wp = fr->fr_win;
	အကယ်၍ (wp->w_status_height != 0 && !statusline)
	{
	    // remove status line
	    win_new_height(wp, wp->w_height + wp->w_status_height);
	    wp->w_status_height = 0;
	    comp_col();
	}
	မဟုတ်ပါက အကယ်၍ (wp->w_status_height == 0 && statusline)
	{
	    // Find a frame to take a line from.
	    fp = fr;
	    နေစဉ် (fp->fr_height <= frame_minheight(fp, NULL))
	    {
		အကယ်၍ (fp == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ပြန်ပေး;
		}
		// In a column of frames: go to frame above.  If already at
		// the top or in a row of frames: go to parent.
		အကယ်၍ (fp->fr_parent->fr_layout == FR_COL && fp->fr_prev != NULL)
		    fp = fp->fr_prev;
		မဟုတ်ပါက
		    fp = fp->fr_parent;
	    }
	    wp->w_status_height = statusline_height(wp);
	    အကယ်၍ (fp != fr)
	    {
		frame_new_height(fp, fp->fr_height - wp->w_status_height,
			FALSE, FALSE, FALSE);
		frame_fix_height(wp);
		win_comp_pos();
	    }
	    မဟုတ်ပါက
		win_new_height(wp, wp->w_height - wp->w_status_height);
	    comp_col();
	    redraw_all_later(UPD_SOME_VALID);
	}
	// Set prev_height when difference is due to 'laststatus'.
	အကယ်၍ (abs(wp->w_height - wp->w_prev_height) == 1)
	    wp->w_prev_height = wp->w_height;
    }
    မဟုတ်ပါက အကယ်၍ (fr->fr_layout == FR_ROW)
    {
	// vertically split windows, set status line for each one
	အကယ်၍ (!statusline)
	{
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		last_status_rec(fp, statusline);
	}
	မဟုတ်ပါက
	{
	    frame_T	*fp2;
	    ကိန်းပြည့်		max_stlh = 0;
	    ကိန်းပြည့်		new_row_height;

	    // Find the max status height needed across all leaf windows.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
		အကယ်၍ (fp->fr_win != NULL && fp->fr_win->w_status_height == 0)
		{
		    ကိန်းပြည့် h = statusline_height(fp->fr_win);
		    အကယ်၍ (h > max_stlh)
			max_stlh = h;
		}
	    အကယ်၍ (max_stlh == 0)
		ပြန်ပေး;

	    // Find a frame to take max_stlh lines from.
	    fp2 = fr;
	    နေစဉ် (fp2->fr_height - frame_minheight(fp2, NULL) < max_stlh)
	    {
		အကယ်၍ (fp2 == topframe)
		{
		    emsg(_(e_not_enough_room));
		    ပြန်ပေး;
		}
		အကယ်၍ (fp2->fr_parent->fr_layout == FR_COL
						&& fp2->fr_prev != NULL)
		    fp2 = fp2->fr_prev;
		မဟုတ်ပါက
		    fp2 = fp2->fr_parent;
	    }

	    အကယ်၍ (fp2 != fr)
	    {
		frame_new_height(fp2, fp2->fr_height - max_stlh,
							FALSE, FALSE, FALSE);
		new_row_height = fr->fr_height + max_stlh;
	    }
	    မဟုတ်ပါက
		new_row_height = fr->fr_height;

	    // Set status and content heights for all leaves.
	    FOR_ALL_FRAMES(fp, fr->fr_child)
	    {
		အကယ်၍ (fp->fr_win != NULL)
		{
		    wp = fp->fr_win;
		    အကယ်၍ (wp->w_status_height == 0)
		    {
			wp->w_status_height = statusline_height(wp);
			win_new_height(wp,
				  new_row_height - wp->w_status_height);
			frame_fix_height(wp);
			comp_col();
			redraw_all_later(UPD_SOME_VALID);
			အကယ်၍ (abs(wp->w_height - wp->w_prev_height) == 1)
			    wp->w_prev_height = wp->w_height;
		    }
		}
		မဟုတ်ပါက
		    last_status_rec(fp, statusline); // nested frames
	    }

	    fr->fr_height = new_row_height;
	    win_comp_pos();
	}
    }
    မဟုတ်ပါက
    {
	// horizontally split window, set status line for last one
	အတွက် (fp = fr->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    ;
	last_status_rec(fp, statusline);
    }
}

#အကယ်၍ defined(FEAT_STL_OPT)
/*
 * Update the cached rendered height of 'statusline'.
 * If "wp" is NULL, update the global stl_rendered_height using p_stl.
 * If "wp" is non-NULL, update wp->w_stl_rendered_height using wp->w_p_stl.
 */
    void
update_stl_rendered_height(win_T *wp)
{
    char_u	*opt_name = (char_u *)"statusline";
    char_u	*stl_val = (wp == NULL) ? p_stl : wp->w_p_stl;
    ကိန်းပြည့်		opt_flags = (wp == NULL) ? 0 : OPT_LOCAL;
    ကိန်းပြည့်		*height = (wp == NULL) ? &stl_rendered_height
						: &wp->w_stl_rendered_height;

    *height = get_stl_rendered_height(wp == NULL ? curwin : wp, stl_val,
					opt_name, opt_flags);
}

/*
 * First pass: clamp the global statusline height against available space.
 * Returns the space-constrained height.
 */
    static ကိန်းပြည့်
frame_find_global_stlh_rec(frame_T *frp, ကိန်းပြည့် h)
{
    အကယ်၍ (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	// Only consider windows with a status line that use global stlo.
	// Every such window constrains the global height (best effort): the
	// result is the largest height that still fits every window.
	အကယ်၍ (wp->w_height > 0 && wp->w_status_height > 0
		&& *wp->w_p_stlo == NUL)
	{
	    ကိန်းပြည့် win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    အကယ်၍ (win_free_height - p_wmh < h)
		h = win_free_height - p_wmh;
	}
    }
    မဟုတ်ပါက
    {
	အတွက် (frp = frp->fr_child; frp != NULL; frp = frp->fr_next)
	    h = frame_find_global_stlh_rec(frp, h);
    }
    ပြန်ပေး h;
}

/*
 * Second pass: apply statusline heights.
 * "global_stlh" is the space-constrained height for windows using global stlo.
 */
    static void
frame_change_statusline_height_rec(frame_T *frp, ကိန်းပြည့် global_stlh)
{
    အကယ်၍ (frp->fr_layout == FR_LEAF)
    {
	win_T  *wp = frp->fr_win;

	အကယ်၍ (wp->w_height > 0 && wp->w_status_height > 0)
	{
	    ကိန်းပြည့် win_free_height = frp->fr_height - WINBAR_HEIGHT(wp);

	    wp->w_status_height = statusline_height(wp);
	    အကယ်၍ (wp->w_p_stlo == NULL || *wp->w_p_stlo == NUL)
	    {
		// Global stlo: cap at the uniform space-constrained height so
		// that all windows share the same status line height.
		အကယ်၍ (wp->w_status_height > global_stlh)
		    wp->w_status_height = global_stlh;
	    }
	    အကယ်၍ (wp->w_status_height > win_free_height - p_wmh)
		wp->w_status_height = win_free_height - p_wmh;
	    // Preserve the buffer view: statusline height changes should
	    // not scroll the window.  win_new_height() calls
	    // scroll_to_fraction() which adjusts w_topline; restore it
	    // and let update_topline() handle cursor visibility for
	    // curwin during redraw.
	    linenr_T saved_topline = wp->w_topline;
# ifdef FEAT_DIFF
	    ကိန်းပြည့်	     saved_topfill = wp->w_topfill;
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
    မဟုတ်ပါက
    {
	အတွက် (frp = frp->fr_child; frp != NULL; frp = frp->fr_next)
	    frame_change_statusline_height_rec(frp, global_stlh);
    }
}

/*
 * Adjust status line heights for all windows across all tab pages.
 * Uses two passes: first finds the space-constrained global height,
 * then applies appropriate heights to each window.
 */
    ကိန်းပြည့်
frame_change_statusline_height(void)
{
    tabpage_T	*tp;
    ကိန်းပြည့်		global_stlh;

    // First pass: find space-constrained global height.
    global_stlh = stlo_mh;
    FOR_ALL_TABPAGES(tp)
	global_stlh = frame_find_global_stlh_rec(tp->tp_topframe, global_stlh);

    // Second pass: apply heights.
    FOR_ALL_TABPAGES(tp)
	frame_change_statusline_height_rec(tp->tp_topframe, global_stlh);
    comp_col();
    redraw_all_later(UPD_SOME_VALID);
    ပြန်ပေး global_stlh;
}

/*
 * Update the global maxheight cache for 'statuslineopt'.
 */
    void
set_stlo_mh(ကိန်းပြည့် mh)
{
    stlo_mh = mh;
}

/*
 * Check "stlopt" as 'statuslineopt' and update the members of "wp".
 * This is called when 'statuslineopt' is changed.
 * Returns FAIL for failure, OK otherwise.
 */
    ကိန်းပြည့်
statuslineopt_changed(
    char_u	*stlopt,	// when NULL: use "wp->w_p_stlo"
    win_T	*wp)		// when NULL: update global stlo_fh/stlo_mh
{
    char_u	*p;
    ကိန်းပြည့်		l_stlo_fh = FALSE;
    ကိန်းပြည့်		l_stlo_mh = 1;

    အကယ်၍ (stlopt != NULL)
	p = stlopt;
    မဟုတ်ပါက
	p = wp->w_p_stlo;

    နေစဉ် (*p != NUL)
    {
	// Note: Keep this in sync with p_stlo_values
	အကယ်၍ (STRNCMP(p, "maxheight:", 10) == 0 && VIM_ISDIGIT(p[10]))
	{
	    p += 10;
	    l_stlo_mh = getdigits(&p);
	    အကယ်၍ (l_stlo_mh < 1)
		ပြန်ပေး FAIL;
	}
	မဟုတ်ပါက အကယ်၍ (STRNCMP(p, "fixedheight", 11) == 0)
	{
	    p += 11;
	    l_stlo_fh = TRUE;
	}
	အကယ်၍ (*p != ',' && *p != NUL)
	    ပြန်ပေး FAIL;
	အကယ်၍ (*p == ',')
	    ++p;
    }

    အကယ်၍ (wp == NULL)
    {
	stlo_fh = l_stlo_fh;
	stlo_mh = l_stlo_mh;
    }
    မဟုတ်ပါက
    {
	wp->w_p_stlo_fh = l_stlo_fh;
	wp->w_p_stlo_mh = l_stlo_mh;
    }

    ပြန်ပေး OK;
}
#endif

/*
 * Return the effective statusline height for window "wp".
 * Uses the window-local 'statuslineopt' if set, otherwise the global one.
 * When "wp" is NULL, always uses the global 'statuslineopt' settings.
 */
    ကိန်းပြည့်
statusline_height(win_T *wp UNUSED)
{
    ကိန်းပြည့် stl_height = 1;
#အကယ်၍ defined(FEAT_STL_OPT)
    bool fixed;
    ကိန်းပြည့် l_stl_rendered_height;

    အကယ်၍ (wp != NULL && *wp->w_p_stlo != NUL)
    {
	stl_height = wp->w_p_stlo_mh;
	fixed = !!wp->w_p_stlo_fh;
	l_stl_rendered_height = (*wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }
    မဟုတ်ပါက
    {
	stl_height = stlo_mh;
	fixed = !!stlo_fh;
	l_stl_rendered_height = (wp != NULL && *wp->w_p_stl != NUL)
				    ? wp->w_stl_rendered_height
				    : stl_rendered_height;
    }

    // When not fixed, clamp to rendered height (1 for default statusline).
    အကယ်၍ (!fixed)
    {
	ကိန်းပြည့် has_custom_stl = (*p_stl != NUL)
	    || (wp != NULL && *wp->w_p_stl != NUL);
	အကယ်၍ (has_custom_stl)
	    stl_height = MAX(1, MIN(l_stl_rendered_height, stl_height));
	မဟုတ်ပါက
	    stl_height = 1;
    }
#endif

    ပြန်ပေး stl_height;
}

/*
 * Return the number of lines used by the tab page line.
 */
    ကိန်းပြည့်
tabline_height(void)
{
#ifdef FEAT_GUI_TABLINE
    // When the GUI has the tabline then this always returns zero.
    အကယ်၍ (gui_use_tabline())
	ပြန်ပေး 0;
#endif
    switch (p_stal)
    {
	pak_sa 0: ပြန်ပေး 0;
	pak_sa 1: ပြန်ပေး (first_tabpage->tp_next == NULL) ? 0 : 1;
    }
    ပြန်ပေး 1;
}

/*
 * Return the height of the last window's statusline.
 */
    ကိန်းပြည့်
last_stl_height(
    ကိန်းပြည့်		morewin)	// pretend there are two or more windows
{
    ပြန်ပေး (p_ls == 2 || (p_ls == 1 && (morewin || !ONE_WINDOW)))
		? STATUS_HEIGHT : 0;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows.
 */
    ကိန်းပြည့်
min_rows(void)
{
    အကယ်၍ (firstwin == NULL)	// not initialized yet
	ပြန်ပေး MIN_LINES;

    ပြန်ပေး frame_minheight(curtab->tp_topframe, NULL) + tabline_height()
	+ MIN_CMDHEIGHT;
}

/*
 * Return the minimal number of rows that is needed on the screen to display
 * the current number of windows for all tab pages.
 */
    ကိန်းပြည့်
min_rows_for_all_tabpages(void)
{
    ကိန်းပြည့်		total;
    tabpage_T	*tp;
    ကိန်းပြည့်		n;

    အကယ်၍ (firstwin == NULL)	// not initialized yet
	ပြန်ပေး MIN_LINES;

    total = 0;
    FOR_ALL_TABPAGES(tp)
    {
	n = frame_minheight(tp->tp_topframe, NULL);
	အကယ်၍ (total < n)
	    total = n;
    }
    total += tabline_height();
    total += MIN_CMDHEIGHT;
    ပြန်ပေး total;
}

/*
 * Return TRUE if there is only one window and only one tab page, not
 * counting a help or preview window, unless it is the current window.
 * Does not count unlisted windows.
 */
    ကိန်းပြည့်
only_one_window(void)
{
    ကိန်းပြည့်		count = 0;
    win_T	*wp;

#အကယ်၍ defined(FEAT_PROP_POPUP)
    // If the current window is a popup then there always is another window.
    အကယ်၍ (popup_is_popup(curwin))
	ပြန်ပေး FALSE;
#endif

    // If there is another tab page there always is another window.
    အကယ်၍ (first_tabpage->tp_next != NULL)
	ပြန်ပေး FALSE;

    FOR_ALL_WINDOWS(wp)
	အကယ်၍ (wp->w_buffer != NULL
		&& (!((bt_help(wp->w_buffer) && !bt_help(curbuf))
#ifdef FEAT_QUICKFIX
		    || wp->w_p_pvw
#endif
	     ) || wp == curwin) && !is_aucmd_win(wp))
	    ++count;
    ပြန်ပေး (count <= 1);
}

/*
 * Implementation of check_lnums() and check_lnums_nested().
 */
    static void
check_lnums_both(ကိန်းပြည့် do_curwin, ကိန်းပြည့် nested)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	အကယ်၍ ((do_curwin || wp != curwin) && wp->w_buffer == curbuf)
	{
	    ကိန်းပြည့် need_adjust;

	    အကယ်၍ (!nested)
	    {
		// save the original cursor position and topline
		wp->w_save_cursor.w_cursor_save = wp->w_cursor;
		wp->w_save_cursor.w_topline_save = wp->w_topline;
	    }

	    need_adjust = wp->w_cursor.lnum > curbuf->b_ml.ml_line_count;
	    အကယ်၍ (need_adjust)
		wp->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    အကယ်၍ (need_adjust || !nested)
		// save the (corrected) cursor position
		wp->w_save_cursor.w_cursor_corr = wp->w_cursor;

	    need_adjust = wp->w_topline > curbuf->b_ml.ml_line_count;
	    အကယ်၍ (need_adjust)
		wp->w_topline = curbuf->b_ml.ml_line_count;
	    အကယ်၍ (need_adjust || !nested)
		// save the (corrected) topline
		wp->w_save_cursor.w_topline_corr = wp->w_topline;
	}
}

/*
 * Correct the cursor line number in other windows.  Used after changing the
 * current buffer, and before applying autocommands.
 * When "do_curwin" is TRUE, also check current window.
 */
    void
check_lnums(ကိန်းပြည့် do_curwin)
{
    check_lnums_both(do_curwin, FALSE);
}

/*
 * Like check_lnums() but for when check_lnums() was already called.
 */
    void
check_lnums_nested(ကိန်းပြည့် do_curwin)
{
    check_lnums_both(do_curwin, TRUE);
}

/*
 * Reset cursor and topline to its stored values from check_lnums().
 * check_lnums() must have been called first!
 */
    void
reset_lnums(void)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	အကယ်၍ (wp->w_buffer == curbuf)
	{
	    // Restore the value if the autocommand didn't change it and it was
	    // set.
	    // Note: This triggers e.g. on BufReadPre, when the buffer is not yet
	    //       loaded, so cannot validate the buffer line
	    အကယ်၍ (EQUAL_POS(wp->w_save_cursor.w_cursor_corr, wp->w_cursor)
				  && wp->w_save_cursor.w_cursor_save.lnum != 0)
		wp->w_cursor = wp->w_save_cursor.w_cursor_save;
	    အကယ်၍ (wp->w_save_cursor.w_topline_corr == wp->w_topline
				  && wp->w_save_cursor.w_topline_save != 0)
		wp->w_topline = wp->w_save_cursor.w_topline_save;
	    အကယ်၍ (wp->w_save_cursor.w_topline_save > wp->w_buffer->b_ml.ml_line_count)
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
    ကိန်းပြည့်
make_snapshot(ကိန်းပြည့် idx)
{
    clear_snapshot(curtab, idx);
    အကယ်၍ (make_snapshot_rec(topframe, &curtab->tp_snapshot[idx]) == FAIL)
    {
	clear_snapshot(curtab, idx);
	ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

    static ကိန်းပြည့်
make_snapshot_rec(frame_T *fr, frame_T **frp)
{
    *frp = ALLOC_CLEAR_ONE(frame_T);
    အကယ်၍ (*frp == NULL)
	ပြန်ပေး FAIL;
    (*frp)->fr_layout = fr->fr_layout;
    (*frp)->fr_width = fr->fr_width;
    (*frp)->fr_height = fr->fr_height;
    အကယ်၍ (fr->fr_next != NULL)
    {
	အကယ်၍ (make_snapshot_rec(fr->fr_next, &((*frp)->fr_next)) == FAIL)
	    ပြန်ပေး FAIL;
    }
    အကယ်၍ (fr->fr_child != NULL)
    {
	အကယ်၍ (make_snapshot_rec(fr->fr_child, &((*frp)->fr_child)) == FAIL)
	    ပြန်ပေး FAIL;
    }
    အကယ်၍ (fr->fr_layout == FR_LEAF && fr->fr_win == curwin)
	(*frp)->fr_win = curwin;
    ပြန်ပေး OK;
}

/*
 * Remove any existing snapshot.
 */
    static void
clear_snapshot(tabpage_T *tp, ကိန်းပြည့် idx)
{
    clear_snapshot_rec(tp->tp_snapshot[idx]);
    tp->tp_snapshot[idx] = NULL;
}

    static void
clear_snapshot_rec(frame_T *fr)
{
    အကယ်၍ (fr == NULL)
	ပြန်ပေး;
    clear_snapshot_rec(fr->fr_next);
    clear_snapshot_rec(fr->fr_child);
    vim_free(fr);
}

/*
 * Traverse a snapshot to find the previous curwin.
 */
    static win_T *
get_snapshot_curwin_rec(frame_T *ft)
{
    win_T	*wp;

    အကယ်၍ (ft->fr_next != NULL)
    {
	အကယ်၍ ((wp = get_snapshot_curwin_rec(ft->fr_next)) != NULL)
	    ပြန်ပေး wp;
    }
    အကယ်၍ (ft->fr_child != NULL)
    {
	အကယ်၍ ((wp = get_snapshot_curwin_rec(ft->fr_child)) != NULL)
	    ပြန်ပေး wp;
    }

    ပြန်ပေး ft->fr_win;
}

/*
 * Return the current window stored in the snapshot or NULL.
 */
    static win_T *
get_snapshot_curwin(ကိန်းပြည့် idx)
{
    အကယ်၍ (curtab->tp_snapshot[idx] == NULL)
	ပြန်ပေး NULL;

    ပြန်ပေး get_snapshot_curwin_rec(curtab->tp_snapshot[idx]);
}

/*
 * Restore a previously created snapshot, if there is any.
 * This is only done if the screen size didn't change and the window layout is
 * still the same.
 * "idx" is SNAP_HELP_IDX or SNAP_AUCMD_IDX.
 */
    void
restore_snapshot(
    ကိန်းပြည့်		idx,
    ကိန်းပြည့်		close_curwin)	    // closing current window
{
    win_T	*wp;

    အကယ်၍ (curtab->tp_snapshot[idx] != NULL
	    && curtab->tp_snapshot[idx]->fr_width == topframe->fr_width
	    && curtab->tp_snapshot[idx]->fr_height == topframe->fr_height
	    && check_snapshot_rec(curtab->tp_snapshot[idx], topframe) == OK)
    {
	wp = restore_snapshot_rec(curtab->tp_snapshot[idx], topframe);
	win_comp_pos();
	အကယ်၍ (wp != NULL && close_curwin)
	    win_goto(wp);
	redraw_all_later(UPD_NOT_VALID);
    }
    clear_snapshot(curtab, idx);
}

/*
 * Check if frames "sn" and "fr" have the same layout, same following frames
 * and same children.  And the window pointer is valid.
 */
    static ကိန်းပြည့်
check_snapshot_rec(frame_T *sn, frame_T *fr)
{
    အကယ်၍ (sn->fr_layout != fr->fr_layout
	    || (sn->fr_next == NULL) != (fr->fr_next == NULL)
	    || (sn->fr_child == NULL) != (fr->fr_child == NULL)
	    || (sn->fr_next != NULL
		&& check_snapshot_rec(sn->fr_next, fr->fr_next) == FAIL)
	    || (sn->fr_child != NULL
		&& check_snapshot_rec(sn->fr_child, fr->fr_child) == FAIL)
	    || (sn->fr_win != NULL && !win_valid(sn->fr_win)))
	ပြန်ပေး FAIL;
    ပြန်ပေး OK;
}

/*
 * Copy the size of snapshot frame "sn" to frame "fr".  Do the same for all
 * following frames and children.
 * Returns a pointer to the old current window, or NULL.
 */
    static win_T *
restore_snapshot_rec(frame_T *sn, frame_T *fr)
{
    win_T	*wp = NULL;
    win_T	*wp2;

    fr->fr_height = sn->fr_height;
    fr->fr_width = sn->fr_width;
    အကယ်၍ (fr->fr_layout == FR_LEAF)
    {
	frame_new_height(fr, fr->fr_height, FALSE, FALSE, FALSE);
	frame_new_width(fr, fr->fr_width, FALSE, FALSE);
	wp = sn->fr_win;
    }
    အကယ်၍ (sn->fr_next != NULL)
    {
	wp2 = restore_snapshot_rec(sn->fr_next, fr->fr_next);
	အကယ်၍ (wp2 != NULL)
	    wp = wp2;
    }
    အကယ်၍ (sn->fr_child != NULL)
    {
	wp2 = restore_snapshot_rec(sn->fr_child, fr->fr_child);
	အကယ်၍ (wp2 != NULL)
	    wp = wp2;
    }
    ပြန်ပေး wp;
}

#အကယ်၍ defined(FEAT_GUI)
/*
 * Return TRUE if there is any vertically split window.
 */
    ကိန်းပြည့်
win_hasvertsplit(void)
{
    frame_T	*fr;

    အကယ်၍ (topframe->fr_layout == FR_ROW)
	ပြန်ပေး TRUE;

    အကယ်၍ (topframe->fr_layout == FR_COL)
	FOR_ALL_FRAMES(fr, topframe->fr_child)
	    အကယ်၍ (fr->fr_layout == FR_ROW)
		ပြန်ပေး TRUE;

    ပြန်ပေး FALSE;
}
#endif

#အကယ်၍ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
    ကိန်းပြည့်
get_win_number(win_T *wp, win_T *first_win)
{
    ကိန်းပြည့်		i = 1;
    win_T	*w;

    အတွက် (w = first_win; w != NULL && w != wp; w = W_NEXT(w))
	++i;

    အကယ်၍ (w == NULL)
	ပြန်ပေး 0;
    မဟုတ်ပါက
	ပြန်ပေး i;
}

    ကိန်းပြည့်
get_tab_number(tabpage_T *tp)
{
    ကိန်းပြည့်		i = 1;
    tabpage_T	*t;

    အတွက် (t = first_tabpage; t != NULL && t != tp; t = t->tp_next)
	++i;

    အကယ်၍ (t == NULL)
	ပြန်ပေး 0;
    မဟုတ်ပါက
	ပြန်ပေး i;
}
#endif

/*
 * Return TRUE if "topfrp" and its children are at the right height.
 */
    static ကိန်းပြည့်
frame_check_height(frame_T *topfrp, ကိန်းပြည့် height)
{
    frame_T *frp;

    အကယ်၍ (topfrp->fr_height != height)
	ပြန်ပေး FALSE;

    အကယ်၍ (topfrp->fr_layout == FR_ROW)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    အကယ်၍ (frp->fr_height != height)
		ပြန်ပေး FALSE;

    ပြန်ပေး TRUE;
}

/*
 * Return TRUE if "topfrp" and its children are at the right width.
 */
    static ကိန်းပြည့်
frame_check_width(frame_T *topfrp, ကိန်းပြည့် width)
{
    frame_T *frp;

    အကယ်၍ (topfrp->fr_width != width)
	ပြန်ပေး FALSE;

    အကယ်၍ (topfrp->fr_layout == FR_COL)
	FOR_ALL_FRAMES(frp, topfrp->fr_child)
	    အကယ်၍ (frp->fr_width != width)
		ပြန်ပေး FALSE;

    ပြန်ပေး TRUE;
}

#အကယ်၍ defined(FEAT_SYN_HL)
/*
 * Simple int comparison function for use with qsort()
 */
    static ကိန်းပြည့်
int_cmp(const void *pa, const void *pb)
{
    const ကိန်းပြည့် a = *(const ကိန်းပြည့် *)pa;
    const ကိန်းပြည့် b = *(const ကိန်းပြည့် *)pb;
    အကယ်၍ (a > b)
	ပြန်ပေး 1;
    အကယ်၍ (a < b)
	ပြန်ပေး -1;
    ပြန်ပေး 0;
}

/*
 * Check "cc" as 'colorcolumn' and update the members of "wp".
 * This is called when 'colorcolumn' or 'textwidth' is changed.
 * Returns error message, NULL if it's OK.
 */
    var_na *
check_colorcolumn(
    char_u *cc,		// when NULL: use "wp->w_p_cc"
    win_T *wp)		// when NULL: only parse "cc"
{
    char_u	*s = empty_option;
    ကိန်းပြည့်		tw;
    ကိန်းပြည့်		col;
    ကိန်းပြည့်		count = 0;
    ကိန်းပြည့်		color_cols[256];
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		j = 0;

    အကယ်၍ (wp != NULL && wp->w_buffer == NULL)
	ပြန်ပေး NULL;  // buffer was closed

    အကယ်၍ (cc != NULL)
	s = cc;
    မဟုတ်ပါက အကယ်၍ (wp != NULL)
	s = wp->w_p_cc;

    အကယ်၍ (wp != NULL)
	tw = wp->w_buffer->b_p_tw;
    မဟုတ်ပါက
	// buffer-local value not set, assume zero
	tw = 0;

    နေစဉ် (*s != NUL && count < 255)
    {
	အကယ်၍ (*s == '-' || *s == '+')
	{
	    // -N and +N: add to 'textwidth'
	    col = (*s == '-') ? -1 : 1;
	    ++s;
	    အကယ်၍ (!VIM_ISDIGIT(*s))
		ပြန်ပေး e_invalid_argument;
	    col = col * getdigits(&s);
	    အကယ်၍ (tw == 0)
		goto skip;  // 'textwidth' not set, skip this item
	    col += tw;
	    အကယ်၍ (col < 0)
		goto skip;
	}
	မဟုတ်ပါက အကယ်၍ (VIM_ISDIGIT(*s))
	    col = getdigits(&s);
	မဟုတ်ပါက
	    ပြန်ပေး e_invalid_argument;
	color_cols[count++] = col - 1;  // 1-based to 0-based
skip:
	အကယ်၍ (*s == NUL)
	    bha_nga;
	အကယ်၍ (*s != ',')
	    ပြန်ပေး e_invalid_argument;
	အကယ်၍ (*++s == NUL)
	    ပြန်ပေး e_invalid_argument;  // illegal trailing comma as in "set cc=80,"
    }

    အကယ်၍ (wp == NULL)
	ပြန်ပေး NULL;  // only parse "cc"

    vim_free(wp->w_p_cc_cols);
    အကယ်၍ (count == 0)
	wp->w_p_cc_cols = NULL;
    မဟုတ်ပါက
    {
	wp->w_p_cc_cols = ALLOC_MULT(ကိန်းပြည့်, count + 1);
	အကယ်၍ (wp->w_p_cc_cols != NULL)
	{
	    // sort the columns for faster usage on screen redraw inside
	    // win_line()
	    qsort(color_cols, count, parimaa_na(ကိန်းပြည့်), int_cmp);

	    အတွက် (i = 0; i < count; ++i)
		// skip duplicates
		အကယ်၍ (j == 0 || wp->w_p_cc_cols[j - 1] != color_cols[i])
		    wp->w_p_cc_cols[j++] = color_cols[i];
	    wp->w_p_cc_cols[j] = -1;  // end marker
	}
    }

    ပြန်ပေး NULL;  // no error
}
#endif

    ကိန်းပြည့်
get_last_winid(void)
{
    ပြန်ပေး last_win_id;
}

/*
 * Don't let autocommands close the given window
 */
   ကိန်းပြည့်
win_locked(win_T *wp)
{
    ပြန်ပေး wp->w_locked;
}
