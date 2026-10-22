/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MOTIF
# include <Xm/Xm.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
# ifdef VMS
#  include "gui_gtk_vms.h"
# endif
# ifndef USE_GTK4
#  include <X11/Intrinsic.h>
# endif
# pragma GCC diagnostic ਧੱਕੋ
# pragma GCC diagnostic ignored "-Wstrict-prototypes"
# ਜੇ !defined(USE_GTK3) && !defined(USE_GTK4)
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
# include <gtk/gtk.h>
# pragma GCC diagnostic ਕੱਢੋ
# ifdef USE_GTK4
// Types used in proto files but not available without X11 headers
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *Widget;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *XtAppContext;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ  Display;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਖਿੜਕੀ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ Atom;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ GdkEvent Gdk_Event_ਕੁੰਜੀ;	// GTK4: GdkEventKey merged into GdkEvent
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
# include "gui_haiku.h"
#ਜੇ_ਅੰਤ

// Needed when generating prototypes, since FEAT_GUI is always defined then.
#ਜੇ defined(FEAT_XCLIPBOARD) && !defined(FEAT_GUI_MOTIF) \
	&& !defined(FEAT_GUI_GTK)
# include <X11/Intrinsic.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_PHOTON
# include <Ph.h>
# include <Pt.h>
# include "photon/PxProto.h"
#ਜੇ_ਅੰਤ

/*
 * On some systems scrolling needs to be done right away instead of in the
 * main loop.
 */
#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
# define USE_ON_FLY_SCROLL
#ਜੇ_ਅੰਤ

/*
 * GUIs that support dropping files on a running Vim.
 */
#ਜੇ (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
# define HAVE_DROP_FILE
#ਜੇ_ਅੰਤ

