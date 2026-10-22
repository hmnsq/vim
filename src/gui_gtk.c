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

#ضمن "vim.h"

#که_تعریف_شوی FEAT_GUI_GTK
# include "gui_gtk_f.h"
#د_که_پای

// GTK defines MAX and MIN, but some system header files as well.  Undefine
// them and don't use them.
#که_تعریف_شوی MIN
# undef MIN
#د_که_پای
#که_تعریف_شوی MAX
# undef MAX
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GNOME
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
# إذا defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GTK
# إذا GTK_CHECK_نسخه(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
# وإلا
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# وإلا
#  include <gdk/gdkx.h>
# endif

# include <gtk/gtk.h>
#وإلا
// define these items to be able to generate prototypes without GTK
تعريفنوع صحيح GtkWidget;
# define gpointer صحيح
# define guint8 صحيح
# define GdkPixmap صحيح
# define GdkBitmap صحيح
# define GtkIconFactory صحيح
# define GtkToolbar صحيح
# define GtkAdjustment صحيح
# define gboolean صحيح
# define Gdk_Event_کلی صحيح
# define Cancel_معلومات صحيح
#د_که_پای

ساكن فراغ ثبت_activate_cb(GtkWidget *widget, gpointer معلومات);
ساكن فراغ ثبت_بدل_شوی_cb(GtkWidget *ثبت, GtkWidget *dialog);
ساكن فراغ پیدا_کړه_replace_cb(GtkWidget *widget, gpointer معلومات);
#إذا defined(FEAT_BROWSE)
ساكن فراغ recent_func_log_func(
	ثابت gchar *log_domain,
	GLogLevelFlags log_level,
	ثابت gchar *پیغام_2,
	gpointer user_معلومات);
#د_که_پای

#إذا defined(FEAT_TOOLBAR)
/*
 * Table from BuiltIn## icon indices to GTK+ stock IDs.  Order must exactly
 * match toolbar_names[] in menu.c!  All stock icons including the "vim-*"
 * ones can be overridden in your gtkrc file.
 */
# إذا GTK_CHECK_نسخه(3,10,0)
ساكن ثابت حرف * ثابت menu_themed_نومونه[] =
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
#  إذا GTK_CHECK_نسخه(3,14,0)
    // Use the file names in gui_gtk_res.xml, cutting off the extension.
    // Similar changes follow.
    /* 11 */ "stock_vim_save_all",
    /* 12 */ "stock_vim_session_save",
    /* 13 */ "stock_vim_session_new",
    /* 14 */ "stock_vim_session_load",
#  وإلا
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
#  endif
    /* 15 */ "system-run",		// sub. GTK_STOCK_EXECUTE
    /* 16 */ "edit-find-replace",	// sub. GTK_STOCK_FIND_AND_REPLACE
    /* 17 */ "window-close",		// sub. GTK_STOCK_CLOSE, FIXME: fuzzy
#  إذا GTK_CHECK_نسخه(3,14,0)
    /* 18 */ "stock_vim_window_maximize",
    /* 19 */ "stock_vim_window_minimize",
    /* 20 */ "stock_vim_window_split",
    /* 21 */ "stock_vim_shell",
#  وإلا
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
#  endif
    /* 22 */ "go-previous",		// sub. GTK_STOCK_GO_BACK
    /* 23 */ "go-next",			// sub. GTK_STOCK_GO_FORWARD
#  إذا GTK_CHECK_نسخه(3,14,0)
    /* 24 */ "stock_vim_find_help",
#  وإلا
    /* 24 */ "vim-find-help",
#  endif
    /* 25 */ "gtk-convert",		// sub. GTK_STOCK_CONVERT
    /* 26 */ "go-jump",			// sub. GTK_STOCK_JUMP_TO
#  إذا GTK_CHECK_نسخه(3,14,0)
    /* 27 */ "stock_vim_build_tags",
    /* 28 */ "stock_vim_window_split_vertical",
    /* 29 */ "stock_vim_window_maximize_width",
    /* 30 */ "stock_vim_window_minimize_width",
#  وإلا
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
#  endif
    /* 31 */ "application-exit",	// GTK_STOCK_QUIT
};
# وإلا // !GTK_CHECK_VERSION(3,10,0)
ساكن ثابت حرف * ثابت menu_stock_ids[] =
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
    /* 10 */ GTK_STOCK_پیدا_کړه,
    /* 11 */ "vim-save-all",
    /* 12 */ "vim-session-save",
    /* 13 */ "vim-session-new",
    /* 14 */ "vim-session-load",
    /* 15 */ GTK_STOCK_EXECUTE,
    /* 16 */ GTK_STOCK_پیدا_کړه_AND_REPLACE,
    /* 17 */ GTK_STOCK_CLOSE,		// FIXME: fuzzy
    /* 18 */ "vim-window-maximize",
    /* 19 */ "vim-window-minimize",
    /* 20 */ "vim-window-split",
    /* 21 */ "vim-shell",
    /* 22 */ GTK_STOCK_GO_شا,
    /* 23 */ GTK_STOCK_GO_FORWARD,
    /* 24 */ "vim-find-help",
    /* 25 */ GTK_STOCK_CONVERT,
    /* 26 */ GTK_STOCK_JUMP_منزل,
    /* 27 */ "vim-build-tags",
    /* 28 */ "vim-window-split-vertical",
    /* 29 */ "vim-window-maximize-width",
    /* 30 */ "vim-window-minimize-width",
    /* 31 */ GTK_STOCK_QUIT
};
# endif // !GTK_CHECK_VERSION(3,10,0)

# ifdef USE_GRESOURCE
#  إذا !GTK_CHECK_نسخه(3,10,0)
تعريفنوع بنية Icon_نومونه {
    ثابت حرف *icon_name;
    ثابت حرف *file_name;
} Icon_نومونه;

ساكن Icon_نومونه stock_vim_icons[] = {
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
    { عدم, عدم }
};
#  endif
# endif // USE_G_RESOURCE

# ifndef USE_GRESOURCE
    ساكن فراغ
جمع_کړه_stock_icon(GtkIconFactory	*factory,
	       ثابت حرف	*stock_id,
	       ثابت guint8	*inline_معلومات,
	       صحيح		معلومات_اوږدتیا)
{
    GdkPixbuf	*pixbuf;
    Gtk_Icon_سېټ	*icon_سېټ;

    pixbuf = gdk_pixbuf_new_from_inline(معلومات_اوږدتیا, inline_معلومات, FALSE, عدم);
    icon_سېټ = gtk_icon_سېټ_new_from_pixbuf(pixbuf);

    gtk_icon_factory_جمع_کړه(factory, stock_id, icon_سېټ);

    gtk_icon_سېټ_unref(icon_سېټ);
    g_object_unref(pixbuf);
}
# endif

    ساكن صحيح
lookup_menu_iconfile(char_u *iconfile, char_u *dest)
{
    expand_env(iconfile, dest, MAXPATHL);

    إذا (mch_is_ډک_Name(dest))
    {
	أرجع vim_fexists(dest);
    }
    وإلا
    {
	ساكن ثابت حرف   suffixes[][4] = {"png", "xpm", "bmp"};
	char_u		    buf[MAXPATHL];
	بینښه صحيح	    i;

	لكل (i = 0; i < G_N_ELEMENTS(suffixes); ++i)
	    إذا (gui_پیدا_کړه_bitmap(dest, buf, (حرف *)suffixes[i]) == OK)
	    {
		STRCPY(dest, buf);
		أرجع TRUE;
	    }

	أرجع FALSE;
    }
}

    ساكن GtkWidget *
بارول_menu_iconfile(char_u *name, Gtk_Icon_اندازه icon_اندازه)
{
    GtkWidget	    *image = عدم;
# إذا GTK_CHECK_نسخه(3,10,0)
    صحيح		     pixel_اندازه = -1;

    بدل (icon_اندازه)
    {
	حالة GTK_ICON_اندازه_MENU:
	    pixel_اندازه = 16;
	    اكسر;
	حالة GTK_ICON_اندازه_SMALL_TOOLBAR:
	    pixel_اندازه = 16;
	    اكسر;
	حالة GTK_ICON_اندازه_LARGE_TOOLBAR:
	    pixel_اندازه = 24;
	    اكسر;
	حالة GTK_ICON_اندازه_BUTTON:
	    pixel_اندازه = 16;
	    اكسر;
	حالة GTK_ICON_اندازه_DND:
	    pixel_اندازه = 32;
	    اكسر;
	حالة GTK_ICON_اندازه_DIALOG:
	    pixel_اندازه = 48;
	    اكسر;
	حالة GTK_ICON_اندازه_INVALID:
	    // FALLTHROUGH
	افتراضي:
	    pixel_اندازه = 0;
	    اكسر;
    }

    إذا (pixel_اندازه > 0 || pixel_اندازه == -1)
    {
	GdkPixbuf * ثابت pixbuf
	    = gdk_pixbuf_new_from_file_at_scale((ثابت حرف *)name,
		pixel_اندازه, pixel_اندازه, TRUE, عدم);
	إذا (pixbuf != عدم)
	{
	    image = gtk_image_new_from_pixbuf(pixbuf);
	    g_object_unref(pixbuf);
	}
    }
    إذا (image == عدم)
	image = gtk_image_new_from_icon_name("image-missing", icon_اندازه);

    أرجع image;
# وإلا // !GTK_CHECK_VERSION(3,10,0)
    Gtk_Icon_سېټ	    *icon_سېټ;
    Gtk_Icon_سرچینه   *icon_سرچینه;

    /*
     * Rather than loading the icon directly into a GtkImage, create
     * a new GtkIconSet and put it in there.  This way we can easily
     * scale the toolbar icons on the fly when needed.
     */
    icon_سېټ = gtk_icon_سېټ_new();
    icon_سرچینه = gtk_icon_سرچینه_new();

    gtk_icon_سرچینه_سېټ_filename(icon_سرچینه, (ثابت حرف *)name);
    gtk_icon_سېټ_جمع_کړه_سرچینه(icon_سېټ, icon_سرچینه);

    image = gtk_image_new_from_icon_سېټ(icon_سېټ, icon_اندازه);

    gtk_icon_سرچینه_free(icon_سرچینه);
    gtk_icon_سېټ_unref(icon_سېټ);

    أرجع image;
# endif // !GTK_CHECK_VERSION(3,10,0)
}

    ساكن GtkWidget *
