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

#അന്തര്ഭാവ "vim.h"

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
# എങ്കിൽ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#endif

#ifdef FEAT_GUI_GTK
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# അല്ലെങ്കിൽ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# അല്ലെങ്കിൽ
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#അല്ലെങ്കിൽ
// define these items to be able to generate prototypes without GTK
തരനിർവചനം പൂർണ്ണസംഖ്യ GtkWidget;
# define gpointer പൂർണ്ണസംഖ്യ
# define guint8 പൂർണ്ണസംഖ്യ
# define GdkPixmap പൂർണ്ണസംഖ്യ
# define GdkBitmap പൂർണ്ണസംഖ്യ
# define GtkIconFactory പൂർണ്ണസംഖ്യ
# define GtkToolbar പൂർണ്ണസംഖ്യ
# define GtkAdjustment പൂർണ്ണസംഖ്യ
# define gboolean പൂർണ്ണസംഖ്യ
# define Gdk_Event_താക്കോൽ പൂർണ്ണസംഖ്യ
# define CancelData പൂർണ്ണസംഖ്യ
#endif

സ്ഥിതം ശൂന്യം entry_activate_cb(GtkWidget *widget, gpointer data);
സ്ഥിതം ശൂന്യം entry_changed_cb(GtkWidget *entry, GtkWidget *dialog);
സ്ഥിതം ശൂന്യം find_replace_cb(GtkWidget *widget, gpointer data);
#എങ്കിൽ defined(FEAT_BROWSE)
സ്ഥിതം ശൂന്യം recent_func_log_func(
	സ്ഥിരം gchar *log_domain,
	GLogLevelFlags log_level,
	സ്ഥിരം gchar *message,
	gpointer user_data);
#endif

