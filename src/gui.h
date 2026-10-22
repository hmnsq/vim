/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_تعریف_شده FEAT_GUI_MOTIF
# include <Xm/Xm.h>
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_GTK
# ifdef VMS
#  include "gui_gtk_vms.h"
# endif
# ifndef USE_GTK4
#  include <X11/Intrinsic.h>
# endif
# pragma GCC diagnostic افزودن
# pragma GCC diagnostic ignored "-Wstrict-prototypes"
# إذا !defined(USE_GTK3) && !defined(USE_GTK4)
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
# include <gtk/gtk.h>
# pragma GCC diagnostic برداشت
# ifdef USE_GTK4
// Types used in proto files but not available without X11 headers
تعريفنوع فراغ *Widget;
تعريفنوع فراغ *XtAppContext;
تعريفنوع فراغ  Display;
تعريفنوع بیعلامت دراز پنجره;
تعريفنوع بیعلامت دراز Atom;
تعريفنوع GdkEvent Gdk_Event_کلید;	// GTK4: GdkEventKey merged into GdkEvent
# endif
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_HAIKU
# include "gui_haiku.h"
#پایان_اگر

// Needed when generating prototypes, since FEAT_GUI is always defined then.
#إذا defined(FEAT_XCLIPBOARD) && !defined(FEAT_GUI_MOTIF) \
	&& !defined(FEAT_GUI_GTK)
# include <X11/Intrinsic.h>
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_PHOTON
# include <Ph.h>
# include <Pt.h>
# include "photon/PxProto.h"
#پایان_اگر

/*
 * On some systems scrolling needs to be done right away instead of in the
 * main loop.
 */
#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
# define USE_ON_FLY_SCROLL
#پایان_اگر

/*
 * GUIs that support dropping files on a running Vim.
 */
#إذا (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
# define HAVE_DROP_FILE
#پایان_اگر

/*
 * This define makes menus always use a fontset.
 * We're not sure if this code always works, thus it can be disabled.
 */
#اگر_تعریف_شده FEAT_XFONTSET
# define FONTSET_ALWAYS
#پایان_اگر

/*
 * These macros convert between character row/column and pixel coordinates.
 * TEXT_X   - Convert character column into X pixel coord for drawing strings.
 * TEXT_Y   - Convert character row into Y pixel coord for drawing strings.
 * FILL_X   - Convert character column into X pixel coord for filling the area
 *		under the character.
 * FILL_Y   - Convert character row into Y pixel coord for filling the area
 *		under the character.
 * X_2_COL  - Convert X pixel coord into character column.
 * Y_2_ROW  - Convert Y pixel coord into character row.
 */
#اگر_تعریف_شده FEAT_GUI_MSWIN
# define متن_X(ستون)	((ستون) * gui.char_عرض)
# define متن_Y(ردیف)	((ردیف) * gui.char_ارتفاع + gui.char_ascent)
# define FILL_X(ستون)	((ستون) * gui.char_عرض)
# define FILL_Y(ردیف)	((ردیف) * gui.char_ارتفاع)
# define X_2_ستون(x)	((x) / gui.char_عرض)
# define Y_2_ردیف(y)	((y) / gui.char_ارتفاع)
#وإلا
# define متن_X(ستون)	((ستون) * gui.char_عرض  + gui.border_جابه_جایی)
# define FILL_X(ستون)	((ستون) * gui.char_عرض  + gui.border_جابه_جایی)
# define X_2_ستون(x)	(((x) - gui.border_جابه_جایی) / gui.char_عرض)
# define متن_Y(ردیف)	((ردیف) * gui.char_ارتفاع + gui.char_ascent \
							+ gui.border_جابه_جایی)
# define FILL_Y(ردیف)	((ردیف) * gui.char_ارتفاع + gui.border_جابه_جایی)
# define Y_2_ردیف(y)	(((y) - gui.border_جابه_جایی) / gui.char_ارتفاع)
#پایان_اگر

// Indices for arrays of scrollbars
#تعریف_کن SBAR_NONE	    (-1)
#تعریف_کن SBAR_چپ	    0
#تعریف_کن SBAR_راست	    1
#تعریف_کن SBAR_BOTTOM	    2

// Orientations for scrollbars
#تعریف_کن SBAR_VERT	    0
#تعریف_کن SBAR_HORIZ	    1

// Default size of scrollbar
#تعریف_کن SB_DEFAULT_عرض    16

// Default height of the menu bar
#تعریف_کن MENU_DEFAULT_ارتفاع 1		// figure it out at runtime

