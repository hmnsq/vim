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

#شامل "vim.h"

#اگر_تعریف_شدہ FEAT_GUI_GTK
# include "gui_gtk_f.h"
#اگر_ختم

// GTK defines MAX and MIN, but some system header files as well.  Undefine
// them and don't use them.
#اگر_تعریف_شدہ MIN
# undef MIN
#اگر_ختم
#اگر_تعریف_شدہ MAX
# undef MAX
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_GNOME
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
# اگر defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_GTK
# اگر GTK_CHECK_نسخہ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# ورنہ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# ورنہ
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#ورنہ
// define these items to be able to generate prototypes without GTK
قسمتعریف صحیح GtkWidget;
# define gpointer صحیح
# define guint8 صحیح
# define GdkPixmap صحیح
# define GdkBitmap صحیح
# define GtkIconFactory صحیح
# define GtkToolbar صحیح
# define GtkAdjustment صحیح
# define gboolean صحیح
# define Gdk_Event_کلید صحیح
# define Cancel_ڈیٹا صحیح
#اگر_ختم

ساکن خالی اندراج_activate_cb(GtkWidget *widget, gpointer ڈیٹا);
ساکن خالی اندراج_بدلا_cb(GtkWidget *اندراج_2, GtkWidget *dialog);
ساکن خالی ڈھونڈو_replace_cb(GtkWidget *widget, gpointer ڈیٹا);
#اگر defined(FEAT_BROWSE)
ساکن خالی recent_func_log_func(
	ثابت gchar *log_domain,
	GLogLevelFlags log_level,
	ثابت gchar *پیغام_2,
	gpointer user_ڈیٹا);
#اگر_ختم

