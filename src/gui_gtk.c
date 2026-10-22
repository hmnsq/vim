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

#अन्तर्भाव "vim.h"

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
# यदि defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#endif

#ifdef FEAT_GUI_GTK
# यदि GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# अन्यथा
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# अन्यथा
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#अन्यथा
// define these items to be able to generate prototypes without GTK
प्रकारनिर्देश पूर्णाङ्क GtkWidget;
# define gpointer पूर्णाङ्क
# define guint8 पूर्णाङ्क
# define GdkPixmap पूर्णाङ्क
# define GdkBitmap पूर्णाङ्क
# define GtkIconFactory पूर्णाङ्क
# define GtkToolbar पूर्णाङ्क
# define GtkAdjustment पूर्णाङ्क
# define gboolean पूर्णाङ्क
# define Gdk_Event_किल्ली पूर्णाङ्क
# define CancelData पूर्णाङ्क
#endif

स्थिर शून्य entry_activate_cb(GtkWidget *widget, gpointer data);
स्थिर शून्य entry_changed_cb(GtkWidget *entry, GtkWidget *dialog);
स्थिर शून्य find_replace_cb(GtkWidget *widget, gpointer data);
#यदि defined(FEAT_BROWSE)
स्थिर शून्य recent_func_log_func(
	ध्रुव gchar *log_domain,
	GLogLevelFlags log_level,
	ध्रुव gchar *message,
	gpointer user_data);
#endif

