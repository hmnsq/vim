/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ifdef FEAT_GUI_MOTIF
# include <Xm/Xm.h>
#endif

#ifdef FEAT_GUI_GTK
# ifdef VMS
#  include "gui_gtk_vms.h"
# endif
# ifndef USE_GTK4
#  include <X11/Intrinsic.h>
# endif
# pragma GCC diagnostic ढकला
# pragma GCC diagnostic ignored "-Wstrict-prototypes"
# जर !defined(USE_GTK3) && !defined(USE_GTK4)
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
# include <gtk/gtk.h>
# pragma GCC diagnostic काढा
# ifdef USE_GTK4
// Types used in proto files but not available without X11 headers
प्रकारव्याख्या शून्य *Widget;
प्रकारव्याख्या शून्य *XtAppContext;
प्रकारव्याख्या शून्य  Display;
प्रकारव्याख्या अचिन्हित दीर्घ Window;
प्रकारव्याख्या अचिन्हित दीर्घ Atom;
प्रकारव्याख्या GdkEvent Gdk_Event_किल्ली;	// GTK4: GdkEventKey merged into GdkEvent
# endif
#endif

#ifdef FEAT_GUI_HAIKU
# include "gui_haiku.h"
#endif

// Needed when generating prototypes, since FEAT_GUI is always defined then.
#जर defined(FEAT_XCLIPBOARD) && !defined(FEAT_GUI_MOTIF) \
	&& !defined(FEAT_GUI_GTK)
# include <X11/Intrinsic.h>
#endif

#ifdef FEAT_GUI_PHOTON
# include <Ph.h>
# include <Pt.h>
# include "photon/PxProto.h"
#endif

/*
 * On some systems scrolling needs to be done right away instead of in the
 * main loop.
 */
#जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
# define USE_ON_FLY_SCROLL
#endif

/*
 * GUIs that support dropping files on a running Vim.
 */
#जर (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
# define HAVE_DROP_FILE
#endif

/*
 * This define makes menus always use a fontset.
 * We're not sure if this code always works, thus it can be disabled.
 */
#ifdef FEAT_XFONTSET
# define FONTSET_ALWAYS
#endif

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
#ifdef FEAT_GUI_MSWIN
# define TEXT_X(col)	((col) * gui.char_width)
# define TEXT_Y(row)	((row) * gui.char_height + gui.char_ascent)
# define FILL_X(col)	((col) * gui.char_width)
# define FILL_Y(row)	((row) * gui.char_height)
# define X_2_COL(x)	((x) / gui.char_width)
# define Y_2_ROW(y)	((y) / gui.char_height)
#अन्यथा
# define TEXT_X(col)	((col) * gui.char_width  + gui.border_offset)
# define FILL_X(col)	((col) * gui.char_width  + gui.border_offset)
# define X_2_COL(x)	(((x) - gui.border_offset) / gui.char_width)
# define TEXT_Y(row)	((row) * gui.char_height + gui.char_ascent \
							+ gui.border_offset)
# define FILL_Y(row)	((row) * gui.char_height + gui.border_offset)
# define Y_2_ROW(y)	(((y) - gui.border_offset) / gui.char_height)
#endif

// Indices for arrays of scrollbars
#define SBAR_NONE	    (-1)
#define SBAR_डावा	    0
#define SBAR_उजवा	    1
#define SBAR_BOTTOM	    2

// Orientations for scrollbars
#define SBAR_VERT	    0
#define SBAR_HORIZ	    1

// Default size of scrollbar
#define SB_DEFAULT_WIDTH    16

// Default height of the menu bar
#define MENU_DEFAULT_HEIGHT 1		// figure it out at runtime

// Flags for gui_mch_outstr_nowrap()
#define GUI_MON_WRAP_CURSOR	0x01	// wrap cursor at end of line
#define GUI_MON_INVERT		0x02	// invert the characters
#define GUI_MON_IS_CURSOR	0x04	// drawing cursor
#define GUI_MON_TRS_CURSOR	0x08	// drawing transparent cursor
#define GUI_MON_NOCLEAR		0x10	// don't clear selection

