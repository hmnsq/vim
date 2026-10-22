/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#నిర్వచితమైతే FEAT_GUI_MOTIF
# include <Xm/Xm.h>
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GTK
# ifdef VMS
#  include "gui_gtk_vms.h"
# endif
# ifndef USE_GTK4
#  include <X11/Intrinsic.h>
# endif
# pragma GCC diagnostic నెట్టు
# pragma GCC diagnostic ignored "-Wstrict-prototypes"
# ఒకవేళ !defined(USE_GTK3) && !defined(USE_GTK4)
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
# include <gtk/gtk.h>
# pragma GCC diagnostic తీయి
# ifdef USE_GTK4
// Types used in proto files but not available without X11 headers
రకనిర్వచనం శూన్య *Widget;
రకనిర్వచనం శూన్య *XtAppContext;
రకనిర్వచనం శూన్య  Display;
రకనిర్వచనం గుర్తులేని దీర్ఘ కిటికీ;
రకనిర్వచనం గుర్తులేని దీర్ఘ Atom;
రకనిర్వచనం GdkEvent Gdk_Event_కీలకం;	// GTK4: GdkEventKey merged into GdkEvent
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_HAIKU
# include "gui_haiku.h"
#అయితే_ముగింపు

// Needed when generating prototypes, since FEAT_GUI is always defined then.
#ఒకవేళ defined(FEAT_XCLIPBOARD) && !defined(FEAT_GUI_MOTIF) \
	&& !defined(FEAT_GUI_GTK)
# include <X11/Intrinsic.h>
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_PHOTON
# include <Ph.h>
# include <Pt.h>
# include "photon/PxProto.h"
#అయితే_ముగింపు

/*
 * On some systems scrolling needs to be done right away instead of in the
 * main loop.
 */
#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
# define USE_ON_FLY_SCROLL
#అయితే_ముగింపు

/*
 * GUIs that support dropping files on a running Vim.
 */
#ఒకవేళ (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
# define HAVE_DROP_FILE
#అయితే_ముగింపు

/*
 * This define makes menus always use a fontset.
 * We're not sure if this code always works, thus it can be disabled.
 */
#నిర్వచితమైతే FEAT_XFONTSET
# define FONTSET_ALWAYS
#అయితే_ముగింపు

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
#నిర్వచితమైతే FEAT_GUI_MSWIN
# define పాఠ్యం_X(నిలువు)	((నిలువు) * gui.char_వెడల్పు)
# define పాఠ్యం_Y(అడ్డువరుస)	((అడ్డువరుస) * gui.char_ఎత్తు + gui.char_ascent)
# define FILL_X(నిలువు)	((నిలువు) * gui.char_వెడల్పు)
# define FILL_Y(అడ్డువరుస)	((అడ్డువరుస) * gui.char_ఎత్తు)
# define X_2_నిలువు(x)	((x) / gui.char_వెడల్పు)
# define Y_2_అడ్డువరుస(y)	((y) / gui.char_ఎత్తు)
#లేకపోతే
# define పాఠ్యం_X(నిలువు)	((నిలువు) * gui.char_వెడల్పు  + gui.border_స్థానభ్రంశం)
# define FILL_X(నిలువు)	((నిలువు) * gui.char_వెడల్పు  + gui.border_స్థానభ్రంశం)
# define X_2_నిలువు(x)	(((x) - gui.border_స్థానభ్రంశం) / gui.char_వెడల్పు)
# define పాఠ్యం_Y(అడ్డువరుస)	((అడ్డువరుస) * gui.char_ఎత్తు + gui.char_ascent \
							+ gui.border_స్థానభ్రంశం)
# define FILL_Y(అడ్డువరుస)	((అడ్డువరుస) * gui.char_ఎత్తు + gui.border_స్థానభ్రంశం)
# define Y_2_అడ్డువరుస(y)	(((y) - gui.border_స్థానభ్రంశం) / gui.char_ఎత్తు)
#అయితే_ముగింపు

// Indices for arrays of scrollbars
#నిర్వచించు SBAR_NONE	    (-1)
#నిర్వచించు SBAR_ఎడమ	    0
#నిర్వచించు SBAR_కుడి	    1
#నిర్వచించు SBAR_BOTTOM	    2

// Orientations for scrollbars
#నిర్వచించు SBAR_VERT	    0
#నిర్వచించు SBAR_HORIZ	    1

