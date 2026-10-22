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

#ضمن "vim.h"

#إذا !defined(GTK_CHECK_نسخو)
# define GTK_CHECK_نسخو(a, b, c) 0
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_GTK) && defined(FEAT_XIM)
# ifdef USE_GTK4
#  include <gdk/gdkkeysyms.h>
# elif GTK_CHECK_نسخو(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# وإلا
#  include <gdk/gdkkeysyms.h>
# endif
# إذا !defined(USE_GTK4)
#  ifdef MSWIN
#   include <gdk/gdkwin32.h>
#  وإلا
#   include <gdk/gdkx.h>
#  endif
# endif
#جيڪڏهن_پورو

/*
 * XIM often causes trouble.  Define XIM_DEBUG to get a log of XIM callbacks
 * in the "xim.log" file.
 */
// #define XIM_DEBUG
#إذا defined(XIM_DEBUG) && defined(FEAT_GUI_GTK)
ساكن فراغ xim_log(حرف *s, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);

    ساكن فراغ
xim_log(حرف *s, ...)
{
    va_list arglist;
    ساكن FILE *fd = عدم;

    إذا (fd == (FILE *)-1)
	أرجع;
    إذا (fd == عدم)
    {
	fd = mch_fopen("xim.log", "w");
	إذا (fd == عدم)
	{
	    emsg("Cannot open xim.log");
	    fd = (FILE *)-1;
	    أرجع;
	}
    }

    va_start(arglist, s);
    vfprintf(fd, s, arglist);
    va_end(arglist);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_MSWIN)
