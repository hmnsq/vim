/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MOTIF
# include <Xm/Xm.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
# ifdef VMS
#  include "gui_gtk_vms.h"
# endif
# ifndef USE_GTK4
#  include <X11/Intrinsic.h>
# endif
# pragma GCC diagnostic ತಳ್ಳು
# pragma GCC diagnostic ignored "-Wstrict-prototypes"
# ಆದರೆ !defined(USE_GTK3) && !defined(USE_GTK4)
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
# endif
# include <gtk/gtk.h>
# pragma GCC diagnostic ತೆಗೆ
# ifdef USE_GTK4
// Types used in proto files but not available without X11 headers
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *Widget;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *XtAppContext;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ  Display;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕಿಟಕಿ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ Atom;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ GdkEvent Gdk_Event_ಕೀಲಿ;	// GTK4: GdkEventKey merged into GdkEvent
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
# include "gui_haiku.h"
#ಆದರೆ_ಅಂತ್ಯ

// Needed when generating prototypes, since FEAT_GUI is always defined then.
#ಆದರೆ defined(FEAT_XCLIPBOARD) && !defined(FEAT_GUI_MOTIF) \
	&& !defined(FEAT_GUI_GTK)
# include <X11/Intrinsic.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_PHOTON
# include <Ph.h>
# include <Pt.h>
# include "photon/PxProto.h"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * On some systems scrolling needs to be done right away instead of in the
 * main loop.
 */
#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
# define USE_ON_FLY_SCROLL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * GUIs that support dropping files on a running Vim.
 */
#ಆದರೆ (defined(FEAT_DND) && defined(FEAT_GUI_GTK)) \
	|| defined(FEAT_GUI_MSWIN) \
	|| defined(FEAT_GUI_HAIKU)
# define HAVE_DROP_FILE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * This define makes menus always use a fontset.
 * We're not sure if this code always works, thus it can be disabled.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
# define FONTSET_ALWAYS
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
# define ಪಠ್ಯ_X(ಲಂಬಸಾಲು)	((ಲಂಬಸಾಲು) * gui.char_ಅಗಲ)
# define ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು)	((ಅಡ್ಡಸಾಲು) * gui.char_ಎತ್ತರ + gui.char_ascent)
# define FILL_X(ಲಂಬಸಾಲು)	((ಲಂಬಸಾಲು) * gui.char_ಅಗಲ)
# define FILL_Y(ಅಡ್ಡಸಾಲು)	((ಅಡ್ಡಸಾಲು) * gui.char_ಎತ್ತರ)
# define X_2_ಲಂಬಸಾಲು(x)	((x) / gui.char_ಅಗಲ)
# define Y_2_ಅಡ್ಡಸಾಲು(y)	((y) / gui.char_ಎತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
# define ಪಠ್ಯ_X(ಲಂಬಸಾಲು)	((ಲಂಬಸಾಲು) * gui.char_ಅಗಲ  + gui.border_ಸ್ಥಾನಾಂತರ)
# define FILL_X(ಲಂಬಸಾಲು)	((ಲಂಬಸಾಲು) * gui.char_ಅಗಲ  + gui.border_ಸ್ಥಾನಾಂತರ)
# define X_2_ಲಂಬಸಾಲು(x)	(((x) - gui.border_ಸ್ಥಾನಾಂತರ) / gui.char_ಅಗಲ)
# define ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು)	((ಅಡ್ಡಸಾಲು) * gui.char_ಎತ್ತರ + gui.char_ascent \
							+ gui.border_ಸ್ಥಾನಾಂತರ)
# define FILL_Y(ಅಡ್ಡಸಾಲು)	((ಅಡ್ಡಸಾಲು) * gui.char_ಎತ್ತರ + gui.border_ಸ್ಥಾನಾಂತರ)
# define Y_2_ಅಡ್ಡಸಾಲು(y)	(((y) - gui.border_ಸ್ಥಾನಾಂತರ) / gui.char_ಎತ್ತರ)
#ಆದರೆ_ಅಂತ್ಯ