create_menu_icon(vimmenu_T *menu, Gtk_Icon_اندازه icon_اندازه)
{
    GtkWidget	*image = عدم;
    char_u	buf[MAXPATHL];

    // First use a specified "icon=" argument.
    إذا (menu->iconfile != عدم && lookup_menu_iconfile(menu->iconfile, buf))
	image = بارول_menu_iconfile(buf, icon_اندازه);

    // If not found and not builtin specified try using the menu name.
    إذا (image == عدم && !menu->icon_builtin
				     && lookup_menu_iconfile(menu->name, buf))
	image = بارول_menu_iconfile(buf, icon_اندازه);

    // Still not found?  Then use a builtin icon, a blank one as fallback.
    إذا (image == عدم)
    {
# إذا GTK_CHECK_نسخه(3,10,0)
	ثابت حرف *icon_name = عدم;
	ثابت صحيح   n_نومونه = G_N_ELEMENTS(menu_themed_نومونه);

	إذا (menu->iconidx >= 0 && menu->iconidx < n_نومونه)
	    icon_name = menu_themed_نومونه[menu->iconidx];
	إذا (icon_name == عدم)
	    icon_name = "image-missing";

	image = gtk_image_new_from_icon_name(icon_name, icon_اندازه);
# وإلا
	ثابت حرف  *stock_id;
	ثابت صحيح   n_ids = G_N_ELEMENTS(menu_stock_ids);

	إذا (menu->iconidx >= 0 && menu->iconidx < n_ids)
	    stock_id = menu_stock_ids[menu->iconidx];
	وإلا
	    stock_id = GTK_STOCK_MISSING_IMAGE;

	image = gtk_image_new_from_stock(stock_id, icon_اندازه);
# endif
    }

    أرجع image;
}

    ساكن gint
toolbar_button_focus_دننه_event(GtkWidget *widget ناکارول_شوی,
			      GdkEventFocus *event ناکارول_شوی,
			      gpointer معلومات ناکارول_شوی)
{
    // When we're in a GtkPlug, we don't have window focus events, only widget
    // focus.  To emulate stand-alone gvim, if a button gets focus (e.g.,
    // <Tab> into GtkPlug) immediately pass it to mainwin.
    إذا (gtk_socket_id != 0)
	gtk_widget_grab_focus(gui.drawarea);

    أرجع TRUE;
}
#د_که_پای // FEAT_TOOLBAR

#إذا defined(FEAT_TOOLBAR)

    فراغ
gui_gtk_ثبتوونکی_stock_icons(فراغ)
{
# ifndef USE_GRESOURCE
#  include "../pixmaps/stock_icons.h"
    GtkIconFactory *factory;

    factory = gtk_icon_factory_new();
#  define جمع_کړه_ICON(Name, معلومات_2) جمع_کړه_stock_icon(factory, Name, معلومات_2, (صحيح)حجم(معلومات_2))

    جمع_کړه_ICON("vim-build-tags",		  stock_vim_build_نښانونه);
    جمع_کړه_ICON("vim-find-help",		  stock_vim_پیدا_کړه_help);
    جمع_کړه_ICON("vim-save-all",		  stock_vim_save_all);
    جمع_کړه_ICON("vim-session-load",	  stock_vim_session_بارول);
    جمع_کړه_ICON("vim-session-new",		  stock_vim_session_new);
    جمع_کړه_ICON("vim-session-save",	  stock_vim_session_save);
    جمع_کړه_ICON("vim-shell",		  stock_vim_shell);
    جمع_کړه_ICON("vim-window-maximize",	  stock_vim_کړکۍ_maximize);
    جمع_کړه_ICON("vim-window-maximize-width", stock_vim_کړکۍ_maximize_پلنوالی);
    جمع_کړه_ICON("vim-window-minimize",	  stock_vim_کړکۍ_minimize);
    جمع_کړه_ICON("vim-window-minimize-width", stock_vim_کړکۍ_minimize_پلنوالی);
    جمع_کړه_ICON("vim-window-split",	  stock_vim_کړکۍ_وېشل);
    جمع_کړه_ICON("vim-window-split-vertical", stock_vim_کړکۍ_وېشل_vertical);

#  undef جمع_کړه_ICON

    gtk_icon_factory_جمع_کړه_default(factory);
    g_object_unref(factory);
# وإلا // defined(USE_GRESOURCE)
    ثابت حرف * ثابت لار_مختاړی = "/org/vim/gui/icon";
#  إذا GTK_CHECK_نسخه(3,14,0)
    GdkScreen    *screen = عدم;
    GtkIconTheme *icon_theme = عدم;

    إذا (GTK_IS_WIDGET(gui.mainwin))
	screen = gtk_widget_get_screen(gui.mainwin);
    وإلا
	screen = gdk_screen_get_default();
    icon_theme = gtk_icon_theme_get_for_screen(screen);
    gtk_icon_theme_جمع_کړه_resource_لار(icon_theme, لار_مختاړی);
#  elif GTK_CHECK_نسخه(3,0,0)
    Icon_نومونه *نومونه;

    لكل (نومونه = stock_vim_icons; نومونه->icon_name != عدم; نومونه++)
    {
	حرف لار[MAXPATHL];

	vim_snprintf(لار, MAXPATHL, "%s/%s", لار_مختاړی, نومونه->file_name);
	GdkPixbuf *pixbuf = عدم;
	pixbuf = gdk_pixbuf_new_from_resource(لار, عدم);
	إذا (pixbuf != عدم)
	{
	    ثابت gint اندازه = MAX(gdk_pixbuf_get_پلنوالی(pixbuf),
				  gdk_pixbuf_get_لوړوالی(pixbuf));
	    إذا (اندازه > 16)
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
		إذا (pixbuf == عدم)
		    pixbuf = src;
		وإلا
		    g_object_unref(src);
	    }
	    gtk_icon_theme_جمع_کړه_builtin_icon(نومونه->icon_name, اندازه, pixbuf);
	    g_object_unref(pixbuf);
	}
    }
#  وإلا // !GTK_CHECK_VERSION(3,0.0)
    GtkIconFactory * ثابت factory = gtk_icon_factory_new();
    Icon_نومونه *نومونه;

    لكل (نومونه = stock_vim_icons; نومونه->icon_name != عدم; نومونه++)
    {
	حرف لار[MAXPATHL];
	GdkPixbuf *pixbuf;

	vim_snprintf(لار, MAXPATHL, "%s/%s", لار_مختاړی, نومونه->file_name);
	pixbuf = gdk_pixbuf_new_from_resource(لار, عدم);
	إذا (pixbuf != عدم)
	{
	    Gtk_Icon_سېټ *icon_سېټ = gtk_icon_سېټ_new_from_pixbuf(pixbuf);
	    gtk_icon_factory_جمع_کړه(factory, نومونه->icon_name, icon_سېټ);
	    gtk_icon_سېټ_unref(icon_سېټ);
	    g_object_unref(pixbuf);
	}
    }

    gtk_icon_factory_جمع_کړه_default(factory);
    g_object_unref(factory);
#  endif // !GTK_CHECK_VERSION(3,0,0)
# endif // defined(USE_GRESOURCE)
}

#د_که_پای // FEAT_TOOLBAR

#إذا defined(FEAT_MENU)

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
    ساكن char_u *