# define USE_IMACTIVATEFUNC (!gui.اندريون_use && *p_imaf != NUL)
# define USE_IMSTATUSFUNC (!gui.اندريون_use && *p_imsf != NUL)
#وإلا
# define USE_IMACTIVATEFUNC (*p_imaf != NUL)
# define USE_IMSTATUSFUNC (*p_imsf != NUL)
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL) && \
     (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
ساكن callback_T imaf_cb;	    // 'imactivatefunc' callback function
ساكن callback_T imsf_cb;	    // 'imstatusfunc' callback function

    حرف *
did_سيٽ_imactivatefunc(optset_T *دليلون اڻ_استعمال_ٿيل)
{
    إذا (option_سيٽ_callback_func(p_imaf, &imaf_cb) == FAIL)
	أرجع e_invalid_دليل;

    أرجع عدم;
}

    حرف *
did_سيٽ_imstatusfunc(optset_T *دليلون اڻ_استعمال_ٿيل)
{
    إذا (option_سيٽ_callback_func(p_imsf, &imsf_cb) == FAIL)
	أرجع e_invalid_دليل;

    أرجع عدم;
}

    ساكن فراغ
call_imactivatefunc(صحيح سرگرم)
{
    typval_T argv[2];
    صحيح save_ڪنجي_Typed = ڪنجي_Typed;

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = سرگرم ? 1 : 0;
    argv[1].v_type = VAR_UNKNOWN;
    (فراغ)call_callback_retnr(&imaf_cb, 1, argv);

    ڪنجي_Typed = save_ڪنجي_Typed;
}

    ساكن صحيح
call_imstatusfunc(فراغ)
{
    صحيح is_سرگرم;
    صحيح save_ڪنجي_Typed = ڪنجي_Typed;

    // FIXME: Don't execute user function in unsafe situation.
    إذا (exiting || is_autocmd_blocked())
	أرجع FALSE;
    // FIXME: :py print 'xxx' is shown duplicate result.
    // Use silent to avoid it.
    ++نياپو_silent;
    is_سرگرم = call_callback_retnr(&imsf_cb, 0, عدم);
    --نياپو_silent;

    ڪنجي_Typed = save_ڪنجي_Typed;
    أرجع (is_سرگرم > 0);
}
#جيڪڏهن_پورو

#إذا defined(EXITFREE)
    فراغ
free_xim_stuff(فراغ)
{
# إذا defined(FEAT_EVAL) && \
    (defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL))
    free_callback(&imaf_cb);
    free_callback(&imsf_cb);
# endif
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * Mark the global 'imactivatefunc' and 'imstatusfunc' callbacks with "copyID"
 * so that they are not garbage collected.
 */
    صحيح
سيٽ_ref_اندريون_im_funcs(صحيح copyID اڻ_استعمال_ٿيل)
{
    صحيح abort = FALSE;

# إذا defined(FEAT_XIM) || defined(IME_WITHOUT_XIM) || defined(VIMDLL)
    abort = سيٽ_ref_اندريون_callback(&imaf_cb, copyID);
    abort = abort || سيٽ_ref_اندريون_callback(&imsf_cb, copyID);
# endif

    أرجع abort;
}
#جيڪڏهن_پورو


#إذا defined(FEAT_XIM)

# إذا defined(FEAT_GUI_GTK)
ساكن صحيح xim_has_preediting = FALSE;  // IM current status

/*
 * Set preedit_start_col to the current cursor position.
 */
    ساكن فراغ
init_preedit_شروعات_کانو(فراغ)
{
    إذا (State & گهڻائي_CMDLINE)
	preedit_شروعات_کانو = cmdline_getvcol_cursor();
    وإلا إذا (curwin != عدم && curwin->w_بفر != عدم)
	getvcol(curwin, &curwin->w_cursor, &preedit_شروعات_کانو, عدم, عدم, 0);
    // Prevent that preediting marks the buffer as changed.
    xim_بدليو_while_preediting = curbuf->b_بدليو;
}

ساكن صحيح im_is_سرگرم	       = FALSE;	// IM is enabled for current mode
ساكن صحيح preedit_is_سرگرم   = FALSE;
ساكن صحيح im_preedit_cursor   = 0;	// cursor offset in characters
ساكن صحيح im_preedit_trailing = 0;	// number of characters after cursor

ساكن بنانشان ڊگهو im_commit_handler_id  = 0;
#  ifndef USE_GTK4
#   ifndef GDK_ڪنجي_Void_نشان
#    define GDK_ڪنجي_Void_نشان GDK_Void_نشان
#   endif
ساكن بنانشان صحيح  im_activatekey_keyval = GDK_ڪنجي_Void_نشان;
ساكن بنانشان صحيح  im_activatekey_state  = 0;
#  endif

ساكن GtkWidget *preedit_دري = عدم;
ساكن GtkWidget *preedit_label = عدم;
#  ifdef USE_GTK4
ساكن صحيح preedit_label_ويڪر = 0;	// last measured popover natural width
ساكن صحيح preedit_popover_اوچائي = 0;	// last measured popover natural height
// CSS provider that styles the preedit popover frame; rebuilt whenever
// gui.back_pixel changes so the popover background matches the editor's
// background colour and fully covers the cursor cell.
ساكن GtkCssProvider *preedit_css_provider = عدم;
ساكن guicolor_T preedit_css_cached_bg = INVALCOLOR;
#  endif

ساكن فراغ im_preedit_دري_سيٽ_position(فراغ);

    فراغ
im_سيٽ_سرگرم(صحيح سرگرم)
{
    صحيح was_سرگرم;

    was_سرگرم = !!im_get_حالت();
    im_is_سرگرم = (سرگرم && !p_imdisable);

    إذا (im_is_سرگرم != was_سرگرم)
	xim_reset();
}

    فراغ
xim_سيٽ_focus(صحيح focus)
{
    إذا (xic == عدم)
	أرجع;

    إذا (focus)
	gtk_im_context_focus_اندريون(xic);
    وإلا
	gtk_im_context_focus_ٻاهريون(xic);
}

    فراغ
im_سيٽ_position(صحيح صف, صحيح کانو)
{
    إذا (xic == عدم)
	أرجع;

    GdkRectangle area;

    area.x = FILL_X(کانو);
    area.y = FILL_Y(صف);
    // The screen buffer may not be allocated yet when this is called from
    // xim_init() during gui_mch_init() to seed the IM's initial cursor
    // location, so guard mb_lefthalve() against a NULL LineOffset[].
    area.ويڪر  = gui.char_ويڪر
		    * ((Screen_سٽون != عدم && mb_lefthalve(صف, کانو)) ? 2 : 1);
    area.اوچائي = gui.char_اوچائي;

#  ifdef USE_GTK4
    // Stretch the cursor rect height to cover the preedit popover so the
    // IM's candidate window is placed below the popover rather than on
    // top of it.  Coordinates stay in client-widget (drawarea) space;
    // GTK4's IM module translates to the surface internally.
    إذا (p_imst == IM_OVER_THE_SPOT)
    {
	صحيح popover_h = preedit_popover_اوچائي;
	إذا (popover_h <= 0)
	    popover_h = gui.char_اوچائي;
	إذا (popover_h > area.اوچائي)
	    area.اوچائي = popover_h;
    }
#  endif

    gtk_im_context_سيٽ_cursor_location(xic, &area);

    إذا (p_imst == IM_OVER_THE_SPOT)
	im_preedit_دري_سيٽ_position();
}

#  إذا 0 || defined(PROTO) // apparently only used in gui_x11.c
    فراغ
xim_سيٽ_preedit(فراغ)
{
    im_سيٽ_position(gui.صف, gui.کانو);
}
#  endif

    ساكن فراغ
im_جوڙيو_منزل_داخلا(char_u *str, صحيح len)
{
    // Convert from 'termencoding' (always "utf-8") to 'encoding'
    إذا (داخلا_conv.vc_type != CONV_NONE)
    {
	str = string_convert(&داخلا_conv, str, &len);
	g_return_if_fail(str != عدم);
    }

    جوڙيو_منزل_داخلا_buf_csi(str, len);

    إذا (داخلا_conv.vc_type != CONV_NONE)
	vim_free(str);

    إذا (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ساكن فراغ
im_preedit_دري_سيٽ_position(فراغ)
{
    إذا (preedit_دري == عدم)
	أرجع;

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
    graphene_نقطو_t pt_اندريون, pt_ٻاهريون;

    pt_اندريون.x = FILL_X(gui.کانو);
    pt_اندريون.y = FILL_Y(gui.صف);
    إذا (!gtk_widget_compute_نقطو(gui.drawarea, gui.mainwin,
						&pt_اندريون, &pt_ٻاهريون))
    {
	pt_ٻاهريون.x = pt_اندريون.x;
	pt_ٻاهريون.y = pt_اندريون.y;
    }
    rect.x = (صحيح)pt_ٻاهريون.x;
    rect.y = (صحيح)pt_ٻاهريون.y;
    rect.ويڪر = preedit_label_ويڪر;
    rect.اوچائي = 0;
    // make up for the difference between GVim's line height and GtkLabel's line height
    صحيح وٿي = gui.char_اوچائي - preedit_popover_اوچائي;
    gtk_popover_سيٽ_وٿي(GTK_POPOVER(preedit_دري), 0, وٿي);
    // make the arrow pointer at start so that it only starts sliding
    // horizontally when screen border is reached
    gtk_widget_سيٽ_halign(preedit_دري, GTK_ALIGN_شروعات);
    gtk_popover_سيٽ_pointing_منزل(GTK_POPOVER(preedit_دري), &rect);
#  وإلا
    صحيح x, y, ويڪر, اوچائي;
    صحيح screen_x, screen_y, screen_ويڪر, screen_اوچائي;

    gui_gtk_get_screen_geom_جو_win(gui.drawarea, 0, 0,
			  &screen_x, &screen_y, &screen_ويڪر, &screen_اوچائي);
    gdk_دري_get_origin(gtk_widget_get_دري(gui.drawarea), &x, &y);
    gtk_دري_get_سائيز(GTK_دري(preedit_دري), &ويڪر, &اوچائي);
    x = x + FILL_X(gui.کانو);
    y = y + FILL_Y(gui.صف);
    إذا (x + ويڪر > screen_x + screen_ويڪر)
	x = screen_x + screen_ويڪر - ويڪر;
    إذا (y + اوچائي > screen_y + screen_اوچائي)
	y = screen_y + screen_اوچائي - اوچائي;
    gtk_دري_move(GTK_دري(preedit_دري), x, y);
#  endif
}

    ساكن فراغ
im_preedit_دري_open(فراغ)
{
    حرف *preedit_string;
#  إذا !GTK_CHECK_نسخو(3,16,0)
    حرف buf[8];
#  endif
    PangoAttrList *attr_list;
    PangoLayout *layout;
#  إذا GTK_CHECK_نسخو(3,0,0)
#   إذا !GTK_CHECK_نسخو(3,16,0)
    GdkRGBA color;
#   endif
#  وإلا
    GdkColor color;
#  endif
    gint w, h;

    إذا (preedit_دري == عدم)
    {
#  ifdef USE_GTK4
	// GTK4: use GtkPopover (xdg_popup) so the compositor keeps keyboard
	// focus on the drawing area and does not disable text-input-v3.
	// See issue #20257.  Parent the popover to gui.mainwin (a real
	// container) rather than gui.drawarea: GtkDrawingArea is a leaf
	// widget and using it as a popover parent causes snapshot artifacts
	// (white-out) on the drawing area.
	preedit_دري = gtk_popover_new();
	gtk_popover_سيٽ_autohide(GTK_POPOVER(preedit_دري), FALSE);
	gtk_popover_سيٽ_has_arrow(GTK_POPOVER(preedit_دري), FALSE);
	// Pin the popover below the anchor.  Together with anchor.h == 0 in
	// im_preedit_window_set_position() this lands the popover's top edge
	// exactly on FILL_Y(row), so the popover covers the cursor cell rather
	// than landing above it (the default GTK_POS_TOP would do that and
	// only flip to bottom when there is no room above, which is flaky).
	gtk_popover_سيٽ_position(GTK_POPOVER(preedit_دري), GTK_POS_BOTTOM);
	gtk_widget_سيٽ_can_focus(preedit_دري, FALSE);
	gtk_widget_سيٽ_focusable(preedit_دري, FALSE);
	gtk_widget_سيٽ_can_هدف(preedit_دري, FALSE);
	gtk_widget_سيٽ_والد(preedit_دري, gui.mainwin);
#  وإلا
	preedit_دري = gtk_دري_new(GTK_دري_POPUP);
	gtk_دري_سيٽ_transient_for(GTK_دري(preedit_دري),
						     GTK_دري(gui.mainwin));
#  endif
	preedit_label = gtk_label_new("");
	gtk_widget_سيٽ_name(preedit_label, "vim-gui-preedit-area");
#  ifdef USE_GTK4
	gtk_label_سيٽ_xalign(GTK_LABEL(preedit_label), 0.0);
	gtk_widget_سيٽ_halign(preedit_label, GTK_ALIGN_شروعات);
	gtk_widget_سيٽ_valign(preedit_label, GTK_ALIGN_شروعات);
	// Tag the popover with a CSS class so the minimal frame CSS below
	// (no border, no rounded corners, no shadow, no padding) applies
	// only to our preedit popover and not other popovers in the app.
	gtk_widget_جوڙيو_css_class(preedit_دري, "vim-preedit-popover");
	gtk_popover_سيٽ_ٻار(GTK_POPOVER(preedit_دري), preedit_label);
#  وإلا
	gtk_container_جوڙيو(GTK_CONTAINER(preedit_دري), preedit_label);
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
    إذا (preedit_css_provider == عدم || preedit_css_cached_bg != gui.پٺ_pixel)
    {
	GdkDisplay *display = gdk_display_get_default();
	gchar *css;

	إذا (preedit_css_provider != عدم)
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
		(بنانشان ڊگهو)((gui.پٺ_pixel >> 16) & 0xff),
		(بنانشان ڊگهو)((gui.پٺ_pixel >> 8) & 0xff),
		(بنانشان ڊگهو)(gui.پٺ_pixel & 0xff));
	gtk_css_provider_ڀريو_from_string(preedit_css_provider, css);
	g_free(css);
	gtk_style_context_جوڙيو_provider_for_display(display,
			    GTK_STYLE_PROVIDER(preedit_css_provider), G_MAXUINT);
	preedit_css_cached_bg = gui.پٺ_pixel;
    }
#  endif

#  ifndef USE_GTK4
#   إذا GTK_CHECK_نسخو(3,16,0)
    {
	GtkStyleContext * ثابت context
				  = gtk_widget_get_style_context(preedit_label);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar		   *css = عدم;
	ثابت حرف * ثابت fontname
			   = pango_font_description_get_family(gui.norm_font);
	// Since font sizes can be specified as non-integer values like 10.5,
	// they must be handled as floating-point (gdouble).
	gdouble	fontsize
		= (gdouble)pango_font_description_get_سائيز(gui.norm_font) / PANGO_SCALE;
	gchar	*fontsize_propval = عدم;

	إذا (!pango_font_description_get_سائيز_is_absolute(gui.norm_font))
	{
	    // fontsize was given in points.  Convert it into that in pixels
	    // to use with CSS.
	    GdkScreen * ثابت screen
		  = gdk_دري_get_screen(gtk_widget_get_دري(gui.mainwin));
	    ثابت gdouble dpi = gdk_screen_get_resolution(screen);
	    fontsize = dpi * fontsize / 72;
	}
	إذا (fontsize > 0)
	    fontsize_propval = g_strdup_printf("%dpx", (gint)(fontsize + 0.5));
	وإلا
	    fontsize_propval = g_strdup_printf("inherit");

	css = g_strdup_printf(
		"#vim-gui-preedit-area {\n"
		"  font-family: %s,monospace;\n"
		"  font-size: %s;\n"
		"  color: #%.2lx%.2lx%.2lx;\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		fontname != عدم ? fontname : "inherit",
		fontsize_propval,
		(gui.norm_pixel >> 16) & 0xff,
		(gui.norm_pixel >> 8) & 0xff,
		gui.norm_pixel & 0xff,
		(gui.پٺ_pixel >> 16) & 0xff,
		(gui.پٺ_pixel >> 8) & 0xff,
		gui.پٺ_pixel & 0xff);

	gtk_css_provider_ڀريو_from_معلومات(provider, css, -1, عدم);
	gtk_style_context_جوڙيو_provider(context,
				     GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_free(fontsize_propval);
	g_object_unref(provider);
    }
#   elif GTK_CHECK_نسخو(3,0,0)
    gtk_widget_override_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, حجم(buf), "#%06X", gui.norm_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_color(preedit_label, GTK_STATE_FLAG_عام, &color);

    vim_snprintf(buf, حجم(buf), "#%06X", gui.پٺ_pixel);
    gdk_rgba_parse(&color, buf);
    gtk_widget_override_background_color(preedit_label, GTK_STATE_FLAG_عام,
								      &color);
#   وإلا
    gtk_widget_modify_font(preedit_label, gui.norm_font);

    vim_snprintf(buf, حجم(buf), "#%06X", (بنانشان)gui.norm_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_fg(preedit_label, GTK_STATE_عام, &color);

    vim_snprintf(buf, حجم(buf), "#%06X", (بنانشان)gui.پٺ_pixel);
    gdk_color_parse(buf, &color);
    gtk_widget_modify_bg(preedit_دري, GTK_STATE_عام, &color);
#   endif
#  endif // !USE_GTK4

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, عدم);

    إذا (preedit_string[0] != NUL)
    {
#  ifdef USE_GTK4
	// GTK4: drive all styling (font + foreground + background) via
	// PangoAttributes on the label rather than CSS.  This pins the
	// preedit text to gui.norm_font (family + weight + style + size)
	// and the editor colors without going through CSS DPI conversion
	// or selector matching.  These attributes are merged with whatever
	// the IM gave us (e.g. underline for composing range).
	PangoAttribute *pa;

	إذا (attr_list == عدم)
	    attr_list = pango_attr_list_new();

	إذا (gui.norm_font != عدم)
	{
	    pa = pango_attr_font_desc_new(gui.norm_font);
	    pa->شروعات_اشاريو = 0;
	    pa->پڇاڙي_اشاريو = G_MAXUINT;
	    pango_attr_list_وجهو(attr_list, pa);
	}

	pa = pango_attr_foreground_new(
		((gui.norm_pixel >> 16) & 0xff) * 257,
		((gui.norm_pixel >> 8) & 0xff) * 257,
		(gui.norm_pixel & 0xff) * 257);
	pa->شروعات_اشاريو = 0;
	pa->پڇاڙي_اشاريو = G_MAXUINT;
	pango_attr_list_وجهو(attr_list, pa);

	pa = pango_attr_background_new(
		((gui.پٺ_pixel >> 16) & 0xff) * 257,
		((gui.پٺ_pixel >> 8) & 0xff) * 257,
		(gui.پٺ_pixel & 0xff) * 257);
	pa->شروعات_اشاريو = 0;
	pa->پڇاڙي_اشاريو = G_MAXUINT;
	pango_attr_list_وجهو(attr_list, pa);
#  endif
	gtk_label_سيٽ_متن(GTK_LABEL(preedit_label), preedit_string);
	gtk_label_سيٽ_خاصيتون(GTK_LABEL(preedit_label), attr_list);

	layout = gtk_label_get_layout(GTK_LABEL(preedit_label));
	pango_layout_get_pixel_سائيز(layout, &w, &h);
	h = MAX(h, gui.char_اوچائي);
#  ifdef USE_GTK4
	// Cache label/popover size derived from the Pango layout, which is
	// available without mapping the popover.  Using these for positioning
	// avoids calling gtk_popover_set_pointing_to() after popup, which
	// triggers a GDK "compositor doesn't support moving popups" warning
	// on compositors without xdg_popup.reposition (e.g. Weston).
	preedit_label_ويڪر = w;
	preedit_popover_اوچائي = h;

	// Report an enlarged cursor rectangle that covers both the cursor
	// cell and the preedit popover.  This way the IM (e.g. fcitx5)
	// places its candidate window below the popover instead of on top
	// of it.  Coordinates are in client-widget (drawarea) space.
	إذا (xic != عدم)
	{
	    GdkRectangle area;

	    area.x = FILL_X(gui.کانو);
	    area.y = FILL_Y(gui.صف);
	    area.ويڪر = preedit_label_ويڪر;
	    area.اوچائي = MAX(gui.char_اوچائي, preedit_popover_اوچائي);
	    gtk_im_context_سيٽ_cursor_location(xic, &area);
	}

	// Pop down before re-anchoring: on compositors that lack
	// xdg_popup.reposition (e.g. Weston), calling set_pointing_to() on
	// a mapped popover triggers GDK's remap fallback and a warning.
	// Doing the popdown ourselves makes the next popup land at the new
	// anchor cleanly without that warning.
	إذا (gtk_widget_get_mapped(preedit_دري))
	    gtk_popover_popdown(GTK_POPOVER(preedit_دري));

	im_preedit_دري_سيٽ_position();

	gtk_widget_قطار_resize(preedit_دري);
	gtk_popover_popup(GTK_POPOVER(preedit_دري));
#  وإلا
	gtk_دري_resize(GTK_دري(preedit_دري), w, h);
	gtk_widget_show_all(preedit_دري);

	im_preedit_دري_سيٽ_position();
#  endif
    }

    g_free(preedit_string);
    pango_attr_list_unref(attr_list);
}

    ساكن فراغ
im_preedit_دري_close(فراغ)
{
    إذا (preedit_دري == عدم)
	أرجع;
#  ifdef USE_GTK4
    gtk_popover_popdown(GTK_POPOVER(preedit_دري));
#  وإلا
    gtk_widget_hide(preedit_دري);
#  endif
}

    ساكن فراغ
im_show_preedit(فراغ)
{
    im_preedit_دري_open();

    إذا (p_mh) // blank out the pointer if necessary
	gui_mch_mousehide(TRUE);
}

    ساكن فراغ
im_delete_preedit(فراغ)
{
    char_u bskey[]  = {CSI, 'k', 'b'};
    char_u delkey[] = {CSI, 'k', 'D'};

    إذا (p_imst == IM_OVER_THE_SPOT)
    {
	im_preedit_دري_close();
	أرجع;
    }

    إذا (State & گهڻائي_عام
#  ifdef FEAT_TERMINAL
	    && !رڪن_use_loop()
#  endif
       )
    {
	im_preedit_cursor = 0;
	أرجع;
    }
    لكل (; im_preedit_cursor > 0; --im_preedit_cursor)
	جوڙيو_منزل_داخلا_buf(bskey, (صحيح)حجم(bskey));

    لكل (; im_preedit_trailing > 0; --im_preedit_trailing)
	جوڙيو_منزل_داخلا_buf(delkey, (صحيح)حجم(delkey));
}

/*
 * Move the cursor left by "num_move_back" characters.
 * Note that ins_left() checks im_is_preediting() to avoid breaking undo for
 * these K_LEFT keys.
 */
    ساكن فراغ
im_correct_cursor(صحيح انگ_move_پٺ)
{
    char_u backkey[] = {CSI, 'k', 'l'};

    إذا (State & گهڻائي_عام)
	أرجع;
#  ifdef FEAT_RIGHTLEFT
    إذا ((State & گهڻائي_CMDLINE) == 0 && curwin != عدم && curwin->w_p_rl)
	backkey[2] = 'r';
#  endif
    لكل (; انگ_move_پٺ > 0; --انگ_move_پٺ)
	جوڙيو_منزل_داخلا_buf(backkey, (صحيح)حجم(backkey));
}

ساكن صحيح xim_expected_char = NUL;
ساكن صحيح xim_ignored_char = FALSE;

/*
 * Update the mode and cursor while in an IM callback.
 */
    ساكن فراغ
im_show_info(فراغ)
{
    صحيح	    old_vgetc_busy;

    old_vgetc_busy = vgetc_busy;
    vgetc_busy = TRUE;
    showmode();
    vgetc_busy = old_vgetc_busy;
    إذا ((State & گهڻائي_عام) || (State & گهڻائي_وجهو))
	setcursor();
    ٻاهريون_flush();
}

/*
 * Callback invoked when the user finished preediting.
 * Put the final string into the input buffer.
 */
    ساكن فراغ
im_commit_cb(GtkIMContext *context اڻ_استعمال_ٿيل,
	     ثابت gchar *str,
	     gpointer معلومات اڻ_استعمال_ٿيل)
{
    صحيح		slen = (صحيح)STRLEN(str);
    صحيح		جوڙيو_منزل_داخلا = TRUE;
    صحيح		clen;
    صحيح		len = slen;
    صحيح		commit_with_preedit = TRUE;
    char_u	*im_str;

#  ifdef XIM_DEBUG
    xim_log("im_commit_cb(): %s\n", str);
#  endif

    إذا (p_imst == IM_ON_THE_SPOT)
    {
	// The imhangul module doesn't reset the preedit string before
	// committing.  Call im_delete_preedit() to work around that.
	im_delete_preedit();

	// Indicate that preediting has finished.
	إذا (preedit_شروعات_کانو == MAXCOL)
	{
	    init_preedit_شروعات_کانو();
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
	إذا (داخلا_conv.vc_type != CONV_NONE)
	{
	    im_str = string_convert(&داخلا_conv, (char_u *)str, &len);
	    g_return_if_fail(im_str != عدم);
	}
	وإلا
	    im_str = (char_u *)str;

	clen = mb_string2cells(im_str, len);

	إذا (داخلا_conv.vc_type != CONV_NONE)
	    vim_free(im_str);
	preedit_شروعات_کانو += clen;
    }

    // Is this a single character that matches a keypad key that's just
    // been pressed?  If so, we don't want it to be entered as such - let
    // us carry on processing the raw keycode so that it may be used in
    // mappings as <kSomething>.
    إذا (xim_expected_char != NUL)
    {
	// We're currently processing a keypad or other special key
	إذا (slen == 1 && str[0] == xim_expected_char)
	{
	    // It's a match - don't do it here
	    xim_ignored_char = TRUE;
	    جوڙيو_منزل_داخلا = FALSE;
	}
	وإلا
	{
	    // Not a match
	    xim_ignored_char = FALSE;
	}
    }

    إذا (جوڙيو_منزل_داخلا)
	im_جوڙيو_منزل_داخلا((char_u *)str, slen);

    إذا (p_imst == IM_ON_THE_SPOT)
    {
	// Inserting chars while "im_is_active" is set does not cause a
	// change of buffer.  When the chars are committed the buffer must be
	// marked as changed.
	إذا (!commit_with_preedit)
	    preedit_شروعات_کانو = MAXCOL;

	// This flag is used in changed() at next call.
	xim_بدليو_while_preediting = TRUE;
    }

    إذا (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Callback invoked after start to the preedit.
 */
    ساكن فراغ
im_preedit_شروعات_cb(GtkIMContext *context اڻ_استعمال_ٿيل, gpointer معلومات اڻ_استعمال_ٿيل)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_start_cb()\n");
#  endif

    im_is_سرگرم = TRUE;
    preedit_is_سرگرم = TRUE;
    gui_update_cursor(TRUE, FALSE);
    im_show_info();
}

/*
 * Callback invoked after end to the preedit.
 */
    ساكن فراغ
im_preedit_پڇاڙي_cb(GtkIMContext *context اڻ_استعمال_ٿيل, gpointer معلومات اڻ_استعمال_ٿيل)
{
#  ifdef XIM_DEBUG
    xim_log("im_preedit_end_cb()\n");
#  endif
    im_delete_preedit();

    // Indicate that preediting has finished
    إذا (p_imst == IM_ON_THE_SPOT)
	preedit_شروعات_کانو = MAXCOL;
    xim_has_preediting = FALSE;

#  إذا 0
    // Removal of this line suggested by Takuhiro Nishioka.  Fixes that IM was
    // switched off unintentionally.  We now use preedit_is_active (added by
    // SungHyun Nam).
    im_is_سرگرم = FALSE;
#  endif
    preedit_is_سرگرم = FALSE;
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
    ساكن فراغ
im_preedit_بدليو_cb(GtkIMContext *context, gpointer معلومات اڻ_استعمال_ٿيل)
{
    حرف    *preedit_string = عدم;
    صحيح	    cursor_اشاريو    = 0;
    صحيح	    انگ_move_پٺ   = 0;
    char_u  *str;
    char_u  *p;
    صحيح	    i;

    إذا (p_imst == IM_ON_THE_SPOT)
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, عدم,
					  &cursor_اشاريو);
    وإلا
	gtk_im_context_get_preedit_string(context,
					  &preedit_string, عدم,
					  عدم);

#  ifdef XIM_DEBUG
    xim_log("im_preedit_changed_cb(): %s\n", preedit_string);
#  endif

    g_return_if_fail(preedit_string != عدم); // just in case

    إذا (p_imst == IM_OVER_THE_SPOT)
    {
	إذا (preedit_string[0] == NUL)
	{
	    xim_has_preediting = FALSE;
	    im_delete_preedit();
	}
	وإلا
	{
	    xim_has_preediting = TRUE;
	    im_show_preedit();
	}
    }
    وإلا
    {
	// If preedit_start_col is MAXCOL set it to the current cursor position.
	إذا (preedit_شروعات_کانو == MAXCOL && preedit_string[0] != '\0')
	{
	    xim_has_preediting = TRUE;

	    // Urgh, this breaks if the input buffer isn't empty now
	    init_preedit_شروعات_کانو();
	}
	وإلا إذا (cursor_اشاريو == 0 && preedit_string[0] == '\0')
	{
	    xim_has_preediting = FALSE;

	    // If at the start position (after typing backspace)
	    // preedit_start_col must be reset.
	    preedit_شروعات_کانو = MAXCOL;
	}

	im_delete_preedit();

	// Compute the end of the preediting area: "preedit_end_col".
	// According to the documentation of
	// gtk_im_context_get_preedit_string(), the cursor_pos output argument
	// returns the offset in bytes.  This is unfortunately not true -- real
	// life shows the offset is in characters, and the GTK+ source code
	// agrees with me.  Will file a bug later.
	إذا (preedit_شروعات_کانو != MAXCOL)
	    preedit_پڇاڙي_کانو = preedit_شروعات_کانو;
	str = (char_u *)preedit_string;
	لكل (p = str, i = 0; *p != NUL; p += utf_byte2len(*p), ++i)
	{
	    صحيح is_composing;

	    is_composing = ((*p & 0x80) != 0
					  && utf_iscomposing(utf_ptr2char(p)));
	    // These offsets are used as counters when generating <BS> and
	    // <Del> to delete the preedit string.  So don't count composing
	    // characters unless 'delcombine' is enabled.
	    إذا (!is_composing || p_deco)
	    {
		إذا (i < cursor_اشاريو)
		    ++im_preedit_cursor;
		وإلا
		    ++im_preedit_trailing;
	    }
	    إذا (!is_composing && i >= cursor_اشاريو)
	    {
		// This is essentially the same as im_preedit_trailing, except
		// composing characters are not counted even if p_deco is set.
		++انگ_move_پٺ;
	    }
	    إذا (preedit_شروعات_کانو != MAXCOL)
		preedit_پڇاڙي_کانو += utf_ptr2cells(p);
	}

	إذا (p > str)
	{
	    im_جوڙيو_منزل_داخلا(str, (صحيح)(p - str));
	    im_correct_cursor(انگ_move_پٺ);
	}
    }

    g_free(preedit_string);

    إذا (gtk_main_level() > 0)
	gtk_main_quit();
}

/*
 * Translate the Pango attributes at iter to Vim highlighting attributes.
 * Ignore attributes not supported by Vim highlighting.  This shouldn't have
 * too much impact -- right now we handle even more attributes than necessary
 * for the IM modules I tested with.
 */
    ساكن صحيح
translate_pango_خاصيتون(PangoAttrIterator *iter)
{
    PangoAttribute  *attr;
    صحيح		    char_attr = HL_عام;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
    إذا (attr != عدم && ((PangoAttrInt *)attr)->قدر
						 != (صحيح)PANGO_UNDERLINE_NONE)
	char_attr |= HL_UNDERLINE;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_وزن);
    إذا (attr != عدم && ((PangoAttrInt *)attr)->قدر >= (صحيح)PANGO_وزن_BOLD)
	char_attr |= HL_BOLD;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_STYLE);
    إذا (attr != عدم && ((PangoAttrInt *)attr)->قدر
						   != (صحيح)PANGO_STYLE_عام)
	char_attr |= HL_ITALIC;

    attr = pango_attr_iterator_get(iter, PANGO_ATTR_BACKGROUND);
    إذا (attr != عدم)
    {
	ثابت PangoColor *color = &((PangoAttrColor *)attr)->color;

	// Assume inverse if black background is requested
	إذا ((color->red | color->green | color->blue) == 0)
	    char_attr |= HL_INVERSE;
    }

    أرجع char_attr;
}

/*
 * Retrieve the highlighting attributes at column col in the preedit string.
 * Return -1 if not in preediting mode or if col is out of range.
 */
    صحيح
im_get_feedback_attr(صحيح کانو)
{
    حرف	    *preedit_string = عدم;
    PangoAttrList   *attr_list	    = عدم;
    صحيح		    char_attr	    = -1;

    إذا (xic == عدم)
	أرجع char_attr;

    gtk_im_context_get_preedit_string(xic, &preedit_string, &attr_list, عدم);

    إذا (preedit_string != عدم && attr_list != عدم)
    {
	صحيح idx;

	// Get the byte index as used by PangoAttrIterator
	لكل (idx = 0; کانو > 0 && preedit_string[idx] != '\0'; --کانو)
	    idx += utfc_ptr2len((char_u *)preedit_string + idx);

	إذا (preedit_string[idx] != '\0')
	{
	    PangoAttrIterator	*iter;
	    صحيح			شروعات, پڇاڙي;

	    char_attr = HL_عام;
	    iter = pango_attr_list_get_iterator(attr_list);

	    // Extract all relevant attributes from the list.
	    افعل
	    {
		pango_attr_iterator_range(iter, &شروعات, &پڇاڙي);

		إذا (idx >= شروعات && idx < پڇاڙي)
		    char_attr |= translate_pango_خاصيتون(iter);
	    }
	    طالما (pango_attr_iterator_ايندڙ(iter));

	    pango_attr_iterator_destroy(iter);
	}
    }

    إذا (attr_list != عدم)
	pango_attr_list_unref(attr_list);
    g_free(preedit_string);

    أرجع char_attr;
}

    فراغ
xim_init(فراغ)
{
#  ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#  endif

    g_return_if_fail(gui.drawarea != عدم);
#  ifndef USE_GTK4
    g_return_if_fail(gtk_widget_get_دري(gui.drawarea) != عدم);
#  endif

    xic = gtk_im_multicontext_new();

    im_commit_handler_id = g_signal_connect(G_OBJECT(xic), "commit",
					    G_CALLBACK(&im_commit_cb), عدم);
    g_signal_connect(G_OBJECT(xic), "preedit_changed",
		     G_CALLBACK(&im_preedit_بدليو_cb), عدم);
    g_signal_connect(G_OBJECT(xic), "preedit_start",
		     G_CALLBACK(&im_preedit_شروعات_cb), عدم);
    g_signal_connect(G_OBJECT(xic), "preedit_end",
		     G_CALLBACK(&im_preedit_پڇاڙي_cb), عدم);

#  ifdef USE_GTK4
    gtk_im_context_سيٽ_client_widget(xic, gui.drawarea);

    // Seed the cursor location immediately.  GTK4's Wayland IM module
    // batches text-input-v3 state and sends it on the next commit cycle
    // (typically triggered by focus_in).  Without this seed the very
    // first commit would carry the protocol default (0, 0, 0, 0) for the
    // cursor rectangle, and the IM's candidate window for the very first
    // composition would appear at the top-left of the surface instead of
    // next to the cursor.
    im_سيٽ_position(gui.صف, gui.کانو);
#  وإلا
    gtk_im_context_سيٽ_client_دري(xic, gtk_widget_get_دري(gui.drawarea));
#  endif
}

    فراغ
im_shutdown(فراغ)
{
#  ifdef XIM_DEBUG
    xim_log("im_shutdown()\n");
#  endif

    إذا (xic != عدم)
    {
	gtk_im_context_focus_ٻاهريون(xic);
	g_object_unref(xic);
	xic = عدم;
    }
#  ifdef USE_GTK4
    // GTK4: a widget added via gtk_widget_set_parent() must be detached
    // with gtk_widget_unparent() before its parent is finalized, otherwise
    // GTK prints "Finalizing ..., but it still has children left".
    إذا (preedit_دري != عدم)
    {
	gtk_widget_unparent(preedit_دري);
	preedit_دري = عدم;
	preedit_label = عدم;
    }
    إذا (preedit_css_provider != عدم)
    {
	gtk_style_context_remove_provider_for_display(gdk_display_get_default(),
				GTK_STYLE_PROVIDER(preedit_css_provider));
	g_object_unref(preedit_css_provider);
	preedit_css_provider = عدم;
	preedit_css_cached_bg = INVALCOLOR;
    }
#  endif
    im_is_سرگرم = FALSE;
    im_commit_handler_id = 0;
    إذا (p_imst == IM_ON_THE_SPOT)
	preedit_شروعات_کانو = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
/*
 * Convert the string argument to keyval and state for GdkEventKey.
 * If str is valid return TRUE, otherwise FALSE.
 *
 * See 'imactivatekey' for documentation of the format.
 */
    ساكن صحيح
im_string_منزل_keyval(ثابت حرف *str, بنانشان صحيح *keyval, بنانشان صحيح *state)
{
    ثابت حرف	    *mods_پڇاڙي;
    بنانشان	    tmp_keyval;
    بنانشان	    tmp_state = 0;

    mods_پڇاڙي = strrchr(str, '-');
    mods_پڇاڙي = (mods_پڇاڙي != عدم) ? mods_پڇاڙي + 1 : str;

    // Parse modifier keys
    طالما (str < mods_پڇاڙي)
	بدل (*str++)
	{
	    حالة '-':							اكسر;
	    حالة 'S': حالة 's': tmp_state |= (بنانشان)GDK_سرڪايو_نقاب;	اكسر;
	    حالة 'L': حالة 'l': tmp_state |= (بنانشان)GDK_تالو_نقاب;	اكسر;
	    حالة 'C': حالة 'c': tmp_state |= (بنانشان)GDK_CONTROL_نقاب;اكسر;
	    حالة '1':		tmp_state |= (بنانشان)GDK_MOD1_نقاب;	اكسر;
	    حالة '2':		tmp_state |= (بنانشان)GDK_MOD2_نقاب;	اكسر;
	    حالة '3':		tmp_state |= (بنانشان)GDK_MOD3_نقاب;	اكسر;
	    حالة '4':		tmp_state |= (بنانشان)GDK_MOD4_نقاب;	اكسر;
	    حالة '5':		tmp_state |= (بنانشان)GDK_MOD5_نقاب;	اكسر;
	    افتراضي:
		أرجع FALSE;
	}

    tmp_keyval = gdk_keyval_from_name(str);

    إذا (tmp_keyval == 0 || tmp_keyval == GDK_Void_نشان)
	أرجع FALSE;

    إذا (keyval != عدم)
	*keyval = tmp_keyval;
    إذا (state != عدم)
	*state = tmp_state;

    أرجع TRUE;
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
    صحيح
im_xim_isvalid_imactivate(فراغ)
{
    إذا (p_imak[0] == NUL)
    {
	im_activatekey_keyval = GDK_Void_نشان;
	im_activatekey_state  = 0;
	أرجع TRUE;
    }

    أرجع im_string_منزل_keyval((ثابت حرف *)p_imak,
			       &im_activatekey_keyval,
			       &im_activatekey_state);
}
#  endif // !USE_GTK4

#  ifndef USE_GTK4
    ساكن فراغ
im_synthesize_keypress(بنانشان صحيح keyval, بنانشان صحيح state)
{
    Gdk_Event_ڪنجي *event;

    event = (Gdk_Event_ڪنجي *)gdk_event_new(GDK_ڪنجي_PRESS);
    g_object_ref(gtk_widget_get_دري(gui.drawarea));
					// unreffed by gdk_event_free()
    event->دري_2 = gtk_widget_get_دري(gui.drawarea);
    event->send_event = TRUE;
    event->وقت = GDK_CURRENT_وقت;
    event->state  = state;
    event->keyval = keyval;
    event->hardware_keycode = // needed for XIM
	XKeysym_منزل_Keycode(GDK_دري_XDISPLAY(event->دري_2), (ڪنجي_Sym)keyval);
    event->ڊيگهائي = 0;
    event->string = عدم;

    gtk_im_context_filter_keypress(xic, event);

    // For consistency, also send the corresponding release event.
    event->type = GDK_ڪنجي_RELEASE;
    event->send_event = FALSE;
    gtk_im_context_filter_keypress(xic, event);

    gdk_event_free((GdkEvent *)event);
}
#  endif // !USE_GTK4

    فراغ
xim_reset(فراغ)
{
#  ifdef FEAT_EVAL
    إذا (USE_IMACTIVATEFUNC)
	call_imactivatefunc(im_is_سرگرم);
    وإلا
#  endif
    إذا (xic != عدم)
    {
	gtk_im_context_reset(xic);

	إذا (p_imdisable)
	    im_shutdown();
	وإلا
	{
	    xim_سيٽ_focus(gui.اندريون_focus);

#  ifdef USE_GTK4
	    im_shutdown();
	    xim_init();
	    xim_سيٽ_focus(gui.اندريون_focus);
#  وإلا
	    إذا (im_activatekey_keyval != GDK_Void_نشان)
	    {
		إذا (im_is_سرگرم)
		{
		    g_signal_handler_بلاڪ(xic, im_commit_handler_id);
		    im_synthesize_keypress(im_activatekey_keyval,
						    im_activatekey_state);
		    g_signal_handler_unblock(xic, im_commit_handler_id);
		}
	    }
	    وإلا
	    {
		im_shutdown();
		xim_init();
		xim_سيٽ_focus(gui.اندريون_focus);
	    }
#  endif
	}
    }

    إذا (p_imst == IM_ON_THE_SPOT)
	preedit_شروعات_کانو = MAXCOL;
    xim_has_preediting = FALSE;
}

#  ifndef USE_GTK4
    صحيح
xim_قطار_ڪنجي_press_event(Gdk_Event_ڪنجي *event, صحيح هيٺ_ڏانهن)
{
#   ifdef FEAT_GUI_GTK
    إذا (event->state & GDK_SUPER_نقاب) أرجع FALSE;
#   endif
    إذا (هيٺ_ڏانهن)
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
	بدل (event->keyval)
	{
	    حالة GDK_KP_جوڙيو:      xim_expected_char = '+';  اكسر;
	    حالة GDK_KP_ڪٽيو: xim_expected_char = '-';  اكسر;
	    حالة GDK_KP_Divide:   xim_expected_char = '/';  اكسر;
	    حالة GDK_KP_Multiply: xim_expected_char = '*';  اكسر;
	    حالة GDK_KP_Decimal:  xim_expected_char = '.';  اكسر;
	    حالة GDK_KP_Equal:    xim_expected_char = '=';  اكسر;
	    حالة GDK_KP_0:	  xim_expected_char = '0';  اكسر;
	    حالة GDK_KP_1:	  xim_expected_char = '1';  اكسر;
	    حالة GDK_KP_2:	  xim_expected_char = '2';  اكسر;
	    حالة GDK_KP_3:	  xim_expected_char = '3';  اكسر;
	    حالة GDK_KP_4:	  xim_expected_char = '4';  اكسر;
	    حالة GDK_KP_5:	  xim_expected_char = '5';  اكسر;
	    حالة GDK_KP_6:	  xim_expected_char = '6';  اكسر;
	    حالة GDK_KP_7:	  xim_expected_char = '7';  اكسر;
	    حالة GDK_KP_8:	  xim_expected_char = '8';  اكسر;
	    حالة GDK_KP_9:	  xim_expected_char = '9';  اكسر;
	    حالة GDK_space:	  xim_expected_char = ' ';  اكسر;
	    افتراضي:		  xim_expected_char = NUL;
	}
	xim_ignored_char = FALSE;
    }

    // When typing fFtT, XIM may be activated. Thus it must pass
    // gtk_im_context_filter_keypress() in Normal mode.
    // And while doing :sh too.
    إذا (xic != عدم && !p_imdisable
		    && (State & (گهڻائي_وجهو | گهڻائي_CMDLINE
					      | گهڻائي_عام | گهڻائي_EXTERNCMD)))
    {
	// Filter 'imactivatekey' and map it to CTRL-^.  This way, Vim is
	// always aware of the current status of IM, and can even emulate
	// the activation key for modules that don't support one.
	إذا (event->keyval == im_activatekey_keyval
	     && (event->state & im_activatekey_state) == im_activatekey_state)
	{
	    بنانشان صحيح state_نقاب;

	    // Require the state of the 3 most used modifiers to match exactly.
	    // Otherwise e.g. <S-C-space> would be unusable for other purposes
	    // if the IM activate key is <S-space>.
	    state_نقاب  = im_activatekey_state;
	    state_نقاب |= ((صحيح)GDK_سرڪايو_نقاب | (صحيح)GDK_CONTROL_نقاب
							| (صحيح)GDK_MOD1_نقاب);

	    إذا ((event->state & state_نقاب) != im_activatekey_state)
		أرجع FALSE;

	    // Don't send it a second time on GDK_KEY_RELEASE.
	    إذا (event->type != GDK_ڪنجي_PRESS)
		أرجع TRUE;

	    إذا (map_منزل_exists_گهڻائي((char_u *)"", گهڻائي_LANGMAP, FALSE))
	    {
		im_سيٽ_سرگرم(FALSE);

		// ":lmap" mappings exists, toggle use of mappings.
		State ^= گهڻائي_LANGMAP;
		إذا (State & گهڻائي_LANGMAP)
		{
		    curbuf->b_p_iminsert = B_IMODE_NONE;
		    State &= ~گهڻائي_LANGMAP;
		}
		وإلا
		{
		    curbuf->b_p_iminsert = B_IMODE_LMAP;
		    State |= گهڻائي_LANGMAP;
		}
		أرجع TRUE;
	    }

	    أرجع gtk_im_context_filter_keypress(xic, event);
	}

	// Don't filter events through the IM context if IM isn't active
	// right now.  Unlike with GTK+ 1.2 we cannot rely on the IM module
	// not doing anything before the activation key was sent.
	إذا (im_activatekey_keyval == GDK_Void_نشان || im_is_سرگرم)
	{
	    صحيح imresult = gtk_im_context_filter_keypress(xic, event);

	    إذا (p_imst == IM_ON_THE_SPOT)
	    {
		// Some XIM send following sequence:
		// 1. preedited string.
		// 2. committed string.
		// 3. line changed key.
		// 4. preedited string.
		// 5. remove preedited string.
		// if 3, Vim can't move back the above line for 5.
		// thus, this part should not parse the key.
		إذا (!imresult && preedit_شروعات_کانو != MAXCOL
					    && event->keyval == GDK_Return)
		{
		    im_synthesize_keypress(GDK_Return, 0U);
		    أرجع FALSE;
		}
	    }

	    // If XIM tried to commit a keypad key as a single char.,
	    // ignore it so we can use the keypad key 'raw', for mappings.
	    إذا (xim_expected_char != NUL && xim_ignored_char)
		// We had a keypad key, and XIM tried to thieve it
		أرجع FALSE;

	    // This is supposed to fix a problem with iBus, that space
	    // characters don't work in input mode.
	    xim_expected_char = NUL;

	    // Normal processing
	    أرجع imresult;
	}
    }

    أرجع FALSE;
}
#  وإلا // USE_GTK4
// GTK4: imactivatekey is not supported because GTK4's GtkIMContext
// does not allow synthesizing key events for IM activation.
    صحيح
im_xim_isvalid_imactivate(فراغ)
{
    // Empty string is always valid (means no activation key).
    // Any other value is not supported in GTK4.
    أرجع p_imak[0] == NUL;
}

    صحيح
xim_قطار_ڪنجي_press_event(GdkEvent *event اڻ_استعمال_ٿيل, صحيح هيٺ_ڏانهن اڻ_استعمال_ٿيل)
{
    أرجع FALSE;
}
#  endif // !USE_GTK4

    صحيح
im_get_حالت(فراغ)
{
#  ifdef FEAT_EVAL
    إذا (USE_IMSTATUSFUNC)
	أرجع call_imstatusfunc();
#  endif
    أرجع im_is_سرگرم;
}

    صحيح
preedit_get_حالت(فراغ)
{
    أرجع preedit_is_سرگرم;
}

    صحيح
im_is_preediting(فراغ)
{
    أرجع xim_has_preediting;
}

# وإلا // !FEAT_GUI_GTK

ساكن صحيح	xim_is_سرگرم = FALSE;  // XIM should be active in the current
					//گهڻائي
ساكن صحيح	xim_has_focus = FALSE;	// XIM is really being used for Vim
#  ifdef FEAT_GUI_X11
ساكن XIMStyle	داخلا_style;
ساكن صحيح	حالت_area_enabled = TRUE;
#  endif

/*
 * Switch using XIM on/off.  This is used by the code that changes "State".
 * When 'imactivatefunc' is defined use that function instead.
 */
    فراغ
im_سيٽ_سرگرم(صحيح سرگرم_arg)
{
    صحيح سرگرم = سرگرم_arg;

    // If 'imdisable' is set, XIM is never active.
    إذا (p_imdisable)
	سرگرم = FALSE;
    وإلا إذا (داخلا_style & XIMPreeditPosition)
	// There is a problem in switching XIM off when preediting is used,
	// and it is not clear how this can be solved.  For now, keep XIM on
	// all the time, like it was done in Vim 5.8.
	سرگرم = TRUE;

#  إذا defined(FEAT_EVAL)
    إذا (USE_IMACTIVATEFUNC)
    {
	إذا (سرگرم != im_get_حالت())
	{
	    call_imactivatefunc(سرگرم);
	    xim_has_focus = سرگرم;
	}
	أرجع;
    }
#  endif

    إذا (xic == عدم)
	أرجع;

    // Remember the active state, it is needed when Vim gets keyboard focus.
    xim_is_سرگرم = سرگرم;
    xim_سيٽ_preedit();
}

/*
 * Adjust using XIM for gaining or losing keyboard focus.  Also called when
 * "xim_is_active" changes.
 */
    فراغ
xim_سيٽ_focus(صحيح focus)
{
    إذا (xic == عدم)
	أرجع;

    // XIM only gets focus when the Vim window has keyboard focus and XIM has
    // been set active for the current mode.
    إذا (focus && xim_is_سرگرم)
    {
	إذا (!xim_has_focus)
	{
	    xim_has_focus = TRUE;
	    XSetICFocus(xic);
	}
    }
    وإلا
    {
	إذا (xim_has_focus)
	{
	    xim_has_focus = FALSE;
	    XUnsetICFocus(xic);
	}
    }
}

    فراغ
im_سيٽ_position(صحيح صف اڻ_استعمال_ٿيل, صحيح کانو اڻ_استعمال_ٿيل)
{
    xim_سيٽ_preedit();
}

/*
 * Set the XIM to the current cursor position.
 */
    فراغ
xim_سيٽ_preedit(فراغ)
{
    XVaNestedList attr_list;
    XRectangle spot_area;
    XPoint over_spot;
    صحيح سٽ_space;

    إذا (xic == عدم)
	أرجع;

    xim_سيٽ_focus(TRUE);

    إذا (!xim_has_focus)
    {
	// hide XIM cursor
	over_spot.x = 0;
	over_spot.y = -100; // arbitrary invisible position
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
							XNSpotLocation,
							&over_spot,
							عدم);
	XSetICValues(xic, XNPreedit_خاصيتون, attr_list, عدم);
	XFree(attr_list);
	أرجع;
    }

    إذا (داخلا_style & XIMPreeditPosition)
    {
	إذا (xim_fg_color == INVALCOLOR)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_پٺ_pixel;
	}
	over_spot.x = متن_X(gui.کانو);
	over_spot.y = متن_Y(gui.صف);
	spot_area.x = 0;
	spot_area.y = 0;
	spot_area.اوچائي = gui.char_اوچائي * صفون;
	spot_area.ويڪر  = gui.char_ويڪر * Columns;
	سٽ_space = gui.char_اوچائي;
	attr_list = (XVaNestedList) XVaCreateNestedList(0,
					XNSpotLocation, &over_spot,
					XNForeground, (Pixel) xim_fg_color,
					XNBackground, (Pixel) xim_bg_color,
					XNArea, &spot_area,
					XNLineSpace, سٽ_space,
					عدم);
	إذا (XSetICValues(xic, XNPreedit_خاصيتون, attr_list, عدم))
	    emsg(_(e_cannot_سيٽ_ic_قدرون));
	XFree(attr_list);
    }
}

#  إذا defined(FEAT_GUI_X11)
#   إذا defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(SUN_SYSTEM)
#    define USE_X11R6_XIM
#   endif

ساكن صحيح xim_real_init(دري x11_دري, Display *x11_display);


#   ifdef USE_X11R6_XIM
    ساكن فراغ
xim_instantiate_cb(
    Display	*display,
    XPointer	client_معلومات_2 اڻ_استعمال_ٿيل,
    XPointer	call_معلومات اڻ_استعمال_ٿيل)
{
    دري	x11_دري;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_instantiate_cb()\n");
#    endif

    gui_get_x11_windis(&x11_دري, &x11_display);
    إذا (display != x11_display)
	أرجع;

    xim_real_init(x11_دري, x11_display);
    gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    إذا (xic != عدم)
	XUnregisterIMInstantiateCallback(x11_display, عدم, عدم, عدم,
					 xim_instantiate_cb, عدم);
}

    ساكن فراغ
xim_destroy_cb(
    XIM		im اڻ_استعمال_ٿيل,
    XPointer	client_معلومات_2 اڻ_استعمال_ٿيل,
    XPointer	call_معلومات اڻ_استعمال_ٿيل)
{
    دري	x11_دري;
    Display	*x11_display;

#    ifdef XIM_DEBUG
    xim_log("xim_destroy_cb()\n");
#    endif
    gui_get_x11_windis(&x11_دري, &x11_display);

    xic = عدم;
    حالت_area_enabled = FALSE;

    gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_BOTH);

    XRegisterIMInstantiateCallback(x11_display, عدم, عدم, عدم,
				   xim_instantiate_cb, عدم);
}
#   endif

    فراغ
xim_init(فراغ)
{
    دري	x11_دري;
    Display	*x11_display;

#   ifdef XIM_DEBUG
    xim_log("xim_init()\n");
#   endif

    gui_get_x11_windis(&x11_دري, &x11_display);

    xic = عدم;

    إذا (xim_real_init(x11_دري, x11_display))
	أرجع;

    gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_BOTH);

#   ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, عدم, عدم, عدم,
				   xim_instantiate_cb, عدم);
#   endif
}

    ساكن صحيح