/*
 * This define makes menus always use a fontset.
 * We're not sure if this code always works, thus it can be disabled.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
# define FONTSET_ALWAYS
#ਜੇ_ਅੰਤ

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# define ਪਾਠ_X(ਖਾਨਾ)	((ਖਾਨਾ) * gui.char_ਚੌੜਾਈ)
# define ਪਾਠ_Y(ਪੰਕਤੀ)	((ਪੰਕਤੀ) * gui.char_ਉਚਾਈ + gui.char_ascent)
# define FILL_X(ਖਾਨਾ)	((ਖਾਨਾ) * gui.char_ਚੌੜਾਈ)
# define FILL_Y(ਪੰਕਤੀ)	((ਪੰਕਤੀ) * gui.char_ਉਚਾਈ)
# define X_2_ਖਾਨਾ(x)	((x) / gui.char_ਚੌੜਾਈ)
# define Y_2_ਪੰਕਤੀ(y)	((y) / gui.char_ਉਚਾਈ)
#ਨਹੀਂਤਾਂ
# define ਪਾਠ_X(ਖਾਨਾ)	((ਖਾਨਾ) * gui.char_ਚੌੜਾਈ  + gui.border_ਵਿੱਥ)
# define FILL_X(ਖਾਨਾ)	((ਖਾਨਾ) * gui.char_ਚੌੜਾਈ  + gui.border_ਵਿੱਥ)
# define X_2_ਖਾਨਾ(x)	(((x) - gui.border_ਵਿੱਥ) / gui.char_ਚੌੜਾਈ)
# define ਪਾਠ_Y(ਪੰਕਤੀ)	((ਪੰਕਤੀ) * gui.char_ਉਚਾਈ + gui.char_ascent \
							+ gui.border_ਵਿੱਥ)
# define FILL_Y(ਪੰਕਤੀ)	((ਪੰਕਤੀ) * gui.char_ਉਚਾਈ + gui.border_ਵਿੱਥ)
# define Y_2_ਪੰਕਤੀ(y)	(((y) - gui.border_ਵਿੱਥ) / gui.char_ਉਚਾਈ)
#ਜੇ_ਅੰਤ

// Indices for arrays of scrollbars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBAR_NONE	    (-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBAR_ਖੱਬਾ	    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBAR_ਸੱਜਾ	    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBAR_BOTTOM	    2

// Orientations for scrollbars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBAR_VERT	    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SBAR_HORIZ	    1

// Default size of scrollbar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SB_DEFAULT_ਚੌੜਾਈ    16

// Default height of the menu bar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MENU_DEFAULT_ਉਚਾਈ 1		// figure it out at runtime

// Flags for gui_mch_outstr_nowrap()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_MON_WRAP_CURSOR	0x01	// wrap cursor at end of line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_MON_INVERT		0x02	// invert the characters
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_MON_IS_CURSOR	0x04	// drawing cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_MON_TRS_CURSOR	0x08	// drawing transparent cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_MON_NOCLEAR		0x10	// don't clear selection

// Flags for gui_mch_draw_string()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_TRANSP		0x01	// draw with transparent bg
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_BOLD		0x02	// draw bold text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_UNDERL		0x04	// draw underline text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_UNDERC		0x08	// draw undercurl text
#ਜੇ defined(FEAT_GUI_GTK)
# define DRAW_ITALIC		0x10	// draw italic text
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_CURSOR		0x20	// drawing block cursor (win32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_STRIKE		0x40	// strikethrough

// For our own tearoff menu item
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEAR_STRING		"-->Detach"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEAR_LEN		(9)	// length of above string

// for the toolbar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TOOLBAR_BUTTON_ਉਚਾਈ	18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TOOLBAR_BUTTON_ਚੌੜਾਈ	18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TOOLBAR_BORDER_ਉਚਾਈ	12  // room above+below buttons for MSWindows

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# define TABLINE_ਉਚਾਈ 22
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MOTIF
# define TABLINE_ਉਚਾਈ 30
#ਜੇ_ਅੰਤ

#ਜੇ defined(NO_CONSOLE) || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
# define NO_CONSOLE_ਨਿਵੇਸ਼	// use no_console_input() to check if there
				// is no console input possible
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ GuiScrollbar
{
    ਲੰਮਾ	ident;		// Unique identifier for each scrollbar
    win_T	*wp;		// Scrollbar's window, NULL for bottom
    ਪੂਰਨ		type;		// one of SBAR_{LEFT,RIGHT,BOTTOM}
    ਲੰਮਾ	ਮੁੱਲ;		// Represents top line number visible
    ਲੰਮਾ	ਮਾਪ;		// Size of scrollbar thumb
    ਲੰਮਾ	max;		// Number of lines in buffer

    // Values measured in characters:
    ਪੂਰਨ		ਸਿਖਰ;		// Top of scroll bar (chars from row 0)
    ਪੂਰਨ		ਉਚਾਈ;		// Current height of scroll bar in rows
    ਪੂਰਨ		ਚੌੜਾਈ;		// Current width of scroll bar in cols
    ਪੂਰਨ		ਸਥਿਤੀ_ਉਚਾਈ;	// Height of status line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
    Widget	id;		// Id of real scroll bar
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    GtkWidget *id;		// Id of real scroll bar
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ handler_id;   // Id of "value_changed" signal handler
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    HWND	id;		// Id of real scroll bar
    ਪੂਰਨ		scroll_ਖਿਸਕਾਓ;	// The scrollbar stuff can handle only up to
				// 32767 lines.  When the file is longer,
				// scroll_shift is set to the number of shifts
				// to reduce the count.
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    VimScrollBar *id;		// Pointer to real scroll bar
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_PHOTON
    PtWidget_t	*id;
#ਜੇ_ਅੰਤ
} scrollbar_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ	    guicolor_T;	// handle for a GUI color; for X11 this should
				// be "Pixel", but that's an unsigned and we
				// need a signed value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALCOLOR ((guicolor_T)-11111)	// number for invalid color; on 32 bit
				   // displays there is a tiny chance this is an
				   // actual color
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTERMCOLOR ((guicolor_T)-11110)	// only used for cterm.bg_rgb and
					// cterm.fg_rgb: use cterm color

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ PangoFontDescription	*GuiFont;       // handle for a GUI font
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ PangoFontDescription  *GuiFontset;    // handle for a GUI fontset
# define NOFONT		(GuiFont)ਨਿਰਾ
# define NOFONTSET	(GuiFontset)ਨਿਰਾ
#ਨਹੀਂਤਾਂ
# ifdef FEAT_GUI_PHOTON
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ		*GuiFont;
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ		*GuiFontset;
#  define NOFONT	(GuiFont)ਨਿਰਾ
#  define NOFONTSET	(GuiFontset)ਨਿਰਾ
# ਨਹੀਂਤਾਂ
#  ifdef FEAT_GUI_X11
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ XFontStruct	*GuiFont;	// handle for a GUI font
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ XFont_ਸੈੱਟ	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  ਨਹੀਂਤਾਂ
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ long_u	GuiFont;	// handle for a GUI font
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ long_u	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
// Use spawn when GUI is starting.
# define GUI_MAY_SPAWN

// Uncomment the next definition if you want to use the `:gui` command on
// Windows.  It uses `:mksession` to inherit the session from vim.exe to
// gvim.exe.  So, it doesn't work perfectly. (EXPERIMENTAL)
//# define EXPERIMENTAL_GUI_CMD
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Gui
{
    bool	ਵਿੱਚ_focus;	    // Vim has input focus
    bool	ਵਿੱਚ_use;		    // Is the GUI being used?
    bool	starting;	    // GUI will start in a little while
    bool	shell_created;	    // Has the shell been created yet?
    bool	dying;		    // Is vim dying? Then output to terminal
    bool	dofork;		    // Use fork() when GUI is starting
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GUI_MAY_SPAWN
    bool	dospawn;	    // Use spawn() when GUI is starting
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		dragged_sb;	    // Which scrollbar being dragged, if any?
    win_T	*dragged_wp;	    // Which WIN's sb being dragged, if any?
    bool	ਸੂਚਕ_hidden;	    // Is the mouse pointer hidden?
    ਪੂਰਨ		ਖਾਨਾ;		    // Current cursor column in GUI display
    ਪੂਰਨ		ਪੰਕਤੀ;		    // Current cursor row in GUI display
    ਪੂਰਨ		cursor_ਖਾਨਾ;	    // Physical cursor column in GUI display
    ਪੂਰਨ		cursor_ਪੰਕਤੀ;	    // Physical cursor row in GUI display
    bool	cursor_is_valid;    // There is a cursor at cursor_row/col
    ਪੂਰਨ		ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ;	    // Number of columns
    ਪੂਰਨ		ਅੰਕ_ਪੰਕਤੀਆਂ;	    // Number of rows
    ਪੂਰਨ		scroll_region_ਸਿਖਰ;  // Top (first) line of scroll region
    ਪੂਰਨ		scroll_region_bot;  // Bottom (last) line of scroll region
    ਪੂਰਨ		scroll_region_ਖੱਬਾ;  // Left (first) column of scroll region
    ਪੂਰਨ		scroll_region_ਸੱਜਾ;  // Right (last) col. of scroll region
    ਪੂਰਨ		highlight_ਨਕਾਬ;	    // Highlight attribute mask
    ਪੂਰਨ		scrollbar_ਚੌੜਾਈ;    // Width of vertical scrollbars
    ਪੂਰਨ		scrollbar_ਉਚਾਈ;   // Height of horizontal scrollbar
    ਪੂਰਨ		ਖੱਬਾ_sbar_x;	    // Calculated x coord for left scrollbar
    ਪੂਰਨ		ਸੱਜਾ_sbar_x;	    // Calculated x coord for right scrollbar
    bool	force_redraw;       // Force a redraw even e.g. not resized
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX
    bool	directx_enabled;    // DirectX (DirectWrite) rendering active
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    ਪੂਰਨ		bleed_ਸੱਜਾ;	    // Number of pixels to bleed bg color right
    ਪੂਰਨ		bleed_bot;	    // Number of pixels to bleed bg color down
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# ifndef FEAT_GUI_GTK
    ਪੂਰਨ		menu_ਉਚਾਈ;	    // Height of the menu bar
    ਪੂਰਨ		menu_ਚੌੜਾਈ;	    // Width of the menu bar
# endif
    bool	menu_is_ਸਰਗਰਮ;	    // true if menu is present
#ਜੇ_ਅੰਤ

    scrollbar_T bottom_sbar;	    // Bottom scrollbar
    bool	which_scrollbars[3];// Which scrollbar boxes are active?
    ਪੂਰਨ		prev_wrap;	    // For updating the horizontal scrollbar
    ਪੂਰਨ		char_ਚੌੜਾਈ;	    // Width of char cell in pixels
    ਪੂਰਨ		char_ਉਚਾਈ;	    // Height of char cell in pixels, includes
				    // 'linespace'
    ਪੂਰਨ		char_ascent;	    // Ascent of char in pixels
    ਪੂਰਨ		border_ਚੌੜਾਈ;	    // Width of our border around text area
    ਪੂਰਨ		border_ਵਿੱਥ;	    // Total pixel offset for all borders

    GuiFont	norm_font;	    // Normal font
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    GuiFont	bold_font;	    // Bold font
    GuiFont	ital_font;	    // Italic font
    GuiFont	boldital_font;	    // Bold-Italic font
#ਨਹੀਂਤਾਂ
    bool	font_can_bold;	    // Whether norm_font supports bold weight.
				    // The styled font variants are not used.
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK)
# ifdef FONTSET_ALWAYS
    GuiFontset	menu_fontset;	    // set of fonts for multi-byte chars
# ਨਹੀਂਤਾਂ
    GuiFont	menu_font;	    // menu item font
# endif
#ਜੇ_ਅੰਤ
    GuiFont	wide_font;	    // Normal 'guifontwide' font
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    GuiFont	wide_bold_font;	    // Bold 'guifontwide' font
    GuiFont	wide_ital_font;	    // Italic 'guifontwide' font
    GuiFont	wide_boldital_font; // Bold-Italic 'guifontwide' font
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    GuiFontset	fontset;	    // set of fonts for multi-byte chars
#ਜੇ_ਅੰਤ
    guicolor_T	ਪਿੱਠ_pixel;	    // Color of background
    guicolor_T	norm_pixel;	    // Color of normal text
    guicolor_T	def_ਪਿੱਠ_pixel;	    // default Color of background
    guicolor_T	def_norm_pixel;	    // default Color of normal text

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
    ਅੱਖਰ	*rsrc_menu_fg_name;	// Color of menu & dialog foreground
    guicolor_T	menu_fg_pixel;		// Same in Pixel format
    ਅੱਖਰ	*rsrc_menu_bg_name;	// Color of menu & dialog background
    guicolor_T	menu_bg_pixel;		// Same in Pixel format
    ਅੱਖਰ	*rsrc_scroll_fg_name;	// Color of scrollbar foreground
    guicolor_T	scroll_fg_pixel;	// Same in Pixel format
    ਅੱਖਰ	*rsrc_scroll_bg_name;	// Color of scrollbar background
    guicolor_T	scroll_bg_pixel;	// Same in Pixel format

    Display	*dpy;		    // X display
    ਖਿੜਕੀ	wid;		    // Window id of text area
    ਪੂਰਨ		visibility;	    // Is shell partially/fully obscured?
    GC		ਪਾਠ_gc;
    GC		ਪਿੱਠ_gc;
    GC		invert_gc;
    Cursor	blank_ਸੂਚਕ;	    // Blank pointer

    // X Resources
    char_u	*rsrc_font_name;    // Resource font name, used if 'guifont'
				    // not set
    char_u	*rsrc_bold_font_name; // Resource bold font name
    char_u	*rsrc_ital_font_name; // Resource italic font name
    char_u	*rsrc_boldital_font_name;  // Resource bold-italic font name
    char_u	*rsrc_menu_font_name;    // Resource menu Font name
    Bool	rsrc_rev_video;	    // Use reverse video?

    char_u	*geom;		    // Geometry, eg "80x24"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
# ਜੇ !defined(USE_GTK3) && !defined(USE_GTK4)
    ਪੂਰਨ		visibility;	    // Is shell partially/fully obscured?
# endif
    GdkCursor	*blank_ਸੂਚਕ;	    // Blank pointer

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
# ਜੇ defined(USE_GTK3) || defined(USE_GTK4)
    GdkRGBA	*fgcolor;	    // GDK-styled foreground color
    GdkRGBA	*bgcolor;	    // GDK-styled background color
    GdkRGBA	*spcolor;	    // GDK-styled special color
# ਨਹੀਂਤਾਂ
    GdkColor	*fgcolor;	    // GDK-styled foreground color
    GdkColor	*bgcolor;	    // GDK-styled background color
    GdkColor	*spcolor;	    // GDK-styled special color
# endif
# ਜੇ defined(USE_GTK3) && !defined(USE_GTK4)
    cairo_surface_t *surface;       // drawarea surface
# elif !defined(USE_GTK4)
    GdkGC	*ਪਾਠ_gc;	    // cached GC for normal text
# endif
    PangoContext     *ਪਾਠ_context; // the context used for all text
    PangoFont	     *ascii_font;   // cached font for ASCII strings
    PangoGlyphString *ascii_glyphs; // cached code point -> glyph map
# ifdef FEAT_GUI_TABLINE
    GtkWidget	*tabline;	    // tab pages line handle
# endif

# ifndef USE_GTK4
    Gtk_Accel_ਸਮੂਹ *accel_ਸਮੂਹ;
# endif
    GtkWidget	*filedlg;	    // file selection dialog
    char_u	*browse_fname;	    // file name from filedlg

    guint32	event_ਸਮਾਂ;
# ifdef FEAT_GUI_DIALOG
    // Multiple dialogs not allowed, just tracked for future use.
    ਪੂਰਨ		dialogs_ਸਰਗਰਮ;     // number of active GUI dialogs
    // Suppress focus-in event when focus returns.
    ਪੂਰਨ		dialog_focus_pending;
# endif
    bool	is_x11;	            // active gdk backend in gtk is x11
# ifdef GDK_WINDOWING_WAYLAND
    bool	is_wayland;	    // active gdk backend in gtk is wayland
# endif
# ਜੇ defined(FEAT_IMAGE) && defined(FEAT_GUI_GTK)
    ਦੁੱਗਣਾ	scale;		    // Current scaling (may be fractional)
# endif
#ਜੇ_ਅੰਤ	// FEAT_GUI_GTK

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    char_u ligatures_map[256];	    // ascii map for characters 0-255, value is
				    // 1 if in 'guiligatures'
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE) \
	&& (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
		|| defined(FEAT_GUI_HAIKU))
    ਪੂਰਨ		tabline_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR) \
	&& (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    ਪੂਰਨ		toolbar_ਉਚਾਈ;	    // height of the toolbar
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_TIP
    // Tooltip properties; also used for balloon evaluation
    char_u	*rsrc_tooltip_font_name; // tooltip font name
    ਅੱਖਰ	*rsrc_tooltip_fg_name;	// tooltip foreground color name
    ਅੱਖਰ	*rsrc_tooltip_bg_name;	// tooltip background color name
    guicolor_T	tooltip_fg_pixel;	// tooltip foreground color
    guicolor_T	tooltip_bg_pixel;	// tooltip background color
    XFont_ਸੈੱਟ	tooltip_fontset;	// tooltip fontset
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    GuiFont	currFont;	    // Current font
    guicolor_T	currFgColor;	    // Current foreground text color
    guicolor_T	currBgColor;	    // Current background text color
    guicolor_T	currSpColor;	    // Current special text color

    guicolor_T	title_bg_pixel;	    // window title bar color
    guicolor_T	title_fg_pixel;	    // window title bar's text color
    guicolor_T	titlenc_bg_pixel;   // window title bar color not current
    guicolor_T	titlenc_fg_pixel;   // window title bar's text color not current
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    VimApp     *vimApp;
    Vim_ਖਿੜਕੀ  *vim_ਖਿੜਕੀ;
    VimFormView *vimForm;
    Vim_ਪਾਠ_Area_View *vim_ਪਾਠ_Area;
    ਪੂਰਨ	vdcmp;			    // Vim Direct Communication Message Port
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_PHOTON
    PtWidget_t	*vim_ਖਿੜਕੀ;		// PtWindow
    PtWidget_t	*vim_ਪਾਠ_Area;		// PtRaw
    PtWidget_t	*vimContainer;		// PtPanel
# ਜੇ defined(FEAT_MENU) || defined(FEAT_TOOLBAR)
    PtWidget_t	*vim_Tool_Bar_ਸਮੂਹ;
# endif
# ifdef FEAT_MENU
    PtWidget_t	*vimMenuBar;
# endif
# ifdef FEAT_TOOLBAR
    PtWidget_t	*vimToolBar;
    ਪੂਰਨ		toolbar_ਉਚਾਈ;
# endif
    PhEvent_t	*event_ਬਫਰ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    ਅੱਖਰ	*rsrc_ਨਿਵੇਸ਼_ਵਿਧੀ;
    ਅੱਖਰ	*rsrc_preedit_type_name;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    ਪੂਰਨ decor_ਉਚਾਈ;

    // Size of the form widget last asked for with gui_mch_set_shellsize().
    // "pending_form_skip" counts how many allocations that do not answer it
    // may still be ignored.
    ਪੂਰਨ pending_form_w;
    ਪੂਰਨ pending_form_h;
    ਪੂਰਨ pending_form_skip;

    // Used for clipboard functionality in GTK4 GUI
    GdkContentProvider *regular_provider;
    GdkContentProvider *primary_provider;

# ifdef FEAT_DND
    Gtk_Drop_ਨਿਸ਼ਾਨਾ_Async *drop_ਨਿਸ਼ਾਨਾ;
# endif
#ਜੇ_ਅੰਤ
} gui_T;

ਬਾਹਰੀ gui_T gui;			// this is defined in gui.c

// definitions of available window positionings for gui_*_position_in_parent()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    VW_POS_MOUSE,
    VW_POS_CENTER,
    VW_POS_ਸਿਖਰ_CENTER
} gui_win_pos_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਲੱਭੋ_REPLACE_DIALOG
/*
 * Flags used to distinguish the different contexts in which the
 * find/replace callback may be called.
 */
