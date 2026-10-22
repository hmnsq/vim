/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#که_تعریف_شوی FEAT_GUI_MOTIF
# include <Xm/Xm.h>
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GTK
# ifdef VMS
#  include "gui_gtk_vms.h"
# endif
# ifndef USE_GTK4
#  include <X11/Intrinsic.h>
# endif
# pragma GCC diagnostic ټېل
# pragma GCC diagnostic ignored "-Wstrict-prototypes"
# إذا !defined(USE_GTK3) && !defined(USE_GTK4)
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
# include <gtk/gtk.h>
# pragma GCC diagnostic راوباسه
# ifdef USE_GTK4
// Types used in proto files but not available without X11 headers
تعريفنوع فراغ *Widget;
تعريفنوع فراغ *XtAppContext;
تعريفنوع فراغ  Display;
تعريفنوع بینښه اوږد کړکۍ;
تعريفنوع بینښه اوږد Atom;
تعريفنوع GdkEvent Gdk_Event_کلی;	// GTK4: GdkEventKey merged into GdkEvent
# endif
#د_که_پای

#که_تعریف_شوی FEAT_GUI_HAIKU
# include "gui_haiku.h"
#د_که_پای

// Needed when generating prototypes, since FEAT_GUI is always defined then.
#إذا defined(FEAT_XCLIPBOARD) && !defined(FEAT_GUI_MOTIF) \
	&& !defined(FEAT_GUI_GTK)
# include <X11/Intrinsic.h>
#د_که_پای

#که_تعریف_شوی FEAT_GUI_PHOTON
# include <Ph.h>
# include <Pt.h>
# include "photon/PxProto.h"
#د_که_پای

/*
 * On some systems scrolling needs to be done right away instead of in the
 * main loop.
 */
#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
# define USE_ON_FLY_SCROLL
#د_که_پای

/*
 * GUIs that support dropping files on a running Vim.
 */
#إذا (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
# define HAVE_DROP_FILE
#د_که_پای

/*
 * This define makes menus always use a fontset.
 * We're not sure if this code always works, thus it can be disabled.
 */
#که_تعریف_شوی FEAT_XFONTSET
# define FONTSET_ALWAYS
#د_که_پای

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
#که_تعریف_شوی FEAT_GUI_MSWIN
# define متن_X(ستون)	((ستون) * gui.char_پلنوالی)
# define متن_Y(لیکه)	((لیکه) * gui.char_لوړوالی + gui.char_ascent)
# define FILL_X(ستون)	((ستون) * gui.char_پلنوالی)
# define FILL_Y(لیکه)	((لیکه) * gui.char_لوړوالی)
# define X_2_ستون(x)	((x) / gui.char_پلنوالی)
# define Y_2_لیکه(y)	((y) / gui.char_لوړوالی)
#وإلا
# define متن_X(ستون)	((ستون) * gui.char_پلنوالی  + gui.border_دننه_واټن)
# define FILL_X(ستون)	((ستون) * gui.char_پلنوالی  + gui.border_دننه_واټن)
# define X_2_ستون(x)	(((x) - gui.border_دننه_واټن) / gui.char_پلنوالی)
# define متن_Y(لیکه)	((لیکه) * gui.char_لوړوالی + gui.char_ascent \
							+ gui.border_دننه_واټن)
# define FILL_Y(لیکه)	((لیکه) * gui.char_لوړوالی + gui.border_دننه_واټن)
# define Y_2_لیکه(y)	(((y) - gui.border_دننه_واټن) / gui.char_لوړوالی)
#د_که_پای

// Indices for arrays of scrollbars
#تعریف_کړه SBAR_NONE	    (-1)
#تعریف_کړه SBAR_کیڼ	    0
#تعریف_کړه SBAR_ښی	    1
#تعریف_کړه SBAR_BOTTOM	    2

// Orientations for scrollbars
#تعریف_کړه SBAR_VERT	    0
#تعریف_کړه SBAR_HORIZ	    1

// Default size of scrollbar
#تعریف_کړه SB_DEFAULT_پلنوالی    16

// Default height of the menu bar
#تعریف_کړه MENU_DEFAULT_لوړوالی 1		// figure it out at runtime