xim_real_init(دري x11_دري, Display *x11_display)
{
    صحيح		i;
    حرف	*p,
		*s,
		*ns,
		*پڇاڙي,
		tmp[1024];
#   define IMLEN_MAX 40
    حرف	buf[IMLEN_MAX + 7];
    XIM		xim = عدم;
    XIMStyles	*xim_styles;
    XIMStyle	this_داخلا_style = 0;
    Boolean	مليو;
    XPoint	over_spot;
    XVaNestedList preedit_list, حالت_list;

    داخلا_style = 0;
    حالت_area_enabled = FALSE;

    إذا (xic != عدم)
	أرجع FALSE;

    إذا (gui.rsrc_داخلا_طريقو != عدم && *gui.rsrc_داخلا_طريقو != NUL)
    {
	vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_داخلا_طريقو,
	    حجم(tmp) - 1);
	لكل (ns = s = tmp; ns != عدم && *s != NUL;)
	{
	    s = (حرف *)skipwhite((char_u *)s);
	    إذا (*s == NUL)
		اكسر;
	    إذا ((ns = پڇاڙي = strchr(s, ',')) == عدم)
		پڇاڙي = s + لڙي_ڊيگهه(s);
	    طالما (محفوظ_isspace(پڇاڙي[-1]))
		پڇاڙي--;
	    *پڇاڙي = NUL;

	    إذا (لڙي_ڊيگهه(s) <= IMLEN_MAX)
	    {
		strcpy(buf, "@im=");
		strcat(buf, s);
		إذا ((p = XSetLocaleModifiers(buf)) != عدم && *p != NUL
			&& (xim = XOpenIM(x11_display, عدم, عدم, عدم))
								      != عدم)
		    اكسر;
	    }

	    s = ns + 1;
	}
    }

    إذا (xim == عدم && (p = XSetLocaleModifiers("")) != عدم && *p != NUL)
	xim = XOpenIM(x11_display, عدم, عدم, عدم);

    // This is supposed to be useful to obtain characters through
    // XmbLookupString() without really using a XIM.
    إذا (xim == عدم && (p = XSetLocaleModifiers("@im=none")) != عدم
								 && *p != NUL)
	xim = XOpenIM(x11_display, عدم, عدم, عدم);

    إذا (xim == عدم)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_failed_منزل_open_داخلا_طريقو));
	    verbose_leave();
	}
	أرجع FALSE;
    }