#यदि defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# यदि GTK_CHECK_VERSION(3,10,0)
स्थिर ध्रुव वर्ण * ध्रुव menu_themed_names[] =
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
#  यदि GTK_CHECK_VERSION(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  अन्यथा
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  यदि GTK_CHECK_VERSION(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  अन्यथा
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  यदि GTK_CHECK_VERSION(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  अन्यथा
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  यदि GTK_CHECK_VERSION(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  अन्यथा
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# अन्यथा // !GTK_CHECK_VERSION(3,10,0)
स्थिर ध्रुव वर्ण * ध्रुव menu_stock_ids[] =
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
#  यदि !GTK_CHECK_VERSION(3,10,0)
प्रकारनिर्देश संरचना IconNames {
    ध्रुव वर्ण *icon_name;
    ध्रुव वर्ण *file_name;
} IconNames;

स्थिर IconNames stock_vim_icons[] = {
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
    { रिक्त, रिक्त }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    स्थिर शून्य
add_stock_icon(GtkIconFactory	*factory,
	       ध्रुव वर्ण	*stock_id,
	       ध्रुव guint8	*inline_data,
	       पूर्णाङ्क		data_length)
{
    GdkPixbuf	*pixbuf;
    GtkIconSet	*icon_set;

    pixbuf = gdk_pixbuf_new_from_inline(data_length, inline_data, FALSE, रिक्त);
    icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);

    gtk_icon_factory_add(factory, stock_id, icon_set);

    gtk_icon_set_unref(icon_set);
    g_object_unref(pixbuf);
}
# endif

    स्थिर पूर्णाङ्क
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    यदि (mch_isFullName(dest))
    {
	प्रत्यागम vim_fexists(dest);
    }
    अन्यथा
    {
	स्थिर ध्रुव वर्ण   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	unsigned पूर्णाङ्क	    i;

	प्रति (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    यदि (gui_find_bitmap(dest, buf, (वर्ण *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		प्रत्यागम TRUE;
	    }

	प्रत्यागम FALSE;
    }
}

    स्थिर GtkWidget *
load_menu_iconfile(char_u *name, GtkIconSize icon_size)
{
    GtkWidget	    *image = रिक्त;
# यदि GTK_CHECK_VERSION(3,10,0)
    पूर्णाङ्क		     pixel_size = -1;

    विकल्प (icon_size)
    {
	पक्ष GTK_ICON_SIZE_MENU:
	    pixel_size = 16;
	    भङ्ग;
	पक्ष GTK_ICON_SIZE_SMALL_TOOLBAR:
	    pixel_size = 16;
	    भङ्ग;
	पक्ष GTK_ICON_SIZE_LARGE_TOOLBAR:
	    pixel_size = 24;
	    भङ्ग;
	पक्ष GTK_ICON_SIZE_BUTTON:
	    pixel_size = 16;
	    भङ्ग;
	पक्ष GTK_ICON_SIZE_DND:
	    pixel_size = 32;
	    भङ्ग;
	पक्ष GTK_ICON_SIZE_DIALOG:
	    pixel_size = 48;
	    भङ्ग;
	पक्ष GTK_ICON_SIZE_INVALID:
	    // FALLTHROUGH
	स्वभाव:
	    pixel_size = 0;
	    भङ्ग;
    }

    यदि (pixel_size > 0 || pixel_size == -1)
    {
	GdkPixbuf * ध्रुव pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((ध्रुव वर्ण *)name,
		pixel_size, pixel_size, TRUE, रिक्त);
	यदि (pixbuf != रिक्त)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    यदि (image == रिक्त)
	image = gtk_image_new_from_icon_name("image-missing", icon_size);

    प्रत्यागम image;
# अन्यथा // !GTK_CHECK_VERSION(3,10,0)
    GtkIconSet	    *icon_set;
    GtkIconSource   *icon_source;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_set = gtk_icon_set_new();
    icon_source = gtk_icon_source_new();

    gtk_icon_source_set_filename(icon_source, (ध्रुव वर्ण *)name);
    gtk_icon_set_add_source(icon_set, icon_source);

    image = gtk_image_new_from_icon_set(icon_set, icon_size);

    gtk_icon_source_free(icon_source);
    gtk_icon_set_unref(icon_set);

    प्रत्यागम image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    स्थिर GtkWidget *
create_menu_icon(vimmenu_T *menu, GtkIconSize icon_size)
{
    GtkWidget	*image = रिक्त;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    यदि (menu->iconfile != रिक्त && lookup_menu_iconfile(menu->iconfile, buf))
	image = load_menu_iconfile(buf, icon_size);

    // If not found and not builtin specified try using the menu name.
    यदि (image == रिक्त && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = load_menu_iconfile(buf, icon_size);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    यदि (image == रिक्त)
    {
# यदि GTK_CHECK_VERSION(3,10,0)
	ध्रुव वर्ण *icon_name = रिक्त;
	ध्रुव पूर्णाङ्क   n_names = G_N_ELEMENTS(menu_themed_names);

	यदि (menu->iconidx >= 0 && menu->iconidx < n_names)
	    icon_name = menu_themed_names[menu->iconidx];
	यदि (icon_name == रिक्त)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_size);
# अन्यथा
	ध्रुव वर्ण  *stock_id;
	ध्रुव पूर्णाङ्क   n_ids = G_N_ELEMENTS(menu_stock_ids);

	यदि (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	अन्यथा
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_size);
# endif
    }

    प्रत्यागम image;
}

    स्थिर gint
toolbar_button_focus_in_event(GtkWidget *widget UNUSED,
			      GdkEventFocus *event UNUSED,
			      gpointer data UNUSED)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    यदि (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    प्रत्यागम TRUE;
}
#endif // FEAT_TOOLBAR

#यदि defined(FEAT_TOOLBAR)

    शून्य
gui_gtk_register_stock_icons(शून्य)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define ADD_ICON(Name, Data) add_stock_icon(factory, Name, Data, (पूर्णाङ्क)परिमाण(Data))

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
# अन्यथा // defined(USE_GRESOURCE)
    ध्रुव वर्ण * ध्रुव path_prefix = "/org/vim/gui/icon";
#  यदि GTK_CHECK_VERSION(3,14,0)
    GdkScreen    *screen = रिक्त;
    GtkIconTheme *icon_theme = रिक्त;

    यदि (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    अन्यथा
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_add_resource_path(icon_theme, path_prefix);
#  elif GTK_CHECK_VERSION(3,0,0)
    IconNames *names;

    प्रति (names = stock_vim_icons; names->icon_name != रिक्त; names++)
    {
	वर्ण path[MAXPATHL];

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	GdkPixbuf *pixbuf = रिक्त;
	pixbuf = gdk_pixbuf_new_from_resource(path, रिक्त);
	यदि (pixbuf != रिक्त)
	{
	    ध्रुव gint size = MAX(gdk_pixbuf_get_width(pixbuf),
				  gdk_pixbuf_get_height(pixbuf));
	    यदि (size > 16)
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
		यदि (pixbuf == रिक्त)
		    pixbuf = src;
		अन्यथा
		    g_object_unref(src);
	    }
	    gtk_icon_theme_add_builtin_icon(names->icon_name, size, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  अन्यथा // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * ध्रुव factory = gtk_icon_factory_new();
    IconNames *names;

    प्रति (names = stock_vim_icons; names->icon_name != रिक्त; names++)
    {
	वर्ण path[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(path, MAXPATHL, "%s/%s", path_prefix, names->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(path, रिक्त);
	यदि (pixbuf != रिक्त)
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

#यदि defined(FEAT_MENU)

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
    स्थिर char_u *
translate_mnemonic_tag(char_u *name, पूर्णाङ्क use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    पूर्णाङ्क	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    यदि (name == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	यदि (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    यदि (buf != रिक्त)
    {
	pdest = buf;
	प्रति (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    यदि (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    अन्यथा यदि (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    अन्यथा यदि (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    अन्यथा यदि (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    प्रत्यागम buf;
}

    स्थिर शून्य
menu_item_new(vimmenu_T *menu, GtkWidget *parent_widget)
{
    GtkWidget	*box;
    char_u	*text;
    पूर्णाङ्क		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_item_new();
# यदि GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
# अन्यथा
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(parent_widget));
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((ध्रुव वर्ण *)text);
    vim_free(text);

    gtk_box_pack_start(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    यदि (menu->actext != रिक्त && menu->actext[0] != NUL)
    {
	text = CONVERT_TO_UTF8(menu->actext);

	gtk_box_pack_end(GTK_BOX(box),
			 gtk_label_new((ध्रुव वर्ण *)text),
			 FALSE, FALSE, 0);

	CONVERT_TO_UTF8_FREE(text);
    }

    gtk_container_add(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    शून्य
gui_mch_add_menu(vimmenu_T *menu, पूर्णाङ्क idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;

    यदि (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	प्रत्यागम;
    }

    parent = menu->parent;

    यदि ((parent != रिक्त && parent->submenu_id == रिक्त)
	    || !menu_is_menubar(menu->name))
	प्रत्यागम;

    parent_widget = (parent != रिक्त) ? parent->submenu_id : gui.menubar;
    menu_item_new(menu, parent_widget);

# यदि !GTK_CHECK_VERSION(3,4,0)
    // since the tearoff should always appear first, increment idx
    यदि (parent != रिक्त && !menu_is_popup(parent->name))
	++idx;
# endif

    gtk_menu_shell_घाला(GTK_MENU_SHELL(parent_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_set_accel_group(GTK_MENU(menu->submenu_id), gui.accel_group);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->id), menu->submenu_id);

# यदि !GTK_CHECK_VERSION(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_item_new();
    यदि (vim_strchr(p_go, GO_TEAROFF) != रिक्त)
	gtk_widget_show(menu->tearoff_handle);
#  यदि GTK_CHECK_VERSION(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  अन्यथा
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    स्थिर शून्य
menu_item_activate(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    स्थिर शून्य
menu_item_select(GtkWidget *widget UNUSED, gpointer data)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    स्थिर पूर्णाङ्क	did_संदेश = FALSE;

    यदि (State & MODE_CMDLINE)
	प्रत्यागम;
    menu = (vimmenu_T *)data;
    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    यदि (tooltip != रिक्त && utf_valid_string(tooltip, रिक्त))
    {
	संदेश((वर्ण *)tooltip);
	did_संदेश = TRUE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    अन्यथा यदि (did_संदेश)
    {
	संदेश("");
	did_संदेश = FALSE;
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    CONVERT_TO_UTF8_FREE(tooltip);
}

    शून्य
gui_mch_add_menu_item(vimmenu_T *menu, पूर्णाङ्क idx)
{
    vimmenu_T *parent;

    parent = menu->parent;

# ifdef FEAT_TOOLBAR
    यदि (menu_is_toolbar(parent->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = रिक्त;

	यदि (menu_is_separator(menu->name))
	{
#  यदि GTK_CHECK_VERSION(3,0,0)
	    GtkToolItem *item = gtk_separator_tool_item_new();
	    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(item),
		    TRUE);
	    gtk_tool_item_set_expand(GTK_TOOL_ITEM(item), FALSE);
	    gtk_widget_show(GTK_WIDGET(item));

	    gtk_toolbar_घाला(toolbar, item, idx);
#  अन्यथा
	    gtk_toolbar_घाला_space(toolbar, idx);
#  endif
	    menu->id = रिक्त;
	}
	अन्यथा
	{
	    char_u *text;
	    char_u *tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    यदि (tooltip != रिक्त && !utf_valid_string(tooltip, रिक्त))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_TO_UTF8_FREE(tooltip);

#  यदि GTK_CHECK_VERSION(3,0,0)
	    {
		GtkWidget *icon;
		GtkToolItem *item;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_size(toolbar));
		item = gtk_tool_button_new(icon, (ध्रुव gchar *)text);
		gtk_tool_item_set_tooltip_text(item, (ध्रुव gchar *)tooltip);
		g_signal_connect(G_OBJECT(item), "clicked",
			G_CALLBACK(&menu_item_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(item));

		gtk_toolbar_घाला(toolbar, item, idx);

		menu->id = GTK_WIDGET(item);
	    }
#  अन्यथा
	    menu->id = gtk_toolbar_घाला_item(
		    toolbar,
		    (ध्रुव वर्ण *)text,
		    (ध्रुव वर्ण *)tooltip,
		    रिक्त,
		    create_menu_icon(menu, gtk_toolbar_get_icon_size(toolbar)),
		    G_CALLBACK(&menu_item_activate),
		    menu,
		    idx);
#  endif

	    यदि (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_in_event), रिक्त);

	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
    }
    अन्यथा
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	यदि (parent == रिक्त || parent->submenu_id == रिक्त)
	    प्रत्यागम;

# यदि !GTK_CHECK_VERSION(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	यदि (!menu_is_popup(parent->name))
	    ++idx;
# endif

	यदि (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# यदि GTK_CHECK_VERSION(3,0,0)
	    menu->id = gtk_separator_menu_item_new();
# अन्यथा
	    menu->id = gtk_menu_item_new();
	    gtk_widget_set_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_घाला(GTK_MENU_SHELL(parent->submenu_id),
		    menu->id, idx);

	    प्रत्यागम;
	}

	// Add textual menu item.
	menu_item_new(menu, parent->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_घाला(GTK_MENU_SHELL(parent->submenu_id),
		menu->id, idx);

	यदि (menu->id != रिक्त)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_item_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_item_select), menu);
	}
    }
}
#endif // FEAT_MENU


    शून्य
gui_mch_set_text_area_pos(पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#यदि defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    शून्य
gui_gtk_set_mnemonics(पूर्णाङ्क enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	यदि (menu->id == रिक्त)
	    अनुवर्त;

	name = translate_mnemonic_tag(menu->name, enable);
	gtk_label_set_text_with_mnemonic(GTK_LABEL(menu->label),
					 (ध्रुव वर्ण *)name);
	vim_free(name);
    }
}

# यदि !GTK_CHECK_VERSION(3,4,0)
    स्थिर शून्य
recurse_tearoffs(vimmenu_T *menu, पूर्णाङ्क val)
{
    प्रति (; menu != रिक्त; menu = menu->next)
    {
	यदि (menu->submenu_id != रिक्त && menu->tearoff_handle != रिक्त
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    यदि (val)
		gtk_widget_show(menu->tearoff_handle);
	    अन्यथा
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# यदि GTK_CHECK_VERSION(3,4,0)
    शून्य
gui_mch_toggle_tearoffs(पूर्णाङ्क enable UNUSED)
{
    // Do nothing
}
# अन्यथा
    शून्य
gui_mch_toggle_tearoffs(पूर्णाङ्क enable)
{
    recurse_tearoffs(मूळ_menu, enable);
}
# endif
#endif // FEAT_MENU

#यदि defined(FEAT_TOOLBAR)
    स्थिर पूर्णाङ्क
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*गाठ;
    पूर्णाङ्क		idx = 0;

    प्रति (गाठ = menu->parent->children; गाठ != menu; गाठ = गाठ->next)
    {
	g_return_val_if_fail(गाठ != रिक्त, -1);
	++idx;
    }

    प्रत्यागम idx;
}
#endif // FEAT_TOOLBAR


#यदि defined(FEAT_TOOLBAR)
    शून्य
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    यदि (menu->id == रिक्त || menu->parent == रिक्त || gui.toolbar == रिक्त)
	प्रत्यागम;

    char_u *tooltip;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    यदि (tooltip != रिक्त && utf_valid_string(tooltip, रिक्त))
# यदि GTK_CHECK_VERSION(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_set_tooltip_text(menu->id, (ध्रुव gchar *)tooltip);
# अन्यथा
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_set_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (ध्रुव वर्ण *)tooltip, रिक्त);
# endif
    CONVERT_TO_UTF8_FREE(tooltip);
}
#endif // FEAT_TOOLBAR


#यदि defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    शून्य
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    यदि (menu->id != रिक्त)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    यदि (menu->id != रिक्त && gui.menubar != रिक्त
			    && gtk_widget_get_parent(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    यदि (menu->parent != रिक्त && menu_is_toolbar(menu->parent->name))
    {
	यदि (menu_is_separator(menu->name))
#  यदि GTK_CHECK_VERSION(3,0,0)
	{
	    GtkToolItem *item = रिक्त;

	    item = gtk_toolbar_get_nth_item(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    यदि (item != रिक्त)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(item));
	}
#  अन्यथा
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	अन्यथा यदि (menu->id != रिक्त)
	    gtk_widget_destroy(menu->id);
    }
    अन्यथा
# endif // FEAT_TOOLBAR
    {
	यदि (menu->submenu_id != रिक्त)
	    gtk_widget_destroy(menu->submenu_id);

	यदि (menu->id != रिक्त)
	    gtk_widget_destroy(menu->id);
    }

    यदि (menu->id != रिक्त)
	g_object_unref(menu->id);
    menu->submenu_id = रिक्त;
    menu->id = रिक्त;
}
#endif // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    शून्य
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    यदि (sb->id == रिक्त)
	प्रत्यागम;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_set_lower(adjustment, 0.0);
    gtk_adjustment_set_मूल्य(adjustment, val);
    gtk_adjustment_set_upper(adjustment, max + 1);
    gtk_adjustment_set_page_size(adjustment, size);
    gtk_adjustment_set_page_increment(adjustment,
	    size < 3L ? 1L : size - 2L);
    gtk_adjustment_set_step_increment(adjustment, 1.0);

    g_signal_handler_block(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#यदि !GTK_CHECK_VERSION(3,18,0)
    gtk_adjustment_changed(adjustment);
#endif

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    शून्य
gui_mch_set_scrollbar_pos(scrollbar_T *sb, पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    यदि (sb->id != रिक्त)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    पूर्णाङ्क
gui_mch_get_scrollbar_xpadding(शून्य)
{
    पूर्णाङ्क xpad;
#यदि GTK_CHECK_VERSION(3,0,0)
    xpad = gtk_widget_get_allocated_width(gui.formwin)
	  - gtk_widget_get_allocated_width(gui.drawarea) - gui.scrollbar_width;
#अन्यथा
    xpad = gui.formwin->allocation.width - gui.drawarea->allocation.width
							 - gui.scrollbar_width;
#endif
    यदि (gui.which_scrollbars[SBAR_डावा] && gui.which_scrollbars[SBAR_उजवा])
	xpad -= gui.scrollbar_width;

    प्रत्यागम (xpad < 0) ? 0 : xpad;
}

    पूर्णाङ्क
gui_mch_get_scrollbar_ypadding(शून्य)
{
    पूर्णाङ्क ypad;
#यदि GTK_CHECK_VERSION(3,0,0)
    ypad = gtk_widget_get_allocated_height(gui.formwin)
	- gtk_widget_get_allocated_height(gui.drawarea) - gui.scrollbar_height;
#अन्यथा
    ypad = gui.formwin->allocation.height - gui.drawarea->allocation.height
							- gui.scrollbar_height;
#endif
    प्रत्यागम (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    स्थिर शून्य
adjustment_मूल्य_changed(GtkAdjustment *adjustment, gpointer data)
{
    scrollbar_T	*sb;
    long	मूल्य;
    पूर्णाङ्क		dragging = FALSE;

#ifdef FEAT_XIM
    // cancel any preediting
    यदि (im_is_preediting())
	xim_reset();
#endif

    sb = gui_find_scrollbar((long)data);
    मूल्य = gtk_adjustment_get_मूल्य(adjustment);
#यदि !GTK_CHECK_VERSION(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    यदि (sb != रिक्त)
    {
	dragging = TRUE;

	यदि (sb->wp != रिक्त && GDK_IS_DRAWABLE(sb->id->window))
	{
	    पूर्णाङ्क			x;
	    पूर्णाङ्क			y;
	    GdkModifierType	state;
	    पूर्णाङ्क			width;
	    पूर्णाङ्क			height;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_window_get_pointer(sb->id->window, &x, &y, &state);
	    gdk_window_get_size(sb->id->window, &width, &height);
	    यदि (x >= 0 && x < width && y >= 0 && y < height)
	    {
		यदि (y < width)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    मूल्य = sb->wp->w_topline - 2;
		}
		अन्यथा यदि (y > height - width)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    मूल्य = sb->wp->w_topline;
		}
	    }
	}
    }
#endif // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, मूल्य, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    शून्य
gui_mch_create_scrollbar(scrollbar_T *sb, पूर्णाङ्क orient)
{
    यदि (orient == SBAR_HORIZ)
#यदि GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, रिक्त);
#अन्यथा
	sb->id = gtk_hscrollbar_new(रिक्त);
#endif
    अन्यथा यदि (orient == SBAR_VERT)
#यदि GTK_CHECK_VERSION(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, रिक्त);
#अन्यथा
	sb->id = gtk_vscrollbar_new(रिक्त);
#endif

    यदि (sb->id == रिक्त)
	प्रत्यागम;

    GtkAdjustment *adjustment;

    gtk_widget_set_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_मूल्य_changed),
	    GINT_TO_POINTER(sb->ident));
    gui_mch_update();
}

    शून्य
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    यदि (sb->id != रिक्त)
    {
	gtk_widget_destroy(sb->id);
	sb->id = रिक्त;
    }
    gui_mch_update();
}

#यदि defined(FEAT_BROWSE)
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
gui_mch_browse(पूर्णाङ्क saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext UNUSED,
	       char_u *initdir,
	       char_u *filter)
{
# यदि GTK_CHECK_VERSION(3,20,0)
    GtkFileChooserNative	*fc;
# अन्यथा
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    ध्रुव gchar		*domain = "Gtk";

    title = CONVERT_TO_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    यदि (initdir == रिक्त || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    अन्यथा यदि (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    add_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_set_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, रिक्त);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# यदि GTK_CHECK_VERSION(3,20,0)
    fc = gtk_file_chooser_native_new(
# अन्यथा
    fc = gtk_file_chooser_dialog_new(
# endif
	    (ध्रुव gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# यदि GTK_CHECK_VERSION(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# अन्यथा
#  यदि GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  अन्यथा
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    रिक्त);
# endif
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fc),
						       (ध्रुव gchar *)dirbuf);

    यदि (filter != रिक्त && *filter != NUL)
    {
	पूर्णाङ्क     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	यदि (patt != रिक्त)
	{
	    यावत् (p != रिक्त && *p != NUL)
	    {
		यदि (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    यदि (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    अन्यथा
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			यदि (*p == '\n')
			{
			    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    यदि (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		अन्यथा
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    यदि (saving && dflt != रिक्त && *dflt != NUL)
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(fc), (वर्ण *)dflt);

    gui.browse_fname = रिक्त;
# यदि GTK_CHECK_VERSION(3,20,0)
    यदि (gtk_native_dialog_run(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# अन्यथा
    यदि (gtk_dialog_run(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	वर्ण *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# यदि GTK_CHECK_VERSION(3,20,0)
    g_object_unref(fc);
# अन्यथा
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_TO_UTF8_FREE(title);
    यदि (gui.browse_fname == रिक्त)
	प्रत्यागम रिक्त;

    // shorten the file name if possible
    प्रत्यागम vim_strsave(shorten_fname1(gui.browse_fname));
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
# यदि defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = रिक्त;

    title = CONVERT_TO_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (ध्रुव gchar *)title,
	    GTK_WINDOW(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  यदि GTK_CHECK_VERSION(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  अन्यथा
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    रिक्त);

    CONVERT_TO_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    यदि (initdir == रिक्त || *initdir == NUL
	       || vim_FullName(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    add_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dirdlg),
						      (ध्रुव gchar *)dirbuf);

    // Run the dialog.
    यदि (gtk_dialog_run(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    यदि (dirname == रिक्त)
	प्रत्यागम रिक्त;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    प्रत्यागम p;

# अन्यथा // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    प्रत्यागम gui_mch_browse(0, title, रिक्त, रिक्त, initdir, रिक्त);
# endif // !defined(GTK_FILE_CHOOSER)
}


#endif	// FEAT_BROWSE

#यदि defined(FEAT_GUI_DIALOG)

    स्थिर GtkWidget *
create_message_dialog(पूर्णाङ्क type, char_u *title, char_u *message)
{
    GtkWidget	    *dialog;
    GtkMessageType  message_type;

    विकल्प (type)
    {
	पक्ष VIM_ERROR:	    message_type = GTK_MESSAGE_ERROR;	 भङ्ग;
	पक्ष VIM_WARNING:   message_type = GTK_MESSAGE_WARNING;	 भङ्ग;
	पक्ष VIM_QUESTION:  message_type = GTK_MESSAGE_QUESTION; भङ्ग;
	स्वभाव:	    message_type = GTK_MESSAGE_INFO;	 भङ्ग;
    }

    message = CONVERT_TO_UTF8(message);
    dialog  = gtk_message_dialog_new(GTK_WINDOW(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_PARENT,
				     message_type,
				     GTK_BUTTONS_NONE,
				     "%s", (ध्रुव वर्ण *)message);
    CONVERT_TO_UTF8_FREE(message);

    यदि (title != रिक्त)
    {
	title = CONVERT_TO_UTF8(title);
	gtk_window_set_title(GTK_WINDOW(dialog), (ध्रुव वर्ण *)title);
	CONVERT_TO_UTF8_FREE(title);
    }
    अन्यथा यदि (type == VIM_GENERIC)
    {
	gtk_window_set_title(GTK_WINDOW(dialog), "VIM");
    }

    प्रत्यागम dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    स्थिर वर्ण **
split_button_string(char_u *button_string, पूर्णाङ्क *n_buttons)
{
    वर्ण	    **array;
    char_u	    *p;
    unsigned पूर्णाङ्क    मोजणी = 1;

    प्रति (p = button_string; *p != NUL; ++p)
	यदि (*p == DLG_BUTTON_SEP)
	    ++मोजणी;

    array = ALLOC_MULT(वर्ण *, मोजणी + 1);
    मोजणी = 0;

    यदि (array != रिक्त)
    {
	array[मोजणी++] = (वर्ण *)button_string;
	प्रति (p = button_string; *p != NUL; )
	{
	    यदि (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[मोजणी++] = (वर्ण *)p;
	    }
	    अन्यथा यदि (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    अन्यथा
		MB_PTR_ADV(p);
	}
	array[मोजणी] = रिक्त; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = मोजणी;
    प्रत्यागम array;
}

# यदि !GTK_CHECK_VERSION(3,10,0)
    स्थिर वर्ण **
split_button_translation(ध्रुव वर्ण *message)
{
    वर्ण    **buttons = रिक्त;
    char_u  *str;
    पूर्णाङ्क	    n_buttons = 0;
    पूर्णाङ्क	    n_expected = 1;

    प्रति (str = (char_u *)message; *str != NUL; ++str)
	यदि (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(message);
    यदि (str != रिक्त)
    {
	यदि (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, रिक्त);
	अन्यथा
	    str = vim_strsave(str);

	यदि (str != रिक्त)
	    buttons = split_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    यदि (buttons == रिक्त || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = रिक्त;
	str = vim_strsave((char_u *)message);

	यदि (str != रिक्त)
	    buttons = split_button_string(str, &n_buttons);
	यदि (buttons == रिक्त)
	    vim_free(str);
    }

    प्रत्यागम buttons;
}

    स्थिर पूर्णाङ्क
button_equal(ध्रुव वर्ण *a, ध्रुव वर्ण *b)
{
    यावत् (*a != '\0' && *b != '\0')
    {
	यदि (*a == '_' && *++a == '\0')
	    भङ्ग;
	यदि (*b == '_' && *++b == '\0')
	    भङ्ग;

	यदि (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    प्रत्यागम FALSE;

	a = g_utf8_next_char(a);
	b = g_utf8_next_char(b);
    }

    प्रत्यागम (*a == '\0' && *b == '\0');
}
# endif

    स्थिर शून्य
dialog_add_buttons(GtkDialog *dialog, char_u *button_string)
{
# यदि !GTK_CHECK_VERSION(3,10,0)
    वर्ण    **ok;
    वर्ण    **ync;  // "yes no cancel"
# endif
    वर्ण    **buttons;
    पूर्णाङ्क	    n_buttons = 0;
    पूर्णाङ्क	    idx;

    button_string = vim_strsave(button_string); // must be writable
    यदि (button_string == रिक्त)
	प्रत्यागम;

    // Check 'v' flag in 'guioptions': vertical button placement.
    यदि (vim_strchr(p_go, GO_VERTICAL) != रिक्त)
    {
# यदि !GTK_CHECK_VERSION(3,0,0)
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
# यदि !GTK_CHECK_VERSION(3,10,0)
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
    प्रति (idx = 1; idx <= n_buttons; ++idx)
    {
	वर्ण	*label;
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
# यदि !GTK_CHECK_VERSION(3,10,0)
	यदि (ok != रिक्त && ync != रिक्त) // almost impossible to fail
	{
	    यदि	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    अन्यथा यदि (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    अन्यथा यदि (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    अन्यथा यदि (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    अन्यथा यदि (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    अन्यथा यदि (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    अन्यथा यदि (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    अन्यथा यदि (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_TO_UTF8((char_u *)label);
	gtk_dialog_add_button(dialog, (ध्रुव gchar *)label8, idx);
	CONVERT_TO_UTF8_FREE(label8);
    }

# यदि !GTK_CHECK_VERSION(3,10,0)
    यदि (ok != रिक्त)
	vim_free(*ok);
    यदि (ync != रिक्त)
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
प्रकारनिर्देश संरचना _DialogInfo
{
    पूर्णाङ्क		ignore_enter;	    // no default button, ignore "Enter"
    पूर्णाङ्क		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    स्थिर gboolean
dialog_किल्ली_press_event_cb(GtkWidget *widget, Gdk_Event_किल्ली *event, gpointer data)
{
    DialogInfo *di = (DialogInfo *)data;

    // Ignore hitting Enter (or Space) when there is no default button.
    यदि (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	प्रत्यागम TRUE;
    अन्यथा    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    यदि (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	प्रत्यागम TRUE;
    }

    यदि (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_mask()) == 0)
    {
	प्रत्यागम gtk_window_mnemonic_activate(
		   GTK_WINDOW(widget), event->keyval,
		   gtk_window_get_mnemonic_modifier(GTK_WINDOW(widget)));
    }

    प्रत्यागम FALSE; // continue emission
}

    पूर्णाङ्क
gui_mch_dialog(पूर्णाङ्क	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*message,   // message text
	       char_u	*buttons,   // names of buttons
	       पूर्णाङ्क	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       पूर्णाङ्क	ex_cmd UNUSED)
{
    GtkWidget	*dialog;
    GtkWidget	*entry = रिक्त;
    char_u	*text;
    पूर्णाङ्क		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_active;

    dialog = create_message_dialog(type, title, message);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_add_buttons(GTK_DIALOG(dialog), buttons);

    यदि (textfield != रिक्त)
    {
	GtkWidget *alignment;

	entry = gtk_entry_new();
	gtk_widget_show(entry);

	// Make Enter work like pressing OK.
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

	text = CONVERT_TO_UTF8(textfield);
	gtk_entry_set_text(GTK_ENTRY(entry), (ध्रुव वर्ण *)text);
	CONVERT_TO_UTF8_FREE(text);

# यदि GTK_CHECK_VERSION(3,14,0)
	gtk_widget_set_halign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_valign(GTK_WIDGET(entry), GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(GTK_WIDGET(entry), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(entry), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# अन्यथा
	alignment = gtk_alignment_new((भिन्न)0.5, (भिन्न)0.5,
						      (भिन्न)1.0, (भिन्न)1.0);
# endif
	gtk_container_add(GTK_CONTAINER(alignment), entry);
	gtk_container_set_border_width(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# यदि GTK_CHECK_VERSION(3,0,0)
	{
	    GtkWidget * ध्रुव vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_start(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# अन्यथा
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    अन्यथा
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_किल्ली_press_event_cb), &dialoginfo);

    यदि (def_but > 0)
    {
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    अन्यथा
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    यदि (response != GTK_RESPONSE_NONE)
    {
	यदि (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	यदि (textfield != रिक्त)
	{
	    text = (char_u *)gtk_entry_get_text(GTK_ENTRY(entry));
	    text = CONVERT_FROM_UTF8(text);

	    vim_strncpy(textfield, text, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(text);
	}
	gtk_widget_destroy(dialog);
    }

    यदि (gui.dialogs_active > 0)
	--gui.dialogs_active;
    प्रत्यागम response > 0 ? response : 0;
}

#endif // FEAT_GUI_DIALOG


#यदि defined(FEAT_MENU)

    शून्य
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# यदि defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  यदि !GTK_CHECK_VERSION(3,10,0)
    यदि (xic != रिक्त && g_object_get_data(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == रिक्त)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_TO_UTF8(name);
	menuitem = gtk_menu_item_new_with_mnemonic((ध्रुव वर्ण *)name);
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

# यदि GTK_CHECK_VERSION(3,22,2)
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
	trigger.axes       = रिक्त;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = रिक्त;
	trigger.x_मूळ     = 0.0;
	trigger.y_मूळ     = 0.0;

	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# अन्यथा
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   रिक्त, रिक्त,
		   (GtkMenuPositionFunc)रिक्त, रिक्त,
		   3U, gui.event_time);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
स्थिर पूर्णाङ्क popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    स्थिर शून्य
popup_menu_position_func(GtkMenu *menu UNUSED,
			 gint *x, gint *y,
			 gboolean *ढकला_in UNUSED,
			 gpointer user_data UNUSED)
{
    gdk_window_get_origin(gtk_widget_get_window(gui.drawarea), x, y);

    यदि (popup_mouse_pos)
    {
	पूर्णाङ्क	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    अन्यथा यदि (curwin != रिक्त && gui.drawarea != रिक्त &&
	     gtk_widget_get_window(gui.drawarea) != रिक्त)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    शून्य
gui_make_popup(char_u *path_name, पूर्णाङ्क mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_find_menu(path_name);
    यदि (menu == रिक्त || menu->submenu_id == रिक्त)
	प्रत्यागम;

# यदि GTK_CHECK_VERSION(3,22,2)
    GdkWindow * ध्रुव win = gtk_widget_get_window(gui.drawarea);
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
    trigger.axes       = रिक्त;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = रिक्त;
    trigger.x_मूळ     = 0.0;
    trigger.y_मूळ     = 0.0;

    यदि (mouse_pos)
	gtk_menu_popup_at_pointer(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    अन्यथा
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_window_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(रिक्त, &rect.x, &rect.y, रिक्त, रिक्त);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# अन्यथा
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    रिक्त, रिक्त,
	    &popup_menu_position_func, रिक्त,
	    0U, (guint32)GDK_CURRENT_TIME);
# endif
}

#endif // FEAT_MENU


/*
 * We don't create it twice.
 */

प्रकारनिर्देश संरचना _SharedFindReplace
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

स्थिर SharedFindReplace find_widgets = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
स्थिर SharedFindReplace repl_widgets = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};

    स्थिर पूर्णाङ्क
find_किल्ली_press_event(
		GtkWidget	*widget UNUSED,
		Gdk_Event_किल्ली	*event,
		SharedFindReplace *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    यदि (event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	प्रत्यागम FALSE;

    // the Escape key synthesizes a cancellation action
    यदि (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	प्रत्यागम TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    प्रत्यागम FALSE;
}

    स्थिर GtkWidget *
#यदि GTK_CHECK_VERSION(3,10,0)
create_image_button(ध्रुव वर्ण *stock_id UNUSED,
		    ध्रुव वर्ण *label)
#अन्यथा
create_image_button(ध्रुव वर्ण *stock_id,
		    ध्रुव वर्ण *label)
#endif
{
    char_u	*text;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    text = CONVERT_TO_UTF8((char_u *)label);

#यदि GTK_CHECK_VERSION(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
#अन्यथा
    box = gtk_hbox_new(FALSE, 3);
#endif
#यदि !GTK_CHECK_VERSION(3,10,0)
    यदि (stock_id != रिक्त)
	gtk_box_pack_start(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_BUTTON),
			   FALSE, FALSE, 0);
#endif
    gtk_box_pack_start(GTK_BOX(box),
		       gtk_label_new((ध्रुव वर्ण *)text),
		       FALSE, FALSE, 0);

    CONVERT_TO_UTF8_FREE(text);

#यदि GTK_CHECK_VERSION(3,14,0)
    gtk_widget_set_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#अन्यथा
    alignment = gtk_alignment_new((भिन्न)0.5, (भिन्न)0.5,
						      (भिन्न)0.0, (भिन्न)0.0);
#endif
    gtk_container_add(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), alignment);

    प्रत्यागम button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    स्थिर ध्रुव वर्ण *
convert_localized_message(char_u **buffer, ध्रुव वर्ण *message)
{
    यदि (output_conv.vc_type == CONV_NONE)
	प्रत्यागम message;

    vim_free(*buffer);
    *buffer = string_convert(&output_conv, (char_u *)message, रिक्त);

    प्रत्यागम (ध्रुव वर्ण *)*buffer;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    स्थिर unsigned long
entry_get_text_length(GtkEntry *entry)
{
    g_return_val_if_fail(entry != रिक्त, 0);
    g_return_val_if_fail(GTK_IS_ENTRY(entry) == TRUE, 0);

#यदि GTK_CHECK_VERSION(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    प्रत्यागम gtk_entry_buffer_get_length(gtk_entry_get_buffer(entry));
#नोचेद्यदि GTK_CHECK_VERSION(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    प्रत्यागम gtk_entry_get_text_length(entry);
#अन्यथा
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    प्रत्यागम g_utf8_strlen(gtk_entry_get_text(entry), -1);
#endif
}

    स्थिर शून्य
find_replace_dialog_create(char_u *arg, पूर्णाङ्क do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*table;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    SharedFindReplace *frdp;
    char_u	*entry_text;
    पूर्णाङ्क		wword = FALSE;
    पूर्णाङ्क		mcase = !p_ic;
    char_u	*conv_buffer = रिक्त;
#define CONV(message) convert_localized_message(&conv_buffer, (message))

    frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    यदि (entry_text != रिक्त && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_text = entry_text;
	entry_text = string_convert(&output_conv, entry_text, रिक्त);
	vim_free(old_text);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    यदि (frdp->dialog)
    {
	यदि (entry_text != रिक्त)
	{
	    gtk_entry_set_text(GTK_ENTRY(frdp->what), (वर्ण *)entry_text);
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
	यदि (do_replace && entry_get_text_length(GTK_ENTRY(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	प्रत्यागम;
    }

    frdp->dialog = gtk_dialog_new();
#यदि GTK_CHECK_VERSION(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#अन्यथा
    gtk_dialog_set_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#endif
    gtk_window_set_transient_for(GTK_WINDOW(frdp->dialog), GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(frdp->dialog), TRUE);

    यदि (do_replace)
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    अन्यथा
    {
	gtk_window_set_title(GTK_WINDOW(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#यदि GTK_CHECK_VERSION(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
#अन्यथा
    hbox = gtk_hbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
#यदि GTK_CHECK_VERSION(3,0,0)
    {
	GtkWidget * ध्रुव dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_add(GTK_CONTAINER(dialog_vbox), hbox);
    }
#अन्यथा
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#endif

    यदि (do_replace)
#यदि GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#अन्यथा
	table = gtk_table_new(1024, 4, FALSE);
#endif
    अन्यथा
#यदि GTK_CHECK_VERSION(3,4,0)
	table = gtk_grid_new();
#अन्यथा
	table = gtk_table_new(1024, 3, FALSE);
#endif
    gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(table), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#यदि GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#नोचेद्यदि GTK_CHECK_VERSION(3,14,0)
    {
	GValue align_val = G_मूल्य_INIT;

	g_मूल्य_init(&align_val, G_TYPE_FLOAT);

	g_मूल्य_set_float(&align_val, 0.0);
	g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	g_मूल्य_set_float(&align_val, 0.5);
	g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	g_मूल्य_unset(&align_val);
    }
#अन्यथा
    gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#यदि GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), tmp, 0, 0, 2, 1);
#अन्यथा
    gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    frdp->what = gtk_entry_new();
    sensitive = (entry_text != रिक्त && entry_text[0] != NUL);
    यदि (entry_text != रिक्त)
	gtk_entry_set_text(GTK_ENTRY(frdp->what), (वर्ण *)entry_text);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(entry_changed_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(find_किल्ली_press_event),
			   (gpointer) frdp);
#यदि GTK_CHECK_VERSION(3,4,0)
    gtk_grid_attach(GTK_GRID(table), frdp->what, 2, 0, 5, 1);
#अन्यथा
    gtk_table_attach(GTK_TABLE(table), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    यदि (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#यदि GTK_CHECK_VERSION(3,16,0)
	gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_set_yalign(GTK_LABEL(tmp), 0.5);
#नोचेद्यदि GTK_CHECK_VERSION(3,14,0)
	{
	    GValue align_val = G_मूल्य_INIT;

	    g_मूल्य_init(&align_val, G_TYPE_FLOAT);

	    g_मूल्य_set_float(&align_val, 0.0);
	    g_object_set_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_मूल्य_set_float(&align_val, 0.5);
	    g_object_set_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_मूल्य_unset(&align_val);
	}
#अन्यथा
	gtk_misc_set_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#endif
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 0, 1, 2, 1);
#अन्यथा
	gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
	frdp->with = gtk_entry_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(find_किल्ली_press_event),
			       (gpointer) frdp);
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->with, 2, 1, 5, 1);
#अन्यथा
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
    अन्यथा
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
    यदि (do_replace)
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 2, 5, 1);
#अन्यथा
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    अन्यथा
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->wword, 0, 3, 5, 1);
#अन्यथा
	gtk_table_attach(GTK_TABLE(table), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    यदि (do_replace)
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 3, 5, 1);
#अन्यथा
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif
    अन्यथा
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), frdp->mcase, 0, 4, 5, 1);
#अन्यथा
	gtk_table_attach(GTK_TABLE(table), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#endif

    tmp = gtk_frame_new(CONV(_("Direction")));
    यदि (do_replace)
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 2, 4);
#अन्यथा
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
    अन्यथा
#यदि GTK_CHECK_VERSION(3,4,0)
	gtk_grid_attach(GTK_GRID(table), tmp, 5, 2, 1, 3);
#अन्यथा
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#endif
#यदि GTK_CHECK_VERSION(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
#अन्यथा
    vbox = gtk_vbox_new(FALSE, 0);
#endif
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
    gtk_container_add(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->up = gtk_radio_button_new_with_label(रिक्त, CONV(_("Up")));
    gtk_box_pack_start(GTK_BOX(vbox), frdp->up, TRUE, TRUE, 0);
    frdp->down = gtk_radio_button_new_with_label(
			gtk_radio_button_get_group(GTK_RADIO_BUTTON(frdp->up)),
			CONV(_("Down")));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frdp->down), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_start(GTK_BOX(vbox), frdp->down, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#यदि GTK_CHECK_VERSION(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#अन्यथा
    actionarea = gtk_vbutton_box_new();
#endif
    gtk_container_set_border_width(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_end(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#यदि GTK_CHECK_VERSION(3,10,0)
    frdp->find = create_image_button(रिक्त, _("Find Next"));
#अन्यथा
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

    यदि (do_replace)
    {
	// 'Replace' button
#यदि GTK_CHECK_VERSION(3,10,0)
	frdp->replace = create_image_button(रिक्त, _("Replace"));
#अन्यथा
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#endif
	gtk_widget_set_sensitive(frdp->replace, sensitive);
	gtk_widget_set_can_default(frdp->find, TRUE);
	gtk_box_pack_start(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(find_replace_cb),
			 GINT_TO_POINTER(FRD_REPLACE));

	// 'Replace All' button
#यदि GTK_CHECK_VERSION(3,10,0)
	frdp->all = create_image_button(रिक्त, _("Replace All"));
#अन्यथा
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
#यदि GTK_CHECK_VERSION(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#अन्यथा
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

#यदि GTK_CHECK_VERSION(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#अन्यथा
    tmp = gtk_vseparator_new();
#endif
    gtk_box_pack_end(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#यदि GTK_CHECK_VERSION(3,0,0)
# यदि !GTK_CHECK_VERSION(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#अन्यथा
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#endif
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(entry_text);
    vim_free(conv_buffer);
#undef CONV
}

    शून्य
gui_mch_find_dialog(exarg_T *eap)
{
    यदि (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    शून्य
gui_mch_replace_dialog(exarg_T *eap)
{
    यदि (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    स्थिर शून्य
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    पूर्णाङ्क			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = (पूर्णाङ्क)(long)data;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    यदि (flags == FRD_FINDNEXT)
    {
	repl_text = रिक्त;
	sfr = &find_widgets;
    }
    अन्यथा
    {
	repl_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_entry_get_text(GTK_ENTRY(sfr->what));
    direction_down = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->down));

    यदि (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    यदि (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sfr->mcase)))
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
    स्थिर शून्य
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
    स्थिर शून्य
entry_changed_cb(GtkWidget * entry, GtkWidget * dialog)
{
    ध्रुव gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

    यदि (!entry_text)
	प्रत्यागम;			// shouldn't happen

    nonempty = (entry_text[0] != '\0');

    यदि (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    यदि (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    शून्य
ex_helpfind(exarg_T *eap UNUSED)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#यदि defined(FEAT_BROWSE)
    स्थिर शून्य
recent_func_log_func(ध्रुव gchar *log_domain UNUSED,
		     GLogLevelFlags log_level UNUSED,
		     ध्रुव gchar *message UNUSED,
		     gpointer user_data UNUSED)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#endif

    शून्य
gui_mch_set_fullscreen(पूर्णाङ्क flag)
{
    यदि (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    अन्यथा
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}