// Flags for gui_mch_outstr_nowrap()
#تعریف_کن GUI_MON_WRAP_CURSOR	0x01	// wrap cursor at end of line
#تعریف_کن GUI_MON_INVERT		0x02	// invert the characters
#تعریف_کن GUI_MON_IS_CURSOR	0x04	// drawing cursor
#تعریف_کن GUI_MON_TRS_CURSOR	0x08	// drawing transparent cursor
#تعریف_کن GUI_MON_NOCLEAR		0x10	// don't clear selection

// Flags for gui_mch_draw_string()
#تعریف_کن DRAW_TRANSP		0x01	// draw with transparent bg
#تعریف_کن DRAW_BOLD		0x02	// draw bold text
#تعریف_کن DRAW_UNDERL		0x04	// draw underline text
#تعریف_کن DRAW_UNDERC		0x08	// draw undercurl text
#إذا defined(FEAT_GUI_GTK)
# define DRAW_ITALIC		0x10	// draw italic text
#پایان_اگر
#تعریف_کن DRAW_CURSOR		0x20	// drawing block cursor (win32)
#تعریف_کن DRAW_STRIKE		0x40	// strikethrough

// For our own tearoff menu item
#تعریف_کن TEAR_STRING		"-->Detach"
#تعریف_کن TEAR_LEN		(9)	// length of above string

// for the toolbar
#تعریف_کن TOOLBAR_BUTTON_ارتفاع	18
#تعریف_کن TOOLBAR_BUTTON_عرض	18
#تعریف_کن TOOLBAR_BORDER_ارتفاع	12  // room above+below buttons for MSWindows

#اگر_تعریف_شده FEAT_GUI_MSWIN
# define TABLINE_ارتفاع 22
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_MOTIF
# define TABLINE_ارتفاع 30
#پایان_اگر

#إذا defined(NO_CONSOLE) || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
# define NO_CONSOLE_درون_داد	// use no_console_input() to check if there
				// is no console input possible
#پایان_اگر

تعريفنوع بنية GuiScrollbar
{
    دراز	ident;		// Unique identifier for each scrollbar
    win_T	*wp;		// Scrollbar's window, NULL for bottom
    صحيح		type;		// one of SBAR_{LEFT,RIGHT,BOTTOM}
    دراز	مقدار;		// Represents top line number visible
    دراز	اندازه;		// Size of scrollbar thumb
    دراز	max;		// Number of lines in buffer

    // Values measured in characters:
    صحيح		بالاترین;		// Top of scroll bar (chars from row 0)
    صحيح		ارتفاع;		// Current height of scroll bar in rows
    صحيح		عرض;		// Current width of scroll bar in cols
    صحيح		وضعیت_ارتفاع;	// Height of status line
#اگر_تعریف_شده FEAT_GUI_X11
    Widget	id;		// Id of real scroll bar
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_GTK
    GtkWidget *id;		// Id of real scroll bar
    بیعلامت دراز handler_id;   // Id of "value_changed" signal handler
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_MSWIN
    HWND	id;		// Id of real scroll bar
    صحيح		scroll_لغزش;	// The scrollbar stuff can handle only up to
				// 32767 lines.  When the file is longer,
				// scroll_shift is set to the number of shifts
				// to reduce the count.
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_HAIKU
    VimScrollBar *id;		// Pointer to real scroll bar
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_PHOTON
    PtWidget_t	*id;
#پایان_اگر
} scrollbar_T;

تعريفنوع دراز	    guicolor_T;	// handle for a GUI color; for X11 this should
				// be "Pixel", but that's an unsigned and we
				// need a signed value
#تعریف_کن INVALCOLOR ((guicolor_T)-11111)	// number for invalid color; on 32 bit
				   // displays there is a tiny chance this is an
				   // actual color
#تعریف_کن CTERMCOLOR ((guicolor_T)-11110)	// only used for cterm.bg_rgb and
					// cterm.fg_rgb: use cterm color

#اگر_تعریف_شده FEAT_GUI_GTK
  تعريفنوع PangoFontDescription	*GuiFont;       // handle for a GUI font
  تعريفنوع PangoFontDescription  *GuiFontset;    // handle for a GUI fontset
# define NOFONT		(GuiFont)عدم
# define NOFONTSET	(GuiFontset)عدم
#وإلا
# ifdef FEAT_GUI_PHOTON
  تعريفنوع حرف		*GuiFont;
  تعريفنوع حرف		*GuiFontset;