#   ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_معلومات_2 = عدم;
	إذا (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, عدم))
	    emsg(_(e_warning_could_not_سيٽ_destroy_callback_منزل_im));
    }
#   endif

    إذا (XGetIMValues(xim, XNQuery_داخلا_Style, &xim_styles, عدم) || !xim_styles)
    {
	emsg(_(e_داخلا_طريقو_doesnt_support_any_style));
	XCloseIM(xim);
	أرجع FALSE;
    }

    مليو = False;
    vim_strncpy((char_u *)tmp, (char_u *)gui.rsrc_preedit_type_name,
	    حجم(tmp) - 1);
    لكل (s = tmp; s && !مليو; )
    {
	طالما (*s && محفوظ_isspace(*s))
	    s++;
	إذا (!*s)
	    اكسر;
	إذا ((ns = پڇاڙي = strchr(s, ',')) != 0)
	    ns++;
	وإلا
	    پڇاڙي = s + لڙي_ڊيگهه(s);
	طالما (محفوظ_isspace(*پڇاڙي))
	    پڇاڙي--;
	*پڇاڙي = '\0';

	إذا (!strcmp(s, "OverTheSpot"))
	    this_داخلا_style = (XIMPreeditPosition | XIMStatusArea);
	وإلا إذا (!strcmp(s, "OffTheSpot"))
	    this_داخلا_style = (XIMPreeditArea | XIMStatusArea);
	وإلا إذا (!strcmp(s, "Root"))
	    this_داخلا_style = (XIMPreeditNothing | XIMStatusNothing);

	لكل (i = 0; (بنانشان ننڍو)i < xim_styles->ڳڻپ_styles; i++)
	{
	    إذا (this_داخلا_style == xim_styles->supported_styles[i])
	    {
		مليو = True;
		اكسر;
	    }
	}
	إذا (!مليو)
	    لكل (i = 0; (بنانشان ننڍو)i < xim_styles->ڳڻپ_styles; i++)
	    {
		إذا ((xim_styles->supported_styles[i] & this_داخلا_style)
			== (this_داخلا_style & ~XIMStatusArea))
		{
		    this_داخلا_style &= ~XIMStatusArea;
		    مليو = True;
		    اكسر;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    إذا (!مليو)
    {
	// Only give this message when verbose is set, because too many people
	// got this message when they didn't want to use a XIM.
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    emsg(_(e_داخلا_طريقو_doesnt_support_my_preedit_type));
	    verbose_leave();
	}
	XCloseIM(xim);
	أرجع FALSE;
    }

    over_spot.x = متن_X(gui.کانو);
    over_spot.y = متن_Y(gui.صف);
    داخلا_style = this_داخلا_style;

    // A crash was reported when trying to pass gui.norm_font as XNFontSet,
    // thus that has been removed.  Hopefully the default works...
#   ifdef FEAT_XFONTSET
    إذا (gui.fontset != NOFONTSET)
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_پٺ_pixel,
				XNFont_سيٽ, (XFont_سيٽ)gui.fontset,
				عدم);
	حالت_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_پٺ_pixel,
				XNFont_سيٽ, (XFont_سيٽ)gui.fontset,
				عدم);
    }
    وإلا