// Default size of scrollbar
#నిర్వచించు SB_DEFAULT_వెడల్పు    16

// Default height of the menu bar
#నిర్వచించు MENU_DEFAULT_ఎత్తు 1		// figure it out at runtime

// Flags for gui_mch_outstr_nowrap()
#నిర్వచించు GUI_MON_WRAP_CURSOR	0x01	// wrap cursor at end of line
#నిర్వచించు GUI_MON_INVERT		0x02	// invert the characters
#నిర్వచించు GUI_MON_IS_CURSOR	0x04	// drawing cursor
#నిర్వచించు GUI_MON_TRS_CURSOR	0x08	// drawing transparent cursor
#నిర్వచించు GUI_MON_NOCLEAR		0x10	// don't clear selection

// Flags for gui_mch_draw_string()
#నిర్వచించు DRAW_TRANSP		0x01	// draw with transparent bg
#నిర్వచించు DRAW_BOLD		0x02	// draw bold text
#నిర్వచించు DRAW_UNDERL		0x04	// draw underline text
#నిర్వచించు DRAW_UNDERC		0x08	// draw undercurl text
#ఒకవేళ defined(FEAT_GUI_GTK)
# define DRAW_ITALIC		0x10	// draw italic text
#అయితే_ముగింపు
#నిర్వచించు DRAW_CURSOR		0x20	// drawing block cursor (win32)
#నిర్వచించు DRAW_STRIKE		0x40	// strikethrough

// For our own tearoff menu item
#నిర్వచించు TEAR_STRING		"-->Detach"
#నిర్వచించు TEAR_LEN		(9)	// length of above string

// for the toolbar
#నిర్వచించు TOOLBAR_BUTTON_ఎత్తు	18
#నిర్వచించు TOOLBAR_BUTTON_వెడల్పు	18
#నిర్వచించు TOOLBAR_BORDER_ఎత్తు	12  // room above+below buttons for MSWindows

#నిర్వచితమైతే FEAT_GUI_MSWIN
# define TABLINE_ఎత్తు 22
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MOTIF
# define TABLINE_ఎత్తు 30
#అయితే_ముగింపు

#ఒకవేళ defined(NO_CONSOLE) || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
# define NO_CONSOLE_ఆదానం	// use no_console_input() to check if there
				// is no console input possible
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం GuiScrollbar
{
    దీర్ఘ	ident;		// Unique identifier for each scrollbar
    win_T	*wp;		// Scrollbar's window, NULL for bottom
    పూర్ణాంకం		type;		// one of SBAR_{LEFT,RIGHT,BOTTOM}
    దీర్ఘ	విలువ;		// Represents top line number visible
    దీర్ఘ	కొలత;		// Size of scrollbar thumb
    దీర్ఘ	max;		// Number of lines in buffer

    // Values measured in characters:
    పూర్ణాంకం		శిఖరం;		// Top of scroll bar (chars from row 0)
    పూర్ణాంకం		ఎత్తు;		// Current height of scroll bar in rows
    పూర్ణాంకం		వెడల్పు;		// Current width of scroll bar in cols
    పూర్ణాంకం		స్థితి_ఎత్తు;	// Height of status line
#నిర్వచితమైతే FEAT_GUI_X11
    Widget	id;		// Id of real scroll bar
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_GTK
    GtkWidget *id;		// Id of real scroll bar
    గుర్తులేని దీర్ఘ handler_id;   // Id of "value_changed" signal handler
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_MSWIN
    HWND	id;		// Id of real scroll bar
    పూర్ణాంకం		scroll_జరుపు;	// The scrollbar stuff can handle only up to
				// 32767 lines.  When the file is longer,
				// scroll_shift is set to the number of shifts
				// to reduce the count.
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_HAIKU
    VimScrollBar *id;		// Pointer to real scroll bar
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_PHOTON
    PtWidget_t	*id;
#అయితే_ముగింపు
} scrollbar_T;

రకనిర్వచనం దీర్ఘ	    guicolor_T;	// handle for a GUI color; for X11 this should
				// be "Pixel", but that's an unsigned and we
				// need a signed value
#నిర్వచించు INVALCOLOR ((guicolor_T)-11111)	// number for invalid color; on 32 bit
				   // displays there is a tiny chance this is an
				   // actual color