// Indices for arrays of scrollbars
#ವ್ಯಾಖ್ಯಾನಿಸು SBAR_NONE	    (-1)
#ವ್ಯಾಖ್ಯಾನಿಸು SBAR_ಎಡ	    0
#ವ್ಯಾಖ್ಯಾನಿಸು SBAR_ಬಲ	    1
#ವ್ಯಾಖ್ಯಾನಿಸು SBAR_BOTTOM	    2

// Orientations for scrollbars
#ವ್ಯಾಖ್ಯಾನಿಸು SBAR_VERT	    0
#ವ್ಯಾಖ್ಯಾನಿಸು SBAR_HORIZ	    1

// Default size of scrollbar
#ವ್ಯಾಖ್ಯಾನಿಸು SB_DEFAULT_ಅಗಲ    16

// Default height of the menu bar
#ವ್ಯಾಖ್ಯಾನಿಸು MENU_DEFAULT_ಎತ್ತರ 1		// figure it out at runtime

// Flags for gui_mch_outstr_nowrap()
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_MON_WRAP_CURSOR	0x01	// wrap cursor at end of line
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_MON_INVERT		0x02	// invert the characters
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_MON_IS_CURSOR	0x04	// drawing cursor
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_MON_TRS_CURSOR	0x08	// drawing transparent cursor
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_MON_NOCLEAR		0x10	// don't clear selection

// Flags for gui_mch_draw_string()
#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_TRANSP		0x01	// draw with transparent bg
#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_BOLD		0x02	// draw bold text
#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_UNDERL		0x04	// draw underline text
#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_UNDERC		0x08	// draw undercurl text
#ಆದರೆ defined(FEAT_GUI_GTK)
# define DRAW_ITALIC		0x10	// draw italic text
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_CURSOR		0x20	// drawing block cursor (win32)
#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_STRIKE		0x40	// strikethrough

// For our own tearoff menu item
#ವ್ಯಾಖ್ಯಾನಿಸು TEAR_STRING		"-->Detach"
#ವ್ಯಾಖ್ಯಾನಿಸು TEAR_LEN		(9)	// length of above string

// for the toolbar
#ವ್ಯಾಖ್ಯಾನಿಸು TOOLBAR_BUTTON_ಎತ್ತರ	18
#ವ್ಯಾಖ್ಯಾನಿಸು TOOLBAR_BUTTON_ಅಗಲ	18
#ವ್ಯಾಖ್ಯಾನಿಸು TOOLBAR_BORDER_ಎತ್ತರ	12  // room above+below buttons for MSWindows

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
# define TABLINE_ಎತ್ತರ 22
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MOTIF
# define TABLINE_ಎತ್ತರ 30
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(NO_CONSOLE) || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
# define NO_CONSOLE_ಒಳಹರಿವು	// use no_console_input() to check if there
				// is no console input possible
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ GuiScrollbar
{
    ದೀರ್ಘ	ident;		// Unique identifier for each scrollbar
    win_T	*wp;		// Scrollbar's window, NULL for bottom
    ಪೂರ್ಣಾಂಕ		type;		// one of SBAR_{LEFT,RIGHT,BOTTOM}
    ದೀರ್ಘ	ಮೌಲ್ಯ;		// Represents top line number visible
    ದೀರ್ಘ	ಗಾತ್ರ_2;		// Size of scrollbar thumb
    ದೀರ್ಘ	max;		// Number of lines in buffer

    // Values measured in characters:
    ಪೂರ್ಣಾಂಕ		ತುದಿ;		// Top of scroll bar (chars from row 0)
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;		// Current height of scroll bar in rows
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;		// Current width of scroll bar in cols
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_ಎತ್ತರ;	// Height of status line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
    Widget	id;		// Id of real scroll bar
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    GtkWidget *id;		// Id of real scroll bar
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ handler_id;   // Id of "value_changed" signal handler
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    HWND	id;		// Id of real scroll bar
    ಪೂರ್ಣಾಂಕ		scroll_ಸರಿಸು;	// The scrollbar stuff can handle only up to
				// 32767 lines.  When the file is longer,
				// scroll_shift is set to the number of shifts
				// to reduce the count.
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    VimScrollBar *id;		// Pointer to real scroll bar
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_PHOTON
    PtWidget_t	*id;
#ಆದರೆ_ಅಂತ್ಯ
} scrollbar_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ	    guicolor_T;	// handle for a GUI color; for X11 this should
				// be "Pixel", but that's an unsigned and we
				// need a signed value
