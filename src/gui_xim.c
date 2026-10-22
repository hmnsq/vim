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

#شامل "vim.h"

#اگر !defined(GTK_CHECK_VERSION)
# define GTK_CHECK_VERSION(a, b, c) 0
#endif

#اگر defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# وگرنه
#  include <gdk/gdkkeysyms.h>
# endif
# اگر !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  وگرنه
#   include <gdk/gdkx.h>
#  endif
# endif
#endif

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#اگر defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
ایستا تهی xim_log(نویسه *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    ایستا تهی
xim_log(نویسه *s, ...)
{
    va_list arglist;
    ایستا FILE *fd = پوچ;

    اگر (fd == (FILE *)-1)
	بازگشت;
    اگر (fd == پوچ)
    {
	fd = mch_fopen("xim.log", "w");
	اگر (fd == پوچ)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    بازگشت;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#endif

#اگر defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.in_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.in_use && *p_imsf != NUL)
#وگرنه
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#endif

#اگر defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
ایستا callback_T imaf_cb;	    // 'imactivatefunc' callback function
ایستا callback_T imsf_cb;	    // 'imstatusfunc' callback function

    نویسه *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    اگر (option_set_callback_func(p_imaf, &imaf_cb) == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    نویسه *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    اگر (option_set_callback_func(p_imsf, &imsf_cb) == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    ایستا تهی
call_imactivatefunc(صحیح active)
{
    typval_T argv[2];
    صحیح save_کلید_Typed = کلید_Typed;

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = active ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (تهی)call_callback_retnr(&imaf_cb, 1, argv);

    کلید_Typed = save_کلید_Typed;
}

    ایستا صحیح
call_imstatusfunc(تهی)
{
    صحیح is_active;
    صحیح save_کلید_Typed = کلید_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    اگر (exiting || is_autocmd_blocked())
	بازگشت FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++پیام_silent;
    is_active = call_callback_retnr(&imsf_cb, 0, پوچ);
    --پیام_silent;

    کلید_Typed = save_کلید_Typed;
    بازگشت (is_active > 0);
}
#endif

#اگر defined(EXITFREE)
    تهی
free_xim_stuff(تهی)
{
# اگر defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    صحیح
set_ref_in_im_funcs(صحیح copyID UNUSED)
{
    صحیح abort = FALSE;

# اگر defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = set_ref_in_callback(&imaf_cb, copyID);
    abort = abort || set_ref_in_callback(&imsf_cb, copyID);
# endif

    بازگشت abort;
}
#endif


#اگر defined(FEAT_XIM)

# اگر defined(FEAT_GUI_GTK)
ایستا صحیح xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    ایستا تهی
init_preedit_start_col(تهی)
{
    اگر (State & MODE_CMDLINE)
	preedit_start_col = cmdline_getvcol_cursor();
    وگرنه اگر (curwin != پوچ && curwin->w_buffer != پوچ)
	getvcol(curwin, &curwin->w_cursor, &preedit_start_col, پوچ, پوچ, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_changed_while_preediting = curbuf->b_changed;
}

ایستا صحیح im_is_active	       = FALSE;	// IM is enabled for current mode
ایستا صحیح preedit_is_active   = FALSE;
ایستا صحیح im_preedit_cursor   = 0;	// cursor offset in characters
ایستا صحیح im_preedit_trailing = 0;	// number of characters after cursor

ایستا unsigned long im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_کلید_Void_نماد
#    define GDK_کلید_Void_نماد GDK_Void_نماد
#   endif
ایستا unsigned صحیح  im_activatekey_keyval = GDK_کلید_Void_نماد;
ایستا unsigned صحیح  im_activatekey_state  = 0;
#  endif

ایستا GtkWidget *preedit_window = پوچ;
ایستا GtkWidget *preedit_label = پوچ;
#  ifdef USE_GTK4
ایستا صحیح preedit_label_width = 0;	// last measured popover natural width
ایستا صحیح preedit_popover_height = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
ایستا GtkCssProvider *preedit_css_provider = پوچ;
ایستا guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

ایستا تهی im_preedit_window_set_position(تهی);

    تهی
im_set_active(صحیح active)
{
    صحیح was_active;

    was_active = !!im_get_status();
    im_is_active = (active && !p_imdisable);

    اگر (im_is_active != was_active)
	xim_reset();
}

    تهی
xim_set_focus(صحیح focus)
{
    اگر (xic == پوچ)
	بازگشت;

    اگر (focus)
	gtk_im_context_focus_in(xic);
    وگرنه
	gtk_im_context_focus_out(xic);
}

    تهی
im_set_position(صحیح row, صحیح col)
{
    اگر (xic == پوچ)
	بازگشت;

    GdkRectangle area;

    area.x = FILL_X(col);
    area.y = FILL_Y(row);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.width  = gui.char_width
		    * ((Screen_سطرها != پوچ && mb_lefthalve(row, col)) ? 2 : 1);
    area.height = gui.char_height;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    اگر (p_imst == IM_OVER_THE_SPOT)
    {
	صحیح popover_h = preedit_popover_height;
	اگر (popover_h <= 0)
	    popover_h = gui.char_height;
	اگر (popover_h > area.height)
	    area.height = popover_h;
    }
#  endif

    gtk_im_context_set_cursor_location(xic, &area);

    اگر (p_imst == IM_OVER_THE_SPOT)
	im_preedit_window_set_position();
}

#  اگر 0 || defined(PROTO) // apparently only used in gui_x11.c
    تهی
xim_set_preedit(تهی)
{
    im_set_position(gui.row, gui.col);
}
#  endif

    ایستا تهی
im_add_to_درون_داد(char_u *str, صحیح len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    اگر (درون_داد_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&درون_داد_conv, str, &len);
	g_return_if_fail(str != پوچ);
    }

    add_to_درون_داد_buf_csi(str, len);

    اگر (درون_داد_conv.vc_type != CONV_NONE)
	vim_free(str);

    اگر (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ایستا تهی
im_preedit_window_set_position(تهی)
{
    اگر (preedit_window == پوچ)
	بازگشت;

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
    اگر (!gtk_widget_compute_point(gui.drawarea, gui.mainwin,
						&pt_in, &pt_out))
    {
	pt_out.x = pt_in.x;
	pt_out.y = pt_in.y;
    }
    rect.x = (صحیح)pt_out.x;
    rect.y = (صحیح)pt_out.y;
    rect.width = preedit_label_width;
    rect.height = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    صحیح offset = gui.char_height - preedit_popover_height;
    gtk_popover_set_offset(GTK_POPOVER(preedit_window), 0, offset);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_set_halign(preedit_window, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(preedit_window), &rect);
#  وگرنه
    صحیح x, y, width, height;
    صحیح screen_x, screen_y, screen_width, screen_height;

    gui_gtk_get_screen_geom_of_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_width, &screen_height);
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), &x, &y);
    gtk_window_get_size(GTK_WINDOW(preedit_window), &width, &height);
    x = x + FILL_X(gui.col);
    y = y + FILL_Y(gui.row);
    اگر (x + width > screen_x + screen_width)
	x = screen_x + screen_width - width;
    اگر (y + height > screen_y + screen_height)
	y = screen_y + screen_height - height;
    gtk_window_move(GTK_WINDOW(preedit_window), x, y);
#  endif
}

    ایستا تهی
im_preedit_window_open(تهی)
{
    نویسه *preedit_string;
#  اگر !GTK_CHECK_VERSION(3,16,0)
    نویسه buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  اگر GTK_CHECK_VERSION(3,0,0)
#   اگر !GTK_CHECK_VERSION(3,16,0)
    GdkRGBA color;
#   endif
#  وگرنه
    GdkColor color;
#  endif
    gint w, h;

    اگر (preedit_window == پوچ)
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
#  وگرنه
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
#  وگرنه
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
    اگر (preedit_css_provider == پوچ || preedit_css_cached_bg != gui.back_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	اگر (preedit_css_provider != پوچ)
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
#   اگر GTK_CHECK_VERSION(3,16,0)
    {
	GtkStyleContext * ثابت context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar		   *css = پوچ;
	ثابت نویسه * ثابت fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_size(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = پوچ;

	اگر (!pango_font_description_get_size_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ثابت screen
		  = gdk_window_get_screen(gtk_widget_get_window(gui.mainwin));
	    ثابت gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	اگر (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	وگرنه
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != پوچ ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.back_pixel >> 16) & 0xff,
		(gui.back_pixel >> 8) & 0xff,
		gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, پوچ);
	gtk_style_context_add_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_VERSION(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, اندازه(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_NORMAL, &color);

    vim_snprintf(buf, اندازه(buf), "#%06X", gui.back_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_NORMAL,
								      &color);
#   وگرنه
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, اندازه(buf), "#%06X", (unsigned)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_NORMAL, &color);

    vim_snprintf(buf, اندازه(buf), "#%06X", (unsigned)gui.back_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_window, GTK_STATE_NORMAL, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, پوچ);

    اگر (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	اگر (attr_list == پوچ)
	    attr_list = pango_attr_list_new();

	اگر (gui.norm_font != پوچ)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->start_index = 0;
	    pa->end_index = G_MAXUINT;
	    pango_attr_list_درج(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_درج(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.back_pixel >> 16) & 0xff) * 257,
		((gui.back_pixel >> 8) & 0xff) * 257,
		(gui.back_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_درج(attr_list, pa);
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
	اگر (xic != پوچ)
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
	اگر (gtk_widget_get_mapped(preedit_window))
	    gtk_popover_popdown(GTK_POPOVER(preedit_window));

	im_preedit_window_set_position();

	gtk_widget_queue_resize(preedit_window);
	gtk_popover_popup(GTK_POPOVER(preedit_window));
#  وگرنه
	gtk_window_resize(GTK_WINDOW(preedit_window), w, h);
	gtk_widget_show_all(preedit_window);

	im_preedit_window_set_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    ایستا تهی
im_preedit_window_close(تهی)
{
    اگر (preedit_window == پوچ)
	بازگشت;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_window));
#  وگرنه
    gtk_widget_hide(preedit_window);
#  endif
}

    ایستا تهی
im_show_preedit(تهی)
{
    im_preedit_window_open();

    اگر (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ایستا تهی
im_delete_preedit(تهی)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    اگر (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_window_close();
	بازگشت;
    }

    اگر (State & MODE_NORMAL
#  ifdef FEAT_TERMINAL
	    && !term_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	بازگشت;
    }
    برای (; im_preedit_cursor > 0; --im_preedit_cursor)
	add_to_درون_داد_buf(bskey, (صحیح)اندازه(bskey));

    برای (; im_preedit_trailing > 0; --im_preedit_trailing)
	add_to_درون_داد_buf(delkey, (صحیح)اندازه(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    ایستا تهی
im_correct_cursor(صحیح رقم_move_back)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    اگر (State & MODE_NORMAL)
	بازگشت;
#  ifdef FEAT_RIGHTLEFT
    اگر ((State & MODE_CMDLINE) == 0 && curwin != پوچ && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    برای (; رقم_move_back > 0; --رقم_move_back)
	add_to_درون_داد_buf(backkey, (صحیح)اندازه(backkey));
}

ایستا صحیح xim_expected_char = NUL;
ایستا صحیح xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    ایستا تهی
im_show_info(تهی)
{
    صحیح	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    اگر ((State & MODE_NORMAL) || (State & MODE_درج))
	setcursor();
    out_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    ایستا تهی
im_commit_cb(GtkIMContext *context UNUSED,
	     ثابت gchar *str,
	     gpointer data UNUSED)
{
    صحیح		slen = (صحیح)STRLEN(str);
    صحیح		add_to_درون_داد = TRUE;
    صحیح		clen;
    صحیح		len = slen;
    صحیح		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    اگر (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	اگر (preedit_start_col == MAXCOL)
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
	اگر (درون_داد_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&درون_داد_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != پوچ);
	}
	وگرنه
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	اگر (درون_داد_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_start_col += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    اگر (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	اگر (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    add_to_درون_داد = FALSE;
	}
	وگرنه
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    اگر (add_to_درون_داد)
	im_add_to_درون_داد((char_u *)str, slen);

    اگر (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	اگر (!commit_with_preedit)
	    preedit_start_col = MAXCOL;

	// This flag is used in changed() at next call.
	xim_changed_while_preediting = TRUE;
    }

    اگر (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    ایستا تهی
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
    ایستا تهی
im_preedit_end_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    اگر (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;

#  اگر 0
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
    ایستا تهی
im_preedit_changed_cb(GtkIMContext *context, gpointer data UNUSED)
{
    نویسه    *preedit_string = پوچ;
    صحیح	    cursor_index    = 0;
    صحیح	    رقم_move_back   = 0;
    char_u  *str;
    char_u  *p;
    صحیح	    i;

    اگر (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, پوچ,
					  &cursor_index);
    وگرنه
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, پوچ,
					  پوچ);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != پوچ); // just in case

    اگر (p_imst == IM_OVER_THE_SPOT)
    {
	اگر (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	وگرنه
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    وگرنه
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	اگر (preedit_start_col == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_start_col();
	}
	وگرنه اگر (cursor_index == 0 && preedit_string[0] == '\0')
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
	اگر (preedit_start_col != MAXCOL)
	    preedit_end_col = preedit_start_col;
	str = (char_u *)preedit_string;
	برای (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    صحیح is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    اگر (!is_composing || p_deco)
	    {
		اگر (i < cursor_index)
		    ++im_preedit_cursor;
		وگرنه
		    ++im_preedit_trailing;
	    }
	    اگر (!is_composing && i >= cursor_index)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++رقم_move_back;
	    }
	    اگر (preedit_start_col != MAXCOL)
		preedit_end_col += utf_ptr2cells(p);
	}

	اگر (p > str)
	{
	    im_add_to_درون_داد(str, (صحیح)(p - str));
	    im_correct_cursor(رقم_move_back);
	}
    }

    g_free(preedit_string);

    اگر (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    ایستا صحیح
translate_pango_attributes(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    صحیح		    char_attr = HL_NORMAL;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    اگر (attr != پوچ && ((PangoAttrInt *)attr)->مقدار
						 != (صحیح)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_WEIGHT);
    اگر (attr != پوچ && ((PangoAttrInt *)attr)->مقدار >= (صحیح)PANGO_WEIGHT_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    اگر (attr != پوچ && ((PangoAttrInt *)attr)->مقدار
						   != (صحیح)PANGO_STYLE_NORMAL)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    اگر (attr != پوچ)
    {
	ثابت PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	اگر ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    بازگشت char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    صحیح
im_get_feedback_attr(صحیح col)
{
    نویسه	    *preedit_string = پوچ;
    PangoAttrList   *attr_list	    = پوچ;
    صحیح		    char_attr	    = -1;

    اگر (xic == پوچ)
	بازگشت char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, پوچ);

    اگر (preedit_string != پوچ && attr_list != پوچ)
    {
	صحیح idx;

	// Get the byte index as used by PangoAttrIterator
	برای (idx = 0; col > 0 && preedit_string[idx] != '\0'; --col)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	اگر (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    صحیح			start, end;

	    char_attr = HL_NORMAL;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    انجامبده
	    {
		pango_attr_iterator_range(iter, &start, &end);

		اگر (idx >= start && idx < end)
		    char_attr |= translate_pango_attributes(iter);
	    }
	    وقتی (pango_attr_iterator_next(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    اگر (attr_list != پوچ)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    بازگشت char_attr;
}

    تهی
xim_init(تهی)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != پوچ);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_window(gui.drawarea) != پوچ);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), پوچ);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_changed_cb), پوچ);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_start_cb), پوچ);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_end_cb), پوچ);

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
#  وگرنه
    gtk_im_context_set_client_window(xic, gtk_widget_get_window(gui.drawarea));
#  endif
}

    تهی
im_shutdown(تهی)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    اگر (xic != پوچ)
    {
	gtk_im_context_focus_out(xic);
	g_object_unref(xic);
	xic = پوچ;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    اگر (preedit_window != پوچ)
    {
	gtk_widget_unparent(preedit_window);
	preedit_window = پوچ;
	preedit_label = پوچ;
    }
    اگر (preedit_css_provider != پوچ)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = پوچ;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_active = FALSE;
    im_commit_handler_id = 0;
    اگر (p_imst == IM_ON_THE_SPOT)
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
    ایستا صحیح
im_string_to_keyval(ثابت نویسه *str, unsigned صحیح *keyval, unsigned صحیح *state)
{
    ثابت نویسه	    *mods_end;
    unsigned	    tmp_keyval;
    unsigned	    tmp_state = 0;

    mods_end = strrchr(str, '-');
    mods_end = (mods_end != پوچ) ? mods_end + 1 : str;

    // Parse modifier keys
    وقتی (str < mods_end)
	انتخاب (*str++)
	{
	    حالت '-':							بشکن;
	    حالت 'S': حالت 's': tmp_state |= (unsigned)GDK_SHIFT_MASK;	بشکن;
	    حالت 'L': حالت 'l': tmp_state |= (unsigned)GDK_LOCK_MASK;	بشکن;
	    حالت 'C': حالت 'c': tmp_state |= (unsigned)GDK_CONTROL_MASK;بشکن;
	    حالت '1':		tmp_state |= (unsigned)GDK_MOD1_MASK;	بشکن;
	    حالت '2':		tmp_state |= (unsigned)GDK_MOD2_MASK;	بشکن;
	    حالت '3':		tmp_state |= (unsigned)GDK_MOD3_MASK;	بشکن;
	    حالت '4':		tmp_state |= (unsigned)GDK_MOD4_MASK;	بشکن;
	    حالت '5':		tmp_state |= (unsigned)GDK_MOD5_MASK;	بشکن;
	    پیشفرض:
		بازگشت FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    اگر (tmp_keyval == 0 || tmp_keyval == GDK_Void_نماد)
	بازگشت FALSE;

    اگر (keyval != پوچ)
	*keyval = tmp_keyval;
    اگر (state != پوچ)
	*state = tmp_state;

    بازگشت TRUE;
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
    صحیح
im_xim_isvalid_imactivate(تهی)
{
    اگر (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_نماد;
	im_activatekey_state  = 0;
	بازگشت TRUE;
    }

    بازگشت im_string_to_keyval((ثابت نویسه *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    ایستا تهی
im_synthesize_keypress(unsigned صحیح keyval, unsigned صحیح state)
{
    Gdk_Event_کلید *event;

    event = (Gdk_Event_کلید *)gdk_event_new(GDK_کلید_PRESS);
    g_object_ref(gtk_widget_get_window(gui.drawarea));
					// unreffed by gdk_event_free()
    event->window = gtk_widget_get_window(gui.drawarea);
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysymToKeycode(GDK_WINDOW_XDISPLAY(event->window), (کلید_Sym)keyval);
    event->length = 0;
    event->string = پوچ;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_کلید_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    تهی
xim_reset(تهی)
{
#  ifdef FEAT_EVAL
    اگر (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_active);
    وگرنه
#  endif
    اگر (xic != پوچ)
    {
	gtk_im_context_reset(xic);

	اگر (p_imdisable)
	    im_shutdown();
	وگرنه
	{
	    xim_set_focus(gui.in_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_set_focus(gui.in_focus);
#  وگرنه
	    اگر (im_activatekey_keyval != GDK_Void_نماد)
	    {
		اگر (im_is_active)
		{
		    g_signal_handler_block(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    وگرنه
	    {
		im_shutdown();
		xim_init();
		xim_set_focus(gui.in_focus);
	    }
#  endif
	}
    }

    اگر (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    صحیح
xim_queue_کلید_press_event(Gdk_Event_کلید *event, صحیح down)
{
#   ifdef FEAT_GUI_GTK
    اگر (event->state & GDK_SUPER_MASK) بازگشت FALSE;
#   endif
    اگر (down)
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
	انتخاب (event->keyval)
	{
	    حالت GDK_KP_Add:      xim_expected_char = '+';  بشکن;
	    حالت GDK_KP_Subtract: xim_expected_char = '-';  بشکن;
	    حالت GDK_KP_Divide:   xim_expected_char = '/';  بشکن;
	    حالت GDK_KP_Multiply: xim_expected_char = '*';  بشکن;
	    حالت GDK_KP_Decimal:  xim_expected_char = '.';  بشکن;
	    حالت GDK_KP_Equal:    xim_expected_char = '=';  بشکن;
	    حالت GDK_KP_0:	  xim_expected_char = '0';  بشکن;
	    حالت GDK_KP_1:	  xim_expected_char = '1';  بشکن;
	    حالت GDK_KP_2:	  xim_expected_char = '2';  بشکن;
	    حالت GDK_KP_3:	  xim_expected_char = '3';  بشکن;
	    حالت GDK_KP_4:	  xim_expected_char = '4';  بشکن;
	    حالت GDK_KP_5:	  xim_expected_char = '5';  بشکن;
	    حالت GDK_KP_6:	  xim_expected_char = '6';  بشکن;
	    حالت GDK_KP_7:	  xim_expected_char = '7';  بشکن;
	    حالت GDK_KP_8:	  xim_expected_char = '8';  بشکن;
	    حالت GDK_KP_9:	  xim_expected_char = '9';  بشکن;
	    حالت GDK_space:	  xim_expected_char = ' ';  بشکن;
	    پیشفرض:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    اگر (xic != پوچ && !p_imdisable
		    && (State & (MODE_درج | MODE_CMDLINE
					      | MODE_NORMAL | MODE_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	اگر (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    unsigned صحیح state_mask;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_mask  = im_activatekey_state;
	    state_mask |= ((صحیح)GDK_SHIFT_MASK | (صحیح)GDK_CONTROL_MASK
							| (صحیح)GDK_MOD1_MASK);

	    اگر ((event->state & state_mask) != im_activatekey_state)
		بازگشت FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    اگر (event->type != GDK_کلید_PRESS)
		بازگشت TRUE;

	    اگر (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
	    {
		im_set_active(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= MODE_LANGMAP;
		اگر (State & MODE_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~MODE_LANGMAP;
		}
		وگرنه
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= MODE_LANGMAP;
		}
		بازگشت TRUE;
	    }

	    بازگشت gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	اگر (im_activatekey_keyval == GDK_Void_نماد || im_is_active)
	{
	    صحیح imresult = gtk_im_context_filter_keypress(xic, event);

	    اگر (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		اگر (!imresult && preedit_start_col != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    بازگشت FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    اگر (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		بازگشت FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    بازگشت imresult;
	}
    }

    بازگشت FALSE;
}
#  وگرنه // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    صحیح
im_xim_isvalid_imactivate(تهی)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    بازگشت p_imak[0] == NUL;
}

    صحیح
xim_queue_کلید_press_event(GdkEvent *event UNUSED, صحیح down UNUSED)
{
    بازگشت FALSE;
}
#  endif // !USE_GTK4

    صحیح
im_get_status(تهی)
{
#  ifdef FEAT_EVAL
    اگر (USE_IMSTATUSFUNC)
	بازگشت call_imstatusfunc();
#  endif
    بازگشت im_is_active;
}

    صحیح
preedit_get_status(تهی)
{
    بازگشت preedit_is_active;
}

    صحیح
im_is_preediting(تهی)
{
    بازگشت xim_has_preediting;
}

# وگرنه // !FEAT_GUI_GTK

ایستا صحیح	xim_is_active = FALSE;  // XIM should be active in the current
					// mode
ایستا صحیح	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
ایستا XIMStyle	درون_داد_style;
ایستا صحیح	status_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    تهی
im_set_active(صحیح active_arg)
{
    صحیح active = active_arg;

    // If 'imdisable' is set, XIM is never active.
    اگر (p_imdisable)
	active = FALSE;
    وگرنه اگر (درون_داد_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	active = TRUE;

#  اگر defined(FEAT_EVAL)
    اگر (USE_IMACTIVATEFUNC)
    {
	اگر (active != im_get_status())
	{
	    call_imactivatefunc(active);
	    xim_has_focus = active;
	}
	بازگشت;
    }
#  endif

    اگر (xic == پوچ)
	بازگشت;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_active = active;
    xim_set_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    تهی
xim_set_focus(صحیح focus)
{
    اگر (xic == پوچ)
	بازگشت;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    اگر (focus && xim_is_active)
    {
	اگر (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    وگرنه
    {
	اگر (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    تهی
im_set_position(صحیح row UNUSED, صحیح col UNUSED)
{
    xim_set_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    تهی
xim_set_preedit(تهی)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    صحیح سطر_space;

    اگر (xic == پوچ)
	بازگشت;

    xim_set_focus(TRUE);

    اگر (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							پوچ);
	XSetICValues(xic, XNPreeditAttributes, attr_list, پوچ);
	XFree(attr_list);
	بازگشت;
    }

    اگر (درون_داد_style & XIMPreeditPosition)
    {
	اگر (xim_fg_color == INVALCOLOR)
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
	سطر_space = gui.char_height;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, سطر_space,
					پوچ);
	اگر (XSetICValues(xic, XNPreeditAttributes, attr_list, پوچ))
	    emsg(_(e_cannot_set_ic_مقادیر));
	XFree(attr_list);
    }
}

#  اگر defined(FEAT_GUI_X11)
#   اگر defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

ایستا صحیح xim_real_init(Window x11_window, Display *x11_display);


#   ifdef USE_X11R6_XIM
    ایستا تهی
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
    اگر (display != x11_display)
	بازگشت;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    اگر (xic != پوچ)
	XUnregisterIMInstantiateCallback(x11_display, پوچ, پوچ, پوچ,
					 xim_instantiate_cb, پوچ);
}

    ایستا تهی
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

    xic = پوچ;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, پوچ, پوچ, پوچ,
				   xim_instantiate_cb, پوچ);
}
#   endif

    تهی
xim_init(تهی)
{
    Window	x11_window;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = پوچ;

    اگر (xim_real_init(x11_window, x11_display))
	بازگشت;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, پوچ, پوچ, پوچ,
				   xim_instantiate_cb, پوچ);
#   endif
}

    ایستا صحیح
xim_real_init(Window x11_window, Display *x11_display)
{
    صحیح		i;
    نویسه	*p,
		*s,
		*ns,
		*end,
		tmp[1024];
#   define IMLEN_MAX 40
    نویسه	buf[IMLEN_MAX + 7];
    XIM		xim = پوچ;
    XIMStyles	*xim_styles;
    XIMStyle	this_درون_داد_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    درون_داد_style = 0;
    status_area_enabled = FALSE;

    اگر (xic != پوچ)
	بازگشت FALSE;

    اگر (gui.rsrc_درون_داد_method != پوچ && *gui.rsrc_درون_داد_method != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_درون_داد_method,
	    اندازه(tmp) - 1);
	برای (ns = s = tmp; ns != پوچ && *s != NUL;)
	{
	    s = (نویسه *)skipwhite((char_u *)s);
	    اگر (*s == NUL)
		بشکن;
	    اگر ((ns = end = strchr(s, ',')) == پوچ)
		end = s + strlen(s);
	    وقتی (SAFE_isspace(end[-1]))
		end--;
	    *end = NUL;

	    اگر (strlen(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		اگر ((p = XSetLocaleModifiers(buf)) != پوچ && *p != NUL
			&& (xim = XOpenIM(x11_display, پوچ, پوچ, پوچ))
								      != پوچ)
		    بشکن;
	    }

	    s = ns + 1;
	}
    }

    اگر (xim == پوچ && (p = XSetLocaleModifiers("")) != پوچ && *p != NUL)
	xim = XOpenIM(x11_display, پوچ, پوچ, پوچ);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    اگر (xim == پوچ && (p = XSetLocaleModifiers("@im=none")) != پوچ
								 && *p != NUL)
	xim = XOpenIM(x11_display, پوچ, پوچ, پوچ);

    اگر (xim == پوچ)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_to_open_درون_داد_method));
	    verbose_leave();
	}
	بازگشت FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_data = پوچ;
	اگر (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, پوچ))
	    emsg(_(e_warning_could_not_set_destroy_callback_to_im));
    }
#   endif

    اگر (XGetIMValues(xim, XNQuery_درون_داد_Style, &xim_styles, پوچ) || !xim_styles)
    {
	emsg(_(e_درون_داد_method_doesnt_support_any_style));
	XCloseIM(xim);
	بازگشت FALSE;
    }

    found = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    اندازه(tmp) - 1);
    برای (s = tmp; s && !found; )
    {
	وقتی (*s && SAFE_isspace(*s))
	    s++;
	اگر (!*s)
	    بشکن;
	اگر ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	وگرنه
	    end = s + strlen(s);
	وقتی (SAFE_isspace(*end))
	    end--;
	*end = '\0';

	اگر (!strcmp(s, "OverTheSpot"))
	    this_درون_داد_style = (XIMPreeditPosition | XIMStatusArea);
	وگرنه اگر (!strcmp(s, "OffTheSpot"))
	    this_درون_داد_style = (XIMPreeditArea | XIMStatusArea);
	وگرنه اگر (!strcmp(s, "Root"))
	    this_درون_داد_style = (XIMPreeditNothing | XIMStatusNothing);

	برای (i = 0; (unsigned short)i < xim_styles->شمار_styles; i++)
	{
	    اگر (this_درون_داد_style == xim_styles->supported_styles[i])
	    {
		found = True;
		بشکن;
	    }
	}
	اگر (!found)
	    برای (i = 0; (unsigned short)i < xim_styles->شمار_styles; i++)
	    {
		اگر ((xim_styles->supported_styles[i] & this_درون_داد_style)
			== (this_درون_داد_style & ~XIMStatusArea))
		{
		    this_درون_داد_style &= ~XIMStatusArea;
		    found = True;
		    بشکن;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    اگر (!found)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_درون_داد_method_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	بازگشت FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    درون_داد_style = this_درون_داد_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    اگر (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				پوچ);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				پوچ);
    }
    وگرنه
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				پوچ);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				پوچ);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, درون_داد_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    پوچ);
    XFree(status_list);
    XFree(preedit_list);
    اگر (xic != پوچ)
    {
	اگر (درون_داد_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	وگرنه
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    وگرنه
    {
	اگر (!is_not_a_term())
	    emsg(_(e_failed_to_create_درون_داد_context));
	XCloseIM(xim);
	بازگشت FALSE;
    }

    بازگشت TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    صحیح
im_get_status(تهی)
{
#  ifdef FEAT_EVAL
    اگر (USE_IMSTATUSFUNC)
	بازگشت call_imstatusfunc();
#  endif
    بازگشت xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# اگر !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    تهی
xim_set_status_area(تهی)
{
    XVaNestedList preedit_list = 0, status_list = 0, list = 0;
    XRectangle pre_area, status_area;

    اگر (xic == پوچ)
	بازگشت;

    اگر (درون_داد_style & XIMStatusArea)
    {
	اگر (درون_داد_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    status_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, پوچ);
	    XGetICValues(xic, XNStatusAttributes, status_list, پوچ);
	    XFree(status_list);

	    status_area.width = needed_rect->width;
	}
	وگرنه
	    status_area.width = gui.char_width * Columns;

	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	اگر (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	اگر (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.height = gui.char_height;
	status_list = XVaCreateNestedList(0, XNArea, &status_area, پوچ);
    }
    وگرنه
    {
	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	اگر (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	اگر (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.width = 0;
	status_area.height = gui.char_height;
    }

    اگر (درون_داد_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = status_area.x + status_area.width;
	pre_area.y = gui.char_height * Rows + gui.border_offset;
	pre_area.width = gui.char_width * Columns - pre_area.x;
	اگر (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	اگر (gui.menu_is_active)
	    pre_area.y += gui.menu_height;
#  endif
	pre_area.height = gui.char_height;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, پوچ);
    }
    وگرنه اگر (درون_داد_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.height = gui.char_height * Rows;
	pre_area.width = gui.char_width * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, پوچ);
    }

    اگر (preedit_list && status_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   XNStatusAttributes, status_list, پوچ);
    وگرنه اگر (preedit_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   پوچ);
    وگرنه اگر (status_list)
	list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				   پوچ);
    وگرنه
	list = پوچ;

    اگر (list)
    {
	XSetICValues(xic, XNVaNestedList, list, پوچ);
	XFree(list);
    }
    اگر (status_list)
	XFree(status_list);
    اگر (preedit_list)
	XFree(preedit_list);
}

    صحیح
xim_get_status_area_height(تهی)
{
    اگر (status_area_enabled)
	بازگشت gui.char_height;
    بازگشت 0;
}
# endif

#وگرنه // !defined(FEAT_XIM)

# اگر defined(IME_WITHOUT_XIM) || defined(VIMDLL)
ایستا صحیح im_was_set_active = FALSE;

    صحیح
#  ifdef VIMDLL
mbyte_im_get_status(تهی)
#  وگرنه
im_get_status(تهی)
#  endif
{
#  اگر defined(FEAT_EVAL)
    اگر (USE_IMSTATUSFUNC)
	بازگشت call_imstatusfunc();
#  endif
    بازگشت im_was_set_active;
}

    تهی
#  ifdef VIMDLL
mbyte_im_set_active(صحیح active_arg)
#  وگرنه
im_set_active(صحیح active_arg)
#  endif
{
#  اگر defined(FEAT_EVAL)
    صحیح	    active = !p_imdisable && active_arg;

    اگر (USE_IMACTIVATEFUNC && active != im_get_status())
    {
	call_imactivatefunc(active);
	im_was_set_active = active;
    }
#  endif
}

#  اگر defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    تهی
im_set_position(صحیح row UNUSED, صحیح col UNUSED)
{
}
#  endif
# endif

#endif // FEAT_XIM
