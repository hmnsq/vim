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

#include "vim.h"

#ຖ້າ !defined(GTK_CHECK_VERSION)
# define GTK_CHECK_VERSION(a, b, c) 0
#endif

#ຖ້າ defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ບໍ່ດັ່ງນັ້ນ
#  include <gdk/gdkkeysyms.h>
# endif
# ຖ້າ !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  ບໍ່ດັ່ງນັ້ນ
#   include <gdk/gdkx.h>
#  endif
# endif
#endif

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#ຖ້າ defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
static void xim_log(var_na *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    static void
xim_log(var_na *s, ...)
{
    va_list arglist;
    static FILE *fd = NULL;

    ຖ້າ (fd == (FILE *)-1)
	ສົ່ງຄືນ;
    ຖ້າ (fd == NULL)
    {
	fd = mch_fopen("xim.log", "w");
	ຖ້າ (fd == NULL)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    ສົ່ງຄືນ;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#endif

#ຖ້າ defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.in_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.in_use && *p_imsf != NUL)
#ບໍ່ດັ່ງນັ້ນ
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#endif

#ຖ້າ defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
static callback_T imaf_cb;	    // 'imactivatefunc' callback function
static callback_T imsf_cb;	    // 'imstatusfunc' callback function

    var_na *
did_set_imactivatefunc(optset_T *args UNUSED)
{
    ຖ້າ (option_set_callback_func(p_imaf, &imaf_cb) == FAIL)
	ສົ່ງຄືນ e_invalid_argument;

    ສົ່ງຄືນ NULL;
}

    var_na *
did_set_imstatusfunc(optset_T *args UNUSED)
{
    ຖ້າ (option_set_callback_func(p_imsf, &imsf_cb) == FAIL)
	ສົ່ງຄືນ e_invalid_argument;

    ສົ່ງຄືນ NULL;
}

    static void
call_imactivatefunc(ຈຳນວນເຕັມ active)
{
    typval_T argv[2];
    ຈຳນວນເຕັມ save_KeyTyped = KeyTyped;

    argv[0].v_type = VAR_NUMBER;
    argv[0].vval.v_number = active ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (void)call_callback_retnr(&imaf_cb, 1, argv);

    KeyTyped = save_KeyTyped;
}

    static ຈຳນວນເຕັມ
call_imstatusfunc(void)
{
    ຈຳນວນເຕັມ is_active;
    ຈຳນວນເຕັມ save_KeyTyped = KeyTyped;

    // FIXME: Don't execute user function in unsafe situation.
    ຖ້າ (exiting || is_autocmd_blocked())
	ສົ່ງຄືນ FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++msg_silent;
    is_active = call_callback_retnr(&imsf_cb, 0, NULL);
    --msg_silent;

    KeyTyped = save_KeyTyped;
    ສົ່ງຄືນ (is_active > 0);
}
#endif

#ຖ້າ defined(EXITFREE)
    void
free_xim_stuff(void)
{
# ຖ້າ defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#endif

#ຖ້າ defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    ຈຳນວນເຕັມ
set_ref_in_im_funcs(ຈຳນວນເຕັມ copyID UNUSED)
{
    ຈຳນວນເຕັມ abort = FALSE;

# ຖ້າ defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = set_ref_in_callback(&imaf_cb, copyID);
    abort = abort || set_ref_in_callback(&imsf_cb, copyID);
# endif

    ສົ່ງຄືນ abort;
}
#endif


#ຖ້າ defined(FEAT_XIM)

# ຖ້າ defined(FEAT_GUI_GTK)
static ຈຳນວນເຕັມ xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    static void
init_preedit_start_col(void)
{
    ຖ້າ (State & MODE_CMDLINE)
	preedit_start_col = cmdline_getvcol_cursor();
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (curwin != NULL && curwin->w_buffer != NULL)
	getvcol(curwin, &curwin->w_cursor, &preedit_start_col, NULL, NULL, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_changed_while_preediting = curbuf->b_changed;
}

static ຈຳນວນເຕັມ im_is_active	       = FALSE;	// IM is enabled for current mode
static ຈຳນວນເຕັມ preedit_is_active   = FALSE;
static ຈຳນວນເຕັມ im_preedit_cursor   = 0;	// cursor offset in characters
static ຈຳນວນເຕັມ im_preedit_trailing = 0;	// number of characters after cursor

static unsigned long im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_KEY_VoidSymbol
#    define GDK_KEY_VoidSymbol GDK_VoidSymbol
#   endif
static unsigned ຈຳນວນເຕັມ  im_activatekey_keyval = GDK_KEY_VoidSymbol;
static unsigned ຈຳນວນເຕັມ  im_activatekey_state  = 0;
#  endif

static GtkWidget *preedit_window = NULL;
static GtkWidget *preedit_label = NULL;
#  ifdef USE_GTK4
static ຈຳນວນເຕັມ preedit_label_width = 0;	// last measured popover natural width
static ຈຳນວນເຕັມ preedit_popover_height = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
static GtkCssProvider *preedit_css_provider = NULL;
static guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

static void im_preedit_window_set_position(void);

    void
im_set_active(ຈຳນວນເຕັມ active)
{
    ຈຳນວນເຕັມ was_active;

    was_active = !!im_get_status();
    im_is_active = (active && !p_imdisable);

    ຖ້າ (im_is_active != was_active)
	xim_reset();
}

    void
xim_set_focus(ຈຳນວນເຕັມ focus)
{
    ຖ້າ (xic == NULL)
	ສົ່ງຄືນ;

    ຖ້າ (focus)
	gtk_im_context_focus_in(xic);
    ບໍ່ດັ່ງນັ້ນ
	gtk_im_context_focus_out(xic);
}

    void
im_set_position(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col)
{
    ຖ້າ (xic == NULL)
	ສົ່ງຄືນ;

    GdkRectangle area;

    area.x = FILL_X(col);
    area.y = FILL_Y(row);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.width  = gui.char_width
		    * ((ScreenLines != NULL && mb_lefthalve(row, col)) ? 2 : 1);
    area.height = gui.char_height;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    ຖ້າ (p_imst == IM_OVER_THE_SPOT)
    {
	ຈຳນວນເຕັມ popover_h = preedit_popover_height;
	ຖ້າ (popover_h <= 0)
	    popover_h = gui.char_height;
	ຖ້າ (popover_h > area.height)
	    area.height = popover_h;
    }
#  endif

    gtk_im_context_set_cursor_location(xic, &area);

    ຖ້າ (p_imst == IM_OVER_THE_SPOT)
	im_preedit_window_set_position();
}

#  ຖ້າ 0 || defined(PROTO) // apparently only used in gui_x11.c
    void
xim_set_preedit(void)
{
    im_set_position(gui.row, gui.col);
}
#  endif

    static void
im_add_to_input(char_u *str, ຈຳນວນເຕັມ len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    ຖ້າ (input_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&input_conv, str, &len);
	g_return_if_fail(str != NULL);
    }

    add_to_input_buf_csi(str, len);

    ຖ້າ (input_conv.vc_type != CONV_NONE)
	vim_free(str);

    ຖ້າ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    static void
im_preedit_window_set_position(void)
{
    ຖ້າ (preedit_window == NULL)
	ສົ່ງຄືນ;

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
    ຖ້າ (!gtk_widget_compute_point(gui.drawarea, gui.mainwin,
						&pt_in, &pt_out))
    {
	pt_out.x = pt_in.x;
	pt_out.y = pt_in.y;
    }
    rect.x = (ຈຳນວນເຕັມ)pt_out.x;
    rect.y = (ຈຳນວນເຕັມ)pt_out.y;
    rect.width = preedit_label_width;
    rect.height = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    ຈຳນວນເຕັມ offset = gui.char_height - preedit_popover_height;
    gtk_popover_set_offset(GTK_POPOVER(preedit_window), 0, offset);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_set_halign(preedit_window, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(preedit_window), &rect);
#  ບໍ່ດັ່ງນັ້ນ
    ຈຳນວນເຕັມ x, y, width, height;
    ຈຳນວນເຕັມ screen_x, screen_y, screen_width, screen_height;

    gui_gtk_get_screen_geom_of_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_width, &screen_height);
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), &x, &y);
    gtk_window_get_size(GTK_WINDOW(preedit_window), &width, &height);
    x = x + FILL_X(gui.col);
    y = y + FILL_Y(gui.row);
    ຖ້າ (x + width > screen_x + screen_width)
	x = screen_x + screen_width - width;
    ຖ້າ (y + height > screen_y + screen_height)
	y = screen_y + screen_height - height;
    gtk_window_move(GTK_WINDOW(preedit_window), x, y);
#  endif
}

    static void
im_preedit_window_open(void)
{
    var_na *preedit_string;
#  ຖ້າ !GTK_CHECK_VERSION(3,16,0)
    var_na buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  ຖ້າ GTK_CHECK_VERSION(3,0,0)
#   ຖ້າ !GTK_CHECK_VERSION(3,16,0)
    GdkRGBA color;
#   endif
#  ບໍ່ດັ່ງນັ້ນ
    GdkColor color;
#  endif
    gint w, h;

    ຖ້າ (preedit_window == NULL)
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
#  ບໍ່ດັ່ງນັ້ນ
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
#  ບໍ່ດັ່ງນັ້ນ
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
    ຖ້າ (preedit_css_provider == NULL || preedit_css_cached_bg != gui.back_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	ຖ້າ (preedit_css_provider != NULL)
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
#   ຖ້າ GTK_CHECK_VERSION(3,16,0)
    {
	GtkStyleContext * const context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * const provider = gtk_css_provider_new();
	gchar		   *css = NULL;
	const var_na * const fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_size(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = NULL;

	ຖ້າ (!pango_font_description_get_size_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * const screen
		  = gdk_window_get_screen(gtk_widget_get_window(gui.mainwin));
	    const gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	ຖ້າ (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	ບໍ່ດັ່ງນັ້ນ
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != NULL ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.back_pixel >> 16) & 0xff,
		(gui.back_pixel >> 8) & 0xff,
		gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, NULL);
	gtk_style_context_add_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_VERSION(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, parimaa_na(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_NORMAL, &color);

    vim_snprintf(buf, parimaa_na(buf), "#%06X", gui.back_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_NORMAL,
								      &color);
#   ບໍ່ດັ່ງນັ້ນ
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, parimaa_na(buf), "#%06X", (unsigned)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_NORMAL, &color);

    vim_snprintf(buf, parimaa_na(buf), "#%06X", (unsigned)gui.back_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_window, GTK_STATE_NORMAL, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, NULL);

    ຖ້າ (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	ຖ້າ (attr_list == NULL)
	    attr_list = pango_attr_list_new();

	ຖ້າ (gui.norm_font != NULL)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->start_index = 0;
	    pa->end_index = G_MAXUINT;
	    pango_attr_list_insert(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_insert(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.back_pixel >> 16) & 0xff) * 257,
		((gui.back_pixel >> 8) & 0xff) * 257,
		(gui.back_pixel & 0xff) * 257);
	pa->start_index = 0;
	pa->end_index = G_MAXUINT;
	pango_attr_list_insert(attr_list, pa);
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
	ຖ້າ (xic != NULL)
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
	ຖ້າ (gtk_widget_get_mapped(preedit_window))
	    gtk_popover_popdown(GTK_POPOVER(preedit_window));

	im_preedit_window_set_position();

	gtk_widget_queue_resize(preedit_window);
	gtk_popover_popup(GTK_POPOVER(preedit_window));
#  ບໍ່ດັ່ງນັ້ນ
	gtk_window_resize(GTK_WINDOW(preedit_window), w, h);
	gtk_widget_show_all(preedit_window);

	im_preedit_window_set_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    static void
im_preedit_window_close(void)
{
    ຖ້າ (preedit_window == NULL)
	ສົ່ງຄືນ;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_window));
#  ບໍ່ດັ່ງນັ້ນ
    gtk_widget_hide(preedit_window);
#  endif
}

    static void
im_show_preedit(void)
{
    im_preedit_window_open();

    ຖ້າ (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    static void
im_delete_preedit(void)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    ຖ້າ (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_window_close();
	ສົ່ງຄືນ;
    }

    ຖ້າ (State & MODE_NORMAL
#  ifdef FEAT_TERMINAL
	    && !term_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	ສົ່ງຄືນ;
    }
    ສຳລັບ (; im_preedit_cursor > 0; --im_preedit_cursor)
	add_to_input_buf(bskey, (ຈຳນວນເຕັມ)parimaa_na(bskey));

    ສຳລັບ (; im_preedit_trailing > 0; --im_preedit_trailing)
	add_to_input_buf(delkey, (ຈຳນວນເຕັມ)parimaa_na(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    static void
im_correct_cursor(ຈຳນວນເຕັມ num_move_back)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    ຖ້າ (State & MODE_NORMAL)
	ສົ່ງຄືນ;
#  ifdef FEAT_RIGHTLEFT
    ຖ້າ ((State & MODE_CMDLINE) == 0 && curwin != NULL && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    ສຳລັບ (; num_move_back > 0; --num_move_back)
	add_to_input_buf(backkey, (ຈຳນວນເຕັມ)parimaa_na(backkey));
}

static ຈຳນວນເຕັມ xim_expected_char = NUL;
static ຈຳນວນເຕັມ xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    static void
im_show_info(void)
{
    ຈຳນວນເຕັມ	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    ຖ້າ ((State & MODE_NORMAL) || (State & MODE_INSERT))
	setcursor();
    out_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    static void
im_commit_cb(GtkIMContext *context UNUSED,
	     const gchar *str,
	     gpointer data UNUSED)
{
    ຈຳນວນເຕັມ		slen = (ຈຳນວນເຕັມ)STRLEN(str);
    ຈຳນວນເຕັມ		add_to_input = TRUE;
    ຈຳນວນເຕັມ		clen;
    ຈຳນວນເຕັມ		len = slen;
    ຈຳນວນເຕັມ		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    ຖ້າ (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	ຖ້າ (preedit_start_col == MAXCOL)
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
	ຖ້າ (input_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&input_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != NULL);
	}
	ບໍ່ດັ່ງນັ້ນ
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	ຖ້າ (input_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_start_col += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    ຖ້າ (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	ຖ້າ (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    add_to_input = FALSE;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    ຖ້າ (add_to_input)
	im_add_to_input((char_u *)str, slen);

    ຖ້າ (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	ຖ້າ (!commit_with_preedit)
	    preedit_start_col = MAXCOL;

	// This flag is used in changed() at next call.
	xim_changed_while_preediting = TRUE;
    }

    ຖ້າ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    static void
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
    static void
im_preedit_end_cb(GtkIMContext *context UNUSED, gpointer data UNUSED)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    ຖ້າ (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;

#  ຖ້າ 0
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
    static void
im_preedit_changed_cb(GtkIMContext *context, gpointer data UNUSED)
{
    var_na    *preedit_string = NULL;
    ຈຳນວນເຕັມ	    cursor_index    = 0;
    ຈຳນວນເຕັມ	    num_move_back   = 0;
    char_u  *str;
    char_u  *p;
    ຈຳນວນເຕັມ	    i;

    ຖ້າ (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, NULL,
					  &cursor_index);
    ບໍ່ດັ່ງນັ້ນ
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, NULL,
					  NULL);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != NULL); // just in case

    ຖ້າ (p_imst == IM_OVER_THE_SPOT)
    {
	ຖ້າ (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	ຖ້າ (preedit_start_col == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_start_col();
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (cursor_index == 0 && preedit_string[0] == '\0')
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
	ຖ້າ (preedit_start_col != MAXCOL)
	    preedit_end_col = preedit_start_col;
	str = (char_u *)preedit_string;
	ສຳລັບ (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    ຈຳນວນເຕັມ is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    ຖ້າ (!is_composing || p_deco)
	    {
		ຖ້າ (i < cursor_index)
		    ++im_preedit_cursor;
		ບໍ່ດັ່ງນັ້ນ
		    ++im_preedit_trailing;
	    }
	    ຖ້າ (!is_composing && i >= cursor_index)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++num_move_back;
	    }
	    ຖ້າ (preedit_start_col != MAXCOL)
		preedit_end_col += utf_ptr2cells(p);
	}

	ຖ້າ (p > str)
	{
	    im_add_to_input(str, (ຈຳນວນເຕັມ)(p - str));
	    im_correct_cursor(num_move_back);
	}
    }

    g_free(preedit_string);

    ຖ້າ (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    static ຈຳນວນເຕັມ
translate_pango_attributes(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    ຈຳນວນເຕັມ		    char_attr = HL_NORMAL;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    ຖ້າ (attr != NULL && ((PangoAttrInt *)attr)->value
						 != (ຈຳນວນເຕັມ)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_WEIGHT);
    ຖ້າ (attr != NULL && ((PangoAttrInt *)attr)->value >= (ຈຳນວນເຕັມ)PANGO_WEIGHT_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    ຖ້າ (attr != NULL && ((PangoAttrInt *)attr)->value
						   != (ຈຳນວນເຕັມ)PANGO_STYLE_NORMAL)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    ຖ້າ (attr != NULL)
    {
	const PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	ຖ້າ ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    ສົ່ງຄືນ char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    ຈຳນວນເຕັມ
im_get_feedback_attr(ຈຳນວນເຕັມ col)
{
    var_na	    *preedit_string = NULL;
    PangoAttrList   *attr_list	    = NULL;
    ຈຳນວນເຕັມ		    char_attr	    = -1;

    ຖ້າ (xic == NULL)
	ສົ່ງຄືນ char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, NULL);

    ຖ້າ (preedit_string != NULL && attr_list != NULL)
    {
	ຈຳນວນເຕັມ idx;

	// Get the byte index as used by PangoAttrIterator
	ສຳລັບ (idx = 0; col > 0 && preedit_string[idx] != '\0'; --col)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	ຖ້າ (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    ຈຳນວນເຕັມ			start, end;

	    char_attr = HL_NORMAL;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    do
	    {
		pango_attr_iterator_range(iter, &start, &end);

		ຖ້າ (idx >= start && idx < end)
		    char_attr |= translate_pango_attributes(iter);
	    }
	    ໃນຂະນະທີ່ (pango_attr_iterator_next(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    ຖ້າ (attr_list != NULL)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    ສົ່ງຄືນ char_attr;
}

    void
xim_init(void)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != NULL);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_window(gui.drawarea) != NULL);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), NULL);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_changed_cb), NULL);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_start_cb), NULL);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_end_cb), NULL);

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
#  ບໍ່ດັ່ງນັ້ນ
    gtk_im_context_set_client_window(xic, gtk_widget_get_window(gui.drawarea));
#  endif
}

    void
im_shutdown(void)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    ຖ້າ (xic != NULL)
    {
	gtk_im_context_focus_out(xic);
	g_object_unref(xic);
	xic = NULL;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    ຖ້າ (preedit_window != NULL)
    {
	gtk_widget_unparent(preedit_window);
	preedit_window = NULL;
	preedit_label = NULL;
    }
    ຖ້າ (preedit_css_provider != NULL)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = NULL;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_active = FALSE;
    im_commit_handler_id = 0;
    ຖ້າ (p_imst == IM_ON_THE_SPOT)
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
    static ຈຳນວນເຕັມ
im_string_to_keyval(const var_na *str, unsigned ຈຳນວນເຕັມ *keyval, unsigned ຈຳນວນເຕັມ *state)
{
    const var_na	    *mods_end;
    unsigned	    tmp_keyval;
    unsigned	    tmp_state = 0;

    mods_end = strrchr(str, '-');
    mods_end = (mods_end != NULL) ? mods_end + 1 : str;

    // Parse modifier keys
    ໃນຂະນະທີ່ (str < mods_end)
	switch (*str++)
	{
	    pak_sa '-':							bha_nga;
	    pak_sa 'S': pak_sa 's': tmp_state |= (unsigned)GDK_SHIFT_MASK;	bha_nga;
	    pak_sa 'L': pak_sa 'l': tmp_state |= (unsigned)GDK_LOCK_MASK;	bha_nga;
	    pak_sa 'C': pak_sa 'c': tmp_state |= (unsigned)GDK_CONTROL_MASK;bha_nga;
	    pak_sa '1':		tmp_state |= (unsigned)GDK_MOD1_MASK;	bha_nga;
	    pak_sa '2':		tmp_state |= (unsigned)GDK_MOD2_MASK;	bha_nga;
	    pak_sa '3':		tmp_state |= (unsigned)GDK_MOD3_MASK;	bha_nga;
	    pak_sa '4':		tmp_state |= (unsigned)GDK_MOD4_MASK;	bha_nga;
	    pak_sa '5':		tmp_state |= (unsigned)GDK_MOD5_MASK;	bha_nga;
	    default:
		ສົ່ງຄືນ FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    ຖ້າ (tmp_keyval == 0 || tmp_keyval == GDK_VoidSymbol)
	ສົ່ງຄືນ FALSE;

    ຖ້າ (keyval != NULL)
	*keyval = tmp_keyval;
    ຖ້າ (state != NULL)
	*state = tmp_state;

    ສົ່ງຄືນ TRUE;
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
    ຈຳນວນເຕັມ
im_xim_isvalid_imactivate(void)
{
    ຖ້າ (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_VoidSymbol;
	im_activatekey_state  = 0;
	ສົ່ງຄືນ TRUE;
    }

    ສົ່ງຄືນ im_string_to_keyval((const var_na *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    static void
im_synthesize_keypress(unsigned ຈຳນວນເຕັມ keyval, unsigned ຈຳນວນເຕັມ state)
{
    GdkEventKey *event;

    event = (GdkEventKey *)gdk_event_new(GDK_KEY_PRESS);
    g_object_ref(gtk_widget_get_window(gui.drawarea));
					// unreffed by gdk_event_free()
    event->window = gtk_widget_get_window(gui.drawarea);
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysymToKeycode(GDK_WINDOW_XDISPLAY(event->window), (KeySym)keyval);
    event->length = 0;
    event->string = NULL;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_KEY_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    void
xim_reset(void)
{
#  ifdef FEAT_EVAL
    ຖ້າ (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_active);
    ບໍ່ດັ່ງນັ້ນ
#  endif
    ຖ້າ (xic != NULL)
    {
	gtk_im_context_reset(xic);

	ຖ້າ (p_imdisable)
	    im_shutdown();
	ບໍ່ດັ່ງນັ້ນ
	{
	    xim_set_focus(gui.in_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_set_focus(gui.in_focus);
#  ບໍ່ດັ່ງນັ້ນ
	    ຖ້າ (im_activatekey_keyval != GDK_VoidSymbol)
	    {
		ຖ້າ (im_is_active)
		{
		    g_signal_handler_block(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		im_shutdown();
		xim_init();
		xim_set_focus(gui.in_focus);
	    }
#  endif
	}
    }

    ຖ້າ (p_imst == IM_ON_THE_SPOT)
	preedit_start_col = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    ຈຳນວນເຕັມ
xim_queue_key_press_event(GdkEventKey *event, ຈຳນວນເຕັມ down)
{
#   ifdef FEAT_GUI_GTK
    ຖ້າ (event->state & GDK_SUPER_MASK) ສົ່ງຄືນ FALSE;
#   endif
    ຖ້າ (down)
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
	switch (event->keyval)
	{
	    pak_sa GDK_KP_Add:      xim_expected_char = '+';  bha_nga;
	    pak_sa GDK_KP_Subtract: xim_expected_char = '-';  bha_nga;
	    pak_sa GDK_KP_Divide:   xim_expected_char = '/';  bha_nga;
	    pak_sa GDK_KP_Multiply: xim_expected_char = '*';  bha_nga;
	    pak_sa GDK_KP_Decimal:  xim_expected_char = '.';  bha_nga;
	    pak_sa GDK_KP_Equal:    xim_expected_char = '=';  bha_nga;
	    pak_sa GDK_KP_0:	  xim_expected_char = '0';  bha_nga;
	    pak_sa GDK_KP_1:	  xim_expected_char = '1';  bha_nga;
	    pak_sa GDK_KP_2:	  xim_expected_char = '2';  bha_nga;
	    pak_sa GDK_KP_3:	  xim_expected_char = '3';  bha_nga;
	    pak_sa GDK_KP_4:	  xim_expected_char = '4';  bha_nga;
	    pak_sa GDK_KP_5:	  xim_expected_char = '5';  bha_nga;
	    pak_sa GDK_KP_6:	  xim_expected_char = '6';  bha_nga;
	    pak_sa GDK_KP_7:	  xim_expected_char = '7';  bha_nga;
	    pak_sa GDK_KP_8:	  xim_expected_char = '8';  bha_nga;
	    pak_sa GDK_KP_9:	  xim_expected_char = '9';  bha_nga;
	    pak_sa GDK_space:	  xim_expected_char = ' ';  bha_nga;
	    default:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    ຖ້າ (xic != NULL && !p_imdisable
		    && (State & (MODE_INSERT | MODE_CMDLINE
					      | MODE_NORMAL | MODE_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	ຖ້າ (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    unsigned ຈຳນວນເຕັມ state_mask;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_mask  = im_activatekey_state;
	    state_mask |= ((ຈຳນວນເຕັມ)GDK_SHIFT_MASK | (ຈຳນວນເຕັມ)GDK_CONTROL_MASK
							| (ຈຳນວນເຕັມ)GDK_MOD1_MASK);

	    ຖ້າ ((event->state & state_mask) != im_activatekey_state)
		ສົ່ງຄືນ FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    ຖ້າ (event->type != GDK_KEY_PRESS)
		ສົ່ງຄືນ TRUE;

	    ຖ້າ (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
	    {
		im_set_active(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= MODE_LANGMAP;
		ຖ້າ (State & MODE_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~MODE_LANGMAP;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= MODE_LANGMAP;
		}
		ສົ່ງຄືນ TRUE;
	    }

	    ສົ່ງຄືນ gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	ຖ້າ (im_activatekey_keyval == GDK_VoidSymbol || im_is_active)
	{
	    ຈຳນວນເຕັມ imresult = gtk_im_context_filter_keypress(xic, event);

	    ຖ້າ (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		ຖ້າ (!imresult && preedit_start_col != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    ສົ່ງຄືນ FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    ຖ້າ (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		ສົ່ງຄືນ FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    ສົ່ງຄືນ imresult;
	}
    }

    ສົ່ງຄືນ FALSE;
}
#  ບໍ່ດັ່ງນັ້ນ // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    ຈຳນວນເຕັມ
im_xim_isvalid_imactivate(void)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    ສົ່ງຄືນ p_imak[0] == NUL;
}

    ຈຳນວນເຕັມ
xim_queue_key_press_event(GdkEvent *event UNUSED, ຈຳນວນເຕັມ down UNUSED)
{
    ສົ່ງຄືນ FALSE;
}
#  endif // !USE_GTK4

    ຈຳນວນເຕັມ
im_get_status(void)
{
#  ifdef FEAT_EVAL
    ຖ້າ (USE_IMSTATUSFUNC)
	ສົ່ງຄືນ call_imstatusfunc();
#  endif
    ສົ່ງຄືນ im_is_active;
}

    ຈຳນວນເຕັມ
preedit_get_status(void)
{
    ສົ່ງຄືນ preedit_is_active;
}

    ຈຳນວນເຕັມ
im_is_preediting(void)
{
    ສົ່ງຄືນ xim_has_preediting;
}

# ບໍ່ດັ່ງນັ້ນ // !FEAT_GUI_GTK

static ຈຳນວນເຕັມ	xim_is_active = FALSE;  // XIM should be active in the current
					// mode
static ຈຳນວນເຕັມ	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
static XIMStyle	input_style;
static ຈຳນວນເຕັມ	status_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    void
im_set_active(ຈຳນວນເຕັມ active_arg)
{
    ຈຳນວນເຕັມ active = active_arg;

    // If 'imdisable' is set, XIM is never active.
    ຖ້າ (p_imdisable)
	active = FALSE;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (input_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	active = TRUE;

#  ຖ້າ defined(FEAT_EVAL)
    ຖ້າ (USE_IMACTIVATEFUNC)
    {
	ຖ້າ (active != im_get_status())
	{
	    call_imactivatefunc(active);
	    xim_has_focus = active;
	}
	ສົ່ງຄືນ;
    }
#  endif

    ຖ້າ (xic == NULL)
	ສົ່ງຄືນ;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_active = active;
    xim_set_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    void
xim_set_focus(ຈຳນວນເຕັມ focus)
{
    ຖ້າ (xic == NULL)
	ສົ່ງຄືນ;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    ຖ້າ (focus && xim_is_active)
    {
	ຖ້າ (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    void
im_set_position(ຈຳນວນເຕັມ row UNUSED, ຈຳນວນເຕັມ col UNUSED)
{
    xim_set_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    void
xim_set_preedit(void)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    ຈຳນວນເຕັມ line_space;

    ຖ້າ (xic == NULL)
	ສົ່ງຄືນ;

    xim_set_focus(TRUE);

    ຖ້າ (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							NULL);
	XSetICValues(xic, XNPreeditAttributes, attr_list, NULL);
	XFree(attr_list);
	ສົ່ງຄືນ;
    }

    ຖ້າ (input_style & XIMPreeditPosition)
    {
	ຖ້າ (xim_fg_color == INVALCOLOR)
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
	line_space = gui.char_height;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, line_space,
					NULL);
	ຖ້າ (XSetICValues(xic, XNPreeditAttributes, attr_list, NULL))
	    emsg(_(e_cannot_set_ic_values));
	XFree(attr_list);
    }
}

#  ຖ້າ defined(FEAT_GUI_X11)
#   ຖ້າ defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

static ຈຳນວນເຕັມ xim_real_init(Window x11_window, Display *x11_display);


#   ifdef USE_X11R6_XIM
    static void
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
    ຖ້າ (display != x11_display)
	ສົ່ງຄືນ;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    ຖ້າ (xic != NULL)
	XUnregisterIMInstantiateCallback(x11_display, NULL, NULL, NULL,
					 xim_instantiate_cb, NULL);
}

    static void
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

    xic = NULL;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, NULL, NULL, NULL,
				   xim_instantiate_cb, NULL);
}
#   endif

    void
xim_init(void)
{
    Window	x11_window;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = NULL;

    ຖ້າ (xim_real_init(x11_window, x11_display))
	ສົ່ງຄືນ;

    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, NULL, NULL, NULL,
				   xim_instantiate_cb, NULL);
#   endif
}

    static ຈຳນວນເຕັມ
xim_real_init(Window x11_window, Display *x11_display)
{
    ຈຳນວນເຕັມ		i;
    var_na	*p,
		*s,
		*ns,
		*end,
		tmp[1024];
#   define IMLEN_MAX 40
    var_na	buf[IMLEN_MAX + 7];
    XIM		xim = NULL;
    XIMStyles	*xim_styles;
    XIMStyle	this_input_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    input_style = 0;
    status_area_enabled = FALSE;

    ຖ້າ (xic != NULL)
	ສົ່ງຄືນ FALSE;

    ຖ້າ (gui.rsrc_input_method != NULL && *gui.rsrc_input_method != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_input_method,
	    parimaa_na(tmp) - 1);
	ສຳລັບ (ns = s = tmp; ns != NULL && *s != NUL;)
	{
	    s = (var_na *)skipwhite((char_u *)s);
	    ຖ້າ (*s == NUL)
		bha_nga;
	    ຖ້າ ((ns = end = strchr(s, ',')) == NULL)
		end = s + strlen(s);
	    ໃນຂະນະທີ່ (SAFE_isspace(end[-1]))
		end--;
	    *end = NUL;

	    ຖ້າ (strlen(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		ຖ້າ ((p = XSetLocaleModifiers(buf)) != NULL && *p != NUL
			&& (xim = XOpenIM(x11_display, NULL, NULL, NULL))
								      != NULL)
		    bha_nga;
	    }

	    s = ns + 1;
	}
    }

    ຖ້າ (xim == NULL && (p = XSetLocaleModifiers("")) != NULL && *p != NUL)
	xim = XOpenIM(x11_display, NULL, NULL, NULL);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    ຖ້າ (xim == NULL && (p = XSetLocaleModifiers("@im=none")) != NULL
								 && *p != NUL)
	xim = XOpenIM(x11_display, NULL, NULL, NULL);

    ຖ້າ (xim == NULL)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ຖ້າ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_to_open_input_method));
	    verbose_leave();
	}
	ສົ່ງຄືນ FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_data = NULL;
	ຖ້າ (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, NULL))
	    emsg(_(e_warning_could_not_set_destroy_callback_to_im));
    }
#   endif

    ຖ້າ (XGetIMValues(xim, XNQueryInputStyle, &xim_styles, NULL) || !xim_styles)
    {
	emsg(_(e_input_method_doesnt_support_any_style));
	XCloseIM(xim);
	ສົ່ງຄືນ FALSE;
    }

    found = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    parimaa_na(tmp) - 1);
    ສຳລັບ (s = tmp; s && !found; )
    {
	ໃນຂະນະທີ່ (*s && SAFE_isspace(*s))
	    s++;
	ຖ້າ (!*s)
	    bha_nga;
	ຖ້າ ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	ບໍ່ດັ່ງນັ້ນ
	    end = s + strlen(s);
	ໃນຂະນະທີ່ (SAFE_isspace(*end))
	    end--;
	*end = '\0';

	ຖ້າ (!strcmp(s, "OverTheSpot"))
	    this_input_style = (XIMPreeditPosition | XIMStatusArea);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (!strcmp(s, "OffTheSpot"))
	    this_input_style = (XIMPreeditArea | XIMStatusArea);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (!strcmp(s, "Root"))
	    this_input_style = (XIMPreeditNothing | XIMStatusNothing);

	ສຳລັບ (i = 0; (unsigned short)i < xim_styles->count_styles; i++)
	{
	    ຖ້າ (this_input_style == xim_styles->supported_styles[i])
	    {
		found = True;
		bha_nga;
	    }
	}
	ຖ້າ (!found)
	    ສຳລັບ (i = 0; (unsigned short)i < xim_styles->count_styles; i++)
	    {
		ຖ້າ ((xim_styles->supported_styles[i] & this_input_style)
			== (this_input_style & ~XIMStatusArea))
		{
		    this_input_style &= ~XIMStatusArea;
		    found = True;
		    bha_nga;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    ຖ້າ (!found)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	ຖ້າ (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_input_method_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	ສົ່ງຄືນ FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    input_style = this_input_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    ຖ້າ (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				NULL);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				XNFontSet, (XFontSet)gui.fontset,
				NULL);
    }
    ບໍ່ດັ່ງນັ້ນ
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				NULL);
	status_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_back_pixel,
				NULL);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, input_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    NULL);
    XFree(status_list);
    XFree(preedit_list);
    ຖ້າ (xic != NULL)
    {
	ຖ້າ (input_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	ບໍ່ດັ່ງນັ້ນ
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (!is_not_a_term())
	    emsg(_(e_failed_to_create_input_context));
	XCloseIM(xim);
	ສົ່ງຄືນ FALSE;
    }

    ສົ່ງຄືນ TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    ຈຳນວນເຕັມ
im_get_status(void)
{
#  ifdef FEAT_EVAL
    ຖ້າ (USE_IMSTATUSFUNC)
	ສົ່ງຄືນ call_imstatusfunc();
#  endif
    ສົ່ງຄືນ xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# ຖ້າ !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    void
xim_set_status_area(void)
{
    XVaNestedList preedit_list = 0, status_list = 0, list = 0;
    XRectangle pre_area, status_area;

    ຖ້າ (xic == NULL)
	ສົ່ງຄືນ;

    ຖ້າ (input_style & XIMStatusArea)
    {
	ຖ້າ (input_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    status_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, NULL);
	    XGetICValues(xic, XNStatusAttributes, status_list, NULL);
	    XFree(status_list);

	    status_area.width = needed_rect->width;
	}
	ບໍ່ດັ່ງນັ້ນ
	    status_area.width = gui.char_width * Columns;

	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	ຖ້າ (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	ຖ້າ (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.height = gui.char_height;
	status_list = XVaCreateNestedList(0, XNArea, &status_area, NULL);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	status_area.x = 0;
	status_area.y = gui.char_height * Rows + gui.border_offset;
	ຖ້າ (gui.which_scrollbars[SBAR_BOTTOM])
	    status_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	ຖ້າ (gui.menu_is_active)
	    status_area.y += gui.menu_height;
#  endif
	status_area.width = 0;
	status_area.height = gui.char_height;
    }

    ຖ້າ (input_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = status_area.x + status_area.width;
	pre_area.y = gui.char_height * Rows + gui.border_offset;
	pre_area.width = gui.char_width * Columns - pre_area.x;
	ຖ້າ (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_height;
#  ifdef FEAT_MENU
	ຖ້າ (gui.menu_is_active)
	    pre_area.y += gui.menu_height;
#  endif
	pre_area.height = gui.char_height;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, NULL);
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (input_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.height = gui.char_height * Rows;
	pre_area.width = gui.char_width * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, NULL);
    }

    ຖ້າ (preedit_list && status_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   XNStatusAttributes, status_list, NULL);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (preedit_list)
	list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				   NULL);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (status_list)
	list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				   NULL);
    ບໍ່ດັ່ງນັ້ນ
	list = NULL;

    ຖ້າ (list)
    {
	XSetICValues(xic, XNVaNestedList, list, NULL);
	XFree(list);
    }
    ຖ້າ (status_list)
	XFree(status_list);
    ຖ້າ (preedit_list)
	XFree(preedit_list);
}

    ຈຳນວນເຕັມ
xim_get_status_area_height(void)
{
    ຖ້າ (status_area_enabled)
	ສົ່ງຄືນ gui.char_height;
    ສົ່ງຄືນ 0;
}
# endif

#ບໍ່ດັ່ງນັ້ນ // !defined(FEAT_XIM)

# ຖ້າ defined(IME_WITHOUT_XIM) || defined(VIMDLL)
static ຈຳນວນເຕັມ im_was_set_active = FALSE;

    ຈຳນວນເຕັມ
#  ifdef VIMDLL
mbyte_im_get_status(void)
#  ບໍ່ດັ່ງນັ້ນ
im_get_status(void)
#  endif
{
#  ຖ້າ defined(FEAT_EVAL)
    ຖ້າ (USE_IMSTATUSFUNC)
	ສົ່ງຄືນ call_imstatusfunc();
#  endif
    ສົ່ງຄືນ im_was_set_active;
}

    void
#  ifdef VIMDLL
mbyte_im_set_active(ຈຳນວນເຕັມ active_arg)
#  ບໍ່ດັ່ງນັ້ນ
im_set_active(ຈຳນວນເຕັມ active_arg)
#  endif
{
#  ຖ້າ defined(FEAT_EVAL)
    ຈຳນວນເຕັມ	    active = !p_imdisable && active_arg;

    ຖ້າ (USE_IMACTIVATEFUNC && active != im_get_status())
    {
	call_imactivatefunc(active);
	im_was_set_active = active;
    }
#  endif
}

#  ຖ້າ defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    void
im_set_position(ຈຳນວນເຕັມ row UNUSED, ຈຳນວນເຕັມ col UNUSED)
{
}
#  endif
# endif

#endif // FEAT_XIM
