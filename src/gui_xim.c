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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି !defined(GTK_CHECK_VERSION)
# define GTK_CHECK_VERSION(a, b, c) 0
#endif

#ଯଦି defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ଅନ୍ୟଥା
#  include <gdk/gdkkeysyms.h>
# endif
# ଯଦି !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  ଅନ୍ୟଥା
#   include <gdk/gdkx.h>
#  endif
# endif
#endif

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#ଯଦି defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
ସ୍ଥିର ଶୂନ୍ୟ xim_log(ଅକ୍ଷର *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    ସ୍ଥିର ଶୂନ୍ୟ
xim_log(ଅକ୍ଷର *s, ...)
{
    va_list arglist;
    ସ୍ଥିର FILE *fd = ଖାଲି;

    ଯଦି (fd == (FILE *)-1)
	ଫେରନ୍ତୁ;
    ଯଦି (fd == ଖାଲି)
    {
	fd = mch_fopen("xim.log", "w");
	ଯଦି (fd == ଖାଲି)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    ଫେରନ୍ତୁ;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#endif

#ଯଦି defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.in_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.in_use && *p_imsf != NUL)
#ଅନ୍ୟଥା
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#endif

#ଯଦି defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
ସ୍ଥିର callback_T imaf_cb;	    // 'imactivatefunc' callback function
ସ୍ଥିର callback_T imsf_cb;	    // 'imstatusfunc' callback function

    ଅକ୍ଷର *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    ଯଦି (option_set_callback_func(p_imaf, &imaf_cb) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ଅକ୍ଷର *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    ଯଦି (option_set_callback_func(p_imsf, &imsf_cb) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
call_imactivatefunc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା active)
{
    typval_T argv[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ଚାବି_Typed = ଚାବି_Typed;

    argv[0].v_type = VAR_ସଂଖ୍ୟା;
    argv[0].vval.v_ସଂଖ୍ୟା = active ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (ଶୂନ୍ୟ)call_callback_retnr(&imaf_cb, 1, argv);

    ଚାବି_Typed = save_ଚାବି_Typed;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_imstatusfunc(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_active;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ଚାବି_Typed = ଚାବି_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    ଯଦି (exiting || is_autocmd_blocked())
	ଫେରନ୍ତୁ FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++ସନ୍ଦେଶ_silent;
    is_active = call_callback_retnr(&imsf_cb, 0, ଖାଲି);
    --ସନ୍ଦେଶ_silent;

    ଚାବି_Typed = save_ଚାବି_Typed;
    ଫେରନ୍ତୁ (is_active > 0);
}
#endif

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_xim_stuff(ଶୂନ୍ୟ)
{
# ଯଦି defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_im_funcs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା abort = FALSE;

# ଯଦି defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = set_ref_in_callback(&imaf_cb, copyID);
    abort = abort || set_ref_in_callback(&imsf_cb, copyID);
# endif

    ଫେରନ୍ତୁ abort;
}
#endif


#ଯଦି defined(FEAT_XIM)

# ଯଦି defined(FEAT_GUI_GTK)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_preedit_start_col(ଶୂନ୍ୟ)
{
    ଯଦି (State & MODE_CMDLINE)
	preedit_start_col = cmdline_getvcol_cursor();
    ଅନ୍ୟଥା ଯଦି (curwin != ଖାଲି && curwin->w_buffer != ଖାଲି)
	getvcol(curwin, &curwin->w_cursor, &preedit_start_col, ଖାଲି, ଖାଲି, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_changed_while_preediting = curbuf->b_changed;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା im_is_active	       = FALSE;	// IM is enabled for current mode
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା preedit_is_active   = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା im_preedit_cursor   = 0;	// cursor offset in characters
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା im_preedit_trailing = 0;	// number of characters after cursor

ସ୍ଥିର unsigned long im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_ଚାବି_Void_ପ୍ରତୀକ
#    define GDK_ଚାବି_Void_ପ୍ରତୀକ GDK_Void_ପ୍ରତୀକ
#   endif
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  im_activatekey_keyval = GDK_ଚାବି_Void_ପ୍ରତୀକ;
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  im_activatekey_state  = 0;
#  endif

ସ୍ଥିର GtkWidget *preedit_window = ଖାଲି;
ସ୍ଥିର GtkWidget *preedit_label = ଖାଲି;
#  ifdef USE_GTK4
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା preedit_label_width = 0;	// last measured popover natural width
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା preedit_popover_height = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
ସ୍ଥିର GtkCssProvider *preedit_css_provider = ଖାଲି;
ସ୍ଥିର guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

ସ୍ଥିର ଶୂନ୍ୟ im_preedit_window_set_position(ଶୂନ୍ୟ);

    ଶୂନ୍ୟ
im_set_active(ପୂର୍ଣ୍ଣସଂଖ୍ୟା active)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା was_active;

    was_active = !!im_get_status();
    im_is_active = (active && !p_imdisable);

    ଯଦି (im_is_active != was_active)
	xim_reset();
}

    ଶୂନ୍ୟ
xim_set_focus(ପୂର୍ଣ୍ଣସଂଖ୍ୟା focus)
{
    ଯଦି (xic == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (focus)
	gtk_im_context_focus_in(xic);
    ଅନ୍ୟଥା
	gtk_im_context_focus_out(xic);
}

    ଶୂନ୍ୟ
im_set_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯଦି (xic == ଖାଲି)
	ଫେରନ୍ତୁ;

    GdkRectangle area;

    area.x = FILL_X(col);
    area.y = FILL_Y(row);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.width  = gui.char_width
		    * ((Screen_ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି && mb_lefthalve(row, col)) ? 2 : 1);
    area.height = gui.char_height;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    ଯଦି (p_imst == IM_OVER_THE_SPOT)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା popover_h = preedit_popover_height;
	ଯଦି (popover_h <= 0)
	    popover_h = gui.char_height;
	ଯଦି (popover_h > area.height)
	    area.height = popover_h;
    }
#  endif

    gtk_im_context_set_cursor_location(xic, &area);

    ଯଦି (p_imst == IM_OVER_THE_SPOT)
	im_preedit_window_set_position();
}

#  ଯଦି 0 || defined(PROTO) // apparently only used in gui_x11.c
    ଶୂନ୍ୟ
xim_set_preedit(ଶୂନ୍ୟ)
{
    im_set_position(gui.row, gui.col);
}
#  endif

    ସ୍ଥିର ଶୂନ୍ୟ
im_add_to_ନିବେଶ(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    ଯଦି (ନିବେଶ_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&ନିବେଶ_conv, str, &len);
	g_return_if_fail(str != ଖାଲି);
    }

    add_to_ନିବେଶ_buf_csi(str, len);

    ଯଦି (ନିବେଶ_conv.vc_type != CONV_NONE)
	vim_free(str);

    ଯଦି (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
im_preedit_window_set_position(ଶୂନ୍ୟ)
{
    ଯଦି (preedit_window == ଖାଲି)
	ଫେରନ୍ତୁ;

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
    graphene_point_t pt_in, pt_out;

    pt_in.x = FILL_X(gui.col);
    pt_in.y = FILL_Y(gui.row);
    ଯଦି (!gtk_widget_compute_point(gui.drawarea, gui.mainwin,
						&pt_in, &pt_out))
    {
	pt_out.x = pt_in.x;
	pt_out.y = pt_in.y;
    }
    rect.x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pt_out.x;
    rect.y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pt_out.y;
    rect.width = preedit_label_width;
    rect.height = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = gui.char_height - preedit_popover_height;
    gtk_popover_set_offset(GTK_POPOVER(preedit_window), 0, offset);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_set_halign(preedit_window, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(preedit_window), &rect);
#  ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, y, width, height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_x, screen_y, screen_width, screen_height;

    gui_gtk_get_screen_geom_of_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_width, &screen_height);
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), &x, &y);
    gtk_window_get_size(GTK_WINDOW(preedit_window), &width, &height);
    x = x + FILL_X(gui.col);
    y = y + FILL_Y(gui.row);
    ଯଦି (x + width > screen_x + screen_width)
	x = screen_x + screen_width - width;
    ଯଦି (y + height > screen_y + screen_height)
	y = screen_y + screen_height - height;
    gtk_window_move(GTK_WINDOW(preedit_window), x, y);
#  endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
im_preedit_window_open(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର *preedit_string;
#  ଯଦି !GTK_CHECK_VERSION(3,16,0)
    ଅକ୍ଷର buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  ଯଦି GTK_CHECK_VERSION(3,0,0)
#   ଯଦି !GTK_CHECK_VERSION(3,16,0)
    GdkRGBA color;
#   endif
#  ଅନ୍ୟଥା
    GdkColor color;
#  endif
    gint w, h;

    ଯଦି (preedit_window == ଖାଲି)
    {
#  ifdef USE_GTK4
	// GTK4: use GtkPopover (xdg_popup) so the compositor keeps keyboard
	// focus on the drawing area and does not disable text-input-v3.
	// See issue #20257.  Parent the popover to gui.mainwin (a real
	// container) rather than gui.drawarea: GtkDrawingArea is a leaf
	// widget and using it as a popover parent causes snapshot artifacts
	// (white-out) on the drawing area.
	preedit_window = gtk_popover_new();
	gtk_popover_set_autohide(GTK_POPOVER(preedit_window), FALSE);
	gtk_popover_set_has_arrow(GTK_POPOVER(preedit_window), FALSE);
	// Pin the popover below the anchor.  Together with anchor.h == 0 in
	// im_preedit_window_set_position() this lands the popover's top edge
	// exactly on FILL_Y(row), so the popover covers the cursor cell rather
	// than landing above it (the default GTK_POS_TOP would do that and
	// only flip to bottom when there is no room above, which is flaky).
	gtk_popover_set_position(GTK_POPOVER(preedit_window), GTK_POS_BOTTOM);
	gtk_widget_set_can_focus(preedit_window, FALSE);
	gtk_widget_set_focusable(preedit_window, FALSE);
	gtk_widget_set_can_target(preedit_window, FALSE);
	gtk_widget_set_parent(preedit_window, gui.mainwin);
#  ଅନ୍ୟଥା
	preedit_window = gtk_window_new(GTK_WINDOW_POPUP);
	gtk_window_set_transient_for(GTK_WINDOW(preedit_window),
						     GTK_WINDOW(gui.mainwin));
#  endif
	preedit_label = gtk_label_new("");
	gtk_widget_set_name(preedit_label, "vim-gui-preedit-area");
#  ifdef USE_GTK4
	gtk_label_set_xalign(GTK_LABEL(preedit_label), 0.0);
	gtk_widget_set_halign(preedit_label, GTK_ALIGN_START);
	gtk_widget_set_valign(preedit_label, GTK_ALIGN_START);
	// Tag the popover with a CSS class so the minimal frame CSS below
	// (no border, no rounded corners, no shadow, no padding) applies
	// only to our preedit popover and not other popovers in the app.
	gtk_widget_add_css_class(preedit_window, "vim-preedit-popover");
	gtk_popover_set_child(GTK_POPOVER(preedit_window), preedit_label);
#  ଅନ୍ୟଥା
	gtk_container_add(GTK_CONTAINER(preedit_window), preedit_label);
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
    ଯଦି (preedit_css_provider == ଖାଲି || preedit_css_cached_bg != gui.back_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	ଯଦି (preedit_css_provider != ଖାଲି)
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
		(unsigned long)((gui.back_pixel >> 16) & 0xff),
		(unsigned long)((gui.back_pixel >> 8) & 0xff),
		(unsigned long)(gui.back_pixel & 0xff));
	gtk_css_provider_load_from_string(preedit_css_provider, css);
	g_free(css);
	gtk_style_context_add_provider_for_display(display,
			    GTK_STYLE_PROVIDER(preedit_css_provider), G_MAXUINT);
	preedit_css_cached_bg = gui.back_pixel;
    }
#  endif

#  ifndef USE_GTK4
#   ଯଦି GTK_CHECK_VERSION(3,16,0)
    {
	GtkStyleContext * ଅଚଳ context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ଅଚଳ provider = gtk_css_provider_new();
	gchar		   *css = ଖାଲି;
	ଅଚଳ ଅକ୍ଷର * ଅଚଳ fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_size(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = ଖାଲି;

	ଯଦି (!pango_font_description_get_size_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ଅଚଳ screen
		  = gdk_window_get_screen(gtk_widget_get_window(gui.mainwin));
	    ଅଚଳ gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	ଯଦି (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	ଅନ୍ୟଥା
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != ଖାଲି ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.back_pixel >> 16) & 0xff,
		(gui.back_pixel >> 8) & 0xff,
		gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, ଖାଲି);
	gtk_style_context_add_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_VERSION(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, ଆକାର(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_NORMAL, &color);

    vim_snprintf(buf, ଆକାର(buf), "#%06X", gui.back_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_NORMAL,
								      &color);
#   ଅନ୍ୟଥା
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, ଆକାର(buf), "#%06X", (unsigned)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_NORMAL, &color);

    vim_snprintf(buf, ଆକାର(buf), "#%06X", (unsigned)gui.back_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_window, GTK_STATE_NORMAL, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ଖାଲି);

    ଯଦି (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	ଯଦି (attr_list == ଖାଲି)
	    attr_list = pango_attr_list_new();

	ଯଦି (gui.norm_font != ଖାଲି)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->start_index = 0;
	    pa->end_index = G_MAXUINT;
	    pango_attr_list_ଭର୍ତ୍ତି(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_ଭର୍ତ୍ତି(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.back_pixel >> 16) & 0xff) * 257,
		((gui.back_pixel >> 8) & 0xff) * 257,
		(gui.back_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_ଭର୍ତ୍ତି(attr_list, pa);
#  endif
	gtk_label_set_text(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_set_attributes(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_size(layout, &w, &h);
	h = MAX(h, gui.char_height);
#  ifdef USE_GTK4
	// Cache label/popover size derived from the Pango layout, which is
	// available without mapping the popover.  Using these for positioning
	// avoids calling gtk_popover_set_pointing_to() after popup, which
	// triggers a GDK "compositor doesn't support moving popups" warning
	// on compositors without xdg_popup.reposition (e.g. Weston).
	preedit_label_width = w;
	preedit_popover_height = h;

	// Report an enlarged cursor rectangle that covers both the cursor
	// cell and the preedit popover.  This way the IM (e.g. fcitx5)
	// places its candidate window below the popover instead of on top
	// of it.  Coordinates are in client-widget (drawarea) space.
	ଯଦି (xic != ଖାଲି)
	{
	    GdkRectangle area;

	    area.x = FILL_X(gui.col);
	    area.y = FILL_Y(gui.row);
	    area.width = preedit_label_width;
	    area.height = MAX(gui.char_height, preedit_popover_height);
	    gtk_im_context_set_cursor_location(xic, &area);
	}

	// Pop down before re-anchoring: on compositors that lack
	// xdg_popup.reposition (e.g. Weston), calling set_pointing_to() on
	// a mapped popover triggers GDK's remap fallback and a warning.
	// Doing the popdown ourselves makes the next popup land at the new
	// anchor cleanly without that warning.
	ଯଦି (gtk_widget_get_mapped(preedit_window))
	    gtk_popover_popdown(GTK_POPOVER(preedit_window));

	im_preedit_window_set_position();

	gtk_widget_queue_resize(preedit_window);
	gtk_popover_popup(GTK_POPOVER(preedit_window));
#  ଅନ୍ୟଥା
	gtk_window_resize(GTK_WINDOW(preedit_window), w, h);
	gtk_widget_show_all(preedit_window);

	im_preedit_window_set_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    ସ୍ଥିର ଶୂନ୍ୟ
im_preedit_window_close(ଶୂନ୍ୟ)
{
    ଯଦି (preedit_window == ଖାଲି)
	ଫେରନ୍ତୁ;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_window));
#  ଅନ୍ୟଥା
    gtk_widget_hide(preedit_window);
#  endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
im_show_preedit(ଶୂନ୍ୟ)
{
    im_preedit_window_open();

    ଯଦି (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
im_delete_preedit(ଶୂନ୍ୟ)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    ଯଦି (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_window_close();
	ଫେରନ୍ତୁ;
    }

    ଯଦି (State & MODE_NORMAL
#  ifdef FEAT_TERMINAL
	    && !term_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	ଫେରନ୍ତୁ;
    }
    ପାଇଁ (; im_preedit_cursor > 0; --im_preedit_cursor)
	add_to_ନିବେଶ_buf(bskey, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(bskey));

    ପାଇଁ (; im_preedit_trailing > 0; --im_preedit_trailing)
	add_to_ନିବେଶ_buf(delkey, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
im_correct_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_move_back)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    ଯଦି (State & MODE_NORMAL)
	ଫେରନ୍ତୁ;
#  ifdef FEAT_RIGHTLEFT
    ଯଦି ((State & MODE_CMDLINE) == 0 && curwin != ଖାଲି && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    ପାଇଁ (; ଅଙ୍କ_move_back > 0; --ଅଙ୍କ_move_back)
	add_to_ନିବେଶ_buf(backkey, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(backkey));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xim_expected_char = NUL;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
im_show_info(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    ଯଦି ((State & MODE_NORMAL) || (State & MODE_ଭର୍ତ୍ତି))
	setcursor();
    out_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
im_commit_cb(GtkIMContext *context UNUSED,
	     ଅଚଳ gchar *str,
	     gpointer data UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		slen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_to_ନିବେଶ = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		clen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = slen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    ଯଦି (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	ଯଦି (preedit_start_col == MAXCOL)
	{
	    init_preedit_start_col();
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
	ଯଦି (ନିବେଶ_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&ନିବେଶ_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != ଖାଲି);
	}
	ଅନ୍ୟଥା
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	ଯଦି (ନିବେଶ_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_start_col += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    ଯଦି (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	ଯଦି (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    add_to_ନିବେଶ = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    ଯଦି (add_to_ନିବେଶ)
	im_add_to_ନିବେଶ((char_u *)str, slen);

    ଯଦି (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	ଯଦି (!commit_with_preedit)
	    preedit_start_col = MAXCOL;

	// This flag is used in changed() at next call.
	xim_changed_while_preediting = TRUE;
    }

    ଯଦି (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
im_preedit_start_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_start_cb()\n");
#  endif

    im_is_active = TRUE;
    preedit_is_active = TRUE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after end to the preedit.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
im_preedit_end_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    ଯଦି (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;

#  ଯଦି 0
    // Removal of this line suggested by Takuhiro Nishioka.  Fixes that IM was
    // switched off unintentionally.  We now use preedit_is_active (added by
    // SungHyun Nam).
    im_is_active = FALSE;
#  endif
    preedit_is_active = FALSE;
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
    ସ୍ଥିର ଶୂନ୍ୟ
im_preedit_changed_cb(GtkIMContext *context, gpointer data UNUSED)
{
    ଅକ୍ଷର    *preedit_string = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cursor_index    = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_move_back   = 0;
    char_u  *str;
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଯଦି (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ଖାଲି,
					  &cursor_index);
    ଅନ୍ୟଥା
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, ଖାଲି,
					  ଖାଲି);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != ଖାଲି); // just in case

    ଯଦି (p_imst == IM_OVER_THE_SPOT)
    {
	ଯଦି (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	ଅନ୍ୟଥା
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    ଅନ୍ୟଥା
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	ଯଦି (preedit_start_col == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_start_col();
	}
	ଅନ୍ୟଥା ଯଦି (cursor_index == 0 && preedit_string[0] == '\0')
	{
	    xim_has_preediting = FALSE;

	    // If at the start position (after typing backspace)
	    // preedit_start_col must be reset.
	    preedit_start_col = MAXCOL;
	}

	im_delete_preedit();

	// Compute the end of the preediting area: "preedit_end_col".
	// According to the documentation of
	// gtk_im_context_get_preedit_string(), the cursor_pos output argument
	// returns the offset in bytes.  This is unfortunately not true -- real
	// life shows the offset is in characters, and the GTK+ source code
	// agrees with me.  Will file a bug later.
	ଯଦି (preedit_start_col != MAXCOL)
	    preedit_end_col = preedit_start_col;
	str = (char_u *)preedit_string;
	ପାଇଁ (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    ଯଦି (!is_composing || p_deco)
	    {
		ଯଦି (i < cursor_index)
		    ++im_preedit_cursor;
		ଅନ୍ୟଥା
		    ++im_preedit_trailing;
	    }
	    ଯଦି (!is_composing && i >= cursor_index)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++ଅଙ୍କ_move_back;
	    }
	    ଯଦି (preedit_start_col != MAXCOL)
		preedit_end_col += utf_ptr2cells(p);
	}

	ଯଦି (p > str)
	{
	    im_add_to_ନିବେଶ(str, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - str));
	    im_correct_cursor(ଅଙ୍କ_move_back);
	}
    }

    g_free(preedit_string);

    ଯଦି (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
translate_pango_attributes(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    char_attr = HL_NORMAL;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    ଯଦି (attr != ଖାଲି && ((PangoAttrInt *)attr)->ମୂଲ୍ୟ
						 != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_WEIGHT);
    ଯଦି (attr != ଖାଲି && ((PangoAttrInt *)attr)->ମୂଲ୍ୟ >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PANGO_WEIGHT_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    ଯଦି (attr != ଖାଲି && ((PangoAttrInt *)attr)->ମୂଲ୍ୟ
						   != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PANGO_STYLE_NORMAL)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    ଯଦି (attr != ଖାଲି)
    {
	ଅଚଳ PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	ଯଦି ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    ଫେରନ୍ତୁ char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_get_feedback_attr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଅକ୍ଷର	    *preedit_string = ଖାଲି;
    PangoAttrList   *attr_list	    = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    char_attr	    = -1;

    ଯଦି (xic == ଖାଲି)
	ଫେରନ୍ତୁ char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, ଖାଲି);

    ଯଦି (preedit_string != ଖାଲି && attr_list != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

	// Get the byte index as used by PangoAttrIterator
	ପାଇଁ (idx = 0; col > 0 && preedit_string[idx] != '\0'; --col)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	ଯଦି (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			start, end;

	    char_attr = HL_NORMAL;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    କର
	    {
		pango_attr_iterator_range(iter, &start, &end);

		ଯଦି (idx >= start && idx < end)
		    char_attr |= translate_pango_attributes(iter);
	    }
	    ଯେତେବେଳେ (pango_attr_iterator_next(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    ଯଦି (attr_list != ଖାଲି)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    ଫେରନ୍ତୁ char_attr;
}

    ଶୂନ୍ୟ
xim_init(ଶୂନ୍ୟ)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != ଖାଲି);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_window(gui.drawarea) != ଖାଲି);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), ଖାଲି);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_changed_cb), ଖାଲି);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_start_cb), ଖାଲି);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_end_cb), ଖାଲି);

#  ifdef USE_GTK4
    gtk_im_context_set_client_widget(xic, gui.drawarea);

    // Seed the cursor location immediately.  GTK4's Wayland IM module
    // batches text-input-v3 state and sends it on the next commit cycle
    // (typically triggered by focus_in).  Without this seed the very
    // first commit would carry the protocol default (0, 0, 0, 0) for the
    // cursor rectangle, and the IM's candidate window for the very first
    // composition would appear at the top-left of the surface instead of
    // next to the cursor.
    im_set_position(gui.row, gui.col);
#  ଅନ୍ୟଥା
    gtk_im_context_set_client_window(xic, gtk_widget_get_window(gui.drawarea));
#  endif
}

    ଶୂନ୍ୟ
im_shutdown(ଶୂନ୍ୟ)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    ଯଦି (xic != ଖାଲି)
    {
	gtk_im_context_focus_out(xic);
	g_object_unref(xic);
	xic = ଖାଲି;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    ଯଦି (preedit_window != ଖାଲି)
    {
	gtk_widget_unparent(preedit_window);
	preedit_window = ଖାଲି;
	preedit_label = ଖାଲି;
    }
    ଯଦି (preedit_css_provider != ଖାଲି)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = ଖାଲି;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_active = FALSE;
    im_commit_handler_id = 0;
    ଯଦି (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
/*
 * Convert the string argument to keyval and state for GdkEventKey.
 * If str is valid return TRUE, otherwise FALSE.
 *
 * See 'imactivatekey' for documentation of the format.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_string_to_keyval(ଅଚଳ ଅକ୍ଷର *str, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *keyval, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *state)
{
    ଅଚଳ ଅକ୍ଷର	    *mods_end;
    unsigned	    tmp_keyval;
    unsigned	    tmp_state = 0;

    mods_end = strrchr(str, '-');
    mods_end = (mods_end != ଖାଲି) ? mods_end + 1 : str;

    // Parse modifier keys
    ଯେତେବେଳେ (str < mods_end)
	ବାଛ (*str++)
	{
	    ସ୍ଥିତି '-':							ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'S': ସ୍ଥିତି 's': tmp_state |= (unsigned)GDK_SHIFT_MASK;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'L': ସ୍ଥିତି 'l': tmp_state |= (unsigned)GDK_LOCK_MASK;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'C': ସ୍ଥିତି 'c': tmp_state |= (unsigned)GDK_CONTROL_MASK;ଭାଙ୍ଗ;
	    ସ୍ଥିତି '1':		tmp_state |= (unsigned)GDK_MOD1_MASK;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି '2':		tmp_state |= (unsigned)GDK_MOD2_MASK;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି '3':		tmp_state |= (unsigned)GDK_MOD3_MASK;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି '4':		tmp_state |= (unsigned)GDK_MOD4_MASK;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି '5':		tmp_state |= (unsigned)GDK_MOD5_MASK;	ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    ଯଦି (tmp_keyval == 0 || tmp_keyval == GDK_Void_ପ୍ରତୀକ)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (keyval != ଖାଲି)
	*keyval = tmp_keyval;
    ଯଦି (state != ଖାଲି)
	*state = tmp_state;

    ଫେରନ୍ତୁ TRUE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_xim_isvalid_imactivate(ଶୂନ୍ୟ)
{
    ଯଦି (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_ପ୍ରତୀକ;
	im_activatekey_state  = 0;
	ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ im_string_to_keyval((ଅଚଳ ଅକ୍ଷର *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    ସ୍ଥିର ଶୂନ୍ୟ
im_synthesize_keypress(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା keyval, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା state)
{
    Gdk_Event_ଚାବି *event;

    event = (Gdk_Event_ଚାବି *)gdk_event_new(GDK_ଚାବି_PRESS);
    g_object_ref(gtk_widget_get_window(gui.drawarea));
					// unreffed by gdk_event_free()
    event->window = gtk_widget_get_window(gui.drawarea);
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysymToKeycode(GDK_WINDOW_XDISPLAY(event->window), (ଚାବି_Sym)keyval);
    event->length = 0;
    event->string = ଖାଲି;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_ଚାବି_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    ଶୂନ୍ୟ
xim_reset(ଶୂନ୍ୟ)
{
#  ifdef FEAT_EVAL
    ଯଦି (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_active);
    ଅନ୍ୟଥା
#  endif
    ଯଦି (xic != ଖାଲି)
    {
	gtk_im_context_reset(xic);

	ଯଦି (p_imdisable)
	    im_shutdown();
	ଅନ୍ୟଥା
	{
	    xim_set_focus(gui.in_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_set_focus(gui.in_focus);
#  ଅନ୍ୟଥା
	    ଯଦି (im_activatekey_keyval != GDK_Void_ପ୍ରତୀକ)
	    {
		ଯଦି (im_is_active)
		{
		    g_signal_handler_block(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		im_shutdown();
		xim_init();
		xim_set_focus(gui.in_focus);
	    }
#  endif
	}
    }

    ଯଦି (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
xim_queue_ଚାବି_press_event(Gdk_Event_ଚାବି *event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା down)
{
#   ifdef FEAT_GUI_GTK
    ଯଦି (event->state & GDK_SUPER_MASK) ଫେରନ୍ତୁ FALSE;
#   endif
    ଯଦି (down)
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
	ବାଛ (event->keyval)
	{
	    ସ୍ଥିତି GDK_KP_Add:      xim_expected_char = '+';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_Subtract: xim_expected_char = '-';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_Divide:   xim_expected_char = '/';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_Multiply: xim_expected_char = '*';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_Decimal:  xim_expected_char = '.';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_Equal:    xim_expected_char = '=';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_0:	  xim_expected_char = '0';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_1:	  xim_expected_char = '1';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_2:	  xim_expected_char = '2';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_3:	  xim_expected_char = '3';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_4:	  xim_expected_char = '4';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_5:	  xim_expected_char = '5';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_6:	  xim_expected_char = '6';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_7:	  xim_expected_char = '7';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_8:	  xim_expected_char = '8';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_KP_9:	  xim_expected_char = '9';  ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_space:	  xim_expected_char = ' ';  ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    ଯଦି (xic != ଖାଲି && !p_imdisable
		    && (State & (MODE_ଭର୍ତ୍ତି | MODE_CMDLINE
					      | MODE_NORMAL | MODE_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	ଯଦି (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା state_mask;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_mask  = im_activatekey_state;
	    state_mask |= ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_SHIFT_MASK | (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_CONTROL_MASK
							| (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_MOD1_MASK);

	    ଯଦି ((event->state & state_mask) != im_activatekey_state)
		ଫେରନ୍ତୁ FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    ଯଦି (event->type != GDK_ଚାବି_PRESS)
		ଫେରନ୍ତୁ TRUE;

	    ଯଦି (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
	    {
		im_set_active(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= MODE_LANGMAP;
		ଯଦି (State & MODE_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~MODE_LANGMAP;
		}
		ଅନ୍ୟଥା
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= MODE_LANGMAP;
		}
		ଫେରନ୍ତୁ TRUE;
	    }

	    ଫେରନ୍ତୁ gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	ଯଦି (im_activatekey_keyval == GDK_Void_ପ୍ରତୀକ || im_is_active)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା imresult = gtk_im_context_filter_keypress(xic, event);

	    ଯଦି (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		ଯଦି (!imresult && preedit_start_col != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    ଫେରନ୍ତୁ FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    ଯଦି (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		ଫେରନ୍ତୁ FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    ଫେରନ୍ତୁ imresult;
	}
    }

    ଫେରନ୍ତୁ FALSE;
}
#  ଅନ୍ୟଥା // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_xim_isvalid_imactivate(ଶୂନ୍ୟ)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    ଫେରନ୍ତୁ p_imak[0] == NUL;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
xim_queue_ଚାବି_press_event(GdkEvent *event UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା down UNUSED)
{
    ଫେରନ୍ତୁ FALSE;
}
#  endif // !USE_GTK4

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_get_status(ଶୂନ୍ୟ)
{
#  ifdef FEAT_EVAL
    ଯଦି (USE_IMSTATUSFUNC)
	ଫେରନ୍ତୁ call_imstatusfunc();
#  endif
    ଫେରନ୍ତୁ im_is_active;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
preedit_get_status(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ preedit_is_active;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_is_preediting(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ xim_has_preediting;
}

# ଅନ୍ୟଥା // !FEAT_GUI_GTK

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	xim_is_active = FALSE;  // XIM should be active in the current
					// mode
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
ସ୍ଥିର XIMStyle	ନିବେଶ_style;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	status_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    ଶୂନ୍ୟ
im_set_active(ପୂର୍ଣ୍ଣସଂଖ୍ୟା active_arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା active = active_arg;

    // If 'imdisable' is set, XIM is never active.
    ଯଦି (p_imdisable)
	active = FALSE;
    ଅନ୍ୟଥା ଯଦି (ନିବେଶ_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	active = TRUE;

#  ଯଦି defined(FEAT_EVAL)
    ଯଦି (USE_IMACTIVATEFUNC)
    {
	ଯଦି (active != im_get_status())
	{
	    call_imactivatefunc(active);
	    xim_has_focus = active;
	}
	ଫେରନ୍ତୁ;
    }
#  endif

    ଯଦି (xic == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_active = active;
    xim_set_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    ଶୂନ୍ୟ
xim_set_focus(ପୂର୍ଣ୍ଣସଂଖ୍ୟା focus)
{
    ଯଦି (xic == ଖାଲି)
	ଫେରନ୍ତୁ;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    ଯଦି (focus && xim_is_active)
    {
	ଯଦି (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    ଶୂନ୍ୟ
im_set_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col UNUSED)
{
    xim_set_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    ଶୂନ୍ୟ
xim_set_preedit(ଶୂନ୍ୟ)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_space;

    ଯଦି (xic == ଖାଲି)
	ଫେରନ୍ତୁ;

    xim_set_focus(TRUE);

    ଯଦି (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							ଖାଲି);
	XSetICValues(xic, XNPreeditAttributes, attr_list, ଖାଲି);
	XFree(attr_list);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ନିବେଶ_style & XIMPreeditPosition)
    {
	ଯଦି (xim_fg_color == INVALCOLOR)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_back_pixel;
	}
	over_spot.x = TEXT_X(gui.col);
	over_spot.y = TEXT_Y(gui.row);
	spot_area.x = 0;
	spot_area.y = 0;
	spot_area.height = gui.char_height * Rows;
	spot_area.width  = gui.char_width * Columns;
	ଧାଡ଼ି_space = gui.char_height;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, ଧାଡ଼ି_space,
					ଖାଲି);
	ଯଦି (XSetICValues(xic, XNPreeditAttributes, attr_list, ଖାଲି))
	    emsg(_(e_cannot_set_ic_ମୂଲ୍ୟଗୁଡ଼ିକ));
	XFree(attr_list);
    }
}

#  ଯଦି defined(FEAT_GUI_X11)
#   ଯଦି defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xim_real_init(Window x11_window, Display *x11_display);


#   ifdef USE_X11R6_XIM
    ସ୍ଥିର ଶୂନ୍ୟ
xim_instantiate_cb(
    Display	*display,
    XPointer	client_data UNUSED,
    XPointer	call_data UNUSED)
{
    Window	x11_window;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_window, &x11_display);
    ଯଦି (display != x11_display)
	ଫେରନ୍ତୁ;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    ଯଦି (xic != ଖାଲି)
	XUnregisterIMInstantiateCallback(x11_display, ଖାଲି, ଖାଲି, ଖାଲି,
					 xim_instantiate_cb, ଖାଲି);
}

    ସ୍ଥିର ଶୂନ୍ୟ
xim_destroy_cb(
    XIM		im UNUSED,
    XPointer	client_data UNUSED,
    XPointer	call_data UNUSED)
{
    Window	x11_window;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_window, &x11_display);

    xic = ଖାଲି;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, ଖାଲି, ଖାଲି, ଖାଲି,
				   xim_instantiate_cb, ଖାଲି);
}
#   endif

    ଶୂନ୍ୟ
xim_init(ଶୂନ୍ୟ)
{
    Window	x11_window;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = ଖାଲି;

    ଯଦି (xim_real_init(x11_window, x11_display))
	ଫେରନ୍ତୁ;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, ଖାଲି, ଖାଲି, ଖାଲି,
				   xim_instantiate_cb, ଖାଲି);
#   endif
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
xim_real_init(Window x11_window, Display *x11_display)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ଅକ୍ଷର	*p,
		*s,
		*ns,
		*end,
		tmp[1024];
#   define IMLEN_MAX 40
    ଅକ୍ଷର	buf[IMLEN_MAX + 7];
    XIM		xim = ଖାଲି;
    XIMStyles	*xim_styles;
    XIMStyle	this_ନିବେଶ_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    ନିବେଶ_style = 0;
    status_area_enabled = FALSE;

    ଯଦି (xic != ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (gui.rsrc_ନିବେଶ_method != ଖାଲି && *gui.rsrc_ନିବେଶ_method != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_ନିବେଶ_method,
	    ଆକାର(tmp) - 1);
	ପାଇଁ (ns = s = tmp; ns != ଖାଲି && *s != NUL;)
	{
	    s = (ଅକ୍ଷର *)skipwhite((char_u *)s);
	    ଯଦି (*s == NUL)
		ଭାଙ୍ଗ;
	    ଯଦି ((ns = end = strchr(s, ',')) == ଖାଲି)
		end = s + strlen(s);
	    ଯେତେବେଳେ (SAFE_isspace(end[-1]))
		end--;
	    *end = NUL;

	    ଯଦି (strlen(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		ଯଦି ((p = XSetLocaleModifiers(buf)) != ଖାଲି && *p != NUL
			&& (xim = XOpenIM(x11_display, ଖାଲି, ଖାଲି, ଖାଲି))
								      != ଖାଲି)
		    ଭାଙ୍ଗ;
	    }

	    s = ns + 1;
	}
    }

    ଯଦି (xim == ଖାଲି && (p = XSetLocaleModifiers("")) != ଖାଲି && *p != NUL)
	xim = XOpenIM(x11_display, ଖାଲି, ଖାଲି, ଖାଲି);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    ଯଦି (xim == ଖାଲି && (p = XSetLocaleModifiers("@im=none")) != ଖାଲି
								 && *p != NUL)
	xim = XOpenIM(x11_display, ଖାଲି, ଖାଲି, ଖାଲି);

    ଯଦି (xim == ଖାଲି)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_to_open_ନିବେଶ_method));
	    verbose_leave();
	}
	ଫେରନ୍ତୁ FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_data = ଖାଲି;
	ଯଦି (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, ଖାଲି))
	    emsg(_(e_warning_could_not_set_destroy_callback_to_im));
    }
#   endif

    ଯଦି (XGetIMValues(xim, XNQuery_ନିବେଶ_Style, &xim_styles, ଖାଲି) || !xim_styles)
    {
	emsg(_(e_ନିବେଶ_method_doesnt_support_any_style));
	XCloseIM(xim);
	ଫେରନ୍ତୁ FALSE;
    }

    found = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    ଆକାର(tmp) - 1);
    ପାଇଁ (s = tmp; s && !found; )
    {
	ଯେତେବେଳେ (*s && SAFE_isspace(*s))
	    s++;
	ଯଦି (!*s)
	    ଭାଙ୍ଗ;
	ଯଦି ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	ଅନ୍ୟଥା
	    end = s + strlen(s);
	ଯେତେବେଳେ (SAFE_isspace(*end))
	    end--;
	*end = '\0';

	ଯଦି (!strcmp(s, "OverTheSpot"))
	    this_ନିବେଶ_style = (XIMPreeditPosition | XIMStatusArea);
	ଅନ୍ୟଥା ଯଦି (!strcmp(s, "OffTheSpot"))
	    this_ନିବେଶ_style = (XIMPreeditArea | XIMStatusArea);
	ଅନ୍ୟଥା ଯଦି (!strcmp(s, "Root"))
	    this_ନିବେଶ_style = (XIMPreeditNothing | XIMStatusNothing);

	ପାଇଁ (i = 0; (unsigned short)i < xim_styles->ଗଣନା_styles; i++)
	{
	    ଯଦି (this_ନିବେଶ_style == xim_styles->supported_styles[i])
	    {
		found = True;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (!found)
	    ପାଇଁ (i = 0; (unsigned short)i < xim_styles->ଗଣନା_styles; i++)
	    {
		ଯଦି ((xim_styles->supported_styles[i] & this_ନିବେଶ_style)
			== (this_ନିବେଶ_style & ~XIMStatusArea))
		{
		    this_ନିବେଶ_style &= ~XIMStatusArea;
		    found = True;
		    ଭାଙ୍ଗ;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    ଯଦି (!found)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_ନିବେଶ_method_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	ଫେରନ୍ତୁ FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    ନିବେଶ_style = this_ନିବେଶ_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    ଯଦି (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				ଖାଲି);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				ଖାଲି);
    }
    ଅନ୍ୟଥା
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				ଖାଲି);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				ଖାଲି);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, ନିବେଶ_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    ଖାଲି);
    XFree(status_list);
    XFree(preedit_list);
    ଯଦି (xic != ଖାଲି)
    {
	ଯଦି (ନିବେଶ_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	ଅନ୍ୟଥା
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (!is_not_a_term())
	    emsg(_(e_failed_to_create_ନିବେଶ_context));
	XCloseIM(xim);
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_get_status(ଶୂନ୍ୟ)
{
#  ifdef FEAT_EVAL
    ଯଦି (USE_IMSTATUSFUNC)
	ଫେରନ୍ତୁ call_imstatusfunc();
#  endif
    ଫେରନ୍ତୁ xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# ଯଦି !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    ଶୂନ୍ୟ
xim_set_status_area(ଶୂନ୍ୟ)
{
    XVaNestedList preedit_list = 0, status_list = 0, list = 0;
    XRectangle pre_area, status_area;

    ଯଦି (xic == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (ନିବେଶ_style & XIMStatusArea)
    {
	ଯଦି (ନିବେଶ_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    status_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, ଖାଲି);
	    XGetICValues(xic, XNStatusAttributes, status_list, ଖାଲି);
	    XFree(status_list);

	    status_area.width = needed_rect->width;
	}
	ଅନ୍ୟଥା
	    status_area.width = gui.char_width * Columns;

	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	ଯଦି (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	ଯଦି (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.height = gui.char_height;
	status_list = XVaCreateNestedList(0, XNArea, &status_area, ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	ଯଦି (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	ଯଦି (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.width = 0;
	status_area.height = gui.char_height;
    }

    ଯଦି (ନିବେଶ_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = status_area.x + status_area.width;
	pre_area.y = gui.char_height * Rows + gui.border_offset;
	pre_area.width = gui.char_width * Columns - pre_area.x;
	ଯଦି (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	ଯଦି (gui.menu_is_active)
	    pre_area.y += gui.menu_height;
#  endif
	pre_area.height = gui.char_height;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି (ନିବେଶ_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.height = gui.char_height * Rows;
	pre_area.width = gui.char_width * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, ଖାଲି);
    }

    ଯଦି (preedit_list && status_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   XNStatusAttributes, status_list, ଖାଲି);
    ଅନ୍ୟଥା ଯଦି (preedit_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   ଖାଲି);
    ଅନ୍ୟଥା ଯଦି (status_list)
	list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				   ଖାଲି);
    ଅନ୍ୟଥା
	list = ଖାଲି;

    ଯଦି (list)
    {
	XSetICValues(xic, XNVaNestedList, list, ଖାଲି);
	XFree(list);
    }
    ଯଦି (status_list)
	XFree(status_list);
    ଯଦି (preedit_list)
	XFree(preedit_list);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
xim_get_status_area_height(ଶୂନ୍ୟ)
{
    ଯଦି (status_area_enabled)
	ଫେରନ୍ତୁ gui.char_height;
    ଫେରନ୍ତୁ 0;
}
# endif

#ଅନ୍ୟଥା // !defined(FEAT_XIM)

# ଯଦି defined(IME_WITHOUT_XIM) || defined(VIMDLL)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା im_was_set_active = FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
#  ifdef VIMDLL
mbyte_im_get_status(ଶୂନ୍ୟ)
#  ଅନ୍ୟଥା
im_get_status(ଶୂନ୍ୟ)
#  endif
{
#  ଯଦି defined(FEAT_EVAL)
    ଯଦି (USE_IMSTATUSFUNC)
	ଫେରନ୍ତୁ call_imstatusfunc();
#  endif
    ଫେରନ୍ତୁ im_was_set_active;
}

    ଶୂନ୍ୟ
#  ifdef VIMDLL
mbyte_im_set_active(ପୂର୍ଣ୍ଣସଂଖ୍ୟା active_arg)
#  ଅନ୍ୟଥା
im_set_active(ପୂର୍ଣ୍ଣସଂଖ୍ୟା active_arg)
#  endif
{
#  ଯଦି defined(FEAT_EVAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    active = !p_imdisable && active_arg;

    ଯଦି (USE_IMACTIVATEFUNC && active != im_get_status())
    {
	call_imactivatefunc(active);
	im_was_set_active = active;
    }
#  endif
}

#  ଯଦି defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    ଶୂନ୍ୟ
im_set_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col UNUSED)
{
}
#  endif
# endif

#endif // FEAT_XIM