#నిర్వచించు CTERMCOLOR ((guicolor_T)-11110)	// only used for cterm.bg_rgb and
					// cterm.fg_rgb: use cterm color

#నిర్వచితమైతే FEAT_GUI_GTK
  రకనిర్వచనం PangoFontDescription	*GuiFont;       // handle for a GUI font
  రకనిర్వచనం PangoFontDescription  *GuiFontset;    // handle for a GUI fontset
# define NOFONT		(GuiFont)ఖాళీ
# define NOFONTSET	(GuiFontset)ఖాళీ
#లేకపోతే
# ifdef FEAT_GUI_PHOTON
  రకనిర్వచనం అక్షరం		*GuiFont;
  రకనిర్వచనం అక్షరం		*GuiFontset;
#  define NOFONT	(GuiFont)ఖాళీ
#  define NOFONTSET	(GuiFontset)ఖాళీ
# లేకపోతే
#  ifdef FEAT_GUI_X11
  రకనిర్వచనం XFontStruct	*GuiFont;	// handle for a GUI font
  రకనిర్వచనం XFont_సమితి	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  లేకపోతే
  రకనిర్వచనం long_u	GuiFont;	// handle for a GUI font
  రకనిర్వచనం long_u	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే VIMDLL
// Use spawn when GUI is starting.
# define GUI_MAY_SPAWN

// Uncomment the next definition if you want to use the `:gui` command on
// Windows.  It uses `:mksession` to inherit the session from vim.exe to
// gvim.exe.  So, it doesn't work perfectly. (EXPERIMENTAL)
//# define EXPERIMENTAL_GUI_CMD
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం Gui
{
    bool	లోపల_focus;	    // Vim has input focus
    bool	లోపల_use;		    // Is the GUI being used?
    bool	starting;	    // GUI will start in a little while
    bool	shell_created;	    // Has the shell been created yet?
    bool	dying;		    // Is vim dying? Then output to terminal
    bool	dofork;		    // Use fork() when GUI is starting
#నిర్వచితమైతే GUI_MAY_SPAWN
    bool	dospawn;	    // Use spawn() when GUI is starting
#అయితే_ముగింపు
    పూర్ణాంకం		dragged_sb;	    // Which scrollbar being dragged, if any?
    win_T	*dragged_wp;	    // Which WIN's sb being dragged, if any?
    bool	సూచిక_hidden;	    // Is the mouse pointer hidden?
    పూర్ణాంకం		నిలువు;		    // Current cursor column in GUI display
    పూర్ణాంకం		అడ్డువరుస;		    // Current cursor row in GUI display
    పూర్ణాంకం		cursor_నిలువు;	    // Physical cursor column in GUI display
    పూర్ణాంకం		cursor_అడ్డువరుస;	    // Physical cursor row in GUI display
    bool	cursor_is_valid;    // There is a cursor at cursor_row/col
    పూర్ణాంకం		అంకె_నిలువుల_సంఖ్య;	    // Number of columns
    పూర్ణాంకం		అంకె_అడ్డువరుసలు;	    // Number of rows
    పూర్ణాంకం		scroll_region_శిఖరం;  // Top (first) line of scroll region
    పూర్ణాంకం		scroll_region_bot;  // Bottom (last) line of scroll region
    పూర్ణాంకం		scroll_region_ఎడమ;  // Left (first) column of scroll region
    పూర్ణాంకం		scroll_region_కుడి;  // Right (last) col. of scroll region
    పూర్ణాంకం		highlight_ముసుగు;	    // Highlight attribute mask
    పూర్ణాంకం		scrollbar_వెడల్పు;    // Width of vertical scrollbars
    పూర్ణాంకం		scrollbar_ఎత్తు;   // Height of horizontal scrollbar
    పూర్ణాంకం		ఎడమ_sbar_x;	    // Calculated x coord for left scrollbar
    పూర్ణాంకం		కుడి_sbar_x;	    // Calculated x coord for right scrollbar
    bool	force_redraw;       // Force a redraw even e.g. not resized
#నిర్వచితమైతే FEAT_DIRECTX
    bool	directx_enabled;    // DirectX (DirectWrite) rendering active
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    పూర్ణాంకం		bleed_కుడి;	    // Number of pixels to bleed bg color right
    పూర్ణాంకం		bleed_bot;	    // Number of pixels to bleed bg color down
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
# ifndef FEAT_GUI_GTK
    పూర్ణాంకం		menu_ఎత్తు;	    // Height of the menu bar
    పూర్ణాంకం		menu_వెడల్పు;	    // Width of the menu bar
# endif
    bool	menu_is_చురుకైన;	    // true if menu is present
#అయితే_ముగింపు

    scrollbar_T bottom_sbar;	    // Bottom scrollbar
    bool	which_scrollbars[3];// Which scrollbar boxes are active?
    పూర్ణాంకం		prev_wrap;	    // For updating the horizontal scrollbar
    పూర్ణాంకం		char_వెడల్పు;	    // Width of char cell in pixels
    పూర్ణాంకం		char_ఎత్తు;	    // Height of char cell in pixels, includes
				    // 'linespace'
    పూర్ణాంకం		char_ascent;	    // Ascent of char in pixels
    పూర్ణాంకం		border_వెడల్పు;	    // Width of our border around text area
    పూర్ణాంకం		border_స్థానభ్రంశం;	    // Total pixel offset for all borders

    GuiFont	norm_font;	    // Normal font
#నిర్వచితం_కాకపోతే FEAT_GUI_GTK
    GuiFont	bold_font;	    // Bold font
    GuiFont	ital_font;	    // Italic font
    GuiFont	boldital_font;	    // Bold-Italic font
#లేకపోతే
    bool	font_can_bold;	    // Whether norm_font supports bold weight.
				    // The styled font variants are not used.
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK)
# ifdef FONTSET_ALWAYS
    GuiFontset	menu_fontset;	    // set of fonts for multi-byte chars