translate_mnemonic_نښان(char_u *name, صحيح use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    صحيح	    n_underscores = 0;

    name = CONVERT_منزل_UTF8(name);
    إذا (name == عدم)
	أرجع عدم;

    لكل (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	إذا (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    إذا (buf != عدم)
    {
	pdest = buf;
	لكل (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    إذا (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    وإلا إذا (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    وإلا إذا (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    وإلا إذا (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_منزل_UTF8_FREE(name);
    أرجع buf;
}

    ساكن فراغ
menu_توکی_new(vimmenu_T *menu, GtkWidget *پلار_widget)
{
    GtkWidget	*box;
    char_u	*متن;
    صحيح		use_mnemonic;

    // It would be neat to have image menu items, but that would require major
    // changes to Vim's menu system.  Not to mention that all the translations
    // had to be updated.
    menu->id = gtk_menu_توکی_new();
# إذا GTK_CHECK_نسخه(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_box_سېټ_homogeneous(GTK_BOX(box), FALSE);
# وإلا
    box = gtk_hbox_new(FALSE, 20);
# endif

    use_mnemonic = (p_wak[0] != 'n' || !GTK_IS_MENU_BAR(پلار_widget));
    متن = translate_mnemonic_نښان(menu->name, use_mnemonic);

    menu->label = gtk_label_new_with_mnemonic((ثابت حرف *)متن);
    vim_free(متن);

    gtk_box_pack_پیل(GTK_BOX(box), menu->label, FALSE, FALSE, 0);

    إذا (menu->actext != عدم && menu->actext[0] != NUL)
    {
	متن = CONVERT_منزل_UTF8(menu->actext);

	gtk_box_pack_پای_ټکی(GTK_BOX(box),
			 gtk_label_new((ثابت حرف *)متن),
			 FALSE, FALSE, 0);

	CONVERT_منزل_UTF8_FREE(متن);
    }

    gtk_container_جمع_کړه(GTK_CONTAINER(menu->id), box);
    gtk_widget_show_all(menu->id);
}

    فراغ
gui_mch_جمع_کړه_menu(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T	*پلار;
    GtkWidget	*پلار_widget;

    إذا (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	أرجع;
    }

    پلار = menu->پلار;

    إذا ((پلار != عدم && پلار->submenu_id == عدم)
	    || !menu_is_menubar(menu->name))
	أرجع;

    پلار_widget = (پلار != عدم) ? پلار->submenu_id : gui.menubar;
    menu_توکی_new(menu, پلار_widget);

# إذا !GTK_CHECK_نسخه(3,4,0)
    // since the tearoff should always appear first, increment idx
    إذا (پلار != عدم && !menu_is_popup(پلار->name))
	++idx;
# endif

    gtk_menu_shell_ورننباسه(GTK_MENU_SHELL(پلار_widget), menu->id, idx);

    menu->submenu_id = gtk_menu_new();

    gtk_menu_سېټ_accel_ډله(GTK_MENU(menu->submenu_id), gui.accel_ډله);
    gtk_menu_توکی_سېټ_submenu(GTK_MENU_توکی(menu->id), menu->submenu_id);

# إذا !GTK_CHECK_نسخه(3,4,0)
    menu->tearoff_handle = gtk_tearoff_menu_توکی_new();
    إذا (vim_strchr(p_go, GO_TEAROFF) != عدم)
	gtk_widget_show(menu->tearoff_handle);
#  إذا GTK_CHECK_نسخه(3,0,0)
    gtk_menu_shell_prepend(GTK_MENU_SHELL(menu->submenu_id),
	    menu->tearoff_handle);
#  وإلا
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
#  endif
# endif
}

    ساكن فراغ
menu_توکی_activate(GtkWidget *widget ناکارول_شوی, gpointer معلومات)
{
    gui_menu_cb((vimmenu_T *)معلومات);
}

    ساكن فراغ
menu_توکی_select(GtkWidget *widget ناکارول_شوی, gpointer معلومات)
{
    vimmenu_T	*menu;
    char_u	*tooltip;
    ساكن صحيح	did_پیغام = FALSE;

    إذا (State & ډېرپېښ_CMDLINE)
	أرجع;
    menu = (vimmenu_T *)معلومات;
    tooltip = CONVERT_منزل_UTF8(menu->مزي_ګانې[MENU_شاخص_TIP]);
    إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
    {
	پیغام((حرف *)tooltip);
	did_پیغام = TRUE;
	setcursor();
	بهر_flush_cursor(TRUE, FALSE);
    }
    وإلا إذا (did_پیغام)
    {
	پیغام("");
	did_پیغام = FALSE;
	setcursor();
	بهر_flush_cursor(TRUE, FALSE);
    }
    CONVERT_منزل_UTF8_FREE(tooltip);
}

    فراغ
gui_mch_جمع_کړه_menu_توکی(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T *پلار;

    پلار = menu->پلار;

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(پلار->name))
    {
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR(gui.toolbar);
	menu->submenu_id = عدم;

	إذا (menu_is_separator(menu->name))
	{
#  إذا GTK_CHECK_نسخه(3,0,0)
	    Gtk_Tool_توکی *توکی = gtk_separator_tool_توکی_new();
	    gtk_separator_tool_توکی_سېټ_draw(GTK_SEPARATOR_TOOL_توکی(توکی),
		    TRUE);
	    gtk_tool_توکی_سېټ_expand(GTK_TOOL_توکی(توکی), FALSE);
	    gtk_widget_show(GTK_WIDGET(توکی));

	    gtk_toolbar_ورننباسه(toolbar, توکی, idx);
#  وإلا
	    gtk_toolbar_ورننباسه_space(toolbar, idx);
#  endif
	    menu->id = عدم;
	}
	وإلا
	{
	    char_u *متن;
	    char_u *tooltip;

	    متن    = CONVERT_منزل_UTF8(menu->dname);
	    tooltip = CONVERT_منزل_UTF8(menu->مزي_ګانې[MENU_شاخص_TIP]);
	    إذا (tooltip != عدم && !utf_valid_string(tooltip, عدم))
		// Invalid text, can happen when 'encoding' is changed.  Avoid
		// a nasty GTK error message, skip the tooltip.
		CONVERT_منزل_UTF8_FREE(tooltip);

#  إذا GTK_CHECK_نسخه(3,0,0)
	    {
		GtkWidget *icon;
		Gtk_Tool_توکی *توکی;

		icon = create_menu_icon(menu,
			gtk_toolbar_get_icon_اندازه(toolbar));
		توکی = gtk_tool_button_new(icon, (ثابت gchar *)متن);
		gtk_tool_توکی_سېټ_tooltip_متن(توکی, (ثابت gchar *)tooltip);
		g_signal_connect(G_OBJECT(توکی), "clicked",
			G_CALLBACK(&menu_توکی_activate), menu);
		gtk_widget_show_all(GTK_WIDGET(توکی));

		gtk_toolbar_ورننباسه(toolbar, توکی, idx);

		menu->id = GTK_WIDGET(توکی);
	    }
#  وإلا
	    menu->id = gtk_toolbar_ورننباسه_توکی(
		    toolbar,
		    (ثابت حرف *)متن,
		    (ثابت حرف *)tooltip,
		    عدم,
		    create_menu_icon(menu, gtk_toolbar_get_icon_اندازه(toolbar)),
		    G_CALLBACK(&menu_توکی_activate),
		    menu,
		    idx);
#  endif

	    إذا (gtk_socket_id != 0)
		g_signal_connect(G_OBJECT(menu->id), "focus-in-event",
			G_CALLBACK(toolbar_button_focus_دننه_event), عدم);

	    CONVERT_منزل_UTF8_FREE(متن);
	    CONVERT_منزل_UTF8_FREE(tooltip);
	}
    }
    وإلا
# endif // FEAT_TOOLBAR
    {
	// No parent, must be a non-menubar menu
	إذا (پلار == عدم || پلار->submenu_id == عدم)
	    أرجع;

# إذا !GTK_CHECK_نسخه(3,4,0)
	// Make place for the possible tearoff handle item.  Not in the popup
	// menu, it doesn't have a tearoff item.
	إذا (!menu_is_popup(پلار->name))
	    ++idx;
# endif

	إذا (menu_is_separator(menu->name))
	{
	    // Separator: Just add it
# إذا GTK_CHECK_نسخه(3,0,0)
	    menu->id = gtk_separator_menu_توکی_new();
# وإلا
	    menu->id = gtk_menu_توکی_new();
	    gtk_widget_سېټ_sensitive(menu->id, FALSE);
# endif
	    gtk_widget_show(menu->id);
	    gtk_menu_shell_ورننباسه(GTK_MENU_SHELL(پلار->submenu_id),
		    menu->id, idx);

	    أرجع;
	}

	// Add textual menu item.
	menu_توکی_new(menu, پلار->submenu_id);
	gtk_widget_show(menu->id);
	gtk_menu_shell_ورننباسه(GTK_MENU_SHELL(پلار->submenu_id),
		menu->id, idx);

	إذا (menu->id != عدم)
	{
	    g_signal_connect(G_OBJECT(menu->id), "activate",
			     G_CALLBACK(menu_توکی_activate), menu);
	    g_signal_connect(G_OBJECT(menu->id), "select",
			     G_CALLBACK(menu_توکی_select), menu);
	}
    }
}
#د_که_پای // FEAT_MENU


    فراغ
gui_mch_سېټ_متن_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    gui_gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#إذا defined(FEAT_MENU)
/*
 * Enable or disable accelerators for the toplevel menus.
 */
    فراغ
gui_gtk_سېټ_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	إذا (menu->id == عدم)
	    استمر;

	name = translate_mnemonic_نښان(menu->name, enable);
	gtk_label_سېټ_متن_with_mnemonic(GTK_LABEL(menu->label),
					 (ثابت حرف *)name);
	vim_free(name);
    }
}

# إذا !GTK_CHECK_نسخه(3,4,0)
    ساكن فراغ
recurse_tearoffs(vimmenu_T *menu, صحيح val)
{
    لكل (; menu != عدم; menu = menu->بل)
    {
	إذا (menu->submenu_id != عدم && menu->tearoff_handle != عدم
		&& menu->name[0] != ']' && !menu_is_popup(menu->name))
	{
	    إذا (val)
		gtk_widget_show(menu->tearoff_handle);
	    وإلا
		gtk_widget_hide(menu->tearoff_handle);
	}
	recurse_tearoffs(menu->children, val);
    }
}
# endif

# إذا GTK_CHECK_نسخه(3,4,0)
    فراغ
gui_mch_toggle_tearoffs(صحيح enable ناکارول_شوی)
{
    // Do nothing
}
# وإلا
    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    recurse_tearoffs(ریښه_menu, enable);
}
# endif
#د_که_پای // FEAT_MENU

#إذا defined(FEAT_TOOLBAR)
    ساكن صحيح
get_menu_position(vimmenu_T *menu)
{
    vimmenu_T	*غوټه;
    صحيح		idx = 0;

    لكل (غوټه = menu->پلار->children; غوټه != menu; غوټه = غوټه->بل)
    {
	g_return_val_if_fail(غوټه != عدم, -1);
	++idx;
    }

    أرجع idx;
}
#د_که_پای // FEAT_TOOLBAR


#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_menu_سېټ_tip(vimmenu_T *menu)
{
    إذا (menu->id == عدم || menu->پلار == عدم || gui.toolbar == عدم)
	أرجع;

    char_u *tooltip;

    tooltip = CONVERT_منزل_UTF8(menu->مزي_ګانې[MENU_شاخص_TIP]);
    إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
# إذا GTK_CHECK_نسخه(3,0,0)
	// Only set the tooltip when it's valid utf-8.
	gtk_widget_سېټ_tooltip_متن(menu->id, (ثابت gchar *)tooltip);
# وإلا
    // Only set the tooltip when it's valid utf-8.
    gtk_tooltips_سېټ_tip(GTK_TOOLBAR(gui.toolbar)->tooltips,
	    menu->id, (ثابت حرف *)tooltip, عدم);
# endif
    CONVERT_منزل_UTF8_FREE(tooltip);
}
#د_که_پای // FEAT_TOOLBAR


#إذا defined(FEAT_MENU)
/*
 * Destroy the machine specific menu widget.
 */
    فراغ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Don't let gtk_container_remove automatically destroy menu->id.
    إذا (menu->id != عدم)
	g_object_ref(menu->id);

    // Workaround for a spurious gtk warning in Ubuntu: "Trying to remove
    // a child that doesn't believe we're its parent."
    // Remove widget from gui.menubar before destroying it.
    إذا (menu->id != عدم && gui.menubar != عدم
			    && gtk_widget_get_پلار(menu->id) == gui.menubar)
	gtk_container_remove(GTK_CONTAINER(gui.menubar), menu->id);

# ifdef FEAT_TOOLBAR
    إذا (menu->پلار != عدم && menu_is_toolbar(menu->پلار->name))
    {
	إذا (menu_is_separator(menu->name))
#  إذا GTK_CHECK_نسخه(3,0,0)
	{
	    Gtk_Tool_توکی *توکی = عدم;

	    توکی = gtk_toolbar_get_nth_توکی(GTK_TOOLBAR(gui.toolbar),
					    get_menu_position(menu));
	    إذا (توکی != عدم)
		gtk_container_remove(GTK_CONTAINER(gui.toolbar),
				     GTK_WIDGET(توکی));
	}
#  وإلا
	    gtk_toolbar_remove_space(GTK_TOOLBAR(gui.toolbar),
				     get_menu_position(menu));
#  endif
	وإلا إذا (menu->id != عدم)
	    gtk_widget_destroy(menu->id);
    }
    وإلا
# endif // FEAT_TOOLBAR
    {
	إذا (menu->submenu_id != عدم)
	    gtk_widget_destroy(menu->submenu_id);

	إذا (menu->id != عدم)
	    gtk_widget_destroy(menu->id);
    }

    إذا (menu->id != عدم)
	g_object_unref(menu->id);
    menu->submenu_id = عدم;
    menu->id = عدم;
}
#د_که_پای // FEAT_MENU


/*
 * Scrollbar stuff.
 */
    فراغ
gui_mch_سېټ_scrollbar_thumb(scrollbar_T *sb, اوږد val, اوږد اندازه, اوږد max)
{
    إذا (sb->id == عدم)
	أرجع;

    GtkAdjustment *adjustment;

    // ignore events triggered by moving the thumb (happens in GTK 3)
    ++hold_gui_events;

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    gtk_adjustment_سېټ_ښکتنی(adjustment, 0.0);
    gtk_adjustment_سېټ_ارزښت(adjustment, val);
    gtk_adjustment_سېټ_پاسنی(adjustment, max + 1);
    gtk_adjustment_سېټ_پاڼه_اندازه(adjustment, اندازه);
    gtk_adjustment_سېټ_پاڼه_increment(adjustment,
	    اندازه < 3L ? 1L : اندازه - 2L);
    gtk_adjustment_سېټ_ګام_increment(adjustment, 1.0);

    g_signal_handler_بلاک(G_OBJECT(adjustment), (gulong)sb->handler_id);

    --hold_gui_events;

#إذا !GTK_CHECK_نسخه(3,18,0)
    gtk_adjustment_بدل_شوی(adjustment);
#د_که_پای

    g_signal_handler_unblock(G_OBJECT(adjustment),
	    (gulong)sb->handler_id);
}

    فراغ
gui_mch_سېټ_scrollbar_pos(scrollbar_T *sb, صحيح x, صحيح y, صحيح w, صحيح h)
{
    إذا (sb->id != عدم)
	gui_gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح xpad;
#إذا GTK_CHECK_نسخه(3,0,0)
    xpad = gtk_widget_get_allocated_پلنوالی(gui.formwin)
	  - gtk_widget_get_allocated_پلنوالی(gui.drawarea) - gui.scrollbar_پلنوالی;
#وإلا
    xpad = gui.formwin->ویش.پلنوالی - gui.drawarea->ویش.پلنوالی
							 - gui.scrollbar_پلنوالی;
#د_که_پای
    إذا (gui.which_scrollbars[SBAR_کیڼ] && gui.which_scrollbars[SBAR_ښی])
	xpad -= gui.scrollbar_پلنوالی;

    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح ypad;
#إذا GTK_CHECK_نسخه(3,0,0)
    ypad = gtk_widget_get_allocated_لوړوالی(gui.formwin)
	- gtk_widget_get_allocated_لوړوالی(gui.drawarea) - gui.scrollbar_لوړوالی;
#وإلا
    ypad = gui.formwin->ویش.لوړوالی - gui.drawarea->ویش.لوړوالی
							- gui.scrollbar_لوړوالی;
#د_که_پای
    أرجع (ypad < 0) ? 0 : ypad;
}

/*
 * Take action upon scrollbar dragging.
 */
    ساكن فراغ
adjustment_ارزښت_بدل_شوی(GtkAdjustment *adjustment, gpointer معلومات)
{
    scrollbar_T	*sb;
    اوږد	ارزښت;
    صحيح		dragging = FALSE;

#که_تعریف_شوی FEAT_XIM
    // cancel any preediting
    إذا (im_is_preediting())
	xim_reset();
#د_که_پای

    sb = gui_پیدا_کړه_scrollbar((اوږد)معلومات);
    ارزښت = gtk_adjustment_get_ارزښت(adjustment);
#إذا !GTK_CHECK_نسخه(3,0,0)
    /*
     * The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions.
     *
     * FIXME: Well, it doesn't work in GTK2. :)
     * HACK: Get the mouse pointer position, if it appears to be on an arrow
     * button set "dragging" to FALSE.  This assumes square buttons!
     */
    إذا (sb != عدم)
    {
	dragging = TRUE;

	إذا (sb->wp != عدم && GDK_IS_DRAWABLE(sb->id->کړکۍ_2))
	{
	    صحيح			x;
	    صحيح			y;
	    GdkModifierType	state;
	    صحيح			پلنوالی;
	    صحيح			لوړوالی;

	    // vertical scrollbar: need to set "dragging" properly in case
	    // there are closed folds.
	    gdk_کړکۍ_get_اشاره_ګر(sb->id->کړکۍ_2, &x, &y, &state);
	    gdk_کړکۍ_get_اندازه(sb->id->کړکۍ_2, &پلنوالی, &لوړوالی);
	    إذا (x >= 0 && x < پلنوالی && y >= 0 && y < لوړوالی)
	    {
		إذا (y < پلنوالی)
		{
		    // up arrow: move one (closed fold) line up
		    dragging = FALSE;
		    ارزښت = sb->wp->w_topline - 2;
		}
		وإلا إذا (y > لوړوالی - پلنوالی)
		{
		    // down arrow: move one (closed fold) line down
		    dragging = FALSE;
		    ارزښت = sb->wp->w_topline;
		}
	    }
	}
    }
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)
    gui_drag_scrollbar(sb, ارزښت, dragging);
}

// SBAR_VERT or SBAR_HORIZ
    فراغ
gui_mch_create_scrollbar(scrollbar_T *sb, صحيح orient)
{
    إذا (orient == SBAR_HORIZ)
#إذا GTK_CHECK_نسخه(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, عدم);
#وإلا
	sb->id = gtk_hscrollbar_new(عدم);
#د_که_پای
    وإلا إذا (orient == SBAR_VERT)
#إذا GTK_CHECK_نسخه(3,2,0)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, عدم);
#وإلا
	sb->id = gtk_vscrollbar_new(عدم);
#د_که_پای

    إذا (sb->id == عدم)
	أرجع;

    GtkAdjustment *adjustment;

    gtk_widget_سېټ_can_focus(sb->id, FALSE);
    gui_gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);

    adjustment = gtk_range_get_adjustment(GTK_RANGE(sb->id));

    sb->handler_id = g_signal_connect(
	    G_OBJECT(adjustment), "value-changed",
	    G_CALLBACK(adjustment_ارزښت_بدل_شوی),
	    GINT_منزل_اشاره_ګر(sb->ident));
    gui_mch_update();
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    إذا (sb->id != عدم)
    {
	gtk_widget_destroy(sb->id);
	sb->id = عدم;
    }
    gui_mch_update();
}

#إذا defined(FEAT_BROWSE)
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
gui_mch_browse(صحيح saving,
	       char_u *title,
	       char_u *dflt,
	       char_u *ext ناکارول_شوی,
	       char_u *initdir,
	       char_u *filter)
{
# إذا GTK_CHECK_نسخه(3,20,0)
    GtkFileChooserNative	*fc;
# وإلا
    GtkWidget			*fc;
# endif
    char_u		dirbuf[MAXPATHL];
    guint		log_handler;
    ثابت gchar		*domain = "Gtk";

    title = CONVERT_منزل_UTF8(title);

    // GTK has a bug, it only works with an absolute path.
    إذا (initdir == عدم || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    وإلا إذا (vim_ډک_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    // Always need a trailing slash for a directory.
    جمع_کړه_pathsep(dirbuf);

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Hack: The GTK file dialog warns when it can't access a new file, this
    // makes it shut up. http://bugzilla.gnome.org/show_bug.cgi?id=664587
    log_handler = g_log_سېټ_handler(domain, G_LOG_LEVEL_WARNING,
						  recent_func_log_func, عدم);

    // We create the dialog each time, so that the button text can be "Open"
    // or "Save" according to the action.
# إذا GTK_CHECK_نسخه(3,20,0)
    fc = gtk_file_chooser_native_new(
# وإلا
    fc = gtk_file_chooser_dialog_new(
# endif
	    (ثابت gchar *)title,
	    GTK_کړکۍ(gui.mainwin),
	    saving ? GTK_FILE_CHOOSER_ACTION_SAVE
					   : GTK_FILE_CHOOSER_ACTION_OPEN,
# إذا GTK_CHECK_نسخه(3,20,0)
	    saving ? _("_Save") : _("_Open"), _("_Cancel"));
# وإلا
#  إذا GTK_CHECK_نسخه(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    saving ? _("_Save") : _("_Open"), GTK_RESPONSE_ACCEPT,
#  وإلا
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    saving ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
#  endif
	    عدم);
# endif
    gtk_file_chooser_سېټ_current_folder(GTK_FILE_CHOOSER(fc),
						       (ثابت gchar *)dirbuf);

    إذا (filter != عدم && *filter != NUL)
    {
	صحيح     i = 0;
	char_u  *patt;
	char_u  *p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	إذا (patt != عدم)
	{
	    طالما (p != عدم && *p != NUL)
	    {
		إذا (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    إذا (*p == '\t')
			gtk_file_filter_سېټ_name(gfilter, (gchar *)patt);
		    وإلا
		    {
			gtk_file_filter_جمع_کړه_نمونه(gfilter, (gchar *)patt);
			إذا (*p == '\n')
			{
			    gtk_file_chooser_جمع_کړه_filter(GTK_FILE_CHOOSER(fc),
				    gfilter);
			    إذا (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		وإلا
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    إذا (saving && dflt != عدم && *dflt != NUL)
	gtk_file_chooser_سېټ_current_name(GTK_FILE_CHOOSER(fc), (حرف *)dflt);

    gui.browse_fname = عدم;
# إذا GTK_CHECK_نسخه(3,20,0)
    إذا (gtk_native_dialog_پرلپسې(GTK_NATIVE_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# وإلا
    إذا (gtk_dialog_پرلپسې(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT)
# endif
    {
	حرف *filename;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
	gui.browse_fname = (char_u *)g_strdup(filename);
	g_free(filename);
    }
# إذا GTK_CHECK_نسخه(3,20,0)
    g_object_unref(fc);
# وإلا
    gtk_widget_destroy(GTK_WIDGET(fc));
# endif

    g_log_remove_handler(domain, log_handler);

    CONVERT_منزل_UTF8_FREE(title);
    إذا (gui.browse_fname == عدم)
	أرجع عدم;

    // shorten the file name if possible
    أرجع vim_strsave(shorten_fname1(gui.browse_fname));
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
# إذا defined(GTK_FILE_CHOOSER)	    // Only in GTK 2.4 and later.
    char_u		dirbuf[MAXPATHL];
    char_u		*p;
    GtkWidget		*dirdlg;	    // file selection dialog
    char_u		*dirname = عدم;

    title = CONVERT_منزل_UTF8(title);

    dirdlg = gtk_file_chooser_dialog_new(
	    (ثابت gchar *)title,
	    GTK_کړکۍ(gui.mainwin),
	    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
#  إذا GTK_CHECK_نسخه(3,10,0)
	    _("_Cancel"), GTK_RESPONSE_CANCEL,
	    _("_OK"), GTK_RESPONSE_ACCEPT,
#  وإلا
	    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
#  endif
	    عدم);

    CONVERT_منزل_UTF8_FREE(title);

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // GTK appears to insist on an absolute path.
    إذا (initdir == عدم || *initdir == NUL
	       || vim_ډک_Name(initdir, dirbuf, MAXPATHL - 10, FALSE) == FAIL)
	mch_dirname(dirbuf, MAXPATHL - 10);

    // Always need a trailing slash for a directory.
    // Also add a dummy file name, so that we get to the directory.
    جمع_کړه_pathsep(dirbuf);
    STRCAT(dirbuf, "@zd(*&1|");
    gtk_file_chooser_سېټ_filename(GTK_FILE_CHOOSER(dirdlg),
						      (ثابت gchar *)dirbuf);

    // Run the dialog.
    إذا (gtk_dialog_پرلپسې(GTK_DIALOG(dirdlg)) == GTK_RESPONSE_ACCEPT)
	dirname = (char_u *)gtk_file_chooser_get_filename(
						    GTK_FILE_CHOOSER(dirdlg));
    gtk_widget_destroy(dirdlg);
    إذا (dirname == عدم)
	أرجع عدم;

    // shorten the file name if possible
    p = vim_strsave(shorten_fname1(dirname));
    g_free(dirname);
    أرجع p;

# وإلا // !defined(GTK_FILE_CHOOSER)
    // For GTK 2.2 and earlier: fall back to ordinary file selector.
    أرجع gui_mch_browse(0, title, عدم, عدم, initdir, عدم);
# endif // !defined(GTK_FILE_CHOOSER)
}


#د_که_پای	// FEAT_BROWSE

#إذا defined(FEAT_GUI_DIALOG)

    ساكن GtkWidget *
create_پیغام_dialog(صحيح type, char_u *title, char_u *پیغام_2)
{
    GtkWidget	    *dialog;
    Gtk_پیغام_Type  پیغام_type;

    بدل (type)
    {
	حالة VIM_ERROR:	    پیغام_type = GTK_پیغام_ERROR;	 اكسر;
	حالة VIM_WARNING:   پیغام_type = GTK_پیغام_WARNING;	 اكسر;
	حالة VIM_QUESTION:  پیغام_type = GTK_پیغام_QUESTION; اكسر;
	افتراضي:	    پیغام_type = GTK_پیغام_INFO;	 اكسر;
    }

    پیغام_2 = CONVERT_منزل_UTF8(پیغام_2);
    dialog  = gtk_پیغام_dialog_new(GTK_کړکۍ(gui.mainwin),
				     GTK_DIALOG_DESTROY_WITH_پلار,
				     پیغام_type,
				     GTK_BUTTONS_NONE,
				     "%s", (ثابت حرف *)پیغام_2);
    CONVERT_منزل_UTF8_FREE(پیغام_2);

    إذا (title != عدم)
    {
	title = CONVERT_منزل_UTF8(title);
	gtk_کړکۍ_سېټ_title(GTK_کړکۍ(dialog), (ثابت حرف *)title);
	CONVERT_منزل_UTF8_FREE(title);
    }
    وإلا إذا (type == VIM_GENERIC)
    {
	gtk_کړکۍ_سېټ_title(GTK_کړکۍ(dialog), "VIM");
    }

    أرجع dialog;
}

/*
 * Split up button_string into individual button labels by inserting
 * NUL bytes.  Also replace the Vim-style mnemonic accelerator prefix
 * '&' with '_'.  button_string must point to allocated memory!
 * Return an allocated array of pointers into button_string.
 */
    ساكن حرف **
وېشل_button_string(char_u *button_string, صحيح *n_buttons)
{
    حرف	    **array;
    char_u	    *p;
    بینښه صحيح    شمېر = 1;

    لكل (p = button_string; *p != NUL; ++p)
	إذا (*p == DLG_BUTTON_SEP)
	    ++شمېر;

    array = ALLOC_MULT(حرف *, شمېر + 1);
    شمېر = 0;

    إذا (array != عدم)
    {
	array[شمېر++] = (حرف *)button_string;
	لكل (p = button_string; *p != NUL; )
	{
	    إذا (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[شمېر++] = (حرف *)p;
	    }
	    وإلا إذا (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    وإلا
		MB_PTR_ADV(p);
	}
	array[شمېر] = عدم; // currently not relied upon, but doesn't hurt
    }

    *n_buttons = شمېر;
    أرجع array;
}

# إذا !GTK_CHECK_نسخه(3,10,0)
    ساكن حرف **
وېشل_button_translation(ثابت حرف *پیغام_2)
{
    حرف    **buttons = عدم;
    char_u  *str;
    صحيح	    n_buttons = 0;
    صحيح	    n_expected = 1;

    لكل (str = (char_u *)پیغام_2; *str != NUL; ++str)
	إذا (*str == DLG_BUTTON_SEP)
	    ++n_expected;

    str = (char_u *)_(پیغام_2);
    إذا (str != عدم)
    {
	إذا (output_conv.vc_type != CONV_NONE)
	    str = string_convert(&output_conv, str, عدم);
	وإلا
	    str = vim_strsave(str);

	إذا (str != عدم)
	    buttons = وېشل_button_string(str, &n_buttons);
    }
    /*
     * Uh-oh... this should never ever happen.	But we don't wanna crash
     * if the translation is broken, thus fall back to the untranslated
     * buttons string in case of emergency.
     */
    إذا (buttons == عدم || n_buttons != n_expected)
    {
	vim_free(buttons);
	vim_free(str);
	buttons = عدم;
	str = vim_strsave((char_u *)پیغام_2);

	إذا (str != عدم)
	    buttons = وېشل_button_string(str, &n_buttons);
	إذا (buttons == عدم)
	    vim_free(str);
    }

    أرجع buttons;
}

    ساكن صحيح
button_equal(ثابت حرف *a, ثابت حرف *b)
{
    طالما (*a != '\0' && *b != '\0')
    {
	إذا (*a == '_' && *++a == '\0')
	    اكسر;
	إذا (*b == '_' && *++b == '\0')
	    اكسر;

	إذا (g_unichar_tolower(g_utf8_get_char(a))
		!= g_unichar_tolower(g_utf8_get_char(b)))
	    أرجع FALSE;

	a = g_utf8_بل_char(a);
	b = g_utf8_بل_char(b);
    }

    أرجع (*a == '\0' && *b == '\0');
}
# endif

    ساكن فراغ
dialog_جمع_کړه_buttons(GtkDialog *dialog, char_u *button_string)
{
# إذا !GTK_CHECK_نسخه(3,10,0)
    حرف    **ok;
    حرف    **ync;  // "yes no cancel"
# endif
    حرف    **buttons;
    صحيح	    n_buttons = 0;
    صحيح	    idx;

    button_string = vim_strsave(button_string); // must be writable
    إذا (button_string == عدم)
	أرجع;

    // Check 'v' flag in 'guioptions': vertical button placement.
    إذا (vim_strchr(p_go, GO_VERTICAL) != عدم)
    {
# إذا !GTK_CHECK_نسخه(3,0,0)
	GtkWidget	*vbutton_box;

	vbutton_box = gtk_vbutton_box_new();
	gtk_widget_show(vbutton_box);
	gtk_box_pack_پای_ټکی(GTK_BOX(GTK_DIALOG(dialog)->vbox),
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
# إذا !GTK_CHECK_نسخه(3,10,0)
    ok	    = وېشل_button_translation(N_("&Ok"));
    ync     = وېشل_button_translation(N_("&Yes\n&No\n&Cancel"));
# endif
    buttons = وېشل_button_string(button_string, &n_buttons);

    /*
     * Yes, the buttons are in reversed order to match the GNOME 2 desktop
     * environment.  Don't hit me -- it's all about consistency.
     * Well, apparently somebody changed his mind: with GTK 2.2.4 it works the
     * other way around...
     */
    لكل (idx = 1; idx <= n_buttons; ++idx)
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
# إذا !GTK_CHECK_نسخه(3,10,0)
	إذا (ok != عدم && ync != عدم) // almost impossible to fail
	{
	    إذا	    (button_equal(label, ok[0]))    label = GTK_STOCK_OK;
	    وإلا إذا (button_equal(label, ync[0]))   label = GTK_STOCK_YES;
	    وإلا إذا (button_equal(label, ync[1]))   label = GTK_STOCK_NO;
	    وإلا إذا (button_equal(label, ync[2]))   label = GTK_STOCK_CANCEL;
	    وإلا إذا (button_equal(label, "Ok"))     label = GTK_STOCK_OK;
	    وإلا إذا (button_equal(label, "Yes"))    label = GTK_STOCK_YES;
	    وإلا إذا (button_equal(label, "No"))     label = GTK_STOCK_NO;
	    وإلا إذا (button_equal(label, "Cancel")) label = GTK_STOCK_CANCEL;
	}
# endif
	label8 = CONVERT_منزل_UTF8((char_u *)label);
	gtk_dialog_جمع_کړه_button(dialog, (ثابت gchar *)label8, idx);
	CONVERT_منزل_UTF8_FREE(label8);
    }

# إذا !GTK_CHECK_نسخه(3,10,0)
    إذا (ok != عدم)
	vim_free(*ok);
    إذا (ync != عدم)
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
تعريفنوع بنية _DialogInfo
{
    صحيح		ignore_enter;	    // no default button, ignore "Enter"
    صحيح		noalt;		    // accept accelerators without Alt
    GtkDialog	*dialog;	    // Widget of the dialog
} DialogInfo;

    ساكن gboolean
dialog_کلی_press_event_cb(GtkWidget *widget, Gdk_Event_کلی *event, gpointer معلومات)
{
    DialogInfo *di = (DialogInfo *)معلومات;

    // Ignore hitting Enter (or Space) when there is no default button.
    إذا (di->ignore_enter && (event->keyval == GDK_Return
						     || event->keyval == ' '))
	أرجع TRUE;
    وإلا    // A different key was pressed, return to normal behavior
	di->ignore_enter = FALSE;

    // Close the dialog when hitting "Esc".
    إذا (event->keyval == GDK_Escape)
    {
	gtk_dialog_response(di->dialog, GTK_RESPONSE_REJECT);
	أرجع TRUE;
    }

    إذا (di->noalt
	      && (event->state & gtk_accelerator_get_default_mod_ماسک()) == 0)
    {
	أرجع gtk_کړکۍ_mnemonic_activate(
		   GTK_کړکۍ(widget), event->keyval,
		   gtk_کړکۍ_get_mnemonic_modifier(GTK_کړکۍ(widget)));
    }

    أرجع FALSE; // continue emission
}

    صحيح
gui_mch_dialog(صحيح	type,	    // type of dialog
	       char_u	*title,	    // title of dialog
	       char_u	*پیغام_2,   // message text
	       char_u	*buttons,   // names of buttons
	       صحيح	def_but,    // default button
	       char_u	*textfield, // text for textfield or NULL
	       صحيح	ex_cmd ناکارول_شوی)
{
    GtkWidget	*dialog;
    GtkWidget	*ثبت = عدم;
    char_u	*متن;
    صحيح		response;
    DialogInfo  dialoginfo;

    ++gui.dialogs_فعال;

    dialog = create_پیغام_dialog(type, title, پیغام_2);
    dialoginfo.dialog = GTK_DIALOG(dialog);
    dialog_جمع_کړه_buttons(GTK_DIALOG(dialog), buttons);

    إذا (textfield != عدم)
    {
	GtkWidget *alignment;

	ثبت = gtk_ثبت_new();
	gtk_widget_show(ثبت);

	// Make Enter work like pressing OK.
	gtk_ثبت_سېټ_activates_default(GTK_ثبت(ثبت), TRUE);

	متن = CONVERT_منزل_UTF8(textfield);
	gtk_ثبت_سېټ_متن(GTK_ثبت(ثبت), (ثابت حرف *)متن);
	CONVERT_منزل_UTF8_FREE(متن);

# إذا GTK_CHECK_نسخه(3,14,0)
	gtk_widget_سېټ_halign(GTK_WIDGET(ثبت), GTK_ALIGN_CENTER);
	gtk_widget_سېټ_valign(GTK_WIDGET(ثبت), GTK_ALIGN_CENTER);
	gtk_widget_سېټ_hexpand(GTK_WIDGET(ثبت), TRUE);
	gtk_widget_سېټ_vexpand(GTK_WIDGET(ثبت), TRUE);

	alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
# وإلا
	alignment = gtk_alignment_new((عائم)0.5, (عائم)0.5,
						      (عائم)1.0, (عائم)1.0);
# endif
	gtk_container_جمع_کړه(GTK_CONTAINER(alignment), ثبت);
	gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(alignment), 5);
	gtk_widget_show(alignment);

# إذا GTK_CHECK_نسخه(3,0,0)
	{
	    GtkWidget * ثابت vbox
		= gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	    gtk_box_pack_پیل(GTK_BOX(vbox),
		    alignment, TRUE, FALSE, 0);
	}
# وإلا
	gtk_box_pack_پیل(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   alignment, TRUE, FALSE, 0);
# endif
	dialoginfo.noalt = FALSE;
    }
    وإلا
	dialoginfo.noalt = TRUE;

    // Allow activation of mnemonic accelerators without pressing <Alt> when
    // there is no textfield.  Handle pressing Esc.
    g_signal_connect(G_OBJECT(dialog), "key-press-event",
			 G_CALLBACK(&dialog_کلی_press_event_cb), &dialoginfo);

    إذا (def_but > 0)
    {
	gtk_dialog_سېټ_default_response(GTK_DIALOG(dialog), def_but);
	dialoginfo.ignore_enter = FALSE;
    }
    وإلا
	// No default button, ignore pressing Enter.
	dialoginfo.ignore_enter = TRUE;

    // Show the mouse pointer if it's currently hidden.
    gui_mch_mousehide(FALSE);

    response = gtk_dialog_پرلپسې(GTK_DIALOG(dialog));

    // GTK_RESPONSE_NONE means the dialog was programmatically destroyed.
    إذا (response != GTK_RESPONSE_NONE)
    {
	إذا (response == GTK_RESPONSE_ACCEPT)	    // Enter pressed
	    response = def_but;
	إذا (textfield != عدم)
	{
	    متن = (char_u *)gtk_ثبت_get_متن(GTK_ثبت(ثبت));
	    متن = CONVERT_FROM_UTF8(متن);

	    vim_strncpy(textfield, متن, IOSIZE - 1);

	    CONVERT_FROM_UTF8_FREE(متن);
	}
	gtk_widget_destroy(dialog);
    }

    إذا (gui.dialogs_فعال > 0)
	--gui.dialogs_فعال;
    أرجع response > 0 ? response : 0;
}

#د_که_پای // FEAT_GUI_DIALOG


#إذا defined(FEAT_MENU)

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
# إذا defined(FEAT_XIM)
    /*
     * Append a submenu for selecting an input method.	This is
     * currently the only way to switch input methods at runtime.
     */
#  إذا !GTK_CHECK_نسخه(3,10,0)
    إذا (xic != عدم && g_object_get_معلومات(G_OBJECT(menu->submenu_id),
					 "vim-has-im-menu") == عدم)
    {
	GtkWidget   *menuitem;
	GtkWidget   *submenu;
	char_u	    *name;

	menuitem = gtk_separator_menu_توکی_new();
	gtk_widget_show(menuitem);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	name = (char_u *)_("Input _Methods");
	name = CONVERT_منزل_UTF8(name);
	menuitem = gtk_menu_توکی_new_with_mnemonic((ثابت حرف *)name);
	CONVERT_منزل_UTF8_FREE(name);
	gtk_widget_show(menuitem);

	submenu = gtk_menu_new();
	gtk_menu_توکی_سېټ_submenu(GTK_MENU_توکی(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu->submenu_id), menuitem);

	gtk_im_multicontext_append_menuitems(GTK_IM_MULTICONTEXT(xic),
					     GTK_MENU_SHELL(submenu));
	g_object_سېټ_معلومات(G_OBJECT(menu->submenu_id),
			  "vim-has-im-menu", GINT_منزل_اشاره_ګر(TRUE));
    }
#  endif
# endif // FEAT_XIM

# إذا GTK_CHECK_نسخه(3,22,2)
    {
	GdkEventButton trigger;

	// A pseudo event to have gtk_menu_popup_at_pointer() work. Since the
	// function calculates the popup menu position on the basis of the
	// actual pointer position when it is invoked, the fields x, y, x_root
	// and y_root are set to zero for convenience.
	trigger.type       = GDK_BUTTON_PRESS;
	trigger.کړکۍ_2     = gtk_widget_get_کړکۍ(gui.drawarea);
	trigger.send_event = FALSE;
	trigger.وخت       = gui.event_وخت;
	trigger.x	   = 0.0;
	trigger.y	   = 0.0;
	trigger.axes       = عدم;
	trigger.state      = 0;
	trigger.button     = 3;
	trigger.device     = عدم;
	trigger.x_ریښه     = 0.0;
	trigger.y_ریښه     = 0.0;

	gtk_menu_popup_at_اشاره_ګر(GTK_MENU(menu->submenu_id),
				  (GdkEvent *)&trigger);
    }
# وإلا
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
		   عدم, عدم,
		   (GtkMenuPositionFunc)عدم, عدم,
		   3U, gui.event_وخت);
# endif
}

// Ugly global variable to pass "mouse_pos" flag from gui_make_popup() to
// popup_menu_position_func().
ساكن صحيح popup_mouse_pos;

/*
 * Menu position callback; used by gui_make_popup() to place the menu
 * at the current text cursor position.
 *
 * Note: The push_in output argument seems to affect scrolling of huge
 * menus that don't fit on the screen.	Leave it at the default for now.
 */
    ساكن فراغ
popup_menu_position_func(GtkMenu *menu ناکارول_شوی,
			 gint *x, gint *y,
			 gboolean *ټېل_دننه ناکارول_شوی,
			 gpointer user_معلومات ناکارول_شوی)
{
    gdk_کړکۍ_get_origin(gtk_widget_get_کړکۍ(gui.drawarea), x, y);

    إذا (popup_mouse_pos)
    {
	صحيح	mx, my;

	gui_mch_getmouse(&mx, &my);
	*x += mx;
	*y += my;
    }
    وإلا إذا (curwin != عدم && gui.drawarea != عدم &&
	     gtk_widget_get_کړکۍ(gui.drawarea) != عدم)
    {
	// Find the cursor position in the current window
	*x += FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	*y += FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }
}

    فراغ
gui_make_popup(char_u *لار_name, صحيح mouse_pos)
{
    vimmenu_T *menu;

    popup_mouse_pos = mouse_pos;

    menu = gui_پیدا_کړه_menu(لار_name);
    إذا (menu == عدم || menu->submenu_id == عدم)
	أرجع;

# إذا GTK_CHECK_نسخه(3,22,2)
    Gdk_کړکۍ * ثابت win = gtk_widget_get_کړکۍ(gui.drawarea);
    GdkEventButton trigger;

    // A pseudo event to have gtk_menu_popup_at_*() functions work. Since
    // the position where the menu pops up is automatically adjusted by
    // the functions, none of the fields x, y, x_root and y_root has to be
    // set to a specific value here; therefore, they are set to zero for
    // convenience.
    trigger.type       = GDK_BUTTON_PRESS;
    trigger.کړکۍ_2     = win;
    trigger.send_event = FALSE;
    trigger.وخت       = GDK_CURRENT_وخت;
    trigger.x	   = 0.0;
    trigger.y	   = 0.0;
    trigger.axes       = عدم;
    trigger.state      = 0;
    trigger.button     = 0;
    trigger.device     = عدم;
    trigger.x_ریښه     = 0.0;
    trigger.y_ریښه     = 0.0;

    إذا (mouse_pos)
	gtk_menu_popup_at_اشاره_ګر(GTK_MENU(menu->submenu_id),
		(GdkEvent *)&trigger);
    وإلا
    {
	gint origin_x, origin_y;
	GdkRectangle rect = { 0, 0, 0, 0 };

	gdk_کړکۍ_get_origin(win, &origin_x, &origin_y);
	popup_menu_position_func(عدم, &rect.x, &rect.y, عدم, عدم);

	rect.x -= origin_x;
	rect.y -= origin_y;

	gtk_menu_popup_at_rect(GTK_MENU(menu->submenu_id),
		win,
		&rect,
		GDK_GRAVITY_SOUTH_EAST,
		GDK_GRAVITY_NORTH_WEST,
		(GdkEvent *)&trigger);
    }
# وإلا
    gtk_menu_popup(GTK_MENU(menu->submenu_id),
	    عدم, عدم,
	    &popup_menu_position_func, عدم,
	    0U, (guint32)GDK_CURRENT_وخت);
# endif
}

#د_که_پای // FEAT_MENU


/*
 * We don't create it twice.
 */

تعريفنوع بنية Shared_پیدا_کړه_Replace
{
    GtkWidget *dialog;	// the main dialog widget
    GtkWidget *wword;	// 'Whole word only' check button
    GtkWidget *mcase;	// 'Match case' check button
    GtkWidget *پورته;	// search direction 'Up' radio button
    GtkWidget *ښکته;	// search direction 'Down' radio button
    GtkWidget *what;	// 'Find what' entry text widget
    GtkWidget *with;	// 'Replace with' entry text widget
    GtkWidget *پیدا_کړه;	// 'Find Next' action button
    GtkWidget *replace;	// 'Replace With' action button
    GtkWidget *all;	// 'Replace All' action button
} Shared_پیدا_کړه_Replace_2;

ساكن Shared_پیدا_کړه_Replace_2 پیدا_کړه_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};
ساكن Shared_پیدا_کړه_Replace_2 repl_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};

    ساكن صحيح
پیدا_کړه_کلی_press_event(
		GtkWidget	*widget ناکارول_شوی,
		Gdk_Event_کلی	*event,
		Shared_پیدا_کړه_Replace_2 *frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    إذا (event->state & (GDK_CONTROL_ماسک | GDK_ښويدنه_ماسک))
	أرجع FALSE;

    // the Escape key synthesizes a cancellation action
    إذا (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	أرجع TRUE;
    }

    // It would be delightful if it where possible to do search history
    // operations on the K_UP and K_DOWN keys here.

    أرجع FALSE;
}

    ساكن GtkWidget *
#إذا GTK_CHECK_نسخه(3,10,0)
create_image_button(ثابت حرف *stock_id ناکارول_شوی,
		    ثابت حرف *label)
#وإلا
create_image_button(ثابت حرف *stock_id,
		    ثابت حرف *label)
#د_که_پای
{
    char_u	*متن;
    GtkWidget	*box;
    GtkWidget	*alignment;
    GtkWidget	*button;

    متن = CONVERT_منزل_UTF8((char_u *)label);

#إذا GTK_CHECK_نسخه(3,2,0)
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_سېټ_homogeneous(GTK_BOX(box), FALSE);
#وإلا
    box = gtk_hbox_new(FALSE, 3);
#د_که_پای
#إذا !GTK_CHECK_نسخه(3,10,0)
    إذا (stock_id != عدم)
	gtk_box_pack_پیل(GTK_BOX(box),
			   gtk_image_new_from_stock(stock_id, GTK_ICON_اندازه_BUTTON),
			   FALSE, FALSE, 0);
#د_که_پای
    gtk_box_pack_پیل(GTK_BOX(box),
		       gtk_label_new((ثابت حرف *)متن),
		       FALSE, FALSE, 0);

    CONVERT_منزل_UTF8_FREE(متن);

#إذا GTK_CHECK_نسخه(3,14,0)
    gtk_widget_سېټ_halign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_سېټ_valign(GTK_WIDGET(box), GTK_ALIGN_CENTER);
    gtk_widget_سېټ_hexpand(GTK_WIDGET(box), TRUE);
    gtk_widget_سېټ_vexpand(GTK_WIDGET(box), TRUE);

    alignment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#وإلا
    alignment = gtk_alignment_new((عائم)0.5, (عائم)0.5,
						      (عائم)0.0, (عائم)0.0);
#د_که_پای
    gtk_container_جمع_کړه(GTK_CONTAINER(alignment), box);
    gtk_widget_show_all(alignment);

    button = gtk_button_new();
    gtk_container_جمع_کړه(GTK_CONTAINER(button), alignment);

    أرجع button;
}

/*
 * This is currently only used by find_replace_dialog_create(), and
 * I'd really like to keep it at that.	In other words: don't spread
 * this nasty hack all over the code.  Think twice.
 */
    ساكن ثابت حرف *
convert_localized_پیغام(char_u **بفر, ثابت حرف *پیغام_2)
{
    إذا (output_conv.vc_type == CONV_NONE)
	أرجع پیغام_2;

    vim_free(*بفر);
    *بفر = string_convert(&output_conv, (char_u *)پیغام_2, عدم);

    أرجع (ثابت حرف *)*بفر;
}

/*
 * Returns the number of characters in GtkEntry.
 */
    ساكن بینښه اوږد
ثبت_get_متن_اوږدتیا(Gtk_ثبت *ثبت)
{
    g_return_val_if_fail(ثبت != عدم, 0);
    g_return_val_if_fail(GTK_IS_ثبت(ثبت) == TRUE, 0);

#إذا GTK_CHECK_نسخه(2,18,0)
    // 2.18 introduced a new object GtkEntryBuffer to handle text data for
    // GtkEntry instead of letting each instance of the latter have its own
    // storage for that.  The code below is almost identical to the
    // implementation of gtk_entry_get_text_length() for the versions >= 2.18.
    أرجع gtk_ثبت_بفر_get_اوږدتیا(gtk_ثبت_get_بفر(ثبت));
#وإلاإذا GTK_CHECK_نسخه(2,14,0)
    // 2.14 introduced a new function to avoid memory management bugs which can
    // happen when gtk_entry_get_text() is used without due care and attention.
    أرجع gtk_ثبت_get_متن_اوږدتیا(ثبت);
#وإلا
    // gtk_entry_get_text() returns the pointer to the storage allocated
    // internally by the widget.  Accordingly, use the one with great care:
    // Don't free it nor modify the contents it points to; call the function
    // every time you need the pointer since its value may have been changed
    // by the widget.
    أرجع g_utf8_strlen(gtk_ثبت_get_متن(ثبت), -1);
#د_که_پای
}

    ساكن فراغ
پیدا_کړه_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    GtkWidget	*hbox;		// main top down box
    GtkWidget	*actionarea;
    GtkWidget	*جدول;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    Shared_پیدا_کړه_Replace_2 *frdp;
    char_u	*ثبت_متن;
    صحيح		wword = FALSE;
    صحيح		mcase = !p_ic;
    char_u	*conv_بفر = عدم;
#تعریف_کړه CONV(پیغام_2) convert_localized_پیغام(&conv_بفر, (پیغام_2))

    frdp = (do_replace) ? (&repl_widgets) : (&پیدا_کړه_widgets);

    // Get the search string to use.
    ثبت_متن = get_پیدا_کړه_dialog_متن(arg, &wword, &mcase);

    إذا (ثبت_متن != عدم && output_conv.vc_type != CONV_NONE)
    {
	char_u *old_متن = ثبت_متن;
	ثبت_متن = string_convert(&output_conv, ثبت_متن, عدم);
	vim_free(old_متن);
    }

    /*
     * If the dialog already exists, just raise it.
     */
    إذا (frdp->dialog)
    {
	إذا (ثبت_متن != عدم)
	{
	    gtk_ثبت_سېټ_متن(GTK_ثبت(frdp->what), (حرف *)ثبت_متن);
	    gtk_toggle_button_سېټ_فعال(GTK_TOGGLE_BUTTON(frdp->wword),
							     (gboolean)wword);
	    gtk_toggle_button_سېټ_فعال(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
	}
	gtk_کړکۍ_present(GTK_کړکۍ(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	إذا (do_replace && ثبت_get_متن_اوږدتیا(GTK_ثبت(frdp->what)) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(ثبت_متن);
	أرجع;
    }

    frdp->dialog = gtk_dialog_new();
#إذا GTK_CHECK_نسخه(3,0,0)
    // Nothing equivalent to gtk_dialog_set_has_separator() in GTK+ 3.
#وإلا
    gtk_dialog_سېټ_has_separator(GTK_DIALOG(frdp->dialog), FALSE);
#د_که_پای
    gtk_کړکۍ_سېټ_transient_for(GTK_کړکۍ(frdp->dialog), GTK_کړکۍ(gui.mainwin));
    gtk_کړکۍ_سېټ_destroy_with_پلار(GTK_کړکۍ(frdp->dialog), TRUE);

    إذا (do_replace)
    {
	gtk_کړکۍ_سېټ_title(GTK_کړکۍ(frdp->dialog),
			     CONV(_("VIM - Search and Replace...")));
    }
    وإلا
    {
	gtk_کړکۍ_سېټ_title(GTK_کړکۍ(frdp->dialog),
			     CONV(_("VIM - Search...")));
    }

#إذا GTK_CHECK_نسخه(3,2,0)
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_سېټ_homogeneous(GTK_BOX(hbox), FALSE);
#وإلا
    hbox = gtk_hbox_new(FALSE, 0);
#د_که_پای
    gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(hbox), 10);
#إذا GTK_CHECK_نسخه(3,0,0)
    {
	GtkWidget * ثابت dialog_vbox
	    = gtk_dialog_get_content_area(GTK_DIALOG(frdp->dialog));
	gtk_container_جمع_کړه(GTK_CONTAINER(dialog_vbox), hbox);
    }
#وإلا
    gtk_container_جمع_کړه(GTK_CONTAINER(GTK_DIALOG(frdp->dialog)->vbox), hbox);
#د_که_پای

    إذا (do_replace)
#إذا GTK_CHECK_نسخه(3,4,0)
	جدول = gtk_grid_new();
#وإلا
	جدول = gtk_جدول_new(1024, 4, FALSE);
#د_که_پای
    وإلا
#إذا GTK_CHECK_نسخه(3,4,0)
	جدول = gtk_grid_new();
#وإلا
	جدول = gtk_جدول_new(1024, 3, FALSE);
#د_که_پای
    gtk_box_pack_پیل(GTK_BOX(hbox), جدول, TRUE, TRUE, 0);
    gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(جدول), 4);

    tmp = gtk_label_new(CONV(_("Find what:")));
#إذا GTK_CHECK_نسخه(3,16,0)
    gtk_label_سېټ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_label_سېټ_yalign(GTK_LABEL(tmp), 0.5);
#وإلاإذا GTK_CHECK_نسخه(3,14,0)
    {
	GValue align_val = G_ارزښت_INIT;

	g_ارزښت_init(&align_val, G_TYPE_FLOAT);

	g_ارزښت_سېټ_float(&align_val, 0.0);
	g_object_سېټ_property(G_OBJECT(tmp), "xalign", &align_val);

	g_ارزښت_سېټ_float(&align_val, 0.5);
	g_object_سېټ_property(G_OBJECT(tmp), "yalign", &align_val);

	g_ارزښت_unset(&align_val);
    }
#وإلا
    gtk_misc_سېټ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#د_که_پای
#إذا GTK_CHECK_نسخه(3,4,0)
    gtk_grid_attach(GTK_GRID(جدول), tmp, 0, 0, 2, 1);
#وإلا
    gtk_جدول_attach(GTK_جدول(جدول), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای
    frdp->what = gtk_ثبت_new();
    sensitive = (ثبت_متن != عدم && ثبت_متن[0] != NUL);
    إذا (ثبت_متن != عدم)
	gtk_ثبت_سېټ_متن(GTK_ثبت(frdp->what), (حرف *)ثبت_متن);
    g_signal_connect(G_OBJECT(frdp->what), "changed",
		     G_CALLBACK(ثبت_بدل_شوی_cb), frdp->dialog);
    g_signal_connect_after(G_OBJECT(frdp->what), "key-press-event",
			   G_CALLBACK(پیدا_کړه_کلی_press_event),
			   (gpointer) frdp);
#إذا GTK_CHECK_نسخه(3,4,0)
    gtk_grid_attach(GTK_GRID(جدول), frdp->what, 2, 0, 5, 1);
#وإلا
    gtk_جدول_attach(GTK_جدول(جدول), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای

    إذا (do_replace)
    {
	tmp = gtk_label_new(CONV(_("Replace with:")));
#إذا GTK_CHECK_نسخه(3,16,0)
	gtk_label_سېټ_xalign(GTK_LABEL(tmp), 0.0);
	gtk_label_سېټ_yalign(GTK_LABEL(tmp), 0.5);
#وإلاإذا GTK_CHECK_نسخه(3,14,0)
	{
	    GValue align_val = G_ارزښت_INIT;

	    g_ارزښت_init(&align_val, G_TYPE_FLOAT);

	    g_ارزښت_سېټ_float(&align_val, 0.0);
	    g_object_سېټ_property(G_OBJECT(tmp), "xalign", &align_val);

	    g_ارزښت_سېټ_float(&align_val, 0.5);
	    g_object_سېټ_property(G_OBJECT(tmp), "yalign", &align_val);

	    g_ارزښت_unset(&align_val);
	}
#وإلا
	gtk_misc_سېټ_alignment(GTK_MISC(tmp), (gfloat)0.0, (gfloat)0.5);
#د_که_پای
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), tmp, 0, 1, 2, 1);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای
	frdp->with = gtk_ثبت_new();
	g_signal_connect(G_OBJECT(frdp->with), "activate",
			 G_CALLBACK(پیدا_کړه_replace_cb),
			 GINT_منزل_اشاره_ګر(FRD_R_FINDNEXT));
	g_signal_connect_after(G_OBJECT(frdp->with), "key-press-event",
			       G_CALLBACK(پیدا_کړه_کلی_press_event),
			       (gpointer) frdp);
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->with, 2, 1, 5, 1);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), frdp->with, 1, 1024, 1, 2,
			 GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای

	/*
	 * Make the entry activation only change the input focus onto the
	 * with item.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(ثبت_activate_cb), frdp->with);
    }
    وإلا
    {
	/*
	 * Make the entry activation do the search.
	 */
	g_signal_connect(G_OBJECT(frdp->what), "activate",
			 G_CALLBACK(پیدا_کړه_replace_cb),
			 GINT_منزل_اشاره_ګر(FRD_FINDNEXT));
    }

    // whole word only button
    frdp->wword = gtk_check_button_new_with_label(CONV(_("Match whole word only")));
    gtk_toggle_button_سېټ_فعال(GTK_TOGGLE_BUTTON(frdp->wword),
							(gboolean)wword);
    إذا (do_replace)
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->wword, 0, 2, 5, 1);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), frdp->wword, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای
    وإلا
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->wword, 0, 3, 5, 1);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), frdp->wword, 0, 1023, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای

    // match case button
    frdp->mcase = gtk_check_button_new_with_label(CONV(_("Match case")));
    gtk_toggle_button_سېټ_فعال(GTK_TOGGLE_BUTTON(frdp->mcase),
							     (gboolean)mcase);
    إذا (do_replace)
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->mcase, 0, 3, 5, 1);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), frdp->mcase, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای
    وإلا
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), frdp->mcase, 0, 4, 5, 1);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), frdp->mcase, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);
#د_که_پای

    tmp = gtk_چوکاټ_new(CONV(_("Direction")));
    إذا (do_replace)
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), tmp, 5, 2, 2, 4);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
#د_که_پای
    وإلا
