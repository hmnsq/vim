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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
# include "gui_gtk_f.h"
#ਜੇ_ਅੰਤ

// GTK defines MAX and MIN, but some system header files as well.  Undefine
// them and don't use them.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MIN
# undef MIN
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MAX
# undef MAX
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
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
# ਜੇ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ਨਹੀਂਤਾਂ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# ਨਹੀਂਤਾਂ
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#ਨਹੀਂਤਾਂ
// define these items to be able to generate prototypes without GTK
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ GtkWidget;
# define gpointer ਪੂਰਨ
# define guint8 ਪੂਰਨ
# define GdkPixmap ਪੂਰਨ
# define GdkBitmap ਪੂਰਨ
# define GtkIconFactory ਪੂਰਨ
# define GtkToolbar ਪੂਰਨ
# define GtkAdjustment ਪੂਰਨ
# define gboolean ਪੂਰਨ
# define Gdk_Event_ਕੁੰਜੀ ਪੂਰਨ
# define Cancel_ਅੰਕੜੇ ਪੂਰਨ
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ ਇੰਦਰਾਜ_activate_cb(GtkWidget *widget, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ ਇੰਦਰਾਜ_ਬਦਲਿਆ_cb(GtkWidget *ਇੰਦਰਾਜ, GtkWidget *dialog);
ਸਥਿਰ ਖਾਲੀ ਲੱਭੋ_replace_cb(GtkWidget *widget, gpointer ਅੰਕੜੇ);
#ਜੇ defined(FEAT_BROWSE)
ਸਥਿਰ ਖਾਲੀ recent_func_log_func(
	ਅਚਲ gchar *log_domain,
	GLogLevelFlags log_level,
	ਅਚਲ gchar *ਸੁਨੇਹਾ_2,
	gpointer user_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ menu_themed_ਨਾਂ[] =
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
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  ਨਹੀਂਤਾਂ
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  ਨਹੀਂਤਾਂ
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  ਨਹੀਂਤਾਂ
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  ਨਹੀਂਤਾਂ
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,10,0)
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ menu_stock_ids[] =
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
    /* 10 */ GTK_STOCK_ਲੱਭੋ,
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
    /* 15 */ GTK_STOCK_EXECUTE,
    /* 16 */ GTK_STOCK_ਲੱਭੋ_AND_REPLACE,
    /* 17 */ GTK_STOCK_CLOSE,		// FIXME: fuzzy
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
    /* 22 */ GTK_STOCK_GO_ਪਿੱਠ,
    /* 23 */ GTK_STOCK_GO_FORWARD,
    /* 24 */ "vim-find-help",
    /* 25 */ GTK_STOCK_CONVERT,
    /* 26 */ GTK_STOCK_JUMP_ਮੰਜ਼ਿਲ,
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
    /* 31 */ GTK_STOCK_QUIT
};
# endif // !GTK_CHECK_VERSION(3,10,0)

# ifdef USE_GRESOURCE
#  ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Icon_ਨਾਂ {
    ਅਚਲ ਅੱਖਰ *icon_name;
    ਅਚਲ ਅੱਖਰ *file_name;
} Icon_ਨਾਂ;