#ವ್ಯಾಖ್ಯಾನಿಸು INVALCOLOR ((guicolor_T)-11111)	// number for invalid color; on 32 bit
				   // displays there is a tiny chance this is an
				   // actual color
#ವ್ಯಾಖ್ಯಾನಿಸು CTERMCOLOR ((guicolor_T)-11110)	// only used for cterm.bg_rgb and
					// cterm.fg_rgb: use cterm color

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ PangoFontDescription	*GuiFont;       // handle for a GUI font
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ PangoFontDescription  *GuiFontset;    // handle for a GUI fontset
# define NOFONT		(GuiFont)ಖಾಲಿ
# define NOFONTSET	(GuiFontset)ಖಾಲಿ
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_GUI_PHOTON
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ		*GuiFont;
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ		*GuiFontset;
#  define NOFONT	(GuiFont)ಖಾಲಿ
#  define NOFONTSET	(GuiFontset)ಖಾಲಿ
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef FEAT_GUI_X11
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ XFontStruct	*GuiFont;	// handle for a GUI font
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ XFont_ಗಣ	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  ಇಲ್ಲದಿದ್ದರೆ
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ long_u	GuiFont;	// handle for a GUI font
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ long_u	GuiFontset;	// handle for a GUI fontset
#   define NOFONT	(GuiFont)0
#   define NOFONTSET	(GuiFontset)0
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
// Use spawn when GUI is starting.
# define GUI_MAY_SPAWN