// Flags for gui_mch_outstr_nowrap()
#تعریف_کړه GUI_MON_WRAP_CURSOR	0x01	// wrap cursor at end of line
#تعریف_کړه GUI_MON_INVERT		0x02	// invert the characters
#تعریف_کړه GUI_MON_IS_CURSOR	0x04	// drawing cursor
#تعریف_کړه GUI_MON_TRS_CURSOR	0x08	// drawing transparent cursor
#تعریف_کړه GUI_MON_NOCLEAR		0x10	// don't clear selection

// Flags for gui_mch_draw_string()
#تعریف_کړه DRAW_TRANSP		0x01	// draw with transparent bg
#تعریف_کړه DRAW_BOLD		0x02	// draw bold text
#تعریف_کړه DRAW_UNDERL		0x04	// draw underline text
#تعریف_کړه DRAW_UNDERC		0x08	// draw undercurl text
#إذا defined(FEAT_GUI_GTK)
# define DRAW_ITALIC		0x10	// draw italic text
#د_که_پای
#تعریف_کړه DRAW_CURSOR		0x20	// drawing block cursor (win32)
#تعریف_کړه DRAW_STRIKE		0x40	// strikethrough

// For our own tearoff menu item
#تعریف_کړه TEAR_STRING		"-->Detach"
#تعریف_کړه TEAR_LEN		(9)	// length of above string

// for the toolbar
#تعریف_کړه TOOLBAR_BUTTON_لوړوالی	18
#تعریف_کړه TOOLBAR_BUTTON_پلنوالی	18
#تعریف_کړه TOOLBAR_BORDER_لوړوالی	12  // room above+below buttons for MSWindows

#که_تعریف_شوی FEAT_GUI_MSWIN
# define TABLINE_لوړوالی 22
#د_که_پای
#که_تعریف_شوی FEAT_GUI_MOTIF
# define TABLINE_لوړوالی 30
#د_که_پای

#إذا defined(NO_CONSOLE) || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
# define NO_CONSOLE_ننوت	// use no_console_input() to check if there
				// is no console input possible
#د_که_پای

تعريفنوع بنية GuiScrollbar
{
    اوږد	ident;		// Unique identifier for each scrollbar
    win_T	*wp;		// Scrollbar's window, NULL for bottom
    صحيح		type;		// one of SBAR_{LEFT,RIGHT,BOTTOM}
    اوږد	ارزښت;		// Represents top line number visible
    اوږد	اندازه;		// Size of scrollbar thumb
    اوږد	max;		// Number of lines in buffer

    // Values measured in characters:
    صحيح		سر_2;		// Top of scroll bar (chars from row 0)
    صحيح		لوړوالی;		// Current height of scroll bar in rows
    صحيح		پلنوالی;		// Current width of scroll bar in cols
    صحيح		حالت_لوړوالی;	// Height of status line
#که_تعریف_شوی FEAT_GUI_X11
    Widget	id;		// Id of real scroll bar
#د_که_پای
#که_تعریف_شوی FEAT_GUI_GTK
    GtkWidget *id;		// Id of real scroll bar
    بینښه اوږد handler_id;   // Id of "value_changed" signal handler
#د_که_پای

#که_تعریف_شوی FEAT_GUI_MSWIN
    HWND	id;		// Id of real scroll bar
    صحيح		scroll_ښويدنه;	// The scrollbar stuff can handle only up to
				// 32767 lines.  When the file is longer,
				// scroll_shift is set to the number of shifts
				// to reduce the count.
#د_که_پای

#که_تعریف_شوی FEAT_GUI_HAIKU
    VimScrollBar *id;		// Pointer to real scroll bar
#د_که_پای
#که_تعریف_شوی FEAT_GUI_PHOTON
    PtWidget_t	*id;
#د_که_پای
} scrollbar_T;

تعريفنوع اوږد	    guicolor_T;	// handle for a GUI color; for X11 this should
				// be "Pixel", but that's an unsigned and we
				// need a signed value
#تعریف_کړه INVALCOLOR ((guicolor_T)-11111)	// number for invalid color; on 32 bit
				   // displays there is a tiny chance this is an
				   // actual color
#تعریف_کړه CTERMCOLOR ((guicolor_T)-11110)	// only used for cterm.bg_rgb and
					// cterm.fg_rgb: use cterm color