#   endif
    {
	preedit_list = XVaCreateNestedList(0,
				XNSpotLocation, &over_spot,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_پٺ_pixel,
				عدم);
	حالت_list = XVaCreateNestedList(0,
				XNForeground, (Pixel)gui.def_norm_pixel,
				XNBackground, (Pixel)gui.def_پٺ_pixel,
				عدم);
    }

    xic = XCreateIC(xim,
		    XNInputStyle, داخلا_style,
		    XNClient_دري, x11_دري,
		    XNFocus_دري, gui.wid,
		    XNPreedit_خاصيتون, preedit_list,
		    XNStatus_خاصيتون, حالت_list,
		    عدم);
    XFree(حالت_list);
    XFree(preedit_list);
    إذا (xic != عدم)
    {
	إذا (داخلا_style & XIMStatusArea)
	{
	    xim_سيٽ_حالت_area();
	    حالت_area_enabled = TRUE;
	}
	وإلا
	    gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    }
    وإلا
    {
	إذا (!is_not_a_رڪن())
	    emsg(_(e_failed_منزل_create_داخلا_context));
	XCloseIM(xim);
	أرجع FALSE;
    }

    أرجع TRUE;
}

#  endif // FEAT_GUI_X11

/*
 * Get IM status.  When IM is on, return TRUE.  Else return FALSE.
 * FIXME: This doesn't work correctly: Having focus doesn't always mean XIM is
 * active, when not having focus XIM may still be active (e.g., when using a
 * tear-off menu item).
 */
    صحيح