// Uncomment the next definition if you want to use the `:gui` command on
// Windows.  It uses `:mksession` to inherit the session from vim.exe to
// gvim.exe.  So, it doesn't work perfectly. (EXPERIMENTAL)
//# define EXPERIMENTAL_GUI_CMD
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Gui
{
    bool	ಒಳ_focus;	    // Vim has input focus
    bool	ಒಳ_use;		    // Is the GUI being used?
    bool	starting;	    // GUI will start in a little while
    bool	shell_created;	    // Has the shell been created yet?
    bool	dying;		    // Is vim dying? Then output to terminal
    bool	dofork;		    // Use fork() when GUI is starting
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_SPAWN
    bool	dospawn;	    // Use spawn() when GUI is starting
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		dragged_sb;	    // Which scrollbar being dragged, if any?
    win_T	*dragged_wp;	    // Which WIN's sb being dragged, if any?
    bool	ಸೂಚಕ_hidden;	    // Is the mouse pointer hidden?
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;		    // Current cursor column in GUI display
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;		    // Current cursor row in GUI display
    ಪೂರ್ಣಾಂಕ		cursor_ಲಂಬಸಾಲು;	    // Physical cursor column in GUI display
    ಪೂರ್ಣಾಂಕ		cursor_ಅಡ್ಡಸಾಲು;	    // Physical cursor row in GUI display
    bool	cursor_is_valid;    // There is a cursor at cursor_row/col
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;	    // Number of columns
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು;	    // Number of rows
    ಪೂರ್ಣಾಂಕ		scroll_region_ತುದಿ;  // Top (first) line of scroll region
    ಪೂರ್ಣಾಂಕ		scroll_region_bot;  // Bottom (last) line of scroll region
    ಪೂರ್ಣಾಂಕ		scroll_region_ಎಡ;  // Left (first) column of scroll region
    ಪೂರ್ಣಾಂಕ		scroll_region_ಬಲ;  // Right (last) col. of scroll region
    ಪೂರ್ಣಾಂಕ		highlight_ಮುಖವಾಡ;	    // Highlight attribute mask
    ಪೂರ್ಣಾಂಕ		scrollbar_ಅಗಲ;    // Width of vertical scrollbars
    ಪೂರ್ಣಾಂಕ		scrollbar_ಎತ್ತರ;   // Height of horizontal scrollbar
    ಪೂರ್ಣಾಂಕ		ಎಡ_sbar_x;	    // Calculated x coord for left scrollbar
    ಪೂರ್ಣಾಂಕ		ಬಲ_sbar_x;	    // Calculated x coord for right scrollbar
    bool	force_redraw;       // Force a redraw even e.g. not resized
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX
    bool	directx_enabled;    // DirectX (DirectWrite) rendering active
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    ಪೂರ್ಣಾಂಕ		bleed_ಬಲ;	    // Number of pixels to bleed bg color right
    ಪೂರ್ಣಾಂಕ		bleed_bot;	    // Number of pixels to bleed bg color down
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# ifndef FEAT_GUI_GTK
    ಪೂರ್ಣಾಂಕ		menu_ಎತ್ತರ;	    // Height of the menu bar
    ಪೂರ್ಣಾಂಕ		menu_ಅಗಲ;	    // Width of the menu bar
# endif
    bool	menu_is_ಸಕ್ರಿಯ;	    // true if menu is present
#ಆದರೆ_ಅಂತ್ಯ

    scrollbar_T bottom_sbar;	    // Bottom scrollbar
    bool	which_scrollbars[3];// Which scrollbar boxes are active?
    ಪೂರ್ಣಾಂಕ		prev_wrap;	    // For updating the horizontal scrollbar
    ಪೂರ್ಣಾಂಕ		char_ಅಗಲ;	    // Width of char cell in pixels
    ಪೂರ್ಣಾಂಕ		char_ಎತ್ತರ;	    // Height of char cell in pixels, includes
				    // 'linespace'
    ಪೂರ್ಣಾಂಕ		char_ascent;	    // Ascent of char in pixels
    ಪೂರ್ಣಾಂಕ		border_ಅಗಲ;	    // Width of our border around text area
    ಪೂರ್ಣಾಂಕ		border_ಸ್ಥಾನಾಂತರ;	    // Total pixel offset for all borders

    GuiFont	norm_font;	    // Normal font
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_GTK
    GuiFont	bold_font;	    // Bold font
    GuiFont	ital_font;	    // Italic font
    GuiFont	boldital_font;	    // Bold-Italic font
#ಇಲ್ಲದಿದ್ದರೆ
    bool	font_can_bold;	    // Whether norm_font supports bold weight.
				    // The styled font variants are not used.
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_MENU) && !defined(FEAT_GUI_GTK)
# ifdef FONTSET_ALWAYS
    GuiFontset	menu_fontset;	    // set of fonts for multi-byte chars
# ಇಲ್ಲದಿದ್ದರೆ
    GuiFont	menu_font;	    // menu item font