# లేకపోతే
    GuiFont	menu_font;	    // menu item font
# endif
#అయితే_ముగింపు
    GuiFont	wide_font;	    // Normal 'guifontwide' font
#నిర్వచితం_కాకపోతే FEAT_GUI_GTK
    GuiFont	wide_bold_font;	    // Bold 'guifontwide' font
    GuiFont	wide_ital_font;	    // Italic 'guifontwide' font
    GuiFont	wide_boldital_font; // Bold-Italic 'guifontwide' font
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XFONTSET
    GuiFontset	fontset;	    // set of fonts for multi-byte chars
#అయితే_ముగింపు
    guicolor_T	వెనుక_pixel;	    // Color of background
    guicolor_T	norm_pixel;	    // Color of normal text
    guicolor_T	def_వెనుక_pixel;	    // default Color of background
    guicolor_T	def_norm_pixel;	    // default Color of normal text

#నిర్వచితమైతే FEAT_GUI_X11
    అక్షరం	*rsrc_menu_fg_name;	// Color of menu & dialog foreground
    guicolor_T	menu_fg_pixel;		// Same in Pixel format
    అక్షరం	*rsrc_menu_bg_name;	// Color of menu & dialog background
    guicolor_T	menu_bg_pixel;		// Same in Pixel format
    అక్షరం	*rsrc_scroll_fg_name;	// Color of scrollbar foreground
    guicolor_T	scroll_fg_pixel;	// Same in Pixel format
    అక్షరం	*rsrc_scroll_bg_name;	// Color of scrollbar background
    guicolor_T	scroll_bg_pixel;	// Same in Pixel format

    Display	*dpy;		    // X display
    కిటికీ	wid;		    // Window id of text area
    పూర్ణాంకం		visibility;	    // Is shell partially/fully obscured?
    GC		పాఠ్యం_gc;
    GC		వెనుక_gc;
    GC		invert_gc;
    Cursor	blank_సూచిక;	    // Blank pointer

    // X Resources
    char_u	*rsrc_font_name;    // Resource font name, used if 'guifont'
				    // not set
    char_u	*rsrc_bold_font_name; // Resource bold font name
    char_u	*rsrc_ital_font_name; // Resource italic font name
    char_u	*rsrc_boldital_font_name;  // Resource bold-italic font name
    char_u	*rsrc_menu_font_name;    // Resource menu Font name
    Bool	rsrc_rev_video;	    // Use reverse video?

    char_u	*geom;		    // Geometry, eg "80x24"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GTK
# ఒకవేళ !defined(USE_GTK3) && !defined(USE_GTK4)
    పూర్ణాంకం		visibility;	    // Is shell partially/fully obscured?