ਸਥਿਰ Icon_ਨਾਂ stock_vim_icons[] = {
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
    { ਨਿਰਾ, ਨਿਰਾ }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_stock_icon(GtkIconFactory	*factory,
	       ਅਚਲ ਅੱਖਰ	*stock_id,
	       ਅਚਲ guint8	*inline_ਅੰਕੜੇ,
	       ਪੂਰਨ		ਅੰਕੜੇ_ਲੰਮਾਈ)
{
    GdkPixbuf	*pixbuf;
    Gtk_Icon_ਸੈੱਟ	*icon_ਸੈੱਟ;

    pixbuf = gdk_pixbuf_new_from_inline(ਅੰਕੜੇ_ਲੰਮਾਈ, inline_ਅੰਕੜੇ, FALSE, ਨਿਰਾ);
    icon_ਸੈੱਟ = gtk_icon_ਸੈੱਟ_new_from_pixbuf(pixbuf);

    gtk_icon_factory_ਜੋੜੋ(factory, stock_id, icon_ਸੈੱਟ);

    gtk_icon_ਸੈੱਟ_unref(icon_ਸੈੱਟ);
    g_object_unref(pixbuf);
}
# endif

    ਸਥਿਰ ਪੂਰਨ
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    ਜੇ (mch_is_ਭਰਿਆ_Name(dest))
    {
	ਵਾਪਸ vim_fexists(dest);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਥਿਰ ਅਚਲ ਅੱਖਰ   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	    i;

	ਲਈ (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    ਜੇ (gui_ਲੱਭੋ_bitmap(dest, buf, (ਅੱਖਰ *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		ਵਾਪਸ TRUE;
	    }

	ਵਾਪਸ FALSE;
    }
}

    ਸਥਿਰ GtkWidget *
ਲੱਦੋ_menu_iconfile(char_u *name, Gtk_Icon_ਮਾਪ icon_ਮਾਪ)
{
    GtkWidget	    *image = ਨਿਰਾ;
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ਪੂਰਨ		     pixel_ਮਾਪ = -1;

    ਚੋਣ (icon_ਮਾਪ)
    {
	ਹਾਲਤ GTK_ICON_ਮਾਪ_MENU:
	    pixel_ਮਾਪ = 16;
	    ਰੋਕੋ;
	ਹਾਲਤ GTK_ICON_ਮਾਪ_SMALL_TOOLBAR:
	    pixel_ਮਾਪ = 16;
	    ਰੋਕੋ;
	ਹਾਲਤ GTK_ICON_ਮਾਪ_LARGE_TOOLBAR:
	    pixel_ਮਾਪ = 24;
	    ਰੋਕੋ;
	ਹਾਲਤ GTK_ICON_ਮਾਪ_BUTTON:
	    pixel_ਮਾਪ = 16;
	    ਰੋਕੋ;
	ਹਾਲਤ GTK_ICON_ਮਾਪ_DND:
	    pixel_ਮਾਪ = 32;
	    ਰੋਕੋ;
	ਹਾਲਤ GTK_ICON_ਮਾਪ_DIALOG:
	    pixel_ਮਾਪ = 48;
	    ਰੋਕੋ;
	ਹਾਲਤ GTK_ICON_ਮਾਪ_INVALID:
	    // FALLTHROUGH
	ਮੂਲ:
	    pixel_ਮਾਪ = 0;
	    ਰੋਕੋ;
    }

    ਜੇ (pixel_ਮਾਪ > 0 || pixel_ਮਾਪ == -1)
    {
	GdkPixbuf * ਅਚਲ pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((ਅਚਲ ਅੱਖਰ *)name,
		pixel_ਮਾਪ, pixel_ਮਾਪ, TRUE, ਨਿਰਾ);
	ਜੇ (pixbuf != ਨਿਰਾ)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    ਜੇ (image == ਨਿਰਾ)
	image = gtk_image_new_from_icon_name("image-missing", icon_ਮਾਪ);

    ਵਾਪਸ image;
# ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,10,0)
    Gtk_Icon_ਸੈੱਟ	    *icon_ਸੈੱਟ;
    Gtk_Icon_ਸਰੋਤ   *icon_ਸਰੋਤ;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_ਸੈੱਟ = gtk_icon_ਸੈੱਟ_new();
    icon_ਸਰੋਤ = gtk_icon_ਸਰੋਤ_new();

    gtk_icon_ਸਰੋਤ_ਸੈੱਟ_filename(icon_ਸਰੋਤ, (ਅਚਲ ਅੱਖਰ *)name);
    gtk_icon_ਸੈੱਟ_ਜੋੜੋ_ਸਰੋਤ(icon_ਸੈੱਟ, icon_ਸਰੋਤ);

    image = gtk_image_new_from_icon_ਸੈੱਟ(icon_ਸੈੱਟ, icon_ਮਾਪ);

    gtk_icon_ਸਰੋਤ_free(icon_ਸਰੋਤ);
    gtk_icon_ਸੈੱਟ_unref(icon_ਸੈੱਟ);

    ਵਾਪਸ image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    ਸਥਿਰ GtkWidget *
create_menu_icon(vimmenu_T *menu, Gtk_Icon_ਮਾਪ icon_ਮਾਪ)
{
    GtkWidget	*image = ਨਿਰਾ;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    ਜੇ (menu->iconfile != ਨਿਰਾ && lookup_menu_iconfile(menu->iconfile, buf))
	image = ਲੱਦੋ_menu_iconfile(buf, icon_ਮਾਪ);

    // If not found and not builtin specified try using the menu name.
    ਜੇ (image == ਨਿਰਾ && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = ਲੱਦੋ_menu_iconfile(buf, icon_ਮਾਪ);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    ਜੇ (image == ਨਿਰਾ)
    {
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
	ਅਚਲ ਅੱਖਰ *icon_name = ਨਿਰਾ;
	ਅਚਲ ਪੂਰਨ   n_ਨਾਂ = G_N_ELEMENTS(menu_themed_ਨਾਂ);

	ਜੇ (menu->iconidx >= 0 && menu->iconidx < n_ਨਾਂ)
	    icon_name = menu_themed_ਨਾਂ[menu->iconidx];
	ਜੇ (icon_name == ਨਿਰਾ)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_ਮਾਪ);
# ਨਹੀਂਤਾਂ
	ਅਚਲ ਅੱਖਰ  *stock_id;
	ਅਚਲ ਪੂਰਨ   n_ids = G_N_ELEMENTS(menu_stock_ids);

	ਜੇ (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	ਨਹੀਂਤਾਂ
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_ਮਾਪ);
# endif
    }

    ਵਾਪਸ image;
}

    ਸਥਿਰ gint
toolbar_button_focus_ਵਿੱਚ_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
			      GdkEventFocus *event ਅਣਵਰਤਿਆ,
			      gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    ਜੇ (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ // FEAT_TOOLBAR

#ਜੇ defined(FEAT_TOOLBAR)

    ਖਾਲੀ
gui_gtk_ਰਜਿਸਟਰ_stock_icons(ਖਾਲੀ)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define ਜੋੜੋ_ICON(Name, ਅੰਕੜੇ_2) ਜੋੜੋ_stock_icon(factory, Name, ਅੰਕੜੇ_2, (ਪੂਰਨ)ਆਕਾਰ(ਅੰਕੜੇ_2))

    ਜੋੜੋ_ICON("vim-build-tags",		  stock_vim_build_ਟੈਗ_ਸੂਚੀ);
    ਜੋੜੋ_ICON("vim-find-help",		  stock_vim_ਲੱਭੋ_help);
    ਜੋੜੋ_ICON("vim-save-all",		  stock_vim_save_all);
    ਜੋੜੋ_ICON("vim-session-load",	  stock_vim_session_ਲੱਦੋ);
    ਜੋੜੋ_ICON("vim-session-new",		  stock_vim_session_new);
    ਜੋੜੋ_ICON("vim-session-save",	  stock_vim_session_save);
    ਜੋੜੋ_ICON("vim-shell",		  stock_vim_shell);
    ਜੋੜੋ_ICON("vim-window-maximize",	  stock_vim_ਖਿੜਕੀ_maximize);
    ਜੋੜੋ_ICON("vim-window-maximize-width", stock_vim_ਖਿੜਕੀ_maximize_ਚੌੜਾਈ);
    ਜੋੜੋ_ICON("vim-window-minimize",	  stock_vim_ਖਿੜਕੀ_minimize);
    ਜੋੜੋ_ICON("vim-window-minimize-width", stock_vim_ਖਿੜਕੀ_minimize_ਚੌੜਾਈ);
    ਜੋੜੋ_ICON("vim-window-split",	  stock_vim_ਖਿੜਕੀ_ਵੰਡੋ);
    ਜੋੜੋ_ICON("vim-window-split-vertical", stock_vim_ਖਿੜਕੀ_ਵੰਡੋ_vertical);

#  undef ਜੋੜੋ_ICON

    gtk_icon_factory_ਜੋੜੋ_default(factory);
    g_object_unref(factory);
# ਨਹੀਂਤਾਂ // defined(USE_GRESOURCE)
    ਅਚਲ ਅੱਖਰ * ਅਚਲ ਰਸਤਾ_ਅਗੇਤਰ = "/org/vim/gui/icon";
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    GdkScreen    *screen = ਨਿਰਾ;
    GtkIconTheme *icon_theme = ਨਿਰਾ;

    ਜੇ (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    ਨਹੀਂਤਾਂ
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_ਜੋੜੋ_resource_ਰਸਤਾ(icon_theme, ਰਸਤਾ_ਅਗੇਤਰ);
#  elif GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    Icon_ਨਾਂ *ਨਾਂ;

    ਲਈ (ਨਾਂ = stock_vim_icons; ਨਾਂ->icon_name != ਨਿਰਾ; ਨਾਂ++)
    {
	ਅੱਖਰ ਰਸਤਾ[MAXPATHL];

	vim_snprintf(ਰਸਤਾ, MAXPATHL, "%s/%s", ਰਸਤਾ_ਅਗੇਤਰ, ਨਾਂ->file_name);
	GdkPixbuf *pixbuf = ਨਿਰਾ;
	pixbuf = gdk_pixbuf_new_from_resource(ਰਸਤਾ, ਨਿਰਾ);
	ਜੇ (pixbuf != ਨਿਰਾ)
	{
	    ਅਚਲ gint ਮਾਪ = MAX(gdk_pixbuf_get_ਚੌੜਾਈ(pixbuf),
				  gdk_pixbuf_get_ਉਚਾਈ(pixbuf));
	    ਜੇ (ਮਾਪ > 16)
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
		ਜੇ (pixbuf == ਨਿਰਾ)
		    pixbuf = src;
		ਨਹੀਂਤਾਂ
		    g_object_unref(src);
	    }
	    gtk_icon_theme_ਜੋੜੋ_builtin_icon(ਨਾਂ->icon_name, ਮਾਪ, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * ਅਚਲ factory = gtk_icon_factory_new();
    Icon_ਨਾਂ *ਨਾਂ;

    ਲਈ (ਨਾਂ = stock_vim_icons; ਨਾਂ->icon_name != ਨਿਰਾ; ਨਾਂ++)
    {
	ਅੱਖਰ ਰਸਤਾ[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(ਰਸਤਾ, MAXPATHL, "%s/%s", ਰਸਤਾ_ਅਗੇਤਰ, ਨਾਂ->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(ਰਸਤਾ, ਨਿਰਾ);
	ਜੇ (pixbuf != ਨਿਰਾ)
	{
	    Gtk_Icon_ਸੈੱਟ *icon_ਸੈੱਟ = gtk_icon_ਸੈੱਟ_new_from_pixbuf(pixbuf);
	    gtk_icon_factory_ਜੋੜੋ(factory, ਨਾਂ->icon_name, icon_ਸੈੱਟ);
	    gtk_icon_ਸੈੱਟ_unref(icon_ਸੈੱਟ);
	    g_object_unref(pixbuf);
	}
    }

    gtk_icon_factory_ਜੋੜੋ_default(factory);
    g_object_unref(factory);
#  endif // !GTK_CHECK_VERSION(3,0,0)
# endif // defined(USE_GRESOURCE)
}

#ਜੇ_ਅੰਤ // FEAT_TOOLBAR

#ਜੇ defined(FEAT_MENU)

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
    ਸਥਿਰ char_u *
translate_mnemonic_ਟੈਗ(char_u *name, ਪੂਰਨ use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    ਪੂਰਨ	    n_underscores = 0;

    name = CONVERT_ਮੰਜ਼ਿਲ_UTF8(name);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ਜੇ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ਜੇ (buf != ਨਿਰਾ)
    {
	pdest = buf;
	ਲਈ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ਜੇ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(name);
    ਵਾਪਸ buf;
}

    ਸਥਿਰ ਖਾਲੀ
menu_ਵਸਤੂ_new(vimmenu_T *menu, GtkWidget *ਮਾਪਾ_widget)
{
    GtkWidget	*box;
    char_u	*ਪਾਠ;
    ਪੂਰਨ		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_ਵਸਤੂ_new();
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(box), FALSE);
# ਨਹੀਂਤਾਂ
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(ਮਾਪਾ_widget));
    ਪਾਠ = translate_mnemonic_ਟੈਗ(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((ਅਚਲ ਅੱਖਰ *)ਪਾਠ);
    vim_free(ਪਾਠ);

    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    ਜੇ (menu->actext != ਨਿਰਾ && menu->actext[0] != NUL)
    {
	ਪਾਠ = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->actext);

	gtk_box_pack_ਸਿਰਾ(GTK_BOX(box),
			 gtk_label_new((ਅਚਲ ਅੱਖਰ *)ਪਾਠ),
			 FALSE, FALSE, 0);

	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(ਪਾਠ);
    }

    gtk_container_ਜੋੜੋ(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu(vimmenu_T *menu, ਪੂਰਨ idx)
{
    vimmenu_T	*ਮਾਪਾ;
    GtkWidget	*ਮਾਪਾ_widget;

    ਜੇ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	ਵਾਪਸ;
    }

    ਮਾਪਾ = menu->ਮਾਪਾ;

    ਜੇ ((ਮਾਪਾ != ਨਿਰਾ && ਮਾਪਾ->submenu_id == ਨਿਰਾ)
	    || !menu_is_menubar(menu->name))
	ਵਾਪਸ;

    ਮਾਪਾ_widget = (ਮਾਪਾ != ਨਿਰਾ) ? ਮਾਪਾ->submenu_id : gui.menubar;
    menu_ਵਸਤੂ_new(menu, ਮਾਪਾ_widget);

# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    // since the tearoff should always appear first, increment idx
    ਜੇ (ਮਾਪਾ != ਨਿਰਾ && !menu_is_popup(ਮਾਪਾ->name))
	++idx;
# endif

    gtk_menu_shell_ਪਾਓ(GTK_MENU_SHELL(ਮਾਪਾ_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_ਸੈੱਟ_accel_ਸਮੂਹ(GTK_MENU(menu->submenu_id), gui.accel_ਸਮੂਹ);
    gtk_menu_ਵਸਤੂ_ਸੈੱਟ_submenu(GTK_MENU_ਵਸਤੂ(menu->id), menu->submenu_id);

# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_ਵਸਤੂ_new();
    ਜੇ (vim_strchr(p_go, GO_TEAROFF) != ਨਿਰਾ)
	gtk_widget_show(menu->tearoff_handle);
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  ਨਹੀਂਤਾਂ
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    ਸਥਿਰ ਖਾਲੀ
menu_ਵਸਤੂ_activate(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    gui_menu_cb((vimmenu_T *)ਅੰਕੜੇ);
}

    ਸਥਿਰ ਖਾਲੀ
menu_ਵਸਤੂ_select(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    ਸਥਿਰ ਪੂਰਨ	did_ਸੁਨੇਹਾ = FALSE;

    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	ਵਾਪਸ;
    menu = (vimmenu_T *)ਅੰਕੜੇ;
    tooltip = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP]);
    ਜੇ (tooltip != ਨਿਰਾ && utf_valid_string(tooltip, ਨਿਰਾ))
    {
	ਸੁਨੇਹਾ((ਅੱਖਰ *)tooltip);
	did_ਸੁਨੇਹਾ = TRUE;
	setcursor();
	ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (did_ਸੁਨੇਹਾ)
    {
	ਸੁਨੇਹਾ("");
	did_ਸੁਨੇਹਾ = FALSE;
	setcursor();
	ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
    }
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);
}

    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(vimmenu_T *menu, ਪੂਰਨ idx)
{
    vimmenu_T *ਮਾਪਾ;

    ਮਾਪਾ = menu->ਮਾਪਾ;

# ifdef FEAT_TOOLBAR
    ਜੇ (menu_is_toolbar(ਮਾਪਾ->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = ਨਿਰਾ;

	ਜੇ (menu_is_separator(menu->name))
	{
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	    Gtk_Tool_ਵਸਤੂ *ਵਸਤੂ = gtk_separator_tool_ਵਸਤੂ_new();
	    gtk_separator_tool_ਵਸਤੂ_ਸੈੱਟ_draw(GTK_SEPARATOR_TOOL_ਵਸਤੂ(ਵਸਤੂ),
		    TRUE);
	    gtk_tool_ਵਸਤੂ_ਸੈੱਟ_expand(GTK_TOOL_ਵਸਤੂ(ਵਸਤੂ), FALSE);
	    gtk_widget_show(GTK_WIDGET(ਵਸਤੂ));

	    gtk_toolbar_ਪਾਓ(toolbar, ਵਸਤੂ, idx);
#  ਨਹੀਂਤਾਂ
	    gtk_toolbar_ਪਾਓ_space(toolbar, idx);
#  endif
	    menu->id = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u *ਪਾਠ;
	    char_u *tooltip;

	    ਪਾਠ    = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->dname);
	    tooltip = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP]);
	    ਜੇ (tooltip != ਨਿਰਾ && !utf_valid_string(tooltip, ਨਿਰਾ))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);

#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	    {
		GtkWidget *icon;
		Gtk_Tool_ਵਸਤੂ *ਵਸਤੂ;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_ਮਾਪ(toolbar));
		ਵਸਤੂ = gtk_tool_button_new(icon, (ਅਚਲ gchar *)ਪਾਠ);
		gtk_tool_ਵਸਤੂ_ਸੈੱਟ_tooltip_ਪਾਠ(ਵਸਤੂ, (ਅਚਲ gchar *)tooltip);
		g_signal_connect(G_OBJECT(ਵਸਤੂ), "clicked",
			G_CALLBACK(&menu_ਵਸਤੂ_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(ਵਸਤੂ));

		gtk_toolbar_ਪਾਓ(toolbar, ਵਸਤੂ, idx);

		menu->id = GTK_WIDGET(ਵਸਤੂ);
	    }
#  ਨਹੀਂਤਾਂ
	    menu->id = gtk_toolbar_ਪਾਓ_ਵਸਤੂ(
		    toolbar,
		    (ਅਚਲ ਅੱਖਰ *)ਪਾਠ,
		    (ਅਚਲ ਅੱਖਰ *)tooltip,
		    ਨਿਰਾ,
		    create_menu_icon(menu, gtk_toolbar_get_icon_ਮਾਪ(toolbar)),
		    G_CALLBACK(&menu_ਵਸਤੂ_activate),
		    menu,
		    idx);
#  endif

	    ਜੇ (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_ਵਿੱਚ_event), ਨਿਰਾ);

	    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(ਪਾਠ);
	    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);
	}
    }
    ਨਹੀਂਤਾਂ
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	ਜੇ (ਮਾਪਾ == ਨਿਰਾ || ਮਾਪਾ->submenu_id == ਨਿਰਾ)
	    ਵਾਪਸ;

# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	ਜੇ (!menu_is_popup(ਮਾਪਾ->name))
	    ++idx;
# endif

	ਜੇ (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	    menu->id = gtk_separator_menu_ਵਸਤੂ_new();
# ਨਹੀਂਤਾਂ
	    menu->id = gtk_menu_ਵਸਤੂ_new();
	    gtk_widget_ਸੈੱਟ_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_ਪਾਓ(GTK_MENU_SHELL(ਮਾਪਾ->submenu_id),
		    menu->id, idx);

	    ਵਾਪਸ;
	}

	// Add textual menu item.
	menu_ਵਸਤੂ_new(menu, ਮਾਪਾ->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_ਪਾਓ(GTK_MENU_SHELL(ਮਾਪਾ->submenu_id),
		menu->id, idx);

	ਜੇ (menu->id != ਨਿਰਾ)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_ਵਸਤੂ_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_ਵਸਤੂ_select), menu);
	}
    }
}
#ਜੇ_ਅੰਤ // FEAT_MENU


    ਖਾਲੀ
gui_mch_ਸੈੱਟ_ਪਾਠ_area_pos(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#ਜੇ defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ਖਾਲੀ
gui_gtk_ਸੈੱਟ_mnemonics(ਪੂਰਨ enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ਜੇ (menu->id == ਨਿਰਾ)
	    ਜਾਰੀ;

	name = translate_mnemonic_ਟੈਗ(menu->name, enable);
	gtk_label_ਸੈੱਟ_ਪਾਠ_with_mnemonic(GTK_LABEL(menu->label),
					 (ਅਚਲ ਅੱਖਰ *)name);
	vim_free(name);
    }
}

# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    ਸਥਿਰ ਖਾਲੀ
recurse_tearoffs(vimmenu_T *menu, ਪੂਰਨ val)
{
    ਲਈ (; menu != ਨਿਰਾ; menu = menu->ਅਗਲਾ)
    {
	ਜੇ (menu->submenu_id != ਨਿਰਾ && menu->tearoff_handle != ਨਿਰਾ
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    ਜੇ (val)
		gtk_widget_show(menu->tearoff_handle);
	    ਨਹੀਂਤਾਂ
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    ਖਾਲੀ
gui_mch_toggle_tearoffs(ਪੂਰਨ enable ਅਣਵਰਤਿਆ)
{
    // Do nothing
}
# ਨਹੀਂਤਾਂ
    ਖਾਲੀ
gui_mch_toggle_tearoffs(ਪੂਰਨ enable)
{
    recurse_tearoffs(ਜੜ੍ਹ_menu, enable);
}
# endif
#ਜੇ_ਅੰਤ // FEAT_MENU

#ਜੇ defined(FEAT_TOOLBAR)
    ਸਥਿਰ ਪੂਰਨ
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*ਗੰਢ;
    ਪੂਰਨ		idx = 0;

    ਲਈ (ਗੰਢ = menu->ਮਾਪਾ->children; ਗੰਢ != menu; ਗੰਢ = ਗੰਢ->ਅਗਲਾ)
    {
	g_return_val_if_fail(ਗੰਢ != ਨਿਰਾ, -1);
	++idx;
    }

    ਵਾਪਸ idx;
}
#ਜੇ_ਅੰਤ // FEAT_TOOLBAR


#ਜੇ defined(FEAT_TOOLBAR)
    ਖਾਲੀ
gui_mch_menu_ਸੈੱਟ_tip(vimmenu_T *menu)
{
    ਜੇ (menu->id == ਨਿਰਾ || menu->ਮਾਪਾ == ਨਿਰਾ || gui.toolbar == ਨਿਰਾ)
	ਵਾਪਸ;

    char_u *tooltip;

    tooltip = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP]);
    ਜੇ (tooltip != ਨਿਰਾ && utf_valid_string(tooltip, ਨਿਰਾ))
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_ਸੈੱਟ_tooltip_ਪਾਠ(menu->id, (ਅਚਲ gchar *)tooltip);
# ਨਹੀਂਤਾਂ
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_ਸੈੱਟ_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (ਅਚਲ ਅੱਖਰ *)tooltip, ਨਿਰਾ);
# endif
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);
}
#ਜੇ_ਅੰਤ // FEAT_TOOLBAR


