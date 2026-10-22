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

#அந்தர்பாவ "vim.h"

#எனில் !defined(GTK_CHECK_VERSION)
# define GTK_CHECK_VERSION(a, b, c) 0
#endif

#எனில் defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# இல்லையெனில்
#  include <gdk/gdkkeysyms.h>
# endif
# எனில் !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  இல்லையெனில்
#   include <gdk/gdkx.h>
#  endif
# endif
#endif

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#எனில் defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
நிலையான வெறுமை xim_log(எழுத்து *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    நிலையான வெறுமை
xim_log(எழுத்து *s, ...)
{
    va_list arglist;
    நிலையான FILE *fd = வெற்று;

    எனில் (fd == (FILE *)-1)
	திரும்பு;
    எனில் (fd == வெற்று)
    {
	fd = mch_fopen("xim.log", "w");
	எனில் (fd == வெற்று)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    திரும்பு;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#endif

#எனில் defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.in_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.in_use && *p_imsf != NUL)
#இல்லையெனில்
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#endif

#எனில் defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
நிலையான callback_T imaf_cb;	    // 'imactivatefunc' callback function
நிலையான callback_T imsf_cb;	    // 'imstatusfunc' callback function

    எழுத்து *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    எனில் (option_set_callback_func(p_imaf, &imaf_cb) == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    எழுத்து *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    எனில் (option_set_callback_func(p_imsf, &imsf_cb) == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    நிலையான வெறுமை
call_imactivatefunc(முழு active)
{
    typval_T argv[2];
    முழு save_சாவி_Typed = சாவி_Typed;

    argv[0].v_type = VAR_எண்;
    argv[0].vval.v_எண் = active ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (வெறுமை)call_callback_retnr(&imaf_cb, 1, argv);

    சாவி_Typed = save_சாவி_Typed;
}

    நிலையான முழு
call_imstatusfunc(வெறுமை)
{
    முழு is_active;
    முழு save_சாவி_Typed = சாவி_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    எனில் (exiting || is_autocmd_blocked())
	திரும்பு FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++செய்தி_silent;
    is_active = call_callback_retnr(&imsf_cb, 0, வெற்று);
    --செய்தி_silent;

    சாவி_Typed = save_சாவி_Typed;
    திரும்பு (is_active > 0);
}
#endif

#எனில் defined(EXITFREE)
    வெறுமை
free_xim_stuff(வெறுமை)
{
# எனில் defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    முழு
set_ref_in_im_funcs(முழு copyID UNUSED)
{
    முழு abort = FALSE;

# எனில் defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = set_ref_in_callback(&imaf_cb, copyID);
    abort = abort || set_ref_in_callback(&imsf_cb, copyID);
# endif

    திரும்பு abort;
}
#endif


#எனில் defined(FEAT_XIM)

# எனில் defined(FEAT_GUI_GTK)
நிலையான முழு xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    நிலையான வெறுமை
init_preedit_start_col(வெறுமை)
{
    எனில் (State & MODE_CMDLINE)
	preedit_start_col = cmdline_getvcol_cursor();
    இல்லையெனில் எனில் (curwin != வெற்று && curwin->w_buffer != வெற்று)
	getvcol(curwin, &curwin->w_cursor, &preedit_start_col, வெற்று, வெற்று, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_changed_while_preediting = curbuf->b_changed;
}

நிலையான முழு im_is_active	       = FALSE;	// IM is enabled for current mode
நிலையான முழு preedit_is_active   = FALSE;
நிலையான முழு im_preedit_cursor   = 0;	// cursor offset in characters
நிலையான முழு im_preedit_trailing = 0;	// number of characters after cursor

நிலையான unsigned long im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_சாவி_Void_குறியீடு
#    define GDK_சாவி_Void_குறியீடு GDK_Void_குறியீடு
#   endif
நிலையான unsigned முழு  im_activatekey_keyval = GDK_சாவி_Void_குறியீடு;
நிலையான unsigned முழு  im_activatekey_state  = 0;
#  endif

நிலையான GtkWidget *preedit_window = வெற்று;
நிலையான GtkWidget *preedit_label = வெற்று;
#  ifdef USE_GTK4
நிலையான முழு preedit_label_width = 0;	// last measured popover natural width
நிலையான முழு preedit_popover_height = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
நிலையான GtkCssProvider *preedit_css_provider = வெற்று;
நிலையான guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

நிலையான வெறுமை im_preedit_window_set_position(வெறுமை);

    வெறுமை
im_set_active(முழு active)
{
    முழு was_active;

    was_active = !!im_get_status();
    im_is_active = (active && !p_imdisable);

    எனில் (im_is_active != was_active)
	xim_reset();
}

    வெறுமை
xim_set_focus(முழு focus)
{
    எனில் (xic == வெற்று)
	திரும்பு;

    எனில் (focus)
	gtk_im_context_focus_in(xic);
    இல்லையெனில்
	gtk_im_context_focus_out(xic);
}

    வெறுமை
im_set_position(முழு row, முழு col)
{
    எனில் (xic == வெற்று)
	திரும்பு;

    GdkRectangle area;

    area.x = FILL_X(col);
    area.y = FILL_Y(row);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.width  = gui.char_width
		    * ((Screen_வரிகள் != வெற்று && mb_lefthalve(row, col)) ? 2 : 1);
    area.height = gui.char_height;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    எனில் (p_imst == IM_OVER_THE_SPOT)
    {
	முழு popover_h = preedit_popover_height;
	எனில் (popover_h <= 0)
	    popover_h = gui.char_height;
	எனில் (popover_h > area.height)
	    area.height = popover_h;
    }
#  endif

    gtk_im_context_set_cursor_location(xic, &area);

    எனில் (p_imst == IM_OVER_THE_SPOT)
	im_preedit_window_set_position();
}

#  எனில் 0 || defined(PROTO) // apparently only used in gui_x11.c
    வெறுமை
xim_set_preedit(வெறுமை)
{
    im_set_position(gui.row, gui.col);
}
#  endif

    நிலையான வெறுமை
im_add_to_உள்ளீடு(char_u *str, முழு len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    எனில் (உள்ளீடு_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&உள்ளீடு_conv, str, &len);
	g_return_if_fail(str != வெற்று);
    }

    add_to_உள்ளீடு_buf_csi(str, len);

    எனில் (உள்ளீடு_conv.vc_type != CONV_NONE)
	vim_free(str);

    எனில் (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    நிலையான வெறுமை
im_preedit_window_set_position(வெறுமை)
{
    எனில் (preedit_window == வெற்று)
	திரும்பு;

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
    எனில் (!gtk_widget_compute_point(gui.drawarea, gui.mainwin,
						&pt_in, &pt_out))
    {
	pt_out.x = pt_in.x;
	pt_out.y = pt_in.y;
    }
    rect.x = (முழு)pt_out.x;
    rect.y = (முழு)pt_out.y;
    rect.width = preedit_label_width;
    rect.height = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    முழு offset = gui.char_height - preedit_popover_height;
    gtk_popover_set_offset(GTK_POPOVER(preedit_window), 0, offset);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_set_halign(preedit_window, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(preedit_window), &rect);
#  இல்லையெனில்
    முழு x, y, width, height;
    முழு screen_x, screen_y, screen_width, screen_height;

    gui_gtk_get_screen_geom_of_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_width, &screen_height);
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), &x, &y);
    gtk_window_get_size(GTK_WINDOW(preedit_window), &width, &height);
    x = x + FILL_X(gui.col);
    y = y + FILL_Y(gui.row);
    எனில் (x + width > screen_x + screen_width)
	x = screen_x + screen_width - width;
    எனில் (y + height > screen_y + screen_height)
	y = screen_y + screen_height - height;
    gtk_window_move(GTK_WINDOW(preedit_window), x, y);
#  endif
}

    நிலையான வெறுமை
im_preedit_window_open(வெறுமை)
{
    எழுத்து *preedit_string;
#  எனில் !GTK_CHECK_VERSION(3,16,0)
    எழுத்து buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  எனில் GTK_CHECK_VERSION(3,0,0)
#   எனில் !GTK_CHECK_VERSION(3,16,0)
    GdkRGBA color;
#   endif
#  இல்லையெனில்
    GdkColor color;
#  endif
    gint w, h;

    எனில் (preedit_window == வெற்று)
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
#  இல்லையெனில்
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
#  இல்லையெனில்
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
    எனில் (preedit_css_provider == வெற்று || preedit_css_cached_bg != gui.back_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	எனில் (preedit_css_provider != வெற்று)
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
#   எனில் GTK_CHECK_VERSION(3,16,0)
    {
	GtkStyleContext * மாறிலி context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * மாறிலி provider = gtk_css_provider_new();
	gchar		   *css = வெற்று;
	மாறிலி எழுத்து * மாறிலி fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_size(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = வெற்று;

	எனில் (!pango_font_description_get_size_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * மாறிலி screen
		  = gdk_window_get_screen(gtk_widget_get_window(gui.mainwin));
	    மாறிலி gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	எனில் (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	இல்லையெனில்
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != வெற்று ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.back_pixel >> 16) & 0xff,
		(gui.back_pixel >> 8) & 0xff,
		gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, வெற்று);
	gtk_style_context_add_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_VERSION(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, அளவு(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_NORMAL, &color);

    vim_snprintf(buf, அளவு(buf), "#%06X", gui.back_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_NORMAL,
								      &color);
#   இல்லையெனில்
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, அளவு(buf), "#%06X", (unsigned)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_NORMAL, &color);

    vim_snprintf(buf, அளவு(buf), "#%06X", (unsigned)gui.back_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_window, GTK_STATE_NORMAL, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, வெற்று);

    எனில் (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	எனில் (attr_list == வெற்று)
	    attr_list = pango_attr_list_new();

	எனில் (gui.norm_font != வெற்று)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->start_index = 0;
	    pa->end_index = G_MAXUINT;
	    pango_attr_list_செருகு(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_செருகு(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.back_pixel >> 16) & 0xff) * 257,
		((gui.back_pixel >> 8) & 0xff) * 257,
		(gui.back_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_செருகு(attr_list, pa);
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
	எனில் (xic != வெற்று)
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
	எனில் (gtk_widget_get_mapped(preedit_window))
	    gtk_popover_popdown(GTK_POPOVER(preedit_window));

	im_preedit_window_set_position();

	gtk_widget_queue_resize(preedit_window);
	gtk_popover_popup(GTK_POPOVER(preedit_window));
#  இல்லையெனில்
	gtk_window_resize(GTK_WINDOW(preedit_window), w, h);
	gtk_widget_show_all(preedit_window);

	im_preedit_window_set_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    நிலையான வெறுமை
im_preedit_window_close(வெறுமை)
{
    எனில் (preedit_window == வெற்று)
	திரும்பு;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_window));
#  இல்லையெனில்
    gtk_widget_hide(preedit_window);
#  endif
}

    நிலையான வெறுமை
im_show_preedit(வெறுமை)
{
    im_preedit_window_open();

    எனில் (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    நிலையான வெறுமை
im_delete_preedit(வெறுமை)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    எனில் (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_window_close();
	திரும்பு;
    }

    எனில் (State & MODE_NORMAL
#  ifdef FEAT_TERMINAL
	    && !term_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	திரும்பு;
    }
    க்கு (; im_preedit_cursor > 0; --im_preedit_cursor)
	add_to_உள்ளீடு_buf(bskey, (முழு)அளவு(bskey));

    க்கு (; im_preedit_trailing > 0; --im_preedit_trailing)
	add_to_உள்ளீடு_buf(delkey, (முழு)அளவு(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    நிலையான வெறுமை
im_correct_cursor(முழு இலக்கம்_move_back)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    எனில் (State & MODE_NORMAL)
	திரும்பு;
#  ifdef FEAT_RIGHTLEFT
    எனில் ((State & MODE_CMDLINE) == 0 && curwin != வெற்று && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    க்கு (; இலக்கம்_move_back > 0; --இலக்கம்_move_back)
	add_to_உள்ளீடு_buf(backkey, (முழு)அளவு(backkey));
}

நிலையான முழு xim_expected_char = NUL;
நிலையான முழு xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    நிலையான வெறுமை
im_show_info(வெறுமை)
{
    முழு	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    எனில் ((State & MODE_NORMAL) || (State & MODE_செருகு))
	setcursor();
    out_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    நிலையான வெறுமை
im_commit_cb(GtkIMContext *context UNUSED,
	     மாறிலி gchar *str,
	     gpointer data UNUSED)
{
    முழு		slen = (முழு)STRLEN(str);
    முழு		add_to_உள்ளீடு = TRUE;
    முழு		clen;
    முழு		len = slen;
    முழு		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    எனில் (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	எனில் (preedit_start_col == MAXCOL)
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
	எனில் (உள்ளீடு_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&உள்ளீடு_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != வெற்று);
	}
	இல்லையெனில்
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	எனில் (உள்ளீடு_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_start_col += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    எனில் (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	எனில் (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    add_to_உள்ளீடு = FALSE;
	}
	இல்லையெனில்
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    எனில் (add_to_உள்ளீடு)
	im_add_to_உள்ளீடு((char_u *)str, slen);

    எனில் (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	எனில் (!commit_with_preedit)
	    preedit_start_col = MAXCOL;

	// This flag is used in changed() at next call.
	xim_changed_while_preediting = TRUE;
    }

    எனில் (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    நிலையான வெறுமை
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
    நிலையான வெறுமை
im_preedit_end_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    எனில் (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;

#  எனில் 0
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
    நிலையான வெறுமை
im_preedit_changed_cb(GtkIMContext *context, gpointer data UNUSED)
{
    எழுத்து    *preedit_string = வெற்று;
    முழு	    cursor_index    = 0;
    முழு	    இலக்கம்_move_back   = 0;
    char_u  *str;
    char_u  *p;
    முழு	    i;

    எனில் (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, வெற்று,
					  &cursor_index);
    இல்லையெனில்
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, வெற்று,
					  வெற்று);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != வெற்று); // just in case

    எனில் (p_imst == IM_OVER_THE_SPOT)
    {
	எனில் (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	இல்லையெனில்
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    இல்லையெனில்
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	எனில் (preedit_start_col == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_start_col();
	}
	இல்லையெனில் எனில் (cursor_index == 0 && preedit_string[0] == '\0')
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
	எனில் (preedit_start_col != MAXCOL)
	    preedit_end_col = preedit_start_col;
	str = (char_u *)preedit_string;
	க்கு (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    முழு is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    எனில் (!is_composing || p_deco)
	    {
		எனில் (i < cursor_index)
		    ++im_preedit_cursor;
		இல்லையெனில்
		    ++im_preedit_trailing;
	    }
	    எனில் (!is_composing && i >= cursor_index)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++இலக்கம்_move_back;
	    }
	    எனில் (preedit_start_col != MAXCOL)
		preedit_end_col += utf_ptr2cells(p);
	}

	எனில் (p > str)
	{
	    im_add_to_உள்ளீடு(str, (முழு)(p - str));
	    im_correct_cursor(இலக்கம்_move_back);
	}
    }

    g_free(preedit_string);

    எனில் (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    நிலையான முழு
translate_pango_attributes(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    முழு		    char_attr = HL_NORMAL;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    எனில் (attr != வெற்று && ((PangoAttrInt *)attr)->மதிப்பு
						 != (முழு)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_WEIGHT);
    எனில் (attr != வெற்று && ((PangoAttrInt *)attr)->மதிப்பு >= (முழு)PANGO_WEIGHT_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    எனில் (attr != வெற்று && ((PangoAttrInt *)attr)->மதிப்பு
						   != (முழு)PANGO_STYLE_NORMAL)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    எனில் (attr != வெற்று)
    {
	மாறிலி PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	எனில் ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    திரும்பு char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    முழு
im_get_feedback_attr(முழு col)
{
    எழுத்து	    *preedit_string = வெற்று;
    PangoAttrList   *attr_list	    = வெற்று;
    முழு		    char_attr	    = -1;

    எனில் (xic == வெற்று)
	திரும்பு char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, வெற்று);

    எனில் (preedit_string != வெற்று && attr_list != வெற்று)
    {
	முழு idx;

	// Get the byte index as used by PangoAttrIterator
	க்கு (idx = 0; col > 0 && preedit_string[idx] != '\0'; --col)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	எனில் (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    முழு			start, end;

	    char_attr = HL_NORMAL;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    செய்
	    {
		pango_attr_iterator_range(iter, &start, &end);

		எனில் (idx >= start && idx < end)
		    char_attr |= translate_pango_attributes(iter);
	    }
	    போது (pango_attr_iterator_next(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    எனில் (attr_list != வெற்று)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    திரும்பு char_attr;
}

    வெறுமை
xim_init(வெறுமை)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != வெற்று);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_window(gui.drawarea) != வெற்று);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), வெற்று);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_changed_cb), வெற்று);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_start_cb), வெற்று);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_end_cb), வெற்று);

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
#  இல்லையெனில்
    gtk_im_context_set_client_window(xic, gtk_widget_get_window(gui.drawarea));
#  endif
}

    வெறுமை
im_shutdown(வெறுமை)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    எனில் (xic != வெற்று)
    {
	gtk_im_context_focus_out(xic);
	g_object_unref(xic);
	xic = வெற்று;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    எனில் (preedit_window != வெற்று)
    {
	gtk_widget_unparent(preedit_window);
	preedit_window = வெற்று;
	preedit_label = வெற்று;
    }
    எனில் (preedit_css_provider != வெற்று)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = வெற்று;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_active = FALSE;
    im_commit_handler_id = 0;
    எனில் (p_imst == IM_ON_THE_SPOT)
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
    நிலையான முழு
im_string_to_keyval(மாறிலி எழுத்து *str, unsigned முழு *keyval, unsigned முழு *state)
{
    மாறிலி எழுத்து	    *mods_end;
    unsigned	    tmp_keyval;
    unsigned	    tmp_state = 0;

    mods_end = strrchr(str, '-');
    mods_end = (mods_end != வெற்று) ? mods_end + 1 : str;

    // Parse modifier keys
    போது (str < mods_end)
	தேர்வு (*str++)
	{
	    நிலை '-':							நிறுத்து;
	    நிலை 'S': நிலை 's': tmp_state |= (unsigned)GDK_SHIFT_MASK;	நிறுத்து;
	    நிலை 'L': நிலை 'l': tmp_state |= (unsigned)GDK_LOCK_MASK;	நிறுத்து;
	    நிலை 'C': நிலை 'c': tmp_state |= (unsigned)GDK_CONTROL_MASK;நிறுத்து;
	    நிலை '1':		tmp_state |= (unsigned)GDK_MOD1_MASK;	நிறுத்து;
	    நிலை '2':		tmp_state |= (unsigned)GDK_MOD2_MASK;	நிறுத்து;
	    நிலை '3':		tmp_state |= (unsigned)GDK_MOD3_MASK;	நிறுத்து;
	    நிலை '4':		tmp_state |= (unsigned)GDK_MOD4_MASK;	நிறுத்து;
	    நிலை '5':		tmp_state |= (unsigned)GDK_MOD5_MASK;	நிறுத்து;
	    இயல்பு:
		திரும்பு FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    எனில் (tmp_keyval == 0 || tmp_keyval == GDK_Void_குறியீடு)
	திரும்பு FALSE;

    எனில் (keyval != வெற்று)
	*keyval = tmp_keyval;
    எனில் (state != வெற்று)
	*state = tmp_state;

    திரும்பு TRUE;
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
    முழு
im_xim_isvalid_imactivate(வெறுமை)
{
    எனில் (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_குறியீடு;
	im_activatekey_state  = 0;
	திரும்பு TRUE;
    }

    திரும்பு im_string_to_keyval((மாறிலி எழுத்து *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    நிலையான வெறுமை
im_synthesize_keypress(unsigned முழு keyval, unsigned முழு state)
{
    Gdk_Event_சாவி *event;

    event = (Gdk_Event_சாவி *)gdk_event_new(GDK_சாவி_PRESS);
    g_object_ref(gtk_widget_get_window(gui.drawarea));
					// unreffed by gdk_event_free()
    event->window = gtk_widget_get_window(gui.drawarea);
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysymToKeycode(GDK_WINDOW_XDISPLAY(event->window), (சாவி_Sym)keyval);
    event->length = 0;
    event->string = வெற்று;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_சாவி_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    வெறுமை
xim_reset(வெறுமை)
{
#  ifdef FEAT_EVAL
    எனில் (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_active);
    இல்லையெனில்
#  endif
    எனில் (xic != வெற்று)
    {
	gtk_im_context_reset(xic);

	எனில் (p_imdisable)
	    im_shutdown();
	இல்லையெனில்
	{
	    xim_set_focus(gui.in_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_set_focus(gui.in_focus);
#  இல்லையெனில்
	    எனில் (im_activatekey_keyval != GDK_Void_குறியீடு)
	    {
		எனில் (im_is_active)
		{
		    g_signal_handler_block(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    இல்லையெனில்
	    {
		im_shutdown();
		xim_init();
		xim_set_focus(gui.in_focus);
	    }
#  endif
	}
    }

    எனில் (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    முழு
xim_queue_சாவி_press_event(Gdk_Event_சாவி *event, முழு down)
{
#   ifdef FEAT_GUI_GTK
    எனில் (event->state & GDK_SUPER_MASK) திரும்பு FALSE;
#   endif
    எனில் (down)
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
	தேர்வு (event->keyval)
	{
	    நிலை GDK_KP_Add:      xim_expected_char = '+';  நிறுத்து;
	    நிலை GDK_KP_Subtract: xim_expected_char = '-';  நிறுத்து;
	    நிலை GDK_KP_Divide:   xim_expected_char = '/';  நிறுத்து;
	    நிலை GDK_KP_Multiply: xim_expected_char = '*';  நிறுத்து;
	    நிலை GDK_KP_Decimal:  xim_expected_char = '.';  நிறுத்து;
	    நிலை GDK_KP_Equal:    xim_expected_char = '=';  நிறுத்து;
	    நிலை GDK_KP_0:	  xim_expected_char = '0';  நிறுத்து;
	    நிலை GDK_KP_1:	  xim_expected_char = '1';  நிறுத்து;
	    நிலை GDK_KP_2:	  xim_expected_char = '2';  நிறுத்து;
	    நிலை GDK_KP_3:	  xim_expected_char = '3';  நிறுத்து;
	    நிலை GDK_KP_4:	  xim_expected_char = '4';  நிறுத்து;
	    நிலை GDK_KP_5:	  xim_expected_char = '5';  நிறுத்து;
	    நிலை GDK_KP_6:	  xim_expected_char = '6';  நிறுத்து;
	    நிலை GDK_KP_7:	  xim_expected_char = '7';  நிறுத்து;
	    நிலை GDK_KP_8:	  xim_expected_char = '8';  நிறுத்து;
	    நிலை GDK_KP_9:	  xim_expected_char = '9';  நிறுத்து;
	    நிலை GDK_space:	  xim_expected_char = ' ';  நிறுத்து;
	    இயல்பு:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    எனில் (xic != வெற்று && !p_imdisable
		    && (State & (MODE_செருகு | MODE_CMDLINE
					      | MODE_NORMAL | MODE_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	எனில் (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    unsigned முழு state_mask;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_mask  = im_activatekey_state;
	    state_mask |= ((முழு)GDK_SHIFT_MASK | (முழு)GDK_CONTROL_MASK
							| (முழு)GDK_MOD1_MASK);

	    எனில் ((event->state & state_mask) != im_activatekey_state)
		திரும்பு FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    எனில் (event->type != GDK_சாவி_PRESS)
		திரும்பு TRUE;

	    எனில் (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
	    {
		im_set_active(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= MODE_LANGMAP;
		எனில் (State & MODE_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~MODE_LANGMAP;
		}
		இல்லையெனில்
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= MODE_LANGMAP;
		}
		திரும்பு TRUE;
	    }

	    திரும்பு gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	எனில் (im_activatekey_keyval == GDK_Void_குறியீடு || im_is_active)
	{
	    முழு imresult = gtk_im_context_filter_keypress(xic, event);

	    எனில் (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		எனில் (!imresult && preedit_start_col != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    திரும்பு FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    எனில் (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		திரும்பு FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    திரும்பு imresult;
	}
    }

    திரும்பு FALSE;
}
#  இல்லையெனில் // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    முழு
im_xim_isvalid_imactivate(வெறுமை)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    திரும்பு p_imak[0] == NUL;
}

    முழு
xim_queue_சாவி_press_event(GdkEvent *event UNUSED, முழு down UNUSED)
{
    திரும்பு FALSE;
}
#  endif // !USE_GTK4

    முழு
im_get_status(வெறுமை)
{
#  ifdef FEAT_EVAL
    எனில் (USE_IMSTATUSFUNC)
	திரும்பு call_imstatusfunc();
#  endif
    திரும்பு im_is_active;
}

    முழு
preedit_get_status(வெறுமை)
{
    திரும்பு preedit_is_active;
}

    முழு
im_is_preediting(வெறுமை)
{
    திரும்பு xim_has_preediting;
}

# இல்லையெனில் // !FEAT_GUI_GTK

நிலையான முழு	xim_is_active = FALSE;  // XIM should be active in the current
					// mode
நிலையான முழு	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
நிலையான XIMStyle	உள்ளீடு_style;
நிலையான முழு	status_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    வெறுமை
im_set_active(முழு active_arg)
{
    முழு active = active_arg;

    // If 'imdisable' is set, XIM is never active.
    எனில் (p_imdisable)
	active = FALSE;
    இல்லையெனில் எனில் (உள்ளீடு_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	active = TRUE;

#  எனில் defined(FEAT_EVAL)
    எனில் (USE_IMACTIVATEFUNC)
    {
	எனில் (active != im_get_status())
	{
	    call_imactivatefunc(active);
	    xim_has_focus = active;
	}
	திரும்பு;
    }
#  endif

    எனில் (xic == வெற்று)
	திரும்பு;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_active = active;
    xim_set_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    வெறுமை
xim_set_focus(முழு focus)
{
    எனில் (xic == வெற்று)
	திரும்பு;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    எனில் (focus && xim_is_active)
    {
	எனில் (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    இல்லையெனில்
    {
	எனில் (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    வெறுமை
im_set_position(முழு row UNUSED, முழு col UNUSED)
{
    xim_set_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    வெறுமை
xim_set_preedit(வெறுமை)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    முழு வரி_space;

    எனில் (xic == வெற்று)
	திரும்பு;

    xim_set_focus(TRUE);

    எனில் (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							வெற்று);
	XSetICValues(xic, XNPreeditAttributes, attr_list, வெற்று);
	XFree(attr_list);
	திரும்பு;
    }

    எனில் (உள்ளீடு_style & XIMPreeditPosition)
    {
	எனில் (xim_fg_color == INVALCOLOR)
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
	வரி_space = gui.char_height;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, வரி_space,
					வெற்று);
	எனில் (XSetICValues(xic, XNPreeditAttributes, attr_list, வெற்று))
	    emsg(_(e_cannot_set_ic_மதிப்புகள்));
	XFree(attr_list);
    }
}

#  எனில் defined(FEAT_GUI_X11)
#   எனில் defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

நிலையான முழு xim_real_init(Window x11_window, Display *x11_display);


#   ifdef USE_X11R6_XIM
    நிலையான வெறுமை
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
    எனில் (display != x11_display)
	திரும்பு;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    எனில் (xic != வெற்று)
	XUnregisterIMInstantiateCallback(x11_display, வெற்று, வெற்று, வெற்று,
					 xim_instantiate_cb, வெற்று);
}

    நிலையான வெறுமை
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

    xic = வெற்று;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, வெற்று, வெற்று, வெற்று,
				   xim_instantiate_cb, வெற்று);
}
#   endif

    வெறுமை
xim_init(வெறுமை)
{
    Window	x11_window;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = வெற்று;

    எனில் (xim_real_init(x11_window, x11_display))
	திரும்பு;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, வெற்று, வெற்று, வெற்று,
				   xim_instantiate_cb, வெற்று);
#   endif
}

    நிலையான முழு
xim_real_init(Window x11_window, Display *x11_display)
{
    முழு		i;
    எழுத்து	*p,
		*s,
		*ns,
		*end,
		tmp[1024];
#   define IMLEN_MAX 40
    எழுத்து	buf[IMLEN_MAX + 7];
    XIM		xim = வெற்று;
    XIMStyles	*xim_styles;
    XIMStyle	this_உள்ளீடு_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    உள்ளீடு_style = 0;
    status_area_enabled = FALSE;

    எனில் (xic != வெற்று)
	திரும்பு FALSE;

    எனில் (gui.rsrc_உள்ளீடு_method != வெற்று && *gui.rsrc_உள்ளீடு_method != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_உள்ளீடு_method,
	    அளவு(tmp) - 1);
	க்கு (ns = s = tmp; ns != வெற்று && *s != NUL;)
	{
	    s = (எழுத்து *)skipwhite((char_u *)s);
	    எனில் (*s == NUL)
		நிறுத்து;
	    எனில் ((ns = end = strchr(s, ',')) == வெற்று)
		end = s + strlen(s);
	    போது (SAFE_isspace(end[-1]))
		end--;
	    *end = NUL;

	    எனில் (strlen(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		எனில் ((p = XSetLocaleModifiers(buf)) != வெற்று && *p != NUL
			&& (xim = XOpenIM(x11_display, வெற்று, வெற்று, வெற்று))
								      != வெற்று)
		    நிறுத்து;
	    }

	    s = ns + 1;
	}
    }

    எனில் (xim == வெற்று && (p = XSetLocaleModifiers("")) != வெற்று && *p != NUL)
	xim = XOpenIM(x11_display, வெற்று, வெற்று, வெற்று);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    எனில் (xim == வெற்று && (p = XSetLocaleModifiers("@im=none")) != வெற்று
								 && *p != NUL)
	xim = XOpenIM(x11_display, வெற்று, வெற்று, வெற்று);

    எனில் (xim == வெற்று)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_to_open_உள்ளீடு_method));
	    verbose_leave();
	}
	திரும்பு FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_data = வெற்று;
	எனில் (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, வெற்று))
	    emsg(_(e_warning_could_not_set_destroy_callback_to_im));
    }
#   endif

    எனில் (XGetIMValues(xim, XNQuery_உள்ளீடு_Style, &xim_styles, வெற்று) || !xim_styles)
    {
	emsg(_(e_உள்ளீடு_method_doesnt_support_any_style));
	XCloseIM(xim);
	திரும்பு FALSE;
    }

    found = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    அளவு(tmp) - 1);
    க்கு (s = tmp; s && !found; )
    {
	போது (*s && SAFE_isspace(*s))
	    s++;
	எனில் (!*s)
	    நிறுத்து;
	எனில் ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	இல்லையெனில்
	    end = s + strlen(s);
	போது (SAFE_isspace(*end))
	    end--;
	*end = '\0';

	எனில் (!strcmp(s, "OverTheSpot"))
	    this_உள்ளீடு_style = (XIMPreeditPosition | XIMStatusArea);
	இல்லையெனில் எனில் (!strcmp(s, "OffTheSpot"))
	    this_உள்ளீடு_style = (XIMPreeditArea | XIMStatusArea);
	இல்லையெனில் எனில் (!strcmp(s, "Root"))
	    this_உள்ளீடு_style = (XIMPreeditNothing | XIMStatusNothing);

	க்கு (i = 0; (unsigned short)i < xim_styles->மொத்தம்_styles; i++)
	{
	    எனில் (this_உள்ளீடு_style == xim_styles->supported_styles[i])
	    {
		found = True;
		நிறுத்து;
	    }
	}
	எனில் (!found)
	    க்கு (i = 0; (unsigned short)i < xim_styles->மொத்தம்_styles; i++)
	    {
		எனில் ((xim_styles->supported_styles[i] & this_உள்ளீடு_style)
			== (this_உள்ளீடு_style & ~XIMStatusArea))
		{
		    this_உள்ளீடு_style &= ~XIMStatusArea;
		    found = True;
		    நிறுத்து;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    எனில் (!found)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_உள்ளீடு_method_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	திரும்பு FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    உள்ளீடு_style = this_உள்ளீடு_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    எனில் (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				வெற்று);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				வெற்று);
    }
    இல்லையெனில்
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				வெற்று);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				வெற்று);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, உள்ளீடு_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    வெற்று);
    XFree(status_list);
    XFree(preedit_list);
    எனில் (xic != வெற்று)
    {
	எனில் (உள்ளீடு_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	இல்லையெனில்
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    இல்லையெனில்
    {
	எனில் (!is_not_a_term())
	    emsg(_(e_failed_to_create_உள்ளீடு_context));
	XCloseIM(xim);
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    முழு
im_get_status(வெறுமை)
{
#  ifdef FEAT_EVAL
    எனில் (USE_IMSTATUSFUNC)
	திரும்பு call_imstatusfunc();
#  endif
    திரும்பு xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# எனில் !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    வெறுமை
xim_set_status_area(வெறுமை)
{
    XVaNestedList preedit_list = 0, status_list = 0, list = 0;
    XRectangle pre_area, status_area;

    எனில் (xic == வெற்று)
	திரும்பு;

    எனில் (உள்ளீடு_style & XIMStatusArea)
    {
	எனில் (உள்ளீடு_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    status_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, வெற்று);
	    XGetICValues(xic, XNStatusAttributes, status_list, வெற்று);
	    XFree(status_list);

	    status_area.width = needed_rect->width;
	}
	இல்லையெனில்
	    status_area.width = gui.char_width * Columns;

	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	எனில் (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	எனில் (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.height = gui.char_height;
	status_list = XVaCreateNestedList(0, XNArea, &status_area, வெற்று);
    }
    இல்லையெனில்
    {
	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	எனில் (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	எனில் (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.width = 0;
	status_area.height = gui.char_height;
    }

    எனில் (உள்ளீடு_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = status_area.x + status_area.width;
	pre_area.y = gui.char_height * Rows + gui.border_offset;
	pre_area.width = gui.char_width * Columns - pre_area.x;
	எனில் (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	எனில் (gui.menu_is_active)
	    pre_area.y += gui.menu_height;
#  endif
	pre_area.height = gui.char_height;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, வெற்று);
    }
    இல்லையெனில் எனில் (உள்ளீடு_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.height = gui.char_height * Rows;
	pre_area.width = gui.char_width * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, வெற்று);
    }

    எனில் (preedit_list && status_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   XNStatusAttributes, status_list, வெற்று);
    இல்லையெனில் எனில் (preedit_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   வெற்று);
    இல்லையெனில் எனில் (status_list)
	list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				   வெற்று);
    இல்லையெனில்
	list = வெற்று;

    எனில் (list)
    {
	XSetICValues(xic, XNVaNestedList, list, வெற்று);
	XFree(list);
    }
    எனில் (status_list)
	XFree(status_list);
    எனில் (preedit_list)
	XFree(preedit_list);
}

    முழு
xim_get_status_area_height(வெறுமை)
{
    எனில் (status_area_enabled)
	திரும்பு gui.char_height;
    திரும்பு 0;
}
# endif

#இல்லையெனில் // !defined(FEAT_XIM)

# எனில் defined(IME_WITHOUT_XIM) || defined(VIMDLL)
நிலையான முழு im_was_set_active = FALSE;

    முழு
#  ifdef VIMDLL
mbyte_im_get_status(வெறுமை)
#  இல்லையெனில்
im_get_status(வெறுமை)
#  endif
{
#  எனில் defined(FEAT_EVAL)
    எனில் (USE_IMSTATUSFUNC)
	திரும்பு call_imstatusfunc();
#  endif
    திரும்பு im_was_set_active;
}

    வெறுமை
#  ifdef VIMDLL
mbyte_im_set_active(முழு active_arg)
#  இல்லையெனில்
im_set_active(முழு active_arg)
#  endif
{
#  எனில் defined(FEAT_EVAL)
    முழு	    active = !p_imdisable && active_arg;

    எனில் (USE_IMACTIVATEFUNC && active != im_get_status())
    {
	call_imactivatefunc(active);
	im_was_set_active = active;
    }
#  endif
}

#  எனில் defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    வெறுமை
im_set_position(முழு row UNUSED, முழு col UNUSED)
{
}
#  endif
# endif

#endif // FEAT_XIM