# define FRD_FINDNEXT	1	// Find next in find dialog
# define FRD_R_FINDNEXT	2	// Find next in repl dialog
# define FRD_REPLACE	3	// Replace once
# define FRD_REPLACEALL	4	// Replace remaining matches
# define FRD_UNDO	5	// Undo replaced text
# define FRD_TYPE_ਨਕਾਬ   7	// Mask for the callback type
// Flags which change the way searching is done.
# define FRD_ਸਾਰਾ_ਸ਼ਬਦ	0x08	// match whole word only
# define FRD_ਮੇਲ_CASE	0x10	// match case
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
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
# define CONVERT_ਮੰਜ਼ਿਲ_UTF8(String)				\
    ((output_conv.vc_type == CONV_NONE || (String) == ਨਿਰਾ)	\
	    ? (String)						\
	    : string_convert(&output_conv, (String), ਨਿਰਾ))

# define CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(String)				\
    ((String) = ((output_conv.vc_type == CONV_NONE)		\
			? (char_u *)ਨਿਰਾ			\
			: (vim_free(String), (char_u *)ਨਿਰਾ)))

# define CONVERT_FROM_UTF8(String)				\
    ((ਨਿਵੇਸ਼_conv.vc_type == CONV_NONE || (String) == ਨਿਰਾ)	\
	    ? (String)						\
	    : string_convert(&ਨਿਵੇਸ਼_conv, (String), ਨਿਰਾ))

# define CONVERT_FROM_UTF8_FREE(String)				\
    ((String) = ((ਨਿਵੇਸ਼_conv.vc_type == CONV_NONE)		\
			? (char_u *)ਨਿਰਾ			\
			: (vim_free(String), (char_u *)ਨਿਰਾ)))

#ਨਹੀਂਤਾਂ
# define CONVERT_ਮੰਜ਼ਿਲ_UTF8(String) (String)
# define CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(String) ((String) = (char_u *)ਨਿਰਾ)
# define CONVERT_FROM_UTF8(String) (String)
# define CONVERT_FROM_UTF8_FREE(String) ((String) = (char_u *)ਨਿਰਾ)
#ਜੇ_ਅੰਤ // FEAT_GUI_GTK

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
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
# ਜੇ defined(__SUNPRO_C) && (defined(USE_GTK3) || defined(USE_GTK4))
#  define FUNC2GENERIC(func) (ਖਾਲੀ *)(func)
# ਨਹੀਂਤਾਂ
#  define FUNC2GENERIC(func) G_CALLBACK(func)
# endif
#ਜੇ_ਅੰਤ // FEAT_GUI_GTK

#ਜੇ defined(UNIX)
# define GUI_MAY_FORK
#ਜੇ_ਅੰਤ