# endif
#ಆದರೆ_ಅಂತ್ಯ
    GuiFont	wide_font;	    // Normal 'guifontwide' font
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GUI_GTK
    GuiFont	wide_bold_font;	    // Bold 'guifontwide' font
    GuiFont	wide_ital_font;	    // Italic 'guifontwide' font
    GuiFont	wide_boldital_font; // Bold-Italic 'guifontwide' font
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    GuiFontset	fontset;	    // set of fonts for multi-byte chars
#ಆದರೆ_ಅಂತ್ಯ
    guicolor_T	ಹಿಂಭಾಗ_pixel;	    // Color of background
    guicolor_T	norm_pixel;	    // Color of normal text
    guicolor_T	def_ಹಿಂಭಾಗ_pixel;	    // default Color of background
    guicolor_T	def_norm_pixel;	    // default Color of normal text

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
    ಅಕ್ಷರ	*rsrc_menu_fg_name;	// Color of menu & dialog foreground
    guicolor_T	menu_fg_pixel;		// Same in Pixel format
    ಅಕ್ಷರ	*rsrc_menu_bg_name;	// Color of menu & dialog background
    guicolor_T	menu_bg_pixel;		// Same in Pixel format
    ಅಕ್ಷರ	*rsrc_scroll_fg_name;	// Color of scrollbar foreground
    guicolor_T	scroll_fg_pixel;	// Same in Pixel format
    ಅಕ್ಷರ	*rsrc_scroll_bg_name;	// Color of scrollbar background
    guicolor_T	scroll_bg_pixel;	// Same in Pixel format

    Display	*dpy;		    // X display
    ಕಿಟಕಿ	wid;		    // Window id of text area
    ಪೂರ್ಣಾಂಕ		visibility;	    // Is shell partially/fully obscured?
    GC		ಪಠ್ಯ_gc;
    GC		ಹಿಂಭಾಗ_gc;
    GC		invert_gc;
    Cursor	blank_ಸೂಚಕ;	    // Blank pointer

    // X Resources
    char_u	*rsrc_font_name;    // Resource font name, used if 'guifont'
				    // not set
    char_u	*rsrc_bold_font_name; // Resource bold font name
    char_u	*rsrc_ital_font_name; // Resource italic font name
    char_u	*rsrc_boldital_font_name;  // Resource bold-italic font name
    char_u	*rsrc_menu_font_name;    // Resource menu Font name
    Bool	rsrc_rev_video;	    // Use reverse video?

    char_u	*geom;		    // Geometry, eg "80x24"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
# ಆದರೆ !defined(USE_GTK3) && !defined(USE_GTK4)
    ಪೂರ್ಣಾಂಕ		visibility;	    // Is shell partially/fully obscured?
# endif
    GdkCursor	*blank_ಸೂಚಕ;	    // Blank pointer

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
# ಆದರೆ defined(USE_GTK3) || defined(USE_GTK4)
    GdkRGBA	*fgcolor;	    // GDK-styled foreground color
    GdkRGBA	*bgcolor;	    // GDK-styled background color
    GdkRGBA	*spcolor;	    // GDK-styled special color
# ಇಲ್ಲದಿದ್ದರೆ
    GdkColor	*fgcolor;	    // GDK-styled foreground color
    GdkColor	*bgcolor;	    // GDK-styled background color
    GdkColor	*spcolor;	    // GDK-styled special color
# endif
# ಆದರೆ defined(USE_GTK3) && !defined(USE_GTK4)
    cairo_surface_t *surface;       // drawarea surface
# elif !defined(USE_GTK4)
    GdkGC	*ಪಠ್ಯ_gc;	    // cached GC for normal text
# endif
    PangoContext     *ಪಠ್ಯ_context; // the context used for all text
    PangoFont	     *ascii_font;   // cached font for ASCII strings
    PangoGlyphString *ascii_glyphs; // cached code point -> glyph map
# ifdef FEAT_GUI_TABLINE
    GtkWidget	*tabline;	    // tab pages line handle
# endif

# ifndef USE_GTK4
    Gtk_Accel_ಗುಂಪು *accel_ಗುಂಪು;
# endif
    GtkWidget	*filedlg;	    // file selection dialog
    char_u	*browse_fname;	    // file name from filedlg

    guint32	event_ಸಮಯ;