im_get_حالت(فراغ)
{
#  ifdef FEAT_EVAL
    إذا (USE_IMSTATUSFUNC)
	أرجع call_imstatusfunc();
#  endif
    أرجع xim_has_focus;
}

# endif // !FEAT_GUI_GTK

# إذا !defined(FEAT_GUI_GTK)
/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    فراغ
xim_سيٽ_حالت_area(فراغ)
{
    XVaNestedList preedit_list = 0, حالت_list = 0, list = 0;
    XRectangle pre_area, حالت_area;

    إذا (xic == عدم)
	أرجع;

    إذا (داخلا_style & XIMStatusArea)
    {
	إذا (داخلا_style & XIMPreeditArea)
	{
	    XRectangle *needed_rect;

	    // to get status_area width
	    حالت_list = XVaCreateNestedList(0, XNAreaNeeded,
					      &needed_rect, عدم);
	    XGetICValues(xic, XNStatus_خاصيتون, حالت_list, عدم);
	    XFree(حالت_list);

	    حالت_area.ويڪر = needed_rect->ويڪر;
	}
	وإلا
	    حالت_area.ويڪر = gui.char_ويڪر * Columns;

	حالت_area.x = 0;
	حالت_area.y = gui.char_اوچائي * صفون + gui.border_وٿي;
	إذا (gui.which_scrollbars[SBAR_BOTTOM])
	    حالت_area.y += gui.scrollbar_اوچائي;
#  ifdef FEAT_MENU
	إذا (gui.menu_is_سرگرم)
	    حالت_area.y += gui.menu_اوچائي;
#  endif
	حالت_area.اوچائي = gui.char_اوچائي;
	حالت_list = XVaCreateNestedList(0, XNArea, &حالت_area, عدم);
    }
    وإلا
    {
	حالت_area.x = 0;
	حالت_area.y = gui.char_اوچائي * صفون + gui.border_وٿي;
	إذا (gui.which_scrollbars[SBAR_BOTTOM])
	    حالت_area.y += gui.scrollbar_اوچائي;
#  ifdef FEAT_MENU
	إذا (gui.menu_is_سرگرم)
	    حالت_area.y += gui.menu_اوچائي;
#  endif
	حالت_area.ويڪر = 0;
	حالت_area.اوچائي = gui.char_اوچائي;
    }

    إذا (داخلا_style & XIMPreeditArea)   // off-the-spot
    {
	pre_area.x = حالت_area.x + حالت_area.ويڪر;
	pre_area.y = gui.char_اوچائي * صفون + gui.border_وٿي;
	pre_area.ويڪر = gui.char_ويڪر * Columns - pre_area.x;
	إذا (gui.which_scrollbars[SBAR_BOTTOM])
	    pre_area.y += gui.scrollbar_اوچائي;
#  ifdef FEAT_MENU
	إذا (gui.menu_is_سرگرم)
	    pre_area.y += gui.menu_اوچائي;
#  endif
	pre_area.اوچائي = gui.char_اوچائي;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, عدم);
    }
    وإلا إذا (داخلا_style & XIMPreeditPosition)   // over-the-spot
    {
	pre_area.x = 0;
	pre_area.y = 0;
	pre_area.اوچائي = gui.char_اوچائي * صفون;
	pre_area.ويڪر = gui.char_ويڪر * Columns;
	preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, عدم);
    }

    إذا (preedit_list && حالت_list)
	list = XVaCreateNestedList(0, XNPreedit_خاصيتون, preedit_list,
				   XNStatus_خاصيتون, حالت_list, عدم);
    وإلا إذا (preedit_list)
	list = XVaCreateNestedList(0, XNPreedit_خاصيتون, preedit_list,
				   عدم);
    وإلا إذا (حالت_list)
	list = XVaCreateNestedList(0, XNStatus_خاصيتون, حالت_list,
				   عدم);
    وإلا
	list = عدم;

    إذا (list)
    {
	XSetICValues(xic, XNVaNestedList, list, عدم);
	XFree(list);
    }
    إذا (حالت_list)
	XFree(حالت_list);
    إذا (preedit_list)
	XFree(preedit_list);
}

    صحيح