// Flags for gui_mch_draw_string()
#define DRAW_TRANSP		0x01	// draw with transparent bg
#define DRAW_BOLD		0x02	// draw bold text
#define DRAW_UNDERL		0x04	// draw underline text
#define DRAW_UNDERC		0x08	// draw undercurl text
#जर defined(FEAT_GUI_GTK)
# define DRAW_ITALIC		0x10	// draw italic text
#endif
#define DRAW_CURSOR		0x20	// drawing block cursor (win32)
#define DRAW_STRIKE		0x40	// strikethrough

// For our own tearoff menu item
#define TEAR_STRING		"-->Detach"
#define TEAR_LEN		(9)	// length of above string

// for the toolbar
#define TOOLBAR_BUTTON_HEIGHT	18
#define TOOLBAR_BUTTON_WIDTH	18
#define TOOLBAR_BORDER_HEIGHT	12  // room above+below buttons for MSWindows

#ifdef FEAT_GUI_MSWIN
# define TABLINE_HEIGHT 22
#endif
#ifdef FEAT_GUI_MOTIF
# define TABLINE_HEIGHT 30
#endif

#जर defined(NO_CONSOLE) || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
# define NO_CONSOLE_आदान	// use no_console_input() to check if there
				// is no console input possible
#endif

प्रकारव्याख्या संरचना GuiScrollbar
{
    दीर्घ	ident;		// Unique identifier for each scrollbar
    win_T	*wp;		// Scrollbar's window, NULL for bottom
    पूर्णांक		type;		// one of SBAR_{LEFT,RIGHT,BOTTOM}
    दीर्घ	मूल्य;		// Represents top line number visible
    दीर्घ	size;		// Size of scrollbar thumb
    दीर्घ	max;		// Number of lines in buffer

    // Values measured in characters:
    पूर्णांक		शीर्ष;		// Top of scroll bar (chars from row 0)
    पूर्णांक		height;		// Current height of scroll bar in rows
    पूर्णांक		width;		// Current width of scroll bar in cols
    पूर्णांक		status_height;	// Height of status line
#ifdef FEAT_GUI_X11
    Widget	id;		// Id of real scroll bar
#endif
#ifdef FEAT_GUI_GTK
    GtkWidget *id;		// Id of real scroll bar
    अचिन्हित दीर्घ handler_id;   // Id of "value_changed" signal handler
#endif

#ifdef FEAT_GUI_MSWIN
    HWND	id;		// Id of real scroll bar
    पूर्णांक		scroll_shift;	// The scrollbar stuff can handle only up to
				// 32767 lines.  When the file is longer,
				// scroll_shift is set to the number of shifts
				// to reduce the count.
#endif

#ifdef FEAT_GUI_HAIKU
    VimScrollBar *id;		// Pointer to real scroll bar
#endif
#ifdef FEAT_GUI_PHOTON
    PtWidget_t	*id;
#endif
} scrollbar_T;

प्रकारव्याख्या दीर्घ	    guicolor_T;	// handle for a GUI color; for X11 this should
				// be "Pixel", but that's an unsigned and we
				// need a signed value
#define INVALCOLOR ((guicolor_T)-11111)	// number for invalid color; on 32 bit
				   // displays there is a tiny chance this is an
				   // actual color
#define CTERMCOLOR ((guicolor_T)-11110)	// only used for cterm.bg_rgb and
					// cterm.fg_rgb: use cterm color

#ifdef FEAT_GUI_GTK
  प्रकारव्याख्या PangoFontDescription	*GuiFont;       // handle for a GUI font
  प्रकारव्याख्या PangoFontDescription  *GuiFontset;    // handle for a GUI fontset