# ifdef FEAT_GUI_DIALOG
    // Multiple dialogs not allowed, just tracked for future use.
    ಪೂರ್ಣಾಂಕ		dialogs_ಸಕ್ರಿಯ;     // number of active GUI dialogs
    // Suppress focus-in event when focus returns.
    ಪೂರ್ಣಾಂಕ		dialog_focus_pending;
# endif
    bool	is_x11;	            // active gdk backend in gtk is x11
# ifdef GDK_WINDOWING_WAYLAND
    bool	is_wayland;	    // active gdk backend in gtk is wayland
# endif
# ಆದರೆ defined(FEAT_IMAGE) && defined(FEAT_GUI_GTK)
    ದ್ವಿಗುಣ	scale;		    // Current scaling (may be fractional)
# endif
#ಆದರೆ_ಅಂತ್ಯ	// FEAT_GUI_GTK

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
    char_u ligatures_map[256];	    // ascii map for characters 0-255, value is
				    // 1 if in 'guiligatures'
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE) \
	&& (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
		|| defined(FEAT_GUI_HAIKU))
    ಪೂರ್ಣಾಂಕ		tabline_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR) \
	&& (defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_MSWIN))
    ಪೂರ್ಣಾಂಕ		toolbar_ಎತ್ತರ;	    // height of the toolbar
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_TIP
    // Tooltip properties; also used for balloon evaluation
    char_u	*rsrc_tooltip_font_name; // tooltip font name
    ಅಕ್ಷರ	*rsrc_tooltip_fg_name;	// tooltip foreground color name
    ಅಕ್ಷರ	*rsrc_tooltip_bg_name;	// tooltip background color name
    guicolor_T	tooltip_fg_pixel;	// tooltip foreground color
    guicolor_T	tooltip_bg_pixel;	// tooltip background color
    XFont_ಗಣ	tooltip_fontset;	// tooltip fontset
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    GuiFont	currFont;	    // Current font
    guicolor_T	currFgColor;	    // Current foreground text color
    guicolor_T	currBgColor;	    // Current background text color
    guicolor_T	currSpColor;	    // Current special text color

    guicolor_T	title_bg_pixel;	    // window title bar color
    guicolor_T	title_fg_pixel;	    // window title bar's text color
    guicolor_T	titlenc_bg_pixel;   // window title bar color not current
    guicolor_T	titlenc_fg_pixel;   // window title bar's text color not current
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    VimApp     *vimApp;
    Vim_ಕಿಟಕಿ  *vim_ಕಿಟಕಿ;
    VimFormView *vimForm;
    Vim_ಪಠ್ಯ_Area_View *vim_ಪಠ್ಯ_Area;
    ಪೂರ್ಣಾಂಕ	vdcmp;			    // Vim Direct Communication Message Port
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_PHOTON
    PtWidget_t	*vim_ಕಿಟಕಿ;		// PtWindow
    PtWidget_t	*vim_ಪಠ್ಯ_Area;		// PtRaw
    PtWidget_t	*vimContainer;		// PtPanel
# ಆದರೆ defined(FEAT_MENU) || defined(FEAT_TOOLBAR)
    PtWidget_t	*vim_Tool_Bar_ಗುಂಪು;
# endif
# ifdef FEAT_MENU
    PtWidget_t	*vimMenuBar;
# endif
# ifdef FEAT_TOOLBAR
    PtWidget_t	*vimToolBar;
    ಪೂರ್ಣಾಂಕ		toolbar_ಎತ್ತರ;