#  define NOFONT	(GuiFont)عدم
#  define NOFONTSET	(GuiFontset)عدم
# وإلا
#  ifdef FEAT_GUI_X11
  تعريفنوع XFontStruct	*GuiFont;	// handle for a GUI font
  تعريفنوع XFont_مجموعه	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  وإلا
  تعريفنوع long_u	GuiFont;	// handle for a GUI font
  تعريفنوع long_u	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  endif
# endif
#پایان_اگر

#اگر_تعریف_شده VIMDLL
// Use spawn when GUI is starting.
# define GUI_MAY_SPAWN

// Uncomment the next definition if you want to use the `:gui` command on
// Windows.  It uses `:mksession` to inherit the session from vim.exe to
// gvim.exe.  So, it doesn't work perfectly. (EXPERIMENTAL)
//# define EXPERIMENTAL_GUI_CMD
#پایان_اگر

تعريفنوع بنية Gui
{
    bool	درون_focus;	    // Vim has input focus
    bool	درون_use;		    // Is the GUI being used?
    bool	starting;	    // GUI will start in a little while
    bool	shell_created;	    // Has the shell been created yet?
    bool	dying;		    // Is vim dying? Then output to terminal
    bool	dofork;		    // Use fork() when GUI is starting
#اگر_تعریف_شده GUI_MAY_SPAWN
    bool	dospawn;	    // Use spawn() when GUI is starting
#پایان_اگر
    صحيح		dragged_sb;	    // Which scrollbar being dragged, if any?
    win_T	*dragged_wp;	    // Which WIN's sb being dragged, if any?
    bool	اشاره_گر_hidden;	    // Is the mouse pointer hidden?
    صحيح		ستون;		    // Current cursor column in GUI display
    صحيح		ردیف;		    // Current cursor row in GUI display
    صحيح		cursor_ستون;	    // Physical cursor column in GUI display
    صحيح		cursor_ردیف;	    // Physical cursor row in GUI display
    bool	cursor_is_valid;    // There is a cursor at cursor_row/col
    صحيح		رقم_تعداد_ستون;	    // Number of columns
    صحيح		رقم_ردیفها;	    // Number of rows
    صحيح		scroll_region_بالاترین;  // Top (first) line of scroll region
    صحيح		scroll_region_bot;  // Bottom (last) line of scroll region
    صحيح		scroll_region_چپ;  // Left (first) column of scroll region
    صحيح		scroll_region_راست;  // Right (last) col. of scroll region
    صحيح		highlight_ماسک;	    // Highlight attribute mask
    صحيح		scrollbar_عرض;    // Width of vertical scrollbars
    صحيح		scrollbar_ارتفاع;   // Height of horizontal scrollbar
    صحيح		چپ_sbar_x;	    // Calculated x coord for left scrollbar
    صحيح		راست_sbar_x;	    // Calculated x coord for right scrollbar
    bool	force_redraw;       // Force a redraw even e.g. not resized
#اگر_تعریف_شده FEAT_DIRECTX
    bool	directx_enabled;    // DirectX (DirectWrite) rendering active
#پایان_اگر
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    صحيح		bleed_راست;	    // Number of pixels to bleed bg color right
    صحيح		bleed_bot;	    // Number of pixels to bleed bg color down
#پایان_اگر

#اگر_تعریف_شده FEAT_MENU
# ifndef FEAT_GUI_GTK
    صحيح		menu_ارتفاع;	    // Height of the menu bar
    صحيح		menu_عرض;	    // Width of the menu bar
# endif
    bool	menu_is_فعال;	    // true if menu is present
#پایان_اگر

    scrollbar_T bottom_sbar;	    // Bottom scrollbar
    bool	which_scrollbars[3];// Which scrollbar boxes are active?
    صحيح		prev_wrap;	    // For updating the horizontal scrollbar
    صحيح		char_عرض;	    // Width of char cell in pixels
    صحيح		char_ارتفاع;	    // Height of char cell in pixels, includes
				    // 'linespace'
    صحيح		char_ascent;	    // Ascent of char in pixels
    صحيح		border_عرض;	    // Width of our border around text area
    صحيح		border_جابه_جایی;	    // Total pixel offset for all borders

    GuiFont	norm_font;	    // Normal font
#اگر_تعریف_نشده FEAT_GUI_GTK
    GuiFont	bold_font;	    // Bold font
    GuiFont	ital_font;	    // Italic font
    GuiFont	boldital_font;	    // Bold-Italic font
#وإلا
    bool	font_can_bold;	    // Whether norm_font supports bold weight.
				    // The styled font variants are not used.
#پایان_اگر

#إذا defined(FEAT_MENU) && !defined(FEAT_GUI_GTK)
# ifdef FONTSET_ALWAYS
    GuiFontset	menu_fontset;	    // set of fonts for multi-byte chars
# وإلا
    GuiFont	menu_font;	    // menu item font
# endif
#پایان_اگر
    GuiFont	wide_font;	    // Normal 'guifontwide' font
#اگر_تعریف_نشده FEAT_GUI_GTK
    GuiFont	wide_bold_font;	    // Bold 'guifontwide' font
    GuiFont	wide_ital_font;	    // Italic 'guifontwide' font
    GuiFont	wide_boldital_font; // Bold-Italic 'guifontwide' font
#پایان_اگر
#اگر_تعریف_شده FEAT_XFONTSET
    GuiFontset	fontset;	    // set of fonts for multi-byte chars
#پایان_اگر
    guicolor_T	پشت_pixel;	    // Color of background
    guicolor_T	norm_pixel;	    // Color of normal text
    guicolor_T	def_پشت_pixel;	    // default Color of background
    guicolor_T	def_norm_pixel;	    // default Color of normal text

#اگر_تعریف_شده FEAT_GUI_X11
    حرف	*rsrc_menu_fg_name;	// Color of menu & dialog foreground
    guicolor_T	menu_fg_pixel;		// Same in Pixel format
    حرف	*rsrc_menu_bg_name;	// Color of menu & dialog background
    guicolor_T	menu_bg_pixel;		// Same in Pixel format
    حرف	*rsrc_scroll_fg_name;	// Color of scrollbar foreground
    guicolor_T	scroll_fg_pixel;	// Same in Pixel format
    حرف	*rsrc_scroll_bg_name;	// Color of scrollbar background
    guicolor_T	scroll_bg_pixel;	// Same in Pixel format

    Display	*dpy;		    // X display
    پنجره	wid;		    // Window id of text area
    صحيح		visibility;	    // Is shell partially/fully obscured?
    GC		متن_gc;
    GC		پشت_gc;
    GC		invert_gc;
    Cursor	blank_اشاره_گر;	    // Blank pointer

    // X Resources
    char_u	*rsrc_font_name;    // Resource font name, used if 'guifont'
				    // not set
    char_u	*rsrc_bold_font_name; // Resource bold font name
    char_u	*rsrc_ital_font_name; // Resource italic font name
    char_u	*rsrc_boldital_font_name;  // Resource bold-italic font name
    char_u	*rsrc_menu_font_name;    // Resource menu Font name
    Bool	rsrc_rev_video;	    // Use reverse video?

    char_u	*geom;		    // Geometry, eg "80x24"
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_GTK
# إذا !defined(USE_GTK3) && !defined(USE_GTK4)
    صحيح		visibility;	    // Is shell partially/fully obscured?
# endif
    GdkCursor	*blank_اشاره_گر;	    // Blank pointer

    // X Resources
    char_u	*geom;		    // Geometry, eg "80x24"

    GtkWidget	*mainwin;	    // top level GTK window
    GtkWidget	*formwin;	    // manages all the windows below
    GtkWidget	*drawarea;	    // the "text" area
# ifdef FEAT_MENU
    GtkWidget	*menubar;	    // menubar
# endif
# ifdef FEAT_TOOLBAR
    GtkWidget	*toolbar;	    // toolbar
# endif
# ifdef FEAT_GUI_GNOME
    GtkWidget	*menubar_h;	    // menubar handle
    GtkWidget	*toolbar_h;	    // toolbar handle
# endif
# إذا defined(USE_GTK3) || defined(USE_GTK4)
    GdkRGBA	*fgcolor;	    // GDK-styled foreground color
    GdkRGBA	*bgcolor;	    // GDK-styled background color
    GdkRGBA	*spcolor;	    // GDK-styled special color
# وإلا
    GdkColor	*fgcolor;	    // GDK-styled foreground color
    GdkColor	*bgcolor;	    // GDK-styled background color
    GdkColor	*spcolor;	    // GDK-styled special color
# endif
# إذا defined(USE_GTK3) && !defined(USE_GTK4)
    cairo_surface_t *surface;       // drawarea surface
# elif !defined(USE_GTK4)
    GdkGC	*متن_gc;	    // cached GC for normal text
# endif
    PangoContext     *متن_context; // the context used for all text
    PangoFont	     *ascii_font;   // cached font for ASCII strings
    PangoGlyphString *ascii_glyphs; // cached code point -> glyph map
# ifdef FEAT_GUI_TABLINE
    GtkWidget	*tabline;	    // tab pages line handle
# endif

# ifndef USE_GTK4
    Gtk_Accel_گروه *accel_گروه;
# endif
    GtkWidget	*filedlg;	    // file selection dialog
    char_u	*browse_fname;	    // file name from filedlg

    guint32	event_زمان;
# ifdef FEAT_GUI_DIALOG
    // Multiple dialogs not allowed, just tracked for future use.
    صحيح		dialogs_فعال;     // number of active GUI dialogs
    // Suppress focus-in event when focus returns.
    صحيح		dialog_focus_pending;
# endif
    bool	is_x11;	            // active gdk backend in gtk is x11
# ifdef GDK_WINDOWING_WAYLAND
    bool	is_wayland;	    // active gdk backend in gtk is wayland
# endif
# إذا defined(FEAT_IMAGE) && defined(FEAT_GUI_GTK)
    مزدوج	scale;		    // Current scaling (may be fractional)
# endif
#پایان_اگر	// FEAT_GUI_GTK

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    char_u ligatures_map[256];	    // ascii map for characters 0-255, value is
				    // 1 if in 'guiligatures'
#پایان_اگر

#إذا defined(FEAT_GUI_TABLINE) \
	&& (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
		|| defined(FEAT_GUI_HAIKU))
    صحيح		tabline_ارتفاع;
#پایان_اگر

#إذا defined(FEAT_TOOLBAR) \
	&& (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    صحيح		toolbar_ارتفاع;	    // height of the toolbar
#پایان_اگر

#اگر_تعریف_شده FEAT_BEVAL_TIP
    // Tooltip properties; also used for balloon evaluation
    char_u	*rsrc_tooltip_font_name; // tooltip font name
    حرف	*rsrc_tooltip_fg_name;	// tooltip foreground color name
    حرف	*rsrc_tooltip_bg_name;	// tooltip background color name
    guicolor_T	tooltip_fg_pixel;	// tooltip foreground color
    guicolor_T	tooltip_bg_pixel;	// tooltip background color
    XFont_مجموعه	tooltip_fontset;	// tooltip fontset
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_MSWIN
    GuiFont	currFont;	    // Current font
    guicolor_T	currFgColor;	    // Current foreground text color
    guicolor_T	currBgColor;	    // Current background text color
    guicolor_T	currSpColor;	    // Current special text color

    guicolor_T	title_bg_pixel;	    // window title bar color
    guicolor_T	title_fg_pixel;	    // window title bar's text color
    guicolor_T	titlenc_bg_pixel;   // window title bar color not current
    guicolor_T	titlenc_fg_pixel;   // window title bar's text color not current
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_HAIKU
    VimApp     *vimApp;
    Vim_پنجره  *vim_پنجره;
    VimFormView *vimForm;
    Vim_متن_Area_View *vim_متن_Area;
    صحيح	vdcmp;			    // Vim Direct Communication Message Port
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_PHOTON
    PtWidget_t	*vim_پنجره;		// PtWindow
    PtWidget_t	*vim_متن_Area;		// PtRaw
    PtWidget_t	*vimContainer;		// PtPanel
# إذا defined(FEAT_MENU) || defined(FEAT_TOOLBAR)
    PtWidget_t	*vim_Tool_Bar_گروه;
# endif
# ifdef FEAT_MENU
    PtWidget_t	*vimMenuBar;
# endif
# ifdef FEAT_TOOLBAR
    PtWidget_t	*vimToolBar;
    صحيح		toolbar_ارتفاع;
# endif
    PhEvent_t	*event_میانگیر;
#پایان_اگر

#اگر_تعریف_شده FEAT_XIM
    حرف	*rsrc_درون_داد_روش;
    حرف	*rsrc_preedit_type_name;
#پایان_اگر
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    صحيح decor_ارتفاع;

    // Size of the form widget last asked for with gui_mch_set_shellsize().
    // "pending_form_skip" counts how many allocations that do not answer it
    // may still be ignored.
    صحيح pending_form_w;
    صحيح pending_form_h;
    صحيح pending_form_skip;

    // Used for clipboard functionality in GTK4 GUI
    GdkContentProvider *regular_provider;
    GdkContentProvider *primary_provider;

# ifdef FEAT_DND
    Gtk_Drop_هدف_Async *drop_هدف;
# endif
#پایان_اگر
} gui_T;

خارجي gui_T gui;			// this is defined in gui.c

// definitions of available window positionings for gui_*_position_in_parent()
تعريفنوع تعداد
{
    VW_POS_MOUSE,
    VW_POS_CENTER,
    VW_POS_بالاترین_CENTER
} gui_win_pos_T;

#اگر_تعریف_شده پیدا_کن_REPLACE_DIALOG
/*
 * Flags used to distinguish the different contexts in which the
 * find/replace callback may be called.
 */
# define FRD_FINDNEXT	1	// Find next in find dialog
# define FRD_R_FINDNEXT	2	// Find next in repl dialog
# define FRD_REPLACE	3	// Replace once
# define FRD_REPLACEALL	4	// Replace remaining matches
# define FRD_UNDO	5	// Undo replaced text
# define FRD_TYPE_ماسک   7	// Mask for the callback type
// Flags which change the way searching is done.
# define FRD_کامل_کلمه	0x08	// match whole word only
# define FRD_تطابق_CASE	0x10	// match case
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_GTK
/*
 * Convenience macros to convert from 'encoding' to 'termencoding' and
 * vice versa.	If no conversion is necessary the passed-in pointer is
 * returned as is, without allocating any memory.  Thus additional _FREE()
 * macros are provided.  The _FREE() macros also set the pointer to NULL,
 * in order to avoid bugs due to illegal memory access only happening if
 * 'encoding' != utf-8...
 *
 * Defining these macros as pure expressions looks a bit tricky but
 * avoids depending on the context of the macro expansion.  One of the
 * rare occasions where the comma operator comes in handy :)
 *
 * Note: Do NOT keep the result around when handling control back to
 * the main Vim!  The user could change 'encoding' at any time.
 */
# define CONVERT_مقصد_UTF8(String)				\
    ((output_conv.vc_type == CONV_NONE || (String) == عدم)	\
	    ? (String)						\
	    : string_convert(&output_conv, (String), عدم))

# define CONVERT_مقصد_UTF8_FREE(String)				\
    ((String) = ((output_conv.vc_type == CONV_NONE)		\
			? (char_u *)عدم			\
			: (vim_free(String), (char_u *)عدم)))

# define CONVERT_FROM_UTF8(String)				\
    ((درون_داد_conv.vc_type == CONV_NONE || (String) == عدم)	\
	    ? (String)						\
	    : string_convert(&درون_داد_conv, (String), عدم))

# define CONVERT_FROM_UTF8_FREE(String)				\
    ((String) = ((درون_داد_conv.vc_type == CONV_NONE)		\
			? (char_u *)عدم			\
			: (vim_free(String), (char_u *)عدم)))

#وإلا
# define CONVERT_مقصد_UTF8(String) (String)
# define CONVERT_مقصد_UTF8_FREE(String) ((String) = (char_u *)عدم)
# define CONVERT_FROM_UTF8(String) (String)
# define CONVERT_FROM_UTF8_FREE(String) ((String) = (char_u *)عدم)
#پایان_اگر // FEAT_GUI_GTK

#اگر_تعریف_شده FEAT_GUI_GTK
/*
 * The second parameter of g_signal_handlers_disconnect_by_func() is supposed
 * to be a function pointer which was passed to g_signal_connect_*() somewhere
 * previously, and hence it must be of type GCallback, i.e., void (*)(void).
 *
 * Meanwhile, g_signal_handlers_disconnect_by_func() is a macro calling
 * g_signal_handlers_disconnect_matched(), and the second parameter of the
 * former is to be passed to the sixth parameter of the latter the type of
 * which, however, is declared as void * in the function signature.
 *
 * While the ISO C Standard does not require that function pointers be
 * interconvertible to void *, widely-used compilers such as gcc and clang
 * do such conversion implicitly and automatically on some platforms without
 * issuing any warning.
 *
 * For Solaris Studio, that is not the case.  An explicit type cast is needed
 * to suppress warnings on that particular conversion.
 */
# إذا defined(__SUNPRO_C) && (defined(USE_GTK3) || defined(USE_GTK4))
#  define FUNC2GENERIC(func) (فراغ *)(func)
# وإلا
#  define FUNC2GENERIC(func) G_CALLBACK(func)
# endif
#پایان_اگر // FEAT_GUI_GTK

#إذا defined(UNIX)
# define GUI_MAY_FORK
#پایان_اگر