# endif
    GdkCursor	*blank_సూచిక;	    // Blank pointer

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
# ఒకవేళ defined(USE_GTK3) || defined(USE_GTK4)
    GdkRGBA	*fgcolor;	    // GDK-styled foreground color
    GdkRGBA	*bgcolor;	    // GDK-styled background color
    GdkRGBA	*spcolor;	    // GDK-styled special color
# లేకపోతే
    GdkColor	*fgcolor;	    // GDK-styled foreground color
    GdkColor	*bgcolor;	    // GDK-styled background color
    GdkColor	*spcolor;	    // GDK-styled special color
# endif
# ఒకవేళ defined(USE_GTK3) && !defined(USE_GTK4)
    cairo_surface_t *surface;       // drawarea surface
# elif !defined(USE_GTK4)
    GdkGC	*పాఠ్యం_gc;	    // cached GC for normal text
# endif
    PangoContext     *పాఠ్యం_context; // the context used for all text
    PangoFont	     *ascii_font;   // cached font for ASCII strings
    PangoGlyphString *ascii_glyphs; // cached code point -> glyph map
# ifdef FEAT_GUI_TABLINE
    GtkWidget	*tabline;	    // tab pages line handle
# endif

# ifndef USE_GTK4
    Gtk_Accel_సమూహం *accel_సమూహం;
# endif
    GtkWidget	*filedlg;	    // file selection dialog
    char_u	*browse_fname;	    // file name from filedlg

    guint32	event_సమయం;
# ifdef FEAT_GUI_DIALOG
    // Multiple dialogs not allowed, just tracked for future use.
    పూర్ణాంకం		dialogs_చురుకైన;     // number of active GUI dialogs
    // Suppress focus-in event when focus returns.
    పూర్ణాంకం		dialog_focus_pending;
# endif
    bool	is_x11;	            // active gdk backend in gtk is x11
# ifdef GDK_WINDOWING_WAYLAND
    bool	is_wayland;	    // active gdk backend in gtk is wayland
# endif
# ఒకవేళ defined(FEAT_IMAGE) && defined(FEAT_GUI_GTK)
    ద్విగుణ	scale;		    // Current scaling (may be fractional)
# endif
#అయితే_ముగింపు	// FEAT_GUI_GTK

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    char_u ligatures_map[256];	    // ascii map for characters 0-255, value is
				    // 1 if in 'guiligatures'
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE) \
	&& (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
		|| defined(FEAT_GUI_HAIKU))
    పూర్ణాంకం		tabline_ఎత్తు;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR) \
	&& (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    పూర్ణాంకం		toolbar_ఎత్తు;	    // height of the toolbar
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_BEVAL_TIP
    // Tooltip properties; also used for balloon evaluation
    char_u	*rsrc_tooltip_font_name; // tooltip font name
    అక్షరం	*rsrc_tooltip_fg_name;	// tooltip foreground color name
    అక్షరం	*rsrc_tooltip_bg_name;	// tooltip background color name
    guicolor_T	tooltip_fg_pixel;	// tooltip foreground color
    guicolor_T	tooltip_bg_pixel;	// tooltip background color
    XFont_సమితి	tooltip_fontset;	// tooltip fontset
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_MSWIN
    GuiFont	currFont;	    // Current font
    guicolor_T	currFgColor;	    // Current foreground text color
    guicolor_T	currBgColor;	    // Current background text color
    guicolor_T	currSpColor;	    // Current special text color

    guicolor_T	title_bg_pixel;	    // window title bar color
    guicolor_T	title_fg_pixel;	    // window title bar's text color
    guicolor_T	titlenc_bg_pixel;   // window title bar color not current
    guicolor_T	titlenc_fg_pixel;   // window title bar's text color not current
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_HAIKU
    VimApp     *vimApp;
    Vim_కిటికీ  *vim_కిటికీ;
    VimFormView *vimForm;
    Vim_పాఠ్యం_Area_View *vim_పాఠ్యం_Area;
    పూర్ణాంకం	vdcmp;			    // Vim Direct Communication Message Port
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_PHOTON
    PtWidget_t	*vim_కిటికీ;		// PtWindow
    PtWidget_t	*vim_పాఠ్యం_Area;		// PtRaw
    PtWidget_t	*vimContainer;		// PtPanel
# ఒకవేళ defined(FEAT_MENU) || defined(FEAT_TOOLBAR)
    PtWidget_t	*vim_Tool_Bar_సమూహం;
# endif
# ifdef FEAT_MENU
    PtWidget_t	*vimMenuBar;
# endif
# ifdef FEAT_TOOLBAR
    PtWidget_t	*vimToolBar;
    పూర్ణాంకం		toolbar_ఎత్తు;
# endif
    PhEvent_t	*event_బఫరు;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_XIM
    అక్షరం	*rsrc_ఆదానం_పద్ధతి;
    అక్షరం	*rsrc_preedit_type_name;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    పూర్ణాంకం decor_ఎత్తు;

    // Size of the form widget last asked for with gui_mch_set_shellsize().
    // "pending_form_skip" counts how many allocations that do not answer it
    // may still be ignored.
    పూర్ణాంకం pending_form_w;
    పూర్ణాంకం pending_form_h;
    పూర్ణాంకం pending_form_skip;

    // Used for clipboard functionality in GTK4 GUI
    GdkContentProvider *regular_provider;
    GdkContentProvider *primary_provider;

# ifdef FEAT_DND
    Gtk_Drop_లక్ష్యం_Async *drop_లక్ష్యం;
# endif
#అయితే_ముగింపు
} gui_T;