#که_تعریف_شوی FEAT_GUI_GTK
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
  تعريفنوع XFont_سېټ	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  وإلا
  تعريفنوع long_u	GuiFont;	// handle for a GUI font
  تعريفنوع long_u	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  endif
# endif
#د_که_پای

#که_تعریف_شوی VIMDLL
// Use spawn when GUI is starting.
# define GUI_MAY_SPAWN

// Uncomment the next definition if you want to use the `:gui` command on
// Windows.  It uses `:mksession` to inherit the session from vim.exe to
// gvim.exe.  So, it doesn't work perfectly. (EXPERIMENTAL)
//# define EXPERIMENTAL_GUI_CMD
#د_که_پای

تعريفنوع بنية Gui
{
    bool	دننه_focus;	    // Vim has input focus
    bool	دننه_use;		    // Is the GUI being used?
    bool	starting;	    // GUI will start in a little while
    bool	shell_created;	    // Has the shell been created yet?
    bool	dying;		    // Is vim dying? Then output to terminal
    bool	dofork;		    // Use fork() when GUI is starting
#که_تعریف_شوی GUI_MAY_SPAWN
    bool	dospawn;	    // Use spawn() when GUI is starting
#د_که_پای
    صحيح		dragged_sb;	    // Which scrollbar being dragged, if any?
    win_T	*dragged_wp;	    // Which WIN's sb being dragged, if any?
    bool	اشاره_ګر_hidden;	    // Is the mouse pointer hidden?
    صحيح		ستون;		    // Current cursor column in GUI display
    صحيح		لیکه;		    // Current cursor row in GUI display
    صحيح		cursor_ستون;	    // Physical cursor column in GUI display
    صحيح		cursor_لیکه;	    // Physical cursor row in GUI display
    bool	cursor_is_valid;    // There is a cursor at cursor_row/col
    صحيح		عدد_ستون_شمېر;	    // Number of columns
    صحيح		عدد_لیکې;	    // Number of rows
    صحيح		scroll_region_سر;  // Top (first) line of scroll region
    صحيح		scroll_region_bot;  // Bottom (last) line of scroll region
    صحيح		scroll_region_کیڼ;  // Left (first) column of scroll region
    صحيح		scroll_region_ښی;  // Right (last) col. of scroll region
    صحيح		highlight_ماسک;	    // Highlight attribute mask
    صحيح		scrollbar_پلنوالی;    // Width of vertical scrollbars
    صحيح		scrollbar_لوړوالی;   // Height of horizontal scrollbar
    صحيح		کیڼ_sbar_x;	    // Calculated x coord for left scrollbar
    صحيح		ښی_sbar_x;	    // Calculated x coord for right scrollbar
    bool	force_redraw;       // Force a redraw even e.g. not resized
#که_تعریف_شوی FEAT_DIRECTX
    bool	directx_enabled;    // DirectX (DirectWrite) rendering active
#د_که_پای
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    صحيح		bleed_ښی;	    // Number of pixels to bleed bg color right
    صحيح		bleed_bot;	    // Number of pixels to bleed bg color down
#د_که_پای

#که_تعریف_شوی FEAT_MENU
# ifndef FEAT_GUI_GTK
    صحيح		menu_لوړوالی;	    // Height of the menu bar
    صحيح		menu_پلنوالی;	    // Width of the menu bar
# endif
    bool	menu_is_فعال;	    // true if menu is present
#د_که_پای

    scrollbar_T bottom_sbar;	    // Bottom scrollbar
    bool	which_scrollbars[3];// Which scrollbar boxes are active?
    صحيح		prev_wrap;	    // For updating the horizontal scrollbar
    صحيح		char_پلنوالی;	    // Width of char cell in pixels
    صحيح		char_لوړوالی;	    // Height of char cell in pixels, includes
				    // 'linespace'
    صحيح		char_ascent;	    // Ascent of char in pixels
    صحيح		border_پلنوالی;	    // Width of our border around text area
    صحيح		border_دننه_واټن;	    // Total pixel offset for all borders

    GuiFont	norm_font;	    // Normal font
#که_نه_تعریف_شوی FEAT_GUI_GTK
    GuiFont	bold_font;	    // Bold font
    GuiFont	ital_font;	    // Italic font
    GuiFont	boldital_font;	    // Bold-Italic font
#وإلا
    bool	font_can_bold;	    // Whether norm_font supports bold weight.
				    // The styled font variants are not used.
#د_که_پای

#إذا defined(FEAT_MENU) && !defined(FEAT_GUI_GTK)
# ifdef FONTSET_ALWAYS
    GuiFontset	menu_fontset;	    // set of fonts for multi-byte chars
# وإلا
    GuiFont	menu_font;	    // menu item font
# endif
#د_که_پای
    GuiFont	wide_font;	    // Normal 'guifontwide' font
#که_نه_تعریف_شوی FEAT_GUI_GTK
    GuiFont	wide_bold_font;	    // Bold 'guifontwide' font
    GuiFont	wide_ital_font;	    // Italic 'guifontwide' font
    GuiFont	wide_boldital_font; // Bold-Italic 'guifontwide' font
#د_که_پای
#که_تعریف_شوی FEAT_XFONTSET
    GuiFontset	fontset;	    // set of fonts for multi-byte chars
#د_که_پای
    guicolor_T	شا_pixel;	    // Color of background
    guicolor_T	norm_pixel;	    // Color of normal text
    guicolor_T	def_شا_pixel;	    // default Color of background
    guicolor_T	def_norm_pixel;	    // default Color of normal text

#که_تعریف_شوی FEAT_GUI_X11
    حرف	*rsrc_menu_fg_name;	// Color of menu & dialog foreground
    guicolor_T	menu_fg_pixel;		// Same in Pixel format
    حرف	*rsrc_menu_bg_name;	// Color of menu & dialog background
    guicolor_T	menu_bg_pixel;		// Same in Pixel format
    حرف	*rsrc_scroll_fg_name;	// Color of scrollbar foreground
    guicolor_T	scroll_fg_pixel;	// Same in Pixel format
    حرف	*rsrc_scroll_bg_name;	// Color of scrollbar background
    guicolor_T	scroll_bg_pixel;	// Same in Pixel format

    Display	*dpy;		    // X display
    کړکۍ	wid;		    // Window id of text area
    صحيح		visibility;	    // Is shell partially/fully obscured?
    GC		متن_gc;
    GC		شا_gc;
    GC		invert_gc;
    Cursor	blank_اشاره_ګر;	    // Blank pointer

    // X Resources
    char_u	*rsrc_font_name;    // Resource font name, used if 'guifont'
				    // not set
    char_u	*rsrc_bold_font_name; // Resource bold font name
    char_u	*rsrc_ital_font_name; // Resource italic font name
    char_u	*rsrc_boldital_font_name;  // Resource bold-italic font name
    char_u	*rsrc_menu_font_name;    // Resource menu Font name
    Bool	rsrc_rev_video;	    // Use reverse video?

    char_u	*geom;		    // Geometry, eg "80x24"
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GTK
# إذا !defined(USE_GTK3) && !defined(USE_GTK4)
    صحيح		visibility;	    // Is shell partially/fully obscured?
# endif
    GdkCursor	*blank_اشاره_ګر;	    // Blank pointer

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
    Gtk_Accel_ډله *accel_ډله;
# endif
    GtkWidget	*filedlg;	    // file selection dialog
    char_u	*browse_fname;	    // file name from filedlg

    guint32	event_وخت;
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
#د_که_پای	// FEAT_GUI_GTK

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    char_u ligatures_map[256];	    // ascii map for characters 0-255, value is
				    // 1 if in 'guiligatures'
#د_که_پای

#إذا defined(FEAT_GUI_TABLINE) \
	&& (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
		|| defined(FEAT_GUI_HAIKU))
    صحيح		tabline_لوړوالی;
#د_که_پای

#إذا defined(FEAT_TOOLBAR) \
	&& (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    صحيح		toolbar_لوړوالی;	    // height of the toolbar
#د_که_پای

#که_تعریف_شوی FEAT_BEVAL_TIP
    // Tooltip properties; also used for balloon evaluation
    char_u	*rsrc_tooltip_font_name; // tooltip font name
    حرف	*rsrc_tooltip_fg_name;	// tooltip foreground color name
    حرف	*rsrc_tooltip_bg_name;	// tooltip background color name
    guicolor_T	tooltip_fg_pixel;	// tooltip foreground color
    guicolor_T	tooltip_bg_pixel;	// tooltip background color
    XFont_سېټ	tooltip_fontset;	// tooltip fontset
#د_که_پای

#که_تعریف_شوی FEAT_GUI_MSWIN
    GuiFont	currFont;	    // Current font
    guicolor_T	currFgColor;	    // Current foreground text color
    guicolor_T	currBgColor;	    // Current background text color
    guicolor_T	currSpColor;	    // Current special text color

    guicolor_T	title_bg_pixel;	    // window title bar color
    guicolor_T	title_fg_pixel;	    // window title bar's text color
    guicolor_T	titlenc_bg_pixel;   // window title bar color not current
    guicolor_T	titlenc_fg_pixel;   // window title bar's text color not current
#د_که_پای

#که_تعریف_شوی FEAT_GUI_HAIKU
    VimApp     *vimApp;
    Vim_کړکۍ  *vim_کړکۍ;
    VimFormView *vimForm;
    Vim_متن_Area_View *vim_متن_Area;
    صحيح	vdcmp;			    // Vim Direct Communication Message Port
#د_که_پای

#که_تعریف_شوی FEAT_GUI_PHOTON
    PtWidget_t	*vim_کړکۍ;		// PtWindow
    PtWidget_t	*vim_متن_Area;		// PtRaw
    PtWidget_t	*vimContainer;		// PtPanel
# إذا defined(FEAT_MENU) || defined(FEAT_TOOLBAR)
    PtWidget_t	*vim_Tool_Bar_ډله;
# endif
# ifdef FEAT_MENU
    PtWidget_t	*vimMenuBar;
# endif
# ifdef FEAT_TOOLBAR
    PtWidget_t	*vimToolBar;
    صحيح		toolbar_لوړوالی;
# endif
    PhEvent_t	*event_بفر;
#د_که_پای

#که_تعریف_شوی FEAT_XIM
    حرف	*rsrc_ننوت_لاره;
    حرف	*rsrc_preedit_type_name;
#د_که_پای
#إذا defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    صحيح decor_لوړوالی;

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
    Gtk_Drop_موخه_Async *drop_موخه;
# endif
#د_که_پای
} gui_T;

خارجي gui_T gui;			// this is defined in gui.c

// definitions of available window positionings for gui_*_position_in_parent()
تعريفنوع تعداد
{
    VW_POS_MOUSE,
    VW_POS_CENTER,
    VW_POS_سر_CENTER
} gui_win_pos_T;

#که_تعریف_شوی پیدا_کړه_REPLACE_DIALOG
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
# define FRD_ټولیز_کلمه	0x08	// match whole word only
# define FRD_سمون_CASE	0x10	// match case
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GTK
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
# define CONVERT_منزل_UTF8(String)				\
    ((output_conv.vc_type == CONV_NONE || (String) == عدم)	\
	    ? (String)						\
	    : string_convert(&output_conv, (String), عدم))

# define CONVERT_منزل_UTF8_FREE(String)				\
    ((String) = ((output_conv.vc_type == CONV_NONE)		\
			? (char_u *)عدم			\
			: (vim_free(String), (char_u *)عدم)))

# define CONVERT_FROM_UTF8(String)				\
    ((ننوت_conv.vc_type == CONV_NONE || (String) == عدم)	\
	    ? (String)						\
	    : string_convert(&ننوت_conv, (String), عدم))

# define CONVERT_FROM_UTF8_FREE(String)				\
    ((String) = ((ننوت_conv.vc_type == CONV_NONE)		\
			? (char_u *)عدم			\
			: (vim_free(String), (char_u *)عدم)))

#وإلا
# define CONVERT_منزل_UTF8(String) (String)
# define CONVERT_منزل_UTF8_FREE(String) ((String) = (char_u *)عدم)
# define CONVERT_FROM_UTF8(String) (String)
# define CONVERT_FROM_UTF8_FREE(String) ((String) = (char_u *)عدم)
#د_که_پای // FEAT_GUI_GTK

#که_تعریف_شوی FEAT_GUI_GTK
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
#د_که_پای // FEAT_GUI_GTK

#إذا defined(UNIX)
# define GUI_MAY_FORK
#د_که_پای
