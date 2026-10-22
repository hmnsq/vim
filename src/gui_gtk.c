/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Porting to GTK+ was done by:
 *
 * (C) 1998,1999,2000 by Marcin Dalecki <martin@dalecki.de>
 *
 * With GREAT support and continuous encouragements by Andy Kahn and of
 * course Bram Moolenaar!
 *
 * Support for GTK+ 2 was added by:
 *
 * (C) 2002,2003  Jason Hildebrand  <jason@peaceworks.ca>
 *		  Daniel Elstner  <daniel.elstner@gmx.net>
 *
 * Best supporting actor (He helped somewhat, aesthetically speaking):
 * Maxime Romano <verbophobe@hotmail.com>
 *
 * Support for GTK+ 3 was added by:
 *
 * 2016  Kazunobu Kuriyama  <kazunobu.kuriyama@gmail.com>
 *
 * With the help of Marius Gedminas and the word of Bram Moolenaar,
 * "Let's give this some time to mature."
 */

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_GUI_GTK
# include "gui_gtk_f.h"
#అయితే_ముగింపు

// GTK defines MAX and MIN, but some system header files as well.  Undefine
// them and don't use them.
#నిర్వచితమైతే MIN
# undef MIN
#అయితే_ముగింపు
#నిర్వచితమైతే MAX
# undef MAX
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GNOME
// Gnome redefines _() and N_().  Grrr...
# ifdef _
#  undef _
# endif
# ifdef N_
#  undef N_
# endif
# ifdef textdomain
#  undef textdomain
# endif
# ifdef bindtextdomain
#  undef bindtextdomain
# endif
# ifdef bind_textdomain_codeset
#  undef bind_textdomain_codeset
# endif
# ఒకవేళ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GTK
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# లేకపోతే
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# లేకపోతే
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#లేకపోతే
// define these items to be able to generate prototypes without GTK
రకనిర్వచనం పూర్ణాంకం GtkWidget;
# define gpointer పూర్ణాంకం
# define guint8 పూర్ణాంకం
# define GdkPixmap పూర్ణాంకం
# define GdkBitmap పూర్ణాంకం
# define GtkIconFactory పూర్ణాంకం
# define GtkToolbar పూర్ణాంకం
# define GtkAdjustment పూర్ణాంకం
# define gboolean పూర్ణాంకం
# define Gdk_Event_కీలకం పూర్ణాంకం
# define Cancel_దత్తాంశం పూర్ణాంకం
#అయితే_ముగింపు

