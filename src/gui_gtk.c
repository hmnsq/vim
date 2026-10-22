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

#অন্তর্ভাব "vim.h"

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
# যদি defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#endif

#ifdef FEAT_GUI_GTK
# যদি GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# অন্যথা
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# অন্যথা
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#অন্যথা
// define these items to be able to generate prototypes without GTK
প্রকারলেখ পূর্ণসংখ্যা GtkWidget;
# define gpointer পূর্ণসংখ্যা
# define guint8 পূর্ণসংখ্যা
# define GdkPixmap পূর্ণসংখ্যা
# define GdkBitmap পূর্ণসংখ্যা
# define GtkIconFactory পূর্ণসংখ্যা
# define GtkToolbar পূর্ণসংখ্যা
# define GtkAdjustment পূর্ণসংখ্যা
# define gboolean পূর্ণসংখ্যা
# define Gdk_Event_চাবি পূর্ণসংখ্যা
# define Cancel_তথ্য পূর্ণসংখ্যা
#endif

স্থির শূন্য entry_activate_cb(GtkWidget *widget, gpointer তথ্য);
স্থির শূন্য entry_changed_cb(GtkWidget *entry, GtkWidget *dialog);
স্থির শূন্য find_replace_cb(GtkWidget *widget, gpointer তথ্য);
#যদি defined(FEAT_BROWSE)
স্থির শূন্য recent_func_log_func(
	ধ্রুবক gchar *log_domain,
	GLogLevelFlags log_level,
	ধ্রুবক gchar *message,
	gpointer user_তথ্য);
#endif