# endif
    PhEvent_t	*event_ಬಫರ್;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    ಅಕ್ಷರ	*rsrc_ಒಳಹರಿವು_ವಿಧಾನ;
    ಅಕ್ಷರ	*rsrc_preedit_type_name;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_GTK) && defined(USE_GTK4)
    ಪೂರ್ಣಾಂಕ decor_ಎತ್ತರ;

    // Size of the form widget last asked for with gui_mch_set_shellsize().
    // "pending_form_skip" counts how many allocations that do not answer it
    // may still be ignored.
    ಪೂರ್ಣಾಂಕ pending_form_w;
    ಪೂರ್ಣಾಂಕ pending_form_h;
    ಪೂರ್ಣಾಂಕ pending_form_skip;

    // Used for clipboard functionality in GTK4 GUI
    GdkContentProvider *regular_provider;
    GdkContentProvider *primary_provider;

# ifdef FEAT_DND
    Gtk_Drop_ಗುರಿ_Async *drop_ಗುರಿ;
# endif
#ಆದರೆ_ಅಂತ್ಯ
} gui_T;

ಬಾಹ್ಯ gui_T gui;			// this is defined in gui.c

// definitions of available window positionings for gui_*_position_in_parent()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    VW_POS_MOUSE,
    VW_POS_CENTER,
    VW_POS_ತುದಿ_CENTER
} gui_win_pos_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಕಂಡುಹಿಡಿ_REPLACE_DIALOG
/*
 * Flags used to distinguish the different contexts in which the
 * find/replace callback may be called.
 */
# define FRD_FINDNEXT	1	// Find next in find dialog
# define FRD_R_FINDNEXT	2	// Find next in repl dialog
# define FRD_REPLACE	3	// Replace once
# define FRD_REPLACEALL	4	// Replace remaining matches
# define FRD_UNDO	5	// Undo replaced text
# define FRD_TYPE_ಮುಖವಾಡ   7	// Mask for the callback type
// Flags which change the way searching is done.
# define FRD_ಸಂಪೂರ್ಣ_ಶಬ್ದ	0x08	// match whole word only
# define FRD_ಹೊಂದಿಕೆ_CASE	0x10	// match case
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
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
# define CONVERT_ಗಮ್ಯ_UTF8(String)				\
    ((output_conv.vc_type == CONV_NONE || (String) == ಖಾಲಿ)	\
	    ? (String)						\
	    : string_convert(&output_conv, (String), ಖಾಲಿ))

# define CONVERT_ಗಮ್ಯ_UTF8_FREE(String)				\
    ((String) = ((output_conv.vc_type == CONV_NONE)		\
			? (char_u *)ಖಾಲಿ			\
			: (vim_free(String), (char_u *)ಖಾಲಿ)))

# define CONVERT_FROM_UTF8(String)				\
    ((ಒಳಹರಿವು_conv.vc_type == CONV_NONE || (String) == ಖಾಲಿ)	\
	    ? (String)						\
	    : string_convert(&ಒಳಹರಿವು_conv, (String), ಖಾಲಿ))

# define CONVERT_FROM_UTF8_FREE(String)				\
    ((String) = ((ಒಳಹರಿವು_conv.vc_type == CONV_NONE)		\
			? (char_u *)ಖಾಲಿ			\
			: (vim_free(String), (char_u *)ಖಾಲಿ)))

#ಇಲ್ಲದಿದ್ದರೆ
# define CONVERT_ಗಮ್ಯ_UTF8(String) (String)
# define CONVERT_ಗಮ್ಯ_UTF8_FREE(String) ((String) = (char_u *)ಖಾಲಿ)
# define CONVERT_FROM_UTF8(String) (String)
# define CONVERT_FROM_UTF8_FREE(String) ((String) = (char_u *)ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_GTK

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
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
# ಆದರೆ defined(__SUNPRO_C) && (defined(USE_GTK3) || defined(USE_GTK4))
#  define FUNC2GENERIC(func) (ಶೂನ್ಯ *)(func)
# ಇಲ್ಲದಿದ್ದರೆ
#  define FUNC2GENERIC(func) G_CALLBACK(func)
# endif
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_GTK

#ಆದರೆ defined(UNIX)
# define GUI_MAY_FORK
#ಆದರೆ_ಅಂತ್ಯ