#إذا GTK_CHECK_نسخه(3,4,0)
	gtk_grid_attach(GTK_GRID(جدول), tmp, 5, 2, 1, 3);
#وإلا
	gtk_جدول_attach(GTK_جدول(جدول), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
#د_که_پای
#إذا GTK_CHECK_نسخه(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سېټ_homogeneous(GTK_BOX(vbox), FALSE);
#وإلا
    vbox = gtk_vbox_new(FALSE, 0);
#د_که_پای
    gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(vbox), 0);
    gtk_container_جمع_کړه(GTK_CONTAINER(tmp), vbox);

    // 'Up' and 'Down' buttons
    frdp->پورته = gtk_radio_button_new_with_label(عدم, CONV(_("Up")));
    gtk_box_pack_پیل(GTK_BOX(vbox), frdp->پورته, TRUE, TRUE, 0);
    frdp->ښکته = gtk_radio_button_new_with_label(
			gtk_radio_button_get_ډله(GTK_RADIO_BUTTON(frdp->پورته)),
			CONV(_("Down")));
    gtk_toggle_button_سېټ_فعال(GTK_TOGGLE_BUTTON(frdp->ښکته), TRUE);
    gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(vbox), 2);
    gtk_box_pack_پیل(GTK_BOX(vbox), frdp->ښکته, TRUE, TRUE, 0);

    // vbox to hold the action buttons