బాహ్య gui_T gui;			// this is defined in gui.c

// definitions of available window positionings for gui_*_position_in_parent()
రకనిర్వచనం గణన
{
    VW_POS_MOUSE,
    VW_POS_CENTER,
    VW_POS_శిఖరం_CENTER
} gui_win_pos_T;

#నిర్వచితమైతే కనుగొను_REPLACE_DIALOG
/*
 * Flags used to distinguish the different contexts in which the
 * find/replace callback may be called.
 */
# define FRD_FINDNEXT	1	// Find next in find dialog
# define FRD_R_FINDNEXT	2	// Find next in repl dialog
# define FRD_REPLACE	3	// Replace once
# define FRD_REPLACEALL	4	// Replace remaining matches
# define FRD_UNDO	5	// Undo replaced text
# define FRD_TYPE_ముసుగు   7	// Mask for the callback type
// Flags which change the way searching is done.
# define FRD_సంపూర్ణ_మాట	0x08	// match whole word only
# define FRD_సరిపోలిక_CASE	0x10	// match case
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GTK
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
# define CONVERT_గమ్యం_UTF8(String)				\
    ((output_conv.vc_type == CONV_NONE || (String) == ఖాళీ)	\
	    ? (String)						\
	    : string_convert(&output_conv, (String), ఖాళీ))

# define CONVERT_గమ్యం_UTF8_FREE(String)				\
    ((String) = ((output_conv.vc_type == CONV_NONE)		\
			? (char_u *)ఖాళీ			\
			: (vim_free(String), (char_u *)ఖాళీ)))

# define CONVERT_FROM_UTF8(String)				\
    ((ఆదానం_conv.vc_type == CONV_NONE || (String) == ఖాళీ)	\
	    ? (String)						\
	    : string_convert(&ఆదానం_conv, (String), ఖాళీ))

# define CONVERT_FROM_UTF8_FREE(String)				\
    ((String) = ((ఆదానం_conv.vc_type == CONV_NONE)		\
			? (char_u *)ఖాళీ			\
			: (vim_free(String), (char_u *)ఖాళీ)))

#లేకపోతే
# define CONVERT_గమ్యం_UTF8(String) (String)
# define CONVERT_గమ్యం_UTF8_FREE(String) ((String) = (char_u *)ఖాళీ)
# define CONVERT_FROM_UTF8(String) (String)
# define CONVERT_FROM_UTF8_FREE(String) ((String) = (char_u *)ఖాళీ)
#అయితే_ముగింపు // FEAT_GUI_GTK

#నిర్వచితమైతే FEAT_GUI_GTK
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
# ఒకవేళ defined(__SUNPRO_C) && (defined(USE_GTK3) || defined(USE_GTK4))
#  define FUNC2GENERIC(func) (శూన్య *)(func)
# లేకపోతే
#  define FUNC2GENERIC(func) G_CALLBACK(func)
# endif
#అయితే_ముగింపు // FEAT_GUI_GTK

#ఒకవేళ defined(UNIX)
# define GUI_MAY_FORK
#అయితే_ముగింపు
