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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_GUI_GTK
# include "gui_gtk_f.h"
#endif

// GTK defines MAX and MIN, but some system header files as well.  Undefine
// them and don't use them.
#ifdef MIN
# undef MIN
#endif
#ifdef MAX
# undef MAX
#endif

#ifdef FEAT_GUI_GNOME
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
# ಯದಿ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#endif

#ifdef FEAT_GUI_GTK
# ಯದಿ GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ಅನ್ಯಥಾ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# ಅನ್ಯಥಾ
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#ಅನ್ಯಥಾ
// define these items to be able to generate prototypes without GTK
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ GtkWidget;
# define gpointer ಪೂರ್ಣಾಙ್ಕ
# define guint8 ಪೂರ್ಣಾಙ್ಕ
# define GdkPixmap ಪೂರ್ಣಾಙ್ಕ
# define GdkBitmap ಪೂರ್ಣಾಙ್ಕ
# define GtkIconFactory ಪೂರ್ಣಾಙ್ಕ
# define GtkToolbar ಪೂರ್ಣಾಙ್ಕ
# define GtkAdjustment ಪೂರ್ಣಾಙ್ಕ
# define gboolean ಪೂರ್ಣಾಙ್ಕ
# define Gdk_Event_ಕೀಲಿ ಪೂರ್ಣಾಙ್ಕ
# define CancelData ಪೂರ್ಣಾಙ್ಕ
#endif