#إذا GTK_CHECK_نسخه(3,2,0)
    actionarea = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#وإلا
    actionarea = gtk_vbutton_box_new();
#د_که_پای
    gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(actionarea), 2);
    gtk_box_pack_پای_ټکی(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    // 'Find Next' button
#إذا GTK_CHECK_نسخه(3,10,0)
    frdp->پیدا_کړه = create_image_button(عدم, _("Find Next"));
#وإلا
    frdp->پیدا_کړه = create_image_button(GTK_STOCK_پیدا_کړه, _("Find Next"));
#د_که_پای
    gtk_widget_سېټ_sensitive(frdp->پیدا_کړه, sensitive);

    g_signal_connect(G_OBJECT(frdp->پیدا_کړه), "clicked",
		     G_CALLBACK(پیدا_کړه_replace_cb),
		     (do_replace) ? GINT_منزل_اشاره_ګر(FRD_R_FINDNEXT)
				  : GINT_منزل_اشاره_ګر(FRD_FINDNEXT));

    gtk_widget_سېټ_can_default(frdp->پیدا_کړه, TRUE);
    gtk_box_pack_پیل(GTK_BOX(actionarea), frdp->پیدا_کړه, FALSE, FALSE, 0);
    gtk_widget_grab_default(frdp->پیدا_کړه);

    إذا (do_replace)
    {
	// 'Replace' button
#إذا GTK_CHECK_نسخه(3,10,0)
	frdp->replace = create_image_button(عدم, _("Replace"));
#وإلا
	frdp->replace = create_image_button(GTK_STOCK_CONVERT, _("Replace"));
#د_که_پای
	gtk_widget_سېټ_sensitive(frdp->replace, sensitive);
	gtk_widget_سېټ_can_default(frdp->پیدا_کړه, TRUE);
	gtk_box_pack_پیل(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->replace), "clicked",
			 G_CALLBACK(پیدا_کړه_replace_cb),
			 GINT_منزل_اشاره_ګر(FRD_REPLACE));

	// 'Replace All' button
#إذا GTK_CHECK_نسخه(3,10,0)
	frdp->all = create_image_button(عدم, _("Replace All"));
#وإلا
	frdp->all = create_image_button(GTK_STOCK_CONVERT, _("Replace All"));
#د_که_پای
	gtk_widget_سېټ_sensitive(frdp->all, sensitive);
	gtk_widget_سېټ_can_default(frdp->all, TRUE);
	gtk_box_pack_پیل(GTK_BOX(actionarea), frdp->all, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(frdp->all), "clicked",
			 G_CALLBACK(پیدا_کړه_replace_cb),
			 GINT_منزل_اشاره_ګر(FRD_REPLACEALL));
    }

    // 'Cancel' button