#ਜੇ defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    ਖਾਲੀ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    ਜੇ (menu->id != ਨਿਰਾ)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    ਜੇ (menu->id != ਨਿਰਾ && gui.menubar != ਨਿਰਾ
			    && gtk_widget_get_ਮਾਪਾ(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    ਜੇ (menu->ਮਾਪਾ != ਨਿਰਾ && menu_is_toolbar(menu->ਮਾਪਾ->name))
    {
	ਜੇ (menu_is_separator(menu->name))
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	{
	    Gtk_Tool_ਵਸਤੂ *ਵਸਤੂ = ਨਿਰਾ;

	    ਵਸਤੂ = gtk_toolbar_get_nth_ਵਸਤੂ(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(ਵਸਤੂ));
	}
#  ਨਹੀਂਤਾਂ
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	ਨਹੀਂਤਾਂ ਜੇ (menu->id != ਨਿਰਾ)
	    gtk_widget_destroy(menu->id);
    }
    ਨਹੀਂਤਾਂ
# endif // FEAT_TOOLBAR
    {
	ਜੇ (menu->submenu_id != ਨਿਰਾ)
	    gtk_widget_destroy(menu->submenu_id);

	ਜੇ (menu->id != ਨਿਰਾ)
	    gtk_widget_destroy(menu->id);
    }

    ਜੇ (menu->id != ਨਿਰਾ)
	g_object_unref(menu->id);
    menu->submenu_id = ਨਿਰਾ;
    menu->id = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_thumb(scrollbar_T *sb, ਲੰਮਾ val, ਲੰਮਾ ਮਾਪ, ਲੰਮਾ max)
{
    ਜੇ (sb->id == ਨਿਰਾ)
	ਵਾਪਸ;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_ਸੈੱਟ_ਹੇਠਲਾ(adjustment, 0.0);
    gtk_adjustment_ਸੈੱਟ_ਮੁੱਲ(adjustment, val);
    gtk_adjustment_ਸੈੱਟ_ਉੱਤਲਾ(adjustment, max + 1);
    gtk_adjustment_ਸੈੱਟ_ਪੰਨਾ_ਮਾਪ(adjustment, ਮਾਪ);
    gtk_adjustment_ਸੈੱਟ_ਪੰਨਾ_increment(adjustment,
	    ਮਾਪ < 3L ? 1L : ਮਾਪ - 2L);
    gtk_adjustment_ਸੈੱਟ_ਕਦਮ_increment(adjustment, 1.0);

    g_signal_handler_ਖੰਡ(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,18,0)
    gtk_adjustment_ਬਦਲਿਆ(adjustment);
#ਜੇ_ਅੰਤ

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_pos(scrollbar_T *sb, ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    ਜੇ (sb->id != ਨਿਰਾ)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    ਪੂਰਨ
gui_mch_get_scrollbar_xpadding(ਖਾਲੀ)
{
    ਪੂਰਨ xpad;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    xpad = gtk_widget_get_allocated_ਚੌੜਾਈ(gui.formwin)
	  - gtk_widget_get_allocated_ਚੌੜਾਈ(gui.drawarea) - gui.scrollbar_ਚੌੜਾਈ;
#ਨਹੀਂਤਾਂ
    xpad = gui.formwin->ਵੰਡ.ਚੌੜਾਈ - gui.drawarea->ਵੰਡ.ਚੌੜਾਈ
							 - gui.scrollbar_ਚੌੜਾਈ;
#ਜੇ_ਅੰਤ
    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ] && gui.which_scrollbars[SBAR_ਸੱਜਾ])
	xpad -= gui.scrollbar_ਚੌੜਾਈ;

    ਵਾਪਸ (xpad < 0) ? 0 : xpad;
}

    ਪੂਰਨ
gui_mch_get_scrollbar_ypadding(ਖਾਲੀ)
{
    ਪੂਰਨ ypad;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ypad = gtk_widget_get_allocated_ਉਚਾਈ(gui.formwin)
	- gtk_widget_get_allocated_ਉਚਾਈ(gui.drawarea) - gui.scrollbar_ਉਚਾਈ;
#ਨਹੀਂਤਾਂ
    ypad = gui.formwin->ਵੰਡ.ਉਚਾਈ - gui.drawarea->ਵੰਡ.ਉਚਾਈ
							- gui.scrollbar_ਉਚਾਈ;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    ਸਥਿਰ ਖਾਲੀ
adjustment_ਮੁੱਲ_ਬਦਲਿਆ(GtkAdjustment *adjustment, gpointer ਅੰਕੜੇ)
{
    scrollbar_T	*sb;
    ਲੰਮਾ	ਮੁੱਲ;
    ਪੂਰਨ		dragging = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    // cancel any preediting
    ਜੇ (im_is_preediting())
	xim_reset();
#ਜੇ_ਅੰਤ

    sb = gui_ਲੱਭੋ_scrollbar((ਲੰਮਾ)ਅੰਕੜੇ);
    ਮੁੱਲ = gtk_adjustment_get_ਮੁੱਲ(adjustment);
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    ਜੇ (sb != ਨਿਰਾ)
    {
	dragging = TRUE;

	ਜੇ (sb->wp != ਨਿਰਾ && GDK_IS_DRAWABLE(sb->id->ਖਿੜਕੀ_2))
	{
	    ਪੂਰਨ			x;
	    ਪੂਰਨ			y;
	    GdkModifierType	state;
	    ਪੂਰਨ			ਚੌੜਾਈ;
	    ਪੂਰਨ			ਉਚਾਈ;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_ਖਿੜਕੀ_get_ਸੂਚਕ(sb->id->ਖਿੜਕੀ_2, &x, &y, &state);
	    gdk_ਖਿੜਕੀ_get_ਮਾਪ(sb->id->ਖਿੜਕੀ_2, &ਚੌੜਾਈ, &ਉਚਾਈ);
	    ਜੇ (x >= 0 && x < ਚੌੜਾਈ && y >= 0 && y < ਉਚਾਈ)
	    {
		ਜੇ (y < ਚੌੜਾਈ)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    ਮੁੱਲ = sb->wp->w_topline - 2;
		}
		ਨਹੀਂਤਾਂ ਜੇ (y > ਉਚਾਈ - ਚੌੜਾਈ)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    ਮੁੱਲ = sb->wp->w_topline;
		}
	    }
	}
    }
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, ਮੁੱਲ, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    ਖਾਲੀ
gui_mch_create_scrollbar(scrollbar_T *sb, ਪੂਰਨ orient)
{
    ਜੇ (orient == SBAR_HORIZ)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	sb->id = gtk_hscrollbar_new(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ ਜੇ (orient == SBAR_VERT)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	sb->id = gtk_vscrollbar_new(ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    ਜੇ (sb->id == ਨਿਰਾ)
	ਵਾਪਸ;

    GtkAdjustment *adjustment;

    gtk_widget_ਸੈੱਟ_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_ਮੁੱਲ_ਬਦਲਿਆ),
	    GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(sb->ident));
    gui_mch_update();
}

    ਖਾਲੀ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ਜੇ (sb->id != ਨਿਰਾ)
    {
	gtk_widget_destroy(sb->id);
	sb->id = ਨਿਰਾ;
    }
    gui_mch_update();
}