#যদি defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# যদি GTK_CHECK_VERSION(3,10,0)
স্থির ধ্রুবক অক্ষর * ধ্রুবক menu_themed_names[] =
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
#  যদি GTK_CHECK_VERSION(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  অন্যথা
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  যদি GTK_CHECK_VERSION(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  অন্যথা
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  যদি GTK_CHECK_VERSION(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  অন্যথা
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  যদি GTK_CHECK_VERSION(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  অন্যথা
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# অন্যথা // !GTK_CHECK_VERSION(3,10,0)
স্থির ধ্রুবক অক্ষর * ধ্রুবক menu_stock_ids[] =
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
#  যদি !GTK_CHECK_VERSION(3,10,0)
প্রকারলেখ কাঠামো IconNames {
    ধ্রুবক অক্ষর *icon_name;
    ধ্রুবক অক্ষর *file_name;
} IconNames;

স্থির IconNames stock_vim_icons[] = {
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
    { ফাঁকা, ফাঁকা }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    স্থির শূন্য
add_stock_icon(GtkIconFactory	*factory,
	       ধ্রুবক অক্ষর	*stock_id,
	       ধ্রুবক guint8	*inline_তথ্য,
	       পূর্ণসংখ্যা		তথ্য_দৈর্ঘ্য)
{
    GdkPixbuf	*pixbuf;
    GtkIconSet	*icon_set;

    pixbuf = gdk_pixbuf_new_from_inline(তথ্য_দৈর্ঘ্য, inline_তথ্য, FALSE, ফাঁকা);
    icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);

    gtk_icon_factory_add(factory, stock_id, icon_set);

    gtk_icon_set_unref(icon_set);
    g_object_unref(pixbuf);
}
# endif

    স্থির পূর্ণসংখ্যা
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    যদি (mch_isFullName(dest))
    {
	ফেরত vim_fexists(dest);
    }
    অন্যথা
    {
	স্থির ধ্রুবক অক্ষর   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	unsigned পূর্ণসংখ্যা	    i;

	জন্য (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    যদি (gui_find_bitmap(dest, buf, (অক্ষর *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		ফেরত TRUE;
	    }

	ফেরত FALSE;
    }
}

    স্থির GtkWidget *
load_menu_iconfile(char_u *name, Gtk_Icon_আকার icon_আকার)
{
    GtkWidget	    *image = ফাঁকা;
# যদি GTK_CHECK_VERSION(3,10,0)
    পূর্ণসংখ্যা		     pixel_আকার = -1;

    নির্বাচন (icon_আকার)
    {
	ক্ষেত্র GTK_ICON_আকার_MENU:
	    pixel_আকার = 16;
	    বিরতি;
	ক্ষেত্র GTK_ICON_আকার_SMALL_TOOLBAR:
	    pixel_আকার = 16;
	    বিরতি;
	ক্ষেত্র GTK_ICON_আকার_LARGE_TOOLBAR:
	    pixel_আকার = 24;
	    বিরতি;
	ক্ষেত্র GTK_ICON_আকার_BUTTON:
	    pixel_আকার = 16;
	    বিরতি;
	ক্ষেত্র GTK_ICON_আকার_DND:
	    pixel_আকার = 32;
	    বিরতি;
	ক্ষেত্র GTK_ICON_আকার_DIALOG:
	    pixel_আকার = 48;
	    বিরতি;
	ক্ষেত্র GTK_ICON_আকার_INVALID:
	    // FALLTHROUGH
	পূর্বনির্ধারিত:
	    pixel_আকার = 0;
	    বিরতি;
    }

    যদি (pixel_আকার > 0 || pixel_আকার == -1)
    {
	GdkPixbuf * ধ্রুবক pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((ধ্রুবক অক্ষর *)name,
		pixel_আকার, pixel_আকার, TRUE, ফাঁকা);
	যদি (pixbuf != ফাঁকা)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    যদি (image == ফাঁকা)
	image = gtk_image_new_from_icon_name("image-missing", icon_আকার);

    ফেরত image;
# অন্যথা // !GTK_CHECK_VERSION(3,10,0)
    GtkIconSet	    *icon_set;
    GtkIconSource   *icon_source;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_set = gtk_icon_set_new();
    icon_source = gtk_icon_source_new();

    gtk_icon_source_set_filename(icon_source, (ধ্রুবক অক্ষর *)name);
    gtk_icon_set_add_source(icon_set, icon_source);

    image = gtk_image_new_from_icon_set(icon_set, icon_আকার);

    gtk_icon_source_free(icon_source);
    gtk_icon_set_unref(icon_set);

    ফেরত image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    স্থির GtkWidget *
create_menu_icon(vimmenu_T *menu, Gtk_Icon_আকার icon_আকার)
{
    GtkWidget	*image = ফাঁকা;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    যদি (menu->iconfile != ফাঁকা && lookup_menu_iconfile(menu->iconfile, buf))
	image = load_menu_iconfile(buf, icon_আকার);

    // If not found and not builtin specified try using the menu name.
    যদি (image == ফাঁকা && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = load_menu_iconfile(buf, icon_আকার);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    যদি (image == ফাঁকা)
    {
# যদি GTK_CHECK_VERSION(3,10,0)
	ধ্রুবক অক্ষর *icon_name = ফাঁকা;
	ধ্রুবক পূর্ণসংখ্যা   n_names = G_N_ELEMENTS(menu_themed_names);

	যদি (menu->iconidx >= 0 && menu->iconidx < n_names)
	    icon_name = menu_themed_names[menu->iconidx];
	যদি (icon_name == ফাঁকা)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_আকার);
# অন্যথা
	ধ্রুবক অক্ষর  *stock_id;
	ধ্রুবক পূর্ণসংখ্যা   n_ids = G_N_ELEMENTS(menu_stock_ids);

	যদি (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	অন্যথা
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_আকার);
# endif
    }

    ফেরত image;
}

    স্থির gint
toolbar_button_focus_in_event(GtkWidget *widget UNUSED,
			      GdkEventFocus *event UNUSED,
			      gpointer তথ্য UNUSED)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    যদি (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    ফেরত TRUE;
}
#endif // FEAT_TOOLBAR

#যদি defined(FEAT_TOOLBAR)

    শূন্য
gui_gtk_register_stock_icons(শূন্য)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define ADD_ICON(Name, তথ্য_2) add_stock_icon(factory, Name, তথ্য_2, (পূর্ণসংখ্যা)আকার(তথ্য_2))

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
# অন্যথা // defined(USE_GRESOURCE)
    ধ্রুবক অক্ষর * ধ্রুবক path_prefix = "/org/vim/gui/icon";
#  যদি GTK_CHECK_VERSION(3,14,0)
    GdkScreen    *screen = ফাঁকা;
    GtkIconTheme *icon_theme = ফাঁকা;

    যদি (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    অন্যথা
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_add_resource_path(icon_theme, path_prefix);
#  elif GTK_CHECK_VERSION(3,0,0)
    IconNames *names;

    জন্য (names = stock_vim_icons; names->icon_name != ফাঁকা; names++)
    {
	অক্ষর path[MAXPATHL];

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	GdkPixbuf *pixbuf = ফাঁকা;
	pixbuf = gdk_pixbuf_new_from_resource(path, ফাঁকা);
	যদি (pixbuf != ফাঁকা)
	{
	    ধ্রুবক gint আকার_2 = MAX(gdk_pixbuf_get_width(pixbuf),
				  gdk_pixbuf_get_height(pixbuf));
	    যদি (আকার_2 > 16)
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
		যদি (pixbuf == ফাঁকা)
		    pixbuf = src;
		অন্যথা
		    g_object_unref(src);
	    }
	    gtk_icon_theme_add_builtin_icon(names->icon_name, আকার_2, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  অন্যথা // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * ধ্রুবক factory = gtk_icon_factory_new();
    IconNames *names;

    জন্য (names = stock_vim_icons; names->icon_name != ফাঁকা; names++)
    {
	অক্ষর path[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(path, ফাঁকা);
	যদি (pixbuf != ফাঁকা)
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

#যদি defined(FEAT_MENU)

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
    স্থির char_u *
translate_mnemonic_tag(char_u *name, পূর্ণসংখ্যা use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    পূর্ণসংখ্যা	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    যদি (name == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	যদি (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    যদি (buf != ফাঁকা)
    {
	pdest = buf;
	জন্য (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    যদি (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    অন্যথা যদি (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    অন্যথা যদি (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    অন্যথা যদি (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    ফেরত buf;
}

    স্থির শূন্য
menu_item_new(vimmenu_T *menu, GtkWidget *parent_widget)
{
    GtkWidget	*box;
    char_u	*text;
    পূর্ণসংখ্যা		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_item_new();
# যদি GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
# অন্যথা
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(parent_widget));
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((ধ্রুবক অক্ষর *)text);
    vim_free(text);

    gtk_box_pack_start(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    যদি (menu->actext != ফাঁকা && menu->actext[0] != NUL)
    {
	text = CONVERT_TO_UTF8(menu->actext);

	gtk_box_pack_end(GTK_BOX(box),
			 gtk_label_new((ধ্রুবক অক্ষর *)text),
			 FALSE, FALSE, 0);

	CONVERT_TO_UTF8_FREE(text);
    }

    gtk_container_add(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    শূন্য
gui_mch_add_menu(vimmenu_T *menu, পূর্ণসংখ্যা idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;

    যদি (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	ফেরত;
    }

    parent = menu->parent;

    যদি ((parent != ফাঁকা && parent->submenu_id == ফাঁকা)
	    || !menu_is_menubar(menu->name))
	ফেরত;

    parent_widget = (parent != ফাঁকা) ? parent->submenu_id : gui.menubar;
    menu_item_new(menu, parent_widget);

# যদি !GTK_CHECK_VERSION(3,4,0)
    // since the tearoff should always appear first, increment idx
    যদি (parent != ফাঁকা && !menu_is_popup(parent->name))
	++idx;
# endif

    gtk_menu_shell_ঢোকাও(GTK_MENU_SHELL(parent_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_set_accel_group(GTK_MENU(menu->submenu_id), gui.accel_group);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->id), menu->submenu_id);

# যদি !GTK_CHECK_VERSION(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_item_new();
    যদি (vim_strchr(p_go, GO_TEAROFF) != ফাঁকা)
	gtk_widget_show(menu->tearoff_handle);
#  যদি GTK_CHECK_VERSION(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  অন্যথা
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    স্থির শূন্য
menu_item_activate(GtkWidget *widget UNUSED, gpointer তথ্য)
{
    gui_menu_cb((vimmenu_T *)তথ্য);
}

    স্থির শূন্য
menu_item_select(GtkWidget *widget UNUSED, gpointer তথ্য)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    স্থির পূর্ণসংখ্যা	did_বার্তা = FALSE;

    যদি (State & MODE_CMDLINE)
	ফেরত;
    menu = (vimmenu_T *)তথ্য;
    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_সূচক_TIP]);
    যদি (tooltip != ফাঁকা && utf_valid_string(tooltip, ফাঁকা))
    {
	বার্তা((অক্ষর *)tooltip);
	did_বার্তা = TRUE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    অন্যথা যদি (did_বার্তা)
    {
	বার্তা("");
	did_বার্তা = FALSE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    CONVERT_TO_UTF8_FREE(tooltip);
}

    শূন্য
gui_mch_add_menu_item(vimmenu_T *menu, পূর্ণসংখ্যা idx)
{
    vimmenu_T *parent;

    parent = menu->parent;

# ifdef FEAT_TOOLBAR
    যদি (menu_is_toolbar(parent->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = ফাঁকা;

	যদি (menu_is_separator(menu->name))
	{
#  যদি GTK_CHECK_VERSION(3,0,0)
	    GtkToolItem *item = gtk_separator_tool_item_new();
	    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(item),
		    TRUE);
	    gtk_tool_item_set_expand(GTK_TOOL_ITEM(item), FALSE);
	    gtk_widget_show(GTK_WIDGET(item));

	    gtk_toolbar_ঢোকাও(toolbar, item, idx);
#  অন্যথা
	    gtk_toolbar_ঢোকাও_space(toolbar, idx);
#  endif
	    menu->id = ফাঁকা;
	}
	অন্যথা
	{
	    char_u *text;
	    char_u *tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_সূচক_TIP]);
	    যদি (tooltip != ফাঁকা && !utf_valid_string(tooltip, ফাঁকা))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_TO_UTF8_FREE(tooltip);

#  যদি GTK_CHECK_VERSION(3,0,0)
	    {
		GtkWidget *icon;
		GtkToolItem *item;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_আকার(toolbar));
		item = gtk_tool_button_new(icon, (ধ্রুবক gchar *)text);
		gtk_tool_item_set_tooltip_text(item, (ধ্রুবক gchar *)tooltip);
		g_signal_connect(G_OBJECT(item), "clicked",
			G_CALLBACK(&menu_item_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(item));

		gtk_toolbar_ঢোকাও(toolbar, item, idx);

		menu->id = GTK_WIDGET(item);
	    }
#  অন্যথা
	    menu->id = gtk_toolbar_ঢোকাও_item(
		    toolbar,
		    (ধ্রুবক অক্ষর *)text,
		    (ধ্রুবক অক্ষর *)tooltip,
		    ফাঁকা,
		    create_menu_icon(menu, gtk_toolbar_get_icon_আকার(toolbar)),
		    G_CALLBACK(&menu_item_activate),
		    menu,
		    idx);
#  endif

	    যদি (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_in_event), ফাঁকা);

	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
    }
    অন্যথা
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	যদি (parent == ফাঁকা || parent->submenu_id == ফাঁকা)
	    ফেরত;

# যদি !GTK_CHECK_VERSION(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	যদি (!menu_is_popup(parent->name))
	    ++idx;
# endif

	যদি (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# যদি GTK_CHECK_VERSION(3,0,0)
	    menu->id = gtk_separator_menu_item_new();
# অন্যথা
	    menu->id = gtk_menu_item_new();
	    gtk_widget_set_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_ঢোকাও(GTK_MENU_SHELL(parent->submenu_id),
		    menu->id, idx);

	    ফেরত;
	}

	// Add textual menu item.
	menu_item_new(menu, parent->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_ঢোকাও(GTK_MENU_SHELL(parent->submenu_id),
		menu->id, idx);

	যদি (menu->id != ফাঁকা)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_item_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_item_select), menu);
	}
    }
}
#endif // FEAT_MENU


    শূন্য
gui_mch_set_text_area_pos(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা w, পূর্ণসংখ্যা h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#যদি defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    শূন্য
gui_gtk_set_mnemonics(পূর্ণসংখ্যা enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	যদি (menu->id == ফাঁকা)
	    চালু;

	name = translate_mnemonic_tag(menu->name, enable);
	gtk_label_set_text_with_mnemonic(GTK_LABEL(menu->label),
					 (ধ্রুবক অক্ষর *)name);
	vim_free(name);
    }
}

# যদি !GTK_CHECK_VERSION(3,4,0)
    স্থির শূন্য
recurse_tearoffs(vimmenu_T *menu, পূর্ণসংখ্যা val)
{
    জন্য (; menu != ফাঁকা; menu = menu->পরবর্তী)
    {
	যদি (menu->submenu_id != ফাঁকা && menu->tearoff_handle != ফাঁকা
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    যদি (val)
		gtk_widget_show(menu->tearoff_handle);
	    অন্যথা
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# যদি GTK_CHECK_VERSION(3,4,0)
    শূন্য
gui_mch_toggle_tearoffs(পূর্ণসংখ্যা enable UNUSED)
{
    // Do nothing
}
# অন্যথা
    শূন্য
gui_mch_toggle_tearoffs(পূর্ণসংখ্যা enable)
{
    recurse_tearoffs(মূল_menu, enable);
}
# endif
#endif // FEAT_MENU

#যদি defined(FEAT_TOOLBAR)
    স্থির পূর্ণসংখ্যা
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*গ্রন্থি;
    পূর্ণসংখ্যা		idx = 0;

    জন্য (গ্রন্থি = menu->parent->children; গ্রন্থি != menu; গ্রন্থি = গ্রন্থি->পরবর্তী)
    {
	g_return_val_if_fail(গ্রন্থি != ফাঁকা, -1);
	++idx;
    }

    ফেরত idx;
}
#endif // FEAT_TOOLBAR


#যদি defined(FEAT_TOOLBAR)
    শূন্য
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    যদি (menu->id == ফাঁকা || menu->parent == ফাঁকা || gui.toolbar == ফাঁকা)
	ফেরত;

    char_u *tooltip;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_সূচক_TIP]);
    যদি (tooltip != ফাঁকা && utf_valid_string(tooltip, ফাঁকা))
# যদি GTK_CHECK_VERSION(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_set_tooltip_text(menu->id, (ধ্রুবক gchar *)tooltip);
# অন্যথা
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_set_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (ধ্রুবক অক্ষর *)tooltip, ফাঁকা);
# endif
    CONVERT_TO_UTF8_FREE(tooltip);
}
#endif // FEAT_TOOLBAR


#যদি defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    শূন্য
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    যদি (menu->id != ফাঁকা)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    যদি (menu->id != ফাঁকা && gui.menubar != ফাঁকা
			    && gtk_widget_get_parent(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    যদি (menu->parent != ফাঁকা && menu_is_toolbar(menu->parent->name))
    {
	যদি (menu_is_separator(menu->name))
#  যদি GTK_CHECK_VERSION(3,0,0)
	{
	    GtkToolItem *item = ফাঁকা;

	    item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    যদি (item != ফাঁকা)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(item));
	}
#  অন্যথা
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	অন্যথা যদি (menu->id != ফাঁকা)
	    gtk_widget_destroy(menu->id);
    }
    অন্যথা
# endif // FEAT_TOOLBAR
    {
	যদি (menu->submenu_id != ফাঁকা)
	    gtk_widget_destroy(menu->submenu_id);

	যদি (menu->id != ফাঁকা)
	    gtk_widget_destroy(menu->id);
    }

    যদি (menu->id != ফাঁকা)
	g_object_unref(menu->id);
    menu->submenu_id = ফাঁকা;
    menu->id = ফাঁকা;
}
#endif // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    শূন্য
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long আকার_2, long max)
{
    যদি (sb->id == ফাঁকা)
	ফেরত;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_set_lower(adjustment, 0.0);
    gtk_adjustment_set_মান(adjustment, val);
    gtk_adjustment_set_upper(adjustment, max + 1);
    gtk_adjustment_set_page_আকার(adjustment, আকার_2);
    gtk_adjustment_set_page_increment(adjustment,
	    আকার_2 < 3L ? 1L : আকার_2 - 2L);
    gtk_adjustment_set_step_increment(adjustment, 1.0);

    g_signal_handler_block(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#যদি !GTK_CHECK_VERSION(3,18,0)
    gtk_adjustment_changed(adjustment);
#endif

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    শূন্য
gui_mch_set_scrollbar_pos(scrollbar_T *sb, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা w, পূর্ণসংখ্যা h)
{
    যদি (sb->id != ফাঁকা)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    পূর্ণসংখ্যা
gui_mch_get_scrollbar_xpadding(শূন্য)
{
    পূর্ণসংখ্যা xpad;
#যদি GTK_CHECK_VERSION(3,0,0)
    xpad = gtk_widget_get_allocated_width(gui.formwin)
	  - gtk_widget_get_allocated_width(gui.drawarea) - gui.scrollbar_width;
#অন্যথা
    xpad = gui.formwin->allocation.width - gui.drawarea->allocation.width
							 - gui.scrollbar_width;
#endif
    যদি (gui.which_scrollbars[SBAR_বাম] && gui.which_scrollbars[SBAR_ডান])
	xpad -= gui.scrollbar_width;

    ফেরত (xpad < 0) ? 0 : xpad;
}

    পূর্ণসংখ্যা
gui_mch_get_scrollbar_ypadding(শূন্য)
{
    পূর্ণসংখ্যা ypad;
#যদি GTK_CHECK_VERSION(3,0,0)
    ypad = gtk_widget_get_allocated_height(gui.formwin)
	- gtk_widget_get_allocated_height(gui.drawarea) - gui.scrollbar_height;
#অন্যথা
    ypad = gui.formwin->allocation.height - gui.drawarea->allocation.height
							- gui.scrollbar_height;
#endif
    ফেরত (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    স্থির শূন্য
adjustment_মান_changed(GtkAdjustment *adjustment, gpointer তথ্য)
{
    scrollbar_T	*sb;
    long	মান;
    পূর্ণসংখ্যা		dragging = FALSE;

#ifdef FEAT_XIM
    // cancel any preediting
    যদি (im_is_preediting())
	xim_reset();
#endif

    sb = gui_find_scrollbar((long)তথ্য);
    মান = gtk_adjustment_get_মান(adjustment);
#যদি !GTK_CHECK_VERSION(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    যদি (sb != ফাঁকা)
    {
	dragging = TRUE;

	যদি (sb->wp != ফাঁকা && GDK_IS_DRAWABLE(sb->id->window))
	{
	    পূর্ণসংখ্যা			x;
	    পূর্ণসংখ্যা			y;
	    GdkModifierType	state;
	    পূর্ণসংখ্যা			width;
	    পূর্ণসংখ্যা			height;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_window_get_pointer(sb->id->window, &x, &y, &state);
	    gdk_window_get_আকার(sb->id->window, &width, &height);
	    যদি (x >= 0 && x < width && y >= 0 && y < height)
	    {
		যদি (y < width)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    মান = sb->wp->w_topline - 2;
		}
		অন্যথা যদি (y > height - width)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    মান = sb->wp->w_topline;
		}
	    }
	}
    }
#endif // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, মান, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    শূন্য
gui_mch_create_scrollbar(scrollbar_T *sb, পূর্ণসংখ্যা orient)
{
    যদি (orient == SBAR_HORIZ)
#যদি GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ফাঁকা);
#অন্যথা
	sb->id = gtk_hscrollbar_new(ফাঁকা);
#endif
    অন্যথা যদি (orient == SBAR_VERT)
#যদি GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ফাঁকা);
#অন্যথা
	sb->id = gtk_vscrollbar_new(ফাঁকা);
#endif

    যদি (sb->id == ফাঁকা)
	ফেরত;

    GtkAdjustment *adjustment;

    gtk_widget_set_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_মান_changed),
	    GINT_TO_POINTER(sb->ident));
    gui_mch_update();
}

    শূন্য
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    যদি (sb->id != ফাঁকা)
    {
	gtk_widget_destroy(sb->id);
	sb->id = ফাঁকা;
    }
    gui_mch_update();
}

#যদি defined(FEAT_BROWSE)
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
gui_mch_browse(পূর্ণসংখ্যা saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext UNUSED,
	       char_u *initdir,
	       char_u *filter)
{
# যদি GTK_CHECK_VERSION(3,20,0)
    GtkFileChooserNative	*fc;
# অন্যথা
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    ধ্রুবক gchar		*domain = "Gtk";

    title = CONVERT_TO_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    যদি (initdir == ফাঁকা || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    অন্যথা যদি (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    add_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_set_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, ফাঁকা);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# যদি GTK_CHECK_VERSION(3,20,0)
    fc = gtk_file_chooser_native_new(
# অন্যথা
    fc = gtk_file_chooser_dialog_new(
# endif
	    (ধ্রুবক gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# যদি GTK_CHECK_VERSION(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# অন্যথা
#  যদি GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  অন্যথা
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    ফাঁকা);
# endif
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fc),
						       (ধ্রুবক gchar *)dirbuf);

    যদি (filter != ফাঁকা && *filter != NUL)
    {
	পূর্ণসংখ্যা     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	যদি (patt != ফাঁকা)
	{
	    যখন (p != ফাঁকা && *p != NUL)
	    {
		যদি (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    যদি (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    অন্যথা
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			যদি (*p == '\n')
			{
			    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    যদি (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		অন্যথা
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    যদি (saving && dflt != ফাঁকা && *dflt != NUL)
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fc), (অক্ষর *)dflt);

    gui.browse_fname = ফাঁকা;
# যদি GTK_CHECK_VERSION(3,20,0)
    যদি (gtk_native_dialog_run(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# অন্যথা
    যদি (gtk_dialog_run(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	অক্ষর *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# যদি GTK_CHECK_VERSION(3,20,0)
    g_object_unref(fc);
# অন্যথা
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_TO_UTF8_FREE(title);
    যদি (gui.browse_fname == ফাঁকা)
	ফেরত ফাঁকা;

    // shorten the file name if possible
    ফেরত vim_strsave(shorten_fname1(gui.browse_fname));
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
# যদি defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = ফাঁকা;

    title = CONVERT_TO_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (ধ্রুবক gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  যদি GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  অন্যথা
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    ফাঁকা);

    CONVERT_TO_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    যদি (initdir == ফাঁকা || *initdir == NUL
	       || vim_FullName(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    add_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dirdlg),
						      (ধ্রুবক gchar *)dirbuf);

    // Run the dialog.
    যদি (gtk_dialog_run(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    যদি (dirname == ফাঁকা)
	ফেরত ফাঁকা;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    ফেরত p;

# অন্যথা // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    ফেরত gui_mch_browse(0, title, ফাঁকা, ফাঁকা, initdir, ফাঁকা);
# endif // !defined(GTK_FILE_CHOOSER)
}


#endif	// FEAT_BROWSE

#যদি defined(FEAT_GUI_DIALOG)

    স্থির GtkWidget *
create_message_dialog(পূর্ণসংখ্যা type, char_u *title, char_u *message)
{
    GtkWidget	    *dialog;
    GtkMessageType  message_type;

    নির্বাচন (type)
    {
	ক্ষেত্র VIM_ERROR:	    message_type = GTK_MESSAGE_ERROR;	 বিরতি;
	ক্ষেত্র VIM_WARNING:   message_type = GTK_MESSAGE_WARNING;	 বিরতি;
	ক্ষেত্র VIM_QUESTION:  message_type = GTK_MESSAGE_QUESTION; বিরতি;
	পূর্বনির্ধারিত:	    message_type = GTK_MESSAGE_INFO;	 বিরতি;
    }

    message = CONVERT_TO_UTF8(message);
    dialog  = gtk_message_dialog_new(GTK_WINDOW(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_PARENT,
				     message_type,
				     GTK_BUTTONS_NONE,
				     "%s", (ধ্রুবক অক্ষর *)message);
    CONVERT_TO_UTF8_FREE(message);

    যদি (title != ফাঁকা)
    {
	title = CONVERT_TO_UTF8(title);
	gtk_window_set_title(GTK_WINDOW(dialog), (ধ্রুবক অক্ষর *)title);
	CONVERT_TO_UTF8_FREE(title);
    }
    অন্যথা যদি (type == VIM_GENERIC)
    {
	gtk_window_set_title(GTK_WINDOW(dialog), "VIM");
    }

    ফেরত dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    স্থির অক্ষর **
split_button_string(char_u *button_string, পূর্ণসংখ্যা *n_buttons)
{
    অক্ষর	    **array;
    char_u	    *p;
    unsigned পূর্ণসংখ্যা    গুনতি = 1;

    জন্য (p = button_string; *p != NUL; ++p)
	যদি (*p == DLG_BUTTON_SEP)
	    ++গুনতি;

    array = ALLOC_MULT(অক্ষর *, গুনতি + 1);
    গুনতি = 0;

    যদি (array != ফাঁকা)
    {
	array[গুনতি++] = (অক্ষর *)button_string;
	জন্য (p = button_string; *p != NUL; )
	{
	    যদি (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[গুনতি++] = (অক্ষর *)p;
	    }
	    অন্যথা যদি (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    অন্যথা
		MB_PTR_ADV(p);
	}
	array[গুনতি] = ফাঁকা; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = গুনতি;
    ফেরত array;
}

# যদি !GTK_CHECK_VERSION(3,10,0)
    স্থির অক্ষর **
split_button_translation(ধ্রুবক অক্ষর *message)
{
    অক্ষর    **buttons = ফাঁকা;
    char_u  *str;
    পূর্ণসংখ্যা	    n_buttons = 0;
    পূর্ণসংখ্যা	    n_expected = 1;

    জন্য (str = (char_u *)message; *str != NUL; ++str)
	যদি (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(message);
    যদি (str != ফাঁকা)
    {
	যদি (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, ফাঁকা);
	অন্যথা
	    str = vim_strsave(str);

	যদি (str != ফাঁকা)
	    buttons = split_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    যদি (buttons == ফাঁকা || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = ফাঁকা;
	str = vim_strsave((char_u *)message);

	যদি (str != ফাঁকা)
	    buttons = split_button_string(str, &n_buttons);
	যদি (buttons == ফাঁকা)
	    vim_free(str);
    }

    ফেরত buttons;
}

    স্থির পূর্ণসংখ্যা
button_equal(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b)
{
    যখন (*a != '\0' && *b != '\0')
    {
	যদি (*a == '_' && *++a == '\0')
	    বিরতি;
	যদি (*b == '_' && *++b == '\0')
	    বিরতি;

	যদি (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    ফেরত FALSE;

	a = g_utf8_পরবর্তী_char(a);
	b = g_utf8_পরবর্তী_char(b);
    }

    ফেরত (*a == '\0' && *b == '\0');
}
# endif

    স্থির শূন্য
dialog_add_buttons(GtkDialog *dialog, char_u *button_string)
{
# যদি !GTK_CHECK_VERSION(3,10,0)
    অক্ষর    **ok;
    অক্ষর    **ync;  // "yes no cancel"
# endif
    অক্ষর    **buttons;
    পূর্ণসংখ্যা	    n_buttons = 0;
    পূর্ণসংখ্যা	    idx;

    button_string = vim_strsave(button_string); // must be writable
    যদি (button_string == ফাঁকা)
	ফেরত;

    // Check 'v' flag in 'guioptions': vertical button placement.
    যদি (vim_strchr(p_go, GO_VERTICAL) != ফাঁকা)
    {
# যদি !GTK_CHECK_VERSION(3,0,0)
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
# যদি !GTK_CHECK_VERSION(3,10,0)
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
    জন্য (idx = 1; idx <= n_buttons; ++idx)
    {
	অক্ষর	*label;
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
# যদি !GTK_CHECK_VERSION(3,10,0)
	যদি (ok != ফাঁকা && ync != ফাঁকা) // almost impossible to fail
	{
	    যদি	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    অন্যথা যদি (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    অন্যথা যদি (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    অন্যথা যদি (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    অন্যথা যদি (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    অন্যথা যদি (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    অন্যথা যদি (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    অন্যথা যদি (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_TO_UTF8((char_u *)label);
	gtk_dialog_add_button(dialog, (ধ্রুবক gchar *)label8, idx);
	CONVERT_TO_UTF8_FREE(label8);
    }

# যদি !GTK_CHECK_VERSION(3,10,0)
    যদি (ok != ফাঁকা)
	vim_free(*ok);
    যদি (ync != ফাঁকা)
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
প্রকারলেখ কাঠামো _DialogInfo
{
    পূর্ণসংখ্যা		ignore_enter;	    // no default button, ignore "Enter"
    পূর্ণসংখ্যা		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    স্থির gboolean
dialog_চাবি_press_event_cb(GtkWidget *widget, Gdk_Event_চাবি *event, gpointer তথ্য)
{
    DialogInfo *di = (DialogInfo *)তথ্য;

    // Ignore hitting Enter (or Space) when there is no default button.
    যদি (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	ফেরত TRUE;
    অন্যথা    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    যদি (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	ফেরত TRUE;
    }

    যদি (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_mask()) == 0)
    {
	ফেরত gtk_window_mnemonic_activate(
		   GTK_WINDOW(widget), event->keyval,
		   gtk_window_get_mnemonic_modifier(GTK_WINDOW(widget)));
    }

    ফেরত FALSE; // continue emission
}

    পূর্ণসংখ্যা
gui_mch_dialog(পূর্ণসংখ্যা	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*message,   // message text
	       char_u	*buttons,   // names of buttons
	       পূর্ণসংখ্যা	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       পূর্ণসংখ্যা	ex_cmd UNUSED)
{
    GtkWidget	*dialog;
    GtkWidget	*entry = ফাঁকা;
    char_u	*text;
    পূর্ণসংখ্যা		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_active;

    dialog = create_message_dialog(type, title, message);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_add_buttons(GTK_DIALOG(dialog), buttons);

    যদি (textfield != ফাঁকা)
    {
	GtkWidget *alignment;

	entry = gtk_entry_new();
	gtk_widget_show(entry);

	// Make Enter work like pressing OK.
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

	text = CONVERT_TO_UTF8(textfield);
	gtk_entry_set_text(GTK_ENTRY(entry), (ধ্রুবক অক্ষর *)text);
	CONVERT_TO_UTF8_FREE(text);

# যদি GTK_CHECK_VERSION(3,14,0)
	gtk_widget_set_halign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_valign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(entry), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# অন্যথা
	alignment = gtk_alignment_new((দশমিক)0.5, (দশমিক)0.5,
						      (দশমিক)1.0, (দশমিক)1.0);
# endif
	gtk_container_add(GTK_CONTAINER(alignment), entry);
	gtk_container_set_border_width(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# যদি GTK_CHECK_VERSION(3,0,0)
	{
	    GtkWidget * ধ্রুবক vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_start(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# অন্যথা
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    অন্যথা
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_চাবি_press_event_cb), &dialoginfo);

    যদি (def_but > 0)
    {
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    অন্যথা
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    যদি (response != GTK_RESPONSE_NONE)
    {
	যদি (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	যদি (textfield != ফাঁকা)
	{
	    text = (char_u *)gtk_entry_get_text(GTK_ENTRY(entry));
	    text = CONVERT_FROM_UTF8(text);

	    vim_strncpy(textfield, text, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(text);
	}
	gtk_widget_destroy(dialog);
    }

    যদি (gui.dialogs_active > 0)
	--gui.dialogs_active;
    ফেরত response > 0 ? response : 0;
}

#endif // FEAT_GUI_DIALOG


#যদি defined(FEAT_MENU)

    শূন্য
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# যদি defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  যদি !GTK_CHECK_VERSION(3,10,0)
    যদি (xic != ফাঁকা && g_object_get_তথ্য(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == ফাঁকা)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_TO_UTF8(name);
	menuitem = gtk_menu_item_new_with_mnemonic((ধ্রুবক অক্ষর *)name);
	CONVERT_TO_UTF8_FREE(name);
	gtk_widget_show(menuitem);

	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	gtk_im_multicontext_append_menuitems(GTK_IM_MULTICONTEXT(xic),
					     GTK_MENU_SHELL(submenu));
	g_object_set_তথ্য(G_OBJECT(menu->submenu_id),
			  "vim-has-im-menu", GINT_TO_POINTER(TRUE));
    }
#  endif
# endif // FEAT_XIM

# যদি GTK_CHECK_VERSION(3,22,2)
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
	trigger.axes       = ফাঁকা;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = ফাঁকা;
	trigger.x_মূল     = 0.0;
	trigger.y_মূল     = 0.0;

	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# অন্যথা
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   ফাঁকা, ফাঁকা,
		   (GtkMenuPositionFunc)ফাঁকা, ফাঁকা,
		   3U, gui.event_time);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
স্থির পূর্ণসংখ্যা popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    স্থির শূন্য
popup_menu_position_func(GtkMenu *menu UNUSED,
			 gint *x, gint *y,
			 gboolean *ঠেলো_in UNUSED,
			 gpointer user_তথ্য UNUSED)
{
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), x, y);

    যদি (popup_mouse_pos)
    {
	পূর্ণসংখ্যা	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    অন্যথা যদি (curwin != ফাঁকা && gui.drawarea != ফাঁকা &&
	     gtk_widget_get_window(gui.drawarea) != ফাঁকা)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    শূন্য
gui_make_popup(char_u *path_name, পূর্ণসংখ্যা mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_find_menu(path_name);
    যদি (menu == ফাঁকা || menu->submenu_id == ফাঁকা)
	ফেরত;

# যদি GTK_CHECK_VERSION(3,22,2)
    GdkWindow * ধ্রুবক win = gtk_widget_get_window(gui.drawarea);
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
    trigger.axes       = ফাঁকা;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = ফাঁকা;
    trigger.x_মূল     = 0.0;
    trigger.y_মূল     = 0.0;

    যদি (mouse_pos)
	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    অন্যথা
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_window_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(ফাঁকা, &rect.x, &rect.y, ফাঁকা, ফাঁকা);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# অন্যথা
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    ফাঁকা, ফাঁকা,
	    &popup_menu_position_func, ফাঁকা,
	    0U, (guint32)GDK_CURRENT_TIME);
# endif
}

#endif // FEAT_MENU


/*
 * We don't create it twice.
 */

প্রকারলেখ কাঠামো _SharedFindReplace
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

স্থির SharedFindReplace find_widgets = {ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা};
স্থির SharedFindReplace repl_widgets = {ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা};

    স্থির পূর্ণসংখ্যা
find_চাবি_press_event(
		GtkWidget	*widget UNUSED,
		Gdk_Event_চাবি	*event,
		SharedFindReplace *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    যদি (event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	ফেরত FALSE;

    // the Escape key synthesizes a cancellation action
    যদি (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	ফেরত TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    ফেরত FALSE;
}

    স্থির GtkWidget *
#যদি GTK_CHECK_VERSION(3,10,0)
create_image_button(ধ্রুবক অক্ষর *stock_id UNUSED,
		    ধ্রুবক অক্ষর *label)
#অন্যথা
create_image_button(ধ্রুবক অক্ষর *stock_id,
		    ধ্রুবক অক্ষর *label)
#endif
{
    char_u	*text;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    text = CONVERT_TO_UTF8((char_u *)label);

#যদি GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
#অন্যথা
    box = gtk_hbox_new(FALSE, 3);
#endif
#যদি !GTK_CHECK_VERSION(3,10,0)
    যদি (stock_id != ফাঁকা)
	gtk_box_pack_start(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_আকার_BUTTON),
			   FALSE, FALSE, 0);
#endif
    gtk_box_pack_start(GTK_BOX(box),
		       gtk_label_new((ধ্রুবক অক্ষর *)text),
		       FALSE, FALSE, 0);

    CONVERT_TO_UTF8_FREE(text);

#যদি GTK_CHECK_VERSION(3,14,0)
    gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#অন্যথা
    alignment = gtk_alignment_new((দশমিক)0.5, (দশমিক)0.5,
						      (দশমিক)0.0, (দশমিক)0.0);
#endif
    gtk_container_add(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), alignment);

    ফেরত button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    স্থির ধ্রুবক অক্ষর *
convert_localized_message(char_u **বাফার, ধ্রুবক অক্ষর *message)
{
    যদি (output_conv.vc_type == CONV_NONE)
	ফেরত message;

    vim_free(*বাফার);
    *বাফার = string_convert(&output_conv, (char_u *)message, ফাঁকা);

    ফেরত (ধ্রুবক অক্ষর *)*বাফার;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    স্থির unsigned long
entry_get_text_দৈর্ঘ্য(GtkEntry *entry)
{
    g_return_val_if_fail(entry != ফাঁকা, 0);
    g_return_val_if_fail(GTK_IS_ENTRY(entry) == TRUE, 0);

#যদি GTK_CHECK_VERSION(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    ফেরত gtk_entry_বাফার_get_দৈর্ঘ্য(gtk_entry_get_বাফার(entry));
#নতুবা_যদি GTK_CHECK_VERSION(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    ফেরত gtk_entry_get_text_দৈর্ঘ্য(entry);
#অন্যথা
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    ফেরত g_utf8_strlen(gtk_entry_get_text(entry), -1);
#endif
}

    স্থির শূন্য
find_replace_dialog_create(char_u *arg, পূর্ণসংখ্যা do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*table;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    SharedFindReplace *frdp;
    char_u	*entry_text;
    পূর্ণসংখ্যা		wword = FALSE;
    পূর্ণসংখ্যা		mcase = !p_ic;
    char_u	*conv_বাফার = ফাঁকা;
#define CONV(message) convert_localized_message(&conv_বাফার, (message))

    frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    যদি (entry_text != ফাঁকা && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_text = entry_text;
	entry_text = string_convert(&output_conv, entry_text, ফাঁকা);
	vim_free(old_text);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    যদি (frdp->dialog)
    {
	যদি (entry_text != ফাঁকা)
	{
	    gtk_entry_set_text(GTK_ENTRY(frdp->what), (অক্ষর *)entry_text);
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
	যদি (do_replace && entry_get_text_দৈর্ঘ্য(GTK_ENTRY(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	ফেরত;
    }

    frdp->dialog = gtk_dialog_new();
#যদি GTK_CHECK_VERSION(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#অন্যথা
    gtk_dialog_set_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#endif
    gtk_window_set_transient_for(GTK_WINDOW(frdp->dialog), GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(frdp->dialog), TRUE);

    যদি (do_replace)
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    অন্যথা
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#যদি GTK_CHECK_VERSION(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
#অন্যথা
    hbox = gtk_hbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
#যদি GTK_CHECK_VERSION(3,0,0)
    {
	GtkWidget * ধ্রুবক dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    }
#অন্যথা
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#endif

    যদি (do_replace)
#যদি GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#অন্যথা
	table = gtk_table_new(1024, 4, FALSE);
#endif
    অন্যথা
#যদি GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#অন্যথা
	table = gtk_table_new(1024, 3, FALSE);
#endif
    gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(table), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#যদি GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#নতুবা_যদি GTK_CHECK_VERSION(3,14,0)
    {
	GValue align_val = G_মান_INIT;

	g_মান_init(&align_val, G_TYPE_FLOAT);

	g_মান_set_float(&align_val, 0.0);
	g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	g_মান_set_float(&align_val, 0.5);
	g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	g_মান_unset(&align_val);
    }
#অন্যথা
    gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#যদি GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), tmp, 0, 0, 2, 1);
#অন্যথা
    gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    frdp->what = gtk_entry_new();
    sensitive = (entry_text != ফাঁকা && entry_text[0] != NUL);
    যদি (entry_text != ফাঁকা)
	gtk_entry_set_text(GTK_ENTRY(frdp->what), (অক্ষর *)entry_text);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(entry_changed_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(find_চাবি_press_event),
			   (gpointer) frdp);
#যদি GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), frdp->what, 2, 0, 5, 1);
#অন্যথা
    gtk_table_attach(GTK_TABLE(table), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    যদি (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#যদি GTK_CHECK_VERSION(3,16,0)
	gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#নতুবা_যদি GTK_CHECK_VERSION(3,14,0)
	{
	    GValue align_val = G_মান_INIT;

	    g_মান_init(&align_val, G_TYPE_FLOAT);

	    g_মান_set_float(&align_val, 0.0);
	    g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_মান_set_float(&align_val, 0.5);
	    g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_মান_unset(&align_val);
	}
#অন্যথা
	gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 0, 1, 2, 1);
#অন্যথা
	gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
	frdp->with = gtk_entry_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(find_চাবি_press_event),
			       (gpointer) frdp);
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->with, 2, 1, 5, 1);
#অন্যথা
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
    অন্যথা
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
    যদি (do_replace)
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 2, 5, 1);
#অন্যথা
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    অন্যথা
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 3, 5, 1);
#অন্যথা
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    যদি (do_replace)
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 3, 5, 1);
#অন্যথা
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    অন্যথা
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 4, 5, 1);
#অন্যথা
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    tmp = gtk_frame_new(CONV(_("Direction")));
    যদি (do_replace)
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 2, 4);
#অন্যথা
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
    অন্যথা
#যদি GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 1, 3);
#অন্যথা
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
#যদি GTK_CHECK_VERSION(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
#অন্যথা
    vbox = gtk_vbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
    gtk_container_add(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->up = gtk_radio_button_new_with_label(ফাঁকা, CONV(_("Up")));
    gtk_box_pack_start(GTK_BOX(vbox), frdp->up, TRUE, TRUE, 0);
    frdp->down = gtk_radio_button_new_with_label(
			gtk_radio_button_get_group(GTK_RADIO_BUTTON(frdp->up)),
			CONV(_("Down")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->down), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_start(GTK_BOX(vbox), frdp->down, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#যদি GTK_CHECK_VERSION(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#অন্যথা
    actionarea = gtk_vbutton_box_new();
#endif
    gtk_container_set_border_width(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_end(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#যদি GTK_CHECK_VERSION(3,10,0)
    frdp->find = create_image_button(ফাঁকা, _("Find Next"));
#অন্যথা
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

    যদি (do_replace)
    {
	// 'Replace' button
#যদি GTK_CHECK_VERSION(3,10,0)
	frdp->replace = create_image_button(ফাঁকা, _("Replace"));
#অন্যথা
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#endif
	gtk_widget_set_sensitive(frdp->replace, sensitive);
	gtk_widget_set_can_default(frdp->find, TRUE);
	gtk_box_pack_start(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_REPLACE));

	// 'Replace All' button
#যদি GTK_CHECK_VERSION(3,10,0)
	frdp->all = create_image_button(ফাঁকা, _("Replace All"));
#অন্যথা
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
#যদি GTK_CHECK_VERSION(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#অন্যথা
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

#যদি GTK_CHECK_VERSION(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#অন্যথা
    tmp = gtk_vseparator_new();
#endif
    gtk_box_pack_end(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#যদি GTK_CHECK_VERSION(3,0,0)
# যদি !GTK_CHECK_VERSION(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#অন্যথা
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#endif
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(entry_text);
    vim_free(conv_বাফার);
#undef CONV
}

    শূন্য
gui_mch_find_dialog(exarg_T *eap)
{
    যদি (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    শূন্য
gui_mch_replace_dialog(exarg_T *eap)
{
    যদি (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    স্থির শূন্য
find_replace_cb(GtkWidget *widget UNUSED, gpointer তথ্য)
{
    পূর্ণসংখ্যা			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = (পূর্ণসংখ্যা)(long)তথ্য;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    যদি (flags == FRD_FINDNEXT)
    {
	repl_text = ফাঁকা;
	sfr = &find_widgets;
    }
    অন্যথা
    {
	repl_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(sfr->what));
    direction_down = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->down));

    যদি (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    যদি (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->mcase)))
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
    স্থির শূন্য
entry_activate_cb(GtkWidget *widget UNUSED, gpointer তথ্য)
{
    gtk_widget_grab_focus(GTK_WIDGET(তথ্য));
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
    স্থির শূন্য
entry_changed_cb(GtkWidget * entry, GtkWidget * dialog)
{
    ধ্রুবক gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

    যদি (!entry_text)
	ফেরত;			// shouldn't happen

    nonempty = (entry_text[0] != '\0');

    যদি (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    যদি (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    শূন্য
ex_helpfind(exarg_T *eap UNUSED)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#যদি defined(FEAT_BROWSE)
    স্থির শূন্য
recent_func_log_func(ধ্রুবক gchar *log_domain UNUSED,
		     GLogLevelFlags log_level UNUSED,
		     ধ্রুবক gchar *message UNUSED,
		     gpointer user_তথ্য UNUSED)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#endif

    শূন্য
gui_mch_set_fullscreen(পূর্ণসংখ্যা flag)
{
    যদি (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    অন্যথা
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}