#اگر defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# اگر GTK_CHECK_نسخہ(3,10,0)
ساکن ثابت حرف * ثابت menu_themed_نام[] =
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
#  اگر GTK_CHECK_نسخہ(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  ورنہ
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  اگر GTK_CHECK_نسخہ(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  ورنہ
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  اگر GTK_CHECK_نسخہ(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  ورنہ
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  اگر GTK_CHECK_نسخہ(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  ورنہ
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# ورنہ // !GTK_CHECK_VERSION(3,10,0)
ساکن ثابت حرف * ثابت menu_stock_ids[] =
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
    /* 10 */ GTK_STOCK_ڈھونڈو,
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
    /* 15 */ GTK_STOCK_EXECUTE,
    /* 16 */ GTK_STOCK_ڈھونڈو_AND_REPLACE,
    /* 17 */ GTK_STOCK_CLOSE,		// FIXME: fuzzy
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
    /* 22 */ GTK_STOCK_GO_عقب,
    /* 23 */ GTK_STOCK_GO_FORWARD,
    /* 24 */ "vim-find-help",
    /* 25 */ GTK_STOCK_CONVERT,
    /* 26 */ GTK_STOCK_JUMP_مقصد,
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
    /* 31 */ GTK_STOCK_QUIT
};
# endif // !GTK_CHECK_VERSION(3,10,0)

# ifdef USE_GRESOURCE
#  اگر !GTK_CHECK_نسخہ(3,10,0)
قسمتعریف ساخت Icon_نام {
    ثابت حرف *icon_name;
    ثابت حرف *file_name;
} Icon_نام;

ساکن Icon_نام stock_vim_icons[] = {
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
    { تہی, تہی }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    ساکن خالی
جوڑو_stock_icon(GtkIconFactory	*factory,
	       ثابت حرف	*stock_id,
	       ثابت guint8	*inline_ڈیٹا,
	       صحیح		ڈیٹا_لمبائی)
{
    GdkPixbuf	*pixbuf;
    Gtk_Icon_سیٹ	*icon_سیٹ;

    pixbuf = gdk_pixbuf_new_from_inline(ڈیٹا_لمبائی, inline_ڈیٹا, FALSE, تہی);
    icon_سیٹ = gtk_icon_سیٹ_new_from_pixbuf(pixbuf);

    gtk_icon_factory_جوڑو(factory, stock_id, icon_سیٹ);

    gtk_icon_سیٹ_unref(icon_سیٹ);
    g_object_unref(pixbuf);
}
# endif

    ساکن صحیح
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    اگر (mch_is_بھرا_Name(dest))
    {
	واپس vim_fexists(dest);
    }
    ورنہ
    {
	ساکن ثابت حرف   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	غیرعلامتی صحیح	    i;

	برائے (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    اگر (gui_ڈھونڈو_bitmap(dest, buf, (حرف *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		واپس TRUE;
	    }

	واپس FALSE;
    }
}

    ساکن GtkWidget *
بھرو_menu_iconfile(char_u *name, Gtk_Icon_سائز icon_سائز)
{
    GtkWidget	    *image = تہی;
# اگر GTK_CHECK_نسخہ(3,10,0)
    صحیح		     pixel_سائز = -1;

    انتخاب (icon_سائز)
    {
	حالت GTK_ICON_سائز_MENU:
	    pixel_سائز = 16;
	    توڑو;
	حالت GTK_ICON_سائز_SMALL_TOOLBAR:
	    pixel_سائز = 16;
	    توڑو;
	حالت GTK_ICON_سائز_LARGE_TOOLBAR:
	    pixel_سائز = 24;
	    توڑو;
	حالت GTK_ICON_سائز_BUTTON:
	    pixel_سائز = 16;
	    توڑو;
	حالت GTK_ICON_سائز_DND:
	    pixel_سائز = 32;
	    توڑو;
	حالت GTK_ICON_سائز_DIALOG:
	    pixel_سائز = 48;
	    توڑو;
	حالت GTK_ICON_سائز_INVALID:
	    // FALLTHROUGH
	طےشدہ:
	    pixel_سائز = 0;
	    توڑو;
    }

    اگر (pixel_سائز > 0 || pixel_سائز == -1)
    {
	GdkPixbuf * ثابت pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((ثابت حرف *)name,
		pixel_سائز, pixel_سائز, TRUE, تہی);
	اگر (pixbuf != تہی)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    اگر (image == تہی)
	image = gtk_image_new_from_icon_name("image-missing", icon_سائز);

    واپس image;
# ورنہ // !GTK_CHECK_VERSION(3,10,0)
    Gtk_Icon_سیٹ	    *icon_سیٹ;
    Gtk_Icon_ماخذ   *icon_ماخذ;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_سیٹ = gtk_icon_سیٹ_new();
    icon_ماخذ = gtk_icon_ماخذ_new();

    gtk_icon_ماخذ_سیٹ_filename(icon_ماخذ, (ثابت حرف *)name);
    gtk_icon_سیٹ_جوڑو_ماخذ(icon_سیٹ, icon_ماخذ);

    image = gtk_image_new_from_icon_سیٹ(icon_سیٹ, icon_سائز);

    gtk_icon_ماخذ_free(icon_ماخذ);
    gtk_icon_سیٹ_unref(icon_سیٹ);

    واپس image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    ساکن GtkWidget *
create_menu_icon(vimmenu_T *menu, Gtk_Icon_سائز icon_سائز)
{
    GtkWidget	*image = تہی;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    اگر (menu->iconfile != تہی && lookup_menu_iconfile(menu->iconfile, buf))
	image = بھرو_menu_iconfile(buf, icon_سائز);

    // If not found and not builtin specified try using the menu name.
    اگر (image == تہی && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = بھرو_menu_iconfile(buf, icon_سائز);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    اگر (image == تہی)
    {
# اگر GTK_CHECK_نسخہ(3,10,0)
	ثابت حرف *icon_name = تہی;
	ثابت صحیح   n_نام = G_N_ELEMENTS(menu_themed_نام);

	اگر (menu->iconidx >= 0 && menu->iconidx < n_نام)
	    icon_name = menu_themed_نام[menu->iconidx];
	اگر (icon_name == تہی)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_سائز);
# ورنہ
	ثابت حرف  *stock_id;
	ثابت صحیح   n_ids = G_N_ELEMENTS(menu_stock_ids);

	اگر (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	ورنہ
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_سائز);
# endif
    }

    واپس image;
}

    ساکن gint
toolbar_button_focus_اندر_event(GtkWidget *widget غیر_مستعمل,
			      GdkEventFocus *event غیر_مستعمل,
			      gpointer ڈیٹا غیر_مستعمل)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    اگر (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    واپس TRUE;
}
#اگر_ختم // FEAT_TOOLBAR

#اگر defined(FEAT_TOOLBAR)

    خالی
gui_gtk_رجسٹر_stock_icons(خالی)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define جوڑو_ICON(Name, ڈیٹا_2) جوڑو_stock_icon(factory, Name, ڈیٹا_2, (صحیح)حجم(ڈیٹا_2))

    جوڑو_ICON("vim-build-tags",		  stock_vim_build_لیبل_فہرست);
    جوڑو_ICON("vim-find-help",		  stock_vim_ڈھونڈو_help);
    جوڑو_ICON("vim-save-all",		  stock_vim_save_all);
    جوڑو_ICON("vim-session-load",	  stock_vim_session_بھرو);
    جوڑو_ICON("vim-session-new",		  stock_vim_session_new);
    جوڑو_ICON("vim-session-save",	  stock_vim_session_save);
    جوڑو_ICON("vim-shell",		  stock_vim_shell);
    جوڑو_ICON("vim-window-maximize",	  stock_vim_کھڑکی_maximize);
    جوڑو_ICON("vim-window-maximize-width", stock_vim_کھڑکی_maximize_چوڑائی);
    جوڑو_ICON("vim-window-minimize",	  stock_vim_کھڑکی_minimize);
    جوڑو_ICON("vim-window-minimize-width", stock_vim_کھڑکی_minimize_چوڑائی);
    جوڑو_ICON("vim-window-split",	  stock_vim_کھڑکی_بانٹو);
    جوڑو_ICON("vim-window-split-vertical", stock_vim_کھڑکی_بانٹو_vertical);

#  undef جوڑو_ICON

    gtk_icon_factory_جوڑو_default(factory);
    g_object_unref(factory);
# ورنہ // defined(USE_GRESOURCE)
    ثابت حرف * ثابت راستہ_سابقہ = "/org/vim/gui/icon";
#  اگر GTK_CHECK_نسخہ(3,14,0)
    GdkScreen    *screen = تہی;
    GtkIconTheme *icon_theme = تہی;

    اگر (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    ورنہ
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_جوڑو_resource_راستہ(icon_theme, راستہ_سابقہ);
#  elif GTK_CHECK_نسخہ(3,0,0)
    Icon_نام *نام;

    برائے (نام = stock_vim_icons; نام->icon_name != تہی; نام++)
    {
	حرف راستہ[MAXPATHL];

	vim_snprintf(راستہ, MAXPATHL, "%s/%s", راستہ_سابقہ, نام->file_name);
	GdkPixbuf *pixbuf = تہی;
	pixbuf = gdk_pixbuf_new_from_resource(راستہ, تہی);
	اگر (pixbuf != تہی)
	{
	    ثابت gint سائز = MAX(gdk_pixbuf_get_چوڑائی(pixbuf),
				  gdk_pixbuf_get_اونچائی(pixbuf));
	    اگر (سائز > 16)
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
		اگر (pixbuf == تہی)
		    pixbuf = src;
		ورنہ
		    g_object_unref(src);
	    }
	    gtk_icon_theme_جوڑو_builtin_icon(نام->icon_name, سائز, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  ورنہ // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * ثابت factory = gtk_icon_factory_new();
    Icon_نام *نام;

    برائے (نام = stock_vim_icons; نام->icon_name != تہی; نام++)
    {
	حرف راستہ[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(راستہ, MAXPATHL, "%s/%s", راستہ_سابقہ, نام->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(راستہ, تہی);
	اگر (pixbuf != تہی)
	{
	    Gtk_Icon_سیٹ *icon_سیٹ = gtk_icon_سیٹ_new_from_pixbuf(pixbuf);
	    gtk_icon_factory_جوڑو(factory, نام->icon_name, icon_سیٹ);
	    gtk_icon_سیٹ_unref(icon_سیٹ);
	    g_object_unref(pixbuf);
	}
    }

    gtk_icon_factory_جوڑو_default(factory);
    g_object_unref(factory);
#  endif // !GTK_CHECK_VERSION(3,0,0)
# endif // defined(USE_GRESOURCE)
}

#اگر_ختم // FEAT_TOOLBAR

#اگر defined(FEAT_MENU)

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
    ساکن char_u *
translate_mnemonic_لیبل(char_u *name, صحیح use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    صحیح	    n_underscores = 0;

    name = CONVERT_مقصد_UTF8(name);
    اگر (name == تہی)
	واپس تہی;

    برائے (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	اگر (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    اگر (buf != تہی)
    {
	pdest = buf;
	برائے (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    اگر (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ورنہ اگر (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ورنہ اگر (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ورنہ اگر (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_مقصد_UTF8_FREE(name);
    واپس buf;
}

    ساکن خالی
menu_شے_new(vimmenu_T *menu, GtkWidget *والد_widget)
{
    GtkWidget	*box;
    char_u	*متن;
    صحیح		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_شے_new();
# اگر GTK_CHECK_نسخہ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_سیٹ_homogeneous(GTK_BOX(box), FALSE);
# ورنہ
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(والد_widget));
    متن = translate_mnemonic_لیبل(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((ثابت حرف *)متن);
    vim_free(متن);

    gtk_box_pack_آغاز(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    اگر (menu->actext != تہی && menu->actext[0] != NUL)
    {
	متن = CONVERT_مقصد_UTF8(menu->actext);

	gtk_box_pack_انتہا(GTK_BOX(box),
			 gtk_label_new((ثابت حرف *)متن),
			 FALSE, FALSE, 0);

	CONVERT_مقصد_UTF8_FREE(متن);
    }

    gtk_container_جوڑو(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    خالی
gui_mch_جوڑو_menu(vimmenu_T *menu, صحیح idx)
{
    vimmenu_T	*والد;
    GtkWidget	*والد_widget;

    اگر (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	واپس;
    }

    والد = menu->والد;

    اگر ((والد != تہی && والد->submenu_id == تہی)
	    || !menu_is_menubar(menu->name))
	واپس;

    والد_widget = (والد != تہی) ? والد->submenu_id : gui.menubar;
    menu_شے_new(menu, والد_widget);

# اگر !GTK_CHECK_نسخہ(3,4,0)
    // since the tearoff should always appear first, increment idx
    اگر (والد != تہی && !menu_is_popup(والد->name))
	++idx;
# endif

    gtk_menu_shell_داخل_کرو(GTK_MENU_SHELL(والد_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_سیٹ_accel_گروہ(GTK_MENU(menu->submenu_id), gui.accel_گروہ);
    gtk_menu_شے_سیٹ_submenu(GTK_MENU_شے(menu->id), menu->submenu_id);

# اگر !GTK_CHECK_نسخہ(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_شے_new();
    اگر (vim_strchr(p_go, GO_TEAROFF) != تہی)
	gtk_widget_show(menu->tearoff_handle);
#  اگر GTK_CHECK_نسخہ(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  ورنہ
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    ساکن خالی
menu_شے_activate(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا)
{
    gui_menu_cb((vimmenu_T *)ڈیٹا);
}

    ساکن خالی
menu_شے_select(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    ساکن صحیح	did_پیغام = FALSE;

    اگر (State & منوال_CMDLINE)
	واپس;
    menu = (vimmenu_T *)ڈیٹا;
    tooltip = CONVERT_مقصد_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
    اگر (tooltip != تہی && utf_valid_string(tooltip, تہی))
    {
	پیغام((حرف *)tooltip);
	did_پیغام = TRUE;
	setcursor();
	برآمد_flush_cursor(TRUE, FALSE);
    }
    ورنہ اگر (did_پیغام)
    {
	پیغام("");
	did_پیغام = FALSE;
	setcursor();
	برآمد_flush_cursor(TRUE, FALSE);
    }
    CONVERT_مقصد_UTF8_FREE(tooltip);
}

    خالی
gui_mch_جوڑو_menu_شے(vimmenu_T *menu, صحیح idx)
{
    vimmenu_T *والد;

    والد = menu->والد;

# ifdef FEAT_TOOLBAR
    اگر (menu_is_toolbar(والد->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = تہی;

	اگر (menu_is_separator(menu->name))
	{
#  اگر GTK_CHECK_نسخہ(3,0,0)
	    Gtk_Tool_شے *شے = gtk_separator_tool_شے_new();
	    gtk_separator_tool_شے_سیٹ_draw(GTK_SEPARATOR_TOOL_شے(شے),
		    TRUE);
	    gtk_tool_شے_سیٹ_expand(GTK_TOOL_شے(شے), FALSE);
	    gtk_widget_show(GTK_WIDGET(شے));

	    gtk_toolbar_داخل_کرو(toolbar, شے, idx);
#  ورنہ
	    gtk_toolbar_داخل_کرو_space(toolbar, idx);
#  endif
	    menu->id = تہی;
	}
	ورنہ
	{
	    char_u *متن;
	    char_u *tooltip;

	    متن    = CONVERT_مقصد_UTF8(menu->dname);
	    tooltip = CONVERT_مقصد_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
	    اگر (tooltip != تہی && !utf_valid_string(tooltip, تہی))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_مقصد_UTF8_FREE(tooltip);

#  اگر GTK_CHECK_نسخہ(3,0,0)
	    {
		GtkWidget *icon;
		Gtk_Tool_شے *شے;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_سائز(toolbar));
		شے = gtk_tool_button_new(icon, (ثابت gchar *)متن);
		gtk_tool_شے_سیٹ_tooltip_متن(شے, (ثابت gchar *)tooltip);
		g_signal_connect(G_OBJECT(شے), "clicked",
			G_CALLBACK(&menu_شے_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(شے));

		gtk_toolbar_داخل_کرو(toolbar, شے, idx);

		menu->id = GTK_WIDGET(شے);
	    }
#  ورنہ
	    menu->id = gtk_toolbar_داخل_کرو_شے(
		    toolbar,
		    (ثابت حرف *)متن,
		    (ثابت حرف *)tooltip,
		    تہی,
		    create_menu_icon(menu, gtk_toolbar_get_icon_سائز(toolbar)),
		    G_CALLBACK(&menu_شے_activate),
		    menu,
		    idx);
#  endif

	    اگر (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_اندر_event), تہی);

	    CONVERT_مقصد_UTF8_FREE(متن);
	    CONVERT_مقصد_UTF8_FREE(tooltip);
	}
    }
    ورنہ
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	اگر (والد == تہی || والد->submenu_id == تہی)
	    واپس;

# اگر !GTK_CHECK_نسخہ(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	اگر (!menu_is_popup(والد->name))
	    ++idx;
# endif

	اگر (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# اگر GTK_CHECK_نسخہ(3,0,0)
	    menu->id = gtk_separator_menu_شے_new();
# ورنہ
	    menu->id = gtk_menu_شے_new();
	    gtk_widget_سیٹ_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_داخل_کرو(GTK_MENU_SHELL(والد->submenu_id),
		    menu->id, idx);

	    واپس;
	}

	// Add textual menu item.
	menu_شے_new(menu, والد->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_داخل_کرو(GTK_MENU_SHELL(والد->submenu_id),
		menu->id, idx);

	اگر (menu->id != تہی)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_شے_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_شے_select), menu);
	}
    }
}
#اگر_ختم // FEAT_MENU


    خالی
gui_mch_سیٹ_متن_area_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#اگر defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    خالی
gui_gtk_سیٹ_mnemonics(صحیح enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	اگر (menu->id == تہی)
	    جاری;

	name = translate_mnemonic_لیبل(menu->name, enable);
	gtk_label_سیٹ_متن_with_mnemonic(GTK_LABEL(menu->label),
					 (ثابت حرف *)name);
	vim_free(name);
    }
}

# اگر !GTK_CHECK_نسخہ(3,4,0)
    ساکن خالی
recurse_tearoffs(vimmenu_T *menu, صحیح val)
{
    برائے (; menu != تہی; menu = menu->اگلا)
    {
	اگر (menu->submenu_id != تہی && menu->tearoff_handle != تہی
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    اگر (val)
		gtk_widget_show(menu->tearoff_handle);
	    ورنہ
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# اگر GTK_CHECK_نسخہ(3,4,0)
    خالی
gui_mch_toggle_tearoffs(صحیح enable غیر_مستعمل)
{
    // Do nothing
}
# ورنہ
    خالی
gui_mch_toggle_tearoffs(صحیح enable)
{
    recurse_tearoffs(جڑ_menu, enable);
}
# endif
#اگر_ختم // FEAT_MENU

#اگر defined(FEAT_TOOLBAR)
    ساکن صحیح
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*گرہ;
    صحیح		idx = 0;

    برائے (گرہ = menu->والد->children; گرہ != menu; گرہ = گرہ->اگلا)
    {
	g_return_val_if_fail(گرہ != تہی, -1);
	++idx;
    }

    واپس idx;
}
#اگر_ختم // FEAT_TOOLBAR


#اگر defined(FEAT_TOOLBAR)
    خالی
gui_mch_menu_سیٹ_tip(vimmenu_T *menu)
{
    اگر (menu->id == تہی || menu->والد == تہی || gui.toolbar == تہی)
	واپس;

    char_u *tooltip;

    tooltip = CONVERT_مقصد_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
    اگر (tooltip != تہی && utf_valid_string(tooltip, تہی))
# اگر GTK_CHECK_نسخہ(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_سیٹ_tooltip_متن(menu->id, (ثابت gchar *)tooltip);
# ورنہ
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_سیٹ_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (ثابت حرف *)tooltip, تہی);
# endif
    CONVERT_مقصد_UTF8_FREE(tooltip);
}
#اگر_ختم // FEAT_TOOLBAR


#اگر defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    خالی
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    اگر (menu->id != تہی)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    اگر (menu->id != تہی && gui.menubar != تہی
			    && gtk_widget_get_والد(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    اگر (menu->والد != تہی && menu_is_toolbar(menu->والد->name))
    {
	اگر (menu_is_separator(menu->name))
#  اگر GTK_CHECK_نسخہ(3,0,0)
	{
	    Gtk_Tool_شے *شے = تہی;

	    شے = gtk_toolbar_get_nth_شے(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    اگر (شے != تہی)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(شے));
	}
#  ورنہ
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	ورنہ اگر (menu->id != تہی)
	    gtk_widget_destroy(menu->id);
    }
    ورنہ
# endif // FEAT_TOOLBAR
    {
	اگر (menu->submenu_id != تہی)
	    gtk_widget_destroy(menu->submenu_id);

	اگر (menu->id != تہی)
	    gtk_widget_destroy(menu->id);
    }

    اگر (menu->id != تہی)
	g_object_unref(menu->id);
    menu->submenu_id = تہی;
    menu->id = تہی;
}
#اگر_ختم // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    خالی
gui_mch_سیٹ_scrollbar_thumb(scrollbar_T *sb, طویل val, طویل سائز, طویل max)
{
    اگر (sb->id == تہی)
	واپس;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_سیٹ_زیریں(adjustment, 0.0);
    gtk_adjustment_سیٹ_قدر(adjustment, val);
    gtk_adjustment_سیٹ_بالائی(adjustment, max + 1);
    gtk_adjustment_سیٹ_صفحہ_سائز(adjustment, سائز);
    gtk_adjustment_سیٹ_صفحہ_increment(adjustment,
	    سائز < 3L ? 1L : سائز - 2L);
    gtk_adjustment_سیٹ_قدم_increment(adjustment, 1.0);

    g_signal_handler_بلاک(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#اگر !GTK_CHECK_نسخہ(3,18,0)
    gtk_adjustment_بدلا(adjustment);
#اگر_ختم

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    خالی
gui_mch_سیٹ_scrollbar_pos(scrollbar_T *sb, صحیح x, صحیح y, صحیح w, صحیح h)
{
    اگر (sb->id != تہی)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    صحیح
gui_mch_get_scrollbar_xpadding(خالی)
{
    صحیح xpad;
#اگر GTK_CHECK_نسخہ(3,0,0)
    xpad = gtk_widget_get_allocated_چوڑائی(gui.formwin)
	  - gtk_widget_get_allocated_چوڑائی(gui.drawarea) - gui.scrollbar_چوڑائی;
#ورنہ
    xpad = gui.formwin->تخصیص.چوڑائی - gui.drawarea->تخصیص.چوڑائی
							 - gui.scrollbar_چوڑائی;
#اگر_ختم
    اگر (gui.which_scrollbars[SBAR_بایاں] && gui.which_scrollbars[SBAR_دایاں])
	xpad -= gui.scrollbar_چوڑائی;

    واپس (xpad < 0) ? 0 : xpad;
}

    صحیح
gui_mch_get_scrollbar_ypadding(خالی)
{
    صحیح ypad;
#اگر GTK_CHECK_نسخہ(3,0,0)
    ypad = gtk_widget_get_allocated_اونچائی(gui.formwin)
	- gtk_widget_get_allocated_اونچائی(gui.drawarea) - gui.scrollbar_اونچائی;
#ورنہ
    ypad = gui.formwin->تخصیص.اونچائی - gui.drawarea->تخصیص.اونچائی
							- gui.scrollbar_اونچائی;
#اگر_ختم
    واپس (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    ساکن خالی
adjustment_قدر_بدلا(GtkAdjustment *adjustment, gpointer ڈیٹا)
{
    scrollbar_T	*sb;
    طویل	قدر;
    صحیح		dragging = FALSE;

#اگر_تعریف_شدہ FEAT_XIM
    // cancel any preediting
    اگر (im_is_preediting())
	xim_reset();
#اگر_ختم

    sb = gui_ڈھونڈو_scrollbar((طویل)ڈیٹا);
    قدر = gtk_adjustment_get_قدر(adjustment);
#اگر !GTK_CHECK_نسخہ(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    اگر (sb != تہی)
    {
	dragging = TRUE;

	اگر (sb->wp != تہی && GDK_IS_DRAWABLE(sb->id->کھڑکی_2))
	{
	    صحیح			x;
	    صحیح			y;
	    GdkModifierType	state;
	    صحیح			چوڑائی;
	    صحیح			اونچائی;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_کھڑکی_get_اشارہ_گر(sb->id->کھڑکی_2, &x, &y, &state);
	    gdk_کھڑکی_get_سائز(sb->id->کھڑکی_2, &چوڑائی, &اونچائی);
	    اگر (x >= 0 && x < چوڑائی && y >= 0 && y < اونچائی)
	    {
		اگر (y < چوڑائی)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    قدر = sb->wp->w_topline - 2;
		}
		ورنہ اگر (y > اونچائی - چوڑائی)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    قدر = sb->wp->w_topline;
		}
	    }
	}
    }
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, قدر, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    خالی
gui_mch_create_scrollbar(scrollbar_T *sb, صحیح orient)
{
    اگر (orient == SBAR_HORIZ)
#اگر GTK_CHECK_نسخہ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, تہی);
#ورنہ
	sb->id = gtk_hscrollbar_new(تہی);
#اگر_ختم
    ورنہ اگر (orient == SBAR_VERT)
#اگر GTK_CHECK_نسخہ(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, تہی);
#ورنہ
	sb->id = gtk_vscrollbar_new(تہی);
#اگر_ختم

    اگر (sb->id == تہی)
	واپس;

    GtkAdjustment *adjustment;

    gtk_widget_سیٹ_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_قدر_بدلا),
	    GINT_مقصد_اشارہ_گر(sb->ident));
    gui_mch_update();
}

    خالی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    اگر (sb->id != تہی)
    {
	gtk_widget_destroy(sb->id);
	sb->id = تہی;
    }
    gui_mch_update();
}

#اگر defined(FEAT_BROWSE)
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
gui_mch_browse(صحیح saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext غیر_مستعمل,
	       char_u *initdir,
	       char_u *filter)
{
# اگر GTK_CHECK_نسخہ(3,20,0)
    GtkFileChooserNative	*fc;
# ورنہ
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    ثابت gchar		*domain = "Gtk";

    title = CONVERT_مقصد_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    اگر (initdir == تہی || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ورنہ اگر (vim_بھرا_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    جوڑو_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_سیٹ_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, تہی);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# اگر GTK_CHECK_نسخہ(3,20,0)
    fc = gtk_file_chooser_native_new(
# ورنہ
    fc = gtk_file_chooser_dialog_new(
# endif
	    (ثابت gchar *)title,
	    GTK_کھڑکی(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# اگر GTK_CHECK_نسخہ(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# ورنہ
#  اگر GTK_CHECK_نسخہ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  ورنہ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    تہی);
# endif
    gtk_file_chooser_سیٹ_current_folder(GTK_FILE_CHOOSER(fc),
						       (ثابت gchar *)dirbuf);

    اگر (filter != تہی && *filter != NUL)
    {
	صحیح     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	اگر (patt != تہی)
	{
	    جب (p != تہی && *p != NUL)
	    {
		اگر (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    اگر (*p == '\t')
			gtk_file_filter_سیٹ_name(gfilter, (gchar *)patt);
		    ورنہ
		    {
			gtk_file_filter_جوڑو_نمونہ(gfilter, (gchar *)patt);
			اگر (*p == '\n')
			{
			    gtk_file_chooser_جوڑو_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    اگر (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ورنہ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    اگر (saving && dflt != تہی && *dflt != NUL)
	gtk_file_chooser_سیٹ_current_name(GTK_FILE_CHOOSER(fc), (حرف *)dflt);

    gui.browse_fname = تہی;
# اگر GTK_CHECK_نسخہ(3,20,0)
    اگر (gtk_native_dialog_تسلسل(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# ورنہ
    اگر (gtk_dialog_تسلسل(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	حرف *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# اگر GTK_CHECK_نسخہ(3,20,0)
    g_object_unref(fc);
# ورنہ
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_مقصد_UTF8_FREE(title);
    اگر (gui.browse_fname == تہی)
	واپس تہی;

    // shorten the file name if possible
    واپس vim_strsave(shorten_fname1(gui.browse_fname));
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
# اگر defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = تہی;

    title = CONVERT_مقصد_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (ثابت gchar *)title,
	    GTK_کھڑکی(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  اگر GTK_CHECK_نسخہ(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  ورنہ
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    تہی);

    CONVERT_مقصد_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    اگر (initdir == تہی || *initdir == NUL
	       || vim_بھرا_Name(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    جوڑو_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_سیٹ_filename(GTK_FILE_CHOOSER(dirdlg),
						      (ثابت gchar *)dirbuf);

    // Run the dialog.
    اگر (gtk_dialog_تسلسل(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    اگر (dirname == تہی)
	واپس تہی;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    واپس p;

# ورنہ // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    واپس gui_mch_browse(0, title, تہی, تہی, initdir, تہی);
# endif // !defined(GTK_FILE_CHOOSER)
}


#اگر_ختم	// FEAT_BROWSE

#اگر defined(FEAT_GUI_DIALOG)

    ساکن GtkWidget *
create_پیغام_dialog(صحیح type, char_u *title, char_u *پیغام_2)
{
    GtkWidget	    *dialog;
    Gtk_پیغام_Type  پیغام_type;

    انتخاب (type)
    {
	حالت VIM_ERROR:	    پیغام_type = GTK_پیغام_ERROR;	 توڑو;
	حالت VIM_WARNING:   پیغام_type = GTK_پیغام_WARNING;	 توڑو;
	حالت VIM_QUESTION:  پیغام_type = GTK_پیغام_QUESTION; توڑو;
	طےشدہ:	    پیغام_type = GTK_پیغام_INFO;	 توڑو;
    }

    پیغام_2 = CONVERT_مقصد_UTF8(پیغام_2);
    dialog  = gtk_پیغام_dialog_new(GTK_کھڑکی(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_والد,
				     پیغام_type,
				     GTK_BUTTONS_NONE,
				     "%s", (ثابت حرف *)پیغام_2);
    CONVERT_مقصد_UTF8_FREE(پیغام_2);

    اگر (title != تہی)
    {
	title = CONVERT_مقصد_UTF8(title);
	gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(dialog), (ثابت حرف *)title);
	CONVERT_مقصد_UTF8_FREE(title);
    }
    ورنہ اگر (type == VIM_GENERIC)
    {
	gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(dialog), "VIM");
    }

    واپس dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    ساکن حرف **
بانٹو_button_string(char_u *button_string, صحیح *n_buttons)
{
    حرف	    **array;
    char_u	    *p;
    غیرعلامتی صحیح    گنتی = 1;

    برائے (p = button_string; *p != NUL; ++p)
	اگر (*p == DLG_BUTTON_SEP)
	    ++گنتی;

    array = ALLOC_MULT(حرف *, گنتی + 1);
    گنتی = 0;

    اگر (array != تہی)
    {
	array[گنتی++] = (حرف *)button_string;
	برائے (p = button_string; *p != NUL; )
	{
	    اگر (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[گنتی++] = (حرف *)p;
	    }
	    ورنہ اگر (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ورنہ
		MB_PTR_ADV(p);
	}
	array[گنتی] = تہی; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = گنتی;
    واپس array;
}

# اگر !GTK_CHECK_نسخہ(3,10,0)
    ساکن حرف **
بانٹو_button_translation(ثابت حرف *پیغام_2)
{
    حرف    **buttons = تہی;
    char_u  *str;
    صحیح	    n_buttons = 0;
    صحیح	    n_expected = 1;

    برائے (str = (char_u *)پیغام_2; *str != NUL; ++str)
	اگر (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(پیغام_2);
    اگر (str != تہی)
    {
	اگر (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, تہی);
	ورنہ
	    str = vim_strsave(str);

	اگر (str != تہی)
	    buttons = بانٹو_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    اگر (buttons == تہی || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = تہی;
	str = vim_strsave((char_u *)پیغام_2);

	اگر (str != تہی)
	    buttons = بانٹو_button_string(str, &n_buttons);
	اگر (buttons == تہی)
	    vim_free(str);
    }

    واپس buttons;
}

    ساکن صحیح
button_equal(ثابت حرف *a, ثابت حرف *b)
{
    جب (*a != '\0' && *b != '\0')
    {
	اگر (*a == '_' && *++a == '\0')
	    توڑو;
	اگر (*b == '_' && *++b == '\0')
	    توڑو;

	اگر (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    واپس FALSE;

	a = g_utf8_اگلا_char(a);
	b = g_utf8_اگلا_char(b);
    }

    واپس (*a == '\0' && *b == '\0');
}
# endif

    ساکن خالی
dialog_جوڑو_buttons(GtkDialog *dialog, char_u *button_string)
{
# اگر !GTK_CHECK_نسخہ(3,10,0)
    حرف    **ok;
    حرف    **ync;  // "yes no cancel"
# endif
    حرف    **buttons;
    صحیح	    n_buttons = 0;
    صحیح	    idx;

    button_string = vim_strsave(button_string); // must be writable
    اگر (button_string == تہی)
	واپس;

    // Check 'v' flag in 'guioptions': vertical button placement.
    اگر (vim_strchr(p_go, GO_VERTICAL) != تہی)
    {
# اگر !GTK_CHECK_نسخہ(3,0,0)
	GtkWidget	*vbutton_box;

	vbutton_box = gtk_vbutton_box_new();
	gtk_widget_show(vbutton_box);
	gtk_box_pack_انتہا(GTK_BOX(GTK_DIALOG(dialog)->vbox),
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
# اگر !GTK_CHECK_نسخہ(3,10,0)
    ok	    = بانٹو_button_translation(N_("&Ok"));
    ync     = بانٹو_button_translation(N_("&Yes\n&No\n&Cancel"));
# endif
    buttons = بانٹو_button_string(button_string, &n_buttons);

    /*
     * Yes, the buttons are in reversed order to match the GNOME 2 desktop
     * environment.  Don't hit me -- it's all about consistency.
     * Well, apparently somebody changed his mind: with GTK 2.2.4 it works the
     * other way around...
     */
    برائے (idx = 1; idx <= n_buttons; ++idx)
    {
	حرف	*label;
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
# اگر !GTK_CHECK_نسخہ(3,10,0)
	اگر (ok != تہی && ync != تہی) // almost impossible to fail
	{
	    اگر	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    ورنہ اگر (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    ورنہ اگر (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    ورنہ اگر (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    ورنہ اگر (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    ورنہ اگر (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    ورنہ اگر (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    ورنہ اگر (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_مقصد_UTF8((char_u *)label);
	gtk_dialog_جوڑو_button(dialog, (ثابت gchar *)label8, idx);
	CONVERT_مقصد_UTF8_FREE(label8);
    }

# اگر !GTK_CHECK_نسخہ(3,10,0)
    اگر (ok != تہی)
	vim_free(*ok);
    اگر (ync != تہی)
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
قسمتعریف ساخت _DialogInfo
{
    صحیح		ignore_enter;	    // no default button, ignore "Enter"
    صحیح		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    ساکن gboolean
dialog_کلید_press_event_cb(GtkWidget *widget, Gdk_Event_کلید *event, gpointer ڈیٹا)
{
    DialogInfo *di = (DialogInfo *)ڈیٹا;

    // Ignore hitting Enter (or Space) when there is no default button.
    اگر (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	واپس TRUE;
    ورنہ    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    اگر (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	واپس TRUE;
    }

    اگر (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_نقاب()) == 0)
    {
	واپس gtk_کھڑکی_mnemonic_activate(
		   GTK_کھڑکی(widget), event->keyval,
		   gtk_کھڑکی_get_mnemonic_modifier(GTK_کھڑکی(widget)));
    }

    واپس FALSE; // continue emission
}

    صحیح
gui_mch_dialog(صحیح	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*پیغام_2,   // message text
	       char_u	*buttons,   // names of buttons
	       صحیح	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       صحیح	ex_cmd غیر_مستعمل)
{
    GtkWidget	*dialog;
    GtkWidget	*اندراج_2 = تہی;
    char_u	*متن;
    صحیح		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_فعال;

    dialog = create_پیغام_dialog(type, title, پیغام_2);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_جوڑو_buttons(GTK_DIALOG(dialog), buttons);

    اگر (textfield != تہی)
    {
	GtkWidget *alignment;

	اندراج_2 = gtk_اندراج_new();
	gtk_widget_show(اندراج_2);

	// Make Enter work like pressing OK.
	gtk_اندراج_سیٹ_activates_default(GTK_اندراج(اندراج_2), TRUE);

	متن = CONVERT_مقصد_UTF8(textfield);
	gtk_اندراج_سیٹ_متن(GTK_اندراج(اندراج_2), (ثابت حرف *)متن);
	CONVERT_مقصد_UTF8_FREE(متن);

# اگر GTK_CHECK_نسخہ(3,14,0)
	gtk_widget_سیٹ_halign(GTK_WIDGET(اندراج_2), GTK_ALIGN_CENTER);
	gtk_widget_سیٹ_valign(GTK_WIDGET(اندراج_2), GTK_ALIGN_CENTER);
	gtk_widget_سیٹ_hexpand(GTK_WIDGET(اندراج_2), TRUE);
	gtk_widget_سیٹ_vexpand(GTK_WIDGET(اندراج_2), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# ورنہ
	alignment = gtk_alignment_new((اعشاری)0.5, (اعشاری)0.5,
						      (اعشاری)1.0, (اعشاری)1.0);
# endif
	gtk_container_جوڑو(GTK_CONTAINER(alignment), اندراج_2);
	gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# اگر GTK_CHECK_نسخہ(3,0,0)
	{
	    GtkWidget * ثابت vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_آغاز(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# ورنہ
	gtk_box_pack_آغاز(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    ورنہ
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_کلید_press_event_cb), &dialoginfo);

    اگر (def_but > 0)
    {
	gtk_dialog_سیٹ_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    ورنہ
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_تسلسل(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    اگر (response != GTK_RESPONSE_NONE)
    {
	اگر (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	اگر (textfield != تہی)
	{
	    متن = (char_u *)gtk_اندراج_get_متن(GTK_اندراج(اندراج_2));
	    متن = CONVERT_FROM_UTF8(متن);

	    vim_strncpy(textfield, متن, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(متن);
	}
	gtk_widget_destroy(dialog);
    }

    اگر (gui.dialogs_فعال > 0)
	--gui.dialogs_فعال;
    واپس response > 0 ? response : 0;
}

#اگر_ختم // FEAT_GUI_DIALOG


#اگر defined(FEAT_MENU)

    خالی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# اگر defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  اگر !GTK_CHECK_نسخہ(3,10,0)
    اگر (xic != تہی && g_object_get_ڈیٹا(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == تہی)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_شے_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_مقصد_UTF8(name);
	menuitem = gtk_menu_شے_new_with_mnemonic((ثابت حرف *)name);
	CONVERT_مقصد_UTF8_FREE(name);
	gtk_widget_show(menuitem);

	submenu = gtk_menu_new();
	gtk_menu_شے_سیٹ_submenu(GTK_MENU_شے(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	gtk_im_multicontext_append_menuitems(GTK_IM_MULTICONTEXT(xic),
					     GTK_MENU_SHELL(submenu));
	g_object_سیٹ_ڈیٹا(G_OBJECT(menu->submenu_id),
			  "vim-has-im-menu", GINT_مقصد_اشارہ_گر(TRUE));
    }
#  endif
# endif // FEAT_XIM

# اگر GTK_CHECK_نسخہ(3,22,2)
    {
	GdkEventButton trigger;

	// A pseudo event to have gtk_menu_popup_at_pointer() work. Since the
	// function calculates the popup menu position on the basis of the
	// actual pointer position when it is invoked, the fields x, y, x_root
	// and y_root are set to zero for convenience.
	trigger.type       = GDK_BUTTON_PRESS;
	trigger.کھڑکی_2     = gtk_widget_get_کھڑکی(gui.drawarea);
	trigger.send_event = FALSE;
	trigger.وقت       = gui.event_وقت;
	trigger.x	   = 0.0;
	trigger.y	   = 0.0;
	trigger.axes       = تہی;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = تہی;
	trigger.x_جڑ     = 0.0;
	trigger.y_جڑ     = 0.0;

	gtk_menu_popup_at_اشارہ_گر(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# ورنہ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   تہی, تہی,
		   (GtkMenuPositionFunc)تہی, تہی,
		   3U, gui.event_وقت);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
ساکن صحیح popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    ساکن خالی
popup_menu_position_func(GtkMenu *menu غیر_مستعمل,
			 gint *x, gint *y,
			 gboolean *ڈالو_اندر غیر_مستعمل,
			 gpointer user_ڈیٹا غیر_مستعمل)
{
    gdk_کھڑکی_get_origin(gtk_widget_get_کھڑکی(gui.drawarea), x, y);

    اگر (popup_mouse_pos)
    {
	صحیح	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    ورنہ اگر (curwin != تہی && gui.drawarea != تہی &&
	     gtk_widget_get_کھڑکی(gui.drawarea) != تہی)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    خالی
gui_make_popup(char_u *راستہ_name, صحیح mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_ڈھونڈو_menu(راستہ_name);
    اگر (menu == تہی || menu->submenu_id == تہی)
	واپس;

# اگر GTK_CHECK_نسخہ(3,22,2)
    Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(gui.drawarea);
    GdkEventButton trigger;

    // A pseudo event to have gtk_menu_popup_at_*() functions work. Since
    // the position where the menu pops up is automatically adjusted by
    // the functions, none of the fields x, y, x_root and y_root has to be
    // set to a specific value here; therefore, they are set to zero for
    // convenience.
    trigger.type       = GDK_BUTTON_PRESS;
    trigger.کھڑکی_2     = win;
    trigger.send_event = FALSE;
    trigger.وقت       = GDK_CURRENT_وقت;
    trigger.x	   = 0.0;
    trigger.y	   = 0.0;
    trigger.axes       = تہی;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = تہی;
    trigger.x_جڑ     = 0.0;
    trigger.y_جڑ     = 0.0;

    اگر (mouse_pos)
	gtk_menu_popup_at_اشارہ_گر(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    ورنہ
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_کھڑکی_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(تہی, &rect.x, &rect.y, تہی, تہی);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# ورنہ
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    تہی, تہی,
	    &popup_menu_position_func, تہی,
	    0U, (guint32)GDK_CURRENT_وقت);
# endif
}

#اگر_ختم // FEAT_MENU


/*
 * We don't create it twice.
 */

قسمتعریف ساخت Shared_ڈھونڈو_Replace
{
    GtkWidget *dialog;	// the main dialog widget
    GtkWidget *wword;	// 'Whole word only' check button
    GtkWidget *mcase;	// 'Match case' check button
    GtkWidget *اوپر;	// search direction 'Up' radio button
    GtkWidget *نیچے_کو;	// search direction 'Down' radio button
    GtkWidget *what;	// 'Find what' entry text widget
    GtkWidget *with;	// 'Replace with' entry text widget
    GtkWidget *ڈھونڈو;	// 'Find Next' action button
    GtkWidget *replace;	// 'Replace With' action button
    GtkWidget *all;	// 'Replace All' action button
} Shared_ڈھونڈو_Replace_2;

ساکن Shared_ڈھونڈو_Replace_2 ڈھونڈو_widgets = {تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی};
ساکن Shared_ڈھونڈو_Replace_2 repl_widgets = {تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی};

    ساکن صحیح
ڈھونڈو_کلید_press_event(
		GtkWidget	*widget غیر_مستعمل,
		Gdk_Event_کلید	*event,
		Shared_ڈھونڈو_Replace_2 *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    اگر (event->state & (GDK_CONTROL_نقاب | GDK_سرکاؤ_نقاب))
	واپس FALSE;

    // the Escape key synthesizes a cancellation action
    اگر (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	واپس TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    واپس FALSE;
}

    ساکن GtkWidget *
#اگر GTK_CHECK_نسخہ(3,10,0)
create_image_button(ثابت حرف *stock_id غیر_مستعمل,
		    ثابت حرف *label)
#ورنہ
create_image_button(ثابت حرف *stock_id,
		    ثابت حرف *label)
#اگر_ختم
{
    char_u	*متن;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    متن = CONVERT_مقصد_UTF8((char_u *)label);

#اگر GTK_CHECK_نسخہ(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_سیٹ_homogeneous(GTK_BOX(box), FALSE);
#ورنہ
    box = gtk_hbox_new(FALSE, 3);
#اگر_ختم
#اگر !GTK_CHECK_نسخہ(3,10,0)
    اگر (stock_id != تہی)
	gtk_box_pack_آغاز(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_سائز_BUTTON),
			   FALSE, FALSE, 0);
#اگر_ختم
    gtk_box_pack_آغاز(GTK_BOX(box),
		       gtk_label_new((ثابت حرف *)متن),
		       FALSE, FALSE, 0);

    CONVERT_مقصد_UTF8_FREE(متن);

#اگر GTK_CHECK_نسخہ(3,14,0)
    gtk_widget_سیٹ_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_سیٹ_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_سیٹ_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_سیٹ_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#ورنہ
    alignment = gtk_alignment_new((اعشاری)0.5, (اعشاری)0.5,
						      (اعشاری)0.0, (اعشاری)0.0);
#اگر_ختم
    gtk_container_جوڑو(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_جوڑو(GTK_CONTAINER(button), alignment);

    واپس button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    ساکن ثابت حرف *
convert_localized_پیغام(char_u **بفر, ثابت حرف *پیغام_2)
{
    اگر (output_conv.vc_type == CONV_NONE)
	واپس پیغام_2;

    vim_free(*بفر);
    *بفر = string_convert(&output_conv, (char_u *)پیغام_2, تہی);

    واپس (ثابت حرف *)*بفر;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    ساکن غیرعلامتی طویل
اندراج_get_متن_لمبائی(Gtk_اندراج *اندراج_2)
{
    g_return_val_if_fail(اندراج_2 != تہی, 0);
    g_return_val_if_fail(GTK_IS_اندراج(اندراج_2) == TRUE, 0);

#اگر GTK_CHECK_نسخہ(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    واپس gtk_اندراج_بفر_get_لمبائی(gtk_اندراج_get_بفر(اندراج_2));
#وگرنہ_اگر GTK_CHECK_نسخہ(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    واپس gtk_اندراج_get_متن_لمبائی(اندراج_2);
#ورنہ
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    واپس g_utf8_strlen(gtk_اندراج_get_متن(اندراج_2), -1);
#اگر_ختم
}

    ساکن خالی
ڈھونڈو_replace_dialog_create(char_u *arg, صحیح do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*جدول;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    Shared_ڈھونڈو_Replace_2 *frdp;
    char_u	*اندراج_متن;
    صحیح		wword = FALSE;
    صحیح		mcase = !p_ic;
    char_u	*conv_بفر = تہی;
#تعریف_کرو CONV(پیغام_2) convert_localized_پیغام(&conv_بفر, (پیغام_2))

    frdp = (do_replace) ? (&repl_widgets) : (&ڈھونڈو_widgets);

    // Get the search string to use.
    اندراج_متن = get_ڈھونڈو_dialog_متن(arg, &wword, &mcase);

    اگر (اندراج_متن != تہی && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_متن = اندراج_متن;
	اندراج_متن = string_convert(&output_conv, اندراج_متن, تہی);
	vim_free(old_متن);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    اگر (frdp->dialog)
    {
	اگر (اندراج_متن != تہی)
	{
	    gtk_اندراج_سیٹ_متن(GTK_اندراج(frdp->what), (حرف *)اندراج_متن);
	    gtk_toggle_button_سیٹ_فعال(GTK_TOGGLE_BUTTON(frdp->wword),
							     (gboolean)wword);
	    gtk_toggle_button_سیٹ_فعال(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
	}
	gtk_کھڑکی_present(GTK_کھڑکی(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	اگر (do_replace && اندراج_get_متن_لمبائی(GTK_اندراج(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(اندراج_متن);
	واپس;
    }

    frdp->dialog = gtk_dialog_new();
#اگر GTK_CHECK_نسخہ(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#ورنہ
    gtk_dialog_سیٹ_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#اگر_ختم
    gtk_کھڑکی_سیٹ_transient_for(GTK_کھڑکی(frdp->dialog), GTK_کھڑکی(gui.mainwin));
    gtk_کھڑکی_سیٹ_destroy_with_والد(GTK_کھڑکی(frdp->dialog), TRUE);

    اگر (do_replace)
    {
	gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    ورنہ
    {
	gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#اگر GTK_CHECK_نسخہ(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_سیٹ_homogeneous(GTK_BOX(hbox), FALSE);
#ورنہ
    hbox = gtk_hbox_new(FALSE, 0);
#اگر_ختم
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(hbox), 10);
#اگر GTK_CHECK_نسخہ(3,0,0)
    {
	GtkWidget * ثابت dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_جوڑو(GTK_CONTAINER(dialog_vbox), hbox);
    }
#ورنہ
    gtk_container_جوڑو(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#اگر_ختم

    اگر (do_replace)
#اگر GTK_CHECK_نسخہ(3,4,0)
	جدول = gtk_grid_new();
#ورنہ
	جدول = gtk_جدول_new(1024, 4, FALSE);
#اگر_ختم
    ورنہ
#اگر GTK_CHECK_نسخہ(3,4,0)
	جدول = gtk_grid_new();
#ورنہ
	جدول = gtk_جدول_new(1024, 3, FALSE);
#اگر_ختم
    gtk_box_pack_آغاز(GTK_BOX(hbox), جدول, TRUE, TRUE, 0);
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(جدول), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#اگر GTK_CHECK_نسخہ(3,16,0)
    gtk_label_سیٹ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_سیٹ_yalign(GTK_LABEL(tmp), 0.5);
#وگرنہ_اگر GTK_CHECK_نسخہ(3,14,0)
    {
	GValue align_val = G_قدر_INIT;

	g_قدر_init(&align_val, G_TYPE_FLOAT);

	g_قدر_سیٹ_float(&align_val, 0.0);
	g_object_سیٹ_property(G_OBJECT(tmp), "xalign", &align_val);

	g_قدر_سیٹ_float(&align_val, 0.5);
	g_object_سیٹ_property(G_OBJECT(tmp), "yalign", &align_val);

	g_قدر_unset(&align_val);
    }
#ورنہ
    gtk_misc_سیٹ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#اگر_ختم
#اگر GTK_CHECK_نسخہ(3,4,0)
    gtk_grid_attach(GTK_GRID(جدول), tmp, 0, 0, 2, 1);
#ورنہ
    gtk_جدول_attach(GTK_جدول(جدول), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم
    frdp->what = gtk_اندراج_new();
    sensitive = (اندراج_متن != تہی && اندراج_متن[0] != NUL);
    اگر (اندراج_متن != تہی)
	gtk_اندراج_سیٹ_متن(GTK_اندراج(frdp->what), (حرف *)اندراج_متن);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(اندراج_بدلا_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(ڈھونڈو_کلید_press_event),
			   (gpointer) frdp);
#اگر GTK_CHECK_نسخہ(3,4,0)
    gtk_grid_attach(GTK_GRID(جدول), frdp->what, 2, 0, 5, 1);
#ورنہ
    gtk_جدول_attach(GTK_جدول(جدول), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم

    اگر (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#اگر GTK_CHECK_نسخہ(3,16,0)
	gtk_label_سیٹ_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_سیٹ_yalign(GTK_LABEL(tmp), 0.5);
#وگرنہ_اگر GTK_CHECK_نسخہ(3,14,0)
	{
	    GValue align_val = G_قدر_INIT;

	    g_قدر_init(&align_val, G_TYPE_FLOAT);

	    g_قدر_سیٹ_float(&align_val, 0.0);
	    g_object_سیٹ_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_قدر_سیٹ_float(&align_val, 0.5);
	    g_object_سیٹ_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_قدر_unset(&align_val);
	}
#ورنہ
	gtk_misc_سیٹ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#اگر_ختم
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), tmp, 0, 1, 2, 1);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم
	frdp->with = gtk_اندراج_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(ڈھونڈو_replace_cb),
			 GINT_مقصد_اشارہ_گر(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(ڈھونڈو_کلید_press_event),
			       (gpointer) frdp);
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->with, 2, 1, 5, 1);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), frdp->with, 1, 1024, 1, 2,
			 GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم

	/*
	 * Make the entry activation only change the input focus onto the
	 * with item.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(اندراج_activate_cb), frdp->with);
    }
    ورنہ
    {
	/*
	 * Make the entry activation do the search.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(ڈھونڈو_replace_cb),
			 GINT_مقصد_اشارہ_گر(FRD_FINDNEXT));
    }

    // whole word only button
    frdp->wword = gtk_check_button_new_with_label(CONV(_("Match whole word only")));
    gtk_toggle_button_سیٹ_فعال(GTK_TOGGLE_BUTTON(frdp->wword),
							(gboolean)wword);
    اگر (do_replace)
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->wword, 0, 2, 5, 1);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم
    ورنہ
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->wword, 0, 3, 5, 1);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_سیٹ_فعال(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    اگر (do_replace)
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->mcase, 0, 3, 5, 1);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم
    ورنہ
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->mcase, 0, 4, 5, 1);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#اگر_ختم

    tmp = gtk_چوکھٹا_new(CONV(_("Direction")));
    اگر (do_replace)
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), tmp, 5, 2, 2, 4);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#اگر_ختم
    ورنہ
#اگر GTK_CHECK_نسخہ(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), tmp, 5, 2, 1, 3);
#ورنہ
	gtk_جدول_attach(GTK_جدول(جدول), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#اگر_ختم
#اگر GTK_CHECK_نسخہ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سیٹ_homogeneous(GTK_BOX(vbox), FALSE);
#ورنہ
    vbox = gtk_vbox_new(FALSE, 0);
#اگر_ختم
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(vbox), 0);
    gtk_container_جوڑو(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->اوپر = gtk_radio_button_new_with_label(تہی, CONV(_("Up")));
    gtk_box_pack_آغاز(GTK_BOX(vbox), frdp->اوپر, TRUE, TRUE, 0);
    frdp->نیچے_کو = gtk_radio_button_new_with_label(
			gtk_radio_button_get_گروہ(GTK_RADIO_BUTTON(frdp->اوپر)),
			CONV(_("Down")));
    gtk_toggle_button_سیٹ_فعال(GTK_TOGGLE_BUTTON(frdp->نیچے_کو), TRUE);
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_آغاز(GTK_BOX(vbox), frdp->نیچے_کو, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#اگر GTK_CHECK_نسخہ(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#ورنہ
    actionarea = gtk_vbutton_box_new();
#اگر_ختم
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_انتہا(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#اگر GTK_CHECK_نسخہ(3,10,0)
    frdp->ڈھونڈو = create_image_button(تہی, _("Find Next"));
#ورنہ
    frdp->ڈھونڈو = create_image_button(GTK_STOCK_ڈھونڈو, _("Find Next"));
#اگر_ختم
    gtk_widget_سیٹ_sensitive(frdp->ڈھونڈو, sensitive);

    g_signal_connect(G_OBJECT(frdp->ڈھونڈو), "clicked",
		     G_CALLBACK(ڈھونڈو_replace_cb),
		     (do_replace) ? GINT_مقصد_اشارہ_گر(FRD_R_FINDNEXT)
				  : GINT_مقصد_اشارہ_گر(FRD_FINDNEXT));

    gtk_widget_سیٹ_can_default(frdp->ڈھونڈو, TRUE);
    gtk_box_pack_آغاز(GTK_BOX(actionarea), frdp->ڈھونڈو, FALSE, FALSE, 0);
    gtk_widget_grab_default(frdp->ڈھونڈو);

    اگر (do_replace)
    {
	// 'Replace' button
#اگر GTK_CHECK_نسخہ(3,10,0)
	frdp->replace = create_image_button(تہی, _("Replace"));
#ورنہ
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#اگر_ختم
	gtk_widget_سیٹ_sensitive(frdp->replace, sensitive);
	gtk_widget_سیٹ_can_default(frdp->ڈھونڈو, TRUE);
	gtk_box_pack_آغاز(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(ڈھونڈو_replace_cb),
			 GINT_مقصد_اشارہ_گر(FRD_REPLACE));

	// 'Replace All' button
#اگر GTK_CHECK_نسخہ(3,10,0)
	frdp->all = create_image_button(تہی, _("Replace All"));
#ورنہ
	frdp->all = create_image_button(GTK_STOCK_CONVERT, _("Replace All"));
#اگر_ختم
	gtk_widget_سیٹ_sensitive(frdp->all, sensitive);
	gtk_widget_سیٹ_can_default(frdp->all, TRUE);
	gtk_box_pack_آغاز(GTK_BOX(actionarea), frdp->all, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->all), "clicked",
			 G_CALLBACK(ڈھونڈو_replace_cb),
			 GINT_مقصد_اشارہ_گر(FRD_REPLACEALL));
    }

    // 'Cancel' button
#اگر GTK_CHECK_نسخہ(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#ورنہ
    tmp = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#اگر_ختم
    gtk_widget_سیٹ_can_default(tmp, TRUE);
    gtk_box_pack_انتہا(GTK_BOX(actionarea), tmp, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(tmp),
			     "clicked", G_CALLBACK(gtk_widget_hide),
			     G_OBJECT(frdp->dialog));
    g_signal_connect_swapped(G_OBJECT(frdp->dialog),
			     "delete-event", G_CALLBACK(gtk_widget_hide_on_delete),
			     G_OBJECT(frdp->dialog));

#اگر GTK_CHECK_نسخہ(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#ورنہ
    tmp = gtk_vseparator_new();
#اگر_ختم
    gtk_box_pack_انتہا(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#اگر GTK_CHECK_نسخہ(3,0,0)
# اگر !GTK_CHECK_نسخہ(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#ورنہ
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#اگر_ختم
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(اندراج_متن);
    vim_free(conv_بفر);
#تعریف_منسوخ CONV
}

    خالی
gui_mch_ڈھونڈو_dialog(exarg_T *eap)
{
    اگر (gui.اندر_use)
	ڈھونڈو_replace_dialog_create(eap->arg, FALSE);
}

    خالی
gui_mch_replace_dialog(exarg_T *eap)
{
    اگر (gui.اندر_use)
	ڈھونڈو_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    ساکن خالی
ڈھونڈو_replace_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا)
{
    صحیح			flags;
    char_u		*ڈھونڈو_متن;
    char_u		*repl_متن;
    gboolean		direction_نیچے_کو;
    Shared_ڈھونڈو_Replace_2	*sfr;

    flags = (صحیح)(طویل)ڈیٹا;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    اگر (flags == FRD_FINDNEXT)
    {
	repl_متن = تہی;
	sfr = &ڈھونڈو_widgets;
    }
    ورنہ
    {
	repl_متن = (char_u *)gtk_اندراج_get_متن(GTK_اندراج(repl_widgets.with));
	sfr = &repl_widgets;
    }

    ڈھونڈو_متن = (char_u *)gtk_اندراج_get_متن(GTK_اندراج(sfr->what));
    direction_نیچے_کو = gtk_toggle_button_get_فعال(GTK_TOGGLE_BUTTON(sfr->نیچے_کو));

    اگر (gtk_toggle_button_get_فعال(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_پورا_لفظ;
    اگر (gtk_toggle_button_get_فعال(GTK_TOGGLE_BUTTON(sfr->mcase)))
	flags |= FRD_مطابقت_CASE;

    repl_متن = CONVERT_FROM_UTF8(repl_متن);
    ڈھونڈو_متن = CONVERT_FROM_UTF8(ڈھونڈو_متن);
    gui_do_findrepl(flags, ڈھونڈو_متن, repl_متن, direction_نیچے_کو);
    CONVERT_FROM_UTF8_FREE(repl_متن);
    CONVERT_FROM_UTF8_FREE(ڈھونڈو_متن);
}

/*
 * our usual callback function
 */
    ساکن خالی
اندراج_activate_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا)
{
    gtk_widget_grab_focus(GTK_WIDGET(ڈیٹا));
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
    ساکن خالی
اندراج_بدلا_cb(GtkWidget * اندراج_2, GtkWidget * dialog)
{
    ثابت gchar	*اندراج_متن;
    gboolean	nonempty;

    اندراج_متن = gtk_اندراج_get_متن(GTK_اندراج(اندراج_2));

    اگر (!اندراج_متن)
	واپس;			// shouldn't happen

    nonempty = (اندراج_متن[0] != '\0');

    اگر (dialog == ڈھونڈو_widgets.dialog)
	gtk_widget_سیٹ_sensitive(ڈھونڈو_widgets.ڈھونڈو, nonempty);

    اگر (dialog == repl_widgets.dialog)
    {
	gtk_widget_سیٹ_sensitive(repl_widgets.ڈھونڈو, nonempty);
	gtk_widget_سیٹ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_سیٹ_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    خالی
ex_helpfind(exarg_T *eap غیر_مستعمل)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#اگر defined(FEAT_BROWSE)
    ساکن خالی
recent_func_log_func(ثابت gchar *log_domain غیر_مستعمل,
		     GLogLevelFlags log_level غیر_مستعمل,
		     ثابت gchar *پیغام_2 غیر_مستعمل,
		     gpointer user_ڈیٹا غیر_مستعمل)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#اگر_ختم

    خالی
gui_mch_سیٹ_fullscreen(صحیح flag)
{
    اگر (flag)
	gtk_کھڑکی_fullscreen(GTK_کھڑکی(gui.mainwin));
    ورنہ
	gtk_کھڑکی_unfullscreen(GTK_کھڑکی(gui.mainwin));
}