స్థిర శూన్య నమోదు_activate_cb(GtkWidget *widget, gpointer దత్తాంశం);
స్థిర శూన్య నమోదు_మారింది_cb(GtkWidget *నమోదు_2, GtkWidget *dialog);
స్థిర శూన్య కనుగొను_replace_cb(GtkWidget *widget, gpointer దత్తాంశం);
#ఒకవేళ defined(FEAT_BROWSE)
స్థిర శూన్య recent_func_log_func(
	స్థిరరాశి gchar *log_domain,
	GLogLevelFlags log_level,
	స్థిరరాశి gchar *సందేశం_2,
	gpointer user_దత్తాంశం);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి menu_themed_పేర్లు[] =
{
    /* 00 */ "document-new",		// sub. GTK_STOCK_NEW
    /* 01 */ "document-open",		// sub. GTK_STOCK_OPEN
    /* 02 */ "document-save",		// sub. GTK_STOCK_SAVE
    /* 03 */ "edit-undo",		// sub. GTK_STOCK_UNDO
    /* 04 */ "edit-redo",		// sub. GTK_STOCK_REDO
    /* 05 */ "edit-cut",		// sub. GTK_STOCK_CUT
    /* 06 */ "edit-copy",		// sub. GTK_STOCK_COPY
    /* 07 */ "edit-paste",		// sub. GTK_STOCK_PASTE
    /* 08 */ "document-print",		// sub. GTK_STOCK_PRINT
    /* 09 */ "help-browser",		// sub. GTK_STOCK_HELP
    /* 10 */ "edit-find",		// sub. GTK_STOCK_FIND
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  లేకపోతే
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  లేకపోతే
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  లేకపోతే
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  లేకపోతే
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# లేకపోతే // !GTK_CHECK_VERSION(3,10,0)
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి menu_stock_ids[] =
{
    /* 00 */ GTK_STOCK_NEW,
    /* 01 */ GTK_STOCK_OPEN,
    /* 02 */ GTK_STOCK_SAVE,
    /* 03 */ GTK_STOCK_UNDO,
    /* 04 */ GTK_STOCK_REDO,
    /* 05 */ GTK_STOCK_CUT,
    /* 06 */ GTK_STOCK_COPY,
    /* 07 */ GTK_STOCK_PASTE,
    /* 08 */ GTK_STOCK_PRINT,
    /* 09 */ GTK_STOCK_HELP,
    /* 10 */ GTK_STOCK_కనుగొను,
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
    /* 15 */ GTK_STOCK_EXECUTE,
    /* 16 */ GTK_STOCK_కనుగొను_AND_REPLACE,
    /* 17 */ GTK_STOCK_CLOSE,		// FIXME: fuzzy
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
    /* 22 */ GTK_STOCK_GO_వెనుక,
    /* 23 */ GTK_STOCK_GO_FORWARD,
    /* 24 */ "vim-find-help",
    /* 25 */ GTK_STOCK_CONVERT,
    /* 26 */ GTK_STOCK_JUMP_గమ్యం,
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
    /* 31 */ GTK_STOCK_QUIT
};
# endif // !GTK_CHECK_VERSION(3,10,0)

# ifdef USE_GRESOURCE
#  ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
రకనిర్వచనం నిర్మాణం Icon_పేర్లు {
    స్థిరరాశి అక్షరం *icon_name;
    స్థిరరాశి అక్షరం *file_name;
} Icon_పేర్లు;

స్థిర Icon_పేర్లు stock_vim_icons[] = {
    { "vim-build-tags", "stock_vim_build_tags.png" },
    { "vim-find-help", "stock_vim_find_help.png" },
    { "vim-save-all", "stock_vim_save_all.png" },
    { "vim-session-load", "stock_vim_session_load.png" },
    { "vim-session-new", "stock_vim_session_new.png" },
    { "vim-session-save", "stock_vim_session_save.png" },
    { "vim-shell", "stock_vim_shell.png" },
    { "vim-window-maximize", "stock_vim_window_maximize.png" },
    { "vim-window-maximize-width", "stock_vim_window_maximize_width.png" },
    { "vim-window-minimize", "stock_vim_window_minimize.png" },
    { "vim-window-minimize-width", "stock_vim_window_minimize_width.png" },
    { "vim-window-split", "stock_vim_window_split.png" },
    { "vim-window-split-vertical", "stock_vim_window_split_vertical.png" },
    { ఖాళీ, ఖాళీ }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    స్థిర శూన్య
కూడు_stock_icon(GtkIconFactory	*factory,
	       స్థిరరాశి అక్షరం	*stock_id,
	       స్థిరరాశి guint8	*inline_దత్తాంశం,
	       పూర్ణాంకం		దత్తాంశం_నిడివి)
{
    GdkPixbuf	*pixbuf;
    Gtk_Icon_సమితి	*icon_సమితి;

    pixbuf = gdk_pixbuf_new_from_inline(దత్తాంశం_నిడివి, inline_దత్తాంశం, FALSE, ఖాళీ);
    icon_సమితి = gtk_icon_సమితి_new_from_pixbuf(pixbuf);

    gtk_icon_factory_కూడు(factory, stock_id, icon_సమితి);

    gtk_icon_సమితి_unref(icon_సమితి);
    g_object_unref(pixbuf);
}
# endif

    స్థిర పూర్ణాంకం
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    ఒకవేళ (mch_is_నిండిన_Name(dest))
    {
	తిరిగి vim_fexists(dest);
    }
    లేకపోతే
    {
	స్థిర స్థిరరాశి అక్షరం   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	గుర్తులేని పూర్ణాంకం	    i;

	కొరకు (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    ఒకవేళ (gui_కనుగొను_bitmap(dest, buf, (అక్షరం *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		తిరిగి TRUE;
	    }

	తిరిగి FALSE;
    }
}

    స్థిర GtkWidget *
ఎక్కించు_menu_iconfile(char_u *name, Gtk_Icon_కొలత icon_కొలత)
{
    GtkWidget	    *image = ఖాళీ;
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
    పూర్ణాంకం		     pixel_కొలత = -1;

    ఎంపిక (icon_కొలత)
    {
	స్థితి GTK_ICON_కొలత_MENU:
	    pixel_కొలత = 16;
	    ఆపు;
	స్థితి GTK_ICON_కొలత_SMALL_TOOLBAR:
	    pixel_కొలత = 16;
	    ఆపు;
	స్థితి GTK_ICON_కొలత_LARGE_TOOLBAR:
	    pixel_కొలత = 24;
	    ఆపు;
	స్థితి GTK_ICON_కొలత_BUTTON:
	    pixel_కొలత = 16;
	    ఆపు;
	స్థితి GTK_ICON_కొలత_DND:
	    pixel_కొలత = 32;
	    ఆపు;
	స్థితి GTK_ICON_కొలత_DIALOG:
	    pixel_కొలత = 48;
	    ఆపు;
	స్థితి GTK_ICON_కొలత_INVALID:
	    // FALLTHROUGH
	అప్రమేయ:
	    pixel_కొలత = 0;
	    ఆపు;
    }

    ఒకవేళ (pixel_కొలత > 0 || pixel_కొలత == -1)
    {
	GdkPixbuf * స్థిరరాశి pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((స్థిరరాశి అక్షరం *)name,
		pixel_కొలత, pixel_కొలత, TRUE, ఖాళీ);
	ఒకవేళ (pixbuf != ఖాళీ)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    ఒకవేళ (image == ఖాళీ)
	image = gtk_image_new_from_icon_name("image-missing", icon_కొలత);

    తిరిగి image;
# లేకపోతే // !GTK_CHECK_VERSION(3,10,0)
    Gtk_Icon_సమితి	    *icon_సమితి;
    Gtk_Icon_ఆధారం   *icon_ఆధారం;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_సమితి = gtk_icon_సమితి_new();
    icon_ఆధారం = gtk_icon_ఆధారం_new();

    gtk_icon_ఆధారం_సమితి_filename(icon_ఆధారం, (స్థిరరాశి అక్షరం *)name);
    gtk_icon_సమితి_కూడు_ఆధారం(icon_సమితి, icon_ఆధారం);

    image = gtk_image_new_from_icon_సమితి(icon_సమితి, icon_కొలత);

    gtk_icon_ఆధారం_free(icon_ఆధారం);
    gtk_icon_సమితి_unref(icon_సమితి);

    తిరిగి image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    స్థిర GtkWidget *
create_menu_icon(vimmenu_T *menu, Gtk_Icon_కొలత icon_కొలత)
{
    GtkWidget	*image = ఖాళీ;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    ఒకవేళ (menu->iconfile != ఖాళీ && lookup_menu_iconfile(menu->iconfile, buf))
	image = ఎక్కించు_menu_iconfile(buf, icon_కొలత);

    // If not found and not builtin specified try using the menu name.
    ఒకవేళ (image == ఖాళీ && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = ఎక్కించు_menu_iconfile(buf, icon_కొలత);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    ఒకవేళ (image == ఖాళీ)
    {
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
	స్థిరరాశి అక్షరం *icon_name = ఖాళీ;
	స్థిరరాశి పూర్ణాంకం   n_పేర్లు = G_N_ELEMENTS(menu_themed_పేర్లు);

	ఒకవేళ (menu->iconidx >= 0 && menu->iconidx < n_పేర్లు)
	    icon_name = menu_themed_పేర్లు[menu->iconidx];
	ఒకవేళ (icon_name == ఖాళీ)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_కొలత);
# లేకపోతే
	స్థిరరాశి అక్షరం  *stock_id;
	స్థిరరాశి పూర్ణాంకం   n_ids = G_N_ELEMENTS(menu_stock_ids);

	ఒకవేళ (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	లేకపోతే
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_కొలత);
# endif
    }

    తిరిగి image;
}

    స్థిర gint
toolbar_button_focus_లోపల_event(GtkWidget *widget వాడని,
			      GdkEventFocus *event వాడని,
			      gpointer దత్తాంశం వాడని)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    ఒకవేళ (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    తిరిగి TRUE;
}
#అయితే_ముగింపు // FEAT_TOOLBAR

#ఒకవేళ defined(FEAT_TOOLBAR)

    శూన్య
gui_gtk_రిజిస్టరు_stock_icons(శూన్య)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define కూడు_ICON(Name, దత్తాంశం_2) కూడు_stock_icon(factory, Name, దత్తాంశం_2, (పూర్ణాంకం)పరిమాణం(దత్తాంశం_2))

    కూడు_ICON("vim-build-tags",		  stock_vim_build_ట్యాగులు);
    కూడు_ICON("vim-find-help",		  stock_vim_కనుగొను_help);
    కూడు_ICON("vim-save-all",		  stock_vim_save_all);
    కూడు_ICON("vim-session-load",	  stock_vim_session_ఎక్కించు);
    కూడు_ICON("vim-session-new",		  stock_vim_session_new);
    కూడు_ICON("vim-session-save",	  stock_vim_session_save);
    కూడు_ICON("vim-shell",		  stock_vim_shell);
    కూడు_ICON("vim-window-maximize",	  stock_vim_కిటికీ_maximize);
    కూడు_ICON("vim-window-maximize-width", stock_vim_కిటికీ_maximize_వెడల్పు);
    కూడు_ICON("vim-window-minimize",	  stock_vim_కిటికీ_minimize);
    కూడు_ICON("vim-window-minimize-width", stock_vim_కిటికీ_minimize_వెడల్పు);
    కూడు_ICON("vim-window-split",	  stock_vim_కిటికీ_విభజించు);
    కూడు_ICON("vim-window-split-vertical", stock_vim_కిటికీ_విభజించు_vertical);

#  undef కూడు_ICON

    gtk_icon_factory_కూడు_default(factory);
    g_object_unref(factory);
# లేకపోతే // defined(USE_GRESOURCE)
    స్థిరరాశి అక్షరం * స్థిరరాశి మార్గం_ఉపసర్గ = "/org/vim/gui/icon";
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,14,0)
    GdkScreen    *screen = ఖాళీ;
    GtkIconTheme *icon_theme = ఖాళీ;

    ఒకవేళ (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    లేకపోతే
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_కూడు_resource_మార్గం(icon_theme, మార్గం_ఉపసర్గ);
#  elif GTK_CHECK_సంస్కరణ(3,0,0)
    Icon_పేర్లు *పేర్లు;

    కొరకు (పేర్లు = stock_vim_icons; పేర్లు->icon_name != ఖాళీ; పేర్లు++)
    {
	అక్షరం మార్గం[MAXPATHL];

	vim_snprintf(మార్గం, MAXPATHL, "%s/%s", మార్గం_ఉపసర్గ, పేర్లు->file_name);
	GdkPixbuf *pixbuf = ఖాళీ;
	pixbuf = gdk_pixbuf_new_from_resource(మార్గం, ఖాళీ);
	ఒకవేళ (pixbuf != ఖాళీ)
	{
	    స్థిరరాశి gint కొలత = MAX(gdk_pixbuf_get_వెడల్పు(pixbuf),
				  gdk_pixbuf_get_ఎత్తు(pixbuf));
	    ఒకవేళ (కొలత > 16)
	    {
		// An icon theme is supposed to provide fixed-size
		// image files for each size, e.g., 16, 22, 24, ...
		// Naturally, in contrast to GtkIconSet, GtkIconTheme
		// won't prepare size variants for us out of a single
		// fixed-size image.
		//
		// Currently, Vim provides 24x24 images only while the
		// icon size on the menu and the toolbar is set to 16x16
		// by default.
		//
		// Resize them by ourselves until we have our own fully
		// fledged icon theme.
		GdkPixbuf *src = pixbuf;
		pixbuf = gdk_pixbuf_scale_simple(src,
						 16, 16,
						 GDK_INTERP_BILINEAR);
		ఒకవేళ (pixbuf == ఖాళీ)
		    pixbuf = src;
		లేకపోతే
		    g_object_unref(src);
	    }
	    gtk_icon_theme_కూడు_builtin_icon(పేర్లు->icon_name, కొలత, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  లేకపోతే // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * స్థిరరాశి factory = gtk_icon_factory_new();
    Icon_పేర్లు *పేర్లు;

    కొరకు (పేర్లు = stock_vim_icons; పేర్లు->icon_name != ఖాళీ; పేర్లు++)
    {
	అక్షరం మార్గం[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(మార్గం, MAXPATHL, "%s/%s", మార్గం_ఉపసర్గ, పేర్లు->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(మార్గం, ఖాళీ);
	ఒకవేళ (pixbuf != ఖాళీ)
	{
	    Gtk_Icon_సమితి *icon_సమితి = gtk_icon_సమితి_new_from_pixbuf(pixbuf);
	    gtk_icon_factory_కూడు(factory, పేర్లు->icon_name, icon_సమితి);
	    gtk_icon_సమితి_unref(icon_సమితి);
	    g_object_unref(pixbuf);
	}
    }

    gtk_icon_factory_కూడు_default(factory);
    g_object_unref(factory);
#  endif // !GTK_CHECK_VERSION(3,0,0)
# endif // defined(USE_GRESOURCE)
}

#అయితే_ముగింపు // FEAT_TOOLBAR

#ఒకవేళ defined(FEAT_MENU)

/*
 * Translate Vim's mnemonic tagging to GTK+ style and convert to UTF-8
 * if necessary.  The caller must vim_free() the returned string.
 *
 *	Input	Output
 *	_	__
 *	&&	&
 *	&	_	stripped if use_mnemonic == FALSE
 *	<Tab>		end of menu label text
 */
    స్థిర char_u *
translate_mnemonic_ట్యాగ్(char_u *name, పూర్ణాంకం use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    పూర్ణాంకం	    n_underscores = 0;

    name = CONVERT_గమ్యం_UTF8(name);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ఒకవేళ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ఒకవేళ (buf != ఖాళీ)
    {
	pdest = buf;
	కొరకు (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ఒకవేళ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    లేకపోతే ఒకవేళ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    లేకపోతే ఒకవేళ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    లేకపోతే ఒకవేళ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_గమ్యం_UTF8_FREE(name);
    తిరిగి buf;
}

    స్థిర శూన్య
menu_అంశం_new(vimmenu_T *menu, GtkWidget *మాతృ_widget)
{
    GtkWidget	*box;
    char_u	*పాఠ్యం;
    పూర్ణాంకం		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_అంశం_new();
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_సమితి_homogeneous(GTK_BOX(box), FALSE);
# లేకపోతే
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(మాతృ_widget));
    పాఠ్యం = translate_mnemonic_ట్యాగ్(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((స్థిరరాశి అక్షరం *)పాఠ్యం);
    vim_free(పాఠ్యం);

    gtk_box_pack_ప్రారంభం(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    ఒకవేళ (menu->actext != ఖాళీ && menu->actext[0] != NUL)
    {
	పాఠ్యం = CONVERT_గమ్యం_UTF8(menu->actext);

	gtk_box_pack_చివర(GTK_BOX(box),
			 gtk_label_new((స్థిరరాశి అక్షరం *)పాఠ్యం),
			 FALSE, FALSE, 0);

	CONVERT_గమ్యం_UTF8_FREE(పాఠ్యం);
    }

    gtk_container_కూడు(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    శూన్య
gui_mch_కూడు_menu(vimmenu_T *menu, పూర్ణాంకం idx)
{
    vimmenu_T	*మాతృ;
    GtkWidget	*మాతృ_widget;

    ఒకవేళ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	తిరిగి;
    }

    మాతృ = menu->మాతృ;

    ఒకవేళ ((మాతృ != ఖాళీ && మాతృ->submenu_id == ఖాళీ)
	    || !menu_is_menubar(menu->name))
	తిరిగి;

    మాతృ_widget = (మాతృ != ఖాళీ) ? మాతృ->submenu_id : gui.menubar;
    menu_అంశం_new(menu, మాతృ_widget);

# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,4,0)
    // since the tearoff should always appear first, increment idx
    ఒకవేళ (మాతృ != ఖాళీ && !menu_is_popup(మాతృ->name))
	++idx;
# endif

    gtk_menu_shell_చొప్పించు(GTK_MENU_SHELL(మాతృ_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_సమితి_accel_సమూహం(GTK_MENU(menu->submenu_id), gui.accel_సమూహం);
    gtk_menu_అంశం_సమితి_submenu(GTK_MENU_అంశం(menu->id), menu->submenu_id);

# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_అంశం_new();
    ఒకవేళ (vim_strchr(p_go, GO_TEAROFF) != ఖాళీ)
	gtk_widget_show(menu->tearoff_handle);
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  లేకపోతే
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    స్థిర శూన్య
menu_అంశం_activate(GtkWidget *widget వాడని, gpointer దత్తాంశం)
{
    gui_menu_cb((vimmenu_T *)దత్తాంశం);
}

    స్థిర శూన్య
menu_అంశం_select(GtkWidget *widget వాడని, gpointer దత్తాంశం)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    స్థిర పూర్ణాంకం	did_సందేశం = FALSE;

    ఒకవేళ (State & బహుళకం_CMDLINE)
	తిరిగి;
    menu = (vimmenu_T *)దత్తాంశం;
    tooltip = CONVERT_గమ్యం_UTF8(menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP]);
    ఒకవేళ (tooltip != ఖాళీ && utf_valid_string(tooltip, ఖాళీ))
    {
	సందేశం((అక్షరం *)tooltip);
	did_సందేశం = TRUE;
	setcursor();
	బహిర్గతం_flush_cursor(TRUE, FALSE);
    }
    లేకపోతే ఒకవేళ (did_సందేశం)
    {
	సందేశం("");
	did_సందేశం = FALSE;
	setcursor();
	బహిర్గతం_flush_cursor(TRUE, FALSE);
    }
    CONVERT_గమ్యం_UTF8_FREE(tooltip);
}

    శూన్య
gui_mch_కూడు_menu_అంశం(vimmenu_T *menu, పూర్ణాంకం idx)
{
    vimmenu_T *మాతృ;

    మాతృ = menu->మాతృ;

# ifdef FEAT_TOOLBAR
    ఒకవేళ (menu_is_toolbar(మాతృ->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = ఖాళీ;

	ఒకవేళ (menu_is_separator(menu->name))
	{
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	    Gtk_Tool_అంశం *అంశం = gtk_separator_tool_అంశం_new();
	    gtk_separator_tool_అంశం_సమితి_draw(GTK_SEPARATOR_TOOL_అంశం(అంశం),
		    TRUE);
	    gtk_tool_అంశం_సమితి_expand(GTK_TOOL_అంశం(అంశం), FALSE);
	    gtk_widget_show(GTK_WIDGET(అంశం));

	    gtk_toolbar_చొప్పించు(toolbar, అంశం, idx);
#  లేకపోతే
	    gtk_toolbar_చొప్పించు_space(toolbar, idx);
#  endif
	    menu->id = ఖాళీ;
	}
	లేకపోతే
	{
	    char_u *పాఠ్యం;
	    char_u *tooltip;

	    పాఠ్యం    = CONVERT_గమ్యం_UTF8(menu->dname);
	    tooltip = CONVERT_గమ్యం_UTF8(menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP]);
	    ఒకవేళ (tooltip != ఖాళీ && !utf_valid_string(tooltip, ఖాళీ))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_గమ్యం_UTF8_FREE(tooltip);

#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	    {
		GtkWidget *icon;
		Gtk_Tool_అంశం *అంశం;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_కొలత(toolbar));
		అంశం = gtk_tool_button_new(icon, (స్థిరరాశి gchar *)పాఠ్యం);
		gtk_tool_అంశం_సమితి_tooltip_పాఠ్యం(అంశం, (స్థిరరాశి gchar *)tooltip);
		g_signal_connect(G_OBJECT(అంశం), "clicked",
			G_CALLBACK(&menu_అంశం_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(అంశం));

		gtk_toolbar_చొప్పించు(toolbar, అంశం, idx);

		menu->id = GTK_WIDGET(అంశం);
	    }
#  లేకపోతే
	    menu->id = gtk_toolbar_చొప్పించు_అంశం(
		    toolbar,
		    (స్థిరరాశి అక్షరం *)పాఠ్యం,
		    (స్థిరరాశి అక్షరం *)tooltip,
		    ఖాళీ,
		    create_menu_icon(menu, gtk_toolbar_get_icon_కొలత(toolbar)),
		    G_CALLBACK(&menu_అంశం_activate),
		    menu,
		    idx);
#  endif

	    ఒకవేళ (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_లోపల_event), ఖాళీ);

	    CONVERT_గమ్యం_UTF8_FREE(పాఠ్యం);
	    CONVERT_గమ్యం_UTF8_FREE(tooltip);
	}
    }
    లేకపోతే
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	ఒకవేళ (మాతృ == ఖాళీ || మాతృ->submenu_id == ఖాళీ)
	    తిరిగి;

# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	ఒకవేళ (!menu_is_popup(మాతృ->name))
	    ++idx;
# endif

	ఒకవేళ (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	    menu->id = gtk_separator_menu_అంశం_new();
# లేకపోతే
	    menu->id = gtk_menu_అంశం_new();
	    gtk_widget_సమితి_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_చొప్పించు(GTK_MENU_SHELL(మాతృ->submenu_id),
		    menu->id, idx);

	    తిరిగి;
	}

	// Add textual menu item.
	menu_అంశం_new(menu, మాతృ->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_చొప్పించు(GTK_MENU_SHELL(మాతృ->submenu_id),
		menu->id, idx);

	ఒకవేళ (menu->id != ఖాళీ)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_అంశం_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_అంశం_select), menu);
	}
    }
}
#అయితే_ముగింపు // FEAT_MENU


    శూన్య
gui_mch_సమితి_పాఠ్యం_area_pos(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#ఒకవేళ defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    శూన్య
gui_gtk_సమితి_mnemonics(పూర్ణాంకం enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ఒకవేళ (menu->id == ఖాళీ)
	    కొనసాగు;

	name = translate_mnemonic_ట్యాగ్(menu->name, enable);
	gtk_label_సమితి_పాఠ్యం_with_mnemonic(GTK_LABEL(menu->label),
					 (స్థిరరాశి అక్షరం *)name);
	vim_free(name);
    }
}

# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,4,0)
    స్థిర శూన్య
recurse_tearoffs(vimmenu_T *menu, పూర్ణాంకం val)
{
    కొరకు (; menu != ఖాళీ; menu = menu->తదుపరి)
    {
	ఒకవేళ (menu->submenu_id != ఖాళీ && menu->tearoff_handle != ఖాళీ
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    ఒకవేళ (val)
		gtk_widget_show(menu->tearoff_handle);
	    లేకపోతే
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
    శూన్య
gui_mch_toggle_tearoffs(పూర్ణాంకం enable వాడని)
{
    // Do nothing
}
# లేకపోతే
    శూన్య
gui_mch_toggle_tearoffs(పూర్ణాంకం enable)
{
    recurse_tearoffs(మూలం_menu, enable);
}
# endif
#అయితే_ముగింపు // FEAT_MENU

#ఒకవేళ defined(FEAT_TOOLBAR)
    స్థిర పూర్ణాంకం
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*కణుపు;
    పూర్ణాంకం		idx = 0;

    కొరకు (కణుపు = menu->మాతృ->children; కణుపు != menu; కణుపు = కణుపు->తదుపరి)
    {
	g_return_val_if_fail(కణుపు != ఖాళీ, -1);
	++idx;
    }

    తిరిగి idx;
}
#అయితే_ముగింపు // FEAT_TOOLBAR


#ఒకవేళ defined(FEAT_TOOLBAR)
    శూన్య
gui_mch_menu_సమితి_tip(vimmenu_T *menu)
{
    ఒకవేళ (menu->id == ఖాళీ || menu->మాతృ == ఖాళీ || gui.toolbar == ఖాళీ)
	తిరిగి;

    char_u *tooltip;

    tooltip = CONVERT_గమ్యం_UTF8(menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP]);
    ఒకవేళ (tooltip != ఖాళీ && utf_valid_string(tooltip, ఖాళీ))
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_సమితి_tooltip_పాఠ్యం(menu->id, (స్థిరరాశి gchar *)tooltip);
# లేకపోతే
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_సమితి_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (స్థిరరాశి అక్షరం *)tooltip, ఖాళీ);
# endif
    CONVERT_గమ్యం_UTF8_FREE(tooltip);
}
#అయితే_ముగింపు // FEAT_TOOLBAR


#ఒకవేళ defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    శూన్య
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    ఒకవేళ (menu->id != ఖాళీ)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    ఒకవేళ (menu->id != ఖాళీ && gui.menubar != ఖాళీ
			    && gtk_widget_get_మాతృ(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    ఒకవేళ (menu->మాతృ != ఖాళీ && menu_is_toolbar(menu->మాతృ->name))
    {
	ఒకవేళ (menu_is_separator(menu->name))
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	{
	    Gtk_Tool_అంశం *అంశం = ఖాళీ;

	    అంశం = gtk_toolbar_get_nth_అంశం(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    ఒకవేళ (అంశం != ఖాళీ)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(అంశం));
	}
#  లేకపోతే
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	లేకపోతే ఒకవేళ (menu->id != ఖాళీ)
	    gtk_widget_destroy(menu->id);
    }
    లేకపోతే
# endif // FEAT_TOOLBAR
    {
	ఒకవేళ (menu->submenu_id != ఖాళీ)
	    gtk_widget_destroy(menu->submenu_id);

	ఒకవేళ (menu->id != ఖాళీ)
	    gtk_widget_destroy(menu->id);
    }

    ఒకవేళ (menu->id != ఖాళీ)
	g_object_unref(menu->id);
    menu->submenu_id = ఖాళీ;
    menu->id = ఖాళీ;
}
#అయితే_ముగింపు // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    శూన్య
gui_mch_సమితి_scrollbar_thumb(scrollbar_T *sb, దీర్ఘ val, దీర్ఘ కొలత, దీర్ఘ max)
{
    ఒకవేళ (sb->id == ఖాళీ)
	తిరిగి;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_సమితి_క్రింది(adjustment, 0.0);
    gtk_adjustment_సమితి_విలువ(adjustment, val);
    gtk_adjustment_సమితి_పై(adjustment, max + 1);
    gtk_adjustment_సమితి_పుట_కొలత(adjustment, కొలత);
    gtk_adjustment_సమితి_పుట_increment(adjustment,
	    కొలత < 3L ? 1L : కొలత - 2L);
    gtk_adjustment_సమితి_అడుగు_increment(adjustment, 1.0);

    g_signal_handler_ఖండం(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,18,0)
    gtk_adjustment_మారింది(adjustment);
#అయితే_ముగింపు

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    శూన్య
gui_mch_సమితి_scrollbar_pos(scrollbar_T *sb, పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    ఒకవేళ (sb->id != ఖాళీ)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    పూర్ణాంకం
gui_mch_get_scrollbar_xpadding(శూన్య)
{
    పూర్ణాంకం xpad;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    xpad = gtk_widget_get_allocated_వెడల్పు(gui.formwin)
	  - gtk_widget_get_allocated_వెడల్పు(gui.drawarea) - gui.scrollbar_వెడల్పు;
#లేకపోతే
    xpad = gui.formwin->కేటాయింపు.వెడల్పు - gui.drawarea->కేటాయింపు.వెడల్పు
							 - gui.scrollbar_వెడల్పు;
#అయితే_ముగింపు
    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ] && gui.which_scrollbars[SBAR_కుడి])
	xpad -= gui.scrollbar_వెడల్పు;

    తిరిగి (xpad < 0) ? 0 : xpad;
}

    పూర్ణాంకం
gui_mch_get_scrollbar_ypadding(శూన్య)
{
    పూర్ణాంకం ypad;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    ypad = gtk_widget_get_allocated_ఎత్తు(gui.formwin)
	- gtk_widget_get_allocated_ఎత్తు(gui.drawarea) - gui.scrollbar_ఎత్తు;
#లేకపోతే
    ypad = gui.formwin->కేటాయింపు.ఎత్తు - gui.drawarea->కేటాయింపు.ఎత్తు
							- gui.scrollbar_ఎత్తు;
#అయితే_ముగింపు
    తిరిగి (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    స్థిర శూన్య
adjustment_విలువ_మారింది(GtkAdjustment *adjustment, gpointer దత్తాంశం)
{
    scrollbar_T	*sb;
    దీర్ఘ	విలువ;
    పూర్ణాంకం		dragging = FALSE;

#నిర్వచితమైతే FEAT_XIM
    // cancel any preediting
    ఒకవేళ (im_is_preediting())
	xim_reset();
#అయితే_ముగింపు

    sb = gui_కనుగొను_scrollbar((దీర్ఘ)దత్తాంశం);
    విలువ = gtk_adjustment_get_విలువ(adjustment);
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    ఒకవేళ (sb != ఖాళీ)
    {
	dragging = TRUE;

	ఒకవేళ (sb->wp != ఖాళీ && GDK_IS_DRAWABLE(sb->id->కిటికీ_2))
	{
	    పూర్ణాంకం			x;
	    పూర్ణాంకం			y;
	    GdkModifierType	state;
	    పూర్ణాంకం			వెడల్పు;
	    పూర్ణాంకం			ఎత్తు;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_కిటికీ_get_సూచిక(sb->id->కిటికీ_2, &x, &y, &state);
	    gdk_కిటికీ_get_కొలత(sb->id->కిటికీ_2, &వెడల్పు, &ఎత్తు);
	    ఒకవేళ (x >= 0 && x < వెడల్పు && y >= 0 && y < ఎత్తు)
	    {
		ఒకవేళ (y < వెడల్పు)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    విలువ = sb->wp->w_topline - 2;
		}
		లేకపోతే ఒకవేళ (y > ఎత్తు - వెడల్పు)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    విలువ = sb->wp->w_topline;
		}
	    }
	}
    }
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, విలువ, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    శూన్య
gui_mch_create_scrollbar(scrollbar_T *sb, పూర్ణాంకం orient)
{
    ఒకవేళ (orient == SBAR_HORIZ)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ఖాళీ);
#లేకపోతే
	sb->id = gtk_hscrollbar_new(ఖాళీ);
#అయితే_ముగింపు
    లేకపోతే ఒకవేళ (orient == SBAR_VERT)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ఖాళీ);
#లేకపోతే
	sb->id = gtk_vscrollbar_new(ఖాళీ);
#అయితే_ముగింపు

    ఒకవేళ (sb->id == ఖాళీ)
	తిరిగి;

    GtkAdjustment *adjustment;

    gtk_widget_సమితి_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_విలువ_మారింది),
	    GINT_గమ్యం_సూచిక(sb->ident));
    gui_mch_update();
}

    శూన్య
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ఒకవేళ (sb->id != ఖాళీ)
    {
	gtk_widget_destroy(sb->id);
	sb->id = ఖాళీ;
    }
    gui_mch_update();
}

#ఒకవేళ defined(FEAT_BROWSE)
/*
 * Implementation of the file selector related stuff
 */

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 * saving,			select file to write
 * title			title for the window
 * dflt				default name
 * ext				not used (extension added)
 * initdir			initial directory, NULL for current dir
 * filter			file name filter
 */
    char_u *
gui_mch_browse(పూర్ణాంకం saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext వాడని,
	       char_u *initdir,
	       char_u *filter)
{
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,20,0)
    GtkFileChooserNative	*fc;
# లేకపోతే
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    స్థిరరాశి gchar		*domain = "Gtk";

    title = CONVERT_గమ్యం_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    ఒకవేళ (initdir == ఖాళీ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    లేకపోతే ఒకవేళ (vim_నిండిన_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    కూడు_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_సమితి_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, ఖాళీ);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,20,0)
    fc = gtk_file_chooser_native_new(
# లేకపోతే
    fc = gtk_file_chooser_dialog_new(
# endif
	    (స్థిరరాశి gchar *)title,
	    GTK_కిటికీ(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# లేకపోతే
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  లేకపోతే
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    ఖాళీ);
# endif
    gtk_file_chooser_సమితి_current_folder(GTK_FILE_CHOOSER(fc),
						       (స్థిరరాశి gchar *)dirbuf);

    ఒకవేళ (filter != ఖాళీ && *filter != NUL)
    {
	పూర్ణాంకం     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ఒకవేళ (patt != ఖాళీ)
	{
	    అయితే (p != ఖాళీ && *p != NUL)
	    {
		ఒకవేళ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ఒకవేళ (*p == '\t')
			gtk_file_filter_సమితి_name(gfilter, (gchar *)patt);
		    లేకపోతే
		    {
			gtk_file_filter_కూడు_నమూనా(gfilter, (gchar *)patt);
			ఒకవేళ (*p == '\n')
			{
			    gtk_file_chooser_కూడు_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    ఒకవేళ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		లేకపోతే
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    ఒకవేళ (saving && dflt != ఖాళీ && *dflt != NUL)
	gtk_file_chooser_సమితి_current_name(GTK_FILE_CHOOSER(fc), (అక్షరం *)dflt);

    gui.browse_fname = ఖాళీ;
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,20,0)
    ఒకవేళ (gtk_native_dialog_నిరంతరం(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# లేకపోతే
    ఒకవేళ (gtk_dialog_నిరంతరం(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	అక్షరం *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,20,0)
    g_object_unref(fc);
# లేకపోతే
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_గమ్యం_UTF8_FREE(title);
    ఒకవేళ (gui.browse_fname == ఖాళీ)
	తిరిగి ఖాళీ;

    // shorten the file name if possible
    తిరిగి vim_strsave(shorten_fname1(gui.browse_fname));
}

/*
 * Put up a directory selector
 * Returns the selected name in allocated memory, or NULL for Cancel.
 * title			title for the window
 * dflt				default name
 * initdir			initial directory, NULL for current dir
 */
    char_u *
gui_mch_browsedir(
	       char_u *title,
	       char_u *initdir)
{
# ఒకవేళ defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = ఖాళీ;

    title = CONVERT_గమ్యం_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (స్థిరరాశి gchar *)title,
	    GTK_కిటికీ(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  లేకపోతే
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    ఖాళీ);

    CONVERT_గమ్యం_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    ఒకవేళ (initdir == ఖాళీ || *initdir == NUL
	       || vim_నిండిన_Name(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    కూడు_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_సమితి_filename(GTK_FILE_CHOOSER(dirdlg),
						      (స్థిరరాశి gchar *)dirbuf);

    // Run the dialog.
    ఒకవేళ (gtk_dialog_నిరంతరం(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    ఒకవేళ (dirname == ఖాళీ)
	తిరిగి ఖాళీ;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    తిరిగి p;

# లేకపోతే // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    తిరిగి gui_mch_browse(0, title, ఖాళీ, ఖాళీ, initdir, ఖాళీ);
# endif // !defined(GTK_FILE_CHOOSER)
}


#అయితే_ముగింపు	// FEAT_BROWSE

#ఒకవేళ defined(FEAT_GUI_DIALOG)

    స్థిర GtkWidget *
create_సందేశం_dialog(పూర్ణాంకం type, char_u *title, char_u *సందేశం_2)
{
    GtkWidget	    *dialog;
    Gtk_సందేశం_Type  సందేశం_type;

    ఎంపిక (type)
    {
	స్థితి VIM_ERROR:	    సందేశం_type = GTK_సందేశం_ERROR;	 ఆపు;
	స్థితి VIM_WARNING:   సందేశం_type = GTK_సందేశం_WARNING;	 ఆపు;
	స్థితి VIM_QUESTION:  సందేశం_type = GTK_సందేశం_QUESTION; ఆపు;
	అప్రమేయ:	    సందేశం_type = GTK_సందేశం_INFO;	 ఆపు;
    }

    సందేశం_2 = CONVERT_గమ్యం_UTF8(సందేశం_2);
    dialog  = gtk_సందేశం_dialog_new(GTK_కిటికీ(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_మాతృ,
				     సందేశం_type,
				     GTK_BUTTONS_NONE,
				     "%s", (స్థిరరాశి అక్షరం *)సందేశం_2);
    CONVERT_గమ్యం_UTF8_FREE(సందేశం_2);

    ఒకవేళ (title != ఖాళీ)
    {
	title = CONVERT_గమ్యం_UTF8(title);
	gtk_కిటికీ_సమితి_title(GTK_కిటికీ(dialog), (స్థిరరాశి అక్షరం *)title);
	CONVERT_గమ్యం_UTF8_FREE(title);
    }
    లేకపోతే ఒకవేళ (type == VIM_GENERIC)
    {
	gtk_కిటికీ_సమితి_title(GTK_కిటికీ(dialog), "VIM");
    }

    తిరిగి dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    స్థిర అక్షరం **
విభజించు_button_string(char_u *button_string, పూర్ణాంకం *n_buttons)
{
    అక్షరం	    **array;
    char_u	    *p;
    గుర్తులేని పూర్ణాంకం    లెక్క = 1;

    కొరకు (p = button_string; *p != NUL; ++p)
	ఒకవేళ (*p == DLG_BUTTON_SEP)
	    ++లెక్క;

    array = ALLOC_MULT(అక్షరం *, లెక్క + 1);
    లెక్క = 0;

    ఒకవేళ (array != ఖాళీ)
    {
	array[లెక్క++] = (అక్షరం *)button_string;
	కొరకు (p = button_string; *p != NUL; )
	{
	    ఒకవేళ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[లెక్క++] = (అక్షరం *)p;
	    }
	    లేకపోతే ఒకవేళ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    లేకపోతే
		MB_PTR_ADV(p);
	}
	array[లెక్క] = ఖాళీ; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = లెక్క;
    తిరిగి array;
}

# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
    స్థిర అక్షరం **
విభజించు_button_translation(స్థిరరాశి అక్షరం *సందేశం_2)
{
    అక్షరం    **buttons = ఖాళీ;
    char_u  *str;
    పూర్ణాంకం	    n_buttons = 0;
    పూర్ణాంకం	    n_expected = 1;

    కొరకు (str = (char_u *)సందేశం_2; *str != NUL; ++str)
	ఒకవేళ (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(సందేశం_2);
    ఒకవేళ (str != ఖాళీ)
    {
	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ఖాళీ);
	లేకపోతే
	    str = vim_strsave(str);

	ఒకవేళ (str != ఖాళీ)
	    buttons = విభజించు_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    ఒకవేళ (buttons == ఖాళీ || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = ఖాళీ;
	str = vim_strsave((char_u *)సందేశం_2);

	ఒకవేళ (str != ఖాళీ)
	    buttons = విభజించు_button_string(str, &n_buttons);
	ఒకవేళ (buttons == ఖాళీ)
	    vim_free(str);
    }

    తిరిగి buttons;
}

    స్థిర పూర్ణాంకం
button_equal(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b)
{
    అయితే (*a != '\0' && *b != '\0')
    {
	ఒకవేళ (*a == '_' && *++a == '\0')
	    ఆపు;
	ఒకవేళ (*b == '_' && *++b == '\0')
	    ఆపు;

	ఒకవేళ (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    తిరిగి FALSE;

	a = g_utf8_తదుపరి_char(a);
	b = g_utf8_తదుపరి_char(b);
    }

    తిరిగి (*a == '\0' && *b == '\0');
}
# endif

    స్థిర శూన్య
dialog_కూడు_buttons(GtkDialog *dialog, char_u *button_string)
{
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
    అక్షరం    **ok;
    అక్షరం    **ync;  // "yes no cancel"
# endif
    అక్షరం    **buttons;
    పూర్ణాంకం	    n_buttons = 0;
    పూర్ణాంకం	    idx;

    button_string = vim_strsave(button_string); // must be writable
    ఒకవేళ (button_string == ఖాళీ)
	తిరిగి;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ఒకవేళ (vim_strchr(p_go, GO_VERTICAL) != ఖాళీ)
    {
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
	GtkWidget	*vbutton_box;

	vbutton_box = gtk_vbutton_box_new();
	gtk_widget_show(vbutton_box);
	gtk_box_pack_చివర(GTK_BOX(GTK_DIALOG(dialog)->vbox),
						 vbutton_box, TRUE, FALSE, 0);
	// Overrule the "action_area" value, hopefully this works...
	GTK_DIALOG(dialog)->action_area = vbutton_box;
# endif
    }

    /*
     * Yes this is ugly, I don't particularly like it either.  But doing it
     * this way has the compelling advantage that translations need not to
     * be touched at all.  See below what 'ok' and 'ync' are used for.
     */
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
    ok	    = విభజించు_button_translation(N_("&Ok"));
    ync     = విభజించు_button_translation(N_("&Yes\n&No\n&Cancel"));
# endif
    buttons = విభజించు_button_string(button_string, &n_buttons);

    /*
     * Yes, the buttons are in reversed order to match the GNOME 2 desktop
     * environment.  Don't hit me -- it's all about consistency.
     * Well, apparently somebody changed his mind: with GTK 2.2.4 it works the
     * other way around...
     */
    కొరకు (idx = 1; idx <= n_buttons; ++idx)
    {
	అక్షరం	*label;
	char_u	*label8;

	label = buttons[idx - 1];
	/*
	 * Perform some guesswork to find appropriate stock items for the
	 * buttons.  We have to compare with a sample of the translated
	 * button string to get things right.  Yes, this is hackish :/
	 *
	 * But even the common button labels aren't necessarily translated,
	 * since anyone can create their own dialogs using Vim functions.
	 * Thus we have to check for those too.
	 */
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
	ఒకవేళ (ok != ఖాళీ && ync != ఖాళీ) // almost impossible to fail
	{
	    ఒకవేళ	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    లేకపోతే ఒకవేళ (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    లేకపోతే ఒకవేళ (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    లేకపోతే ఒకవేళ (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    లేకపోతే ఒకవేళ (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    లేకపోతే ఒకవేళ (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    లేకపోతే ఒకవేళ (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    లేకపోతే ఒకవేళ (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_గమ్యం_UTF8((char_u *)label);
	gtk_dialog_కూడు_button(dialog, (స్థిరరాశి gchar *)label8, idx);
	CONVERT_గమ్యం_UTF8_FREE(label8);
    }

# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
    ఒకవేళ (ok != ఖాళీ)
	vim_free(*ok);
    ఒకవేళ (ync != ఖాళీ)
	vim_free(*ync);
    vim_free(ok);
    vim_free(ync);
# endif
    vim_free(buttons);
    vim_free(button_string);
}

/*
 * Allow mnemonic accelerators to be activated without pressing <Alt>.
 * I'm not sure if it's a wise idea to do this.  However, the old GTK+ 1.2
 * GUI used to work this way, and I consider the impact on UI consistency
 * low enough to justify implementing this as a special Vim feature.
 */
రకనిర్వచనం నిర్మాణం _DialogInfo
{
    పూర్ణాంకం		ignore_enter;	    // no default button, ignore "Enter"
    పూర్ణాంకం		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    స్థిర gboolean
dialog_కీలకం_press_event_cb(GtkWidget *widget, Gdk_Event_కీలకం *event, gpointer దత్తాంశం)
{
    DialogInfo *di = (DialogInfo *)దత్తాంశం;

    // Ignore hitting Enter (or Space) when there is no default button.
    ఒకవేళ (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	తిరిగి TRUE;
    లేకపోతే    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    ఒకవేళ (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	తిరిగి TRUE;
    }

    ఒకవేళ (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_ముసుగు()) == 0)
    {
	తిరిగి gtk_కిటికీ_mnemonic_activate(
		   GTK_కిటికీ(widget), event->keyval,
		   gtk_కిటికీ_get_mnemonic_modifier(GTK_కిటికీ(widget)));
    }

    తిరిగి FALSE; // continue emission
}

    పూర్ణాంకం
gui_mch_dialog(పూర్ణాంకం	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*సందేశం_2,   // message text
	       char_u	*buttons,   // names of buttons
	       పూర్ణాంకం	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       పూర్ణాంకం	ex_cmd వాడని)
{
    GtkWidget	*dialog;
    GtkWidget	*నమోదు_2 = ఖాళీ;
    char_u	*పాఠ్యం;
    పూర్ణాంకం		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_చురుకైన;

    dialog = create_సందేశం_dialog(type, title, సందేశం_2);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_కూడు_buttons(GTK_DIALOG(dialog), buttons);

    ఒకవేళ (textfield != ఖాళీ)
    {
	GtkWidget *alignment;

	నమోదు_2 = gtk_నమోదు_new();
	gtk_widget_show(నమోదు_2);

	// Make Enter work like pressing OK.
	gtk_నమోదు_సమితి_activates_default(GTK_నమోదు(నమోదు_2), TRUE);

	పాఠ్యం = CONVERT_గమ్యం_UTF8(textfield);
	gtk_నమోదు_సమితి_పాఠ్యం(GTK_నమోదు(నమోదు_2), (స్థిరరాశి అక్షరం *)పాఠ్యం);
	CONVERT_గమ్యం_UTF8_FREE(పాఠ్యం);

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,14,0)
	gtk_widget_సమితి_halign(GTK_WIDGET(నమోదు_2), GTK_ALIGN_CENTER);
	gtk_widget_సమితి_valign(GTK_WIDGET(నమోదు_2), GTK_ALIGN_CENTER);
	gtk_widget_సమితి_hexpand(GTK_WIDGET(నమోదు_2), TRUE);
	gtk_widget_సమితి_vexpand(GTK_WIDGET(నమోదు_2), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# లేకపోతే
	alignment = gtk_alignment_new((దశాంశం)0.5, (దశాంశం)0.5,
						      (దశాంశం)1.0, (దశాంశం)1.0);
# endif
	gtk_container_కూడు(GTK_CONTAINER(alignment), నమోదు_2);
	gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	{
	    GtkWidget * స్థిరరాశి vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_ప్రారంభం(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# లేకపోతే
	gtk_box_pack_ప్రారంభం(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    లేకపోతే
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_కీలకం_press_event_cb), &dialoginfo);

    ఒకవేళ (def_but > 0)
    {
	gtk_dialog_సమితి_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    లేకపోతే
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_నిరంతరం(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    ఒకవేళ (response != GTK_RESPONSE_NONE)
    {
	ఒకవేళ (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	ఒకవేళ (textfield != ఖాళీ)
	{
	    పాఠ్యం = (char_u *)gtk_నమోదు_get_పాఠ్యం(GTK_నమోదు(నమోదు_2));
	    పాఠ్యం = CONVERT_FROM_UTF8(పాఠ్యం);

	    vim_strncpy(textfield, పాఠ్యం, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(పాఠ్యం);
	}
	gtk_widget_destroy(dialog);
    }

    ఒకవేళ (gui.dialogs_చురుకైన > 0)
	--gui.dialogs_చురుకైన;
    తిరిగి response > 0 ? response : 0;
}

#అయితే_ముగింపు // FEAT_GUI_DIALOG


#ఒకవేళ defined(FEAT_MENU)

    శూన్య
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# ఒకవేళ defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
    ఒకవేళ (xic != ఖాళీ && g_object_get_దత్తాంశం(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == ఖాళీ)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_అంశం_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_గమ్యం_UTF8(name);
	menuitem = gtk_menu_అంశం_new_with_mnemonic((స్థిరరాశి అక్షరం *)name);
	CONVERT_గమ్యం_UTF8_FREE(name);
	gtk_widget_show(menuitem);

	submenu = gtk_menu_new();
	gtk_menu_అంశం_సమితి_submenu(GTK_MENU_అంశం(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	gtk_im_multicontext_append_menuitems(GTK_IM_MULTICONTEXT(xic),
					     GTK_MENU_SHELL(submenu));
	g_object_సమితి_దత్తాంశం(G_OBJECT(menu->submenu_id),
			  "vim-has-im-menu", GINT_గమ్యం_సూచిక(TRUE));
    }
#  endif
# endif // FEAT_XIM

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
    {
	GdkEventButton trigger;

	// A pseudo event to have gtk_menu_popup_at_pointer() work. Since the
	// function calculates the popup menu position on the basis of the
	// actual pointer position when it is invoked, the fields x, y, x_root
	// and y_root are set to zero for convenience.
	trigger.type       = GDK_BUTTON_PRESS;
	trigger.కిటికీ_2     = gtk_widget_get_కిటికీ(gui.drawarea);
	trigger.send_event = FALSE;
	trigger.సమయం       = gui.event_సమయం;
	trigger.x	   = 0.0;
	trigger.y	   = 0.0;
	trigger.axes       = ఖాళీ;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = ఖాళీ;
	trigger.x_మూలం     = 0.0;
	trigger.y_మూలం     = 0.0;

	gtk_menu_popup_at_సూచిక(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# లేకపోతే
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   ఖాళీ, ఖాళీ,
		   (GtkMenuPositionFunc)ఖాళీ, ఖాళీ,
		   3U, gui.event_సమయం);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
స్థిర పూర్ణాంకం popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    స్థిర శూన్య
popup_menu_position_func(GtkMenu *menu వాడని,
			 gint *x, gint *y,
			 gboolean *నెట్టు_లోపల వాడని,
			 gpointer user_దత్తాంశం వాడని)
{
    gdk_కిటికీ_get_origin(gtk_widget_get_కిటికీ(gui.drawarea), x, y);

    ఒకవేళ (popup_mouse_pos)
    {
	పూర్ణాంకం	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    లేకపోతే ఒకవేళ (curwin != ఖాళీ && gui.drawarea != ఖాళీ &&
	     gtk_widget_get_కిటికీ(gui.drawarea) != ఖాళీ)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    శూన్య
gui_make_popup(char_u *మార్గం_name, పూర్ణాంకం mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_కనుగొను_menu(మార్గం_name);
    ఒకవేళ (menu == ఖాళీ || menu->submenu_id == ఖాళీ)
	తిరిగి;

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
    Gdk_కిటికీ * స్థిరరాశి win = gtk_widget_get_కిటికీ(gui.drawarea);
    GdkEventButton trigger;

    // A pseudo event to have gtk_menu_popup_at_*() functions work. Since
    // the position where the menu pops up is automatically adjusted by
    // the functions, none of the fields x, y, x_root and y_root has to be
    // set to a specific value here; therefore, they are set to zero for
    // convenience.
    trigger.type       = GDK_BUTTON_PRESS;
    trigger.కిటికీ_2     = win;
    trigger.send_event = FALSE;
    trigger.సమయం       = GDK_CURRENT_సమయం;
    trigger.x	   = 0.0;
    trigger.y	   = 0.0;
    trigger.axes       = ఖాళీ;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = ఖాళీ;
    trigger.x_మూలం     = 0.0;
    trigger.y_మూలం     = 0.0;

    ఒకవేళ (mouse_pos)
	gtk_menu_popup_at_సూచిక(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    లేకపోతే
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_కిటికీ_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(ఖాళీ, &rect.x, &rect.y, ఖాళీ, ఖాళీ);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# లేకపోతే
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    ఖాళీ, ఖాళీ,
	    &popup_menu_position_func, ఖాళీ,
	    0U, (guint32)GDK_CURRENT_సమయం);
# endif
}

#అయితే_ముగింపు // FEAT_MENU


/*
 * We don't create it twice.
 */

రకనిర్వచనం నిర్మాణం Shared_కనుగొను_Replace
{
    GtkWidget *dialog;	// the main dialog widget
    GtkWidget *wword;	// 'Whole word only' check button
    GtkWidget *mcase;	// 'Match case' check button
    GtkWidget *పైకి;	// search direction 'Up' radio button
    GtkWidget *కిందికి;	// search direction 'Down' radio button
    GtkWidget *what;	// 'Find what' entry text widget
    GtkWidget *with;	// 'Replace with' entry text widget
    GtkWidget *కనుగొను;	// 'Find Next' action button
    GtkWidget *replace;	// 'Replace With' action button
    GtkWidget *all;	// 'Replace All' action button
} Shared_కనుగొను_Replace_2;

స్థిర Shared_కనుగొను_Replace_2 కనుగొను_widgets = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};
స్థిర Shared_కనుగొను_Replace_2 repl_widgets = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};

    స్థిర పూర్ణాంకం
కనుగొను_కీలకం_press_event(
		GtkWidget	*widget వాడని,
		Gdk_Event_కీలకం	*event,
		Shared_కనుగొను_Replace_2 *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ఒకవేళ (event->state & (GDK_CONTROL_ముసుగు | GDK_జరుపు_ముసుగు))
	తిరిగి FALSE;

    // the Escape key synthesizes a cancellation action
    ఒకవేళ (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	తిరిగి TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    తిరిగి FALSE;
}

    స్థిర GtkWidget *
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
create_image_button(స్థిరరాశి అక్షరం *stock_id వాడని,
		    స్థిరరాశి అక్షరం *label)
#లేకపోతే
create_image_button(స్థిరరాశి అక్షరం *stock_id,
		    స్థిరరాశి అక్షరం *label)
#అయితే_ముగింపు
{
    char_u	*పాఠ్యం;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    పాఠ్యం = CONVERT_గమ్యం_UTF8((char_u *)label);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_సమితి_homogeneous(GTK_BOX(box), FALSE);
#లేకపోతే
    box = gtk_hbox_new(FALSE, 3);
#అయితే_ముగింపు
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,10,0)
    ఒకవేళ (stock_id != ఖాళీ)
	gtk_box_pack_ప్రారంభం(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_కొలత_BUTTON),
			   FALSE, FALSE, 0);
#అయితే_ముగింపు
    gtk_box_pack_ప్రారంభం(GTK_BOX(box),
		       gtk_label_new((స్థిరరాశి అక్షరం *)పాఠ్యం),
		       FALSE, FALSE, 0);

    CONVERT_గమ్యం_UTF8_FREE(పాఠ్యం);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,14,0)
    gtk_widget_సమితి_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_సమితి_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_సమితి_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_సమితి_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#లేకపోతే
    alignment = gtk_alignment_new((దశాంశం)0.5, (దశాంశం)0.5,
						      (దశాంశం)0.0, (దశాంశం)0.0);
#అయితే_ముగింపు
    gtk_container_కూడు(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_కూడు(GTK_CONTAINER(button), alignment);

    తిరిగి button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    స్థిర స్థిరరాశి అక్షరం *
convert_localized_సందేశం(char_u **బఫరు, స్థిరరాశి అక్షరం *సందేశం_2)
{
    ఒకవేళ (output_conv.vc_type == CONV_NONE)
	తిరిగి సందేశం_2;

    vim_free(*బఫరు);
    *బఫరు = string_convert(&output_conv, (char_u *)సందేశం_2, ఖాళీ);

    తిరిగి (స్థిరరాశి అక్షరం *)*బఫరు;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    స్థిర గుర్తులేని దీర్ఘ
నమోదు_get_పాఠ్యం_నిడివి(Gtk_నమోదు *నమోదు_2)
{
    g_return_val_if_fail(నమోదు_2 != ఖాళీ, 0);
    g_return_val_if_fail(GTK_IS_నమోదు(నమోదు_2) == TRUE, 0);

#ఒకవేళ GTK_CHECK_సంస్కరణ(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    తిరిగి gtk_నమోదు_బఫరు_get_నిడివి(gtk_నమోదు_get_బఫరు(నమోదు_2));
#లేదావేళ GTK_CHECK_సంస్కరణ(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    తిరిగి gtk_నమోదు_get_పాఠ్యం_నిడివి(నమోదు_2);
#లేకపోతే
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    తిరిగి g_utf8_strlen(gtk_నమోదు_get_పాఠ్యం(నమోదు_2), -1);
#అయితే_ముగింపు
}

    స్థిర శూన్య
కనుగొను_replace_dialog_create(char_u *arg, పూర్ణాంకం do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*పట్టిక;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    Shared_కనుగొను_Replace_2 *frdp;
    char_u	*నమోదు_పాఠ్యం;
    పూర్ణాంకం		wword = FALSE;
    పూర్ణాంకం		mcase = !p_ic;
    char_u	*conv_బఫరు = ఖాళీ;
#నిర్వచించు CONV(సందేశం_2) convert_localized_సందేశం(&conv_బఫరు, (సందేశం_2))

    frdp = (do_replace) ? (&repl_widgets) : (&కనుగొను_widgets);

    // Get the search string to use.
    నమోదు_పాఠ్యం = get_కనుగొను_dialog_పాఠ్యం(arg, &wword, &mcase);

    ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_పాఠ్యం = నమోదు_పాఠ్యం;
	నమోదు_పాఠ్యం = string_convert(&output_conv, నమోదు_పాఠ్యం, ఖాళీ);
	vim_free(old_పాఠ్యం);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    ఒకవేళ (frdp->dialog)
    {
	ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ)
	{
	    gtk_నమోదు_సమితి_పాఠ్యం(GTK_నమోదు(frdp->what), (అక్షరం *)నమోదు_పాఠ్యం);
	    gtk_toggle_button_సమితి_చురుకైన(GTK_TOGGLE_BUTTON(frdp->wword),
							     (gboolean)wword);
	    gtk_toggle_button_సమితి_చురుకైన(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
	}
	gtk_కిటికీ_present(GTK_కిటికీ(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	ఒకవేళ (do_replace && నమోదు_get_పాఠ్యం_నిడివి(GTK_నమోదు(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(నమోదు_పాఠ్యం);
	తిరిగి;
    }

    frdp->dialog = gtk_dialog_new();
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#లేకపోతే
    gtk_dialog_సమితి_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#అయితే_ముగింపు
    gtk_కిటికీ_సమితి_transient_for(GTK_కిటికీ(frdp->dialog), GTK_కిటికీ(gui.mainwin));
    gtk_కిటికీ_సమితి_destroy_with_మాతృ(GTK_కిటికీ(frdp->dialog), TRUE);

    ఒకవేళ (do_replace)
    {
	gtk_కిటికీ_సమితి_title(GTK_కిటికీ(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    లేకపోతే
    {
	gtk_కిటికీ_సమితి_title(GTK_కిటికీ(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_సమితి_homogeneous(GTK_BOX(hbox), FALSE);
#లేకపోతే
    hbox = gtk_hbox_new(FALSE, 0);
#అయితే_ముగింపు
    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(hbox), 10);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    {
	GtkWidget * స్థిరరాశి dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_కూడు(GTK_CONTAINER(dialog_vbox), hbox);
    }
#లేకపోతే
    gtk_container_కూడు(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#అయితే_ముగింపు

    ఒకవేళ (do_replace)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	పట్టిక = gtk_grid_new();
#లేకపోతే
	పట్టిక = gtk_పట్టిక_new(1024, 4, FALSE);
#అయితే_ముగింపు
    లేకపోతే
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	పట్టిక = gtk_grid_new();
#లేకపోతే
	పట్టిక = gtk_పట్టిక_new(1024, 3, FALSE);
#అయితే_ముగింపు
    gtk_box_pack_ప్రారంభం(GTK_BOX(hbox), పట్టిక, TRUE, TRUE, 0);
    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(పట్టిక), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,16,0)
    gtk_label_సమితి_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_సమితి_yalign(GTK_LABEL(tmp), 0.5);
#లేదావేళ GTK_CHECK_సంస్కరణ(3,14,0)
    {
	GValue align_val = G_విలువ_INIT;

	g_విలువ_init(&align_val, G_TYPE_FLOAT);

	g_విలువ_సమితి_float(&align_val, 0.0);
	g_object_సమితి_property(G_OBJECT(tmp), "xalign", &align_val);

	g_విలువ_సమితి_float(&align_val, 0.5);
	g_object_సమితి_property(G_OBJECT(tmp), "yalign", &align_val);

	g_విలువ_unset(&align_val);
    }
#లేకపోతే
    gtk_misc_సమితి_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#అయితే_ముగింపు
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
    gtk_grid_attach(GTK_GRID(పట్టిక), tmp, 0, 0, 2, 1);
#లేకపోతే
    gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు
    frdp->what = gtk_నమోదు_new();
    sensitive = (నమోదు_పాఠ్యం != ఖాళీ && నమోదు_పాఠ్యం[0] != NUL);
    ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ)
	gtk_నమోదు_సమితి_పాఠ్యం(GTK_నమోదు(frdp->what), (అక్షరం *)నమోదు_పాఠ్యం);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(నమోదు_మారింది_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(కనుగొను_కీలకం_press_event),
			   (gpointer) frdp);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
    gtk_grid_attach(GTK_GRID(పట్టిక), frdp->what, 2, 0, 5, 1);
#లేకపోతే
    gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు

    ఒకవేళ (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,16,0)
	gtk_label_సమితి_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_సమితి_yalign(GTK_LABEL(tmp), 0.5);
#లేదావేళ GTK_CHECK_సంస్కరణ(3,14,0)
	{
	    GValue align_val = G_విలువ_INIT;

	    g_విలువ_init(&align_val, G_TYPE_FLOAT);

	    g_విలువ_సమితి_float(&align_val, 0.0);
	    g_object_సమితి_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_విలువ_సమితి_float(&align_val, 0.5);
	    g_object_సమితి_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_విలువ_unset(&align_val);
	}
#లేకపోతే
	gtk_misc_సమితి_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#అయితే_ముగింపు
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), tmp, 0, 1, 2, 1);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు
	frdp->with = gtk_నమోదు_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(కనుగొను_replace_cb),
			 GINT_గమ్యం_సూచిక(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(కనుగొను_కీలకం_press_event),
			       (gpointer) frdp);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), frdp->with, 2, 1, 5, 1);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), frdp->with, 1, 1024, 1, 2,
			 GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు

	/*
	 * Make the entry activation only change the input focus onto the
	 * with item.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(నమోదు_activate_cb), frdp->with);
    }
    లేకపోతే
    {
	/*
	 * Make the entry activation do the search.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(కనుగొను_replace_cb),
			 GINT_గమ్యం_సూచిక(FRD_FINDNEXT));
    }

    // whole word only button
    frdp->wword = gtk_check_button_new_with_label(CONV(_("Match whole word only")));
    gtk_toggle_button_సమితి_చురుకైన(GTK_TOGGLE_BUTTON(frdp->wword),
							(gboolean)wword);
    ఒకవేళ (do_replace)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), frdp->wword, 0, 2, 5, 1);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు
    లేకపోతే
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), frdp->wword, 0, 3, 5, 1);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_సమితి_చురుకైన(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    ఒకవేళ (do_replace)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), frdp->mcase, 0, 3, 5, 1);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు
    లేకపోతే
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), frdp->mcase, 0, 4, 5, 1);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#అయితే_ముగింపు

    tmp = gtk_చట్రం_new(CONV(_("Direction")));
    ఒకవేళ (do_replace)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), tmp, 5, 2, 2, 4);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#అయితే_ముగింపు
    లేకపోతే
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	gtk_grid_attach(GTK_GRID(పట్టిక), tmp, 5, 2, 1, 3);
#లేకపోతే
	gtk_పట్టిక_attach(GTK_పట్టిక(పట్టిక), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#అయితే_ముగింపు
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_సమితి_homogeneous(GTK_BOX(vbox), FALSE);
#లేకపోతే
    vbox = gtk_vbox_new(FALSE, 0);
#అయితే_ముగింపు
    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(vbox), 0);
    gtk_container_కూడు(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->పైకి = gtk_radio_button_new_with_label(ఖాళీ, CONV(_("Up")));
    gtk_box_pack_ప్రారంభం(GTK_BOX(vbox), frdp->పైకి, TRUE, TRUE, 0);
    frdp->కిందికి = gtk_radio_button_new_with_label(
			gtk_radio_button_get_సమూహం(GTK_RADIO_BUTTON(frdp->పైకి)),
			CONV(_("Down")));
    gtk_toggle_button_సమితి_చురుకైన(GTK_TOGGLE_BUTTON(frdp->కిందికి), TRUE);
    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_ప్రారంభం(GTK_BOX(vbox), frdp->కిందికి, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#లేకపోతే
    actionarea = gtk_vbutton_box_new();
#అయితే_ముగింపు
    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_చివర(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
    frdp->కనుగొను = create_image_button(ఖాళీ, _("Find Next"));
#లేకపోతే
    frdp->కనుగొను = create_image_button(GTK_STOCK_కనుగొను, _("Find Next"));
#అయితే_ముగింపు
    gtk_widget_సమితి_sensitive(frdp->కనుగొను, sensitive);

    g_signal_connect(G_OBJECT(frdp->కనుగొను), "clicked",
		     G_CALLBACK(కనుగొను_replace_cb),
		     (do_replace) ? GINT_గమ్యం_సూచిక(FRD_R_FINDNEXT)
				  : GINT_గమ్యం_సూచిక(FRD_FINDNEXT));

    gtk_widget_సమితి_can_default(frdp->కనుగొను, TRUE);
    gtk_box_pack_ప్రారంభం(GTK_BOX(actionarea), frdp->కనుగొను, FALSE, FALSE, 0);
    gtk_widget_grab_default(frdp->కనుగొను);

    ఒకవేళ (do_replace)
    {
	// 'Replace' button
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
	frdp->replace = create_image_button(ఖాళీ, _("Replace"));
#లేకపోతే
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#అయితే_ముగింపు
	gtk_widget_సమితి_sensitive(frdp->replace, sensitive);
	gtk_widget_సమితి_can_default(frdp->కనుగొను, TRUE);
	gtk_box_pack_ప్రారంభం(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(కనుగొను_replace_cb),
			 GINT_గమ్యం_సూచిక(FRD_REPLACE));

	// 'Replace All' button
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
	frdp->all = create_image_button(ఖాళీ, _("Replace All"));
#లేకపోతే
	frdp->all = create_image_button(GTK_STOCK_CONVERT, _("Replace All"));
#అయితే_ముగింపు
	gtk_widget_సమితి_sensitive(frdp->all, sensitive);
	gtk_widget_సమితి_can_default(frdp->all, TRUE);
	gtk_box_pack_ప్రారంభం(GTK_BOX(actionarea), frdp->all, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->all), "clicked",
			 G_CALLBACK(కనుగొను_replace_cb),
			 GINT_గమ్యం_సూచిక(FRD_REPLACEALL));
    }

    // 'Cancel' button
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#లేకపోతే
    tmp = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#అయితే_ముగింపు
    gtk_widget_సమితి_can_default(tmp, TRUE);
    gtk_box_pack_చివర(GTK_BOX(actionarea), tmp, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(tmp),
			     "clicked", G_CALLBACK(gtk_widget_hide),
			     G_OBJECT(frdp->dialog));
    g_signal_connect_swapped(G_OBJECT(frdp->dialog),
			     "delete-event", G_CALLBACK(gtk_widget_hide_on_delete),
			     G_OBJECT(frdp->dialog));

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#లేకపోతే
    tmp = gtk_vseparator_new();
#అయితే_ముగింపు
    gtk_box_pack_చివర(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#లేకపోతే
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#అయితే_ముగింపు
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(నమోదు_పాఠ్యం);
    vim_free(conv_బఫరు);
#నిర్వచనం_రద్దు CONV
}

    శూన్య
gui_mch_కనుగొను_dialog(exarg_T *eap)
{
    ఒకవేళ (gui.లోపల_use)
	కనుగొను_replace_dialog_create(eap->arg, FALSE);
}

    శూన్య
gui_mch_replace_dialog(exarg_T *eap)
{
    ఒకవేళ (gui.లోపల_use)
	కనుగొను_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    స్థిర శూన్య
కనుగొను_replace_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం)
{
    పూర్ణాంకం			flags;
    char_u		*కనుగొను_పాఠ్యం;
    char_u		*repl_పాఠ్యం;
    gboolean		direction_కిందికి;
    Shared_కనుగొను_Replace_2	*sfr;

    flags = (పూర్ణాంకం)(దీర్ఘ)దత్తాంశం;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    ఒకవేళ (flags == FRD_FINDNEXT)
    {
	repl_పాఠ్యం = ఖాళీ;
	sfr = &కనుగొను_widgets;
    }
    లేకపోతే
    {
	repl_పాఠ్యం = (char_u *)gtk_నమోదు_get_పాఠ్యం(GTK_నమోదు(repl_widgets.with));
	sfr = &repl_widgets;
    }

    కనుగొను_పాఠ్యం = (char_u *)gtk_నమోదు_get_పాఠ్యం(GTK_నమోదు(sfr->what));
    direction_కిందికి = gtk_toggle_button_get_చురుకైన(GTK_TOGGLE_BUTTON(sfr->కిందికి));

    ఒకవేళ (gtk_toggle_button_get_చురుకైన(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_సంపూర్ణ_మాట;
    ఒకవేళ (gtk_toggle_button_get_చురుకైన(GTK_TOGGLE_BUTTON(sfr->mcase)))
	flags |= FRD_సరిపోలిక_CASE;

    repl_పాఠ్యం = CONVERT_FROM_UTF8(repl_పాఠ్యం);
    కనుగొను_పాఠ్యం = CONVERT_FROM_UTF8(కనుగొను_పాఠ్యం);
    gui_do_findrepl(flags, కనుగొను_పాఠ్యం, repl_పాఠ్యం, direction_కిందికి);
    CONVERT_FROM_UTF8_FREE(repl_పాఠ్యం);
    CONVERT_FROM_UTF8_FREE(కనుగొను_పాఠ్యం);
}

/*
 * our usual callback function
 */
    స్థిర శూన్య
నమోదు_activate_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం)
{
    gtk_widget_grab_focus(GTK_WIDGET(దత్తాంశం));
}

/*
 * Syncing the find/replace dialogs on the fly is utterly useless crack,
 * and causes nothing but problems.  Please tell me a use case for which
 * you'd need both a find dialog and a find/replace one at the same time,
 * without being able to actually use them separately since they're syncing
 * all the time.  I don't think it's worthwhile to fix this nonsense,
 * particularly evil incarnation of braindeadness, whatever; I'd much rather
 * see it extinguished from this planet.  Thanks for listening.  Sorry.
 */
    స్థిర శూన్య
నమోదు_మారింది_cb(GtkWidget * నమోదు_2, GtkWidget * dialog)
{
    స్థిరరాశి gchar	*నమోదు_పాఠ్యం;
    gboolean	nonempty;

    నమోదు_పాఠ్యం = gtk_నమోదు_get_పాఠ్యం(GTK_నమోదు(నమోదు_2));

    ఒకవేళ (!నమోదు_పాఠ్యం)
	తిరిగి;			// shouldn't happen

    nonempty = (నమోదు_పాఠ్యం[0] != '\0');

    ఒకవేళ (dialog == కనుగొను_widgets.dialog)
	gtk_widget_సమితి_sensitive(కనుగొను_widgets.కనుగొను, nonempty);

    ఒకవేళ (dialog == repl_widgets.dialog)
    {
	gtk_widget_సమితి_sensitive(repl_widgets.కనుగొను, nonempty);
	gtk_widget_సమితి_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_సమితి_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    శూన్య
ex_helpfind(exarg_T *eap వాడని)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#ఒకవేళ defined(FEAT_BROWSE)
    స్థిర శూన్య
recent_func_log_func(స్థిరరాశి gchar *log_domain వాడని,
		     GLogLevelFlags log_level వాడని,
		     స్థిరరాశి gchar *సందేశం_2 వాడని,
		     gpointer user_దత్తాంశం వాడని)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#అయితే_ముగింపు

    శూన్య
gui_mch_సమితి_fullscreen(పూర్ణాంకం flag)
{
    ఒకవేళ (flag)
	gtk_కిటికీ_fullscreen(GTK_కిటికీ(gui.mainwin));
    లేకపోతే
	gtk_కిటికీ_unfullscreen(GTK_కిటికీ(gui.mainwin));
}