#ਜੇ defined(FEAT_BROWSE)
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
gui_mch_browse(ਪੂਰਨ saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext ਅਣਵਰਤਿਆ,
	       char_u *initdir,
	       char_u *filter)
{
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,20,0)
    GtkFileChooserNative	*fc;
# ਨਹੀਂਤਾਂ
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    ਅਚਲ gchar		*domain = "Gtk";

    title = CONVERT_ਮੰਜ਼ਿਲ_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    ਜੇ (initdir == ਨਿਰਾ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਭਰਿਆ_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    ਜੋੜੋ_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_ਸੈੱਟ_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, ਨਿਰਾ);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,20,0)
    fc = gtk_file_chooser_native_new(
# ਨਹੀਂਤਾਂ
    fc = gtk_file_chooser_dialog_new(
# endif
	    (ਅਚਲ gchar *)title,
	    GTK_ਖਿੜਕੀ(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# ਨਹੀਂਤਾਂ
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  ਨਹੀਂਤਾਂ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    ਨਿਰਾ);
# endif
    gtk_file_chooser_ਸੈੱਟ_current_folder(GTK_FILE_CHOOSER(fc),
						       (ਅਚਲ gchar *)dirbuf);

    ਜੇ (filter != ਨਿਰਾ && *filter != NUL)
    {
	ਪੂਰਨ     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ਜੇ (patt != ਨਿਰਾ)
	{
	    ਜਦੋਂ (p != ਨਿਰਾ && *p != NUL)
	    {
		ਜੇ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ਜੇ (*p == '\t')
			gtk_file_filter_ਸੈੱਟ_name(gfilter, (gchar *)patt);
		    ਨਹੀਂਤਾਂ
		    {
			gtk_file_filter_ਜੋੜੋ_ਨਮੂਨਾ(gfilter, (gchar *)patt);
			ਜੇ (*p == '\n')
			{
			    gtk_file_chooser_ਜੋੜੋ_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    ਜੇ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    ਜੇ (saving && dflt != ਨਿਰਾ && *dflt != NUL)
	gtk_file_chooser_ਸੈੱਟ_current_name(GTK_FILE_CHOOSER(fc), (ਅੱਖਰ *)dflt);

    gui.browse_fname = ਨਿਰਾ;
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,20,0)
    ਜੇ (gtk_native_dialog_ਲੜੀਵਾਰ(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# ਨਹੀਂਤਾਂ
    ਜੇ (gtk_dialog_ਲੜੀਵਾਰ(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	ਅੱਖਰ *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,20,0)
    g_object_unref(fc);
# ਨਹੀਂਤਾਂ
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(title);
    ਜੇ (gui.browse_fname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // shorten the file name if possible
    ਵਾਪਸ vim_strsave(shorten_fname1(gui.browse_fname));
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
# ਜੇ defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = ਨਿਰਾ;

    title = CONVERT_ਮੰਜ਼ਿਲ_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (ਅਚਲ gchar *)title,
	    GTK_ਖਿੜਕੀ(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  ਨਹੀਂਤਾਂ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    ਨਿਰਾ);

    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    ਜੇ (initdir == ਨਿਰਾ || *initdir == NUL
	       || vim_ਭਰਿਆ_Name(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    ਜੋੜੋ_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_ਸੈੱਟ_filename(GTK_FILE_CHOOSER(dirdlg),
						      (ਅਚਲ gchar *)dirbuf);

    // Run the dialog.
    ਜੇ (gtk_dialog_ਲੜੀਵਾਰ(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    ਜੇ (dirname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    ਵਾਪਸ p;

# ਨਹੀਂਤਾਂ // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    ਵਾਪਸ gui_mch_browse(0, title, ਨਿਰਾ, ਨਿਰਾ, initdir, ਨਿਰਾ);
# endif // !defined(GTK_FILE_CHOOSER)
}


#ਜੇ_ਅੰਤ	// FEAT_BROWSE

#ਜੇ defined(FEAT_GUI_DIALOG)

    ਸਥਿਰ GtkWidget *
create_ਸੁਨੇਹਾ_dialog(ਪੂਰਨ type, char_u *title, char_u *ਸੁਨੇਹਾ_2)
{
    GtkWidget	    *dialog;
    Gtk_ਸੁਨੇਹਾ_Type  ਸੁਨੇਹਾ_type;

    ਚੋਣ (type)
    {
	ਹਾਲਤ VIM_ERROR:	    ਸੁਨੇਹਾ_type = GTK_ਸੁਨੇਹਾ_ERROR;	 ਰੋਕੋ;
	ਹਾਲਤ VIM_WARNING:   ਸੁਨੇਹਾ_type = GTK_ਸੁਨੇਹਾ_WARNING;	 ਰੋਕੋ;
	ਹਾਲਤ VIM_QUESTION:  ਸੁਨੇਹਾ_type = GTK_ਸੁਨੇਹਾ_QUESTION; ਰੋਕੋ;
	ਮੂਲ:	    ਸੁਨੇਹਾ_type = GTK_ਸੁਨੇਹਾ_INFO;	 ਰੋਕੋ;
    }

    ਸੁਨੇਹਾ_2 = CONVERT_ਮੰਜ਼ਿਲ_UTF8(ਸੁਨੇਹਾ_2);
    dialog  = gtk_ਸੁਨੇਹਾ_dialog_new(GTK_ਖਿੜਕੀ(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_ਮਾਪਾ,
				     ਸੁਨੇਹਾ_type,
				     GTK_BUTTONS_NONE,
				     "%s", (ਅਚਲ ਅੱਖਰ *)ਸੁਨੇਹਾ_2);
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(ਸੁਨੇਹਾ_2);

    ਜੇ (title != ਨਿਰਾ)
    {
	title = CONVERT_ਮੰਜ਼ਿਲ_UTF8(title);
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(GTK_ਖਿੜਕੀ(dialog), (ਅਚਲ ਅੱਖਰ *)title);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(title);
    }
    ਨਹੀਂਤਾਂ ਜੇ (type == VIM_GENERIC)
    {
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(GTK_ਖਿੜਕੀ(dialog), "VIM");
    }

    ਵਾਪਸ dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    ਸਥਿਰ ਅੱਖਰ **
ਵੰਡੋ_button_string(char_u *button_string, ਪੂਰਨ *n_buttons)
{
    ਅੱਖਰ	    **array;
    char_u	    *p;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    ਗਿਣਤੀ_2 = 1;

    ਲਈ (p = button_string; *p != NUL; ++p)
	ਜੇ (*p == DLG_BUTTON_SEP)
	    ++ਗਿਣਤੀ_2;

    array = ALLOC_MULT(ਅੱਖਰ *, ਗਿਣਤੀ_2 + 1);
    ਗਿਣਤੀ_2 = 0;

    ਜੇ (array != ਨਿਰਾ)
    {
	array[ਗਿਣਤੀ_2++] = (ਅੱਖਰ *)button_string;
	ਲਈ (p = button_string; *p != NUL; )
	{
	    ਜੇ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ਗਿਣਤੀ_2++] = (ਅੱਖਰ *)p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ਨਹੀਂਤਾਂ
		MB_PTR_ADV(p);
	}
	array[ਗਿਣਤੀ_2] = ਨਿਰਾ; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = ਗਿਣਤੀ_2;
    ਵਾਪਸ array;
}

# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ਸਥਿਰ ਅੱਖਰ **
ਵੰਡੋ_button_translation(ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_2)
{
    ਅੱਖਰ    **buttons = ਨਿਰਾ;
    char_u  *str;
    ਪੂਰਨ	    n_buttons = 0;
    ਪੂਰਨ	    n_expected = 1;

    ਲਈ (str = (char_u *)ਸੁਨੇਹਾ_2; *str != NUL; ++str)
	ਜੇ (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(ਸੁਨੇਹਾ_2);
    ਜੇ (str != ਨਿਰਾ)
    {
	ਜੇ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    str = vim_strsave(str);

	ਜੇ (str != ਨਿਰਾ)
	    buttons = ਵੰਡੋ_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    ਜੇ (buttons == ਨਿਰਾ || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = ਨਿਰਾ;
	str = vim_strsave((char_u *)ਸੁਨੇਹਾ_2);

	ਜੇ (str != ਨਿਰਾ)
	    buttons = ਵੰਡੋ_button_string(str, &n_buttons);
	ਜੇ (buttons == ਨਿਰਾ)
	    vim_free(str);
    }

    ਵਾਪਸ buttons;
}

    ਸਥਿਰ ਪੂਰਨ
button_equal(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b)
{
    ਜਦੋਂ (*a != '\0' && *b != '\0')
    {
	ਜੇ (*a == '_' && *++a == '\0')
	    ਰੋਕੋ;
	ਜੇ (*b == '_' && *++b == '\0')
	    ਰੋਕੋ;

	ਜੇ (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    ਵਾਪਸ FALSE;

	a = g_utf8_ਅਗਲਾ_char(a);
	b = g_utf8_ਅਗਲਾ_char(b);
    }

    ਵਾਪਸ (*a == '\0' && *b == '\0');
}
# endif

    ਸਥਿਰ ਖਾਲੀ
dialog_ਜੋੜੋ_buttons(GtkDialog *dialog, char_u *button_string)
{
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ਅੱਖਰ    **ok;
    ਅੱਖਰ    **ync;  // "yes no cancel"
# endif
    ਅੱਖਰ    **buttons;
    ਪੂਰਨ	    n_buttons = 0;
    ਪੂਰਨ	    idx;

    button_string = vim_strsave(button_string); // must be writable
    ਜੇ (button_string == ਨਿਰਾ)
	ਵਾਪਸ;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ਜੇ (vim_strchr(p_go, GO_VERTICAL) != ਨਿਰਾ)
    {
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	GtkWidget	*vbutton_box;

	vbutton_box = gtk_vbutton_box_new();
	gtk_widget_show(vbutton_box);
	gtk_box_pack_ਸਿਰਾ(GTK_BOX(GTK_DIALOG(dialog)->vbox),
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
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ok	    = ਵੰਡੋ_button_translation(N_("&Ok"));
    ync     = ਵੰਡੋ_button_translation(N_("&Yes\n&No\n&Cancel"));
# endif
    buttons = ਵੰਡੋ_button_string(button_string, &n_buttons);

    /*
     * Yes, the buttons are in reversed order to match the GNOME 2 desktop
     * environment.  Don't hit me -- it's all about consistency.
     * Well, apparently somebody changed his mind: with GTK 2.2.4 it works the
     * other way around...
     */
    ਲਈ (idx = 1; idx <= n_buttons; ++idx)
    {
	ਅੱਖਰ	*label;
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
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
	ਜੇ (ok != ਨਿਰਾ && ync != ਨਿਰਾ) // almost impossible to fail
	{
	    ਜੇ	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    ਨਹੀਂਤਾਂ ਜੇ (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    ਨਹੀਂਤਾਂ ਜੇ (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    ਨਹੀਂਤਾਂ ਜੇ (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    ਨਹੀਂਤਾਂ ਜੇ (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    ਨਹੀਂਤਾਂ ਜੇ (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    ਨਹੀਂਤਾਂ ਜੇ (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    ਨਹੀਂਤਾਂ ਜੇ (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_ਮੰਜ਼ਿਲ_UTF8((char_u *)label);
	gtk_dialog_ਜੋੜੋ_button(dialog, (ਅਚਲ gchar *)label8, idx);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(label8);
    }

# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ਜੇ (ok != ਨਿਰਾ)
	vim_free(*ok);
    ਜੇ (ync != ਨਿਰਾ)
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _DialogInfo
{
    ਪੂਰਨ		ignore_enter;	    // no default button, ignore "Enter"
    ਪੂਰਨ		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    ਸਥਿਰ gboolean
dialog_ਕੁੰਜੀ_press_event_cb(GtkWidget *widget, Gdk_Event_ਕੁੰਜੀ *event, gpointer ਅੰਕੜੇ)
{
    DialogInfo *di = (DialogInfo *)ਅੰਕੜੇ;

    // Ignore hitting Enter (or Space) when there is no default button.
    ਜੇ (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	ਵਾਪਸ TRUE;
    ਨਹੀਂਤਾਂ    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    ਜੇ (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	ਵਾਪਸ TRUE;
    }

    ਜੇ (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_ਨਕਾਬ()) == 0)
    {
	ਵਾਪਸ gtk_ਖਿੜਕੀ_mnemonic_activate(
		   GTK_ਖਿੜਕੀ(widget), event->keyval,
		   gtk_ਖਿੜਕੀ_get_mnemonic_modifier(GTK_ਖਿੜਕੀ(widget)));
    }

    ਵਾਪਸ FALSE; // continue emission
}

    ਪੂਰਨ
gui_mch_dialog(ਪੂਰਨ	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*ਸੁਨੇਹਾ_2,   // message text
	       char_u	*buttons,   // names of buttons
	       ਪੂਰਨ	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       ਪੂਰਨ	ex_cmd ਅਣਵਰਤਿਆ)
{
    GtkWidget	*dialog;
    GtkWidget	*ਇੰਦਰਾਜ = ਨਿਰਾ;
    char_u	*ਪਾਠ;
    ਪੂਰਨ		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_ਸਰਗਰਮ;

    dialog = create_ਸੁਨੇਹਾ_dialog(type, title, ਸੁਨੇਹਾ_2);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_ਜੋੜੋ_buttons(GTK_DIALOG(dialog), buttons);

    ਜੇ (textfield != ਨਿਰਾ)
    {
	GtkWidget *alignment;

	ਇੰਦਰਾਜ = gtk_ਇੰਦਰਾਜ_new();
	gtk_widget_show(ਇੰਦਰਾਜ);

	// Make Enter work like pressing OK.
	gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_activates_default(GTK_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ), TRUE);

	ਪਾਠ = CONVERT_ਮੰਜ਼ਿਲ_UTF8(textfield);
	gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_ਪਾਠ(GTK_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ), (ਅਚਲ ਅੱਖਰ *)ਪਾਠ);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(ਪਾਠ);

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
	gtk_widget_ਸੈੱਟ_halign(GTK_WIDGET(ਇੰਦਰਾਜ), GTK_ALIGN_CENTER);
	gtk_widget_ਸੈੱਟ_valign(GTK_WIDGET(ਇੰਦਰਾਜ), GTK_ALIGN_CENTER);
	gtk_widget_ਸੈੱਟ_hexpand(GTK_WIDGET(ਇੰਦਰਾਜ), TRUE);
	gtk_widget_ਸੈੱਟ_vexpand(GTK_WIDGET(ਇੰਦਰਾਜ), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# ਨਹੀਂਤਾਂ
	alignment = gtk_alignment_new((ਦਸ਼ਮਲਵ)0.5, (ਦਸ਼ਮਲਵ)0.5,
						      (ਦਸ਼ਮਲਵ)1.0, (ਦਸ਼ਮਲਵ)1.0);
# endif
	gtk_container_ਜੋੜੋ(GTK_CONTAINER(alignment), ਇੰਦਰਾਜ);
	gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	{
	    GtkWidget * ਅਚਲ vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# ਨਹੀਂਤਾਂ
	gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    ਨਹੀਂਤਾਂ
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_ਕੁੰਜੀ_press_event_cb), &dialoginfo);

    ਜੇ (def_but > 0)
    {
	gtk_dialog_ਸੈੱਟ_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    ਨਹੀਂਤਾਂ
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_ਲੜੀਵਾਰ(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    ਜੇ (response != GTK_RESPONSE_NONE)
    {
	ਜੇ (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	ਜੇ (textfield != ਨਿਰਾ)
	{
	    ਪਾਠ = (char_u *)gtk_ਇੰਦਰਾਜ_get_ਪਾਠ(GTK_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ));
	    ਪਾਠ = CONVERT_FROM_UTF8(ਪਾਠ);

	    vim_strncpy(textfield, ਪਾਠ, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(ਪਾਠ);
	}
	gtk_widget_destroy(dialog);
    }

    ਜੇ (gui.dialogs_ਸਰਗਰਮ > 0)
	--gui.dialogs_ਸਰਗਰਮ;
    ਵਾਪਸ response > 0 ? response : 0;
}

#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG


#ਜੇ defined(FEAT_MENU)

    ਖਾਲੀ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# ਜੇ defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ਜੇ (xic != ਨਿਰਾ && g_object_get_ਅੰਕੜੇ(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == ਨਿਰਾ)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_ਵਸਤੂ_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_ਮੰਜ਼ਿਲ_UTF8(name);
	menuitem = gtk_menu_ਵਸਤੂ_new_with_mnemonic((ਅਚਲ ਅੱਖਰ *)name);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(name);
	gtk_widget_show(menuitem);

	submenu = gtk_menu_new();
	gtk_menu_ਵਸਤੂ_ਸੈੱਟ_submenu(GTK_MENU_ਵਸਤੂ(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	gtk_im_multicontext_append_menuitems(GTK_IM_MULTICONTEXT(xic),
					     GTK_MENU_SHELL(submenu));
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(menu->submenu_id),
			  "vim-has-im-menu", GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(TRUE));
    }
#  endif
# endif // FEAT_XIM

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
    {
	GdkEventButton trigger;

	// A pseudo event to have gtk_menu_popup_at_pointer() work. Since the
	// function calculates the popup menu position on the basis of the
	// actual pointer position when it is invoked, the fields x, y, x_root
	// and y_root are set to zero for convenience.
	trigger.type       = GDK_BUTTON_PRESS;
	trigger.ਖਿੜਕੀ_2     = gtk_widget_get_ਖਿੜਕੀ(gui.drawarea);
	trigger.send_event = FALSE;
	trigger.ਸਮਾਂ       = gui.event_ਸਮਾਂ;
	trigger.x	   = 0.0;
	trigger.y	   = 0.0;
	trigger.axes       = ਨਿਰਾ;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = ਨਿਰਾ;
	trigger.x_ਜੜ੍ਹ     = 0.0;
	trigger.y_ਜੜ੍ਹ     = 0.0;

	gtk_menu_popup_at_ਸੂਚਕ(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# ਨਹੀਂਤਾਂ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   ਨਿਰਾ, ਨਿਰਾ,
		   (GtkMenuPositionFunc)ਨਿਰਾ, ਨਿਰਾ,
		   3U, gui.event_ਸਮਾਂ);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
ਸਥਿਰ ਪੂਰਨ popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    ਸਥਿਰ ਖਾਲੀ
popup_menu_position_func(GtkMenu *menu ਅਣਵਰਤਿਆ,
			 gint *x, gint *y,
			 gboolean *ਧੱਕੋ_ਵਿੱਚ ਅਣਵਰਤਿਆ,
			 gpointer user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gdk_ਖਿੜਕੀ_get_origin(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea), x, y);

    ਜੇ (popup_mouse_pos)
    {
	ਪੂਰਨ	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin != ਨਿਰਾ && gui.drawarea != ਨਿਰਾ &&
	     gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) != ਨਿਰਾ)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    ਖਾਲੀ
gui_make_popup(char_u *ਰਸਤਾ_name, ਪੂਰਨ mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_ਲੱਭੋ_menu(ਰਸਤਾ_name);
    ਜੇ (menu == ਨਿਰਾ || menu->submenu_id == ਨਿਰਾ)
	ਵਾਪਸ;

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
    Gdk_ਖਿੜਕੀ * ਅਚਲ win = gtk_widget_get_ਖਿੜਕੀ(gui.drawarea);
    GdkEventButton trigger;

    // A pseudo event to have gtk_menu_popup_at_*() functions work. Since
    // the position where the menu pops up is automatically adjusted by
    // the functions, none of the fields x, y, x_root and y_root has to be
    // set to a specific value here; therefore, they are set to zero for
    // convenience.
    trigger.type       = GDK_BUTTON_PRESS;
    trigger.ਖਿੜਕੀ_2     = win;
    trigger.send_event = FALSE;
    trigger.ਸਮਾਂ       = GDK_CURRENT_ਸਮਾਂ;
    trigger.x	   = 0.0;
    trigger.y	   = 0.0;
    trigger.axes       = ਨਿਰਾ;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = ਨਿਰਾ;
    trigger.x_ਜੜ੍ਹ     = 0.0;
    trigger.y_ਜੜ੍ਹ     = 0.0;

    ਜੇ (mouse_pos)
	gtk_menu_popup_at_ਸੂਚਕ(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    ਨਹੀਂਤਾਂ
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_ਖਿੜਕੀ_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(ਨਿਰਾ, &rect.x, &rect.y, ਨਿਰਾ, ਨਿਰਾ);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# ਨਹੀਂਤਾਂ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    ਨਿਰਾ, ਨਿਰਾ,
	    &popup_menu_position_func, ਨਿਰਾ,
	    0U, (guint32)GDK_CURRENT_ਸਮਾਂ);
# endif
}

#ਜੇ_ਅੰਤ // FEAT_MENU


/*
 * We don't create it twice.
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Shared_ਲੱਭੋ_Replace
{
    GtkWidget *dialog;	// the main dialog widget
    GtkWidget *wword;	// 'Whole word only' check button
    GtkWidget *mcase;	// 'Match case' check button
    GtkWidget *ਉੱਪਰ;	// search direction 'Up' radio button
    GtkWidget *ਥੱਲੇ;	// search direction 'Down' radio button
    GtkWidget *what;	// 'Find what' entry text widget
    GtkWidget *with;	// 'Replace with' entry text widget
    GtkWidget *ਲੱਭੋ;	// 'Find Next' action button
    GtkWidget *replace;	// 'Replace With' action button
    GtkWidget *all;	// 'Replace All' action button
} Shared_ਲੱਭੋ_Replace_2;

ਸਥਿਰ Shared_ਲੱਭੋ_Replace_2 ਲੱਭੋ_widgets = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
ਸਥਿਰ Shared_ਲੱਭੋ_Replace_2 repl_widgets = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};

    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਕੁੰਜੀ_press_event(
		GtkWidget	*widget ਅਣਵਰਤਿਆ,
		Gdk_Event_ਕੁੰਜੀ	*event,
		Shared_ਲੱਭੋ_Replace_2 *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ਜੇ (event->state & (GDK_CONTROL_ਨਕਾਬ | GDK_ਖਿਸਕਾਓ_ਨਕਾਬ))
	ਵਾਪਸ FALSE;

    // the Escape key synthesizes a cancellation action
    ਜੇ (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	ਵਾਪਸ TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ GtkWidget *
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
create_image_button(ਅਚਲ ਅੱਖਰ *stock_id ਅਣਵਰਤਿਆ,
		    ਅਚਲ ਅੱਖਰ *label)
#ਨਹੀਂਤਾਂ
create_image_button(ਅਚਲ ਅੱਖਰ *stock_id,
		    ਅਚਲ ਅੱਖਰ *label)
#ਜੇ_ਅੰਤ
{
    char_u	*ਪਾਠ;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    ਪਾਠ = CONVERT_ਮੰਜ਼ਿਲ_UTF8((char_u *)label);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(box), FALSE);
#ਨਹੀਂਤਾਂ
    box = gtk_hbox_new(FALSE, 3);
#ਜੇ_ਅੰਤ
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ਜੇ (stock_id != ਨਿਰਾ)
	gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_ਮਾਪ_BUTTON),
			   FALSE, FALSE, 0);
#ਜੇ_ਅੰਤ
    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(box),
		       gtk_label_new((ਅਚਲ ਅੱਖਰ *)ਪਾਠ),
		       FALSE, FALSE, 0);

    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(ਪਾਠ);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    gtk_widget_ਸੈੱਟ_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_ਸੈੱਟ_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_ਸੈੱਟ_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_ਸੈੱਟ_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#ਨਹੀਂਤਾਂ
    alignment = gtk_alignment_new((ਦਸ਼ਮਲਵ)0.5, (ਦਸ਼ਮਲਵ)0.5,
						      (ਦਸ਼ਮਲਵ)0.0, (ਦਸ਼ਮਲਵ)0.0);
#ਜੇ_ਅੰਤ
    gtk_container_ਜੋੜੋ(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_ਜੋੜੋ(GTK_CONTAINER(button), alignment);

    ਵਾਪਸ button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ *
convert_localized_ਸੁਨੇਹਾ(char_u **ਬਫਰ, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_2)
{
    ਜੇ (output_conv.vc_type == CONV_NONE)
	ਵਾਪਸ ਸੁਨੇਹਾ_2;

    vim_free(*ਬਫਰ);
    *ਬਫਰ = string_convert(&output_conv, (char_u *)ਸੁਨੇਹਾ_2, ਨਿਰਾ);

    ਵਾਪਸ (ਅਚਲ ਅੱਖਰ *)*ਬਫਰ;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ
ਇੰਦਰਾਜ_get_ਪਾਠ_ਲੰਮਾਈ(Gtk_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ)
{
    g_return_val_if_fail(ਇੰਦਰਾਜ != ਨਿਰਾ, 0);
    g_return_val_if_fail(GTK_IS_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ) == TRUE, 0);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    ਵਾਪਸ gtk_ਇੰਦਰਾਜ_ਬਫਰ_get_ਲੰਮਾਈ(gtk_ਇੰਦਰਾਜ_get_ਬਫਰ(ਇੰਦਰਾਜ));
#ਜਾਂਜੇ GTK_CHECK_ਸੰਸਕਰਣ(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    ਵਾਪਸ gtk_ਇੰਦਰਾਜ_get_ਪਾਠ_ਲੰਮਾਈ(ਇੰਦਰਾਜ);
#ਨਹੀਂਤਾਂ
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    ਵਾਪਸ g_utf8_strlen(gtk_ਇੰਦਰਾਜ_get_ਪਾਠ(ਇੰਦਰਾਜ), -1);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_dialog_create(char_u *arg, ਪੂਰਨ do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*ਸਾਰਣੀ;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    Shared_ਲੱਭੋ_Replace_2 *frdp;
    char_u	*ਇੰਦਰਾਜ_ਪਾਠ;
    ਪੂਰਨ		wword = FALSE;
    ਪੂਰਨ		mcase = !p_ic;
    char_u	*conv_ਬਫਰ = ਨਿਰਾ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV(ਸੁਨੇਹਾ_2) convert_localized_ਸੁਨੇਹਾ(&conv_ਬਫਰ, (ਸੁਨੇਹਾ_2))

    frdp = (do_replace) ? (&repl_widgets) : (&ਲੱਭੋ_widgets);

    // Get the search string to use.
    ਇੰਦਰਾਜ_ਪਾਠ = get_ਲੱਭੋ_dialog_ਪਾਠ(arg, &wword, &mcase);

    ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_ਪਾਠ = ਇੰਦਰਾਜ_ਪਾਠ;
	ਇੰਦਰਾਜ_ਪਾਠ = string_convert(&output_conv, ਇੰਦਰਾਜ_ਪਾਠ, ਨਿਰਾ);
	vim_free(old_ਪਾਠ);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    ਜੇ (frdp->dialog)
    {
	ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ)
	{
	    gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_ਪਾਠ(GTK_ਇੰਦਰਾਜ(frdp->what), (ਅੱਖਰ *)ਇੰਦਰਾਜ_ਪਾਠ);
	    gtk_toggle_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(frdp->wword),
							     (gboolean)wword);
	    gtk_toggle_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
	}
	gtk_ਖਿੜਕੀ_present(GTK_ਖਿੜਕੀ(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	ਜੇ (do_replace && ਇੰਦਰਾਜ_get_ਪਾਠ_ਲੰਮਾਈ(GTK_ਇੰਦਰਾਜ(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(ਇੰਦਰਾਜ_ਪਾਠ);
	ਵਾਪਸ;
    }

    frdp->dialog = gtk_dialog_new();
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#ਨਹੀਂਤਾਂ
    gtk_dialog_ਸੈੱਟ_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#ਜੇ_ਅੰਤ
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_transient_for(GTK_ਖਿੜਕੀ(frdp->dialog), GTK_ਖਿੜਕੀ(gui.mainwin));
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_destroy_with_ਮਾਪਾ(GTK_ਖਿੜਕੀ(frdp->dialog), TRUE);

    ਜੇ (do_replace)
    {
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(GTK_ਖਿੜਕੀ(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    ਨਹੀਂਤਾਂ
    {
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(GTK_ਖਿੜਕੀ(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(hbox), FALSE);
#ਨਹੀਂਤਾਂ
    hbox = gtk_hbox_new(FALSE, 0);
#ਜੇ_ਅੰਤ
    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(hbox), 10);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    {
	GtkWidget * ਅਚਲ dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_ਜੋੜੋ(GTK_CONTAINER(dialog_vbox), hbox);
    }
#ਨਹੀਂਤਾਂ
    gtk_container_ਜੋੜੋ(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#ਜੇ_ਅੰਤ

    ਜੇ (do_replace)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	ਸਾਰਣੀ = gtk_grid_new();
#ਨਹੀਂਤਾਂ
	ਸਾਰਣੀ = gtk_ਸਾਰਣੀ_new(1024, 4, FALSE);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	ਸਾਰਣੀ = gtk_grid_new();
#ਨਹੀਂਤਾਂ
	ਸਾਰਣੀ = gtk_ਸਾਰਣੀ_new(1024, 3, FALSE);
#ਜੇ_ਅੰਤ
    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(hbox), ਸਾਰਣੀ, TRUE, TRUE, 0);
    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(ਸਾਰਣੀ), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,16,0)
    gtk_label_ਸੈੱਟ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_ਸੈੱਟ_yalign(GTK_LABEL(tmp), 0.5);
#ਜਾਂਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    {
	GValue align_val = G_ਮੁੱਲ_INIT;

	g_ਮੁੱਲ_init(&align_val, G_TYPE_FLOAT);

	g_ਮੁੱਲ_ਸੈੱਟ_float(&align_val, 0.0);
	g_object_ਸੈੱਟ_property(G_OBJECT(tmp), "xalign", &align_val);

	g_ਮੁੱਲ_ਸੈੱਟ_float(&align_val, 0.5);
	g_object_ਸੈੱਟ_property(G_OBJECT(tmp), "yalign", &align_val);

	g_ਮੁੱਲ_unset(&align_val);
    }
#ਨਹੀਂਤਾਂ
    gtk_misc_ਸੈੱਟ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#ਜੇ_ਅੰਤ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), tmp, 0, 0, 2, 1);
#ਨਹੀਂਤਾਂ
    gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ
    frdp->what = gtk_ਇੰਦਰਾਜ_new();
    sensitive = (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ && ਇੰਦਰਾਜ_ਪਾਠ[0] != NUL);
    ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ)
	gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_ਪਾਠ(GTK_ਇੰਦਰਾਜ(frdp->what), (ਅੱਖਰ *)ਇੰਦਰਾਜ_ਪਾਠ);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(ਇੰਦਰਾਜ_ਬਦਲਿਆ_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(ਲੱਭੋ_ਕੁੰਜੀ_press_event),
			   (gpointer) frdp);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), frdp->what, 2, 0, 5, 1);
#ਨਹੀਂਤਾਂ
    gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ

    ਜੇ (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,16,0)
	gtk_label_ਸੈੱਟ_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_ਸੈੱਟ_yalign(GTK_LABEL(tmp), 0.5);
#ਜਾਂਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
	{
	    GValue align_val = G_ਮੁੱਲ_INIT;

	    g_ਮੁੱਲ_init(&align_val, G_TYPE_FLOAT);

	    g_ਮੁੱਲ_ਸੈੱਟ_float(&align_val, 0.0);
	    g_object_ਸੈੱਟ_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_ਮੁੱਲ_ਸੈੱਟ_float(&align_val, 0.5);
	    g_object_ਸੈੱਟ_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_ਮੁੱਲ_unset(&align_val);
	}
#ਨਹੀਂਤਾਂ
	gtk_misc_ਸੈੱਟ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#ਜੇ_ਅੰਤ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), tmp, 0, 1, 2, 1);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ
	frdp->with = gtk_ਇੰਦਰਾਜ_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(ਲੱਭੋ_replace_cb),
			 GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(ਲੱਭੋ_ਕੁੰਜੀ_press_event),
			       (gpointer) frdp);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), frdp->with, 2, 1, 5, 1);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), frdp->with, 1, 1024, 1, 2,
			 GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ

	/*
	 * Make the entry activation only change the input focus onto the
	 * with item.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(ਇੰਦਰਾਜ_activate_cb), frdp->with);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Make the entry activation do the search.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(ਲੱਭੋ_replace_cb),
			 GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_FINDNEXT));
    }

    // whole word only button
    frdp->wword = gtk_check_button_new_with_label(CONV(_("Match whole word only")));
    gtk_toggle_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(frdp->wword),
							(gboolean)wword);
    ਜੇ (do_replace)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), frdp->wword, 0, 2, 5, 1);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), frdp->wword, 0, 3, 5, 1);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    ਜੇ (do_replace)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), frdp->mcase, 0, 3, 5, 1);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), frdp->mcase, 0, 4, 5, 1);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ਜੇ_ਅੰਤ

    tmp = gtk_ਚੌਖਟਾ_new(CONV(_("Direction")));
    ਜੇ (do_replace)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), tmp, 5, 2, 2, 4);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	gtk_grid_attach(GTK_GRID(ਸਾਰਣੀ), tmp, 5, 2, 1, 3);
#ਨਹੀਂਤਾਂ
	gtk_ਸਾਰਣੀ_attach(GTK_ਸਾਰਣੀ(ਸਾਰਣੀ), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#ਜੇ_ਅੰਤ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(vbox), FALSE);
#ਨਹੀਂਤਾਂ
    vbox = gtk_vbox_new(FALSE, 0);
#ਜੇ_ਅੰਤ
    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(vbox), 0);
    gtk_container_ਜੋੜੋ(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->ਉੱਪਰ = gtk_radio_button_new_with_label(ਨਿਰਾ, CONV(_("Up")));
    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(vbox), frdp->ਉੱਪਰ, TRUE, TRUE, 0);
    frdp->ਥੱਲੇ = gtk_radio_button_new_with_label(
			gtk_radio_button_get_ਸਮੂਹ(GTK_RADIO_BUTTON(frdp->ਉੱਪਰ)),
			CONV(_("Down")));
    gtk_toggle_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(frdp->ਥੱਲੇ), TRUE);
    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(vbox), frdp->ਥੱਲੇ, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#ਨਹੀਂਤਾਂ
    actionarea = gtk_vbutton_box_new();
#ਜੇ_ਅੰਤ
    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_ਸਿਰਾ(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    frdp->ਲੱਭੋ = create_image_button(ਨਿਰਾ, _("Find Next"));
#ਨਹੀਂਤਾਂ
    frdp->ਲੱਭੋ = create_image_button(GTK_STOCK_ਲੱਭੋ, _("Find Next"));
#ਜੇ_ਅੰਤ
    gtk_widget_ਸੈੱਟ_sensitive(frdp->ਲੱਭੋ, sensitive);

    g_signal_connect(G_OBJECT(frdp->ਲੱਭੋ), "clicked",
		     G_CALLBACK(ਲੱਭੋ_replace_cb),
		     (do_replace) ? GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_R_FINDNEXT)
				  : GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_FINDNEXT));

    gtk_widget_ਸੈੱਟ_can_default(frdp->ਲੱਭੋ, TRUE);
    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(actionarea), frdp->ਲੱਭੋ, FALSE, FALSE, 0);
    gtk_widget_grab_default(frdp->ਲੱਭੋ);

    ਜੇ (do_replace)
    {
	// 'Replace' button
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
	frdp->replace = create_image_button(ਨਿਰਾ, _("Replace"));
#ਨਹੀਂਤਾਂ
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#ਜੇ_ਅੰਤ
	gtk_widget_ਸੈੱਟ_sensitive(frdp->replace, sensitive);
	gtk_widget_ਸੈੱਟ_can_default(frdp->ਲੱਭੋ, TRUE);
	gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(ਲੱਭੋ_replace_cb),
			 GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_REPLACE));

	// 'Replace All' button
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
	frdp->all = create_image_button(ਨਿਰਾ, _("Replace All"));
#ਨਹੀਂਤਾਂ
	frdp->all = create_image_button(GTK_STOCK_CONVERT, _("Replace All"));
#ਜੇ_ਅੰਤ
	gtk_widget_ਸੈੱਟ_sensitive(frdp->all, sensitive);
	gtk_widget_ਸੈੱਟ_can_default(frdp->all, TRUE);
	gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(actionarea), frdp->all, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->all), "clicked",
			 G_CALLBACK(ਲੱਭੋ_replace_cb),
			 GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_REPLACEALL));
    }

    // 'Cancel' button
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#ਨਹੀਂਤਾਂ
    tmp = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#ਜੇ_ਅੰਤ
    gtk_widget_ਸੈੱਟ_can_default(tmp, TRUE);
    gtk_box_pack_ਸਿਰਾ(GTK_BOX(actionarea), tmp, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(tmp),
			     "clicked", G_CALLBACK(gtk_widget_hide),
			     G_OBJECT(frdp->dialog));
    g_signal_connect_swapped(G_OBJECT(frdp->dialog),
			     "delete-event", G_CALLBACK(gtk_widget_hide_on_delete),
			     G_OBJECT(frdp->dialog));

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#ਨਹੀਂਤਾਂ
    tmp = gtk_vseparator_new();
#ਜੇ_ਅੰਤ
    gtk_box_pack_ਸਿਰਾ(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#ਨਹੀਂਤਾਂ
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#ਜੇ_ਅੰਤ
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(ਇੰਦਰਾਜ_ਪਾਠ);
    vim_free(conv_ਬਫਰ);
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CONV
}

    ਖਾਲੀ
gui_mch_ਲੱਭੋ_dialog(exarg_T *eap)
{
    ਜੇ (gui.ਵਿੱਚ_use)
	ਲੱਭੋ_replace_dialog_create(eap->arg, FALSE);
}

    ਖਾਲੀ
gui_mch_replace_dialog(exarg_T *eap)
{
    ਜੇ (gui.ਵਿੱਚ_use)
	ਲੱਭੋ_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    ਪੂਰਨ			flags;
    char_u		*ਲੱਭੋ_ਪਾਠ;
    char_u		*repl_ਪਾਠ;
    gboolean		direction_ਥੱਲੇ;
    Shared_ਲੱਭੋ_Replace_2	*sfr;

    flags = (ਪੂਰਨ)(ਲੰਮਾ)ਅੰਕੜੇ;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    ਜੇ (flags == FRD_FINDNEXT)
    {
	repl_ਪਾਠ = ਨਿਰਾ;
	sfr = &ਲੱਭੋ_widgets;
    }
    ਨਹੀਂਤਾਂ
    {
	repl_ਪਾਠ = (char_u *)gtk_ਇੰਦਰਾਜ_get_ਪਾਠ(GTK_ਇੰਦਰਾਜ(repl_widgets.with));
	sfr = &repl_widgets;
    }

    ਲੱਭੋ_ਪਾਠ = (char_u *)gtk_ਇੰਦਰਾਜ_get_ਪਾਠ(GTK_ਇੰਦਰਾਜ(sfr->what));
    direction_ਥੱਲੇ = gtk_toggle_button_get_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(sfr->ਥੱਲੇ));

    ਜੇ (gtk_toggle_button_get_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_ਸਾਰਾ_ਸ਼ਬਦ;
    ਜੇ (gtk_toggle_button_get_ਸਰਗਰਮ(GTK_TOGGLE_BUTTON(sfr->mcase)))
	flags |= FRD_ਮੇਲ_CASE;

    repl_ਪਾਠ = CONVERT_FROM_UTF8(repl_ਪਾਠ);
    ਲੱਭੋ_ਪਾਠ = CONVERT_FROM_UTF8(ਲੱਭੋ_ਪਾਠ);
    gui_do_findrepl(flags, ਲੱਭੋ_ਪਾਠ, repl_ਪਾਠ, direction_ਥੱਲੇ);
    CONVERT_FROM_UTF8_FREE(repl_ਪਾਠ);
    CONVERT_FROM_UTF8_FREE(ਲੱਭੋ_ਪਾਠ);
}

/*
 * our usual callback function
 */
    ਸਥਿਰ ਖਾਲੀ
ਇੰਦਰਾਜ_activate_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    gtk_widget_grab_focus(GTK_WIDGET(ਅੰਕੜੇ));
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
    ਸਥਿਰ ਖਾਲੀ
ਇੰਦਰਾਜ_ਬਦਲਿਆ_cb(GtkWidget * ਇੰਦਰਾਜ, GtkWidget * dialog)
{
    ਅਚਲ gchar	*ਇੰਦਰਾਜ_ਪਾਠ;
    gboolean	nonempty;

    ਇੰਦਰਾਜ_ਪਾਠ = gtk_ਇੰਦਰਾਜ_get_ਪਾਠ(GTK_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ));

    ਜੇ (!ਇੰਦਰਾਜ_ਪਾਠ)
	ਵਾਪਸ;			// shouldn't happen

    nonempty = (ਇੰਦਰਾਜ_ਪਾਠ[0] != '\0');

    ਜੇ (dialog == ਲੱਭੋ_widgets.dialog)
	gtk_widget_ਸੈੱਟ_sensitive(ਲੱਭੋ_widgets.ਲੱਭੋ, nonempty);

    ਜੇ (dialog == repl_widgets.dialog)
    {
	gtk_widget_ਸੈੱਟ_sensitive(repl_widgets.ਲੱਭੋ, nonempty);
	gtk_widget_ਸੈੱਟ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_ਸੈੱਟ_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    ਖਾਲੀ
ex_helpfind(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#ਜੇ defined(FEAT_BROWSE)
    ਸਥਿਰ ਖਾਲੀ
recent_func_log_func(ਅਚਲ gchar *log_domain ਅਣਵਰਤਿਆ,
		     GLogLevelFlags log_level ਅਣਵਰਤਿਆ,
		     ਅਚਲ gchar *ਸੁਨੇਹਾ_2 ਅਣਵਰਤਿਆ,
		     gpointer user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fullscreen(ਪੂਰਨ flag)
{
    ਜੇ (flag)
	gtk_ਖਿੜਕੀ_fullscreen(GTK_ਖਿੜਕੀ(gui.mainwin));
    ਨਹੀਂਤਾਂ
	gtk_ਖਿੜਕੀ_unfullscreen(GTK_ਖਿੜਕੀ(gui.mainwin));
}