ಸ್ಥಿರ ಶೂನ್ಯ entry_activate_cb(GtkWidget *widget, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ entry_changed_cb(GtkWidget *entry, GtkWidget *dialog);
ಸ್ಥಿರ ಶೂನ್ಯ find_replace_cb(GtkWidget *widget, gpointer data);
#ಯದಿ defined(FEAT_BROWSE)
ಸ್ಥಿರ ಶೂನ್ಯ recent_func_log_func(
	ಧ್ರುವ gchar *log_domain,
	GLogLevelFlags log_level,
	ಧ್ರುವ gchar *message,
	gpointer user_data);
#endif

#ಯದಿ defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# ಯದಿ GTK_CHECK_VERSION(3,10,0)
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ menu_themed_names[] =
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
#  ಯದಿ GTK_CHECK_VERSION(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  ಅನ್ಯಥಾ
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  ಯದಿ GTK_CHECK_VERSION(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  ಅನ್ಯಥಾ
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  ಯದಿ GTK_CHECK_VERSION(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  ಅನ್ಯಥಾ
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  ಯದಿ GTK_CHECK_VERSION(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  ಅನ್ಯಥಾ
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# ಅನ್ಯಥಾ // !GTK_CHECK_VERSION(3,10,0)
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ menu_stock_ids[] =
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
    /* 10 */ GTK_STOCK_FIND,
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
    /* 15 */ GTK_STOCK_EXECUTE,
    /* 16 */ GTK_STOCK_FIND_AND_REPLACE,
    /* 17 */ GTK_STOCK_CLOSE,		// FIXME: fuzzy
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
    /* 22 */ GTK_STOCK_GO_BACK,
    /* 23 */ GTK_STOCK_GO_FORWARD,
    /* 24 */ "vim-find-help",
    /* 25 */ GTK_STOCK_CONVERT,
    /* 26 */ GTK_STOCK_JUMP_TO,
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
    /* 31 */ GTK_STOCK_QUIT
};
# endif // !GTK_CHECK_VERSION(3,10,0)

# ifdef USE_GRESOURCE
#  ಯದಿ !GTK_CHECK_VERSION(3,10,0)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ IconNames {
    ಧ್ರುವ ವರ್ಣ *icon_name;
    ಧ್ರುವ ವರ್ಣ *file_name;
} IconNames;

ಸ್ಥಿರ IconNames stock_vim_icons[] = {
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
    { ರಿಕ್ತ, ರಿಕ್ತ }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    ಸ್ಥಿರ ಶೂನ್ಯ
add_stock_icon(GtkIconFactory	*factory,
	       ಧ್ರುವ ವರ್ಣ	*stock_id,
	       ಧ್ರುವ guint8	*inline_data,
	       ಪೂರ್ಣಾಙ್ಕ		data_length)
{
    GdkPixbuf	*pixbuf;
    GtkIconSet	*icon_set;

    pixbuf = gdk_pixbuf_new_from_inline(data_length, inline_data, FALSE, ರಿಕ್ತ);
    icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);

    gtk_icon_factory_add(factory, stock_id, icon_set);

    gtk_icon_set_unref(icon_set);
    g_object_unref(pixbuf);
}
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    ಯದಿ (mch_isFullName(dest))
    {
	ಪ್ರತ್ಯಾಗಮ vim_fexists(dest);
    }
    ಅನ್ಯಥಾ
    {
	ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	unsigned ಪೂರ್ಣಾಙ್ಕ	    i;

	ಪ್ರತಿ (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    ಯದಿ (gui_find_bitmap(dest, buf, (ವರ್ಣ *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    }

	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
}

    ಸ್ಥಿರ GtkWidget *
load_menu_iconfile(char_u *name, GtkIconSize icon_size)
{
    GtkWidget	    *image = ರಿಕ್ತ;
# ಯದಿ GTK_CHECK_VERSION(3,10,0)
    ಪೂರ್ಣಾಙ್ಕ		     pixel_size = -1;

    ವಿಕಲ್ಪ (icon_size)
    {
	ಪಕ್ಷ GTK_ICON_SIZE_MENU:
	    pixel_size = 16;
	    ಭಙ್ಗ;
	ಪಕ್ಷ GTK_ICON_SIZE_SMALL_TOOLBAR:
	    pixel_size = 16;
	    ಭಙ್ಗ;
	ಪಕ್ಷ GTK_ICON_SIZE_LARGE_TOOLBAR:
	    pixel_size = 24;
	    ಭಙ್ಗ;
	ಪಕ್ಷ GTK_ICON_SIZE_BUTTON:
	    pixel_size = 16;
	    ಭಙ್ಗ;
	ಪಕ್ಷ GTK_ICON_SIZE_DND:
	    pixel_size = 32;
	    ಭಙ್ಗ;
	ಪಕ್ಷ GTK_ICON_SIZE_DIALOG:
	    pixel_size = 48;
	    ಭಙ್ಗ;
	ಪಕ್ಷ GTK_ICON_SIZE_INVALID:
	    // FALLTHROUGH
	ಸ್ವಭಾವ:
	    pixel_size = 0;
	    ಭಙ್ಗ;
    }

    ಯದಿ (pixel_size > 0 || pixel_size == -1)
    {
	GdkPixbuf * ಧ್ರುವ pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((ಧ್ರುವ ವರ್ಣ *)name,
		pixel_size, pixel_size, TRUE, ರಿಕ್ತ);
	ಯದಿ (pixbuf != ರಿಕ್ತ)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    ಯದಿ (image == ರಿಕ್ತ)
	image = gtk_image_new_from_icon_name("image-missing", icon_size);

    ಪ್ರತ್ಯಾಗಮ image;
# ಅನ್ಯಥಾ // !GTK_CHECK_VERSION(3,10,0)
    GtkIconSet	    *icon_set;
    GtkIconSource   *icon_source;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_set = gtk_icon_set_new();
    icon_source = gtk_icon_source_new();

    gtk_icon_source_set_filename(icon_source, (ಧ್ರುವ ವರ್ಣ *)name);
    gtk_icon_set_add_source(icon_set, icon_source);

    image = gtk_image_new_from_icon_set(icon_set, icon_size);

    gtk_icon_source_free(icon_source);
    gtk_icon_set_unref(icon_set);

    ಪ್ರತ್ಯಾಗಮ image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    ಸ್ಥಿರ GtkWidget *
create_menu_icon(vimmenu_T *menu, GtkIconSize icon_size)
{
    GtkWidget	*image = ರಿಕ್ತ;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    ಯದಿ (menu->iconfile != ರಿಕ್ತ && lookup_menu_iconfile(menu->iconfile, buf))
	image = load_menu_iconfile(buf, icon_size);

    // If not found and not builtin specified try using the menu name.
    ಯದಿ (image == ರಿಕ್ತ && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = load_menu_iconfile(buf, icon_size);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    ಯದಿ (image == ರಿಕ್ತ)
    {
# ಯದಿ GTK_CHECK_VERSION(3,10,0)
	ಧ್ರುವ ವರ್ಣ *icon_name = ರಿಕ್ತ;
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ   n_names = G_N_ELEMENTS(menu_themed_names);

	ಯದಿ (menu->iconidx >= 0 && menu->iconidx < n_names)
	    icon_name = menu_themed_names[menu->iconidx];
	ಯದಿ (icon_name == ರಿಕ್ತ)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_size);
# ಅನ್ಯಥಾ
	ಧ್ರುವ ವರ್ಣ  *stock_id;
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ   n_ids = G_N_ELEMENTS(menu_stock_ids);

	ಯದಿ (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	ಅನ್ಯಥಾ
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_size);
# endif
    }

    ಪ್ರತ್ಯಾಗಮ image;
}

    ಸ್ಥಿರ gint
toolbar_button_focus_in_event(GtkWidget *widget UNUSED,
			      GdkEventFocus *event UNUSED,
			      gpointer data UNUSED)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    ಯದಿ (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif // FEAT_TOOLBAR

#ಯದಿ defined(FEAT_TOOLBAR)

    ಶೂನ್ಯ
gui_gtk_register_stock_icons(ಶೂನ್ಯ)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define ADD_ICON(Name, Data) add_stock_icon(factory, Name, Data, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(Data))

    ADD_ICON("vim-build-tags",		  stock_vim_build_tags);
    ADD_ICON("vim-find-help",		  stock_vim_find_help);
    ADD_ICON("vim-save-all",		  stock_vim_save_all);
    ADD_ICON("vim-session-load",	  stock_vim_session_load);
    ADD_ICON("vim-session-new",		  stock_vim_session_new);
    ADD_ICON("vim-session-save",	  stock_vim_session_save);
    ADD_ICON("vim-shell",		  stock_vim_shell);
    ADD_ICON("vim-window-maximize",	  stock_vim_window_maximize);
    ADD_ICON("vim-window-maximize-width", stock_vim_window_maximize_width);
    ADD_ICON("vim-window-minimize",	  stock_vim_window_minimize);
    ADD_ICON("vim-window-minimize-width", stock_vim_window_minimize_width);
    ADD_ICON("vim-window-split",	  stock_vim_window_split);
    ADD_ICON("vim-window-split-vertical", stock_vim_window_split_vertical);

#  undef ADD_ICON

    gtk_icon_factory_add_default(factory);
    g_object_unref(factory);
# ಅನ್ಯಥಾ // defined(USE_GRESOURCE)
    ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ path_prefix = "/org/vim/gui/icon";
#  ಯದಿ GTK_CHECK_VERSION(3,14,0)
    GdkScreen    *screen = ರಿಕ್ತ;
    GtkIconTheme *icon_theme = ರಿಕ್ತ;

    ಯದಿ (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    ಅನ್ಯಥಾ
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_add_resource_path(icon_theme, path_prefix);
#  elif GTK_CHECK_VERSION(3,0,0)
    IconNames *names;

    ಪ್ರತಿ (names = stock_vim_icons; names->icon_name != ರಿಕ್ತ; names++)
    {
	ವರ್ಣ path[MAXPATHL];

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	GdkPixbuf *pixbuf = ರಿಕ್ತ;
	pixbuf = gdk_pixbuf_new_from_resource(path, ರಿಕ್ತ);
	ಯದಿ (pixbuf != ರಿಕ್ತ)
	{
	    ಧ್ರುವ gint size = MAX(gdk_pixbuf_get_width(pixbuf),
				  gdk_pixbuf_get_height(pixbuf));
	    ಯದಿ (size > 16)
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
		ಯದಿ (pixbuf == ರಿಕ್ತ)
		    pixbuf = src;
		ಅನ್ಯಥಾ
		    g_object_unref(src);
	    }
	    gtk_icon_theme_add_builtin_icon(names->icon_name, size, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  ಅನ್ಯಥಾ // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * ಧ್ರುವ factory = gtk_icon_factory_new();
    IconNames *names;

    ಪ್ರತಿ (names = stock_vim_icons; names->icon_name != ರಿಕ್ತ; names++)
    {
	ವರ್ಣ path[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(path, ರಿಕ್ತ);
	ಯದಿ (pixbuf != ರಿಕ್ತ)
	{
	    GtkIconSet *icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);
	    gtk_icon_factory_add(factory, names->icon_name, icon_set);
	    gtk_icon_set_unref(icon_set);
	    g_object_unref(pixbuf);
	}
    }

    gtk_icon_factory_add_default(factory);
    g_object_unref(factory);
#  endif // !GTK_CHECK_VERSION(3,0,0)
# endif // defined(USE_GRESOURCE)
}

#endif // FEAT_TOOLBAR

#ಯದಿ defined(FEAT_MENU)

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
    ಸ್ಥಿರ char_u *
translate_mnemonic_tag(char_u *name, ಪೂರ್ಣಾಙ್ಕ use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    ಪೂರ್ಣಾಙ್ಕ	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ಯದಿ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ಯದಿ (buf != ರಿಕ್ತ)
    {
	pdest = buf;
	ಪ್ರತಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ಯದಿ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    ಪ್ರತ್ಯಾಗಮ buf;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_item_new(vimmenu_T *menu, GtkWidget *parent_widget)
{
    GtkWidget	*box;
    char_u	*text;
    ಪೂರ್ಣಾಙ್ಕ		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_item_new();
# ಯದಿ GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
# ಅನ್ಯಥಾ
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(parent_widget));
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((ಧ್ರುವ ವರ್ಣ *)text);
    vim_free(text);

    gtk_box_pack_start(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    ಯದಿ (menu->actext != ರಿಕ್ತ && menu->actext[0] != NUL)
    {
	text = CONVERT_TO_UTF8(menu->actext);

	gtk_box_pack_end(GTK_BOX(box),
			 gtk_label_new((ಧ್ರುವ ವರ್ಣ *)text),
			 FALSE, FALSE, 0);

	CONVERT_TO_UTF8_FREE(text);
    }

    gtk_container_add(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    ಶೂನ್ಯ
gui_mch_add_menu(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;

    ಯದಿ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	ಪ್ರತ್ಯಾಗಮ;
    }

    parent = menu->parent;

    ಯದಿ ((parent != ರಿಕ್ತ && parent->submenu_id == ರಿಕ್ತ)
	    || !menu_is_menubar(menu->name))
	ಪ್ರತ್ಯಾಗಮ;

    parent_widget = (parent != ರಿಕ್ತ) ? parent->submenu_id : gui.menubar;
    menu_item_new(menu, parent_widget);

# ಯದಿ !GTK_CHECK_VERSION(3,4,0)
    // since the tearoff should always appear first, increment idx
    ಯದಿ (parent != ರಿಕ್ತ && !menu_is_popup(parent->name))
	++idx;
# endif

    gtk_menu_shell_ಸೇರಿಸು(GTK_MENU_SHELL(parent_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_set_accel_group(GTK_MENU(menu->submenu_id), gui.accel_group);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->id), menu->submenu_id);

# ಯದಿ !GTK_CHECK_VERSION(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_item_new();
    ಯದಿ (vim_strchr(p_go, GO_TEAROFF) != ರಿಕ್ತ)
	gtk_widget_show(menu->tearoff_handle);
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  ಅನ್ಯಥಾ
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_item_activate(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_item_select(GtkWidget *widget UNUSED, gpointer data)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_ಸಂದೇಶ = FALSE;

    ಯದಿ (State & MODE_CMDLINE)
	ಪ್ರತ್ಯಾಗಮ;
    menu = (vimmenu_T *)data;
    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    ಯದಿ (tooltip != ರಿಕ್ತ && utf_valid_string(tooltip, ರಿಕ್ತ))
    {
	ಸಂದೇಶ((ವರ್ಣ *)tooltip);
	did_ಸಂದೇಶ = TRUE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    ಅನ್ಯಥಾ ಯದಿ (did_ಸಂದೇಶ)
    {
	ಸಂದೇಶ("");
	did_ಸಂದೇಶ = FALSE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    CONVERT_TO_UTF8_FREE(tooltip);
}

    ಶೂನ್ಯ
gui_mch_add_menu_item(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ idx)
{
    vimmenu_T *parent;

    parent = menu->parent;

# ifdef FEAT_TOOLBAR
    ಯದಿ (menu_is_toolbar(parent->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = ರಿಕ್ತ;

	ಯದಿ (menu_is_separator(menu->name))
	{
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
	    GtkToolItem *item = gtk_separator_tool_item_new();
	    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(item),
		    TRUE);
	    gtk_tool_item_set_expand(GTK_TOOL_ITEM(item), FALSE);
	    gtk_widget_show(GTK_WIDGET(item));

	    gtk_toolbar_ಸೇರಿಸು(toolbar, item, idx);
#  ಅನ್ಯಥಾ
	    gtk_toolbar_ಸೇರಿಸು_space(toolbar, idx);
#  endif
	    menu->id = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    char_u *text;
	    char_u *tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    ಯದಿ (tooltip != ರಿಕ್ತ && !utf_valid_string(tooltip, ರಿಕ್ತ))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_TO_UTF8_FREE(tooltip);

#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
	    {
		GtkWidget *icon;
		GtkToolItem *item;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_size(toolbar));
		item = gtk_tool_button_new(icon, (ಧ್ರುವ gchar *)text);
		gtk_tool_item_set_tooltip_text(item, (ಧ್ರುವ gchar *)tooltip);
		g_signal_connect(G_OBJECT(item), "clicked",
			G_CALLBACK(&menu_item_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(item));

		gtk_toolbar_ಸೇರಿಸು(toolbar, item, idx);

		menu->id = GTK_WIDGET(item);
	    }
#  ಅನ್ಯಥಾ
	    menu->id = gtk_toolbar_ಸೇರಿಸು_item(
		    toolbar,
		    (ಧ್ರುವ ವರ್ಣ *)text,
		    (ಧ್ರುವ ವರ್ಣ *)tooltip,
		    ರಿಕ್ತ,
		    create_menu_icon(menu, gtk_toolbar_get_icon_size(toolbar)),
		    G_CALLBACK(&menu_item_activate),
		    menu,
		    idx);
#  endif

	    ಯದಿ (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_in_event), ರಿಕ್ತ);

	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
    }
    ಅನ್ಯಥಾ
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	ಯದಿ (parent == ರಿಕ್ತ || parent->submenu_id == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

# ಯದಿ !GTK_CHECK_VERSION(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	ಯದಿ (!menu_is_popup(parent->name))
	    ++idx;
# endif

	ಯದಿ (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# ಯದಿ GTK_CHECK_VERSION(3,0,0)
	    menu->id = gtk_separator_menu_item_new();
# ಅನ್ಯಥಾ
	    menu->id = gtk_menu_item_new();
	    gtk_widget_set_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_ಸೇರಿಸು(GTK_MENU_SHELL(parent->submenu_id),
		    menu->id, idx);

	    ಪ್ರತ್ಯಾಗಮ;
	}

	// Add textual menu item.
	menu_item_new(menu, parent->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_ಸೇರಿಸು(GTK_MENU_SHELL(parent->submenu_id),
		menu->id, idx);

	ಯದಿ (menu->id != ರಿಕ್ತ)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_item_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_item_select), menu);
	}
    }
}
#endif // FEAT_MENU


    ಶೂನ್ಯ
gui_mch_set_text_area_pos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#ಯದಿ defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ಶೂನ್ಯ
gui_gtk_set_mnemonics(ಪೂರ್ಣಾಙ್ಕ enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ಯದಿ (menu->id == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	name = translate_mnemonic_tag(menu->name, enable);
	gtk_label_set_text_with_mnemonic(GTK_LABEL(menu->label),
					 (ಧ್ರುವ ವರ್ಣ *)name);
	vim_free(name);
    }
}

# ಯದಿ !GTK_CHECK_VERSION(3,4,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
recurse_tearoffs(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ val)
{
    ಪ್ರತಿ (; menu != ರಿಕ್ತ; menu = menu->next)
    {
	ಯದಿ (menu->submenu_id != ರಿಕ್ತ && menu->tearoff_handle != ರಿಕ್ತ
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    ಯದಿ (val)
		gtk_widget_show(menu->tearoff_handle);
	    ಅನ್ಯಥಾ
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# ಯದಿ GTK_CHECK_VERSION(3,4,0)
    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಙ್ಕ enable UNUSED)
{
    // Do nothing
}
# ಅನ್ಯಥಾ
    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಙ್ಕ enable)
{
    recurse_tearoffs(ಬೇರು_menu, enable);
}
# endif
#endif // FEAT_MENU

#ಯದಿ defined(FEAT_TOOLBAR)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*ಗಂಟು;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;

    ಪ್ರತಿ (ಗಂಟು = menu->parent->children; ಗಂಟು != menu; ಗಂಟು = ಗಂಟು->next)
    {
	g_return_val_if_fail(ಗಂಟು != ರಿಕ್ತ, -1);
	++idx;
    }

    ಪ್ರತ್ಯಾಗಮ idx;
}
#endif // FEAT_TOOLBAR


#ಯದಿ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    ಯದಿ (menu->id == ರಿಕ್ತ || menu->parent == ರಿಕ್ತ || gui.toolbar == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *tooltip;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    ಯದಿ (tooltip != ರಿಕ್ತ && utf_valid_string(tooltip, ರಿಕ್ತ))
# ಯದಿ GTK_CHECK_VERSION(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_set_tooltip_text(menu->id, (ಧ್ರುವ gchar *)tooltip);
# ಅನ್ಯಥಾ
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_set_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (ಧ್ರುವ ವರ್ಣ *)tooltip, ರಿಕ್ತ);
# endif
    CONVERT_TO_UTF8_FREE(tooltip);
}
#endif // FEAT_TOOLBAR


#ಯದಿ defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    ಯದಿ (menu->id != ರಿಕ್ತ)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    ಯದಿ (menu->id != ರಿಕ್ತ && gui.menubar != ರಿಕ್ತ
			    && gtk_widget_get_parent(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    ಯದಿ (menu->parent != ರಿಕ್ತ && menu_is_toolbar(menu->parent->name))
    {
	ಯದಿ (menu_is_separator(menu->name))
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
	{
	    GtkToolItem *item = ರಿಕ್ತ;

	    item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    ಯದಿ (item != ರಿಕ್ತ)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(item));
	}
#  ಅನ್ಯಥಾ
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	ಅನ್ಯಥಾ ಯದಿ (menu->id != ರಿಕ್ತ)
	    gtk_widget_destroy(menu->id);
    }
    ಅನ್ಯಥಾ
# endif // FEAT_TOOLBAR
    {
	ಯದಿ (menu->submenu_id != ರಿಕ್ತ)
	    gtk_widget_destroy(menu->submenu_id);

	ಯದಿ (menu->id != ರಿಕ್ತ)
	    gtk_widget_destroy(menu->id);
    }

    ಯದಿ (menu->id != ರಿಕ್ತ)
	g_object_unref(menu->id);
    menu->submenu_id = ರಿಕ್ತ;
    menu->id = ರಿಕ್ತ;
}
#endif // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    ಶೂನ್ಯ
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    ಯದಿ (sb->id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_set_lower(adjustment, 0.0);
    gtk_adjustment_set_ಮೌಲ್ಯ(adjustment, val);
    gtk_adjustment_set_upper(adjustment, max + 1);
    gtk_adjustment_set_page_size(adjustment, size);
    gtk_adjustment_set_page_increment(adjustment,
	    size < 3L ? 1L : size - 2L);
    gtk_adjustment_set_step_increment(adjustment, 1.0);

    g_signal_handler_block(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#ಯದಿ !GTK_CHECK_VERSION(3,18,0)
    gtk_adjustment_changed(adjustment);
#endif

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    ಶೂನ್ಯ
gui_mch_set_scrollbar_pos(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    ಯದಿ (sb->id != ರಿಕ್ತ)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ xpad;
#ಯದಿ GTK_CHECK_VERSION(3,0,0)
    xpad = gtk_widget_get_allocated_width(gui.formwin)
	  - gtk_widget_get_allocated_width(gui.drawarea) - gui.scrollbar_width;
#ಅನ್ಯಥಾ
    xpad = gui.formwin->allocation.width - gui.drawarea->allocation.width
							 - gui.scrollbar_width;
#endif
    ಯದಿ (gui.which_scrollbars[SBAR_ಎಡ] && gui.which_scrollbars[SBAR_ಬಲ])
	xpad -= gui.scrollbar_width;

    ಪ್ರತ್ಯಾಗಮ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ ypad;
#ಯದಿ GTK_CHECK_VERSION(3,0,0)
    ypad = gtk_widget_get_allocated_height(gui.formwin)
	- gtk_widget_get_allocated_height(gui.drawarea) - gui.scrollbar_height;
#ಅನ್ಯಥಾ
    ypad = gui.formwin->allocation.height - gui.drawarea->allocation.height
							- gui.scrollbar_height;
#endif
    ಪ್ರತ್ಯಾಗಮ (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
adjustment_ಮೌಲ್ಯ_changed(GtkAdjustment *adjustment, gpointer data)
{
    scrollbar_T	*sb;
    long	ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಙ್ಕ		dragging = FALSE;

#ifdef FEAT_XIM
    // cancel any preediting
    ಯದಿ (im_is_preediting())
	xim_reset();
#endif

    sb = gui_find_scrollbar((long)data);
    ಮೌಲ್ಯ = gtk_adjustment_get_ಮೌಲ್ಯ(adjustment);
#ಯದಿ !GTK_CHECK_VERSION(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    ಯದಿ (sb != ರಿಕ್ತ)
    {
	dragging = TRUE;

	ಯದಿ (sb->wp != ರಿಕ್ತ && GDK_IS_DRAWABLE(sb->id->window))
	{
	    ಪೂರ್ಣಾಙ್ಕ			x;
	    ಪೂರ್ಣಾಙ್ಕ			y;
	    GdkModifierType	state;
	    ಪೂರ್ಣಾಙ್ಕ			width;
	    ಪೂರ್ಣಾಙ್ಕ			height;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_window_get_pointer(sb->id->window, &x, &y, &state);
	    gdk_window_get_size(sb->id->window, &width, &height);
	    ಯದಿ (x >= 0 && x < width && y >= 0 && y < height)
	    {
		ಯದಿ (y < width)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    ಮೌಲ್ಯ = sb->wp->w_topline - 2;
		}
		ಅನ್ಯಥಾ ಯದಿ (y > height - width)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    ಮೌಲ್ಯ = sb->wp->w_topline;
		}
	    }
	}
    }
#endif // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    ಶೂನ್ಯ
gui_mch_create_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ orient)
{
    ಯದಿ (orient == SBAR_HORIZ)
#ಯದಿ GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
	sb->id = gtk_hscrollbar_new(ರಿಕ್ತ);
#endif
    ಅನ್ಯಥಾ ಯದಿ (orient == SBAR_VERT)
#ಯದಿ GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
	sb->id = gtk_vscrollbar_new(ರಿಕ್ತ);
#endif

    ಯದಿ (sb->id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    GtkAdjustment *adjustment;

    gtk_widget_set_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_ಮೌಲ್ಯ_changed),
	    GINT_TO_POINTER(sb->ident));
    gui_mch_update();
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ಯದಿ (sb->id != ರಿಕ್ತ)
    {
	gtk_widget_destroy(sb->id);
	sb->id = ರಿಕ್ತ;
    }
    gui_mch_update();
}

#ಯದಿ defined(FEAT_BROWSE)
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
gui_mch_browse(ಪೂರ್ಣಾಙ್ಕ saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext UNUSED,
	       char_u *initdir,
	       char_u *filter)
{
# ಯದಿ GTK_CHECK_VERSION(3,20,0)
    GtkFileChooserNative	*fc;
# ಅನ್ಯಥಾ
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    ಧ್ರುವ gchar		*domain = "Gtk";

    title = CONVERT_TO_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    ಯದಿ (initdir == ರಿಕ್ತ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ಅನ್ಯಥಾ ಯದಿ (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    add_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_set_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, ರಿಕ್ತ);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# ಯದಿ GTK_CHECK_VERSION(3,20,0)
    fc = gtk_file_chooser_native_new(
# ಅನ್ಯಥಾ
    fc = gtk_file_chooser_dialog_new(
# endif
	    (ಧ್ರುವ gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# ಯದಿ GTK_CHECK_VERSION(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# ಅನ್ಯಥಾ
#  ಯದಿ GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  ಅನ್ಯಥಾ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    ರಿಕ್ತ);
# endif
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fc),
						       (ಧ್ರುವ gchar *)dirbuf);

    ಯದಿ (filter != ರಿಕ್ತ && *filter != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ಯದಿ (patt != ರಿಕ್ತ)
	{
	    ಯಾವತ್ (p != ರಿಕ್ತ && *p != NUL)
	    {
		ಯದಿ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ಯದಿ (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    ಅನ್ಯಥಾ
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			ಯದಿ (*p == '\n')
			{
			    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    ಯದಿ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ಅನ್ಯಥಾ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    ಯದಿ (saving && dflt != ರಿಕ್ತ && *dflt != NUL)
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fc), (ವರ್ಣ *)dflt);

    gui.browse_fname = ರಿಕ್ತ;
# ಯದಿ GTK_CHECK_VERSION(3,20,0)
    ಯದಿ (gtk_native_dialog_run(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# ಅನ್ಯಥಾ
    ಯದಿ (gtk_dialog_run(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	ವರ್ಣ *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# ಯದಿ GTK_CHECK_VERSION(3,20,0)
    g_object_unref(fc);
# ಅನ್ಯಥಾ
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_TO_UTF8_FREE(title);
    ಯದಿ (gui.browse_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // shorten the file name if possible
    ಪ್ರತ್ಯಾಗಮ vim_strsave(shorten_fname1(gui.browse_fname));
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
# ಯದಿ defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = ರಿಕ್ತ;

    title = CONVERT_TO_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (ಧ್ರುವ gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  ಯದಿ GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  ಅನ್ಯಥಾ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    ರಿಕ್ತ);

    CONVERT_TO_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    ಯದಿ (initdir == ರಿಕ್ತ || *initdir == NUL
	       || vim_FullName(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    add_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dirdlg),
						      (ಧ್ರುವ gchar *)dirbuf);

    // Run the dialog.
    ಯದಿ (gtk_dialog_run(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    ಯದಿ (dirname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    ಪ್ರತ್ಯಾಗಮ p;

# ಅನ್ಯಥಾ // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    ಪ್ರತ್ಯಾಗಮ gui_mch_browse(0, title, ರಿಕ್ತ, ರಿಕ್ತ, initdir, ರಿಕ್ತ);
# endif // !defined(GTK_FILE_CHOOSER)
}


#endif	// FEAT_BROWSE

#ಯದಿ defined(FEAT_GUI_DIALOG)

    ಸ್ಥಿರ GtkWidget *
create_message_dialog(ಪೂರ್ಣಾಙ್ಕ type, char_u *title, char_u *message)
{
    GtkWidget	    *dialog;
    GtkMessageType  message_type;

    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ VIM_ERROR:	    message_type = GTK_MESSAGE_ERROR;	 ಭಙ್ಗ;
	ಪಕ್ಷ VIM_WARNING:   message_type = GTK_MESSAGE_WARNING;	 ಭಙ್ಗ;
	ಪಕ್ಷ VIM_QUESTION:  message_type = GTK_MESSAGE_QUESTION; ಭಙ್ಗ;
	ಸ್ವಭಾವ:	    message_type = GTK_MESSAGE_INFO;	 ಭಙ್ಗ;
    }

    message = CONVERT_TO_UTF8(message);
    dialog  = gtk_message_dialog_new(GTK_WINDOW(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_PARENT,
				     message_type,
				     GTK_BUTTONS_NONE,
				     "%s", (ಧ್ರುವ ವರ್ಣ *)message);
    CONVERT_TO_UTF8_FREE(message);

    ಯದಿ (title != ರಿಕ್ತ)
    {
	title = CONVERT_TO_UTF8(title);
	gtk_window_set_title(GTK_WINDOW(dialog), (ಧ್ರುವ ವರ್ಣ *)title);
	CONVERT_TO_UTF8_FREE(title);
    }
    ಅನ್ಯಥಾ ಯದಿ (type == VIM_GENERIC)
    {
	gtk_window_set_title(GTK_WINDOW(dialog), "VIM");
    }

    ಪ್ರತ್ಯಾಗಮ dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    ಸ್ಥಿರ ವರ್ಣ **
split_button_string(char_u *button_string, ಪೂರ್ಣಾಙ್ಕ *n_buttons)
{
    ವರ್ಣ	    **array;
    char_u	    *p;
    unsigned ಪೂರ್ಣಾಙ್ಕ    ಎಣಿಕೆ = 1;

    ಪ್ರತಿ (p = button_string; *p != NUL; ++p)
	ಯದಿ (*p == DLG_BUTTON_SEP)
	    ++ಎಣಿಕೆ;

    array = ALLOC_MULT(ವರ್ಣ *, ಎಣಿಕೆ + 1);
    ಎಣಿಕೆ = 0;

    ಯದಿ (array != ರಿಕ್ತ)
    {
	array[ಎಣಿಕೆ++] = (ವರ್ಣ *)button_string;
	ಪ್ರತಿ (p = button_string; *p != NUL; )
	{
	    ಯದಿ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ಎಣಿಕೆ++] = (ವರ್ಣ *)p;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ಅನ್ಯಥಾ
		MB_PTR_ADV(p);
	}
	array[ಎಣಿಕೆ] = ರಿಕ್ತ; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ array;
}

# ಯದಿ !GTK_CHECK_VERSION(3,10,0)
    ಸ್ಥಿರ ವರ್ಣ **
split_button_translation(ಧ್ರುವ ವರ್ಣ *message)
{
    ವರ್ಣ    **buttons = ರಿಕ್ತ;
    char_u  *str;
    ಪೂರ್ಣಾಙ್ಕ	    n_buttons = 0;
    ಪೂರ್ಣಾಙ್ಕ	    n_expected = 1;

    ಪ್ರತಿ (str = (char_u *)message; *str != NUL; ++str)
	ಯದಿ (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(message);
    ಯದಿ (str != ರಿಕ್ತ)
    {
	ಯದಿ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    str = vim_strsave(str);

	ಯದಿ (str != ರಿಕ್ತ)
	    buttons = split_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    ಯದಿ (buttons == ರಿಕ್ತ || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = ರಿಕ್ತ;
	str = vim_strsave((char_u *)message);

	ಯದಿ (str != ರಿಕ್ತ)
	    buttons = split_button_string(str, &n_buttons);
	ಯದಿ (buttons == ರಿಕ್ತ)
	    vim_free(str);
    }

    ಪ್ರತ್ಯಾಗಮ buttons;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
button_equal(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b)
{
    ಯಾವತ್ (*a != '\0' && *b != '\0')
    {
	ಯದಿ (*a == '_' && *++a == '\0')
	    ಭಙ್ಗ;
	ಯದಿ (*b == '_' && *++b == '\0')
	    ಭಙ್ಗ;

	ಯದಿ (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	a = g_utf8_next_char(a);
	b = g_utf8_next_char(b);
    }

    ಪ್ರತ್ಯಾಗಮ (*a == '\0' && *b == '\0');
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
dialog_add_buttons(GtkDialog *dialog, char_u *button_string)
{
# ಯದಿ !GTK_CHECK_VERSION(3,10,0)
    ವರ್ಣ    **ok;
    ವರ್ಣ    **ync;  // "yes no cancel"
# endif
    ವರ್ಣ    **buttons;
    ಪೂರ್ಣಾಙ್ಕ	    n_buttons = 0;
    ಪೂರ್ಣಾಙ್ಕ	    idx;

    button_string = vim_strsave(button_string); // must be writable
    ಯದಿ (button_string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ಯದಿ (vim_strchr(p_go, GO_VERTICAL) != ರಿಕ್ತ)
    {
# ಯದಿ !GTK_CHECK_VERSION(3,0,0)
	GtkWidget	*vbutton_box;

	vbutton_box = gtk_vbutton_box_new();
	gtk_widget_show(vbutton_box);
	gtk_box_pack_end(GTK_BOX(GTK_DIALOG(dialog)->vbox),
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
# ಯದಿ !GTK_CHECK_VERSION(3,10,0)
    ok	    = split_button_translation(N_("&Ok"));
    ync     = split_button_translation(N_("&Yes\n&No\n&Cancel"));
# endif
    buttons = split_button_string(button_string, &n_buttons);

    /*
     * Yes, the buttons are in reversed order to match the GNOME 2 desktop
     * environment.  Don't hit me -- it's all about consistency.
     * Well, apparently somebody changed his mind: with GTK 2.2.4 it works the
     * other way around...
     */
    ಪ್ರತಿ (idx = 1; idx <= n_buttons; ++idx)
    {
	ವರ್ಣ	*label;
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
# ಯದಿ !GTK_CHECK_VERSION(3,10,0)
	ಯದಿ (ok != ರಿಕ್ತ && ync != ರಿಕ್ತ) // almost impossible to fail
	{
	    ಯದಿ	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    ಅನ್ಯಥಾ ಯದಿ (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    ಅನ್ಯಥಾ ಯದಿ (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    ಅನ್ಯಥಾ ಯದಿ (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    ಅನ್ಯಥಾ ಯದಿ (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    ಅನ್ಯಥಾ ಯದಿ (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    ಅನ್ಯಥಾ ಯದಿ (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    ಅನ್ಯಥಾ ಯದಿ (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_TO_UTF8((char_u *)label);
	gtk_dialog_add_button(dialog, (ಧ್ರುವ gchar *)label8, idx);
	CONVERT_TO_UTF8_FREE(label8);
    }

# ಯದಿ !GTK_CHECK_VERSION(3,10,0)
    ಯದಿ (ok != ರಿಕ್ತ)
	vim_free(*ok);
    ಯದಿ (ync != ರಿಕ್ತ)
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _DialogInfo
{
    ಪೂರ್ಣಾಙ್ಕ		ignore_enter;	    // no default button, ignore "Enter"
    ಪೂರ್ಣಾಙ್ಕ		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    ಸ್ಥಿರ gboolean
dialog_ಕೀಲಿ_press_event_cb(GtkWidget *widget, Gdk_Event_ಕೀಲಿ *event, gpointer data)
{
    DialogInfo *di = (DialogInfo *)data;

    // Ignore hitting Enter (or Space) when there is no default button.
    ಯದಿ (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಅನ್ಯಥಾ    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    ಯದಿ (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_mask()) == 0)
    {
	ಪ್ರತ್ಯಾಗಮ gtk_window_mnemonic_activate(
		   GTK_WINDOW(widget), event->keyval,
		   gtk_window_get_mnemonic_modifier(GTK_WINDOW(widget)));
    }

    ಪ್ರತ್ಯಾಗಮ FALSE; // continue emission
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_dialog(ಪೂರ್ಣಾಙ್ಕ	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*message,   // message text
	       char_u	*buttons,   // names of buttons
	       ಪೂರ್ಣಾಙ್ಕ	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       ಪೂರ್ಣಾಙ್ಕ	ex_cmd UNUSED)
{
    GtkWidget	*dialog;
    GtkWidget	*entry = ರಿಕ್ತ;
    char_u	*text;
    ಪೂರ್ಣಾಙ್ಕ		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_active;

    dialog = create_message_dialog(type, title, message);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_add_buttons(GTK_DIALOG(dialog), buttons);

    ಯದಿ (textfield != ರಿಕ್ತ)
    {
	GtkWidget *alignment;

	entry = gtk_entry_new();
	gtk_widget_show(entry);

	// Make Enter work like pressing OK.
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

	text = CONVERT_TO_UTF8(textfield);
	gtk_entry_set_text(GTK_ENTRY(entry), (ಧ್ರುವ ವರ್ಣ *)text);
	CONVERT_TO_UTF8_FREE(text);

# ಯದಿ GTK_CHECK_VERSION(3,14,0)
	gtk_widget_set_halign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_valign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(entry), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# ಅನ್ಯಥಾ
	alignment = gtk_alignment_new((ಭಿನ್ನ)0.5, (ಭಿನ್ನ)0.5,
						      (ಭಿನ್ನ)1.0, (ಭಿನ್ನ)1.0);
# endif
	gtk_container_add(GTK_CONTAINER(alignment), entry);
	gtk_container_set_border_width(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# ಯದಿ GTK_CHECK_VERSION(3,0,0)
	{
	    GtkWidget * ಧ್ರುವ vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_start(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# ಅನ್ಯಥಾ
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    ಅನ್ಯಥಾ
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_ಕೀಲಿ_press_event_cb), &dialoginfo);

    ಯದಿ (def_but > 0)
    {
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    ಅನ್ಯಥಾ
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    ಯದಿ (response != GTK_RESPONSE_NONE)
    {
	ಯದಿ (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	ಯದಿ (textfield != ರಿಕ್ತ)
	{
	    text = (char_u *)gtk_entry_get_text(GTK_ENTRY(entry));
	    text = CONVERT_FROM_UTF8(text);

	    vim_strncpy(textfield, text, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(text);
	}
	gtk_widget_destroy(dialog);
    }

    ಯದಿ (gui.dialogs_active > 0)
	--gui.dialogs_active;
    ಪ್ರತ್ಯಾಗಮ response > 0 ? response : 0;
}

#endif // FEAT_GUI_DIALOG


#ಯದಿ defined(FEAT_MENU)

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# ಯದಿ defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  ಯದಿ !GTK_CHECK_VERSION(3,10,0)
    ಯದಿ (xic != ರಿಕ್ತ && g_object_get_data(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == ರಿಕ್ತ)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_TO_UTF8(name);
	menuitem = gtk_menu_item_new_with_mnemonic((ಧ್ರುವ ವರ್ಣ *)name);
	CONVERT_TO_UTF8_FREE(name);
	gtk_widget_show(menuitem);

	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	gtk_im_multicontext_append_menuitems(GTK_IM_MULTICONTEXT(xic),
					     GTK_MENU_SHELL(submenu));
	g_object_set_data(G_OBJECT(menu->submenu_id),
			  "vim-has-im-menu", GINT_TO_POINTER(TRUE));
    }
#  endif
# endif // FEAT_XIM

# ಯದಿ GTK_CHECK_VERSION(3,22,2)
    {
	GdkEventButton trigger;

	// A pseudo event to have gtk_menu_popup_at_pointer() work. Since the
	// function calculates the popup menu position on the basis of the
	// actual pointer position when it is invoked, the fields x, y, x_root
	// and y_root are set to zero for convenience.
	trigger.type       = GDK_BUTTON_PRESS;
	trigger.window     = gtk_widget_get_window(gui.drawarea);
	trigger.send_event = FALSE;
	trigger.time       = gui.event_time;
	trigger.x	   = 0.0;
	trigger.y	   = 0.0;
	trigger.axes       = ರಿಕ್ತ;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = ರಿಕ್ತ;
	trigger.x_ಬೇರು     = 0.0;
	trigger.y_ಬೇರು     = 0.0;

	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# ಅನ್ಯಥಾ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   ರಿಕ್ತ, ರಿಕ್ತ,
		   (GtkMenuPositionFunc)ರಿಕ್ತ, ರಿಕ್ತ,
		   3U, gui.event_time);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_menu_position_func(GtkMenu *menu UNUSED,
			 gint *x, gint *y,
			 gboolean *ತಳ್ಳು_in UNUSED,
			 gpointer user_data UNUSED)
{
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), x, y);

    ಯದಿ (popup_mouse_pos)
    {
	ಪೂರ್ಣಾಙ್ಕ	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin != ರಿಕ್ತ && gui.drawarea != ರಿಕ್ತ &&
	     gtk_widget_get_window(gui.drawarea) != ರಿಕ್ತ)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    ಶೂನ್ಯ
gui_make_popup(char_u *path_name, ಪೂರ್ಣಾಙ್ಕ mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_find_menu(path_name);
    ಯದಿ (menu == ರಿಕ್ತ || menu->submenu_id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

# ಯದಿ GTK_CHECK_VERSION(3,22,2)
    GdkWindow * ಧ್ರುವ win = gtk_widget_get_window(gui.drawarea);
    GdkEventButton trigger;

    // A pseudo event to have gtk_menu_popup_at_*() functions work. Since
    // the position where the menu pops up is automatically adjusted by
    // the functions, none of the fields x, y, x_root and y_root has to be
    // set to a specific value here; therefore, they are set to zero for
    // convenience.
    trigger.type       = GDK_BUTTON_PRESS;
    trigger.window     = win;
    trigger.send_event = FALSE;
    trigger.time       = GDK_CURRENT_TIME;
    trigger.x	   = 0.0;
    trigger.y	   = 0.0;
    trigger.axes       = ರಿಕ್ತ;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = ರಿಕ್ತ;
    trigger.x_ಬೇರು     = 0.0;
    trigger.y_ಬೇರು     = 0.0;

    ಯದಿ (mouse_pos)
	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    ಅನ್ಯಥಾ
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_window_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(ರಿಕ್ತ, &rect.x, &rect.y, ರಿಕ್ತ, ರಿಕ್ತ);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# ಅನ್ಯಥಾ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    ರಿಕ್ತ, ರಿಕ್ತ,
	    &popup_menu_position_func, ರಿಕ್ತ,
	    0U, (guint32)GDK_CURRENT_TIME);
# endif
}

#endif // FEAT_MENU


/*
 * We don't create it twice.
 */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _SharedFindReplace
{
    GtkWidget *dialog;	// the main dialog widget
    GtkWidget *wword;	// 'Whole word only' check button
    GtkWidget *mcase;	// 'Match case' check button
    GtkWidget *up;	// search direction 'Up' radio button
    GtkWidget *down;	// search direction 'Down' radio button
    GtkWidget *what;	// 'Find what' entry text widget
    GtkWidget *with;	// 'Replace with' entry text widget
    GtkWidget *find;	// 'Find Next' action button
    GtkWidget *replace;	// 'Replace With' action button
    GtkWidget *all;	// 'Replace All' action button
} SharedFindReplace;

ಸ್ಥಿರ SharedFindReplace find_widgets = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
ಸ್ಥಿರ SharedFindReplace repl_widgets = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_ಕೀಲಿ_press_event(
		GtkWidget	*widget UNUSED,
		Gdk_Event_ಕೀಲಿ	*event,
		SharedFindReplace *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ಯದಿ (event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // the Escape key synthesizes a cancellation action
    ಯದಿ (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ GtkWidget *
#ಯದಿ GTK_CHECK_VERSION(3,10,0)
create_image_button(ಧ್ರುವ ವರ್ಣ *stock_id UNUSED,
		    ಧ್ರುವ ವರ್ಣ *label)
#ಅನ್ಯಥಾ
create_image_button(ಧ್ರುವ ವರ್ಣ *stock_id,
		    ಧ್ರುವ ವರ್ಣ *label)
#endif
{
    char_u	*text;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    text = CONVERT_TO_UTF8((char_u *)label);

#ಯದಿ GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
#ಅನ್ಯಥಾ
    box = gtk_hbox_new(FALSE, 3);
#endif
#ಯದಿ !GTK_CHECK_VERSION(3,10,0)
    ಯದಿ (stock_id != ರಿಕ್ತ)
	gtk_box_pack_start(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_BUTTON),
			   FALSE, FALSE, 0);
#endif
    gtk_box_pack_start(GTK_BOX(box),
		       gtk_label_new((ಧ್ರುವ ವರ್ಣ *)text),
		       FALSE, FALSE, 0);

    CONVERT_TO_UTF8_FREE(text);

#ಯದಿ GTK_CHECK_VERSION(3,14,0)
    gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#ಅನ್ಯಥಾ
    alignment = gtk_alignment_new((ಭಿನ್ನ)0.5, (ಭಿನ್ನ)0.5,
						      (ಭಿನ್ನ)0.0, (ಭಿನ್ನ)0.0);
#endif
    gtk_container_add(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), alignment);

    ಪ್ರತ್ಯಾಗಮ button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *
convert_localized_message(char_u **buffer, ಧ್ರುವ ವರ್ಣ *message)
{
    ಯದಿ (output_conv.vc_type == CONV_NONE)
	ಪ್ರತ್ಯಾಗಮ message;

    vim_free(*buffer);
    *buffer = string_convert(&output_conv, (char_u *)message, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ ವರ್ಣ *)*buffer;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    ಸ್ಥಿರ unsigned long
entry_get_text_length(GtkEntry *entry)
{
    g_return_val_if_fail(entry != ರಿಕ್ತ, 0);
    g_return_val_if_fail(GTK_IS_ENTRY(entry) == TRUE, 0);

#ಯದಿ GTK_CHECK_VERSION(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    ಪ್ರತ್ಯಾಗಮ gtk_entry_buffer_get_length(gtk_entry_get_buffer(entry));
#ನೋಚೇದ್ಯದಿ GTK_CHECK_VERSION(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    ಪ್ರತ್ಯಾಗಮ gtk_entry_get_text_length(entry);
#ಅನ್ಯಥಾ
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    ಪ್ರತ್ಯಾಗಮ g_utf8_strlen(gtk_entry_get_text(entry), -1);
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_dialog_create(char_u *arg, ಪೂರ್ಣಾಙ್ಕ do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*table;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    SharedFindReplace *frdp;
    char_u	*entry_text;
    ಪೂರ್ಣಾಙ್ಕ		wword = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		mcase = !p_ic;
    char_u	*conv_buffer = ರಿಕ್ತ;
#define CONV(message) convert_localized_message(&conv_buffer, (message))

    frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    ಯದಿ (entry_text != ರಿಕ್ತ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_text = entry_text;
	entry_text = string_convert(&output_conv, entry_text, ರಿಕ್ತ);
	vim_free(old_text);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    ಯದಿ (frdp->dialog)
    {
	ಯದಿ (entry_text != ರಿಕ್ತ)
	{
	    gtk_entry_set_text(GTK_ENTRY(frdp->what), (ವರ್ಣ *)entry_text);
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->wword),
							     (gboolean)wword);
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
	}
	gtk_window_present(GTK_WINDOW(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	ಯದಿ (do_replace && entry_get_text_length(GTK_ENTRY(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	ಪ್ರತ್ಯಾಗಮ;
    }

    frdp->dialog = gtk_dialog_new();
#ಯದಿ GTK_CHECK_VERSION(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#ಅನ್ಯಥಾ
    gtk_dialog_set_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#endif
    gtk_window_set_transient_for(GTK_WINDOW(frdp->dialog), GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(frdp->dialog), TRUE);

    ಯದಿ (do_replace)
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    ಅನ್ಯಥಾ
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#ಯದಿ GTK_CHECK_VERSION(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
#ಅನ್ಯಥಾ
    hbox = gtk_hbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
#ಯದಿ GTK_CHECK_VERSION(3,0,0)
    {
	GtkWidget * ಧ್ರುವ dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    }
#ಅನ್ಯಥಾ
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#endif

    ಯದಿ (do_replace)
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#ಅನ್ಯಥಾ
	table = gtk_table_new(1024, 4, FALSE);
#endif
    ಅನ್ಯಥಾ
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#ಅನ್ಯಥಾ
	table = gtk_table_new(1024, 3, FALSE);
#endif
    gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(table), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#ಯದಿ GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#ನೋಚೇದ್ಯದಿ GTK_CHECK_VERSION(3,14,0)
    {
	GValue align_val = G_ಮೌಲ್ಯ_INIT;

	g_ಮೌಲ್ಯ_init(&align_val, G_TYPE_FLOAT);

	g_ಮೌಲ್ಯ_set_float(&align_val, 0.0);
	g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	g_ಮೌಲ್ಯ_set_float(&align_val, 0.5);
	g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	g_ಮೌಲ್ಯ_unset(&align_val);
    }
#ಅನ್ಯಥಾ
    gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), tmp, 0, 0, 2, 1);
#ಅನ್ಯಥಾ
    gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    frdp->what = gtk_entry_new();
    sensitive = (entry_text != ರಿಕ್ತ && entry_text[0] != NUL);
    ಯದಿ (entry_text != ರಿಕ್ತ)
	gtk_entry_set_text(GTK_ENTRY(frdp->what), (ವರ್ಣ *)entry_text);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(entry_changed_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(find_ಕೀಲಿ_press_event),
			   (gpointer) frdp);
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), frdp->what, 2, 0, 5, 1);
#ಅನ್ಯಥಾ
    gtk_table_attach(GTK_TABLE(table), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    ಯದಿ (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#ಯದಿ GTK_CHECK_VERSION(3,16,0)
	gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#ನೋಚೇದ್ಯದಿ GTK_CHECK_VERSION(3,14,0)
	{
	    GValue align_val = G_ಮೌಲ್ಯ_INIT;

	    g_ಮೌಲ್ಯ_init(&align_val, G_TYPE_FLOAT);

	    g_ಮೌಲ್ಯ_set_float(&align_val, 0.0);
	    g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_ಮೌಲ್ಯ_set_float(&align_val, 0.5);
	    g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_ಮೌಲ್ಯ_unset(&align_val);
	}
#ಅನ್ಯಥಾ
	gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 0, 1, 2, 1);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
	frdp->with = gtk_entry_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(find_ಕೀಲಿ_press_event),
			       (gpointer) frdp);
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->with, 2, 1, 5, 1);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), frdp->with, 1, 1024, 1, 2,
			 GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#endif

	/*
	 * Make the entry activation only change the input focus onto the
	 * with item.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(entry_activate_cb), frdp->with);
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Make the entry activation do the search.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_FINDNEXT));
    }

    // whole word only button
    frdp->wword = gtk_check_button_new_with_label(CONV(_("Match whole word only")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->wword),
							(gboolean)wword);
    ಯದಿ (do_replace)
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 2, 5, 1);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    ಅನ್ಯಥಾ
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 3, 5, 1);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    ಯದಿ (do_replace)
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 3, 5, 1);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    ಅನ್ಯಥಾ
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 4, 5, 1);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    tmp = gtk_frame_new(CONV(_("Direction")));
    ಯದಿ (do_replace)
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 2, 4);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
    ಅನ್ಯಥಾ
#ಯದಿ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 1, 3);
#ಅನ್ಯಥಾ
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
#ಯದಿ GTK_CHECK_VERSION(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
#ಅನ್ಯಥಾ
    vbox = gtk_vbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
    gtk_container_add(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->up = gtk_radio_button_new_with_label(ರಿಕ್ತ, CONV(_("Up")));
    gtk_box_pack_start(GTK_BOX(vbox), frdp->up, TRUE, TRUE, 0);
    frdp->down = gtk_radio_button_new_with_label(
			gtk_radio_button_get_group(GTK_RADIO_BUTTON(frdp->up)),
			CONV(_("Down")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->down), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_start(GTK_BOX(vbox), frdp->down, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#ಯದಿ GTK_CHECK_VERSION(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#ಅನ್ಯಥಾ
    actionarea = gtk_vbutton_box_new();
#endif
    gtk_container_set_border_width(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_end(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#ಯದಿ GTK_CHECK_VERSION(3,10,0)
    frdp->find = create_image_button(ರಿಕ್ತ, _("Find Next"));
#ಅನ್ಯಥಾ
    frdp->find = create_image_button(GTK_STOCK_FIND, _("Find Next"));
#endif
    gtk_widget_set_sensitive(frdp->find, sensitive);

    g_signal_connect(G_OBJECT(frdp->find), "clicked",
		     G_CALLBACK(find_replace_cb),
		     (do_replace) ? GINT_TO_POINTER(FRD_R_FINDNEXT)
				  : GINT_TO_POINTER(FRD_FINDNEXT));

    gtk_widget_set_can_default(frdp->find, TRUE);
    gtk_box_pack_start(GTK_BOX(actionarea), frdp->find, FALSE, FALSE, 0);
    gtk_widget_grab_default(frdp->find);

    ಯದಿ (do_replace)
    {
	// 'Replace' button
#ಯದಿ GTK_CHECK_VERSION(3,10,0)
	frdp->replace = create_image_button(ರಿಕ್ತ, _("Replace"));
#ಅನ್ಯಥಾ
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#endif
	gtk_widget_set_sensitive(frdp->replace, sensitive);
	gtk_widget_set_can_default(frdp->find, TRUE);
	gtk_box_pack_start(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_REPLACE));

	// 'Replace All' button
#ಯದಿ GTK_CHECK_VERSION(3,10,0)
	frdp->all = create_image_button(ರಿಕ್ತ, _("Replace All"));
#ಅನ್ಯಥಾ
	frdp->all = create_image_button(GTK_STOCK_CONVERT, _("Replace All"));
#endif
	gtk_widget_set_sensitive(frdp->all, sensitive);
	gtk_widget_set_can_default(frdp->all, TRUE);
	gtk_box_pack_start(GTK_BOX(actionarea), frdp->all, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->all), "clicked",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_REPLACEALL));
    }

    // 'Cancel' button
#ಯದಿ GTK_CHECK_VERSION(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#ಅನ್ಯಥಾ
    tmp = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#endif
    gtk_widget_set_can_default(tmp, TRUE);
    gtk_box_pack_end(GTK_BOX(actionarea), tmp, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(tmp),
			     "clicked", G_CALLBACK(gtk_widget_hide),
			     G_OBJECT(frdp->dialog));
    g_signal_connect_swapped(G_OBJECT(frdp->dialog),
			     "delete-event", G_CALLBACK(gtk_widget_hide_on_delete),
			     G_OBJECT(frdp->dialog));

#ಯದಿ GTK_CHECK_VERSION(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#ಅನ್ಯಥಾ
    tmp = gtk_vseparator_new();
#endif
    gtk_box_pack_end(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#ಯದಿ GTK_CHECK_VERSION(3,0,0)
# ಯದಿ !GTK_CHECK_VERSION(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#ಅನ್ಯಥಾ
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#endif
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(entry_text);
    vim_free(conv_buffer);
#undef CONV
}

    ಶೂನ್ಯ
gui_mch_find_dialog(exarg_T *eap)
{
    ಯದಿ (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
    ಯದಿ (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    ಪೂರ್ಣಾಙ್ಕ			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = (ಪೂರ್ಣಾಙ್ಕ)(long)data;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    ಯದಿ (flags == FRD_FINDNEXT)
    {
	repl_text = ರಿಕ್ತ;
	sfr = &find_widgets;
    }
    ಅನ್ಯಥಾ
    {
	repl_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(sfr->what));
    direction_down = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->down));

    ಯದಿ (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    ಯದಿ (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

/*
 * our usual callback function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
entry_activate_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gtk_widget_grab_focus(GTK_WIDGET(data));
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
    ಸ್ಥಿರ ಶೂನ್ಯ
entry_changed_cb(GtkWidget * entry, GtkWidget * dialog)
{
    ಧ್ರುವ gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

    ಯದಿ (!entry_text)
	ಪ್ರತ್ಯಾಗಮ;			// shouldn't happen

    nonempty = (entry_text[0] != '\0');

    ಯದಿ (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    ಯದಿ (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    ಶೂನ್ಯ
ex_helpfind(exarg_T *eap UNUSED)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#ಯದಿ defined(FEAT_BROWSE)
    ಸ್ಥಿರ ಶೂನ್ಯ
recent_func_log_func(ಧ್ರುವ gchar *log_domain UNUSED,
		     GLogLevelFlags log_level UNUSED,
		     ಧ್ರುವ gchar *message UNUSED,
		     gpointer user_data UNUSED)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#endif

    ಶೂನ್ಯ
gui_mch_set_fullscreen(ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಯದಿ (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    ಅನ್ಯಥಾ
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}