#إذا GTK_CHECK_نسخه(3,10,0)
    tmp = gtk_button_new_with_mnemonic(_("_Close"));
#وإلا
    tmp = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#د_که_پای
    gtk_widget_سېټ_can_default(tmp, TRUE);
    gtk_box_pack_پای_ټکی(GTK_BOX(actionarea), tmp, FALSE, FALSE, 0);
    g_signal_connect_swapped(G_OBJECT(tmp),
			     "clicked", G_CALLBACK(gtk_widget_hide),
			     G_OBJECT(frdp->dialog));
    g_signal_connect_swapped(G_OBJECT(frdp->dialog),
			     "delete-event", G_CALLBACK(gtk_widget_hide_on_delete),
			     G_OBJECT(frdp->dialog));

#إذا GTK_CHECK_نسخه(3,2,0)
    tmp = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
#وإلا
    tmp = gtk_vseparator_new();
#د_که_پای
    gtk_box_pack_پای_ټکی(GTK_BOX(hbox), tmp, FALSE, FALSE, 10);

    // Suppress automatic show of the unused action area
#إذا GTK_CHECK_نسخه(3,0,0)
# إذا !GTK_CHECK_نسخه(3,12,0)
    gtk_widget_hide(gtk_dialog_get_action_area(GTK_DIALOG(frdp->dialog)));
