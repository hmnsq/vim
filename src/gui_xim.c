/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * gui_xim.c: functions for the X Input Method
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ !defined(GTK_CHECK_ಆವೃತ್ತಿ)
# define GTK_CHECK_ಆವೃತ್ತಿ(a, b, c) 0
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <gdk/gdkkeysyms.h>
# endif
# ಆದರೆ !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  ಇಲ್ಲದಿದ್ದರೆ
#   include <gdk/gdkx.h>
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#ಆದರೆ defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
ಸ್ಥಿರ ಶೂನ್ಯ xim_log(ಅಕ್ಷರ *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    ಸ್ಥಿರ ಶೂನ್ಯ
xim_log(ಅಕ್ಷರ *s, ...)
{
    va_list arglist;
    ಸ್ಥಿರ FILE *fd = ಖಾಲಿ;

    ಆದರೆ (fd == (FILE *)-1)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (fd == ಖಾಲಿ)
    {
	fd = mch_fopen("xim.log", "w");
	ಆದರೆ (fd == ಖಾಲಿ)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    ಹಿಂತಿರುಗಿ;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.ಒಳ_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.ಒಳ_use && *p_imsf != NUL)
#ಇಲ್ಲದಿದ್ದರೆ
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
ಸ್ಥಿರ callback_T imaf_cb;	    // 'imactivatefunc' callback function
ಸ್ಥಿರ callback_T imsf_cb;	    // 'imstatusfunc' callback function

    ಅಕ್ಷರ *
did_ಗಣ_imactivatefunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (option_ಗಣ_callback_func(p_imaf, &imaf_cb) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಅಕ್ಷರ *
did_ಗಣ_imstatusfunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (option_ಗಣ_callback_func(p_imsf, &imsf_cb) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
call_imactivatefunc(ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ)
{
    typval_T argv[2];
    ಪೂರ್ಣಾಂಕ save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;

    argv[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argv[0].vval.v_ಸಂಖ್ಯೆ = ಸಕ್ರಿಯ ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (ಶೂನ್ಯ)call_callback_retnr(&imaf_cb, 1, argv);

    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_imstatusfunc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ is_ಸಕ್ರಿಯ;
    ಪೂರ್ಣಾಂಕ save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    ಆದರೆ (exiting || is_autocmd_blocked())
	ಹಿಂತಿರುಗಿ FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++ಸಂದೇಶ_silent;
    is_ಸಕ್ರಿಯ = call_callback_retnr(&imsf_cb, 0, ಖಾಲಿ);
    --ಸಂದೇಶ_silent;

    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
    ಹಿಂತಿರುಗಿ (is_ಸಕ್ರಿಯ > 0);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_xim_stuff(ಶೂನ್ಯ)
{
# ಆದರೆ defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_im_funcs(ಪೂರ್ಣಾಂಕ copyID ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ abort = FALSE;

# ಆದರೆ defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = ಗಣ_ref_ಒಳ_callback(&imaf_cb, copyID);
    abort = abort || ಗಣ_ref_ಒಳ_callback(&imsf_cb, copyID);
# endif

    ಹಿಂತಿರುಗಿ abort;
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_XIM)

# ಆದರೆ defined(FEAT_GUI_GTK)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_preedit_ಆರಂಭ_ಲಂಬಸಾಲು(ಶೂನ್ಯ)
{
    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	preedit_ಆರಂಭ_ಲಂಬಸಾಲು = cmdline_getvcol_cursor();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin != ಖಾಲಿ && curwin->w_ಬಫರ್ != ಖಾಲಿ)
	getvcol(curwin, &curwin->w_cursor, &preedit_ಆರಂಭ_ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_ಬದಲಾಯಿತು_while_preediting = curbuf->b_ಬದಲಾಯಿತು;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ im_is_ಸಕ್ರಿಯ	       = FALSE;	// IM is enabled for current mode
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ preedit_is_ಸಕ್ರಿಯ   = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ im_preedit_cursor   = 0;	// cursor offset in characters
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ im_preedit_trailing = 0;	// number of characters after cursor

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_ಕೀಲಿ_Void_ಚಿಹ್ನೆ
#    define GDK_ಕೀಲಿ_Void_ಚಿಹ್ನೆ GDK_Void_ಚಿಹ್ನೆ
#   endif
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  im_activatekey_keyval = GDK_ಕೀಲಿ_Void_ಚಿಹ್ನೆ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  im_activatekey_state  = 0;
#  endif

ಸ್ಥಿರ GtkWidget *preedit_ಕಿಟಕಿ = ಖಾಲಿ;
ಸ್ಥಿರ GtkWidget *preedit_label = ಖಾಲಿ;
#  ifdef USE_GTK4
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ preedit_label_ಅಗಲ = 0;	// last measured popover natural width
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ preedit_popover_ಎತ್ತರ = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
ಸ್ಥಿರ GtkCssProvider *preedit_css_provider = ಖಾಲಿ;
ಸ್ಥಿರ guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

ಸ್ಥಿರ ಶೂನ್ಯ im_preedit_ಕಿಟಕಿ_ಗಣ_position(ಶೂನ್ಯ);

    ಶೂನ್ಯ
im_ಗಣ_ಸಕ್ರಿಯ(ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ)
{
    ಪೂರ್ಣಾಂಕ was_ಸಕ್ರಿಯ;

    was_ಸಕ್ರಿಯ = !!im_get_ಸ್ಥಿತಿ();
    im_is_ಸಕ್ರಿಯ = (ಸಕ್ರಿಯ && !p_imdisable);

    ಆದರೆ (im_is_ಸಕ್ರಿಯ != was_ಸಕ್ರಿಯ)
	xim_reset();
}

    ಶೂನ್ಯ
xim_ಗಣ_focus(ಪೂರ್ಣಾಂಕ focus)
{
    ಆದರೆ (xic == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (focus)
	gtk_im_context_focus_ಒಳ(xic);
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_im_context_focus_ಹೊರಹೋಗು(xic);
}

    ಶೂನ್ಯ
im_ಗಣ_position(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆದರೆ (xic == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    GdkRectangle area;

    area.x = FILL_X(ಲಂಬಸಾಲು);
    area.y = FILL_Y(ಅಡ್ಡಸಾಲು);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.ಅಗಲ  = gui.char_ಅಗಲ
		    * ((Screen_ಸಾಲುಗಳು != ಖಾಲಿ && mb_lefthalve(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು)) ? 2 : 1);
    area.ಎತ್ತರ = gui.char_ಎತ್ತರ;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    ಆದರೆ (p_imst == IM_OVER_THE_SPOT)
    {
	ಪೂರ್ಣಾಂಕ popover_h = preedit_popover_ಎತ್ತರ;
	ಆದರೆ (popover_h <= 0)
	    popover_h = gui.char_ಎತ್ತರ;
	ಆದರೆ (popover_h > area.ಎತ್ತರ)
	    area.ಎತ್ತರ = popover_h;
    }
#  endif

    gtk_im_context_ಗಣ_cursor_location(xic, &area);

    ಆದರೆ (p_imst == IM_OVER_THE_SPOT)
	im_preedit_ಕಿಟಕಿ_ಗಣ_position();
}

#  ಆದರೆ 0 || defined(PROTO) // apparently only used in gui_x11.c
    ಶೂನ್ಯ
xim_ಗಣ_preedit(ಶೂನ್ಯ)
{
    im_ಗಣ_position(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು);
}
#  endif

    ಸ್ಥಿರ ಶೂನ್ಯ
im_ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು(char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&ಒಳಹರಿವು_conv, str, &len);
	g_return_if_fail(str != ಖಾಲಿ);
    }

    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(str, len);

    ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	vim_free(str);

    ಆದರೆ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
im_preedit_ಕಿಟಕಿ_ಗಣ_position(ಶೂನ್ಯ)
{
    ಆದರೆ (preedit_ಕಿಟಕಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#  ifdef USE_GTK4
    // GTK4: The popover is parented to gui.mainwin (the toplevel) rather
    // than gui.drawarea, because GtkDrawingArea is a leaf widget that does
    // not support children and causes snapshot/redraw artifacts when used
    // as a popover parent.  Translate the cursor cell coordinates from
    // drawarea space to mainwin space, and use the cached label width as
    // the anchor rect width so the popover's left edge aligns with the
    // cursor cell:
    //   popup_origin.x = anchor.x + anchor.w/2 - popup_w/2
    // With anchor.w == label_w and popup_w ≈ label_w (after CSS zeroing
    // popover frame padding), popup_origin.x ≈ anchor.x.
    GdkRectangle rect;
    graphene_ಬಿಂದು_t pt_ಒಳ, pt_ಹೊರಹೋಗು;

    pt_ಒಳ.x = FILL_X(gui.ಲಂಬಸಾಲು);
    pt_ಒಳ.y = FILL_Y(gui.ಅಡ್ಡಸಾಲು);
    ಆದರೆ (!gtk_widget_compute_ಬಿಂದು(gui.drawarea, gui.mainwin,
						&pt_ಒಳ, &pt_ಹೊರಹೋಗು))
    {
	pt_ಹೊರಹೋಗು.x = pt_ಒಳ.x;
	pt_ಹೊರಹೋಗು.y = pt_ಒಳ.y;
    }
    rect.x = (ಪೂರ್ಣಾಂಕ)pt_ಹೊರಹೋಗು.x;
    rect.y = (ಪೂರ್ಣಾಂಕ)pt_ಹೊರಹೋಗು.y;
    rect.ಅಗಲ = preedit_label_ಅಗಲ;
    rect.ಎತ್ತರ = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = gui.char_ಎತ್ತರ - preedit_popover_ಎತ್ತರ;
    gtk_popover_ಗಣ_ಸ್ಥಾನಾಂತರ(GTK_POPOVER(preedit_ಕಿಟಕಿ), 0, ಸ್ಥಾನಾಂತರ);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_ಗಣ_halign(preedit_ಕಿಟಕಿ, GTK_ALIGN_ಆರಂಭ);
    gtk_popover_ಗಣ_pointing_ಗಮ್ಯ(GTK_POPOVER(preedit_ಕಿಟಕಿ), &rect);
#  ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ x, y, ಅಗಲ, ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ screen_x, screen_y, screen_ಅಗಲ, screen_ಎತ್ತರ;

    gui_gtk_get_screen_geom_ದ_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_ಅಗಲ, &screen_ಎತ್ತರ);
    gdk_ಕಿಟಕಿ_get_origin(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea), &x, &y);
    gtk_ಕಿಟಕಿ_get_ಗಾತ್ರ(GTK_ಕಿಟಕಿ(preedit_ಕಿಟಕಿ), &ಅಗಲ, &ಎತ್ತರ);
    x = x + FILL_X(gui.ಲಂಬಸಾಲು);
    y = y + FILL_Y(gui.ಅಡ್ಡಸಾಲು);
    ಆದರೆ (x + ಅಗಲ > screen_x + screen_ಅಗಲ)
	x = screen_x + screen_ಅಗಲ - ಅಗಲ;
    ಆದರೆ (y + ಎತ್ತರ > screen_y + screen_ಎತ್ತರ)
	y = screen_y + screen_ಎತ್ತರ - ಎತ್ತರ;
    gtk_ಕಿಟಕಿ_move(GTK_ಕಿಟಕಿ(preedit_ಕಿಟಕಿ), x, y);
#  endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
im_preedit_ಕಿಟಕಿ_open(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ *preedit_string;
#  ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,16,0)
    ಅಕ್ಷರ buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#   ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,16,0)
    GdkRGBA color;
#   endif
#  ಇಲ್ಲದಿದ್ದರೆ
    GdkColor color;
#  endif
    gint w, h;

    ಆದರೆ (preedit_ಕಿಟಕಿ == ಖಾಲಿ)
    {
#  ifdef USE_GTK4
	// GTK4: use GtkPopover (xdg_popup) so the compositor keeps keyboard
	// focus on the drawing area and does not disable text-input-v3.
	// See issue #20257.  Parent the popover to gui.mainwin (a real
	// container) rather than gui.drawarea: GtkDrawingArea is a leaf
	// widget and using it as a popover parent causes snapshot artifacts
	// (white-out) on the drawing area.
	preedit_ಕಿಟಕಿ = gtk_popover_new();
	gtk_popover_ಗಣ_autohide(GTK_POPOVER(preedit_ಕಿಟಕಿ), FALSE);
	gtk_popover_ಗಣ_has_arrow(GTK_POPOVER(preedit_ಕಿಟಕಿ), FALSE);
	// Pin the popover below the anchor.  Together with anchor.h == 0 in
	// im_preedit_window_set_position() this lands the popover's top edge
	// exactly on FILL_Y(row), so the popover covers the cursor cell rather
	// than landing above it (the default GTK_POS_TOP would do that and
	// only flip to bottom when there is no room above, which is flaky).
	gtk_popover_ಗಣ_position(GTK_POPOVER(preedit_ಕಿಟಕಿ), GTK_POS_BOTTOM);
	gtk_widget_ಗಣ_can_focus(preedit_ಕಿಟಕಿ, FALSE);
	gtk_widget_ಗಣ_focusable(preedit_ಕಿಟಕಿ, FALSE);
	gtk_widget_ಗಣ_can_ಗುರಿ(preedit_ಕಿಟಕಿ, FALSE);
	gtk_widget_ಗಣ_ಪೋಷಕ(preedit_ಕಿಟಕಿ, gui.mainwin);
#  ಇಲ್ಲದಿದ್ದರೆ
	preedit_ಕಿಟಕಿ = gtk_ಕಿಟಕಿ_new(GTK_ಕಿಟಕಿ_POPUP);
	gtk_ಕಿಟಕಿ_ಗಣ_transient_for(GTK_ಕಿಟಕಿ(preedit_ಕಿಟಕಿ),
						     GTK_ಕಿಟಕಿ(gui.mainwin));
#  endif
	preedit_label = gtk_label_new("");
	gtk_widget_ಗಣ_name(preedit_label, "vim-gui-preedit-area");
#  ifdef USE_GTK4
	gtk_label_ಗಣ_xalign(GTK_LABEL(preedit_label), 0.0);
	gtk_widget_ಗಣ_halign(preedit_label, GTK_ALIGN_ಆರಂಭ);
	gtk_widget_ಗಣ_valign(preedit_label, GTK_ALIGN_ಆರಂಭ);
	// Tag the popover with a CSS class so the minimal frame CSS below
	// (no border, no rounded corners, no shadow, no padding) applies
	// only to our preedit popover and not other popovers in the app.
	gtk_widget_ಕೂಡಿಸು_css_class(preedit_ಕಿಟಕಿ, "vim-preedit-popover");
	gtk_popover_ಗಣ_ಮಗು(GTK_POPOVER(preedit_ಕಿಟಕಿ), preedit_label);
#  ಇಲ್ಲದಿದ್ದರೆ
	gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(preedit_ಕಿಟಕಿ), preedit_label);
#  endif
    }

#  ifdef USE_GTK4
    // Build (or rebuild on background-colour change) the CSS that flattens
    // the popover frame and paints its background with gui.back_pixel.
    // Using a solid background instead of `background: transparent` makes
    // the popover surface fully cover the cursor cell even when the popover
    // surface is allocated a pixel or two larger than the glyph bounding
    // box (e.g. due to residual theme insets) -- otherwise the blinking
    // text cursor at the cell origin shows through the popover frame.
    ಆದರೆ (preedit_css_provider == ಖಾಲಿ || preedit_css_cached_bg != gui.ಹಿಂಭಾಗ_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	ಆದರೆ (preedit_css_provider != ಖಾಲಿ)
	{
	    gtk_style_context_remove_provider_for_display(display,
				    GTK_STYLE_PROVIDER(preedit_css_provider));
	    g_object_unref(preedit_css_provider);
	}
	preedit_css_provider = gtk_css_provider_new();
	css = g_strdup_printf(
		"popover.vim-preedit-popover,\n"
		"popover.vim-preedit-popover > contents,\n"
		"popover.vim-preedit-popover label {\n"
		"  border: none;\n"
		"  border-radius: 0;\n"
		"  box-shadow: none;\n"
		"  padding: 0;\n"
		"  margin: 0;\n"
		"  min-width: 0;\n"
		"  min-height: 0;\n"
		"  background-color: #%02lx%02lx%02lx;\n"
		"}\n"
		"popover.vim-preedit-popover > arrow {\n"
		"  background: transparent;\n"
		"  border: none;\n"
		"}\n",
		(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)((gui.ಹಿಂಭಾಗ_pixel >> 16) & 0xff),
		(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)((gui.ಹಿಂಭಾಗ_pixel >> 8) & 0xff),
		(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(gui.ಹಿಂಭಾಗ_pixel & 0xff));
	gtk_css_provider_ತುಂಬು_from_string(preedit_css_provider, css);
	g_free(css);
	gtk_style_context_ಕೂಡಿಸು_provider_for_display(display,
			    GTK_STYLE_PROVIDER(preedit_css_provider), G_MAXUINT);
	preedit_css_cached_bg = gui.ಹಿಂಭಾಗ_pixel;
    }
#  endif

#  ifndef USE_GTK4
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,16,0)
    {
	GtkStyleContext * ಸ್ಥಿರಾಂಕ context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ಸ್ಥಿರಾಂಕ provider = gtk_css_provider_new();
	gchar		   *css = ಖಾಲಿ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_ಗಾತ್ರ(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = ಖಾಲಿ;

	ಆದರೆ (!pango_font_description_get_ಗಾತ್ರ_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ಸ್ಥಿರಾಂಕ screen
		  = gdk_ಕಿಟಕಿ_get_screen(gtk_widget_get_ಕಿಟಕಿ(gui.mainwin));
	    ಸ್ಥಿರಾಂಕ gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	ಆದರೆ (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	ಇಲ್ಲದಿದ್ದರೆ
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != ಖಾಲಿ ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.ಹಿಂಭಾಗ_pixel >> 16) & 0xff,
		(gui.ಹಿಂಭಾಗ_pixel >> 8) & 0xff,
		gui.ಹಿಂಭಾಗ_pixel & 0xff);

	gtk_css_provider_ತುಂಬು_from_ದತ್ತಾಂಶ(provider, css, -1, ಖಾಲಿ);
	gtk_style_context_ಕೂಡಿಸು_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, ಗಾತ್ರ(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_ಸಾಮಾನ್ಯ, &color);

    vim_snprintf(buf, ಗಾತ್ರ(buf), "#%06X", gui.ಹಿಂಭಾಗ_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_ಸಾಮಾನ್ಯ,
								      &color);
#   ಇಲ್ಲದಿದ್ದರೆ
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, ಗಾತ್ರ(buf), "#%06X", (ಚಿಹ್ನೆರಹಿತ)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_ಸಾಮಾನ್ಯ, &color);

    vim_snprintf(buf, ಗಾತ್ರ(buf), "#%06X", (ಚಿಹ್ನೆರಹಿತ)gui.ಹಿಂಭಾಗ_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_ಕಿಟಕಿ, GTK_STATE_ಸಾಮಾನ್ಯ, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ಖಾಲಿ);

    ಆದರೆ (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	ಆದರೆ (attr_list == ಖಾಲಿ)
	    attr_list = pango_attr_list_new();

	ಆದರೆ (gui.norm_font != ಖಾಲಿ)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
	    pa->ಕೊನೆ_ಸೂಚ್ಯಂಕ = G_MAXUINT;
	    pango_attr_list_ಸೇರಿಸು(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
	pa->ಕೊನೆ_ಸೂಚ್ಯಂಕ = G_MAXUINT;
	pango_attr_list_ಸೇರಿಸು(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.ಹಿಂಭಾಗ_pixel >> 16) & 0xff) * 257,
		((gui.ಹಿಂಭಾಗ_pixel >> 8) & 0xff) * 257,
		(gui.ಹಿಂಭಾಗ_pixel & 0xff) * 257);
	pa->ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
	pa->ಕೊನೆ_ಸೂಚ್ಯಂಕ = G_MAXUINT;
	pango_attr_list_ಸೇರಿಸು(attr_list, pa);
#  endif
	gtk_label_ಗಣ_ಪಠ್ಯ(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_ಗಣ_ಗುಣಲಕ್ಷಣಗಳು(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_ಗಾತ್ರ(layout, &w, &h);
	h = MAX(h, gui.char_ಎತ್ತರ);
#  ifdef USE_GTK4
	// Cache label/popover size derived from the Pango layout, which is
	// available without mapping the popover.  Using these for positioning
	// avoids calling gtk_popover_set_pointing_to() after popup, which
	// triggers a GDK "compositor doesn't support moving popups" warning
	// on compositors without xdg_popup.reposition (e.g. Weston).
	preedit_label_ಅಗಲ = w;
	preedit_popover_ಎತ್ತರ = h;

	// Report an enlarged cursor rectangle that covers both the cursor
	// cell and the preedit popover.  This way the IM (e.g. fcitx5)
	// places its candidate window below the popover instead of on top
	// of it.  Coordinates are in client-widget (drawarea) space.
	ಆದರೆ (xic != ಖಾಲಿ)
	{
	    GdkRectangle area;

	    area.x = FILL_X(gui.ಲಂಬಸಾಲು);
	    area.y = FILL_Y(gui.ಅಡ್ಡಸಾಲು);
	    area.ಅಗಲ = preedit_label_ಅಗಲ;
	    area.ಎತ್ತರ = MAX(gui.char_ಎತ್ತರ, preedit_popover_ಎತ್ತರ);
	    gtk_im_context_ಗಣ_cursor_location(xic, &area);
	}

	// Pop down before re-anchoring: on compositors that lack
	// xdg_popup.reposition (e.g. Weston), calling set_pointing_to() on
	// a mapped popover triggers GDK's remap fallback and a warning.
	// Doing the popdown ourselves makes the next popup land at the new
	// anchor cleanly without that warning.
	ಆದರೆ (gtk_widget_get_mapped(preedit_ಕಿಟಕಿ))
	    gtk_popover_popdown(GTK_POPOVER(preedit_ಕಿಟಕಿ));

	im_preedit_ಕಿಟಕಿ_ಗಣ_position();

	gtk_widget_ಸರತಿ_resize(preedit_ಕಿಟಕಿ);
	gtk_popover_popup(GTK_POPOVER(preedit_ಕಿಟಕಿ));
#  ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕಿಟಕಿ_resize(GTK_ಕಿಟಕಿ(preedit_ಕಿಟಕಿ), w, h);
	gtk_widget_show_all(preedit_ಕಿಟಕಿ);

	im_preedit_ಕಿಟಕಿ_ಗಣ_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
im_preedit_ಕಿಟಕಿ_close(ಶೂನ್ಯ)
{
    ಆದರೆ (preedit_ಕಿಟಕಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_ಕಿಟಕಿ));
#  ಇಲ್ಲದಿದ್ದರೆ
    gtk_widget_hide(preedit_ಕಿಟಕಿ);
#  endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
im_show_preedit(ಶೂನ್ಯ)
{
    im_preedit_ಕಿಟಕಿ_open();

    ಆದರೆ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
im_delete_preedit(ಶೂನ್ಯ)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    ಆದರೆ (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_ಕಿಟಕಿ_close();
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ
#  ifdef FEAT_TERMINAL
	    && !ಪದ_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (; im_preedit_cursor > 0; --im_preedit_cursor)
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(bskey, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(bskey));

    ಗಾಗಿ (; im_preedit_trailing > 0; --im_preedit_trailing)
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(delkey, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
im_correct_cursor(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_move_ಹಿಂಭಾಗ)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    ಆದರೆ (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
	ಹಿಂತಿರುಗಿ;
#  ifdef FEAT_RIGHTLEFT
    ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE) == 0 && curwin != ಖಾಲಿ && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    ಗಾಗಿ (; ಅಂಕೆ_move_ಹಿಂಭಾಗ > 0; --ಅಂಕೆ_move_ಹಿಂಭಾಗ)
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(backkey, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(backkey));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xim_expected_char = NUL;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
im_show_info(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    ಆದರೆ ((State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ) || (State & ಬಹುಲಕ_ಸೇರಿಸು))
	setcursor();
    ಹೊರಹೋಗು_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
im_commit_cb(GtkIMContext *context ಬಳಸದ,
	     ಸ್ಥಿರಾಂಕ gchar *str,
	     gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		slen = (ಪೂರ್ಣಾಂಕ)STRLEN(str);
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು = TRUE;
    ಪೂರ್ಣಾಂಕ		clen;
    ಪೂರ್ಣಾಂಕ		len = slen;
    ಪೂರ್ಣಾಂಕ		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	ಆದರೆ (preedit_ಆರಂಭ_ಲಂಬಸಾಲು == MAXCOL)
	{
	    init_preedit_ಆರಂಭ_ಲಂಬಸಾಲು();
	    commit_with_preedit = FALSE;
	}

	// The thing which setting "preedit_start_col" to MAXCOL means that
	// "preedit_start_col" will be set forcedly when calling
	// preedit_changed_cb() next time.
	// "preedit_start_col" should not reset with MAXCOL on this part. Vim
	// is simulating the preediting by using add_to_input_str(). when
	// preedit begin immediately before committed, the typebuf is not
	// flushed to screen, then it can't get correct "preedit_start_col".
	// Thus, it should calculate the cells by adding cells of the committed
	// string.
	ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&ಒಳಹರಿವು_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_ಆರಂಭ_ಲಂಬಸಾಲು += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    ಆದರೆ (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	ಆದರೆ (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    ಆದರೆ (ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು)
	im_ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು((char_u *)str, slen);

    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	ಆದರೆ (!commit_with_preedit)
	    preedit_ಆರಂಭ_ಲಂಬಸಾಲು = MAXCOL;

	// This flag is used in changed() at next call.
	xim_ಬದಲಾಯಿತು_while_preediting = TRUE;
    }

    ಆದರೆ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
im_preedit_ಆರಂಭ_cb(GtkIMContext *context ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_start_cb()\n");
#  endif

    im_is_ಸಕ್ರಿಯ = TRUE;
    preedit_is_ಸಕ್ರಿಯ = TRUE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after end to the preedit.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
im_preedit_ಕೊನೆ_cb(GtkIMContext *context ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
	preedit_ಆರಂಭ_ಲಂಬಸಾಲು = MAXCOL;
    xim_has_preediting = FALSE;

#  ಆದರೆ 0
    // Removal of this line suggested by Takuhiro Nishioka.  Fixes that IM was
    // switched off unintentionally.  We now use preedit_is_active (added by
    // SungHyun Nam).
    im_is_ಸಕ್ರಿಯ = FALSE;
#  endif
    preedit_is_ಸಕ್ರಿಯ = FALSE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after changes to the preedit string.  If the preedit
 * string was empty before, remember the preedit start column so we know
 * where to apply feedback attributes.  Delete the previous preedit string
 * if there was one, save the new preedit cursor offset, and put the new
 * string into the input buffer.
 *
 * TODO: The pragmatic "put into input buffer" approach used here has
 *       several fundamental problems:
 *
 * - The characters in the preedit string are subject to remapping.
 *   That's broken, only the finally committed string should be remapped.
 *
 * - There is a race condition involved:  The retrieved value for the
 *   current cursor position will be wrong if any unprocessed characters
 *   are still queued in the input buffer.
 *
 * - Due to the lack of synchronization between the file buffer in memory
 *   and any typed characters, it's practically impossible to implement the
 *   "retrieve_surrounding" and "delete_surrounding" signals reliably.  IM
 *   modules for languages such as Thai are likely to rely on this feature
 *   for proper operation.
 *
 * Conclusions:  I think support for preediting needs to be moved to the
 * core parts of Vim.  Ideally, until it has been committed, the preediting
 * string should only be displayed and not affect the buffer content at all.
 * The question how to deal with the synchronization issue still remains.
 * Circumventing the input buffer is probably not desirable.  Anyway, I think
 * implementing "retrieve_surrounding" is the only hard problem.
 *
 * One way to solve all of this in a clean manner would be to queue all key
 * press/release events "as is" in the input buffer, and apply the IM filtering
 * at the receiving end of the queue.  This, however, would have a rather large
 * impact on the code base.  If there is an easy way to force processing of all
 * remaining input from within the "retrieve_surrounding" signal handler, this
 * might not be necessary.  Gotta ask on vim-dev for opinions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
im_preedit_ಬದಲಾಯಿತು_cb(GtkIMContext *context, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಅಕ್ಷರ    *preedit_string = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	    cursor_ಸೂಚ್ಯಂಕ    = 0;
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_move_ಹಿಂಭಾಗ   = 0;
    char_u  *str;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ಖಾಲಿ,
					  &cursor_ಸೂಚ್ಯಂಕ);
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ಖಾಲಿ,
					  ಖಾಲಿ);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != ಖಾಲಿ); // just in case

    ಆದರೆ (p_imst == IM_OVER_THE_SPOT)
    {
	ಆದರೆ (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	ಆದರೆ (preedit_ಆರಂಭ_ಲಂಬಸಾಲು == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_ಆರಂಭ_ಲಂಬಸಾಲು();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cursor_ಸೂಚ್ಯಂಕ == 0 && preedit_string[0] == '\0')
	{
	    xim_has_preediting = FALSE;

	    // If at the start position (after typing backspace)
	    // preedit_start_col must be reset.
	    preedit_ಆರಂಭ_ಲಂಬಸಾಲು = MAXCOL;
	}

	im_delete_preedit();

	// Compute the end of the preediting area: "preedit_end_col".
	// According to the documentation of
	// gtk_im_context_get_preedit_string(), the cursor_pos output argument
	// returns the offset in bytes.  This is unfortunately not true -- real
	// life shows the offset is in characters, and the GTK+ source code
	// agrees with me.  Will file a bug later.
	ಆದರೆ (preedit_ಆರಂಭ_ಲಂಬಸಾಲು != MAXCOL)
	    preedit_ಕೊನೆ_ಲಂಬಸಾಲು = preedit_ಆರಂಭ_ಲಂಬಸಾಲು;
	str = (char_u *)preedit_string;
	ಗಾಗಿ (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    ಪೂರ್ಣಾಂಕ is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    ಆದರೆ (!is_composing || p_deco)
	    {
		ಆದರೆ (i < cursor_ಸೂಚ್ಯಂಕ)
		    ++im_preedit_cursor;
		ಇಲ್ಲದಿದ್ದರೆ
		    ++im_preedit_trailing;
	    }
	    ಆದರೆ (!is_composing && i >= cursor_ಸೂಚ್ಯಂಕ)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++ಅಂಕೆ_move_ಹಿಂಭಾಗ;
	    }
	    ಆದರೆ (preedit_ಆರಂಭ_ಲಂಬಸಾಲು != MAXCOL)
		preedit_ಕೊನೆ_ಲಂಬಸಾಲು += utf_ptr2cells(p);
	}

	ಆದರೆ (p > str)
	{
	    im_ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು(str, (ಪೂರ್ಣಾಂಕ)(p - str));
	    im_correct_cursor(ಅಂಕೆ_move_ಹಿಂಭಾಗ);
	}
    }

    g_free(preedit_string);

    ಆದರೆ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
translate_pango_ಗುಣಲಕ್ಷಣಗಳು(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    ಪೂರ್ಣಾಂಕ		    char_attr = HL_ಸಾಮಾನ್ಯ;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    ಆದರೆ (attr != ಖಾಲಿ && ((PangoAttrInt *)attr)->ಮೌಲ್ಯ
						 != (ಪೂರ್ಣಾಂಕ)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_ತೂಕ);
    ಆದರೆ (attr != ಖಾಲಿ && ((PangoAttrInt *)attr)->ಮೌಲ್ಯ >= (ಪೂರ್ಣಾಂಕ)PANGO_ತೂಕ_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    ಆದರೆ (attr != ಖಾಲಿ && ((PangoAttrInt *)attr)->ಮೌಲ್ಯ
						   != (ಪೂರ್ಣಾಂಕ)PANGO_STYLE_ಸಾಮಾನ್ಯ)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    ಆದರೆ (attr != ಖಾಲಿ)
    {
	ಸ್ಥಿರಾಂಕ PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	ಆದರೆ ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    ಹಿಂತಿರುಗಿ char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    ಪೂರ್ಣಾಂಕ
im_get_feedback_attr(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಅಕ್ಷರ	    *preedit_string = ಖಾಲಿ;
    PangoAttrList   *attr_list	    = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    char_attr	    = -1;

    ಆದರೆ (xic == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ಖಾಲಿ);

    ಆದರೆ (preedit_string != ಖಾಲಿ && attr_list != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ idx;

	// Get the byte index as used by PangoAttrIterator
	ಗಾಗಿ (idx = 0; ಲಂಬಸಾಲು > 0 && preedit_string[idx] != '\0'; --ಲಂಬಸಾಲು)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	ಆದರೆ (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    ಪೂರ್ಣಾಂಕ			ಆರಂಭ, ಕೊನೆ;

	    char_attr = HL_ಸಾಮಾನ್ಯ;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    ಮಾಡು
	    {
		pango_attr_iterator_range(iter, &ಆರಂಭ, &ಕೊನೆ);

		ಆದರೆ (idx >= ಆರಂಭ && idx < ಕೊನೆ)
		    char_attr |= translate_pango_ಗುಣಲಕ್ಷಣಗಳು(iter);
	    }
	    ಆಗ (pango_attr_iterator_ಮುಂದಿನ(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    ಆದರೆ (attr_list != ಖಾಲಿ)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    ಹಿಂತಿರುಗಿ char_attr;
}

    ಶೂನ್ಯ
xim_init(ಶೂನ್ಯ)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != ಖಾಲಿ);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) != ಖಾಲಿ);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_ಬದಲಾಯಿತು_cb), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_ಆರಂಭ_cb), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_ಕೊನೆ_cb), ಖಾಲಿ);

#  ifdef USE_GTK4
    gtk_im_context_ಗಣ_client_widget(xic, gui.drawarea);

    // Seed the cursor location immediately.  GTK4's Wayland IM module
    // batches text-input-v3 state and sends it on the next commit cycle
    // (typically triggered by focus_in).  Without this seed the very
    // first commit would carry the protocol default (0, 0, 0, 0) for the
    // cursor rectangle, and the IM's candidate window for the very first
    // composition would appear at the top-left of the surface instead of
    // next to the cursor.
    im_ಗಣ_position(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು);
#  ಇಲ್ಲದಿದ್ದರೆ
    gtk_im_context_ಗಣ_client_ಕಿಟಕಿ(xic, gtk_widget_get_ಕಿಟಕಿ(gui.drawarea));
#  endif
}

    ಶೂನ್ಯ
im_shutdown(ಶೂನ್ಯ)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    ಆದರೆ (xic != ಖಾಲಿ)
    {
	gtk_im_context_focus_ಹೊರಹೋಗು(xic);
	g_object_unref(xic);
	xic = ಖಾಲಿ;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    ಆದರೆ (preedit_ಕಿಟಕಿ != ಖಾಲಿ)
    {
	gtk_widget_unparent(preedit_ಕಿಟಕಿ);
	preedit_ಕಿಟಕಿ = ಖಾಲಿ;
	preedit_label = ಖಾಲಿ;
    }
    ಆದರೆ (preedit_css_provider != ಖಾಲಿ)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = ಖಾಲಿ;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_ಸಕ್ರಿಯ = FALSE;
    im_commit_handler_id = 0;
    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
	preedit_ಆರಂಭ_ಲಂಬಸಾಲು = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
/*
 * Convert the string argument to keyval and state for GdkEventKey.
 * If str is valid return TRUE, otherwise FALSE.
 *
 * See 'imactivatekey' for documentation of the format.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
im_string_ಗಮ್ಯ_keyval(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *keyval, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *state)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	    *mods_ಕೊನೆ;
    ಚಿಹ್ನೆರಹಿತ	    tmp_keyval;
    ಚಿಹ್ನೆರಹಿತ	    tmp_state = 0;

    mods_ಕೊನೆ = strrchr(str, '-');
    mods_ಕೊನೆ = (mods_ಕೊನೆ != ಖಾಲಿ) ? mods_ಕೊನೆ + 1 : str;

    // Parse modifier keys
    ಆಗ (str < mods_ಕೊನೆ)
	ಆಯ್ಕೆ (*str++)
	{
	    ಸ್ಥಿತಿ '-':							ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'S': ಸ್ಥಿತಿ 's': tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_ಸರಿಸು_ಮುಖವಾಡ;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'L': ಸ್ಥಿತಿ 'l': tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_ಬೀಗ_ಮುಖವಾಡ;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'C': ಸ್ಥಿತಿ 'c': tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_CONTROL_ಮುಖವಾಡ;ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '1':		tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_MOD1_ಮುಖವಾಡ;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '2':		tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_MOD2_ಮುಖವಾಡ;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '3':		tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_MOD3_ಮುಖವಾಡ;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '4':		tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_MOD4_ಮುಖವಾಡ;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '5':		tmp_state |= (ಚಿಹ್ನೆರಹಿತ)GDK_MOD5_ಮುಖವಾಡ;	ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    ಆದರೆ (tmp_keyval == 0 || tmp_keyval == GDK_Void_ಚಿಹ್ನೆ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (keyval != ಖಾಲಿ)
	*keyval = tmp_keyval;
    ಆದರೆ (state != ಖಾಲಿ)
	*state = tmp_state;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return TRUE if p_imak is valid, otherwise FALSE.  As a special case, an
 * empty string is also regarded as valid.
 *
 * Note: The numerical key value of p_imak is cached if it was valid; thus
 * boldly assuming im_xim_isvalid_imactivate() will always be called whenever
 * 'imak' changes.  This is currently the case but not obvious -- should
 * probably rename the function for clarity.
 */
    ಪೂರ್ಣಾಂಕ
im_xim_isvalid_imactivate(ಶೂನ್ಯ)
{
    ಆದರೆ (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_ಚಿಹ್ನೆ;
	im_activatekey_state  = 0;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ im_string_ಗಮ್ಯ_keyval((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    ಸ್ಥಿರ ಶೂನ್ಯ
im_synthesize_keypress(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keyval, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ state)
{
    Gdk_Event_ಕೀಲಿ *event;

    event = (Gdk_Event_ಕೀಲಿ *)gdk_event_new(GDK_ಕೀಲಿ_PRESS);
    g_object_ref(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea));
					// unreffed by gdk_event_free()
    event->ಕಿಟಕಿ_2 = gtk_widget_get_ಕಿಟಕಿ(gui.drawarea);
    event->send_event = TRUE;
    event->ಸಮಯ = GDK_CURRENT_ಸಮಯ;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysym_ಗಮ್ಯ_Keycode(GDK_ಕಿಟಕಿ_XDISPLAY(event->ಕಿಟಕಿ_2), (ಕೀಲಿ_Sym)keyval);
    event->ಉದ್ದಳತೆ = 0;
    event->string = ಖಾಲಿ;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_ಕೀಲಿ_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    ಶೂನ್ಯ
xim_reset(ಶೂನ್ಯ)
{
#  ifdef FEAT_EVAL
    ಆದರೆ (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_ಸಕ್ರಿಯ);
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
    ಆದರೆ (xic != ಖಾಲಿ)
    {
	gtk_im_context_reset(xic);

	ಆದರೆ (p_imdisable)
	    im_shutdown();
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    xim_ಗಣ_focus(gui.ಒಳ_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_ಗಣ_focus(gui.ಒಳ_focus);
#  ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (im_activatekey_keyval != GDK_Void_ಚಿಹ್ನೆ)
	    {
		ಆದರೆ (im_is_ಸಕ್ರಿಯ)
		{
		    g_signal_handler_ಖಂಡ(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		im_shutdown();
		xim_init();
		xim_ಗಣ_focus(gui.ಒಳ_focus);
	    }
#  endif
	}
    }

    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
	preedit_ಆರಂಭ_ಲಂಬಸಾಲು = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    ಪೂರ್ಣಾಂಕ
xim_ಸರತಿ_ಕೀಲಿ_press_event(Gdk_Event_ಕೀಲಿ *event, ಪೂರ್ಣಾಂಕ ಕೆಳಕ್ಕೆ)
{
#   ifdef FEAT_GUI_GTK
    ಆದರೆ (event->state & GDK_SUPER_ಮುಖವಾಡ) ಹಿಂತಿರುಗಿ FALSE;
#   endif
    ಆದರೆ (ಕೆಳಕ್ಕೆ)
    {
	// Workaround GTK2 XIM 'feature' that always converts keypad keys to
	// chars., even when not part of an IM sequence (ref. feature of
	// gdk/gdkkeyuni.c).
	// Flag any keypad keys that might represent a single char.
	// If this (on its own - i.e., not part of an IM sequence) is
	// committed while we're processing one of these keys, we can ignore
	// that commit and go ahead & process it ourselves.  That way we can
	// still distinguish keypad keys for use in mappings.
	// Also add GDK_space to make <S-Space> work.
	ಆಯ್ಕೆ (event->keyval)
	{
	    ಸ್ಥಿತಿ GDK_KP_ಕೂಡಿಸು:      xim_expected_char = '+';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_ಕಳೆ: xim_expected_char = '-';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_Divide:   xim_expected_char = '/';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_Multiply: xim_expected_char = '*';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_Decimal:  xim_expected_char = '.';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_Equal:    xim_expected_char = '=';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_0:	  xim_expected_char = '0';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_1:	  xim_expected_char = '1';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_2:	  xim_expected_char = '2';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_3:	  xim_expected_char = '3';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_4:	  xim_expected_char = '4';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_5:	  xim_expected_char = '5';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_6:	  xim_expected_char = '6';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_7:	  xim_expected_char = '7';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_8:	  xim_expected_char = '8';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_KP_9:	  xim_expected_char = '9';  ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_space:	  xim_expected_char = ' ';  ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    ಆದರೆ (xic != ಖಾಲಿ && !p_imdisable
		    && (State & (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE
					      | ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	ಆದರೆ (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ state_ಮುಖವಾಡ;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_ಮುಖವಾಡ  = im_activatekey_state;
	    state_ಮುಖವಾಡ |= ((ಪೂರ್ಣಾಂಕ)GDK_ಸರಿಸು_ಮುಖವಾಡ | (ಪೂರ್ಣಾಂಕ)GDK_CONTROL_ಮುಖವಾಡ
							| (ಪೂರ್ಣಾಂಕ)GDK_MOD1_ಮುಖವಾಡ);

	    ಆದರೆ ((event->state & state_ಮುಖವಾಡ) != im_activatekey_state)
		ಹಿಂತಿರುಗಿ FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    ಆದರೆ (event->type != GDK_ಕೀಲಿ_PRESS)
		ಹಿಂತಿರುಗಿ TRUE;

	    ಆದರೆ (map_ಗಮ್ಯ_exists_ಬಹುಲಕ((char_u *)"", ಬಹುಲಕ_LANGMAP, FALSE))
	    {
		im_ಗಣ_ಸಕ್ರಿಯ(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= ಬಹುಲಕ_LANGMAP;
		ಆದರೆ (State & ಬಹುಲಕ_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~ಬಹುಲಕ_LANGMAP;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= ಬಹುಲಕ_LANGMAP;
		}
		ಹಿಂತಿರುಗಿ TRUE;
	    }

	    ಹಿಂತಿರುಗಿ gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	ಆದರೆ (im_activatekey_keyval == GDK_Void_ಚಿಹ್ನೆ || im_is_ಸಕ್ರಿಯ)
	{
	    ಪೂರ್ಣಾಂಕ imresult = gtk_im_context_filter_keypress(xic, event);

	    ಆದರೆ (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		ಆದರೆ (!imresult && preedit_ಆರಂಭ_ಲಂಬಸಾಲು != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    ಹಿಂತಿರುಗಿ FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    ಆದರೆ (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		ಹಿಂತಿರುಗಿ FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    ಹಿಂತಿರುಗಿ imresult;
	}
    }

    ಹಿಂತಿರುಗಿ FALSE;
}
#  ಇಲ್ಲದಿದ್ದರೆ // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    ಪೂರ್ಣಾಂಕ
im_xim_isvalid_imactivate(ಶೂನ್ಯ)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    ಹಿಂತಿರುಗಿ p_imak[0] == NUL;
}

    ಪೂರ್ಣಾಂಕ
xim_ಸರತಿ_ಕೀಲಿ_press_event(GdkEvent *event ಬಳಸದ, ಪೂರ್ಣಾಂಕ ಕೆಳಕ್ಕೆ ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ FALSE;
}
#  endif // !USE_GTK4

    ಪೂರ್ಣಾಂಕ
im_get_ಸ್ಥಿತಿ(ಶೂನ್ಯ)
{
#  ifdef FEAT_EVAL
    ಆದರೆ (USE_IMSTATUSFUNC)
	ಹಿಂತಿರುಗಿ call_imstatusfunc();
#  endif
    ಹಿಂತಿರುಗಿ im_is_ಸಕ್ರಿಯ;
}

    ಪೂರ್ಣಾಂಕ
preedit_get_ಸ್ಥಿತಿ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ preedit_is_ಸಕ್ರಿಯ;
}

    ಪೂರ್ಣಾಂಕ
im_is_preediting(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ xim_has_preediting;
}

# ಇಲ್ಲದಿದ್ದರೆ // !FEAT_GUI_GTK

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	xim_is_ಸಕ್ರಿಯ = FALSE;  // XIM should be active in the current
					//ಬಹುಲಕ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
ಸ್ಥಿರ XIMStyle	ಒಳಹರಿವು_style;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಸ್ಥಿತಿ_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    ಶೂನ್ಯ
im_ಗಣ_ಸಕ್ರಿಯ(ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ_arg)
{
    ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ = ಸಕ್ರಿಯ_arg;

    // If 'imdisable' is set, XIM is never active.
    ಆದರೆ (p_imdisable)
	ಸಕ್ರಿಯ = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳಹರಿವು_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	ಸಕ್ರಿಯ = TRUE;

#  ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (USE_IMACTIVATEFUNC)
    {
	ಆದರೆ (ಸಕ್ರಿಯ != im_get_ಸ್ಥಿತಿ())
	{
	    call_imactivatefunc(ಸಕ್ರಿಯ);
	    xim_has_focus = ಸಕ್ರಿಯ;
	}
	ಹಿಂತಿರುಗಿ;
    }
#  endif

    ಆದರೆ (xic == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_ಸಕ್ರಿಯ = ಸಕ್ರಿಯ;
    xim_ಗಣ_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    ಶೂನ್ಯ
xim_ಗಣ_focus(ಪೂರ್ಣಾಂಕ focus)
{
    ಆದರೆ (xic == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    ಆದರೆ (focus && xim_is_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    ಶೂನ್ಯ
im_ಗಣ_position(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು ಬಳಸದ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು ಬಳಸದ)
{
    xim_ಗಣ_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    ಶೂನ್ಯ
xim_ಗಣ_preedit(ಶೂನ್ಯ)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    ಪೂರ್ಣಾಂಕ ಸಾಲು_space;

    ಆದರೆ (xic == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    xim_ಗಣ_focus(TRUE);

    ಆದರೆ (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							ಖಾಲಿ);
	XSetICValues(xic, XNPreedit_ಗುಣಲಕ್ಷಣಗಳು, attr_list, ಖಾಲಿ);
	XFree(attr_list);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಒಳಹರಿವು_style & XIMPreeditPosition)
    {
	ಆದರೆ (xim_fg_color == INVALCOLOR)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_ಹಿಂಭಾಗ_pixel;
	}
	over_spot.x = ಪಠ್ಯ_X(gui.ಲಂಬಸಾಲು);
	over_spot.y = ಪಠ್ಯ_Y(gui.ಅಡ್ಡಸಾಲು);
	spot_area.x = 0;
	spot_area.y = 0;
	spot_area.ಎತ್ತರ = gui.char_ಎತ್ತರ * ಅಡ್ಡಸಾಲುಗಳು;
	spot_area.ಅಗಲ  = gui.char_ಅಗಲ * Columns;
	ಸಾಲು_space = gui.char_ಎತ್ತರ;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, ಸಾಲು_space,
					ಖಾಲಿ);
	ಆದರೆ (XSetICValues(xic, XNPreedit_ಗುಣಲಕ್ಷಣಗಳು, attr_list, ಖಾಲಿ))
	    emsg(_(e_cannot_ಗಣ_ic_ಮೌಲ್ಯಗಳು));
	XFree(attr_list);
    }
}

#  ಆದರೆ defined(FEAT_GUI_X11)
#   ಆದರೆ defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xim_real_init(ಕಿಟಕಿ x11_ಕಿಟಕಿ, Display *x11_display);


#   ifdef USE_X11R6_XIM
    ಸ್ಥಿರ ಶೂನ್ಯ
xim_instantiate_cb(
    Display	*display,
    XPointer	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    XPointer	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಕಿಟಕಿ	x11_ಕಿಟಕಿ;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_ಕಿಟಕಿ, &x11_display);
    ಆದರೆ (display != x11_display)
	ಹಿಂತಿರುಗಿ;

    xim_real_init(x11_ಕಿಟಕಿ, x11_display);
    gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    ಆದರೆ (xic != ಖಾಲಿ)
	XUnregisterIMInstantiateCallback(x11_display, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
					 xim_instantiate_cb, ಖಾಲಿ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
xim_destroy_cb(
    XIM		im ಬಳಸದ,
    XPointer	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    XPointer	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಕಿಟಕಿ	x11_ಕಿಟಕಿ;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_ಕಿಟಕಿ, &x11_display);

    xic = ಖಾಲಿ;
    ಸ್ಥಿತಿ_area_enabled = FALSE;

    gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
				   xim_instantiate_cb, ಖಾಲಿ);
}
#   endif

    ಶೂನ್ಯ
xim_init(ಶೂನ್ಯ)
{
    ಕಿಟಕಿ	x11_ಕಿಟಕಿ;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_ಕಿಟಕಿ, &x11_display);

    xic = ಖಾಲಿ;

    ಆದರೆ (xim_real_init(x11_ಕಿಟಕಿ, x11_display))
	ಹಿಂತಿರುಗಿ;

    gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
				   xim_instantiate_cb, ಖಾಲಿ);
#   endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
xim_real_init(ಕಿಟಕಿ x11_ಕಿಟಕಿ, Display *x11_display)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಅಕ್ಷರ	*p,
		*s,
		*ns,
		*ಕೊನೆ,
		tmp[1024];
#   define IMLEN_MAX 40
    ಅಕ್ಷರ	buf[IMLEN_MAX + 7];
    XIM		xim = ಖಾಲಿ;
    XIMStyles	*xim_styles;
    XIMStyle	this_ಒಳಹರಿವು_style = 0;
    Boolean	ಸಿಕ್ಕಿತು;
    XPoint	over_spot;
    XVaNestedList preedit_list, ಸ್ಥಿತಿ_list;

    ಒಳಹರಿವು_style = 0;
    ಸ್ಥಿತಿ_area_enabled = FALSE;

    ಆದರೆ (xic != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (gui.rsrc_ಒಳಹರಿವು_ವಿಧಾನ != ಖಾಲಿ && *gui.rsrc_ಒಳಹರಿವು_ವಿಧಾನ != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_ಒಳಹರಿವು_ವಿಧಾನ,
	    ಗಾತ್ರ(tmp) - 1);
	ಗಾಗಿ (ns = s = tmp; ns != ಖಾಲಿ && *s != NUL;)
	{
	    s = (ಅಕ್ಷರ *)skipwhite((char_u *)s);
	    ಆದರೆ (*s == NUL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((ns = ಕೊನೆ = strchr(s, ',')) == ಖಾಲಿ)
		ಕೊನೆ = s + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
	    ಆಗ (ಸುರಕ್ಷಿತ_isspace(ಕೊನೆ[-1]))
		ಕೊನೆ--;
	    *ಕೊನೆ = NUL;

	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		ಆದರೆ ((p = XSetLocaleModifiers(buf)) != ಖಾಲಿ && *p != NUL
			&& (xim = XOpenIM(x11_display, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ))
								      != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
	    }

	    s = ns + 1;
	}
    }

    ಆದರೆ (xim == ಖಾಲಿ && (p = XSetLocaleModifiers("")) != ಖಾಲಿ && *p != NUL)
	xim = XOpenIM(x11_display, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    ಆದರೆ (xim == ಖಾಲಿ && (p = XSetLocaleModifiers("@im=none")) != ಖಾಲಿ
								 && *p != NUL)
	xim = XOpenIM(x11_display, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

    ಆದರೆ (xim == ಖಾಲಿ)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_ಗಮ್ಯ_open_ಒಳಹರಿವು_ವಿಧಾನ));
	    verbose_leave();
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_ದತ್ತಾಂಶ_2 = ಖಾಲಿ;
	ಆದರೆ (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, ಖಾಲಿ))
	    emsg(_(e_warning_could_not_ಗಣ_destroy_callback_ಗಮ್ಯ_im));
    }
#   endif

    ಆದರೆ (XGetIMValues(xim, XNQuery_ಒಳಹರಿವು_Style, &xim_styles, ಖಾಲಿ) || !xim_styles)
    {
	emsg(_(e_ಒಳಹರಿವು_ವಿಧಾನ_doesnt_support_any_style));
	XCloseIM(xim);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಸಿಕ್ಕಿತು = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    ಗಾತ್ರ(tmp) - 1);
    ಗಾಗಿ (s = tmp; s && !ಸಿಕ್ಕಿತು; )
    {
	ಆಗ (*s && ಸುರಕ್ಷಿತ_isspace(*s))
	    s++;
	ಆದರೆ (!*s)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ ((ns = ಕೊನೆ = strchr(s, ',')) != 0)
	    ns++;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೊನೆ = s + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
	ಆಗ (ಸುರಕ್ಷಿತ_isspace(*ಕೊನೆ))
	    ಕೊನೆ--;
	*ಕೊನೆ = '\0';

	ಆದರೆ (!strcmp(s, "OverTheSpot"))
	    this_ಒಳಹರಿವು_style = (XIMPreeditPosition | XIMStatusArea);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strcmp(s, "OffTheSpot"))
	    this_ಒಳಹರಿವು_style = (XIMPreeditArea | XIMStatusArea);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strcmp(s, "Root"))
	    this_ಒಳಹರಿವು_style = (XIMPreeditNothing | XIMStatusNothing);

	ಗಾಗಿ (i = 0; (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)i < xim_styles->ಎಣಿಕೆ_styles; i++)
	{
	    ಆದರೆ (this_ಒಳಹರಿವು_style == xim_styles->supported_styles[i])
	    {
		ಸಿಕ್ಕಿತು = True;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (!ಸಿಕ್ಕಿತು)
	    ಗಾಗಿ (i = 0; (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)i < xim_styles->ಎಣಿಕೆ_styles; i++)
	    {
		ಆದರೆ ((xim_styles->supported_styles[i] & this_ಒಳಹರಿವು_style)
			== (this_ಒಳಹರಿವು_style & ~XIMStatusArea))
		{
		    this_ಒಳಹರಿವು_style &= ~XIMStatusArea;
		    ಸಿಕ್ಕಿತು = True;
		    ನಿಲ್ಲಿಸು;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    ಆದರೆ (!ಸಿಕ್ಕಿತು)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_ಒಳಹರಿವು_ವಿಧಾನ_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    over_spot.x = ಪಠ್ಯ_X(gui.ಲಂಬಸಾಲು);
    over_spot.y = ಪಠ್ಯ_Y(gui.ಅಡ್ಡಸಾಲು);
    ಒಳಹರಿವು_style = this_ಒಳಹರಿವು_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    ಆದರೆ (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ಹಿಂಭಾಗ_pixel,
				XNFont_ಗಣ, (XFont_ಗಣ)gui.fontset,
				ಖಾಲಿ);
	ಸ್ಥಿತಿ_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ಹಿಂಭಾಗ_pixel,
				XNFont_ಗಣ, (XFont_ಗಣ)gui.fontset,
				ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ಹಿಂಭಾಗ_pixel,
				ಖಾಲಿ);
	ಸ್ಥಿತಿ_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_ಹಿಂಭಾಗ_pixel,
				ಖಾಲಿ);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, ಒಳಹರಿವು_style,
		    XNClient_ಕಿಟಕಿ, x11_ಕಿಟಕಿ,
		    XNFocus_ಕಿಟಕಿ, gui.wid,
		    XNPreedit_ಗುಣಲಕ್ಷಣಗಳು, preedit_list,
		    XNStatus_ಗುಣಲಕ್ಷಣಗಳು, ಸ್ಥಿತಿ_list,
		    ಖಾಲಿ);
    XFree(ಸ್ಥಿತಿ_list);
    XFree(preedit_list);
    ಆದರೆ (xic != ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳಹರಿವು_style & XIMStatusArea)
	{
	    xim_ಗಣ_ಸ್ಥಿತಿ_area();
	    ಸ್ಥಿತಿ_area_enabled = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!is_not_a_ಪದ())
	    emsg(_(e_failed_ಗಮ್ಯ_create_ಒಳಹರಿವು_context));
	XCloseIM(xim);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    ಪೂರ್ಣಾಂಕ
im_get_ಸ್ಥಿತಿ(ಶೂನ್ಯ)
{
#  ifdef FEAT_EVAL
    ಆದರೆ (USE_IMSTATUSFUNC)
	ಹಿಂತಿರುಗಿ call_imstatusfunc();
#  endif
    ಹಿಂತಿರುಗಿ xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# ಆದರೆ !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    ಶೂನ್ಯ
xim_ಗಣ_ಸ್ಥಿತಿ_area(ಶೂನ್ಯ)
{
    XVaNestedList preedit_list = 0, ಸ್ಥಿತಿ_list = 0, list = 0;
    XRectangle pre_area, ಸ್ಥಿತಿ_area;

    ಆದರೆ (xic == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳಹರಿವು_style & XIMStatusArea)
    {
	ಆದರೆ (ಒಳಹರಿವು_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    ಸ್ಥಿತಿ_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, ಖಾಲಿ);
	    XGetICValues(xic, XNStatus_ಗುಣಲಕ್ಷಣಗಳು, ಸ್ಥಿತಿ_list, ಖಾಲಿ);
	    XFree(ಸ್ಥಿತಿ_list);

	    ಸ್ಥಿತಿ_area.ಅಗಲ = needed_rect->ಅಗಲ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಿತಿ_area.ಅಗಲ = gui.char_ಅಗಲ * Columns;

	ಸ್ಥಿತಿ_area.x = 0;
	ಸ್ಥಿತಿ_area.y = gui.char_ಎತ್ತರ * ಅಡ್ಡಸಾಲುಗಳು + gui.border_ಸ್ಥಾನಾಂತರ;
	ಆದರೆ (gui.which_scrollbars[SBAR_BOTTOM])
	    ಸ್ಥಿತಿ_area.y += gui.scrollbar_ಎತ್ತರ;
#  ifdef FEAT_MENU
	ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	    ಸ್ಥಿತಿ_area.y += gui.menu_ಎತ್ತರ;
#  endif
	ಸ್ಥಿತಿ_area.ಎತ್ತರ = gui.char_ಎತ್ತರ;
	ಸ್ಥಿತಿ_list = XVaCreateNestedList(0, XNArea, &ಸ್ಥಿತಿ_area, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸ್ಥಿತಿ_area.x = 0;
	ಸ್ಥಿತಿ_area.y = gui.char_ಎತ್ತರ * ಅಡ್ಡಸಾಲುಗಳು + gui.border_ಸ್ಥಾನಾಂತರ;
	ಆದರೆ (gui.which_scrollbars[SBAR_BOTTOM])
	    ಸ್ಥಿತಿ_area.y += gui.scrollbar_ಎತ್ತರ;
#  ifdef FEAT_MENU
	ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	    ಸ್ಥಿತಿ_area.y += gui.menu_ಎತ್ತರ;
#  endif
	ಸ್ಥಿತಿ_area.ಅಗಲ = 0;
	ಸ್ಥಿತಿ_area.ಎತ್ತರ = gui.char_ಎತ್ತರ;
    }

    ಆದರೆ (ಒಳಹರಿವು_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = ಸ್ಥಿತಿ_area.x + ಸ್ಥಿತಿ_area.ಅಗಲ;
	pre_area.y = gui.char_ಎತ್ತರ * ಅಡ್ಡಸಾಲುಗಳು + gui.border_ಸ್ಥಾನಾಂತರ;
	pre_area.ಅಗಲ = gui.char_ಅಗಲ * Columns - pre_area.x;
	ಆದರೆ (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_ಎತ್ತರ;
#  ifdef FEAT_MENU
	ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	    pre_area.y += gui.menu_ಎತ್ತರ;
#  endif
	pre_area.ಎತ್ತರ = gui.char_ಎತ್ತರ;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳಹರಿವು_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.ಎತ್ತರ = gui.char_ಎತ್ತರ * ಅಡ್ಡಸಾಲುಗಳು;
	pre_area.ಅಗಲ = gui.char_ಅಗಲ * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ಖಾಲಿ);
    }

    ಆದರೆ (preedit_list && ಸ್ಥಿತಿ_list)
	list = XVaCreateNestedList(0, XNPreedit_ಗುಣಲಕ್ಷಣಗಳು, preedit_list,
				   XNStatus_ಗುಣಲಕ್ಷಣಗಳು, ಸ್ಥಿತಿ_list, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (preedit_list)
	list = XVaCreateNestedList(0, XNPreedit_ಗುಣಲಕ್ಷಣಗಳು, preedit_list,
				   ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಿತಿ_list)
	list = XVaCreateNestedList(0, XNStatus_ಗುಣಲಕ್ಷಣಗಳು, ಸ್ಥಿತಿ_list,
				   ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	list = ಖಾಲಿ;

    ಆದರೆ (list)
    {
	XSetICValues(xic, XNVaNestedList, list, ಖಾಲಿ);
	XFree(list);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_list)
	XFree(ಸ್ಥಿತಿ_list);
    ಆದರೆ (preedit_list)
	XFree(preedit_list);
}

    ಪೂರ್ಣಾಂಕ
xim_get_ಸ್ಥಿತಿ_area_ಎತ್ತರ(ಶೂನ್ಯ)
{
    ಆದರೆ (ಸ್ಥಿತಿ_area_enabled)
	ಹಿಂತಿರುಗಿ gui.char_ಎತ್ತರ;
    ಹಿಂತಿರುಗಿ 0;
}
# endif

#ಇಲ್ಲದಿದ್ದರೆ // !defined(FEAT_XIM)

# ಆದರೆ defined(IME_WITHOUT_XIM) || defined(VIMDLL)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ im_was_ಗಣ_ಸಕ್ರಿಯ = FALSE;

    ಪೂರ್ಣಾಂಕ
#  ifdef VIMDLL
mbyte_im_get_ಸ್ಥಿತಿ(ಶೂನ್ಯ)
#  ಇಲ್ಲದಿದ್ದರೆ
im_get_ಸ್ಥಿತಿ(ಶೂನ್ಯ)
#  endif
{
#  ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (USE_IMSTATUSFUNC)
	ಹಿಂತಿರುಗಿ call_imstatusfunc();
#  endif
    ಹಿಂತಿರುಗಿ im_was_ಗಣ_ಸಕ್ರಿಯ;
}

    ಶೂನ್ಯ
#  ifdef VIMDLL
mbyte_im_ಗಣ_ಸಕ್ರಿಯ(ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ_arg)
#  ಇಲ್ಲದಿದ್ದರೆ
im_ಗಣ_ಸಕ್ರಿಯ(ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ_arg)
#  endif
{
#  ಆದರೆ defined(FEAT_EVAL)
    ಪೂರ್ಣಾಂಕ	    ಸಕ್ರಿಯ = !p_imdisable && ಸಕ್ರಿಯ_arg;

    ಆದರೆ (USE_IMACTIVATEFUNC && ಸಕ್ರಿಯ != im_get_ಸ್ಥಿತಿ())
    {
	call_imactivatefunc(ಸಕ್ರಿಯ);
	im_was_ಗಣ_ಸಕ್ರಿಯ = ಸಕ್ರಿಯ;
    }
#  endif
}

#  ಆದರೆ defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    ಶೂನ್ಯ
im_ಗಣ_position(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು ಬಳಸದ, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು ಬಳಸದ)
{
}
#  endif
# endif

#ಆದರೆ_ಅಂತ್ಯ // FEAT_XIM
