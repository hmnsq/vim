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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
# include "gui_gtk_f.h"
#ಆದರೆ_ಅಂತ್ಯ

// GTK defines MAX and MIN, but some system header files as well.  Undefine
// them and don't use them.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MIN
# undef MIN
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MAX
# undef MAX
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
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
# ಆದರೆ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#ಇಲ್ಲದಿದ್ದರೆ
// define these items to be able to generate prototypes without GTK
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ GtkWidget;
# define gpointer ಪೂರ್ಣಾಂಕ
# define guint8 ಪೂರ್ಣಾಂಕ
# define GdkPixmap ಪೂರ್ಣಾಂಕ
# define GdkBitmap ಪೂರ್ಣಾಂಕ
# define GtkIconFactory ಪೂರ್ಣಾಂಕ
# define GtkToolbar ಪೂರ್ಣಾಂಕ
# define GtkAdjustment ಪೂರ್ಣಾಂಕ
# define gboolean ಪೂರ್ಣಾಂಕ
# define Gdk_Event_ಕೀಲಿ ಪೂರ್ಣಾಂಕ
# define Cancel_ದತ್ತಾಂಶ ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ ನಮೂದು_activate_cb(GtkWidget *widget, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ ನಮೂದು_ಬದಲಾಯಿತು_cb(GtkWidget *ನಮೂದು, GtkWidget *dialog);
ಸ್ಥಿರ ಶೂನ್ಯ ಕಂಡುಹಿಡಿ_replace_cb(GtkWidget *widget, gpointer ದತ್ತಾಂಶ);
#ಆದರೆ defined(FEAT_BROWSE)
ಸ್ಥಿರ ಶೂನ್ಯ recent_func_log_func(
	ಸ್ಥಿರಾಂಕ gchar *log_domain,
	GLogLevelFlags log_level,
	ಸ್ಥಿರಾಂಕ gchar *ಸಂದೇಶ_2,
	gpointer user_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ menu_themed_ಹೆಸರುಗಳು[] =
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
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  ಇಲ್ಲದಿದ್ದರೆ
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  ಇಲ್ಲದಿದ್ದರೆ
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  ಇಲ್ಲದಿದ್ದರೆ
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  ಇಲ್ಲದಿದ್ದರೆ
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,10,0)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ menu_stock_ids[] =
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
    /* 10 */ GTK_STOCK_ಕಂಡುಹಿಡಿ,
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
    /* 15 */ GTK_STOCK_EXECUTE,
    /* 16 */ GTK_STOCK_ಕಂಡುಹಿಡಿ_AND_REPLACE,
    /* 17 */ GTK_STOCK_CLOSE,		// FIXME: fuzzy
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
    /* 22 */ GTK_STOCK_GO_ಹಿಂಭಾಗ,
    /* 23 */ GTK_STOCK_GO_FORWARD,
    /* 24 */ "vim-find-help",
    /* 25 */ GTK_STOCK_CONVERT,
    /* 26 */ GTK_STOCK_JUMP_ಗಮ್ಯ,
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
    /* 31 */ GTK_STOCK_QUIT
};
# endif // !GTK_CHECK_VERSION(3,10,0)

# ifdef USE_GRESOURCE
#  ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Icon_ಹೆಸರುಗಳು {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *icon_name;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file_name;
} Icon_ಹೆಸರುಗಳು;