xim_get_حالت_area_اوچائي(فراغ)
{
    إذا (حالت_area_enabled)
	أرجع gui.char_اوچائي;
    أرجع 0;
}
# endif

#وإلا // !defined(FEAT_XIM)

# إذا defined(IME_WITHOUT_XIM) || defined(VIMDLL)
ساكن صحيح im_was_سيٽ_سرگرم = FALSE;

    صحيح
#  ifdef VIMDLL
mbyte_im_get_حالت(فراغ)
#  وإلا
im_get_حالت(فراغ)
#  endif
{
#  إذا defined(FEAT_EVAL)
    إذا (USE_IMSTATUSFUNC)
	أرجع call_imstatusfunc();
#  endif
    أرجع im_was_سيٽ_سرگرم;
}

    فراغ
#  ifdef VIMDLL
mbyte_im_سيٽ_سرگرم(صحيح سرگرم_arg)
#  وإلا
im_سيٽ_سرگرم(صحيح سرگرم_arg)
#  endif
{
#  إذا defined(FEAT_EVAL)
    صحيح	    سرگرم = !p_imdisable && سرگرم_arg;

    إذا (USE_IMACTIVATEFUNC && سرگرم != im_get_حالت())
    {
	call_imactivatefunc(سرگرم);
	im_was_سيٽ_سرگرم = سرگرم;
    }
#  endif
}

#  إذا defined(FEAT_GUI) && !defined(FEAT_GUI_HAIKU) && !defined(VIMDLL)
    فراغ
im_سيٽ_position(صحيح صف اڻ_استعمال_ٿيل, صحيح کانو اڻ_استعمال_ٿيل)
{
}
#  endif
# endif

#جيڪڏهن_پورو // FEAT_XIM