# endif
#وإلا
    gtk_widget_hide(GTK_DIALOG(frdp->dialog)->action_area);
#د_که_پای
    gtk_widget_show_all(hbox);
    gtk_widget_show(frdp->dialog);

    vim_free(ثبت_متن);
    vim_free(conv_بفر);
#تعریف_لغوه CONV
}

    فراغ
gui_mch_پیدا_کړه_dialog(exarg_T *eap)
{
    إذا (gui.دننه_use)
	پیدا_کړه_replace_dialog_create(eap->arg, FALSE);
}

    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (gui.دننه_use)
	پیدا_کړه_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Callback for actions of the find and replace dialogs
 */
    ساكن فراغ
پیدا_کړه_replace_cb(GtkWidget *widget ناکارول_شوی, gpointer معلومات)
{
    صحيح			flags;
    char_u		*پیدا_کړه_متن;
    char_u		*repl_متن;
    gboolean		direction_ښکته;
    Shared_پیدا_کړه_Replace_2	*sfr;

    flags = (صحيح)(اوږد)معلومات;	    // avoid a lint warning here

    // Get the search/replace strings from the dialog
    إذا (flags == FRD_FINDNEXT)
    {
	repl_متن = عدم;
	sfr = &پیدا_کړه_widgets;
    }
    وإلا
    {
	repl_متن = (char_u *)gtk_ثبت_get_متن(GTK_ثبت(repl_widgets.with));
	sfr = &repl_widgets;
    }

    پیدا_کړه_متن = (char_u *)gtk_ثبت_get_متن(GTK_ثبت(sfr->what));
    direction_ښکته = gtk_toggle_button_get_فعال(GTK_TOGGLE_BUTTON(sfr->ښکته));

    إذا (gtk_toggle_button_get_فعال(GTK_TOGGLE_BUTTON(sfr->wword)))
	flags |= FRD_ټولیز_کلمه;
    إذا (gtk_toggle_button_get_فعال(GTK_TOGGLE_BUTTON(sfr->mcase)))
	flags |= FRD_سمون_CASE;

    repl_متن = CONVERT_FROM_UTF8(repl_متن);
    پیدا_کړه_متن = CONVERT_FROM_UTF8(پیدا_کړه_متن);
    gui_do_findrepl(flags, پیدا_کړه_متن, repl_متن, direction_ښکته);
    CONVERT_FROM_UTF8_FREE(repl_متن);
    CONVERT_FROM_UTF8_FREE(پیدا_کړه_متن);
}