ಸ್ಥಿರ Icon_ಹೆಸರುಗಳು stock_vim_icons[] = {
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
    { ಖಾಲಿ, ಖಾಲಿ }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_stock_icon(GtkIconFactory	*factory,
	       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*stock_id,
	       ಸ್ಥಿರಾಂಕ guint8	*inline_ದತ್ತಾಂಶ,
	       ಪೂರ್ಣಾಂಕ		ದತ್ತಾಂಶ_ಉದ್ದಳತೆ)
{
    GdkPixbuf	*pixbuf;
    Gtk_Icon_ಗಣ	*icon_ಗಣ;

    pixbuf = gdk_pixbuf_new_from_inline(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, inline_ದತ್ತಾಂಶ, FALSE, ಖಾಲಿ);
    icon_ಗಣ = gtk_icon_ಗಣ_new_from_pixbuf(pixbuf);

    gtk_icon_factory_ಕೂಡಿಸು(factory, stock_id, icon_ಗಣ);

    gtk_icon_ಗಣ_unref(icon_ಗಣ);
    g_object_unref(pixbuf);
}
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    ಆದರೆ (mch_is_ತುಂಬಿದ_Name(dest))
    {
	ಹಿಂತಿರುಗಿ vim_fexists(dest);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	    i;

	ಗಾಗಿ (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    ಆದರೆ (gui_ಕಂಡುಹಿಡಿ_bitmap(dest, buf, (ಅಕ್ಷರ *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		ಹಿಂತಿರುಗಿ TRUE;
	    }

	ಹಿಂತಿರುಗಿ FALSE;
    }
}

    ಸ್ಥಿರ GtkWidget *
ತುಂಬು_menu_iconfile(char_u *name, Gtk_Icon_ಗಾತ್ರ icon_ಗಾತ್ರ)
{
    GtkWidget	    *image = ಖಾಲಿ;
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ಪೂರ್ಣಾಂಕ		     pixel_ಗಾತ್ರ = -1;

    ಆಯ್ಕೆ (icon_ಗಾತ್ರ)
    {
	ಸ್ಥಿತಿ GTK_ICON_ಗಾತ್ರ_MENU:
	    pixel_ಗಾತ್ರ = 16;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GTK_ICON_ಗಾತ್ರ_SMALL_TOOLBAR:
	    pixel_ಗಾತ್ರ = 16;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GTK_ICON_ಗಾತ್ರ_LARGE_TOOLBAR:
	    pixel_ಗಾತ್ರ = 24;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GTK_ICON_ಗಾತ್ರ_BUTTON:
	    pixel_ಗಾತ್ರ = 16;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GTK_ICON_ಗಾತ್ರ_DND:
	    pixel_ಗಾತ್ರ = 32;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GTK_ICON_ಗಾತ್ರ_DIALOG:
	    pixel_ಗಾತ್ರ = 48;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GTK_ICON_ಗಾತ್ರ_INVALID:
	    // FALLTHROUGH
	ಪೂರ್ವನಿಯೋಜಿತ:
	    pixel_ಗಾತ್ರ = 0;
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (pixel_ಗಾತ್ರ > 0 || pixel_ಗಾತ್ರ == -1)
    {
	GdkPixbuf * ಸ್ಥಿರಾಂಕ pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name,
		pixel_ಗಾತ್ರ, pixel_ಗಾತ್ರ, TRUE, ಖಾಲಿ);
	ಆದರೆ (pixbuf != ಖಾಲಿ)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    ಆದರೆ (image == ಖಾಲಿ)
	image = gtk_image_new_from_icon_name("image-missing", icon_ಗಾತ್ರ);

    ಹಿಂತಿರುಗಿ image;
# ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,10,0)
    Gtk_Icon_ಗಣ	    *icon_ಗಣ;
    Gtk_Icon_ಮೂಲ   *icon_ಮೂಲ;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_ಗಣ = gtk_icon_ಗಣ_new();
    icon_ಮೂಲ = gtk_icon_ಮೂಲ_new();

    gtk_icon_ಮೂಲ_ಗಣ_filename(icon_ಮೂಲ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name);
    gtk_icon_ಗಣ_ಕೂಡಿಸು_ಮೂಲ(icon_ಗಣ, icon_ಮೂಲ);

    image = gtk_image_new_from_icon_ಗಣ(icon_ಗಣ, icon_ಗಾತ್ರ);

    gtk_icon_ಮೂಲ_free(icon_ಮೂಲ);
    gtk_icon_ಗಣ_unref(icon_ಗಣ);

    ಹಿಂತಿರುಗಿ image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    ಸ್ಥಿರ GtkWidget *
create_menu_icon(vimmenu_T *menu, Gtk_Icon_ಗಾತ್ರ icon_ಗಾತ್ರ)
{
    GtkWidget	*image = ಖಾಲಿ;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    ಆದರೆ (menu->iconfile != ಖಾಲಿ && lookup_menu_iconfile(menu->iconfile, buf))
	image = ತುಂಬು_menu_iconfile(buf, icon_ಗಾತ್ರ);

    // If not found and not builtin specified try using the menu name.
    ಆದರೆ (image == ಖಾಲಿ && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = ತುಂಬು_menu_iconfile(buf, icon_ಗಾತ್ರ);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    ಆದರೆ (image == ಖಾಲಿ)
    {
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *icon_name = ಖಾಲಿ;
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ   n_ಹೆಸರುಗಳು = G_N_ELEMENTS(menu_themed_ಹೆಸರುಗಳು);

	ಆದರೆ (menu->iconidx >= 0 && menu->iconidx < n_ಹೆಸರುಗಳು)
	    icon_name = menu_themed_ಹೆಸರುಗಳು[menu->iconidx];
	ಆದರೆ (icon_name == ಖಾಲಿ)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_ಗಾತ್ರ);
# ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *stock_id;
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ   n_ids = G_N_ELEMENTS(menu_stock_ids);

	ಆದರೆ (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	ಇಲ್ಲದಿದ್ದರೆ
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_ಗಾತ್ರ);
# endif
    }

    ಹಿಂತಿರುಗಿ image;
}

    ಸ್ಥಿರ gint
toolbar_button_focus_ಒಳ_event(GtkWidget *widget ಬಳಸದ,
			      GdkEventFocus *event ಬಳಸದ,
			      gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    ಆದರೆ (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR

#ಆದರೆ defined(FEAT_TOOLBAR)

    ಶೂನ್ಯ
gui_gtk_ರಿಜಿಸ್ಟರ್_stock_icons(ಶೂನ್ಯ)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define ಕೂಡಿಸು_ICON(Name, ದತ್ತಾಂಶ_2) ಕೂಡಿಸು_stock_icon(factory, Name, ದತ್ತಾಂಶ_2, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ದತ್ತಾಂಶ_2))

    ಕೂಡಿಸು_ICON("vim-build-tags",		  stock_vim_build_ಟ್ಯಾಗುಗಳು);
    ಕೂಡಿಸು_ICON("vim-find-help",		  stock_vim_ಕಂಡುಹಿಡಿ_help);
    ಕೂಡಿಸು_ICON("vim-save-all",		  stock_vim_save_all);
    ಕೂಡಿಸು_ICON("vim-session-load",	  stock_vim_session_ತುಂಬು);
    ಕೂಡಿಸು_ICON("vim-session-new",		  stock_vim_session_new);
    ಕೂಡಿಸು_ICON("vim-session-save",	  stock_vim_session_save);
    ಕೂಡಿಸು_ICON("vim-shell",		  stock_vim_shell);
    ಕೂಡಿಸು_ICON("vim-window-maximize",	  stock_vim_ಕಿಟಕಿ_maximize);
    ಕೂಡಿಸು_ICON("vim-window-maximize-width", stock_vim_ಕಿಟಕಿ_maximize_ಅಗಲ);
    ಕೂಡಿಸು_ICON("vim-window-minimize",	  stock_vim_ಕಿಟಕಿ_minimize);
    ಕೂಡಿಸು_ICON("vim-window-minimize-width", stock_vim_ಕಿಟಕಿ_minimize_ಅಗಲ);
    ಕೂಡಿಸು_ICON("vim-window-split",	  stock_vim_ಕಿಟಕಿ_ವಿಭಜಿಸು);
    ಕೂಡಿಸು_ICON("vim-window-split-vertical", stock_vim_ಕಿಟಕಿ_ವಿಭಜಿಸು_vertical);

#  undef ಕೂಡಿಸು_ICON

    gtk_icon_factory_ಕೂಡಿಸು_default(factory);
    g_object_unref(factory);
# ಇಲ್ಲದಿದ್ದರೆ // defined(USE_GRESOURCE)
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ = "/org/vim/gui/icon";
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    GdkScreen    *screen = ಖಾಲಿ;
    GtkIconTheme *icon_theme = ಖಾಲಿ;

    ಆದರೆ (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    ಇಲ್ಲದಿದ್ದರೆ
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_ಕೂಡಿಸು_resource_ಮಾರ್ಗ(icon_theme, ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ);
#  elif GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    Icon_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು;

    ಗಾಗಿ (ಹೆಸರುಗಳು = stock_vim_icons; ಹೆಸರುಗಳು->icon_name != ಖಾಲಿ; ಹೆಸರುಗಳು++)
    {
	ಅಕ್ಷರ ಮಾರ್ಗ[MAXPATHL];

	vim_snprintf(ಮಾರ್ಗ, MAXPATHL, "%s/%s", ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ, ಹೆಸರುಗಳು->file_name);
	GdkPixbuf *pixbuf = ಖಾಲಿ;
	pixbuf = gdk_pixbuf_new_from_resource(ಮಾರ್ಗ, ಖಾಲಿ);
	ಆದರೆ (pixbuf != ಖಾಲಿ)
	{
	    ಸ್ಥಿರಾಂಕ gint ಗಾತ್ರ_2 = MAX(gdk_pixbuf_get_ಅಗಲ(pixbuf),
				  gdk_pixbuf_get_ಎತ್ತರ(pixbuf));
	    ಆದರೆ (ಗಾತ್ರ_2 > 16)
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
		ಆದರೆ (pixbuf == ಖಾಲಿ)
		    pixbuf = src;
		ಇಲ್ಲದಿದ್ದರೆ
		    g_object_unref(src);
	    }
	    gtk_icon_theme_ಕೂಡಿಸು_builtin_icon(ಹೆಸರುಗಳು->icon_name, ಗಾತ್ರ_2, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * ಸ್ಥಿರಾಂಕ factory = gtk_icon_factory_new();
    Icon_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು;

    ಗಾಗಿ (ಹೆಸರುಗಳು = stock_vim_icons; ಹೆಸರುಗಳು->icon_name != ಖಾಲಿ; ಹೆಸರುಗಳು++)
    {
	ಅಕ್ಷರ ಮಾರ್ಗ[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(ಮಾರ್ಗ, MAXPATHL, "%s/%s", ಮಾರ್ಗ_ಪೂರ್ವಪ್ರತ್ಯಯ, ಹೆಸರುಗಳು->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(ಮಾರ್ಗ, ಖಾಲಿ);
	ಆದರೆ (pixbuf != ಖಾಲಿ)
	{
	    Gtk_Icon_ಗಣ *icon_ಗಣ = gtk_icon_ಗಣ_new_from_pixbuf(pixbuf);
	    gtk_icon_factory_ಕೂಡಿಸು(factory, ಹೆಸರುಗಳು->icon_name, icon_ಗಣ);
	    gtk_icon_ಗಣ_unref(icon_ಗಣ);
	    g_object_unref(pixbuf);
	}
    }

    gtk_icon_factory_ಕೂಡಿಸು_default(factory);
    g_object_unref(factory);
#  endif // !GTK_CHECK_VERSION(3,0,0)
# endif // defined(USE_GRESOURCE)
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR

#ಆದರೆ defined(FEAT_MENU)

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
translate_mnemonic_ಟ್ಯಾಗ್(char_u *name, ಪೂರ್ಣಾಂಕ use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    ಪೂರ್ಣಾಂಕ	    n_underscores = 0;

    name = CONVERT_ಗಮ್ಯ_UTF8(name);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ಆದರೆ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	pdest = buf;
	ಗಾಗಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ಆದರೆ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_ಗಮ್ಯ_UTF8_FREE(name);
    ಹಿಂತಿರುಗಿ buf;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_ಅಂಶ_new(vimmenu_T *menu, GtkWidget *ಪೋಷಕ_widget)
{
    GtkWidget	*box;
    char_u	*ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_ಅಂಶ_new();
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_ಗಣ_homogeneous(GTK_BOX(box), FALSE);
# ಇಲ್ಲದಿದ್ದರೆ
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(ಪೋಷಕ_widget));
    ಪಠ್ಯ = translate_mnemonic_ಟ್ಯಾಗ್(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ);
    vim_free(ಪಠ್ಯ);

    gtk_box_pack_ಆರಂಭ(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    ಆದರೆ (menu->actext != ಖಾಲಿ && menu->actext[0] != NUL)
    {
	ಪಠ್ಯ = CONVERT_ಗಮ್ಯ_UTF8(menu->actext);

	gtk_box_pack_ಕೊನೆ(GTK_BOX(box),
			 gtk_label_new((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ),
			 FALSE, FALSE, 0);

	CONVERT_ಗಮ್ಯ_UTF8_FREE(ಪಠ್ಯ);
    }

    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ idx)
{
    vimmenu_T	*ಪೋಷಕ;
    GtkWidget	*ಪೋಷಕ_widget;

    ಆದರೆ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	ಹಿಂತಿರುಗಿ;
    }

    ಪೋಷಕ = menu->ಪೋಷಕ;

    ಆದರೆ ((ಪೋಷಕ != ಖಾಲಿ && ಪೋಷಕ->submenu_id == ಖಾಲಿ)
	    || !menu_is_menubar(menu->name))
	ಹಿಂತಿರುಗಿ;

    ಪೋಷಕ_widget = (ಪೋಷಕ != ಖಾಲಿ) ? ಪೋಷಕ->submenu_id : gui.menubar;
    menu_ಅಂಶ_new(menu, ಪೋಷಕ_widget);

# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    // since the tearoff should always appear first, increment idx
    ಆದರೆ (ಪೋಷಕ != ಖಾಲಿ && !menu_is_popup(ಪೋಷಕ->name))
	++idx;
# endif

    gtk_menu_shell_ಸೇರಿಸು(GTK_MENU_SHELL(ಪೋಷಕ_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_ಗಣ_accel_ಗುಂಪು(GTK_MENU(menu->submenu_id), gui.accel_ಗುಂಪು);
    gtk_menu_ಅಂಶ_ಗಣ_submenu(GTK_MENU_ಅಂಶ(menu->id), menu->submenu_id);

# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_ಅಂಶ_new();
    ಆದರೆ (vim_strchr(p_go, GO_TEAROFF) != ಖಾಲಿ)
	gtk_widget_show(menu->tearoff_handle);
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  ಇಲ್ಲದಿದ್ದರೆ
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_ಅಂಶ_activate(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    gui_menu_cb((vimmenu_T *)ದತ್ತಾಂಶ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_ಅಂಶ_select(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_ಸಂದೇಶ = FALSE;

    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	ಹಿಂತಿರುಗಿ;
    menu = (vimmenu_T *)ದತ್ತಾಂಶ;
    tooltip = CONVERT_ಗಮ್ಯ_UTF8(menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP]);
    ಆದರೆ (tooltip != ಖಾಲಿ && utf_valid_string(tooltip, ಖಾಲಿ))
    {
	ಸಂದೇಶ((ಅಕ್ಷರ *)tooltip);
	did_ಸಂದೇಶ = TRUE;
	setcursor();
	ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_ಸಂದೇಶ)
    {
	ಸಂದೇಶ("");
	did_ಸಂದೇಶ = FALSE;
	setcursor();
	ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
    }
    CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);
}

    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ idx)
{
    vimmenu_T *ಪೋಷಕ;

    ಪೋಷಕ = menu->ಪೋಷಕ;

# ifdef FEAT_TOOLBAR
    ಆದರೆ (menu_is_toolbar(ಪೋಷಕ->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = ಖಾಲಿ;

	ಆದರೆ (menu_is_separator(menu->name))
	{
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	    Gtk_Tool_ಅಂಶ *ಅಂಶ = gtk_separator_tool_ಅಂಶ_new();
	    gtk_separator_tool_ಅಂಶ_ಗಣ_draw(GTK_SEPARATOR_TOOL_ಅಂಶ(ಅಂಶ),
		    TRUE);
	    gtk_tool_ಅಂಶ_ಗಣ_expand(GTK_TOOL_ಅಂಶ(ಅಂಶ), FALSE);
	    gtk_widget_show(GTK_WIDGET(ಅಂಶ));

	    gtk_toolbar_ಸೇರಿಸು(toolbar, ಅಂಶ, idx);
#  ಇಲ್ಲದಿದ್ದರೆ
	    gtk_toolbar_ಸೇರಿಸು_space(toolbar, idx);
#  endif
	    menu->id = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *ಪಠ್ಯ;
	    char_u *tooltip;

	    ಪಠ್ಯ    = CONVERT_ಗಮ್ಯ_UTF8(menu->dname);
	    tooltip = CONVERT_ಗಮ್ಯ_UTF8(menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP]);
	    ಆದರೆ (tooltip != ಖಾಲಿ && !utf_valid_string(tooltip, ಖಾಲಿ))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);

#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	    {
		GtkWidget *icon;
		Gtk_Tool_ಅಂಶ *ಅಂಶ;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_ಗಾತ್ರ(toolbar));
		ಅಂಶ = gtk_tool_button_new(icon, (ಸ್ಥಿರಾಂಕ gchar *)ಪಠ್ಯ);
		gtk_tool_ಅಂಶ_ಗಣ_tooltip_ಪಠ್ಯ(ಅಂಶ, (ಸ್ಥಿರಾಂಕ gchar *)tooltip);
		g_signal_connect(G_OBJECT(ಅಂಶ), "clicked",
			G_CALLBACK(&menu_ಅಂಶ_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(ಅಂಶ));

		gtk_toolbar_ಸೇರಿಸು(toolbar, ಅಂಶ, idx);

		menu->id = GTK_WIDGET(ಅಂಶ);
	    }
#  ಇಲ್ಲದಿದ್ದರೆ
	    menu->id = gtk_toolbar_ಸೇರಿಸು_ಅಂಶ(
		    toolbar,
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ,
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)tooltip,
		    ಖಾಲಿ,
		    create_menu_icon(menu, gtk_toolbar_get_icon_ಗಾತ್ರ(toolbar)),
		    G_CALLBACK(&menu_ಅಂಶ_activate),
		    menu,
		    idx);
#  endif

	    ಆದರೆ (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_ಒಳ_event), ಖಾಲಿ);

	    CONVERT_ಗಮ್ಯ_UTF8_FREE(ಪಠ್ಯ);
	    CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	ಆದರೆ (ಪೋಷಕ == ಖಾಲಿ || ಪೋಷಕ->submenu_id == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	ಆದರೆ (!menu_is_popup(ಪೋಷಕ->name))
	    ++idx;
# endif

	ಆದರೆ (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	    menu->id = gtk_separator_menu_ಅಂಶ_new();
# ಇಲ್ಲದಿದ್ದರೆ
	    menu->id = gtk_menu_ಅಂಶ_new();
	    gtk_widget_ಗಣ_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_ಸೇರಿಸು(GTK_MENU_SHELL(ಪೋಷಕ->submenu_id),
		    menu->id, idx);

	    ಹಿಂತಿರುಗಿ;
	}

	// Add textual menu item.
	menu_ಅಂಶ_new(menu, ಪೋಷಕ->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_ಸೇರಿಸು(GTK_MENU_SHELL(ಪೋಷಕ->submenu_id),
		menu->id, idx);

	ಆದರೆ (menu->id != ಖಾಲಿ)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_ಅಂಶ_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_ಅಂಶ_select), menu);
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU


    ಶೂನ್ಯ
gui_mch_ಗಣ_ಪಠ್ಯ_area_pos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#ಆದರೆ defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ಶೂನ್ಯ
gui_gtk_ಗಣ_mnemonics(ಪೂರ್ಣಾಂಕ enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ಆದರೆ (menu->id == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	name = translate_mnemonic_ಟ್ಯಾಗ್(menu->name, enable);
	gtk_label_ಗಣ_ಪಠ್ಯ_with_mnemonic(GTK_LABEL(menu->label),
					 (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name);
	vim_free(name);
    }
}

# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
recurse_tearoffs(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ val)
{
    ಗಾಗಿ (; menu != ಖಾಲಿ; menu = menu->ಮುಂದಿನ)
    {
	ಆದರೆ (menu->submenu_id != ಖಾಲಿ && menu->tearoff_handle != ಖಾಲಿ
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    ಆದರೆ (val)
		gtk_widget_show(menu->tearoff_handle);
	    ಇಲ್ಲದಿದ್ದರೆ
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಂಕ enable ಬಳಸದ)
{
    // Do nothing
}
# ಇಲ್ಲದಿದ್ದರೆ
    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಂಕ enable)
{
    recurse_tearoffs(ಬೇರು_menu, enable);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

#ಆದರೆ defined(FEAT_TOOLBAR)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*ಗಂಟು;
    ಪೂರ್ಣಾಂಕ		idx = 0;

    ಗಾಗಿ (ಗಂಟು = menu->ಪೋಷಕ->children; ಗಂಟು != menu; ಗಂಟು = ಗಂಟು->ಮುಂದಿನ)
    {
	g_return_val_if_fail(ಗಂಟು != ಖಾಲಿ, -1);
	++idx;
    }

    ಹಿಂತಿರುಗಿ idx;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR


#ಆದರೆ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_menu_ಗಣ_tip(vimmenu_T *menu)
{
    ಆದರೆ (menu->id == ಖಾಲಿ || menu->ಪೋಷಕ == ಖಾಲಿ || gui.toolbar == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u *tooltip;

    tooltip = CONVERT_ಗಮ್ಯ_UTF8(menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP]);
    ಆದರೆ (tooltip != ಖಾಲಿ && utf_valid_string(tooltip, ಖಾಲಿ))
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_ಗಣ_tooltip_ಪಠ್ಯ(menu->id, (ಸ್ಥಿರಾಂಕ gchar *)tooltip);
# ಇಲ್ಲದಿದ್ದರೆ
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_ಗಣ_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)tooltip, ಖಾಲಿ);
# endif
    CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR


#ಆದರೆ defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    ಆದರೆ (menu->id != ಖಾಲಿ)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    ಆದರೆ (menu->id != ಖಾಲಿ && gui.menubar != ಖಾಲಿ
			    && gtk_widget_get_ಪೋಷಕ(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    ಆದರೆ (menu->ಪೋಷಕ != ಖಾಲಿ && menu_is_toolbar(menu->ಪೋಷಕ->name))
    {
	ಆದರೆ (menu_is_separator(menu->name))
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	{
	    Gtk_Tool_ಅಂಶ *ಅಂಶ = ಖಾಲಿ;

	    ಅಂಶ = gtk_toolbar_get_nth_ಅಂಶ(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(ಅಂಶ));
	}
#  ಇಲ್ಲದಿದ್ದರೆ
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (menu->id != ಖಾಲಿ)
	    gtk_widget_destroy(menu->id);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif // FEAT_TOOLBAR
    {
	ಆದರೆ (menu->submenu_id != ಖಾಲಿ)
	    gtk_widget_destroy(menu->submenu_id);

	ಆದರೆ (menu->id != ಖಾಲಿ)
	    gtk_widget_destroy(menu->id);
    }

    ಆದರೆ (menu->id != ಖಾಲಿ)
	g_object_unref(menu->id);
    menu->submenu_id = ಖಾಲಿ;
    menu->id = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_thumb(scrollbar_T *sb, ದೀರ್ಘ val, ದೀರ್ಘ ಗಾತ್ರ_2, ದೀರ್ಘ max)
{
    ಆದರೆ (sb->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_ಗಣ_ಅಧೋ(adjustment, 0.0);
    gtk_adjustment_ಗಣ_ಮೌಲ್ಯ(adjustment, val);
    gtk_adjustment_ಗಣ_ಊರ್ಧ್ವ(adjustment, max + 1);
    gtk_adjustment_ಗಣ_ಪುಟ_ಗಾತ್ರ(adjustment, ಗಾತ್ರ_2);
    gtk_adjustment_ಗಣ_ಪುಟ_increment(adjustment,
	    ಗಾತ್ರ_2 < 3L ? 1L : ಗಾತ್ರ_2 - 2L);
    gtk_adjustment_ಗಣ_ಹೆಜ್ಜೆ_increment(adjustment, 1.0);

    g_signal_handler_ಖಂಡ(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,18,0)
    gtk_adjustment_ಬದಲಾಯಿತು(adjustment);
#ಆದರೆ_ಅಂತ್ಯ

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_pos(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    ಆದರೆ (sb->id != ಖಾಲಿ)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ xpad;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    xpad = gtk_widget_get_allocated_ಅಗಲ(gui.formwin)
	  - gtk_widget_get_allocated_ಅಗಲ(gui.drawarea) - gui.scrollbar_ಅಗಲ;
#ಇಲ್ಲದಿದ್ದರೆ
    xpad = gui.formwin->ಹಂಚಿಕೆ.ಅಗಲ - gui.drawarea->ಹಂಚಿಕೆ.ಅಗಲ
							 - gui.scrollbar_ಅಗಲ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ] && gui.which_scrollbars[SBAR_ಬಲ])
	xpad -= gui.scrollbar_ಅಗಲ;

    ಹಿಂತಿರುಗಿ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ypad;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ypad = gtk_widget_get_allocated_ಎತ್ತರ(gui.formwin)
	- gtk_widget_get_allocated_ಎತ್ತರ(gui.drawarea) - gui.scrollbar_ಎತ್ತರ;
#ಇಲ್ಲದಿದ್ದರೆ
    ypad = gui.formwin->ಹಂಚಿಕೆ.ಎತ್ತರ - gui.drawarea->ಹಂಚಿಕೆ.ಎತ್ತರ
							- gui.scrollbar_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
adjustment_ಮೌಲ್ಯ_ಬದಲಾಯಿತು(GtkAdjustment *adjustment, gpointer ದತ್ತಾಂಶ)
{
    scrollbar_T	*sb;
    ದೀರ್ಘ	ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ		dragging = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    // cancel any preediting
    ಆದರೆ (im_is_preediting())
	xim_reset();
#ಆದರೆ_ಅಂತ್ಯ

    sb = gui_ಕಂಡುಹಿಡಿ_scrollbar((ದೀರ್ಘ)ದತ್ತಾಂಶ);
    ಮೌಲ್ಯ = gtk_adjustment_get_ಮೌಲ್ಯ(adjustment);
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    ಆದರೆ (sb != ಖಾಲಿ)
    {
	dragging = TRUE;

	ಆದರೆ (sb->wp != ಖಾಲಿ && GDK_IS_DRAWABLE(sb->id->ಕಿಟಕಿ_2))
	{
	    ಪೂರ್ಣಾಂಕ			x;
	    ಪೂರ್ಣಾಂಕ			y;
	    GdkModifierType	state;
	    ಪೂರ್ಣಾಂಕ			ಅಗಲ;
	    ಪೂರ್ಣಾಂಕ			ಎತ್ತರ;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_ಕಿಟಕಿ_get_ಸೂಚಕ(sb->id->ಕಿಟಕಿ_2, &x, &y, &state);
	    gdk_ಕಿಟಕಿ_get_ಗಾತ್ರ(sb->id->ಕಿಟಕಿ_2, &ಅಗಲ, &ಎತ್ತರ);
	    ಆದರೆ (x >= 0 && x < ಅಗಲ && y >= 0 && y < ಎತ್ತರ)
	    {
		ಆದರೆ (y < ಅಗಲ)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    ಮೌಲ್ಯ = sb->wp->w_topline - 2;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (y > ಎತ್ತರ - ಅಗಲ)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    ಮೌಲ್ಯ = sb->wp->w_topline;
		}
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    ಶೂನ್ಯ
gui_mch_create_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ orient)
{
    ಆದರೆ (orient == SBAR_HORIZ)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	sb->id = gtk_hscrollbar_new(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (orient == SBAR_VERT)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	sb->id = gtk_vscrollbar_new(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (sb->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    GtkAdjustment *adjustment;

    gtk_widget_ಗಣ_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_ಮೌಲ್ಯ_ಬದಲಾಯಿತು),
	    GINT_ಗಮ್ಯ_ಸೂಚಕ(sb->ident));
    gui_mch_update();
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ಆದರೆ (sb->id != ಖಾಲಿ)
    {
	gtk_widget_destroy(sb->id);
	sb->id = ಖಾಲಿ;
    }
    gui_mch_update();
}

#ಆದರೆ defined(FEAT_BROWSE)
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
gui_mch_browse(ಪೂರ್ಣಾಂಕ saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext ಬಳಸದ,
	       char_u *initdir,
	       char_u *filter)
{
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,20,0)
    GtkFileChooserNative	*fc;
# ಇಲ್ಲದಿದ್ದರೆ
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    ಸ್ಥಿರಾಂಕ gchar		*domain = "Gtk";

    title = CONVERT_ಗಮ್ಯ_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    ಆದರೆ (initdir == ಖಾಲಿ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ತುಂಬಿದ_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    ಕೂಡಿಸು_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_ಗಣ_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, ಖಾಲಿ);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,20,0)
    fc = gtk_file_chooser_native_new(
# ಇಲ್ಲದಿದ್ದರೆ
    fc = gtk_file_chooser_dialog_new(
# endif
	    (ಸ್ಥಿರಾಂಕ gchar *)title,
	    GTK_ಕಿಟಕಿ(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  ಇಲ್ಲದಿದ್ದರೆ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    ಖಾಲಿ);
# endif
    gtk_file_chooser_ಗಣ_current_folder(GTK_FILE_CHOOSER(fc),
						       (ಸ್ಥಿರಾಂಕ gchar *)dirbuf);

    ಆದರೆ (filter != ಖಾಲಿ && *filter != NUL)
    {
	ಪೂರ್ಣಾಂಕ     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ಆದರೆ (patt != ಖಾಲಿ)
	{
	    ಆಗ (p != ಖಾಲಿ && *p != NUL)
	    {
		ಆದರೆ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ಆದರೆ (*p == '\t')
			gtk_file_filter_ಗಣ_name(gfilter, (gchar *)patt);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			gtk_file_filter_ಕೂಡಿಸು_ನಮೂನೆ(gfilter, (gchar *)patt);
			ಆದರೆ (*p == '\n')
			{
			    gtk_file_chooser_ಕೂಡಿಸು_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    ಆದರೆ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    ಆದರೆ (saving && dflt != ಖಾಲಿ && *dflt != NUL)
	gtk_file_chooser_ಗಣ_current_name(GTK_FILE_CHOOSER(fc), (ಅಕ್ಷರ *)dflt);

    gui.browse_fname = ಖಾಲಿ;
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,20,0)
    ಆದರೆ (gtk_native_dialog_ಸತತ(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (gtk_dialog_ಸತತ(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	ಅಕ್ಷರ *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,20,0)
    g_object_unref(fc);
# ಇಲ್ಲದಿದ್ದರೆ
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_ಗಮ್ಯ_UTF8_FREE(title);
    ಆದರೆ (gui.browse_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // shorten the file name if possible
    ಹಿಂತಿರುಗಿ vim_strsave(shorten_fname1(gui.browse_fname));
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
# ಆದರೆ defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = ಖಾಲಿ;

    title = CONVERT_ಗಮ್ಯ_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (ಸ್ಥಿರಾಂಕ gchar *)title,
	    GTK_ಕಿಟಕಿ(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  ಇಲ್ಲದಿದ್ದರೆ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    ಖಾಲಿ);

    CONVERT_ಗಮ್ಯ_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    ಆದರೆ (initdir == ಖಾಲಿ || *initdir == NUL
	       || vim_ತುಂಬಿದ_Name(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    ಕೂಡಿಸು_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_ಗಣ_filename(GTK_FILE_CHOOSER(dirdlg),
						      (ಸ್ಥಿರಾಂಕ gchar *)dirbuf);

    // Run the dialog.
    ಆದರೆ (gtk_dialog_ಸತತ(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    ಆದರೆ (dirname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    ಹಿಂತಿರುಗಿ p;

# ಇಲ್ಲದಿದ್ದರೆ // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    ಹಿಂತಿರುಗಿ gui_mch_browse(0, title, ಖಾಲಿ, ಖಾಲಿ, initdir, ಖಾಲಿ);
# endif // !defined(GTK_FILE_CHOOSER)
}


#ಆದರೆ_ಅಂತ್ಯ	// FEAT_BROWSE

#ಆದರೆ defined(FEAT_GUI_DIALOG)

    ಸ್ಥಿರ GtkWidget *
create_ಸಂದೇಶ_dialog(ಪೂರ್ಣಾಂಕ type, char_u *title, char_u *ಸಂದೇಶ_2)
{
    GtkWidget	    *dialog;
    Gtk_ಸಂದೇಶ_Type  ಸಂದೇಶ_type;

    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ VIM_ERROR:	    ಸಂದೇಶ_type = GTK_ಸಂದೇಶ_ERROR;	 ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_WARNING:   ಸಂದೇಶ_type = GTK_ಸಂದೇಶ_WARNING;	 ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_QUESTION:  ಸಂದೇಶ_type = GTK_ಸಂದೇಶ_QUESTION; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:	    ಸಂದೇಶ_type = GTK_ಸಂದೇಶ_INFO;	 ನಿಲ್ಲಿಸು;
    }

    ಸಂದೇಶ_2 = CONVERT_ಗಮ್ಯ_UTF8(ಸಂದೇಶ_2);
    dialog  = gtk_ಸಂದೇಶ_dialog_new(GTK_ಕಿಟಕಿ(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_ಪೋಷಕ,
				     ಸಂದೇಶ_type,
				     GTK_BUTTONS_NONE,
				     "%s", (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಸಂದೇಶ_2);
    CONVERT_ಗಮ್ಯ_UTF8_FREE(ಸಂದೇಶ_2);

    ಆದರೆ (title != ಖಾಲಿ)
    {
	title = CONVERT_ಗಮ್ಯ_UTF8(title);
	gtk_ಕಿಟಕಿ_ಗಣ_title(GTK_ಕಿಟಕಿ(dialog), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)title);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(title);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == VIM_GENERIC)
    {
	gtk_ಕಿಟಕಿ_ಗಣ_title(GTK_ಕಿಟಕಿ(dialog), "VIM");
    }

    ಹಿಂತಿರುಗಿ dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ **
ವಿಭಜಿಸು_button_string(char_u *button_string, ಪೂರ್ಣಾಂಕ *n_buttons)
{
    ಅಕ್ಷರ	    **array;
    char_u	    *p;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    ಎಣಿಕೆ_2 = 1;

    ಗಾಗಿ (p = button_string; *p != NUL; ++p)
	ಆದರೆ (*p == DLG_BUTTON_SEP)
	    ++ಎಣಿಕೆ_2;

    array = ALLOC_MULT(ಅಕ್ಷರ *, ಎಣಿಕೆ_2 + 1);
    ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (array != ಖಾಲಿ)
    {
	array[ಎಣಿಕೆ_2++] = (ಅಕ್ಷರ *)button_string;
	ಗಾಗಿ (p = button_string; *p != NUL; )
	{
	    ಆದರೆ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ಎಣಿಕೆ_2++] = (ಅಕ್ಷರ *)p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ಇಲ್ಲದಿದ್ದರೆ
		MB_PTR_ADV(p);
	}
	array[ಎಣಿಕೆ_2] = ಖಾಲಿ; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ array;
}

# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ಸ್ಥಿರ ಅಕ್ಷರ **
ವಿಭಜಿಸು_button_translation(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_2)
{
    ಅಕ್ಷರ    **buttons = ಖಾಲಿ;
    char_u  *str;
    ಪೂರ್ಣಾಂಕ	    n_buttons = 0;
    ಪೂರ್ಣಾಂಕ	    n_expected = 1;

    ಗಾಗಿ (str = (char_u *)ಸಂದೇಶ_2; *str != NUL; ++str)
	ಆದರೆ (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(ಸಂದೇಶ_2);
    ಆದರೆ (str != ಖಾಲಿ)
    {
	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    str = vim_strsave(str);

	ಆದರೆ (str != ಖಾಲಿ)
	    buttons = ವಿಭಜಿಸು_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    ಆದರೆ (buttons == ಖಾಲಿ || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = ಖಾಲಿ;
	str = vim_strsave((char_u *)ಸಂದೇಶ_2);

	ಆದರೆ (str != ಖಾಲಿ)
	    buttons = ವಿಭಜಿಸು_button_string(str, &n_buttons);
	ಆದರೆ (buttons == ಖಾಲಿ)
	    vim_free(str);
    }

    ಹಿಂತಿರುಗಿ buttons;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
button_equal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b)
{
    ಆಗ (*a != '\0' && *b != '\0')
    {
	ಆದರೆ (*a == '_' && *++a == '\0')
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (*b == '_' && *++b == '\0')
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    ಹಿಂತಿರುಗಿ FALSE;

	a = g_utf8_ಮುಂದಿನ_char(a);
	b = g_utf8_ಮುಂದಿನ_char(b);
    }

    ಹಿಂತಿರುಗಿ (*a == '\0' && *b == '\0');
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
dialog_ಕೂಡಿಸು_buttons(GtkDialog *dialog, char_u *button_string)
{
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ಅಕ್ಷರ    **ok;
    ಅಕ್ಷರ    **ync;  // "yes no cancel"
# endif
    ಅಕ್ಷರ    **buttons;
    ಪೂರ್ಣಾಂಕ	    n_buttons = 0;
    ಪೂರ್ಣಾಂಕ	    idx;

    button_string = vim_strsave(button_string); // must be writable
    ಆದರೆ (button_string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Check 'v' flag in 'guioptions': vertical button placement.
    ಆದರೆ (vim_strchr(p_go, GO_VERTICAL) != ಖಾಲಿ)
    {
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	GtkWidget	*vbutton_box;

	vbutton_box = gtk_vbutton_box_new();
	gtk_widget_show(vbutton_box);
	gtk_box_pack_ಕೊನೆ(GTK_BOX(GTK_DIALOG(dialog)->vbox),
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
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ok	    = ವಿಭಜಿಸು_button_translation(N_("&Ok"));
    ync     = ವಿಭಜಿಸು_button_translation(N_("&Yes\n&No\n&Cancel"));
# endif
    buttons = ವಿಭಜಿಸು_button_string(button_string, &n_buttons);

    /*
     * Yes, the buttons are in reversed order to match the GNOME 2 desktop
     * environment.  Don't hit me -- it's all about consistency.
     * Well, apparently somebody changed his mind: with GTK 2.2.4 it works the
     * other way around...
     */
    ಗಾಗಿ (idx = 1; idx <= n_buttons; ++idx)
    {
	ಅಕ್ಷರ	*label;
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
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
	ಆದರೆ (ok != ಖಾಲಿ && ync != ಖಾಲಿ) // almost impossible to fail
	{
	    ಆದರೆ	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_ಗಮ್ಯ_UTF8((char_u *)label);
	gtk_dialog_ಕೂಡಿಸು_button(dialog, (ಸ್ಥಿರಾಂಕ gchar *)label8, idx);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(label8);
    }

# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ಆದರೆ (ok != ಖಾಲಿ)
	vim_free(*ok);
    ಆದರೆ (ync != ಖಾಲಿ)
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _DialogInfo
{
    ಪೂರ್ಣಾಂಕ		ignore_enter;	    // no default button, ignore "Enter"
    ಪೂರ್ಣಾಂಕ		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    ಸ್ಥಿರ gboolean
dialog_ಕೀಲಿ_press_event_cb(GtkWidget *widget, Gdk_Event_ಕೀಲಿ *event, gpointer ದತ್ತಾಂಶ)
{
    DialogInfo *di = (DialogInfo *)ದತ್ತಾಂಶ;

    // Ignore hitting Enter (or Space) when there is no default button.
    ಆದರೆ (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	ಹಿಂತಿರುಗಿ TRUE;
    ಇಲ್ಲದಿದ್ದರೆ    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    ಆದರೆ (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_ಮುಖವಾಡ()) == 0)
    {
	ಹಿಂತಿರುಗಿ gtk_ಕಿಟಕಿ_mnemonic_activate(
		   GTK_ಕಿಟಕಿ(widget), event->keyval,
		   gtk_ಕಿಟಕಿ_get_mnemonic_modifier(GTK_ಕಿಟಕಿ(widget)));
    }

    ಹಿಂತಿರುಗಿ FALSE; // continue emission
}

    ಪೂರ್ಣಾಂಕ
gui_mch_dialog(ಪೂರ್ಣಾಂಕ	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*ಸಂದೇಶ_2,   // message text
	       char_u	*buttons,   // names of buttons
	       ಪೂರ್ಣಾಂಕ	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       ಪೂರ್ಣಾಂಕ	ex_cmd ಬಳಸದ)
{
    GtkWidget	*dialog;
    GtkWidget	*ನಮೂದು = ಖಾಲಿ;
    char_u	*ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_ಸಕ್ರಿಯ;

    dialog = create_ಸಂದೇಶ_dialog(type, title, ಸಂದೇಶ_2);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_ಕೂಡಿಸು_buttons(GTK_DIALOG(dialog), buttons);

    ಆದರೆ (textfield != ಖಾಲಿ)
    {
	GtkWidget *alignment;

	ನಮೂದು = gtk_ನಮೂದು_new();
	gtk_widget_show(ನಮೂದು);

	// Make Enter work like pressing OK.
	gtk_ನಮೂದು_ಗಣ_activates_default(GTK_ನಮೂದು(ನಮೂದು), TRUE);

	ಪಠ್ಯ = CONVERT_ಗಮ್ಯ_UTF8(textfield);
	gtk_ನಮೂದು_ಗಣ_ಪಠ್ಯ(GTK_ನಮೂದು(ನಮೂದು), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(ಪಠ್ಯ);

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
	gtk_widget_ಗಣ_halign(GTK_WIDGET(ನಮೂದು), GTK_ALIGN_CENTER);
	gtk_widget_ಗಣ_valign(GTK_WIDGET(ನಮೂದು), GTK_ALIGN_CENTER);
	gtk_widget_ಗಣ_hexpand(GTK_WIDGET(ನಮೂದು), TRUE);
	gtk_widget_ಗಣ_vexpand(GTK_WIDGET(ನಮೂದು), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# ಇಲ್ಲದಿದ್ದರೆ
	alignment = gtk_alignment_new((ದಶಮಾಂಶ)0.5, (ದಶಮಾಂಶ)0.5,
						      (ದಶಮಾಂಶ)1.0, (ದಶಮಾಂಶ)1.0);
# endif
	gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(alignment), ನಮೂದು);
	gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	{
	    GtkWidget * ಸ್ಥಿರಾಂಕ vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_ಆರಂಭ(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# ಇಲ್ಲದಿದ್ದರೆ
	gtk_box_pack_ಆರಂಭ(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_ಕೀಲಿ_press_event_cb), &dialoginfo);

    ಆದರೆ (def_but > 0)
    {
	gtk_dialog_ಗಣ_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_ಸತತ(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    ಆದರೆ (response != GTK_RESPONSE_NONE)
    {
	ಆದರೆ (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	ಆದರೆ (textfield != ಖಾಲಿ)
	{
	    ಪಠ್ಯ = (char_u *)gtk_ನಮೂದು_get_ಪಠ್ಯ(GTK_ನಮೂದು(ನಮೂದು));
	    ಪಠ್ಯ = CONVERT_FROM_UTF8(ಪಠ್ಯ);

	    vim_strncpy(textfield, ಪಠ್ಯ, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(ಪಠ್ಯ);
	}
	gtk_widget_destroy(dialog);
    }

    ಆದರೆ (gui.dialogs_ಸಕ್ರಿಯ > 0)
	--gui.dialogs_ಸಕ್ರಿಯ;
    ಹಿಂತಿರುಗಿ response > 0 ? response : 0;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG


#ಆದರೆ defined(FEAT_MENU)

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# ಆದರೆ defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ಆದರೆ (xic != ಖಾಲಿ && g_object_get_ದತ್ತಾಂಶ(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == ಖಾಲಿ)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_ಅಂಶ_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_ಗಮ್ಯ_UTF8(name);
	menuitem = gtk_menu_ಅಂಶ_new_with_mnemonic((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(name);
	gtk_widget_show(menuitem);

	submenu = gtk_menu_new();
	gtk_menu_ಅಂಶ_ಗಣ_submenu(GTK_MENU_ಅಂಶ(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	gtk_im_multicontext_append_menuitems(GTK_IM_MULTICONTEXT(xic),
					     GTK_MENU_SHELL(submenu));
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(menu->submenu_id),
			  "vim-has-im-menu", GINT_ಗಮ್ಯ_ಸೂಚಕ(TRUE));
    }
#  endif
# endif // FEAT_XIM

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
    {
	GdkEventButton trigger;

	// A pseudo event to have gtk_menu_popup_at_pointer() work. Since the
	// function calculates the popup menu position on the basis of the
	// actual pointer position when it is invoked, the fields x, y, x_root
	// and y_root are set to zero for convenience.
	trigger.type       = GDK_BUTTON_PRESS;
	trigger.ಕಿಟಕಿ_2     = gtk_widget_get_ಕಿಟಕಿ(gui.drawarea);
	trigger.send_event = FALSE;
	trigger.ಸಮಯ       = gui.event_ಸಮಯ;
	trigger.x	   = 0.0;
	trigger.y	   = 0.0;
	trigger.axes       = ಖಾಲಿ;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = ಖಾಲಿ;
	trigger.x_ಬೇರು     = 0.0;
	trigger.y_ಬೇರು     = 0.0;

	gtk_menu_popup_at_ಸೂಚಕ(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# ಇಲ್ಲದಿದ್ದರೆ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   ಖಾಲಿ, ಖಾಲಿ,
		   (GtkMenuPositionFunc)ಖಾಲಿ, ಖಾಲಿ,
		   3U, gui.event_ಸಮಯ);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
popup_menu_position_func(GtkMenu *menu ಬಳಸದ,
			 gint *x, gint *y,
			 gboolean *ತಳ್ಳು_ಒಳ ಬಳಸದ,
			 gpointer user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    gdk_ಕಿಟಕಿ_get_origin(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea), x, y);

    ಆದರೆ (popup_mouse_pos)
    {
	ಪೂರ್ಣಾಂಕ	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin != ಖಾಲಿ && gui.drawarea != ಖಾಲಿ &&
	     gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) != ಖಾಲಿ)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    ಶೂನ್ಯ
gui_make_popup(char_u *ಮಾರ್ಗ_name, ಪೂರ್ಣಾಂಕ mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_ಕಂಡುಹಿಡಿ_menu(ಮಾರ್ಗ_name);
    ಆದರೆ (menu == ಖಾಲಿ || menu->submenu_id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
    Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ win = gtk_widget_get_ಕಿಟಕಿ(gui.drawarea);
    GdkEventButton trigger;

    // A pseudo event to have gtk_menu_popup_at_*() functions work. Since
    // the position where the menu pops up is automatically adjusted by
    // the functions, none of the fields x, y, x_root and y_root has to be
    // set to a specific value here; therefore, they are set to zero for
    // convenience.
    trigger.type       = GDK_BUTTON_PRESS;
    trigger.ಕಿಟಕಿ_2     = win;
    trigger.send_event = FALSE;
    trigger.ಸಮಯ       = GDK_CURRENT_ಸಮಯ;
    trigger.x	   = 0.0;
    trigger.y	   = 0.0;
    trigger.axes       = ಖಾಲಿ;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = ಖಾಲಿ;
    trigger.x_ಬೇರು     = 0.0;
    trigger.y_ಬೇರು     = 0.0;

    ಆದರೆ (mouse_pos)
	gtk_menu_popup_at_ಸೂಚಕ(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_ಕಿಟಕಿ_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(ಖಾಲಿ, &rect.x, &rect.y, ಖಾಲಿ, ಖಾಲಿ);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# ಇಲ್ಲದಿದ್ದರೆ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    ಖಾಲಿ, ಖಾಲಿ,
	    &popup_menu_position_func, ಖಾಲಿ,
	    0U, (guint32)GDK_CURRENT_ಸಮಯ);
# endif
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU


/*
 * We don't create it twice.
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Shared_ಕಂಡುಹಿಡಿ_Replace
{
    GtkWidget *dialog;	// the main dialog widget
    GtkWidget *wword;	// 'Whole word only' check button
    GtkWidget *mcase;	// 'Match case' check button
    GtkWidget *ಮೇಲೆ;	// search direction 'Up' radio button
    GtkWidget *ಕೆಳಕ್ಕೆ;	// search direction 'Down' radio button
    GtkWidget *what;	// 'Find what' entry text widget
    GtkWidget *with;	// 'Replace with' entry text widget
    GtkWidget *ಕಂಡುಹಿಡಿ;	// 'Find Next' action button
    GtkWidget *replace;	// 'Replace With' action button
    GtkWidget *all;	// 'Replace All' action button
} Shared_ಕಂಡುಹಿಡಿ_Replace_2;

ಸ್ಥಿರ Shared_ಕಂಡುಹಿಡಿ_Replace_2 ಕಂಡುಹಿಡಿ_widgets = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
ಸ್ಥಿರ Shared_ಕಂಡುಹಿಡಿ_Replace_2 repl_widgets = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಕೀಲಿ_press_event(
		GtkWidget	*widget ಬಳಸದ,
		Gdk_Event_ಕೀಲಿ	*event,
		Shared_ಕಂಡುಹಿಡಿ_Replace_2 *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ಆದರೆ (event->state & (GDK_CONTROL_ಮುಖವಾಡ | GDK_ಸರಿಸು_ಮುಖವಾಡ))
	ಹಿಂತಿರುಗಿ FALSE;

    // the Escape key synthesizes a cancellation action
    ಆದರೆ (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	ಹಿಂತಿರುಗಿ TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ GtkWidget *
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
create_image_button(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *stock_id ಬಳಸದ,
		    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label)
#ಇಲ್ಲದಿದ್ದರೆ
create_image_button(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *stock_id,
		    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label)
#ಆದರೆ_ಅಂತ್ಯ
{
    char_u	*ಪಠ್ಯ;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    ಪಠ್ಯ = CONVERT_ಗಮ್ಯ_UTF8((char_u *)label);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_ಗಣ_homogeneous(GTK_BOX(box), FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
    box = gtk_hbox_new(FALSE, 3);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ಆದರೆ (stock_id != ಖಾಲಿ)
	gtk_box_pack_ಆರಂಭ(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_ಗಾತ್ರ_BUTTON),
			   FALSE, FALSE, 0);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_box_pack_ಆರಂಭ(GTK_BOX(box),
		       gtk_label_new((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ),
		       FALSE, FALSE, 0);

    CONVERT_ಗಮ್ಯ_UTF8_FREE(ಪಠ್ಯ);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    gtk_widget_ಗಣ_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_ಗಣ_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_ಗಣ_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_ಗಣ_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#ಇಲ್ಲದಿದ್ದರೆ
    alignment = gtk_alignment_new((ದಶಮಾಂಶ)0.5, (ದಶಮಾಂಶ)0.5,
						      (ದಶಮಾಂಶ)0.0, (ದಶಮಾಂಶ)0.0);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(button), alignment);

    ಹಿಂತಿರುಗಿ button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
convert_localized_ಸಂದೇಶ(char_u **ಬಫರ್, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_2)
{
    ಆದರೆ (output_conv.vc_type == CONV_NONE)
	ಹಿಂತಿರುಗಿ ಸಂದೇಶ_2;

    vim_free(*ಬಫರ್);
    *ಬಫರ್ = string_convert(&output_conv, (char_u *)ಸಂದೇಶ_2, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)*ಬಫರ್;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ
ನಮೂದು_get_ಪಠ್ಯ_ಉದ್ದಳತೆ(Gtk_ನಮೂದು *ನಮೂದು)
{
    g_return_val_if_fail(ನಮೂದು != ಖಾಲಿ, 0);
    g_return_val_if_fail(GTK_IS_ನಮೂದು(ನಮೂದು) == TRUE, 0);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    ಹಿಂತಿರುಗಿ gtk_ನಮೂದು_ಬಫರ್_get_ಉದ್ದಳತೆ(gtk_ನಮೂದು_get_ಬಫರ್(ನಮೂದು));
#ಅಥವಾಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    ಹಿಂತಿರುಗಿ gtk_ನಮೂದು_get_ಪಠ್ಯ_ಉದ್ದಳತೆ(ನಮೂದು);
#ಇಲ್ಲದಿದ್ದರೆ
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    ಹಿಂತಿರುಗಿ g_utf8_strlen(gtk_ನಮೂದು_get_ಪಠ್ಯ(ನಮೂದು), -1);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_dialog_create(char_u *arg, ಪೂರ್ಣಾಂಕ do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*ಕೋಷ್ಟಕ;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    Shared_ಕಂಡುಹಿಡಿ_Replace_2 *frdp;
    char_u	*ನಮೂದು_ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		wword = FALSE;
    ಪೂರ್ಣಾಂಕ		mcase = !p_ic;
    char_u	*conv_ಬಫರ್ = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿಸು CONV(ಸಂದೇಶ_2) convert_localized_ಸಂದೇಶ(&conv_ಬಫರ್, (ಸಂದೇಶ_2))

    frdp = (do_replace) ? (&repl_widgets) : (&ಕಂಡುಹಿಡಿ_widgets);

    // Get the search string to use.
    ನಮೂದು_ಪಠ್ಯ = get_ಕಂಡುಹಿಡಿ_dialog_ಪಠ್ಯ(arg, &wword, &mcase);

    ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_ಪಠ್ಯ = ನಮೂದು_ಪಠ್ಯ;
	ನಮೂದು_ಪಠ್ಯ = string_convert(&output_conv, ನಮೂದು_ಪಠ್ಯ, ಖಾಲಿ);
	vim_free(old_ಪಠ್ಯ);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    ಆದರೆ (frdp->dialog)
    {
	ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ)
	{
	    gtk_ನಮೂದು_ಗಣ_ಪಠ್ಯ(GTK_ನಮೂದು(frdp->what), (ಅಕ್ಷರ *)ನಮೂದು_ಪಠ್ಯ);
	    gtk_toggle_button_ಗಣ_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(frdp->wword),
							     (gboolean)wword);
	    gtk_toggle_button_ಗಣ_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
	}
	gtk_ಕಿಟಕಿ_present(GTK_ಕಿಟಕಿ(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	ಆದರೆ (do_replace && ನಮೂದು_get_ಪಠ್ಯ_ಉದ್ದಳತೆ(GTK_ನಮೂದು(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(ನಮೂದು_ಪಠ್ಯ);
	ಹಿಂತಿರುಗಿ;
    }

    frdp->dialog = gtk_dialog_new();
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#ಇಲ್ಲದಿದ್ದರೆ
    gtk_dialog_ಗಣ_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_ಕಿಟಕಿ_ಗಣ_transient_for(GTK_ಕಿಟಕಿ(frdp->dialog), GTK_ಕಿಟಕಿ(gui.mainwin));
    gtk_ಕಿಟಕಿ_ಗಣ_destroy_with_ಪೋಷಕ(GTK_ಕಿಟಕಿ(frdp->dialog), TRUE);

    ಆದರೆ (do_replace)
    {
	gtk_ಕಿಟಕಿ_ಗಣ_title(GTK_ಕಿಟಕಿ(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gtk_ಕಿಟಕಿ_ಗಣ_title(GTK_ಕಿಟಕಿ(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_ಗಣ_homogeneous(GTK_BOX(hbox), FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
    hbox = gtk_hbox_new(FALSE, 0);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(hbox), 10);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    {
	GtkWidget * ಸ್ಥಿರಾಂಕ dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(dialog_vbox), hbox);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (do_replace)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	ಕೋಷ್ಟಕ = gtk_grid_new();
#ಇಲ್ಲದಿದ್ದರೆ
	ಕೋಷ್ಟಕ = gtk_ಕೋಷ್ಟಕ_new(1024, 4, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	ಕೋಷ್ಟಕ = gtk_grid_new();
#ಇಲ್ಲದಿದ್ದರೆ
	ಕೋಷ್ಟಕ = gtk_ಕೋಷ್ಟಕ_new(1024, 3, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_box_pack_ಆರಂಭ(GTK_BOX(hbox), ಕೋಷ್ಟಕ, TRUE, TRUE, 0);
    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(ಕೋಷ್ಟಕ), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,16,0)
    gtk_label_ಗಣ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_ಗಣ_yalign(GTK_LABEL(tmp), 0.5);
#ಅಥವಾಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    {
	GValue align_val = G_ಮೌಲ್ಯ_INIT;

	g_ಮೌಲ್ಯ_init(&align_val, G_TYPE_FLOAT);

	g_ಮೌಲ್ಯ_ಗಣ_float(&align_val, 0.0);
	g_object_ಗಣ_property(G_OBJECT(tmp), "xalign", &align_val);

	g_ಮೌಲ್ಯ_ಗಣ_float(&align_val, 0.5);
	g_object_ಗಣ_property(G_OBJECT(tmp), "yalign", &align_val);

	g_ಮೌಲ್ಯ_unset(&align_val);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    gtk_misc_ಗಣ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), tmp, 0, 0, 2, 1);
#ಇಲ್ಲದಿದ್ದರೆ
    gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ
    frdp->what = gtk_ನಮೂದು_new();
    sensitive = (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ && ನಮೂದು_ಪಠ್ಯ[0] != NUL);
    ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ)
	gtk_ನಮೂದು_ಗಣ_ಪಠ್ಯ(GTK_ನಮೂದು(frdp->what), (ಅಕ್ಷರ *)ನಮೂದು_ಪಠ್ಯ);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(ನಮೂದು_ಬದಲಾಯಿತು_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(ಕಂಡುಹಿಡಿ_ಕೀಲಿ_press_event),
			   (gpointer) frdp);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), frdp->what, 2, 0, 5, 1);
#ಇಲ್ಲದಿದ್ದರೆ
    gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,16,0)
	gtk_label_ಗಣ_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_ಗಣ_yalign(GTK_LABEL(tmp), 0.5);
#ಅಥವಾಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
	{
	    GValue align_val = G_ಮೌಲ್ಯ_INIT;

	    g_ಮೌಲ್ಯ_init(&align_val, G_TYPE_FLOAT);

	    g_ಮೌಲ್ಯ_ಗಣ_float(&align_val, 0.0);
	    g_object_ಗಣ_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_ಮೌಲ್ಯ_ಗಣ_float(&align_val, 0.5);
	    g_object_ಗಣ_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_ಮೌಲ್ಯ_unset(&align_val);
	}
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_misc_ಗಣ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), tmp, 0, 1, 2, 1);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ
	frdp->with = gtk_ನಮೂದು_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
			 GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(ಕಂಡುಹಿಡಿ_ಕೀಲಿ_press_event),
			       (gpointer) frdp);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), frdp->with, 2, 1, 5, 1);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), frdp->with, 1, 1024, 1, 2,
			 GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Make the entry activation only change the input focus onto the
	 * with item.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(ನಮೂದು_activate_cb), frdp->with);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Make the entry activation do the search.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
			 GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_FINDNEXT));
    }

    // whole word only button
    frdp->wword = gtk_check_button_new_with_label(CONV(_("Match whole word only")));
    gtk_toggle_button_ಗಣ_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(frdp->wword),
							(gboolean)wword);
    ಆದರೆ (do_replace)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), frdp->wword, 0, 2, 5, 1);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), frdp->wword, 0, 3, 5, 1);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_ಗಣ_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    ಆದರೆ (do_replace)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), frdp->mcase, 0, 3, 5, 1);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), frdp->mcase, 0, 4, 5, 1);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ

    tmp = gtk_ಚೌಕಟ್ಟು_new(CONV(_("Direction")));
    ಆದರೆ (do_replace)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), tmp, 5, 2, 2, 4);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	gtk_grid_attach(GTK_GRID(ಕೋಷ್ಟಕ), tmp, 5, 2, 1, 3);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕೋಷ್ಟಕ_attach(GTK_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_ಗಣ_homogeneous(GTK_BOX(vbox), FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
    vbox = gtk_vbox_new(FALSE, 0);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(vbox), 0);
    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->ಮೇಲೆ = gtk_radio_button_new_with_label(ಖಾಲಿ, CONV(_("Up")));
    gtk_box_pack_ಆರಂಭ(GTK_BOX(vbox), frdp->ಮೇಲೆ, TRUE, TRUE, 0);
    frdp->ಕೆಳಕ್ಕೆ = gtk_radio_button_new_with_label(
			gtk_radio_button_get_ಗುಂಪು(GTK_RADIO_BUTTON(frdp->ಮೇಲೆ)),
			CONV(_("Down")));
    gtk_toggle_button_ಗಣ_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(frdp->ಕೆಳಕ್ಕೆ), TRUE);
    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_ಆರಂಭ(GTK_BOX(vbox), frdp->ಕೆಳಕ್ಕೆ, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#ಇಲ್ಲದಿದ್ದರೆ
    actionarea = gtk_vbutton_box_new();
#ಆದರೆ_ಅಂತ್ಯ
    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_ಕೊನೆ(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    frdp->ಕಂಡುಹಿಡಿ = create_image_button(ಖಾಲಿ, _("Find Next"));
#ಇಲ್ಲದಿದ್ದರೆ
    frdp->ಕಂಡುಹಿಡಿ = create_image_button(GTK_STOCK_ಕಂಡುಹಿಡಿ, _("Find Next"));
#ಆದರೆ_ಅಂತ್ಯ
    gtk_widget_ಗಣ_sensitive(frdp->ಕಂಡುಹಿಡಿ, sensitive);

    g_signal_connect(G_OBJECT(frdp->ಕಂಡುಹಿಡಿ), "clicked",
		     G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
		     (do_replace) ? GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_R_FINDNEXT)
				  : GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_FINDNEXT));

    gtk_widget_ಗಣ_can_default(frdp->ಕಂಡುಹಿಡಿ, TRUE);
    gtk_box_pack_ಆರಂಭ(GTK_BOX(actionarea), frdp->ಕಂಡುಹಿಡಿ, FALSE, FALSE, 0);
    gtk_widget_grab_default(frdp->ಕಂಡುಹಿಡಿ);

    ಆದರೆ (do_replace)
    {
	// 'Replace' button
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
	frdp->replace = create_image_button(ಖಾಲಿ, _("Replace"));
#ಇಲ್ಲದಿದ್ದರೆ
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#ಆದರೆ_ಅಂತ್ಯ
	gtk_widget_ಗಣ_sensitive(frdp->replace, sensitive);
	gtk_widget_ಗಣ_can_default(frdp->ಕಂಡುಹಿಡಿ, TRUE);
	gtk_box_pack_ಆರಂಭ(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
			 GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_REPLACE));

	// 'Replace All' button
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
	frdp->all = create_image_button(ಖಾಲಿ, _("Replace All"));
#ಇಲ್ಲದಿದ್ದರೆ
	frdp->all = create_image_button(GTK_STOCK_CONVERT, _("Replace All"));
#ಆದರೆ_ಅಂತ್ಯ
	gtk_widget_ಗಣ_sensitive(frdp->all, sensitive);
	gtk_widget_ಗಣ_can_default(frdp->all, TRUE);
	gtk_box_pack_ಆರಂಭ(GTK_BOX(actionarea), frdp->all, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->all), "clicked",
			 G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
			 GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_REPLACEALL));
    }

    // 'Cancel' button
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#ಇಲ್ಲದಿದ್ದರೆ
    tmp = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_widget_ಗಣ_can_default(tmp, TRUE);
    gtk_box_pack_ಕೊನೆ(GTK_BOX(actionarea), tmp, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(tmp),
			     "clicked", G_CALLBACK(gtk_widget_hide),
			     G_OBJECT(frdp->dialog));
    g_signal_connect_swapped(G_OBJECT(frdp->dialog),
			     "delete-event", G_CALLBACK(gtk_widget_hide_on_delete),
			     G_OBJECT(frdp->dialog));

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#ಇಲ್ಲದಿದ್ದರೆ
    tmp = gtk_vseparator_new();
#ಆದರೆ_ಅಂತ್ಯ
    gtk_box_pack_ಕೊನೆ(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#ಇಲ್ಲದಿದ್ದರೆ
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#ಆದರೆ_ಅಂತ್ಯ
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(ನಮೂದು_ಪಠ್ಯ);
    vim_free(conv_ಬಫರ್);
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CONV
}

    ಶೂನ್ಯ
gui_mch_ಕಂಡುಹಿಡಿ_dialog(exarg_T *eap)
{
    ಆದರೆ (gui.ಒಳ_use)
	ಕಂಡುಹಿಡಿ_replace_dialog_create(eap->arg, FALSE);
}

    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
    ಆದರೆ (gui.ಒಳ_use)
	ಕಂಡುಹಿಡಿ_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    ಪೂರ್ಣಾಂಕ			flags;
    char_u		*ಕಂಡುಹಿಡಿ_ಪಠ್ಯ;
    char_u		*repl_ಪಠ್ಯ;
    gboolean		direction_ಕೆಳಕ್ಕೆ;
    Shared_ಕಂಡುಹಿಡಿ_Replace_2	*sfr;

    flags = (ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)ದತ್ತಾಂಶ;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    ಆದರೆ (flags == FRD_FINDNEXT)
    {
	repl_ಪಠ್ಯ = ಖಾಲಿ;
	sfr = &ಕಂಡುಹಿಡಿ_widgets;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	repl_ಪಠ್ಯ = (char_u *)gtk_ನಮೂದು_get_ಪಠ್ಯ(GTK_ನಮೂದು(repl_widgets.with));
	sfr = &repl_widgets;
    }

    ಕಂಡುಹಿಡಿ_ಪಠ್ಯ = (char_u *)gtk_ನಮೂದು_get_ಪಠ್ಯ(GTK_ನಮೂದು(sfr->what));
    direction_ಕೆಳಕ್ಕೆ = gtk_toggle_button_get_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(sfr->ಕೆಳಕ್ಕೆ));

    ಆದರೆ (gtk_toggle_button_get_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_ಸಂಪೂರ್ಣ_ಶಬ್ದ;
    ಆದರೆ (gtk_toggle_button_get_ಸಕ್ರಿಯ(GTK_TOGGLE_BUTTON(sfr->mcase)))
	flags |= FRD_ಹೊಂದಿಕೆ_CASE;

    repl_ಪಠ್ಯ = CONVERT_FROM_UTF8(repl_ಪಠ್ಯ);
    ಕಂಡುಹಿಡಿ_ಪಠ್ಯ = CONVERT_FROM_UTF8(ಕಂಡುಹಿಡಿ_ಪಠ್ಯ);
    gui_do_findrepl(flags, ಕಂಡುಹಿಡಿ_ಪಠ್ಯ, repl_ಪಠ್ಯ, direction_ಕೆಳಕ್ಕೆ);
    CONVERT_FROM_UTF8_FREE(repl_ಪಠ್ಯ);
    CONVERT_FROM_UTF8_FREE(ಕಂಡುಹಿಡಿ_ಪಠ್ಯ);
}

/*
 * our usual callback function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ನಮೂದು_activate_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    gtk_widget_grab_focus(GTK_WIDGET(ದತ್ತಾಂಶ));
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
ನಮೂದು_ಬದಲಾಯಿತು_cb(GtkWidget * ನಮೂದು, GtkWidget * dialog)
{
    ಸ್ಥಿರಾಂಕ gchar	*ನಮೂದು_ಪಠ್ಯ;
    gboolean	nonempty;

    ನಮೂದು_ಪಠ್ಯ = gtk_ನಮೂದು_get_ಪಠ್ಯ(GTK_ನಮೂದು(ನಮೂದು));

    ಆದರೆ (!ನಮೂದು_ಪಠ್ಯ)
	ಹಿಂತಿರುಗಿ;			// shouldn't happen

    nonempty = (ನಮೂದು_ಪಠ್ಯ[0] != '\0');

    ಆದರೆ (dialog == ಕಂಡುಹಿಡಿ_widgets.dialog)
	gtk_widget_ಗಣ_sensitive(ಕಂಡುಹಿಡಿ_widgets.ಕಂಡುಹಿಡಿ, nonempty);

    ಆದರೆ (dialog == repl_widgets.dialog)
    {
	gtk_widget_ಗಣ_sensitive(repl_widgets.ಕಂಡುಹಿಡಿ, nonempty);
	gtk_widget_ಗಣ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_ಗಣ_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    ಶೂನ್ಯ
ex_helpfind(exarg_T *eap ಬಳಸದ)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#ಆದರೆ defined(FEAT_BROWSE)
    ಸ್ಥಿರ ಶೂನ್ಯ
recent_func_log_func(ಸ್ಥಿರಾಂಕ gchar *log_domain ಬಳಸದ,
		     GLogLevelFlags log_level ಬಳಸದ,
		     ಸ್ಥಿರಾಂಕ gchar *ಸಂದೇಶ_2 ಬಳಸದ,
		     gpointer user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_ಗಣ_fullscreen(ಪೂರ್ಣಾಂಕ flag)
{
    ಆದರೆ (flag)
	gtk_ಕಿಟಕಿ_fullscreen(GTK_ಕಿಟಕಿ(gui.mainwin));
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕಿಟಕಿ_unfullscreen(GTK_ಕಿಟಕಿ(gui.mainwin));
}