#എങ്കിൽ defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം menu_themed_names[] =
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
#  എങ്കിൽ GTK_CHECK_VERSION(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  അല്ലെങ്കിൽ
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  എങ്കിൽ GTK_CHECK_VERSION(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  അല്ലെങ്കിൽ
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  എങ്കിൽ GTK_CHECK_VERSION(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  അല്ലെങ്കിൽ
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  എങ്കിൽ GTK_CHECK_VERSION(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  അല്ലെങ്കിൽ
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,10,0)
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം menu_stock_ids[] =
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
#  എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
തരനിർവചനം ഘടന IconNames {
    സ്ഥിരം അക്ഷരം *icon_name;
    സ്ഥിരം അക്ഷരം *file_name;
} IconNames;

സ്ഥിതം IconNames stock_vim_icons[] = {
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
    { ശൂന്യ, ശൂന്യ }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    സ്ഥിതം ശൂന്യം
add_stock_icon(GtkIconFactory	*factory,
	       സ്ഥിരം അക്ഷരം	*stock_id,
	       സ്ഥിരം guint8	*inline_data,
	       പൂർണ്ണസംഖ്യ		data_length)
{
    GdkPixbuf	*pixbuf;
    GtkIconSet	*icon_set;

    pixbuf = gdk_pixbuf_new_from_inline(data_length, inline_data, FALSE, ശൂന്യ);
    icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);

    gtk_icon_factory_add(factory, stock_id, icon_set);

    gtk_icon_set_unref(icon_set);
    g_object_unref(pixbuf);
}
# endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    എങ്കിൽ (mch_isFullName(dest))
    {
	തിരികെ vim_fexists(dest);
    }
    അല്ലെങ്കിൽ
    {
	സ്ഥിതം സ്ഥിരം അക്ഷരം   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	unsigned പൂർണ്ണസംഖ്യ	    i;

	വേണ്ടി (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    എങ്കിൽ (gui_find_bitmap(dest, buf, (അക്ഷരം *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		തിരികെ TRUE;
	    }

	തിരികെ FALSE;
    }
}

    സ്ഥിതം GtkWidget *
load_menu_iconfile(char_u *name, GtkIconSize icon_size)
{
    GtkWidget	    *image = ശൂന്യ;
# എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
    പൂർണ്ണസംഖ്യ		     pixel_size = -1;

    തിരഞ്ഞെടുപ്പ് (icon_size)
    {
	അവസ്ഥ GTK_ICON_SIZE_MENU:
	    pixel_size = 16;
	    നിർത്തുക;
	അവസ്ഥ GTK_ICON_SIZE_SMALL_TOOLBAR:
	    pixel_size = 16;
	    നിർത്തുക;
	അവസ്ഥ GTK_ICON_SIZE_LARGE_TOOLBAR:
	    pixel_size = 24;
	    നിർത്തുക;
	അവസ്ഥ GTK_ICON_SIZE_BUTTON:
	    pixel_size = 16;
	    നിർത്തുക;
	അവസ്ഥ GTK_ICON_SIZE_DND:
	    pixel_size = 32;
	    നിർത്തുക;
	അവസ്ഥ GTK_ICON_SIZE_DIALOG:
	    pixel_size = 48;
	    നിർത്തുക;
	അവസ്ഥ GTK_ICON_SIZE_INVALID:
	    // FALLTHROUGH
	സ്ഥിരസ്ഥിതി:
	    pixel_size = 0;
	    നിർത്തുക;
    }

    എങ്കിൽ (pixel_size > 0 || pixel_size == -1)
    {
	GdkPixbuf * സ്ഥിരം pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((സ്ഥിരം അക്ഷരം *)name,
		pixel_size, pixel_size, TRUE, ശൂന്യ);
	എങ്കിൽ (pixbuf != ശൂന്യ)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    എങ്കിൽ (image == ശൂന്യ)
	image = gtk_image_new_from_icon_name("image-missing", icon_size);

    തിരികെ image;
# അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,10,0)
    GtkIconSet	    *icon_set;
    GtkIconSource   *icon_source;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_set = gtk_icon_set_new();
    icon_source = gtk_icon_source_new();

    gtk_icon_source_set_filename(icon_source, (സ്ഥിരം അക്ഷരം *)name);
    gtk_icon_set_add_source(icon_set, icon_source);

    image = gtk_image_new_from_icon_set(icon_set, icon_size);

    gtk_icon_source_free(icon_source);
    gtk_icon_set_unref(icon_set);

    തിരികെ image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    സ്ഥിതം GtkWidget *
create_menu_icon(vimmenu_T *menu, GtkIconSize icon_size)
{
    GtkWidget	*image = ശൂന്യ;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    എങ്കിൽ (menu->iconfile != ശൂന്യ && lookup_menu_iconfile(menu->iconfile, buf))
	image = load_menu_iconfile(buf, icon_size);

    // If not found and not builtin specified try using the menu name.
    എങ്കിൽ (image == ശൂന്യ && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = load_menu_iconfile(buf, icon_size);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    എങ്കിൽ (image == ശൂന്യ)
    {
# എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
	സ്ഥിരം അക്ഷരം *icon_name = ശൂന്യ;
	സ്ഥിരം പൂർണ്ണസംഖ്യ   n_names = G_N_ELEMENTS(menu_themed_names);

	എങ്കിൽ (menu->iconidx >= 0 && menu->iconidx < n_names)
	    icon_name = menu_themed_names[menu->iconidx];
	എങ്കിൽ (icon_name == ശൂന്യ)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_size);
# അല്ലെങ്കിൽ
	സ്ഥിരം അക്ഷരം  *stock_id;
	സ്ഥിരം പൂർണ്ണസംഖ്യ   n_ids = G_N_ELEMENTS(menu_stock_ids);

	എങ്കിൽ (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	അല്ലെങ്കിൽ
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_size);
# endif
    }

    തിരികെ image;
}

    സ്ഥിതം gint
toolbar_button_focus_in_event(GtkWidget *widget UNUSED,
			      GdkEventFocus *event UNUSED,
			      gpointer data UNUSED)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    എങ്കിൽ (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    തിരികെ TRUE;
}
#endif // FEAT_TOOLBAR

#എങ്കിൽ defined(FEAT_TOOLBAR)

    ശൂന്യം
gui_gtk_register_stock_icons(ശൂന്യം)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define ADD_ICON(Name, Data) add_stock_icon(factory, Name, Data, (പൂർണ്ണസംഖ്യ)വലുപ്പം(Data))

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
# അല്ലെങ്കിൽ // defined(USE_GRESOURCE)
    സ്ഥിരം അക്ഷരം * സ്ഥിരം path_prefix = "/org/vim/gui/icon";
#  എങ്കിൽ GTK_CHECK_VERSION(3,14,0)
    GdkScreen    *screen = ശൂന്യ;
    GtkIconTheme *icon_theme = ശൂന്യ;

    എങ്കിൽ (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    അല്ലെങ്കിൽ
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_add_resource_path(icon_theme, path_prefix);
#  elif GTK_CHECK_VERSION(3,0,0)
    IconNames *names;

    വേണ്ടി (names = stock_vim_icons; names->icon_name != ശൂന്യ; names++)
    {
	അക്ഷരം path[MAXPATHL];

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	GdkPixbuf *pixbuf = ശൂന്യ;
	pixbuf = gdk_pixbuf_new_from_resource(path, ശൂന്യ);
	എങ്കിൽ (pixbuf != ശൂന്യ)
	{
	    സ്ഥിരം gint size = MAX(gdk_pixbuf_get_width(pixbuf),
				  gdk_pixbuf_get_height(pixbuf));
	    എങ്കിൽ (size > 16)
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
		എങ്കിൽ (pixbuf == ശൂന്യ)
		    pixbuf = src;
		അല്ലെങ്കിൽ
		    g_object_unref(src);
	    }
	    gtk_icon_theme_add_builtin_icon(names->icon_name, size, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * സ്ഥിരം factory = gtk_icon_factory_new();
    IconNames *names;

    വേണ്ടി (names = stock_vim_icons; names->icon_name != ശൂന്യ; names++)
    {
	അക്ഷരം path[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(path, ശൂന്യ);
	എങ്കിൽ (pixbuf != ശൂന്യ)
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

#എങ്കിൽ defined(FEAT_MENU)

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
    സ്ഥിതം char_u *
translate_mnemonic_tag(char_u *name, പൂർണ്ണസംഖ്യ use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    പൂർണ്ണസംഖ്യ	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	എങ്കിൽ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    എങ്കിൽ (buf != ശൂന്യ)
    {
	pdest = buf;
	വേണ്ടി (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    എങ്കിൽ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    തിരികെ buf;
}

    സ്ഥിതം ശൂന്യം
menu_item_new(vimmenu_T *menu, GtkWidget *parent_widget)
{
    GtkWidget	*box;
    char_u	*text;
    പൂർണ്ണസംഖ്യ		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_item_new();
# എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
# അല്ലെങ്കിൽ
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(parent_widget));
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((സ്ഥിരം അക്ഷരം *)text);
    vim_free(text);

    gtk_box_pack_start(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    എങ്കിൽ (menu->actext != ശൂന്യ && menu->actext[0] != NUL)
    {
	text = CONVERT_TO_UTF8(menu->actext);

	gtk_box_pack_end(GTK_BOX(box),
			 gtk_label_new((സ്ഥിരം അക്ഷരം *)text),
			 FALSE, FALSE, 0);

	CONVERT_TO_UTF8_FREE(text);
    }

    gtk_container_add(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    ശൂന്യം
gui_mch_add_menu(vimmenu_T *menu, പൂർണ്ണസംഖ്യ idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;

    എങ്കിൽ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	തിരികെ;
    }

    parent = menu->parent;

    എങ്കിൽ ((parent != ശൂന്യ && parent->submenu_id == ശൂന്യ)
	    || !menu_is_menubar(menu->name))
	തിരികെ;

    parent_widget = (parent != ശൂന്യ) ? parent->submenu_id : gui.menubar;
    menu_item_new(menu, parent_widget);

# എങ്കിൽ !GTK_CHECK_VERSION(3,4,0)
    // since the tearoff should always appear first, increment idx
    എങ്കിൽ (parent != ശൂന്യ && !menu_is_popup(parent->name))
	++idx;
# endif

    gtk_menu_shell_തിരുകുക(GTK_MENU_SHELL(parent_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_set_accel_group(GTK_MENU(menu->submenu_id), gui.accel_group);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->id), menu->submenu_id);

# എങ്കിൽ !GTK_CHECK_VERSION(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_item_new();
    എങ്കിൽ (vim_strchr(p_go, GO_TEAROFF) != ശൂന്യ)
	gtk_widget_show(menu->tearoff_handle);
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  അല്ലെങ്കിൽ
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    സ്ഥിതം ശൂന്യം
menu_item_activate(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    സ്ഥിതം ശൂന്യം
menu_item_select(GtkWidget *widget UNUSED, gpointer data)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	did_സന്ദേശം = FALSE;

    എങ്കിൽ (State & MODE_CMDLINE)
	തിരികെ;
    menu = (vimmenu_T *)data;
    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    എങ്കിൽ (tooltip != ശൂന്യ && utf_valid_string(tooltip, ശൂന്യ))
    {
	സന്ദേശം((അക്ഷരം *)tooltip);
	did_സന്ദേശം = TRUE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (did_സന്ദേശം)
    {
	സന്ദേശം("");
	did_സന്ദേശം = FALSE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    CONVERT_TO_UTF8_FREE(tooltip);
}

    ശൂന്യം
gui_mch_add_menu_item(vimmenu_T *menu, പൂർണ്ണസംഖ്യ idx)
{
    vimmenu_T *parent;

    parent = menu->parent;

# ifdef FEAT_TOOLBAR
    എങ്കിൽ (menu_is_toolbar(parent->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = ശൂന്യ;

	എങ്കിൽ (menu_is_separator(menu->name))
	{
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	    GtkToolItem *item = gtk_separator_tool_item_new();
	    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(item),
		    TRUE);
	    gtk_tool_item_set_expand(GTK_TOOL_ITEM(item), FALSE);
	    gtk_widget_show(GTK_WIDGET(item));

	    gtk_toolbar_തിരുകുക(toolbar, item, idx);
#  അല്ലെങ്കിൽ
	    gtk_toolbar_തിരുകുക_space(toolbar, idx);
#  endif
	    menu->id = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    char_u *text;
	    char_u *tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    എങ്കിൽ (tooltip != ശൂന്യ && !utf_valid_string(tooltip, ശൂന്യ))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_TO_UTF8_FREE(tooltip);

#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	    {
		GtkWidget *icon;
		GtkToolItem *item;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_size(toolbar));
		item = gtk_tool_button_new(icon, (സ്ഥിരം gchar *)text);
		gtk_tool_item_set_tooltip_text(item, (സ്ഥിരം gchar *)tooltip);
		g_signal_connect(G_OBJECT(item), "clicked",
			G_CALLBACK(&menu_item_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(item));

		gtk_toolbar_തിരുകുക(toolbar, item, idx);

		menu->id = GTK_WIDGET(item);
	    }
#  അല്ലെങ്കിൽ
	    menu->id = gtk_toolbar_തിരുകുക_item(
		    toolbar,
		    (സ്ഥിരം അക്ഷരം *)text,
		    (സ്ഥിരം അക്ഷരം *)tooltip,
		    ശൂന്യ,
		    create_menu_icon(menu, gtk_toolbar_get_icon_size(toolbar)),
		    G_CALLBACK(&menu_item_activate),
		    menu,
		    idx);
#  endif

	    എങ്കിൽ (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_in_event), ശൂന്യ);

	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
    }
    അല്ലെങ്കിൽ
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	എങ്കിൽ (parent == ശൂന്യ || parent->submenu_id == ശൂന്യ)
	    തിരികെ;

# എങ്കിൽ !GTK_CHECK_VERSION(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	എങ്കിൽ (!menu_is_popup(parent->name))
	    ++idx;
# endif

	എങ്കിൽ (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	    menu->id = gtk_separator_menu_item_new();
# അല്ലെങ്കിൽ
	    menu->id = gtk_menu_item_new();
	    gtk_widget_set_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_തിരുകുക(GTK_MENU_SHELL(parent->submenu_id),
		    menu->id, idx);

	    തിരികെ;
	}

	// Add textual menu item.
	menu_item_new(menu, parent->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_തിരുകുക(GTK_MENU_SHELL(parent->submenu_id),
		menu->id, idx);

	എങ്കിൽ (menu->id != ശൂന്യ)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_item_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_item_select), menu);
	}
    }
}
#endif // FEAT_MENU


    ശൂന്യം
gui_mch_set_text_area_pos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#എങ്കിൽ defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ശൂന്യം
gui_gtk_set_mnemonics(പൂർണ്ണസംഖ്യ enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	എങ്കിൽ (menu->id == ശൂന്യ)
	    തുടരുക;

	name = translate_mnemonic_tag(menu->name, enable);
	gtk_label_set_text_with_mnemonic(GTK_LABEL(menu->label),
					 (സ്ഥിരം അക്ഷരം *)name);
	vim_free(name);
    }
}

# എങ്കിൽ !GTK_CHECK_VERSION(3,4,0)
    സ്ഥിതം ശൂന്യം
recurse_tearoffs(vimmenu_T *menu, പൂർണ്ണസംഖ്യ val)
{
    വേണ്ടി (; menu != ശൂന്യ; menu = menu->next)
    {
	എങ്കിൽ (menu->submenu_id != ശൂന്യ && menu->tearoff_handle != ശൂന്യ
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    എങ്കിൽ (val)
		gtk_widget_show(menu->tearoff_handle);
	    അല്ലെങ്കിൽ
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
    ശൂന്യം
gui_mch_toggle_tearoffs(പൂർണ്ണസംഖ്യ enable UNUSED)
{
    // Do nothing
}
# അല്ലെങ്കിൽ
    ശൂന്യം
gui_mch_toggle_tearoffs(പൂർണ്ണസംഖ്യ enable)
{
    recurse_tearoffs(വേര്_menu, enable);
}
# endif
#endif // FEAT_MENU

#എങ്കിൽ defined(FEAT_TOOLBAR)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*കണ്ണി;
    പൂർണ്ണസംഖ്യ		idx = 0;

    വേണ്ടി (കണ്ണി = menu->parent->children; കണ്ണി != menu; കണ്ണി = കണ്ണി->next)
    {
	g_return_val_if_fail(കണ്ണി != ശൂന്യ, -1);
	++idx;
    }

    തിരികെ idx;
}
#endif // FEAT_TOOLBAR


#എങ്കിൽ defined(FEAT_TOOLBAR)
    ശൂന്യം
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    എങ്കിൽ (menu->id == ശൂന്യ || menu->parent == ശൂന്യ || gui.toolbar == ശൂന്യ)
	തിരികെ;

    char_u *tooltip;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    എങ്കിൽ (tooltip != ശൂന്യ && utf_valid_string(tooltip, ശൂന്യ))
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_set_tooltip_text(menu->id, (സ്ഥിരം gchar *)tooltip);
# അല്ലെങ്കിൽ
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_set_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (സ്ഥിരം അക്ഷരം *)tooltip, ശൂന്യ);
# endif
    CONVERT_TO_UTF8_FREE(tooltip);
}
#endif // FEAT_TOOLBAR


#എങ്കിൽ defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    ശൂന്യം
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    എങ്കിൽ (menu->id != ശൂന്യ)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    എങ്കിൽ (menu->id != ശൂന്യ && gui.menubar != ശൂന്യ
			    && gtk_widget_get_parent(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    എങ്കിൽ (menu->parent != ശൂന്യ && menu_is_toolbar(menu->parent->name))
    {
	എങ്കിൽ (menu_is_separator(menu->name))
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	{
	    GtkToolItem *item = ശൂന്യ;

	    item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    എങ്കിൽ (item != ശൂന്യ)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(item));
	}
#  അല്ലെങ്കിൽ
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	അല്ലെങ്കിൽ എങ്കിൽ (menu->id != ശൂന്യ)
	    gtk_widget_destroy(menu->id);
    }
    അല്ലെങ്കിൽ
# endif // FEAT_TOOLBAR
    {
	എങ്കിൽ (menu->submenu_id != ശൂന്യ)
	    gtk_widget_destroy(menu->submenu_id);

	എങ്കിൽ (menu->id != ശൂന്യ)
	    gtk_widget_destroy(menu->id);
    }

    എങ്കിൽ (menu->id != ശൂന്യ)
	g_object_unref(menu->id);
    menu->submenu_id = ശൂന്യ;
    menu->id = ശൂന്യ;
}
#endif // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    ശൂന്യം
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    എങ്കിൽ (sb->id == ശൂന്യ)
	തിരികെ;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_set_lower(adjustment, 0.0);
    gtk_adjustment_set_മൂല്യം(adjustment, val);
    gtk_adjustment_set_upper(adjustment, max + 1);
    gtk_adjustment_set_page_size(adjustment, size);
    gtk_adjustment_set_page_increment(adjustment,
	    size < 3L ? 1L : size - 2L);
    gtk_adjustment_set_step_increment(adjustment, 1.0);

    g_signal_handler_block(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#എങ്കിൽ !GTK_CHECK_VERSION(3,18,0)
    gtk_adjustment_changed(adjustment);
#endif

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    ശൂന്യം
gui_mch_set_scrollbar_pos(scrollbar_T *sb, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    എങ്കിൽ (sb->id != ശൂന്യ)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_xpadding(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ xpad;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    xpad = gtk_widget_get_allocated_width(gui.formwin)
	  - gtk_widget_get_allocated_width(gui.drawarea) - gui.scrollbar_width;
#അല്ലെങ്കിൽ
    xpad = gui.formwin->allocation.width - gui.drawarea->allocation.width
							 - gui.scrollbar_width;
#endif
    എങ്കിൽ (gui.which_scrollbars[SBAR_ഇടത്] && gui.which_scrollbars[SBAR_വലത്])
	xpad -= gui.scrollbar_width;

    തിരികെ (xpad < 0) ? 0 : xpad;
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_ypadding(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ ypad;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    ypad = gtk_widget_get_allocated_height(gui.formwin)
	- gtk_widget_get_allocated_height(gui.drawarea) - gui.scrollbar_height;
#അല്ലെങ്കിൽ
    ypad = gui.formwin->allocation.height - gui.drawarea->allocation.height
							- gui.scrollbar_height;
#endif
    തിരികെ (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    സ്ഥിതം ശൂന്യം
adjustment_മൂല്യം_changed(GtkAdjustment *adjustment, gpointer data)
{
    scrollbar_T	*sb;
    long	മൂല്യം;
    പൂർണ്ണസംഖ്യ		dragging = FALSE;

#ifdef FEAT_XIM
    // cancel any preediting
    എങ്കിൽ (im_is_preediting())
	xim_reset();
#endif

    sb = gui_find_scrollbar((long)data);
    മൂല്യം = gtk_adjustment_get_മൂല്യം(adjustment);
#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    എങ്കിൽ (sb != ശൂന്യ)
    {
	dragging = TRUE;

	എങ്കിൽ (sb->wp != ശൂന്യ && GDK_IS_DRAWABLE(sb->id->window))
	{
	    പൂർണ്ണസംഖ്യ			x;
	    പൂർണ്ണസംഖ്യ			y;
	    GdkModifierType	state;
	    പൂർണ്ണസംഖ്യ			width;
	    പൂർണ്ണസംഖ്യ			height;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_window_get_pointer(sb->id->window, &x, &y, &state);
	    gdk_window_get_size(sb->id->window, &width, &height);
	    എങ്കിൽ (x >= 0 && x < width && y >= 0 && y < height)
	    {
		എങ്കിൽ (y < width)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    മൂല്യം = sb->wp->w_topline - 2;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (y > height - width)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    മൂല്യം = sb->wp->w_topline;
		}
	    }
	}
    }
#endif // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, മൂല്യം, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    ശൂന്യം
gui_mch_create_scrollbar(scrollbar_T *sb, പൂർണ്ണസംഖ്യ orient)
{
    എങ്കിൽ (orient == SBAR_HORIZ)
#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ശൂന്യ);
#അല്ലെങ്കിൽ
	sb->id = gtk_hscrollbar_new(ശൂന്യ);
#endif
    അല്ലെങ്കിൽ എങ്കിൽ (orient == SBAR_VERT)
#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ശൂന്യ);
#അല്ലെങ്കിൽ
	sb->id = gtk_vscrollbar_new(ശൂന്യ);
#endif

    എങ്കിൽ (sb->id == ശൂന്യ)
	തിരികെ;

    GtkAdjustment *adjustment;

    gtk_widget_set_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_മൂല്യം_changed),
	    GINT_TO_POINTER(sb->ident));
    gui_mch_update();
}

    ശൂന്യം
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    എങ്കിൽ (sb->id != ശൂന്യ)
    {
	gtk_widget_destroy(sb->id);
	sb->id = ശൂന്യ;
    }
    gui_mch_update();
}

#എങ്കിൽ defined(FEAT_BROWSE)
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
gui_mch_browse(പൂർണ്ണസംഖ്യ saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext UNUSED,
	       char_u *initdir,
	       char_u *filter)
{
# എങ്കിൽ GTK_CHECK_VERSION(3,20,0)
    GtkFileChooserNative	*fc;
# അല്ലെങ്കിൽ
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    സ്ഥിരം gchar		*domain = "Gtk";

    title = CONVERT_TO_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    എങ്കിൽ (initdir == ശൂന്യ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    അല്ലെങ്കിൽ എങ്കിൽ (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    add_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_set_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, ശൂന്യ);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# എങ്കിൽ GTK_CHECK_VERSION(3,20,0)
    fc = gtk_file_chooser_native_new(
# അല്ലെങ്കിൽ
    fc = gtk_file_chooser_dialog_new(
# endif
	    (സ്ഥിരം gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# എങ്കിൽ GTK_CHECK_VERSION(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# അല്ലെങ്കിൽ
#  എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  അല്ലെങ്കിൽ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    ശൂന്യ);
# endif
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fc),
						       (സ്ഥിരം gchar *)dirbuf);

    എങ്കിൽ (filter != ശൂന്യ && *filter != NUL)
    {
	പൂർണ്ണസംഖ്യ     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	എങ്കിൽ (patt != ശൂന്യ)
	{
	    ആകുമ്പോൾ (p != ശൂന്യ && *p != NUL)
	    {
		എങ്കിൽ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    എങ്കിൽ (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    അല്ലെങ്കിൽ
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			എങ്കിൽ (*p == '\n')
			{
			    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    എങ്കിൽ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		അല്ലെങ്കിൽ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    എങ്കിൽ (saving && dflt != ശൂന്യ && *dflt != NUL)
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fc), (അക്ഷരം *)dflt);

    gui.browse_fname = ശൂന്യ;
# എങ്കിൽ GTK_CHECK_VERSION(3,20,0)
    എങ്കിൽ (gtk_native_dialog_run(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# അല്ലെങ്കിൽ
    എങ്കിൽ (gtk_dialog_run(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	അക്ഷരം *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# എങ്കിൽ GTK_CHECK_VERSION(3,20,0)
    g_object_unref(fc);
# അല്ലെങ്കിൽ
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_TO_UTF8_FREE(title);
    എങ്കിൽ (gui.browse_fname == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // shorten the file name if possible
    തിരികെ vim_strsave(shorten_fname1(gui.browse_fname));
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
# എങ്കിൽ defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = ശൂന്യ;

    title = CONVERT_TO_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (സ്ഥിരം gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  അല്ലെങ്കിൽ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    ശൂന്യ);

    CONVERT_TO_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    എങ്കിൽ (initdir == ശൂന്യ || *initdir == NUL
	       || vim_FullName(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    add_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dirdlg),
						      (സ്ഥിരം gchar *)dirbuf);

    // Run the dialog.
    എങ്കിൽ (gtk_dialog_run(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    എങ്കിൽ (dirname == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    തിരികെ p;

# അല്ലെങ്കിൽ // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    തിരികെ gui_mch_browse(0, title, ശൂന്യ, ശൂന്യ, initdir, ശൂന്യ);
# endif // !defined(GTK_FILE_CHOOSER)
}


#endif	// FEAT_BROWSE

#എങ്കിൽ defined(FEAT_GUI_DIALOG)

    സ്ഥിതം GtkWidget *
create_message_dialog(പൂർണ്ണസംഖ്യ type, char_u *title, char_u *message)
{
    GtkWidget	    *dialog;
    GtkMessageType  message_type;

    തിരഞ്ഞെടുപ്പ് (type)
    {
	അവസ്ഥ VIM_ERROR:	    message_type = GTK_MESSAGE_ERROR;	 നിർത്തുക;
	അവസ്ഥ VIM_WARNING:   message_type = GTK_MESSAGE_WARNING;	 നിർത്തുക;
	അവസ്ഥ VIM_QUESTION:  message_type = GTK_MESSAGE_QUESTION; നിർത്തുക;
	സ്ഥിരസ്ഥിതി:	    message_type = GTK_MESSAGE_INFO;	 നിർത്തുക;
    }

    message = CONVERT_TO_UTF8(message);
    dialog  = gtk_message_dialog_new(GTK_WINDOW(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_PARENT,
				     message_type,
				     GTK_BUTTONS_NONE,
				     "%s", (സ്ഥിരം അക്ഷരം *)message);
    CONVERT_TO_UTF8_FREE(message);

    എങ്കിൽ (title != ശൂന്യ)
    {
	title = CONVERT_TO_UTF8(title);
	gtk_window_set_title(GTK_WINDOW(dialog), (സ്ഥിരം അക്ഷരം *)title);
	CONVERT_TO_UTF8_FREE(title);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (type == VIM_GENERIC)
    {
	gtk_window_set_title(GTK_WINDOW(dialog), "VIM");
    }

    തിരികെ dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    സ്ഥിതം അക്ഷരം **
split_button_string(char_u *button_string, പൂർണ്ണസംഖ്യ *n_buttons)
{
    അക്ഷരം	    **array;
    char_u	    *p;
    unsigned പൂർണ്ണസംഖ്യ    എണ്ണം = 1;

    വേണ്ടി (p = button_string; *p != NUL; ++p)
	എങ്കിൽ (*p == DLG_BUTTON_SEP)
	    ++എണ്ണം;

    array = ALLOC_MULT(അക്ഷരം *, എണ്ണം + 1);
    എണ്ണം = 0;

    എങ്കിൽ (array != ശൂന്യ)
    {
	array[എണ്ണം++] = (അക്ഷരം *)button_string;
	വേണ്ടി (p = button_string; *p != NUL; )
	{
	    എങ്കിൽ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[എണ്ണം++] = (അക്ഷരം *)p;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    അല്ലെങ്കിൽ
		MB_PTR_ADV(p);
	}
	array[എണ്ണം] = ശൂന്യ; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = എണ്ണം;
    തിരികെ array;
}

# എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
    സ്ഥിതം അക്ഷരം **
split_button_translation(സ്ഥിരം അക്ഷരം *message)
{
    അക്ഷരം    **buttons = ശൂന്യ;
    char_u  *str;
    പൂർണ്ണസംഖ്യ	    n_buttons = 0;
    പൂർണ്ണസംഖ്യ	    n_expected = 1;

    വേണ്ടി (str = (char_u *)message; *str != NUL; ++str)
	എങ്കിൽ (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(message);
    എങ്കിൽ (str != ശൂന്യ)
    {
	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ശൂന്യ);
	അല്ലെങ്കിൽ
	    str = vim_strsave(str);

	എങ്കിൽ (str != ശൂന്യ)
	    buttons = split_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    എങ്കിൽ (buttons == ശൂന്യ || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = ശൂന്യ;
	str = vim_strsave((char_u *)message);

	എങ്കിൽ (str != ശൂന്യ)
	    buttons = split_button_string(str, &n_buttons);
	എങ്കിൽ (buttons == ശൂന്യ)
	    vim_free(str);
    }

    തിരികെ buttons;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
button_equal(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b)
{
    ആകുമ്പോൾ (*a != '\0' && *b != '\0')
    {
	എങ്കിൽ (*a == '_' && *++a == '\0')
	    നിർത്തുക;
	എങ്കിൽ (*b == '_' && *++b == '\0')
	    നിർത്തുക;

	എങ്കിൽ (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    തിരികെ FALSE;

	a = g_utf8_next_char(a);
	b = g_utf8_next_char(b);
    }

    തിരികെ (*a == '\0' && *b == '\0');
}
# endif

    സ്ഥിതം ശൂന്യം
dialog_add_buttons(GtkDialog *dialog, char_u *button_string)
{
# എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
    അക്ഷരം    **ok;
    അക്ഷരം    **ync;  // "yes no cancel"
# endif
    അക്ഷരം    **buttons;
    പൂർണ്ണസംഖ്യ	    n_buttons = 0;
    പൂർണ്ണസംഖ്യ	    idx;

    button_string = vim_strsave(button_string); // must be writable
    എങ്കിൽ (button_string == ശൂന്യ)
	തിരികെ;

    // Check 'v' flag in 'guioptions': vertical button placement.
    എങ്കിൽ (vim_strchr(p_go, GO_VERTICAL) != ശൂന്യ)
    {
# എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
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
# എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
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
    വേണ്ടി (idx = 1; idx <= n_buttons; ++idx)
    {
	അക്ഷരം	*label;
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
# എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
	എങ്കിൽ (ok != ശൂന്യ && ync != ശൂന്യ) // almost impossible to fail
	{
	    എങ്കിൽ	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    അല്ലെങ്കിൽ എങ്കിൽ (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    അല്ലെങ്കിൽ എങ്കിൽ (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    അല്ലെങ്കിൽ എങ്കിൽ (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    അല്ലെങ്കിൽ എങ്കിൽ (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    അല്ലെങ്കിൽ എങ്കിൽ (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    അല്ലെങ്കിൽ എങ്കിൽ (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    അല്ലെങ്കിൽ എങ്കിൽ (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_TO_UTF8((char_u *)label);
	gtk_dialog_add_button(dialog, (സ്ഥിരം gchar *)label8, idx);
	CONVERT_TO_UTF8_FREE(label8);
    }

# എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
    എങ്കിൽ (ok != ശൂന്യ)
	vim_free(*ok);
    എങ്കിൽ (ync != ശൂന്യ)
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
തരനിർവചനം ഘടന _DialogInfo
{
    പൂർണ്ണസംഖ്യ		ignore_enter;	    // no default button, ignore "Enter"
    പൂർണ്ണസംഖ്യ		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    സ്ഥിതം gboolean
dialog_താക്കോൽ_press_event_cb(GtkWidget *widget, Gdk_Event_താക്കോൽ *event, gpointer data)
{
    DialogInfo *di = (DialogInfo *)data;

    // Ignore hitting Enter (or Space) when there is no default button.
    എങ്കിൽ (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	തിരികെ TRUE;
    അല്ലെങ്കിൽ    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    എങ്കിൽ (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	തിരികെ TRUE;
    }

    എങ്കിൽ (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_mask()) == 0)
    {
	തിരികെ gtk_window_mnemonic_activate(
		   GTK_WINDOW(widget), event->keyval,
		   gtk_window_get_mnemonic_modifier(GTK_WINDOW(widget)));
    }

    തിരികെ FALSE; // continue emission
}

    പൂർണ്ണസംഖ്യ
gui_mch_dialog(പൂർണ്ണസംഖ്യ	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*message,   // message text
	       char_u	*buttons,   // names of buttons
	       പൂർണ്ണസംഖ്യ	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       പൂർണ്ണസംഖ്യ	ex_cmd UNUSED)
{
    GtkWidget	*dialog;
    GtkWidget	*entry = ശൂന്യ;
    char_u	*text;
    പൂർണ്ണസംഖ്യ		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_active;

    dialog = create_message_dialog(type, title, message);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_add_buttons(GTK_DIALOG(dialog), buttons);

    എങ്കിൽ (textfield != ശൂന്യ)
    {
	GtkWidget *alignment;

	entry = gtk_entry_new();
	gtk_widget_show(entry);

	// Make Enter work like pressing OK.
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

	text = CONVERT_TO_UTF8(textfield);
	gtk_entry_set_text(GTK_ENTRY(entry), (സ്ഥിരം അക്ഷരം *)text);
	CONVERT_TO_UTF8_FREE(text);

# എങ്കിൽ GTK_CHECK_VERSION(3,14,0)
	gtk_widget_set_halign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_valign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(entry), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# അല്ലെങ്കിൽ
	alignment = gtk_alignment_new((ദശാംശം)0.5, (ദശാംശം)0.5,
						      (ദശാംശം)1.0, (ദശാംശം)1.0);
# endif
	gtk_container_add(GTK_CONTAINER(alignment), entry);
	gtk_container_set_border_width(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	{
	    GtkWidget * സ്ഥിരം vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_start(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# അല്ലെങ്കിൽ
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    അല്ലെങ്കിൽ
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_താക്കോൽ_press_event_cb), &dialoginfo);

    എങ്കിൽ (def_but > 0)
    {
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    അല്ലെങ്കിൽ
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    എങ്കിൽ (response != GTK_RESPONSE_NONE)
    {
	എങ്കിൽ (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	എങ്കിൽ (textfield != ശൂന്യ)
	{
	    text = (char_u *)gtk_entry_get_text(GTK_ENTRY(entry));
	    text = CONVERT_FROM_UTF8(text);

	    vim_strncpy(textfield, text, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(text);
	}
	gtk_widget_destroy(dialog);
    }

    എങ്കിൽ (gui.dialogs_active > 0)
	--gui.dialogs_active;
    തിരികെ response > 0 ? response : 0;
}

#endif // FEAT_GUI_DIALOG


#എങ്കിൽ defined(FEAT_MENU)

    ശൂന്യം
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# എങ്കിൽ defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
    എങ്കിൽ (xic != ശൂന്യ && g_object_get_data(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == ശൂന്യ)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_TO_UTF8(name);
	menuitem = gtk_menu_item_new_with_mnemonic((സ്ഥിരം അക്ഷരം *)name);
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

# എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
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
	trigger.axes       = ശൂന്യ;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = ശൂന്യ;
	trigger.x_വേര്     = 0.0;
	trigger.y_വേര്     = 0.0;

	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# അല്ലെങ്കിൽ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   ശൂന്യ, ശൂന്യ,
		   (GtkMenuPositionFunc)ശൂന്യ, ശൂന്യ,
		   3U, gui.event_time);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
സ്ഥിതം പൂർണ്ണസംഖ്യ popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    സ്ഥിതം ശൂന്യം
popup_menu_position_func(GtkMenu *menu UNUSED,
			 gint *x, gint *y,
			 gboolean *തള്ളുക_in UNUSED,
			 gpointer user_data UNUSED)
{
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), x, y);

    എങ്കിൽ (popup_mouse_pos)
    {
	പൂർണ്ണസംഖ്യ	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin != ശൂന്യ && gui.drawarea != ശൂന്യ &&
	     gtk_widget_get_window(gui.drawarea) != ശൂന്യ)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    ശൂന്യം
gui_make_popup(char_u *path_name, പൂർണ്ണസംഖ്യ mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_find_menu(path_name);
    എങ്കിൽ (menu == ശൂന്യ || menu->submenu_id == ശൂന്യ)
	തിരികെ;

# എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
    GdkWindow * സ്ഥിരം win = gtk_widget_get_window(gui.drawarea);
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
    trigger.axes       = ശൂന്യ;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = ശൂന്യ;
    trigger.x_വേര്     = 0.0;
    trigger.y_വേര്     = 0.0;

    എങ്കിൽ (mouse_pos)
	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    അല്ലെങ്കിൽ
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_window_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(ശൂന്യ, &rect.x, &rect.y, ശൂന്യ, ശൂന്യ);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# അല്ലെങ്കിൽ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    ശൂന്യ, ശൂന്യ,
	    &popup_menu_position_func, ശൂന്യ,
	    0U, (guint32)GDK_CURRENT_TIME);
# endif
}

#endif // FEAT_MENU


/*
 * We don't create it twice.
 */

തരനിർവചനം ഘടന _SharedFindReplace
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

സ്ഥിതം SharedFindReplace find_widgets = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};
സ്ഥിതം SharedFindReplace repl_widgets = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};

    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_താക്കോൽ_press_event(
		GtkWidget	*widget UNUSED,
		Gdk_Event_താക്കോൽ	*event,
		SharedFindReplace *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    എങ്കിൽ (event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	തിരികെ FALSE;

    // the Escape key synthesizes a cancellation action
    എങ്കിൽ (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	തിരികെ TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    തിരികെ FALSE;
}

    സ്ഥിതം GtkWidget *
#എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
create_image_button(സ്ഥിരം അക്ഷരം *stock_id UNUSED,
		    സ്ഥിരം അക്ഷരം *label)
#അല്ലെങ്കിൽ
create_image_button(സ്ഥിരം അക്ഷരം *stock_id,
		    സ്ഥിരം അക്ഷരം *label)
#endif
{
    char_u	*text;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    text = CONVERT_TO_UTF8((char_u *)label);

#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
#അല്ലെങ്കിൽ
    box = gtk_hbox_new(FALSE, 3);
#endif
#എങ്കിൽ !GTK_CHECK_VERSION(3,10,0)
    എങ്കിൽ (stock_id != ശൂന്യ)
	gtk_box_pack_start(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_BUTTON),
			   FALSE, FALSE, 0);
#endif
    gtk_box_pack_start(GTK_BOX(box),
		       gtk_label_new((സ്ഥിരം അക്ഷരം *)text),
		       FALSE, FALSE, 0);

    CONVERT_TO_UTF8_FREE(text);

#എങ്കിൽ GTK_CHECK_VERSION(3,14,0)
    gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#അല്ലെങ്കിൽ
    alignment = gtk_alignment_new((ദശാംശം)0.5, (ദശാംശം)0.5,
						      (ദശാംശം)0.0, (ദശാംശം)0.0);
#endif
    gtk_container_add(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), alignment);

    തിരികെ button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    സ്ഥിതം സ്ഥിരം അക്ഷരം *
convert_localized_message(char_u **buffer, സ്ഥിരം അക്ഷരം *message)
{
    എങ്കിൽ (output_conv.vc_type == CONV_NONE)
	തിരികെ message;

    vim_free(*buffer);
    *buffer = string_convert(&output_conv, (char_u *)message, ശൂന്യ);

    തിരികെ (സ്ഥിരം അക്ഷരം *)*buffer;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    സ്ഥിതം unsigned long
entry_get_text_length(GtkEntry *entry)
{
    g_return_val_if_fail(entry != ശൂന്യ, 0);
    g_return_val_if_fail(GTK_IS_ENTRY(entry) == TRUE, 0);

#എങ്കിൽ GTK_CHECK_VERSION(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    തിരികെ gtk_entry_buffer_get_length(gtk_entry_get_buffer(entry));
#അല്ലെങ്കിലെങ്കിൽ GTK_CHECK_VERSION(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    തിരികെ gtk_entry_get_text_length(entry);
#അല്ലെങ്കിൽ
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    തിരികെ g_utf8_strlen(gtk_entry_get_text(entry), -1);
#endif
}

    സ്ഥിതം ശൂന്യം
find_replace_dialog_create(char_u *arg, പൂർണ്ണസംഖ്യ do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*table;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    SharedFindReplace *frdp;
    char_u	*entry_text;
    പൂർണ്ണസംഖ്യ		wword = FALSE;
    പൂർണ്ണസംഖ്യ		mcase = !p_ic;
    char_u	*conv_buffer = ശൂന്യ;
#define CONV(message) convert_localized_message(&conv_buffer, (message))

    frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    എങ്കിൽ (entry_text != ശൂന്യ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_text = entry_text;
	entry_text = string_convert(&output_conv, entry_text, ശൂന്യ);
	vim_free(old_text);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    എങ്കിൽ (frdp->dialog)
    {
	എങ്കിൽ (entry_text != ശൂന്യ)
	{
	    gtk_entry_set_text(GTK_ENTRY(frdp->what), (അക്ഷരം *)entry_text);
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
	എങ്കിൽ (do_replace && entry_get_text_length(GTK_ENTRY(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	തിരികെ;
    }

    frdp->dialog = gtk_dialog_new();
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#അല്ലെങ്കിൽ
    gtk_dialog_set_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#endif
    gtk_window_set_transient_for(GTK_WINDOW(frdp->dialog), GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(frdp->dialog), TRUE);

    എങ്കിൽ (do_replace)
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    അല്ലെങ്കിൽ
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
#അല്ലെങ്കിൽ
    hbox = gtk_hbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    {
	GtkWidget * സ്ഥിരം dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    }
#അല്ലെങ്കിൽ
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#endif

    എങ്കിൽ (do_replace)
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#അല്ലെങ്കിൽ
	table = gtk_table_new(1024, 4, FALSE);
#endif
    അല്ലെങ്കിൽ
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#അല്ലെങ്കിൽ
	table = gtk_table_new(1024, 3, FALSE);
#endif
    gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(table), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#എങ്കിൽ GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#അല്ലെങ്കിലെങ്കിൽ GTK_CHECK_VERSION(3,14,0)
    {
	GValue align_val = G_മൂല്യം_INIT;

	g_മൂല്യം_init(&align_val, G_TYPE_FLOAT);

	g_മൂല്യം_set_float(&align_val, 0.0);
	g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	g_മൂല്യം_set_float(&align_val, 0.5);
	g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	g_മൂല്യം_unset(&align_val);
    }
#അല്ലെങ്കിൽ
    gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), tmp, 0, 0, 2, 1);
#അല്ലെങ്കിൽ
    gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    frdp->what = gtk_entry_new();
    sensitive = (entry_text != ശൂന്യ && entry_text[0] != NUL);
    എങ്കിൽ (entry_text != ശൂന്യ)
	gtk_entry_set_text(GTK_ENTRY(frdp->what), (അക്ഷരം *)entry_text);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(entry_changed_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(find_താക്കോൽ_press_event),
			   (gpointer) frdp);
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), frdp->what, 2, 0, 5, 1);
#അല്ലെങ്കിൽ
    gtk_table_attach(GTK_TABLE(table), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    എങ്കിൽ (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#എങ്കിൽ GTK_CHECK_VERSION(3,16,0)
	gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#അല്ലെങ്കിലെങ്കിൽ GTK_CHECK_VERSION(3,14,0)
	{
	    GValue align_val = G_മൂല്യം_INIT;

	    g_മൂല്യം_init(&align_val, G_TYPE_FLOAT);

	    g_മൂല്യം_set_float(&align_val, 0.0);
	    g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_മൂല്യം_set_float(&align_val, 0.5);
	    g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_മൂല്യം_unset(&align_val);
	}
#അല്ലെങ്കിൽ
	gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 0, 1, 2, 1);
#അല്ലെങ്കിൽ
	gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
	frdp->with = gtk_entry_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(find_താക്കോൽ_press_event),
			       (gpointer) frdp);
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->with, 2, 1, 5, 1);
#അല്ലെങ്കിൽ
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
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (do_replace)
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 2, 5, 1);
#അല്ലെങ്കിൽ
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    അല്ലെങ്കിൽ
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 3, 5, 1);
#അല്ലെങ്കിൽ
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    എങ്കിൽ (do_replace)
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 3, 5, 1);
#അല്ലെങ്കിൽ
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    അല്ലെങ്കിൽ
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 4, 5, 1);
#അല്ലെങ്കിൽ
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    tmp = gtk_frame_new(CONV(_("Direction")));
    എങ്കിൽ (do_replace)
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 2, 4);
#അല്ലെങ്കിൽ
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
    അല്ലെങ്കിൽ
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 1, 3);
#അല്ലെങ്കിൽ
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
#അല്ലെങ്കിൽ
    vbox = gtk_vbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
    gtk_container_add(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->up = gtk_radio_button_new_with_label(ശൂന്യ, CONV(_("Up")));
    gtk_box_pack_start(GTK_BOX(vbox), frdp->up, TRUE, TRUE, 0);
    frdp->down = gtk_radio_button_new_with_label(
			gtk_radio_button_get_group(GTK_RADIO_BUTTON(frdp->up)),
			CONV(_("Down")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->down), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_start(GTK_BOX(vbox), frdp->down, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#അല്ലെങ്കിൽ
    actionarea = gtk_vbutton_box_new();
#endif
    gtk_container_set_border_width(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_end(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
    frdp->find = create_image_button(ശൂന്യ, _("Find Next"));
#അല്ലെങ്കിൽ
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

    എങ്കിൽ (do_replace)
    {
	// 'Replace' button
#എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
	frdp->replace = create_image_button(ശൂന്യ, _("Replace"));
#അല്ലെങ്കിൽ
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#endif
	gtk_widget_set_sensitive(frdp->replace, sensitive);
	gtk_widget_set_can_default(frdp->find, TRUE);
	gtk_box_pack_start(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_REPLACE));

	// 'Replace All' button
#എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
	frdp->all = create_image_button(ശൂന്യ, _("Replace All"));
#അല്ലെങ്കിൽ
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
#എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#അല്ലെങ്കിൽ
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

#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#അല്ലെങ്കിൽ
    tmp = gtk_vseparator_new();
#endif
    gtk_box_pack_end(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
# എങ്കിൽ !GTK_CHECK_VERSION(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#അല്ലെങ്കിൽ
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#endif
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(entry_text);
    vim_free(conv_buffer);
#undef CONV
}

    ശൂന്യം
gui_mch_find_dialog(exarg_T *eap)
{
    എങ്കിൽ (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    ശൂന്യം
gui_mch_replace_dialog(exarg_T *eap)
{
    എങ്കിൽ (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    സ്ഥിതം ശൂന്യം
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    പൂർണ്ണസംഖ്യ			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = (പൂർണ്ണസംഖ്യ)(long)data;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    എങ്കിൽ (flags == FRD_FINDNEXT)
    {
	repl_text = ശൂന്യ;
	sfr = &find_widgets;
    }
    അല്ലെങ്കിൽ
    {
	repl_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(sfr->what));
    direction_down = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->down));

    എങ്കിൽ (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    എങ്കിൽ (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->mcase)))
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
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
entry_changed_cb(GtkWidget * entry, GtkWidget * dialog)
{
    സ്ഥിരം gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

    എങ്കിൽ (!entry_text)
	തിരികെ;			// shouldn't happen

    nonempty = (entry_text[0] != '\0');

    എങ്കിൽ (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    എങ്കിൽ (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    ശൂന്യം
ex_helpfind(exarg_T *eap UNUSED)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#എങ്കിൽ defined(FEAT_BROWSE)
    സ്ഥിതം ശൂന്യം
recent_func_log_func(സ്ഥിരം gchar *log_domain UNUSED,
		     GLogLevelFlags log_level UNUSED,
		     സ്ഥിരം gchar *message UNUSED,
		     gpointer user_data UNUSED)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#endif

    ശൂന്യം
gui_mch_set_fullscreen(പൂർണ്ണസംഖ്യ flag)
{
    എങ്കിൽ (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    അല്ലെങ്കിൽ
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}