/*
 * our usual callback function
 */
    ساكن فراغ
ثبت_activate_cb(GtkWidget *widget ناکارول_شوی, gpointer معلومات)
{
    gtk_widget_grab_focus(GTK_WIDGET(معلومات));
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
    ساكن فراغ
ثبت_بدل_شوی_cb(GtkWidget * ثبت, GtkWidget * dialog)
{
    ثابت gchar	*ثبت_متن;
    gboolean	nonempty;

    ثبت_متن = gtk_ثبت_get_متن(GTK_ثبت(ثبت));

    إذا (!ثبت_متن)
	أرجع;			// shouldn't happen

    nonempty = (ثبت_متن[0] != '\0');

    إذا (dialog == پیدا_کړه_widgets.dialog)
	gtk_widget_سېټ_sensitive(پیدا_کړه_widgets.پیدا_کړه, nonempty);

    إذا (dialog == repl_widgets.dialog)
    {
	gtk_widget_سېټ_sensitive(repl_widgets.پیدا_کړه, nonempty);
	gtk_widget_سېټ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_سېټ_sensitive(repl_widgets.all, nonempty);
    }
}

/*
 * ":helpfind"
 */
    فراغ
ex_helpfind(exarg_T *eap ناکارول_شوی)
{
    // This will fail when menus are not loaded.  Well, it's only for
    // backwards compatibility anyway.
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

#إذا defined(FEAT_BROWSE)
    ساكن فراغ
recent_func_log_func(ثابت gchar *log_domain ناکارول_شوی,
		     GLogLevelFlags log_level ناکارول_شوی,
		     ثابت gchar *پیغام_2 ناکارول_شوی,
		     gpointer user_معلومات ناکارول_شوی)
{
    // We just want to suppress the warnings.
    // http://bugzilla.gnome.org/show_bug.cgi?id=664587
}
#د_که_پای

    فراغ
gui_mch_سېټ_fullscreen(صحيح flag)
{
    إذا (flag)
	gtk_کړکۍ_fullscreen(GTK_کړکۍ(gui.mainwin));
    وإلا
	gtk_کړکۍ_unfullscreen(GTK_کړکۍ(gui.mainwin));
}