# define NOFONT		(GuiFont)रिक्त
# define NOFONTSET	(GuiFontset)रिक्त
#अन्यथा
# ifdef FEAT_GUI_PHOTON
  प्रकारव्याख्या अक्षर		*GuiFont;
  प्रकारव्याख्या अक्षर		*GuiFontset;
#  define NOFONT	(GuiFont)रिक्त
#  define NOFONTSET	(GuiFontset)रिक्त
# अन्यथा
#  ifdef FEAT_GUI_X11
  प्रकारव्याख्या XFontStruct	*GuiFont;	// handle for a GUI font
  प्रकारव्याख्या XFontSet	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  अन्यथा
  प्रकारव्याख्या long_u	GuiFont;	// handle for a GUI font
  प्रकारव्याख्या long_u	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  endif
# endif
#endif

#ifdef VIMDLL
// Use spawn when GUI is starting.
# define GUI_MAY_SPAWN

// Uncomment the next definition if you want to use the `:gui` command on
// Windows.  It uses `:mksession` to inherit the session from vim.exe to
// gvim.exe.  So, it doesn't work perfectly. (EXPERIMENTAL)
//# define EXPERIMENTAL_GUI_CMD
#endif

प्रकारव्याख्या संरचना Gui
{
    bool	in_focus;	    // Vim has input focus
    bool	in_use;		    // Is the GUI being used?
    bool	starting;	    // GUI will start in a little while
    bool	shell_created;	    // Has the shell been created yet?
    bool	dying;		    // Is vim dying? Then output to terminal
    bool	dofork;		    // Use fork() when GUI is starting
#ifdef GUI_MAY_SPAWN
    bool	dospawn;	    // Use spawn() when GUI is starting
#endif
    पूर्णांक		dragged_sb;	    // Which scrollbar being dragged, if any?
    win_T	*dragged_wp;	    // Which WIN's sb being dragged, if any?
    bool	pointer_hidden;	    // Is the mouse pointer hidden?
    पूर्णांक		col;		    // Current cursor column in GUI display
    पूर्णांक		row;		    // Current cursor row in GUI display
    पूर्णांक		cursor_col;	    // Physical cursor column in GUI display
    पूर्णांक		cursor_row;	    // Physical cursor row in GUI display
    bool	cursor_is_valid;    // There is a cursor at cursor_row/col
    पूर्णांक		अंक_cols;	    // Number of columns
    पूर्णांक		अंक_rows;	    // Number of rows
    पूर्णांक		scroll_region_शीर्ष;  // Top (first) line of scroll region
    पूर्णांक		scroll_region_bot;  // Bottom (last) line of scroll region
    पूर्णांक		scroll_region_डावा;  // Left (first) column of scroll region
    पूर्णांक		scroll_region_उजवा;  // Right (last) col. of scroll region
    पूर्णांक		highlight_mask;	    // Highlight attribute mask
    पूर्णांक		scrollbar_width;    // Width of vertical scrollbars
    पूर्णांक		scrollbar_height;   // Height of horizontal scrollbar
    पूर्णांक		डावा_sbar_x;	    // Calculated x coord for left scrollbar
    पूर्णांक		उजवा_sbar_x;	    // Calculated x coord for right scrollbar
    bool	force_redraw;       // Force a redraw even e.g. not resized
#ifdef FEAT_DIRECTX
    bool	directx_enabled;    // DirectX (DirectWrite) rendering active
#endif
#जर defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    पूर्णांक		bleed_उजवा;	    // Number of pixels to bleed bg color right
    पूर्णांक		bleed_bot;	    // Number of pixels to bleed bg color down
#endif

#ifdef FEAT_MENU
# ifndef FEAT_GUI_GTK
    पूर्णांक		menu_height;	    // Height of the menu bar
    पूर्णांक		menu_width;	    // Width of the menu bar
# endif
    bool	menu_is_active;	    // true if menu is present
#endif

    scrollbar_T bottom_sbar;	    // Bottom scrollbar
    bool	which_scrollbars[3];// Which scrollbar boxes are active?
    पूर्णांक		prev_wrap;	    // For updating the horizontal scrollbar
    पूर्णांक		char_width;	    // Width of char cell in pixels
    पूर्णांक		char_height;	    // Height of char cell in pixels, includes
				    // 'linespace'
    पूर्णांक		char_ascent;	    // Ascent of char in pixels
    पूर्णांक		border_width;	    // Width of our border around text area
    पूर्णांक		border_offset;	    // Total pixel offset for all borders

    GuiFont	norm_font;	    // Normal font
#ifndef FEAT_GUI_GTK
    GuiFont	bold_font;	    // Bold font
    GuiFont	ital_font;	    // Italic font
    GuiFont	boldital_font;	    // Bold-Italic font
#अन्यथा
    bool	font_can_bold;	    // Whether norm_font supports bold weight.
				    // The styled font variants are not used.
#endif

#जर defined(FEAT_MENU) && !defined(FEAT_GUI_GTK)
# ifdef FONTSET_ALWAYS
    GuiFontset	menu_fontset;	    // set of fonts for multi-byte chars
# अन्यथा
    GuiFont	menu_font;	    // menu item font
# endif
#endif
    GuiFont	wide_font;	    // Normal 'guifontwide' font
#ifndef FEAT_GUI_GTK
    GuiFont	wide_bold_font;	    // Bold 'guifontwide' font
    GuiFont	wide_ital_font;	    // Italic 'guifontwide' font
    GuiFont	wide_boldital_font; // Bold-Italic 'guifontwide' font
#endif
#ifdef FEAT_XFONTSET
    GuiFontset	fontset;	    // set of fonts for multi-byte chars
#endif
    guicolor_T	back_pixel;	    // Color of background
    guicolor_T	norm_pixel;	    // Color of normal text
    guicolor_T	def_back_pixel;	    // default Color of background
    guicolor_T	def_norm_pixel;	    // default Color of normal text

#ifdef FEAT_GUI_X11
    अक्षर	*rsrc_menu_fg_name;	// Color of menu & dialog foreground
    guicolor_T	menu_fg_pixel;		// Same in Pixel format
    अक्षर	*rsrc_menu_bg_name;	// Color of menu & dialog background
    guicolor_T	menu_bg_pixel;		// Same in Pixel format
    अक्षर	*rsrc_scroll_fg_name;	// Color of scrollbar foreground
    guicolor_T	scroll_fg_pixel;	// Same in Pixel format
    अक्षर	*rsrc_scroll_bg_name;	// Color of scrollbar background
    guicolor_T	scroll_bg_pixel;	// Same in Pixel format

    Display	*dpy;		    // X display
    Window	wid;		    // Window id of text area
    पूर्णांक		visibility;	    // Is shell partially/fully obscured?
    GC		text_gc;
    GC		back_gc;
    GC		invert_gc;
    Cursor	blank_pointer;	    // Blank pointer

    // X Resources
    char_u	*rsrc_font_name;    // Resource font name, used if 'guifont'
				    // not set
    char_u	*rsrc_bold_font_name; // Resource bold font name
    char_u	*rsrc_ital_font_name; // Resource italic font name
    char_u	*rsrc_boldital_font_name;  // Resource bold-italic font name
    char_u	*rsrc_menu_font_name;    // Resource menu Font name
    Bool	rsrc_rev_video;	    // Use reverse video?

    char_u	*geom;		    // Geometry, eg "80x24"
#endif

#ifdef FEAT_GUI_GTK
# जर !defined(USE_GTK3) && !defined(USE_GTK4)
    पूर्णांक		visibility;	    // Is shell partially/fully obscured?
# endif
    GdkCursor	*blank_pointer;	    // Blank pointer

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
# जर defined(USE_GTK3) || defined(USE_GTK4)
    GdkRGBA	*fgcolor;	    // GDK-styled foreground color
    GdkRGBA	*bgcolor;	    // GDK-styled background color
    GdkRGBA	*spcolor;	    // GDK-styled special color
# अन्यथा
    GdkColor	*fgcolor;	    // GDK-styled foreground color
    GdkColor	*bgcolor;	    // GDK-styled background color
    GdkColor	*spcolor;	    // GDK-styled special color
# endif
# जर defined(USE_GTK3) && !defined(USE_GTK4)
    cairo_surface_t *surface;       // drawarea surface
# elif !defined(USE_GTK4)
    GdkGC	*text_gc;	    // cached GC for normal text
# endif
    PangoContext     *text_context; // the context used for all text
    PangoFont	     *ascii_font;   // cached font for ASCII strings
    PangoGlyphString *ascii_glyphs; // cached code point -> glyph map
# ifdef FEAT_GUI_TABLINE
    GtkWidget	*tabline;	    // tab pages line handle
# endif

# ifndef USE_GTK4
    GtkAccelGroup *accel_group;
# endif
    GtkWidget	*filedlg;	    // file selection dialog
    char_u	*browse_fname;	    // file name from filedlg

    guint32	event_time;
# ifdef FEAT_GUI_DIALOG
    // Multiple dialogs not allowed, just tracked for future use.
    पूर्णांक		dialogs_active;     // number of active GUI dialogs
    // Suppress focus-in event when focus returns.
    पूर्णांक		dialog_focus_pending;
# endif
    bool	is_x11;	            // active gdk backend in gtk is x11
# ifdef GDK_WINDOWING_WAYLAND
    bool	is_wayland;	    // active gdk backend in gtk is wayland
# endif
# जर defined(FEAT_IMAGE) && defined(FEAT_GUI_GTK)
    द्विगुण	scale;		    // Current scaling (may be fractional)
# endif
#endif	// FEAT_GUI_GTK

#जर defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    char_u ligatures_map[256];	    // ascii map for characters 0-255, value is
				    // 1 if in 'guiligatures'
#endif

#जर defined(FEAT_GUI_TABLINE) \
	&& (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
		|| defined(FEAT_GUI_HAIKU))
    पूर्णांक		tabline_height;
#endif

#जर defined(FEAT_TOOLBAR) \
	&& (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    पूर्णांक		toolbar_height;	    // height of the toolbar
#endif

#ifdef FEAT_BEVAL_TIP
    // Tooltip properties; also used for balloon evaluation
    char_u	*rsrc_tooltip_font_name; // tooltip font name
    अक्षर	*rsrc_tooltip_fg_name;	// tooltip foreground color name
    अक्षर	*rsrc_tooltip_bg_name;	// tooltip background color name
    guicolor_T	tooltip_fg_pixel;	// tooltip foreground color
    guicolor_T	tooltip_bg_pixel;	// tooltip background color
    XFontSet	tooltip_fontset;	// tooltip fontset
#endif

#ifdef FEAT_GUI_MSWIN
    GuiFont	currFont;	    // Current font
    guicolor_T	currFgColor;	    // Current foreground text color
    guicolor_T	currBgColor;	    // Current background text color
    guicolor_T	currSpColor;	    // Current special text color

    guicolor_T	title_bg_pixel;	    // window title bar color
    guicolor_T	title_fg_pixel;	    // window title bar's text color
    guicolor_T	titlenc_bg_pixel;   // window title bar color not current
    guicolor_T	titlenc_fg_pixel;   // window title bar's text color not current
#endif

#ifdef FEAT_GUI_HAIKU
    VimApp     *vimApp;
    VimWindow  *vimWindow;
    VimFormView *vimForm;
    VimTextAreaView *vimTextArea;
    पूर्णांक	vdcmp;			    // Vim Direct Communication Message Port
#endif

#ifdef FEAT_GUI_PHOTON
    PtWidget_t	*vimWindow;		// PtWindow
    PtWidget_t	*vimTextArea;		// PtRaw
    PtWidget_t	*vimContainer;		// PtPanel
# जर defined(FEAT_MENU) || defined(FEAT_TOOLBAR)
    PtWidget_t	*vimToolBarGroup;
# endif
# ifdef FEAT_MENU
    PtWidget_t	*vimMenuBar;
# endif
# ifdef FEAT_TOOLBAR
    PtWidget_t	*vimToolBar;
    पूर्णांक		toolbar_height;
# endif
    PhEvent_t	*event_buffer;
#endif

#ifdef FEAT_XIM
    अक्षर	*rsrc_आदान_method;
    अक्षर	*rsrc_preedit_type_name;
#endif
#जर defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    पूर्णांक decor_height;

    // Size of the form widget last asked for with gui_mch_set_shellsize().
    // "pending_form_skip" counts how many allocations that do not answer it
    // may still be ignored.
    पूर्णांक pending_form_w;
    पूर्णांक pending_form_h;
    पूर्णांक pending_form_skip;

    // Used for clipboard functionality in GTK4 GUI
    GdkContentProvider *regular_provider;
    GdkContentProvider *primary_provider;

# ifdef FEAT_DND
    GtkDropTargetAsync *drop_target;
# endif
#endif
} gui_T;

बाह्य gui_T gui;			// this is defined in gui.c

// definitions of available window positionings for gui_*_position_in_parent()
प्रकारव्याख्या गणना
{
    VW_POS_MOUSE,
    VW_POS_CENTER,
    VW_POS_शीर्ष_CENTER
} gui_win_pos_T;

#ifdef FIND_REPLACE_DIALOG
/*
 * Flags used to distinguish the different contexts in which the
 * find/replace callback may be called.
 */
# define FRD_FINDNEXT	1	// Find next in find dialog
# define FRD_R_FINDNEXT	2	// Find next in repl dialog
# define FRD_REPLACE	3	// Replace once
# define FRD_REPLACEALL	4	// Replace remaining matches
# define FRD_UNDO	5	// Undo replaced text
# define FRD_TYPE_MASK   7	// Mask for the callback type
// Flags which change the way searching is done.
# define FRD_WHOLE_WORD	0x08	// match whole word only
# define FRD_MATCH_CASE	0x10	// match case
#endif

#ifdef FEAT_GUI_GTK
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
# define CONVERT_TO_UTF8(String)				\
    ((output_conv.vc_type == CONV_NONE || (String) == रिक्त)	\
	    ? (String)						\
	    : string_convert(&output_conv, (String), रिक्त))

# define CONVERT_TO_UTF8_FREE(String)				\
    ((String) = ((output_conv.vc_type == CONV_NONE)		\
			? (char_u *)रिक्त			\
			: (vim_free(String), (char_u *)रिक्त)))

# define CONVERT_FROM_UTF8(String)				\
    ((आदान_conv.vc_type == CONV_NONE || (String) == रिक्त)	\
	    ? (String)						\
	    : string_convert(&आदान_conv, (String), रिक्त))

# define CONVERT_FROM_UTF8_FREE(String)				\
    ((String) = ((आदान_conv.vc_type == CONV_NONE)		\
			? (char_u *)रिक्त			\
			: (vim_free(String), (char_u *)रिक्त)))

#अन्यथा
# define CONVERT_TO_UTF8(String) (String)
# define CONVERT_TO_UTF8_FREE(String) ((String) = (char_u *)रिक्त)
# define CONVERT_FROM_UTF8(String) (String)
# define CONVERT_FROM_UTF8_FREE(String) ((String) = (char_u *)रिक्त)
#endif // FEAT_GUI_GTK

#ifdef FEAT_GUI_GTK
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
# जर defined(__SUNPRO_C) && (defined(USE_GTK3) || defined(USE_GTK4))
#  define FUNC2GENERIC(func) (शून्य *)(func)
# अन्यथा
#  define FUNC2GENERIC(func) G_CALLBACK(func)
# endif
#endif // FEAT_GUI_GTK

#जर defined(UNIX)
# define GUI_MAY_FORK
#endif
