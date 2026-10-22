/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Photon GUI support by Julian Kinraid
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 *
 * Clipboard support is in os_qnx.c
 * PhAttach() is called in os_qnx.c:qnx_init()
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
# include <photon/PxImage.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	ಪೂರ್ಣಾಂಕ
# define PhEvent_t	ಪೂರ್ಣಾಂಕ
# define Ph_ಬಿಂದು_t	ಪೂರ್ಣಾಂಕ
# define PtWidget_t	ಪೂರ್ಣಾಂಕ
# define Pg_BLACK	0
# define PtCallbackF_t	ಪೂರ್ಣಾಂಕ
# define PtCallbackInfo_t ಪೂರ್ಣಾಂಕ
# define PhTile_t	ಪೂರ್ಣಾಂಕ
# define PtWidget_t	ಪೂರ್ಣಾಂಕ
# define PhImage_t	ಪೂರ್ಣಾಂಕ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು RGB(r, g, b) PgRGB(r, g, b)

#ವ್ಯಾಖ್ಯಾನಿಸು EVENT_ಬಫರ್_ಗಾತ್ರ ಗಾತ್ರ(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_HIDE		TRUE
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು USE_PANEL_ಗುಂಪು

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
ಸ್ಥಿರ ಅಕ್ಷರ	*ಖಾಲಿಗಳು_title = "    ";
ಸ್ಥಿರ ಅಕ್ಷರ	**panel_titles = ಖಾಲಿ;
ಸ್ಥಿರ ushort_t	ಅಂಕೆ_panels = 0;
ಸ್ಥಿರ ಗಿಡ್ಡ pg_margin_ಎಡ, pg_margin_ಬಲ, pg_margin_ತುದಿ, pg_margin_bottom;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು GUI_PH_MARGIN		4	// Size of the bevel

#ವ್ಯಾಖ್ಯಾನಿಸು GUI_PH_MOUSE_TYPE		Ph_CURSOR_ಸೇರಿಸು
ಸ್ಥಿರ PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ಸ್ಥಿರ Ph_ಬಿಂದು_t    gui_ph_raw_ಸ್ಥಾನಾಂತರ;
ಸ್ಥಿರ PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ಸ್ಥಿರ PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ಸ್ಥಿರ ಗಿಡ್ಡ	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ಸ್ಥಿರ Ph_ಬಿಂದು_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
ಸ್ಥಿರ ರಚನೆ PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
ಸ್ಥಿರ ಎಣಿಕೆ {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

ಸ್ಥಿರ long_u	blink_waittime	= 700;
ಸ್ಥಿರ long_u	blink_ontime	= 400;
ಸ್ಥಿರ long_u	blink_offtime	= 250;

ಸ್ಥಿರ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    ಕೀಲಿ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {Pk_ಮೇಲೆ,	    'k', 'u'},
    {Pk_ಕೆಳಕ್ಕೆ,	    'k', 'd'},
    {Pk_ಎಡ,	    'k', 'l'},
    {Pk_ಬಲ,	    'k', 'r'},

    {Pk_F1,	    'k', '1'},
    {Pk_F2,	    'k', '2'},
    {Pk_F3,	    'k', '3'},
    {Pk_F4,	    'k', '4'},
    {Pk_F5,	    'k', '5'},
    {Pk_F6,	    'k', '6'},
    {Pk_F7,	    'k', '7'},
    {Pk_F8,	    'k', '8'},
    {Pk_F9,	    'k', '9'},
    {Pk_F10,	    'k', ';'},

    {Pk_F11,	    'F', '1'},
    {Pk_F12,	    'F', '2'},
    {Pk_F13,	    'F', '3'},
    {Pk_F14,	    'F', '4'},
    {Pk_F15,	    'F', '5'},
    {Pk_F16,	    'F', '6'},
    {Pk_F17,	    'F', '7'},
    {Pk_F18,	    'F', '8'},
    {Pk_F19,	    'F', '9'},
    {Pk_F20,	    'F', 'A'},

    {Pk_F21,	    'F', 'B'},
    {Pk_F22,	    'F', 'C'},
    {Pk_F23,	    'F', 'D'},
    {Pk_F24,	    'F', 'E'},
    {Pk_F25,	    'F', 'F'},
    {Pk_F26,	    'F', 'G'},
    {Pk_F27,	    'F', 'H'},
    {Pk_F28,	    'F', 'I'},
    {Pk_F29,	    'F', 'J'},

    {Pk_F30,	    'F', 'K'},
    {Pk_F31,	    'F', 'L'},
    {Pk_F32,	    'F', 'M'},
    {Pk_F33,	    'F', 'N'},
    {Pk_F34,	    'F', 'O'},
    {Pk_F35,	    'F', 'P'},

    {Pk_Help,	    '%', '1'},
    {Pk_ಹಿಂಭಾಗ_Space,  'k', 'b'},
    {Pk_ಸೇರಿಸು,	    'k', 'I'},
    {Pk_Delete,	    'k', 'D'},
    {Pk_Home,	    'k', 'h'},
    {Pk_ಕೊನೆ,	    '@', '7'},
    {Pk_ಪೂರ್ವ,	    'k', 'P'},
    {Pk_ಮುಂದಿನ,	    'k', 'N'},
    {Pk_Print,	    '%', '9'},

    {Pk_KP_ಕೂಡಿಸು,	    'K', '6'},
    {Pk_KP_ಕಳೆ,'K', '7'},
    {Pk_KP_Divide,  'K', '8'},
    {Pk_KP_Multiply,'K', '9'},
    {Pk_KP_Enter,   'K', 'A'},

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //ಸೇರಿಸು
    {Pk_KP_Decimal, KS_EXTRA, KE_KDEL}, // Delete

    {Pk_KP_4,	    'k', 'l'}, // Left
    {Pk_KP_6,	    'k', 'r'}, // Right
    {Pk_KP_8,	    'k', 'u'}, // Up
    {Pk_KP_2,	    'k', 'd'}, // Down

    {Pk_KP_7,	    'K', '1'}, // Home
    {Pk_KP_1,	    'K', '4'}, //ಕೊನೆ

    {Pk_KP_9,	    'K', '3'}, // Page Up
    {Pk_KP_3,	    'K', '5'}, // Page Down

    {Pk_KP_5,	    '&', '8'}, // Undo

    // Keys that we want to be able to use any modifier with:
    {Pk_Return,	    CAR,  NUL},
    {Pk_space,	    ' ', NUL},
    {Pk_Tab,	    TAB, NUL},
    {Pk_Escape,	    ESC, NUL},
    {NL,	    NL,	 NUL},
    {CAR,	    CAR,  NUL},

    // End of list marker:
    {0,		0, 0}
};


////////////////////////////////////////////////////////////////////////////

ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_timer_cursor;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_timer_timeout;

ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_ಕಿಟಕಿ_cb;

ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_scrollbar;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_keyboard;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_mouse;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_pulldown_menu;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_menu;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_menu_unrealized;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
ಸ್ಥಿರ ಶೂನ್ಯ gui_ph_get_panelgroup_margins(ಗಿಡ್ಡ*, ಗಿಡ್ಡ*, ಗಿಡ್ಡ*, ಗಿಡ್ಡ*);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ gui_ph_draw_ಆರಂಭ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ gui_ph_draw_ಕೊನೆ(ಶೂನ್ಯ);

// Set the text for the balloon
ಸ್ಥಿರ PtWidget_t * gui_ph_show_tooltip(PtWidget_t *ಕಿಟಕಿ_2,
			     PtWidget_t *widget,
			     ಪೂರ್ಣಾಂಕ position,
			     ಅಕ್ಷರ *ಪಠ್ಯ,
			     ಅಕ್ಷರ *font,
			     PgColor_t fill_color,
			     PgColor_t ಪಠ್ಯ_color);

////////////////////////////////////////////////////////////////////////////

ಸ್ಥಿರ PtWidget_t * gui_ph_show_tooltip(PtWidget_t *ಕಿಟಕಿ_2,
			     PtWidget_t *widget,
			     ಪೂರ್ಣಾಂಕ position,
			     ಅಕ್ಷರ *ಪಠ್ಯ,
			     ಅಕ್ಷರ *font,
			     PgColor_t fill_color,
			     PgColor_t ಪಠ್ಯ_color)
{
    PtArg_t arg;
    vimmenu_T *menu;
    char_u  *tooltip;

    Pt_ಗಣ_Arg(&arg, Pt_ARG_ಸೂಚಕ, &menu, 0);
    PtGetResources(widget, 1, &arg);

    // Override the text and position

    tooltip = ಪಠ್ಯ;
    ಆದರೆ (menu != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = MENU_ಸೂಚ್ಯಂಕ_TIP;
	ಆದರೆ (menu->ಅಕ್ಷರಸರಣಿಗಳು[ ಸೂಚ್ಯಂಕ ] != ಖಾಲಿ)
	    tooltip = menu->ಅಕ್ಷರಸರಣಿಗಳು[ ಸೂಚ್ಯಂಕ ];
    }

    ಹಿಂತಿರುಗಿ PtInflateBalloon(
	    ಕಿಟಕಿ_2,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_ತುದಿ,
	    tooltip,
	    font,
	    fill_color,
	    ಪಠ್ಯ_color);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_resize_container(ಶೂನ್ಯ)
{
    PhArea_t area;

    PtWidgetArea(gui.vim_ಕಿಟಕಿ, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    Pt_ಗಣ_Resource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	ಶೂನ್ಯ *other,
	PtCallbackInfo_t *info)
{
    PtContainerCallback_t *sizes = info->cbdata;
    PtWidget_t		*container;
    Ph_ಬಿಂದು_t		ಕೆಳಗೆ_menu;
    int_u		ಎತ್ತರ;

    ಎತ್ತರ = sizes->new_dim.h;

    // Because vim treats the toolbar and menubar separately,
    // and here they're lumped together into a PtToolbarGroup,
    // we only need either menu_height or toolbar_height set at once
    ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
    {
	gui.menu_ಎತ್ತರ = ಎತ್ತರ;
	gui.toolbar_ಎತ್ತರ = 0;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಇಲ್ಲದಿದ್ದರೆ
	gui.toolbar_ಎತ್ತರ = ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

    ಕೆಳಗೆ_menu.x = 0;
    ಕೆಳಗೆ_menu.y = ಎತ್ತರ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
    container = gui.vim_Panel_ಗುಂಪು;
#ಇಲ್ಲದಿದ್ದರೆ
    container = gui.vimContainer;
#ಆದರೆ_ಅಂತ್ಯ

    Pt_ಗಣ_Resource(container, Pt_ARG_POS, &ಕೆಳಗೆ_menu, 0);

    gui_ph_resize_container();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
    gui_ph_get_panelgroup_margins(
	    &pg_margin_ತುದಿ, &pg_margin_bottom,
	    &pg_margin_ಎಡ, &pg_margin_ಬಲ);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	ಶೂನ್ಯ *ದತ್ತಾಂಶ,
	PtCallbackInfo_t *info)
{
    ಆದರೆ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	Pt_ಗಣ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	Pt_ಗಣ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    ಶೂನ್ಯ		*ದತ್ತಾಂಶ,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_ಕಿಟಕಿ_cb(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    Ph_ಕಿಟಕಿ_Event_t *we = info->cbdata;
    ushort_t *ಅಗಲ, *ಎತ್ತರ;

    ಆಯ್ಕೆ (we->event_f)
    {
	ಸ್ಥಿತಿ Ph_WM_CLOSE:
	    gui_shell_closed();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    ಆದರೆ (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_ಆರಂಭ_blink();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ph_WM_RESIZE:
	    PtGetResource(gui.vim_ಕಿಟಕಿ, Pt_ARG_ಅಗಲ, &ಅಗಲ, 0);
	    PtGetResource(gui.vim_ಕಿಟಕಿ, Pt_ARG_ಎತ್ತರ, &ಎತ್ತರ, 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
	    ಅಗಲ  -= (pg_margin_ಎಡ + pg_margin_ಬಲ);
	    ಎತ್ತರ -= (pg_margin_ತುದಿ + pg_margin_bottom);
#ಆದರೆ_ಅಂತ್ಯ
	    gui_resize_shell(*ಅಗಲ, *ಎತ್ತರ);
	    gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    Pt_ಕೊನೆ_Flux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_scrollbar(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    ಪೂರ್ಣಾಂಕ	    ಮೌಲ್ಯ, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) ದತ್ತಾಂಶ;
    ಆದರೆ (sb != ಖಾಲಿ)
    {
	ಮೌಲ್ಯ = scroll->position;
	ಆಯ್ಕೆ (scroll->action)
	{
	    ಸ್ಥಿತಿ Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ Pt_SCROLL_ಗಣ:
		// FIXME: return straight away here?
		ಹಿಂತಿರುಗಿ Pt_CONTINUE;
		ನಿಲ್ಲಿಸು;
	}

	gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
    }
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_keyboard(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    Ph_ಕೀಲಿ_Event_t    *ಕೀಲಿ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ   string[6];
    ಪೂರ್ಣಾಂಕ		    len, i;
    ಪೂರ್ಣಾಂಕ		    ch, modifiers;

    ಕೀಲಿ = Ph_Get_ದತ್ತಾಂಶ(info->event);

    ch = modifiers = len = 0;

    ಆದರೆ (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    ಆದರೆ (ಕೀಲಿ->ಕೀಲಿ_flags & Pk_KF_Compose)
	ಹಿಂತಿರುಗಿ Pt_CONTINUE;

    ಆದರೆ ((ಕೀಲಿ->ಕೀಲಿ_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_ಕೀಲಿ_ಕೆಳಕ್ಕೆ(ಕೀಲಿ->ಕೀಲಿ_flags))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	ಆದರೆ (((ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Alt) &&
		    !(ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_ಸರಿಸು) &&
		    !(ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_ಸಕ್ರಿಯ &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(ಕೀಲಿ->ಕೀಲಿ_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	{
	    ಆದರೆ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == ಕೀಲಿ->ಕೀಲಿ_cap)
	    {
		len = 0;
		ಆದರೆ (special_ಕೀಲಿಗಳು[i].vim_code1 == NUL)
		    ch = special_ಕೀಲಿಗಳು[i].vim_code0;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    ಆದರೆ (ಕೀಲಿ->ಕೀಲಿ_cap >= Pk_KP_Enter && ಕೀಲಿ->ಕೀಲಿ_cap <= Pk_KP_9
			    && (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_ಅಂಕೆ_ಬೀಗ))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = ಕೀಲಿ->ಕೀಲಿ_cap - 0xf080;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ch = ಗಮ್ಯ_SPECIAL(special_ಕೀಲಿಗಳು[i].vim_code0,
				special_ಕೀಲಿಗಳು[i].vim_code1);
		}
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_ಮುಖವಾಡ_CTRL;
	ಆದರೆ (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Alt)
	    modifiers |= MOD_ಮುಖವಾಡ_ALT;
	ಆದರೆ (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_ಸರಿಸು)
	    modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;

	// Is this not a special key?
	ಆದರೆ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == 0)
	{
	    ch = PhTo8859_1(ಕೀಲಿ);
	    ಆದರೆ (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_ಕೀಲಿ_ಗಮ್ಯ_Mb(string, ಕೀಲಿ);
		ಆದರೆ (len > 0)
		{
		    ಸ್ಥಿರ ಅಕ್ಷರ buf[6];
		    ಪೂರ್ಣಾಂಕ src_ತೆಗೆದುಕೊಂಡ, dst_made;
		    ಆದರೆ (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_ತೆಗೆದುಕೊಂಡ,
				buf,
				6,
				&dst_made);

			ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(buf, dst_made);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
		    }

		    ಹಿಂತಿರುಗಿ Pt_CONSUME;
		}
		len = 0;
		ch = ಕೀಲಿ->ಕೀಲಿ_cap;
		ಆದರೆ (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    ಆದರೆ (modifiers & MOD_ಮುಖವಾಡ_CTRL)
		    {
			modifiers &= ~MOD_ಮುಖವಾಡ_CTRL;

			ಆದರೆ ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch == '2')
			    ch = NUL;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch == '6')
			    ch = 0x1e;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch == '-')
			    ch = 0x1f;
			ಇಲ್ಲದಿದ್ದರೆ
			    modifiers |= MOD_ಮುಖವಾಡ_CTRL;
		    }

		    ಆದರೆ (modifiers & MOD_ಮುಖವಾಡ_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_ಮುಖವಾಡ_ALT;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		modifiers &= ~MOD_ಮುಖವಾಡ_ಸರಿಸು;
	}

	ch = simplify_ಕೀಲಿ(ch, &modifiers);
	ಆದರೆ (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	ಆದರೆ (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_ಎರಡನೇ(ch);
	    string[ len++ ] = K_ಮೂರನೇ(ch);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    ಪೂರ್ಣಾಂಕ int_ch = check_for_ಅಡಚಣೆ(ch, modifiers);

	    ಆದರೆ (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_ಒಳಹರಿವು_buf();
	    }
	}

	ಆದರೆ (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	ಆದರೆ (len > 0)
	{
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
	    ಹಿಂತಿರುಗಿ Pt_CONSUME;
	}
    }

    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_mouse(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    Ph_ಸೂಚಕ_Event_t *ಸೂಚಕ;
    PhRect_t	     *pos;
    ಪೂರ್ಣಾಂಕ		     button = 0, repeated_click, modifiers = 0x0;
    ಗಿಡ್ಡ	     mouse_x, mouse_y;

    ಸೂಚಕ = Ph_Get_ದತ್ತಾಂಶ(info->event);
    pos = PhGetRects(info->event);

    gui_mch_mousehide(MOUSE_SHOW);

    /*
     * Coordinates need to be relative to the base window,
     * not relative to the vimTextArea widget
     */
    mouse_x = pos->ul.x + gui.border_ಅಗಲ;
    mouse_y = pos->ul.y + gui.border_ಅಗಲ;

    ಆದರೆ (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	ಹಿಂತಿರುಗಿ Pt_CONTINUE;
    }

    ಆದರೆ (ಸೂಚಕ->ಕೀಲಿ_mods & Pk_KM_ಸರಿಸು)
	modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ (ಸೂಚಕ->ಕೀಲಿ_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    ಆದರೆ (ಸೂಚಕ->ಕೀಲಿ_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    ಆದರೆ (ಸೂಚಕ->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_ಎಡ;

    ಆದರೆ (ಸೂಚಕ->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_ಬಲ;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = ಸೂಚಕ->pos.x;
	abs_mouse.y = ಸೂಚಕ->pos.y;
    }

    ಆದರೆ (ಸೂಚಕ->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_ಮಧ್ಯ;

    // Catch a real release (not phantom or other releases
    ಆದರೆ (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    ಆದರೆ (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#ಆದರೆ 0
    // Vim doesn't use button repeats
    ಆದರೆ (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#ಆದರೆ_ಅಂತ್ಯ

    // Don't do anything if it is one of the phantom mouse release events
    ಆದರೆ ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (ಸೂಚಕ->click_ಎಣಿಕೆ >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_focus(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    ಆದರೆ (info->reason == Pt_CB_ಕಳೆದುಹೋದ_FOCUS)
    {
	PtRemoveEventHandler(gui.vim_ಪಠ್ಯ_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ಖಾಲಿ);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Pt_ಕೂಡಿಸು_Event_Handler(gui.vim_ಪಠ್ಯ_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    Ph_ಬಿಂದು_t	ಸ್ಥಾನಾಂತರ;
    Ph_ಬಿಂದು_t	translation;

    ಆದರೆ (is_ignore_draw == TRUE)
	ಹಿಂತಿರುಗಿ;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#ಆದರೆ 0
    /*
     * This causes some weird problems, with drawing being done from
     * within this raw drawing function (rather than just simple clearing
     * and text drawing done by gui_redraw)
     *
     * The main problem is when PhBlit is used, and the cursor appearing
     * in places where it shouldn't
     */
    ಹೊರಹೋಗು_flush();
#ಆದರೆ_ಅಂತ್ಯ

    Pt_Widget_ಸ್ಥಾನಾಂತರ(widget, &ಸ್ಥಾನಾಂತರ);
    Ph_Translate_ಬಿಂದು(&ಸ್ಥಾನಾಂತರ, PtWidgetPos(gui.vim_ಪಠ್ಯ_Area, ಖಾಲಿ));

#ಆದರೆ 1
    // Redraw individual damage regions
    ಆದರೆ (damage->ಮುಂದಿನ != ಖಾಲಿ)
	damage = damage->ಮುಂದಿನ;

    ಆಗ (damage != ಖಾಲಿ)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - ಸ್ಥಾನಾಂತರ.x, r->ul.y - ಸ್ಥಾನಾಂತರ.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->ಮುಂದಿನ;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - ಸ್ಥಾನಾಂತರ.x, r->ul.y - ಸ್ಥಾನಾಂತರ.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#ಆದರೆ_ಅಂತ್ಯ

    Pg_ಗಣ_Translation(&translation, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	ಶೂನ್ಯ *ದತ್ತಾಂಶ,
	PtCallbackInfo_t *info)
{
    ಆದರೆ (ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	vimmenu_T *menu = (vimmenu_T *) ದತ್ತಾಂಶ;

	PtPositionMenu(menu->submenu_id, ಖಾಲಿ);
	PtRealizeWidget(menu->submenu_id);
    }

    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_menu(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    ಆದರೆ (ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	vimmenu_T *menu = (vimmenu_T *) ದತ್ತಾಂಶ;
	gui_menu_cb(menu);
    }
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	ಶೂನ್ಯ *ದತ್ತಾಂಶ,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vim_ಪಠ್ಯ_Area, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_ಕಿಟಕಿ_open(
	PtWidget_t *widget,
	ಶೂನ್ಯ *ದತ್ತಾಂಶ,
	PtCallbackInfo_t *info)
{
    gui_ಗಣ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_ಆರಂಭ  gui_ph_draw_ಆರಂಭ()
#ವ್ಯಾಖ್ಯಾನಿಸು DRAW_ಕೊನೆ    gui_ph_draw_ಕೊನೆ()

/*
 * TODO: Set a clipping rect?
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_draw_ಆರಂಭ(ಶೂನ್ಯ)
{
    PhGC_t *gc;

    gc = PgGetGC();
    Pg_ಗಣ_Region(PtWidgetRid(Pt_ಕಂಡುಹಿಡಿ_Disjoint(gui.vim_ಪಠ್ಯ_Area)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    Pt_Widget_ಸ್ಥಾನಾಂತರ(gui.vim_ಪಠ್ಯ_Area, &gui_ph_raw_ಸ್ಥಾನಾಂತರ);
    Ph_Translate_ಬಿಂದು(&gui_ph_raw_ಸ್ಥಾನಾಂತರ, PtWidgetPos(gui.vim_ಪಠ್ಯ_Area, ಖಾಲಿ));

    Pg_ಗಣ_Translation(&gui_ph_raw_ಸ್ಥಾನಾಂತರ, Pg_RELATIVE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_draw_ಕೊನೆ(ಶೂನ್ಯ)
{
    gui_ph_raw_ಸ್ಥಾನಾಂತರ.x = -gui_ph_raw_ಸ್ಥಾನಾಂತರ.x;
    gui_ph_raw_ಸ್ಥಾನಾಂತರ.y = -gui_ph_raw_ಸ್ಥಾನಾಂತರ.y;
    Pg_ಗಣ_Translation(&gui_ph_raw_ಸ್ಥಾನಾಂತರ, Pg_RELATIVE);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
    ಸ್ಥಿರ vimmenu_T *
gui_ph_ಕಂಡುಹಿಡಿ_ಬಫರ್_ಅಂಶ(char_u *name)
{
    vimmenu_T *ತುದಿ_level = ಬೇರು_menu;
    vimmenu_T *ವಸ್ತುಗಳು = ಖಾಲಿ;

    ಆಗ (ತುದಿ_level != ಖಾಲಿ &&
	    (STRCMP(ತುದಿ_level->dname, "Buffers") != 0))
	ತುದಿ_level = ತುದಿ_level->ಮುಂದಿನ;

    ಆದರೆ (ತುದಿ_level != ಖಾಲಿ)
    {
	ವಸ್ತುಗಳು = ತುದಿ_level->children;

	ಆಗ (ವಸ್ತುಗಳು != ಖಾಲಿ &&
		(STRCMP(ವಸ್ತುಗಳು->dname, name) != 0))
	    ವಸ್ತುಗಳು = ವಸ್ತುಗಳು->ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ ವಸ್ತುಗಳು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_pg_ಗಣ_ಬಫರ್_ಅಂಕೆ(int_u buf_ಅಂಕೆ)
{
    ಪೂರ್ಣಾಂಕ i;
    ಅಕ್ಷರ ಹುಡುಕು[16];
    ಅಕ್ಷರ *mark;

    ಆದರೆ (gui.vim_ಪಠ್ಯ_Area == ಖಾಲಿ || buf_ಅಂಕೆ == 0)
	ಹಿಂತಿರುಗಿ;

    ಹುಡುಕು[0] = '(';
    ultoa(buf_ಅಂಕೆ, &ಹುಡುಕು[1], 10);
    STRCAT(ಹುಡುಕು, ")");

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	ಆದರೆ (mark != ಖಾಲಿ && STRCMP(mark, ಹುಡುಕು) == 0)
	{
	    Pt_ಗಣ_Resource(gui.vim_Panel_ಗುಂಪು, Pt_ARG_PG_CURRENT_ಸೂಚ್ಯಂಕ,
		    i, 0);
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	ಶೂನ್ಯ *ದತ್ತಾಂಶ,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    Pt_Panel_ಗುಂಪು_Callback_t *panel;

    ಆದರೆ (info->event != ಖಾಲಿ)
    {
	panel = info->cbdata;
	ಆದರೆ (panel->new_panel != ಖಾಲಿ)
	{
	    menu = gui_ph_ಕಂಡುಹಿಡಿ_ಬಫರ್_ಅಂಶ(panel->new_panel);
	    ಆದರೆ (menu)
		gui_menu_cb(menu);
	}
    }
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_get_panelgroup_margins(
	ಗಿಡ್ಡ *ತುದಿ,
	ಗಿಡ್ಡ *bottom,
	ಗಿಡ್ಡ *ಎಡ_2,
	ಗಿಡ್ಡ *ಬಲ_2)
{
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *margin_ತುದಿ, *margin_bottom;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *margin_ಎಡ, *margin_ಬಲ;

    PtGetAbsPosition(gui.vim_ಪಠ್ಯ_Area, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vim_Panel_ಗುಂಪು, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vim_Panel_ಗುಂಪು, Pt_ARG_MARGIN_ಬಲ, &margin_ಬಲ, 0);
    PtGetResource(gui.vim_Panel_ಗುಂಪು, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *ತುದಿ    = abs_raw_y;
    *bottom = *margin_bottom;

    *ಎಡ_2  = abs_raw_x;
    *ಬಲ_2 = *margin_ಬಲ;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_is_ಬಫರ್_ಅಂಶ(vimmenu_T *menu, vimmenu_T *ಪೋಷಕ)
{
    ಅಕ್ಷರ *mark;

    ಆದರೆ (STRCMP(ಪೋಷಕ->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	ಆದರೆ (mark != ಖಾಲಿ)
	{
	    mark++;
	    ಆಗ (ಸುರಕ್ಷಿತ_isdigit(*mark))
		mark++;

	    ಆದರೆ (*mark == ')')
		ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_pg_ಕೂಡಿಸು_ಬಫರ್(ಅಕ್ಷರ *name)
{
    ಅಕ್ಷರ **new_titles = ಖಾಲಿ;

    new_titles = ALLOC_MULT(ಅಕ್ಷರ *, (ಅಂಕೆ_panels + 1));
    ಆದರೆ (new_titles == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಅಂಕೆ_panels > 0)
	ಸ್ಮರಣೆ_ನಕಲು(new_titles, panel_titles, ಅಂಕೆ_panels * ಗಾತ್ರ(ಅಕ್ಷರ **));

    new_titles[ ಅಂಕೆ_panels++ ] = name;

    Pt_ಗಣ_Resource(gui.vim_Panel_ಗುಂಪು, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    ಅಂಕೆ_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_pg_remove_ಬಫರ್(ಅಕ್ಷರ *name)
{
    ಪೂರ್ಣಾಂಕ i;
    ಅಕ್ಷರ **new_titles = ಖಾಲಿ;

    // If there is only 1 panel, we just use the temporary place holder
    ಆದರೆ (ಅಂಕೆ_panels > 1)
    {
	new_titles = ALLOC_MULT(ಅಕ್ಷರ *, ಅಂಕೆ_panels - 1);
	ಆದರೆ (new_titles != ಖಾಲಿ)
	{
	    ಅಕ್ಷರ **s = new_titles;
	    // Copy all the titles except the one we're removing
	    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_panels; i++)
	    {
		ಆದರೆ (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    ಅಂಕೆ_panels--;

	    Pt_ಗಣ_Resource(gui.vim_Panel_ಗುಂಪು, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    ಅಂಕೆ_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಂಕೆ_panels--;
	Pt_ಗಣ_Resource(gui.vim_Panel_ಗುಂಪು, Pt_ARG_PG_PANEL_TITLES, &ಖಾಲಿಗಳು_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_handle_ಬಫರ್_remove(
	PtWidget_t *widget,
	ಶೂನ್ಯ *ದತ್ತಾಂಶ,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    ಆದರೆ (ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	menu = (vimmenu_T *) ದತ್ತಾಂಶ;
	gui_ph_pg_remove_ಬಫರ್(menu->dname);
    }

    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_pane_resize(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    ಆದರೆ (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	Pt_ಆರಂಭ_Flux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    ಶೂನ್ಯ
gui_ph_encoding_ಬದಲಾಯಿತು(ಪೂರ್ಣಾಂಕ new_encoding)
{
    // Default encoding is latin1
    ಅಕ್ಷರ *charset = "latin1";
    ಪೂರ್ಣಾಂಕ i;

    ರಚನೆ {
	ಪೂರ್ಣಾಂಕ encoding;
	ಅಕ್ಷರ *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    ಗಾಗಿ (i = 0; i < ARRAY_ಉದ್ದಳತೆ(charsets); i++)
    {
	ಆದರೆ (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = Px_Translate_ಗಣ(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಂಕ *argc, ಅಕ್ಷರ **argv)
{
    PtInit(ಖಾಲಿ);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_init(ಶೂನ್ಯ)
{
    PtArg_t ವಾದಗಳು[10];
    ಪೂರ್ಣಾಂಕ	    flags = 0, n = 0;

    PhDim_t	ಕಿಟಕಿ_ಗಾತ್ರ = {100, 100}; // Arbitrary values
    Ph_ಬಿಂದು_t	pos = {0, 0};

    gui.event_ಬಫರ್ = alloc(EVENT_ಬಫರ್_ಗಾತ್ರ);
    ಆದರೆ (gui.event_ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = Px_Translate_ಗಣ(ಖಾಲಿ, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_ಸ್ಥಾನಾಂತರ = gui.border_ಅಗಲ = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಕಿಟಕಿ_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಕಿಟಕಿ_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_DIM, &ಕಿಟಕಿ_ಗಾತ್ರ, 0);
    gui.vim_ಕಿಟಕಿ = PtCreateWidget(Pt_ಕಿಟಕಿ, ಖಾಲಿ, n, ವಾದಗಳು);
    ಆದರೆ (gui.vim_ಕಿಟಕಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    Pt_ಕೂಡಿಸು_Callback(gui.vim_ಕಿಟಕಿ, Pt_CB_ಕಿಟಕಿ, gui_ph_handle_ಕಿಟಕಿ_cb, ಖಾಲಿ);
    Pt_ಕೂಡಿಸು_Callback(gui.vim_ಕಿಟಕಿ, Pt_CB_ಕಿಟಕಿ_OPENING,
	    gui_ph_handle_ಕಿಟಕಿ_open, ಖಾಲಿ);

    n = 0;
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_DIM, &ಕಿಟಕಿ_ಗಾತ್ರ, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_POS, &pos, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
    // Put in a temporary place holder title
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_PG_PANEL_TITLES, &ಖಾಲಿಗಳು_title, 1);

    gui.vim_Panel_ಗುಂಪು = PtCreateWidget(Pt_Panel_ಗುಂಪು, gui.vim_ಕಿಟಕಿ, n, ವಾದಗಳು);
    ಆದರೆ (gui.vim_Panel_ಗುಂಪು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    Pt_ಕೂಡಿಸು_Callback(gui.vim_Panel_ಗುಂಪು, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    // Turn off all edge decorations
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_BEVEL_ಅಗಲ, 0, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_MARGIN_ಅಗಲ, 0, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_MARGIN_ಎತ್ತರ, 0, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vim_ಕಿಟಕಿ, n, ವಾದಗಳು);
    ಆದರೆ (gui.vimContainer == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    Pt_ಕೂಡಿಸು_Callback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    // Size for the text area is set in gui_mch_set_text_area_pos
    n = 0;

    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_RAW_DRAW_F, gui_ph_handle_raw_draw, 1);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_BEVEL_ಅಗಲ, GUI_PH_MARGIN, 0);
    /*
     * Using focus render also causes the whole widget to be redrawn
     * whenever it changes focus, which is very annoying :p
     */
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_FLAGS, Pt_TRUE,
	    Pt_GETS_FOCUS | Pt_HIGHLIGHTED);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_MOUSESHAPE
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_CURSOR_TYPE, GUI_PH_MOUSE_TYPE, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_CURSOR_COLOR, gui_ph_mouse_color, 0);
#ಆದರೆ_ಅಂತ್ಯ

    gui.vim_ಪಠ್ಯ_Area = PtCreateWidget(PtRaw, Pt_DFLT_ಪೋಷಕ, n, ವಾದಗಳು);
    ಆದರೆ (gui.vim_ಪಠ್ಯ_Area == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    Pt_ಕೂಡಿಸು_Event_Handler(gui.vim_ಪಠ್ಯ_Area,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, ಖಾಲಿ);
    Pt_ಕೂಡಿಸು_Event_Handler(gui.vim_ಪಠ್ಯ_Area, Ph_EV_ಕೀಲಿ,
	    gui_ph_handle_keyboard, ಖಾಲಿ);
    Pt_ಕೂಡಿಸು_Callback(gui.vim_ಪಠ್ಯ_Area, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, ಖಾಲಿ);
    Pt_ಕೂಡಿಸು_Callback(gui.vim_ಪಠ್ಯ_Area, Pt_CB_ಕಳೆದುಹೋದ_FOCUS,
	    gui_ph_handle_focus, ಖಾಲಿ);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vim_ಕಿಟಕಿ, 0, ಖಾಲಿ);
    ಆದರೆ (gui_ph_timer_cursor == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vim_ಕಿಟಕಿ, 0, ಖಾಲಿ);
    ಆದರೆ (gui_ph_timer_timeout == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    Pt_ಕೂಡಿಸು_Callback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, ಖಾಲಿ);
    Pt_ಕೂಡಿಸು_Callback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    n = 0;
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಅಗಲ, ಕಿಟಕಿ_ಗಾತ್ರ.w, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ಎಡ_ಬಲ,
	    Pt_IS_ANCHORED);
    gui.vim_Tool_Bar_ಗುಂಪು = PtCreateWidget(Pt_Toolbar_ಗುಂಪು, gui.vim_ಕಿಟಕಿ,
	    n, ವಾದಗಳು);
    ಆದರೆ (gui.vim_Tool_Bar_ಗುಂಪು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    Pt_ಕೂಡಿಸು_Callback(gui.vim_Tool_Bar_ಗುಂಪು, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, ಖಾಲಿ);

    n = 0;
    flags = 0;
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಅಗಲ, ಕಿಟಕಿ_ಗಾತ್ರ.w, 0);
    ಆದರೆ (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vim_Tool_Bar_ಗುಂಪು, n, ವಾದಗಳು);
    ಆದರೆ (gui.vimMenuBar == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_ಎಡ_ಬಲ |Pt_ತುದಿ_ANCHORED_ತುದಿ, Pt_IS_ANCHORED);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಅಗಲ, ಕಿಟಕಿ_ಗಾತ್ರ.w, 0);

    flags = Pt_GETS_FOCUS;
    ಆದರೆ (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vim_Tool_Bar_ಗುಂಪು, n, ವಾದಗಳು);
    ಆದರೆ (gui.vimToolBar == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (is_photon_ಲಭ್ಯ == TRUE) ? OK : FAIL;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_open(ಶೂನ್ಯ)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.ಹಿಂಭಾಗ_pixel =  Pg_WHITE;

    ಗಣ_ಸಾಮಾನ್ಯ_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_ಹಿಂಭಾಗ_pixel = gui.ಹಿಂಭಾಗ_pixel;

    highlight_gui_started();

    ಆದರೆ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_ಗಣ_winpos(gui_win_x, gui_win_y);

    ಹಿಂತಿರುಗಿ (PtRealizeWidget(gui.vim_ಕಿಟಕಿ) == 0) ? OK : FAIL;
}

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಂಕ rc)
{
    PtDestroyWidget(gui.vim_ಕಿಟಕಿ);

    Px_Translate_ಗಣ(charset_translate, ಖಾಲಿ);

    vim_free(gui.event_ಬಫರ್);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_GROUPS
    vim_free(panel_titles);
#ಆದರೆ_ಅಂತ್ಯ
}

////////////////////////////////////////////////////////////////////////////
// events

/*
 * When no events are available, photon will call this function, working is
 * set to FALSE, and the gui_mch_update loop will exit.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exit_gui_mch_update(ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    *(ಪೂರ್ಣಾಂಕ *)ದತ್ತಾಂಶ = FALSE;
    ಹಿಂತಿರುಗಿ Pt_ಕೊನೆ;
}

    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ working = TRUE;

    Pt_App_ಕೂಡಿಸು_Work_Proc(ಖಾಲಿ, exit_gui_mch_update, &working);
    ಆಗ ((working == TRUE) && !vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ())
	PtProcessEvent();
}

    ಪೂರ್ಣಾಂಕ
gui_mch_wait_for_chars(ದೀರ್ಘ wtime)
{
    is_timeout = FALSE;

    ಆದರೆ (wtime >= 0)
	Pt_ಗಣ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    ಆಗ (1)
    {
	PtProcessEvent();
	ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	{
	    Pt_ಗಣ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    ಹಿಂತಿರುಗಿ OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_timeout == TRUE)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
}

#ಆದರೆ defined(FEAT_BROWSE)
/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 * saving,	    select file to write
 * title	    title for the window
 * default_name	    default name (well duh!)
 * ext		    not used (extension added)
 * initdir	    initial directory, NULL for current dir
 * filter	    not used (file name filter)
 */
    char_u *
gui_mch_browse(
	ಪೂರ್ಣಾಂಕ saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    ಪೂರ್ಣಾಂಕ	    flags;
    char_u  *default_ಮಾರ್ಗ;
    char_u  *open_ಪಠ್ಯ = ಖಾಲಿ;

    flags = 0;
    CLEAR_ಕ್ಷೇತ್ರ(file);

    default_ಮಾರ್ಗ = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    ಆದರೆ (default_ಮಾರ್ಗ != ಖಾಲಿ)
    {
	ಆದರೆ (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_ಪಠ್ಯ = "&Save";
	}

	// combine the directory and filename into a single path
	ಆದರೆ (initdir == ಖಾಲಿ || *initdir == NUL)
	{
	    mch_dirname(default_ಮಾರ್ಗ, MAXPATHL);
	    initdir = default_ಮಾರ್ಗ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    STRCPY(default_ಮಾರ್ಗ, initdir);
	    initdir = default_ಮಾರ್ಗ;
	}

	ಆದರೆ (default_name != ಖಾಲಿ)
	{
	    ಆದರೆ (default_ಮಾರ್ಗ[ STRLEN(default_ಮಾರ್ಗ) - 1 ] != '/')
		STRCAT(default_ಮಾರ್ಗ, "/");

	    STRCAT(default_ಮಾರ್ಗ, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vim_ಕಿಟಕಿ,
		ಖಾಲಿ,
		title,
		default_ಮಾರ್ಗ,
		ಖಾಲಿ,
		open_ಪಠ್ಯ,
		ಖಾಲಿ,
		ಖಾಲಿ,
		&file,
		flags);

	vim_free(default_ಮಾರ್ಗ);

	ಆದರೆ (file.ret == Pt_FSDIALOG_BTN1)
	    ಹಿಂತಿರುಗಿ vim_strsave(file.ಮಾರ್ಗ);
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_DIALOG)
ಸ್ಥಿರ PtWidget_t *gui_ph_dialog_ಪಠ್ಯ = ಖಾಲಿ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_dialog_close(ಪೂರ್ಣಾಂಕ button, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    PtModalCtrl_t *modal_ctrl = ದತ್ತಾಂಶ;
    char_u *dialog_ಪಠ್ಯ, *vim_ಪಠ್ಯ;

    ಆದರೆ (gui_ph_dialog_ಪಠ್ಯ != ಖಾಲಿ)
    {
	PtGetResource(gui_ph_dialog_ಪಠ್ಯ, Pt_ARG_ಪಠ್ಯ_STRING, &dialog_ಪಠ್ಯ, 0);
	PtGetResource(gui_ph_dialog_ಪಠ್ಯ, Pt_ARG_ಸೂಚಕ, &vim_ಪಠ್ಯ, 0);
	STRNCPY(vim_ಪಠ್ಯ, dialog_ಪಠ್ಯ, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (ಶೂನ್ಯ *) button);

    ಹಿಂತಿರುಗಿ Pt_TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_dialog_ಪಠ್ಯ_enter(
    PtWidget_t		*widget,
    ಶೂನ್ಯ		*ದತ್ತಾಂಶ,
    PtCallbackInfo_t	*info)
{
    ಆದರೆ (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_dialog_esc(PtWidget_t *widget, ಶೂನ್ಯ *ದತ್ತಾಂಶ, PtCallbackInfo_t *info)
{
    Ph_ಕೀಲಿ_Event_t *ಕೀಲಿ;

    ಕೀಲಿ = Ph_Get_ದತ್ತಾಂಶ(info->event);
    ಆದರೆ ((ಕೀಲಿ->ಕೀಲಿ_flags & Pk_KF_Cap_Valid) && (ಕೀಲಿ->ಕೀಲಿ_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ Pt_CONSUME;
    }
    ಹಿಂತಿರುಗಿ Pt_PROCESS;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_dialog(
	ಪೂರ್ಣಾಂಕ	type,
	char_u	*title,
	char_u	*ಸಂದೇಶ_2,
	char_u	*buttons,
	ಪೂರ್ಣಾಂಕ	default_button,
	char_u	*textfield,
	ಪೂರ್ಣಾಂಕ	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    ಪೂರ್ಣಾಂಕ		button_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		i, len;
    ಪೂರ್ಣಾಂಕ		dialog_ಫಲಿತಾಂಶ = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_ಎಣಿಕೆ = len = i = 0;

    ಆದರೆ (buttons == ಖಾಲಿ || *buttons == NUL)
	ಹಿಂತಿರುಗಿ -1;

    // There is one less separator than buttons, so bump up the button count
    button_ಎಣಿಕೆ = 1;

    // Count string length and number of separators
    ಗಾಗಿ (str = buttons; *str; str++)
    {
	len++;
	ಆದರೆ (*str == DLG_BUTTON_SEP)
	    button_ಎಣಿಕೆ++;
    }

    ಆದರೆ (title == ಖಾಲಿ)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_ಎಣಿಕೆ);
    ಆದರೆ (buttons_copy != ಖಾಲಿ && button_array != ಖಾಲಿ)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	ಗಾಗಿ (i = 0; i < button_ಎಣಿಕೆ; i++)
	{
	    button_array[ i ] = str;
	    ಗಾಗಿ (; *str; str++)
	    {
		ಆದರೆ (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_ಫಲಿತಾಂಶ = PtAlert(
		gui.vim_ಕಿಟಕಿ, ಖಾಲಿ,
		title,
		ಖಾಲಿ,
		ಸಂದೇಶ_2, ಖಾಲಿ,
		button_ಎಣಿಕೆ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **) button_array, ಖಾಲಿ,
		default_button, 0, Pt_MODAL);
# ಇಲ್ಲದಿದ್ದರೆ
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    ಪೂರ್ಣಾಂಕ n;
	    PtArg_t ವಾದಗಳು[5];
	    PtWidget_t *dialog, *pane;
	    PtModalCtrl_t modal_ctrl;
	    PtDialogInfo_t di;

	    CLEAR_ಕ್ಷೇತ್ರ(di);
	    CLEAR_ಕ್ಷೇತ್ರ(modal_ctrl);

	    n = 0;
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_ಗುಂಪು_ಅಡ್ಡಸಾಲುಗಳು_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, 0, 0);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_ಅಗಲ, 350, 0);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_ಗುಂಪು_ORIENTATION,
		    Pt_ಗುಂಪು_VERTICAL, 0);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_ಗುಂಪು_FLAGS,
		    Pt_TRUE, Pt_ಗುಂಪು_NO_ಕೀಲಿಗಳು | Pt_ಗುಂಪು_STRETCH_HORIZONTAL);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(Pt_ಗುಂಪು, ಖಾಲಿ, n, ವಾದಗಳು);

	    n = 0;
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_ಪಠ್ಯ_STRING, ಸಂದೇಶ_2, 0);
	    PtCreateWidget(PtLabel, pane, n, ವಾದಗಳು);

	    ಆದರೆ (textfield != ಖಾಲಿ)
	    {
		n = 0;
		Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_MAX_ಉದ್ದಳತೆ, IOSIZE - 1, 0);
		Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_ಪಠ್ಯ_STRING, textfield, 0);
		Pt_ಗಣ_Arg(&ವಾದಗಳು[n++], Pt_ARG_ಸೂಚಕ, textfield, 0);
		gui_ph_dialog_ಪಠ್ಯ = PtCreateWidget(Pt_ಪಠ್ಯ, pane, n, ವಾದಗಳು);
		Pt_ಕೂಡಿಸು_Callback(gui_ph_dialog_ಪಠ್ಯ, Pt_CB_ACTIVATE,
			gui_ph_dialog_ಪಠ್ಯ_enter, &modal_ctrl);
	    }

	    di.ಪೋಷಕ = gui.vim_ಕಿಟಕಿ;
	    di.pane = pane;
	    di.title = title;
	    di.buttons = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **) button_array;
	    di.nbtns = button_ಎಣಿಕೆ;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_ಎಣಿಕೆ;
	    di.callback = gui_ph_dialog_close;
	    di.ದತ್ತಾಂಶ = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    Pt_ಕೂಡಿಸು_Filter_Callback(dialog, Ph_EV_ಕೀಲಿ,
		    gui_ph_dialog_esc, &modal_ctrl);

	    ಆದರೆ (gui_ph_dialog_ಪಠ್ಯ != ಖಾಲಿ)
		PtGiveFocus(gui_ph_dialog_ಪಠ್ಯ, ಖಾಲಿ);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_ಫಲಿತಾಂಶ = (ಪೂರ್ಣಾಂಕ) Pt_Modal_ಖಂಡ(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_ಪಠ್ಯ = ಖಾಲಿ;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    ಹಿಂತಿರುಗಿ dialog_ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    ಪೂರ್ಣಾಂಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    Ph_ಬಿಂದು_t *pos;

    pos = PtWidgetPos(gui.vim_ಕಿಟಕಿ, ಖಾಲಿ);

    *x = pos->x;
    *y = pos->y;

    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    Ph_ಬಿಂದು_t pos = { x, y };

    Pt_ಗಣ_Resource(gui.vim_ಕಿಟಕಿ, Pt_ARG_POS, &pos, 0);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_shellsize(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ min_ಅಗಲ, ಪೂರ್ಣಾಂಕ min_ಎತ್ತರ, ಪೂರ್ಣಾಂಕ ತಳ_ಅಗಲ, ಪೂರ್ಣಾಂಕ ತಳ_ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ direction)
{
    PhDim_t ಕಿಟಕಿ_ಗಾತ್ರ = { ಅಗಲ, ಎತ್ತರ };
    PhDim_t min_ಗಾತ್ರ = { min_ಅಗಲ, min_ಎತ್ತರ };

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
    ಕಿಟಕಿ_ಗಾತ್ರ.w += pg_margin_ಎಡ + pg_margin_ಬಲ;
    ಕಿಟಕಿ_ಗಾತ್ರ.h += pg_margin_ತುದಿ + pg_margin_bottom;
#ಆದರೆ_ಅಂತ್ಯ

    Pt_ಗಣ_Resource(gui.vim_ಕಿಟಕಿ, Pt_ARG_MINIMUM_DIM, &min_ಗಾತ್ರ, 0);
    Pt_ಗಣ_Resource(gui.vim_ಕಿಟಕಿ, Pt_ARG_DIM, &ಕಿಟಕಿ_ಗಾತ್ರ, 0);

    ಆದರೆ (! PtWidgetIsRealized(gui.vim_ಕಿಟಕಿ))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(ಪೂರ್ಣಾಂಕ *screen_w, ಪೂರ್ಣಾಂಕ *screen_h)
{
    PhRect_t console;

    Ph_ಕಿಟಕಿ_Query_Visible(Ph_QUERY_WORKSPACE, 0,
	    Ph_ಒಳಹರಿವು_ಗುಂಪು(ಖಾಲಿ), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    Ph_ಕಿಟಕಿ_Event_t event;

    CLEAR_ಕ್ಷೇತ್ರ(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vim_ಕಿಟಕಿ);
    Pt_Forward_ಕಿಟಕಿ_Event(&event);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_foreground(ಶೂನ್ಯ)
{
    Ph_ಕಿಟಕಿ_Event_t event;

    CLEAR_ಕ್ಷೇತ್ರ(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vim_ಕಿಟಕಿ);
    Pt_Forward_ಕಿಟಕಿ_Event(&event);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
    gui_ph_pg_ಗಣ_ಬಫರ್_ಅಂಕೆ(curwin->w_ಬಫರ್->b_fnum);
#ಆದರೆ_ಅಂತ್ಯ
    Pt_ಗಣ_Resource(gui.vim_ಕಿಟಕಿ, Pt_ARG_ಕಿಟಕಿ_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_thumb(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ val, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ max)
{
    ಪೂರ್ಣಾಂಕ	    n = 0;
    PtArg_t ವಾದಗಳು[3];

    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಗರಿಷ್ಠ, max, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_SLIDER_ಗಾತ್ರ, ಗಾತ್ರ_2, 0);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_GAUGE_ಮೌಲ್ಯ, val, 0);
    Pt_ಗಣ_Resources(sb->id, n, ವಾದಗಳು);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_pos(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    Pt_ಗಣ_Resource(sb->id, Pt_ARG_AREA, &area, 0);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಹಿಂತಿರುಗಿ 0;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಹಿಂತಿರುಗಿ 0;
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ orient)
{
    ಪೂರ್ಣಾಂಕ	    n = 0;
//    int	    anchor_flags = 0;
    PtArg_t ವಾದಗಳು[4];

    /*
     * Stop the scrollbar from being realized when the parent
     * is realized, so it can be explicitly realized by vim.
     *
     * Also, don't let the scrollbar get focus
     */
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE,
	    Pt_DELAY_REALIZE | Pt_GETS_FOCUS);
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_SCROLLBAR_FLAGS, Pt_SCROLLBAR_SHOW_ARROWS, 0);
#ಆದರೆ 0
    // Don't need this anchoring for the scrollbars
    ಆದರೆ (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_ಎಡ_ANCHORED_ಎಡ | Pt_ಬಲ_ANCHORED_ಬಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_ತುದಿ_ANCHORED_ತುದಿ;
	ಆದರೆ (sb->wp != ಖಾಲಿ)
	{
	    ಆದರೆ (sb == &sb->wp->w_scrollbars[ SBAR_ಎಡ ])
		anchor_flags |= Pt_ಎಡ_ANCHORED_ಎಡ;
	    ಇಲ್ಲದಿದ್ದರೆ
		anchor_flags |= Pt_ಬಲ_ANCHORED_ಬಲ;
	}
    }
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#ಆದರೆ_ಅಂತ್ಯ
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PANEL_ಗುಂಪು
    sb->id = PtCreateWidget(PtScrollbar, gui.vim_Panel_ಗುಂಪು, n, ವಾದಗಳು);
#ಇಲ್ಲದಿದ್ದರೆ
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, ವಾದಗಳು);
#ಆದರೆ_ಅಂತ್ಯ

    Pt_ಕೂಡಿಸು_Callback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    ಶೂನ್ಯ
gui_mch_enable_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ flag)
{
    ಆದರೆ (flag != 0)
	PtRealizeWidget(sb->id);
    ಇಲ್ಲದಿದ್ದರೆ
	PtUnrealizeWidget(sb->id);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = ಖಾಲಿ;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#ಆದರೆ defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mshape_ids[] =
{
    Ph_CURSOR_ಸೂಚಕ,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_ಸೇರಿಸು,		// beam
    Ph_CURSOR_DRAG_VERTICAL,	// updown
    Ph_CURSOR_DRAG_VERTICAL,	// udsizing
    Ph_CURSOR_DRAG_HORIZONTAL,	// leftright
    Ph_CURSOR_DRAG_HORIZONTAL,	// lrsizing
    Ph_CURSOR_WAIT,		// busy
    Ph_CURSOR_DONT,		// no
    Ph_CURSOR_CROSSHAIR,	// crosshair
    Ph_CURSOR_FINGER,		// hand1
    Ph_CURSOR_FINGER,		// hand2
    Ph_CURSOR_FINGER,		// pencil
    Ph_CURSOR_QUESTION_ಬಿಂದು,	// question
    Ph_CURSOR_ಸೂಚಕ,		// right-arrow
    Ph_CURSOR_ಸೂಚಕ,		// up-arrow
    Ph_CURSOR_ಸೂಚಕ		// last one
};

    ಶೂನ್ಯ
mch_ಗಣ_mouse_shape(ಪೂರ್ಣಾಂಕ shape)
{
    ಪೂರ್ಣಾಂಕ	    id;

    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (shape == MSHAPE_HIDE || gui.ಸೂಚಕ_hidden)
	Pt_ಗಣ_Resource(gui.vim_ಪಠ್ಯ_Area, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_ಸೂಚಕ;
	ಇಲ್ಲದಿದ್ದರೆ
	    id = mshape_ids[shape];

	Pt_ಗಣ_Resource(gui.vim_ಪಠ್ಯ_Area, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    ಆದರೆ (shape != MSHAPE_HIDE)
	ಕೊನೆಯ_shape = shape;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಂಕ hide)
{
    ಆದರೆ (gui.ಸೂಚಕ_hidden == hide)
	ಹಿಂತಿರುಗಿ;

    gui.ಸೂಚಕ_hidden = hide;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
    ಆದರೆ (hide)
	Pt_ಗಣ_Resource(gui.vim_ಪಠ್ಯ_Area, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    ಇಲ್ಲದಿದ್ದರೆ
	mch_ಗಣ_mouse_shape(ಕೊನೆಯ_shape);
#ಇಲ್ಲದಿದ್ದರೆ
    Pt_ಗಣ_Resource(gui.vim_ಪಠ್ಯ_Area, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    PhCursorInfo_t info;
    ಗಿಡ್ಡ ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_ಒಳಹರಿವು_ಗುಂಪು(ಖಾಲಿ), &info);
    PtGetAbsPosition(gui.vim_ಪಠ್ಯ_Area , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    ಗಿಡ್ಡ abs_x, abs_y;

    PtGetAbsPosition(gui.vim_ಪಠ್ಯ_Area, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_ಒಳಹರಿವು_ಗುಂಪು(ಖಾಲಿ), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ಹಿಂತಿರುಗಿ (guicolor_T)(PgRGB(Pg_Red_ಮೌಲ್ಯ(pixel),
				     Pg_Green_ಮೌಲ್ಯ(pixel), Pg_Blue_ಮೌಲ್ಯ(pixel)));
}

    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
#ಆದರೆ 0 // Don't bother changing the cursor colour
    ಗಿಡ್ಡ color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.ಹಿಂಭಾಗ_pixel);

    ಆದರೆ (ನಿರಪೇಕ್ಷ(color_diff) < 64)
    {
	ಗಿಡ್ಡ r, g, b;
	// not a great algorithm...
	r = Pg_Red_ಮೌಲ್ಯ(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_ಮೌಲ್ಯ(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_ಮೌಲ್ಯ(gui_ph_mouse_color) ^ 255;

# ifndef FEAT_MOUSESHAPE
	gui_ph_mouse_color = PgRGB(r, g, b);
	Pt_ಗಣ_Resource(gui.vim_ಪಠ್ಯ_Area, Pt_ARG_CURSOR_COLOR,
		gui_ph_mouse_color, 0);
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ

    Pt_ಗಣ_Resource(gui.vim_ಪಠ್ಯ_Area, Pt_ARG_FILL_COLOR, gui.ಹಿಂಭಾಗ_pixel, 0);
}

/*
 * This should be split out into a separate file,
 * every port does basically the same thing.
 *
 * This is the gui_w32.c version (i think..)
 * Return INVALCOLOR when failed.
 */

    guicolor_T
gui_mch_get_color(char_u *name)
{
    ಹಿಂತಿರುಗಿ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
    ಹಿಂತಿರುಗಿ gui_get_rgb_color_cmn(r, g, b);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_fg_color(guicolor_T color)
{
    Pg_ಗಣ_ಪಠ್ಯ_Color(color);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_bg_color(guicolor_T color)
{
    Pg_ಗಣ_Fill_Color(color);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_sp_color(guicolor_T color)
{
}

    ಶೂನ್ಯ
gui_mch_invert_rectangle(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ nr, ಪೂರ್ಣಾಂಕ nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(ಲಂಬಸಾಲು);
    rect.ul.y = FILL_Y(ಅಡ್ಡಸಾಲು);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_ಅಗಲ;
    rect.lr.y = rect.ul.y + nr * gui.char_ಎತ್ತರ;
    ಆದರೆ (nc > 0)
	rect.lr.x -= 1;
    ಆದರೆ (nr > 0)
	rect.lr.y -= 1;

    DRAW_ಆರಂಭ;
    Pg_ಗಣ_Draw_ಬಹುಲಕ(Pg_Draw_ಬಹುಲಕ_DSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    Pg_ಗಣ_Draw_ಬಹುಲಕ(Pg_Draw_ಬಹುಲಕ_SRCCOPY);
    DRAW_ಕೊನೆ;
}

    ಶೂನ್ಯ
gui_mch_clear_ಖಂಡ(ಪೂರ್ಣಾಂಕ row1, ಪೂರ್ಣಾಂಕ col1, ಪೂರ್ಣಾಂಕ row2, ಪೂರ್ಣಾಂಕ col2)
{
    PhRect_t ಖಂಡ = {
	{ FILL_X(col1), FILL_Y(row1) },
	{ FILL_X(col2 + 1) - 1, FILL_Y(row2 + 1) - 1}
    };

    DRAW_ಆರಂಭ;
    gui_mch_ಗಣ_bg_color(gui.ಹಿಂಭಾಗ_pixel);
    PgDrawRect(&ಖಂಡ, Pg_DRAW_FILL);
    DRAW_ಕೊನೆ;
}

    ಶೂನ್ಯ
gui_mch_clear_all(ಶೂನ್ಯ)
{
    PhRect_t ಪಠ್ಯ_rect = {
	{ gui.border_ಅಗಲ, gui.border_ಅಗಲ },
	{ Columns * gui.char_ಅಗಲ + gui.border_ಅಗಲ - 1 ,
	    ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ + gui.border_ಅಗಲ - 1 }
    };

    ಆದರೆ (is_ignore_draw == TRUE)
	ಹಿಂತಿರುಗಿ;

    DRAW_ಆರಂಭ;
    gui_mch_ಗಣ_bg_color(gui.ಹಿಂಭಾಗ_pixel);
    PgDrawRect(&ಪಠ್ಯ_rect, Pg_DRAW_FILL);
    DRAW_ಕೊನೆ;
}

    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    PhRect_t    rect;
    Ph_ಬಿಂದು_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ಎಡ);
    rect.ul.y = FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು);

    rect.lr.x = FILL_X(gui.scroll_region_ಬಲ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    Pt_Widget_ಸ್ಥಾನಾಂತರ(gui.vim_ಪಠ್ಯ_Area, &gui_ph_raw_ಸ್ಥಾನಾಂತರ);
    Ph_Translate_ಬಿಂದು(&gui_ph_raw_ಸ್ಥಾನಾಂತರ, PtWidgetPos(gui.vim_ಪಠ್ಯ_Area, ಖಾಲಿ));
    PhTranslateRect(&rect, &gui_ph_raw_ಸ್ಥಾನಾಂತರ);

    delta.x = 0;
    delta.y = -ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_ಎತ್ತರ;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ಕಂಡುಹಿಡಿ_Disjoint(gui.vim_ಪಠ್ಯ_Area)), &rect, &delta);

    gui_clear_ಖಂಡ(
	gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
	gui.scroll_region_ಎಡ,
	gui.scroll_region_bot,
	gui.scroll_region_ಬಲ);
}

    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    PhRect_t    rect;
    Ph_ಬಿಂದು_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ಎಡ);
    rect.ul.y = FILL_Y(ಅಡ್ಡಸಾಲು);

    rect.lr.x = FILL_X(gui.scroll_region_ಬಲ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1) - 1;

    Pt_Widget_ಸ್ಥಾನಾಂತರ(gui.vim_ಪಠ್ಯ_Area, &gui_ph_raw_ಸ್ಥಾನಾಂತರ);
    Ph_Translate_ಬಿಂದು(&gui_ph_raw_ಸ್ಥಾನಾಂತರ, PtWidgetPos(gui.vim_ಪಠ್ಯ_Area, ಖಾಲಿ));
    PhTranslateRect(&rect, &gui_ph_raw_ಸ್ಥಾನಾಂತರ);

    delta.x = 0;
    delta.y = ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_ಎತ್ತರ;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ಕಂಡುಹಿಡಿ_Disjoint(gui.vim_ಪಠ್ಯ_Area)) , &rect, &delta);

    gui_clear_ಖಂಡ(ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
	    ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
}

    ಶೂನ್ಯ
gui_mch_draw_string(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ flags)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *utf8_ಬಫರ್ = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	utf8_len = 0;

    Ph_ಬಿಂದು_t	pos = { ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು) };
    PhRect_t	rect;

    ಆದರೆ (is_ignore_draw == TRUE)
	ಹಿಂತಿರುಗಿ;

    DRAW_ಆರಂಭ;

    ಆದರೆ (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಅಡ್ಡಸಾಲು),
		FILL_X(ಲಂಬಸಾಲು + len) - 1, FILL_Y(ಅಡ್ಡಸಾಲು + 1) - 1,
		Pg_DRAW_FILL);
    }

    ಆದರೆ (flags & DRAW_UNDERL)
	Pg_ಗಣ_Underline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    ಆದರೆ (charset_translate != ಖಾಲಿ && enc_utf8 == 0)
    {
	ಪೂರ್ಣಾಂಕ src_ತೆಗೆದುಕೊಂಡ, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	ಆದರೆ (utf8_len < len)
	{
	    ಅಕ್ಷರ *new_ಬಫರ್ = realloc(utf8_ಬಫರ್, len * MB_LEN_MAX);

	    ಆದರೆ (new_ಬಫರ್ == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    utf8_ಬಫರ್ = new_ಬಫರ್;
	    utf8_len = len;
	}

	Px_Translate_ಗಮ್ಯ_UTF(
		charset_translate,
		s,
		len,
		&src_ತೆಗೆದುಕೊಂಡ,
		utf8_ಬಫರ್,
		utf8_len,
		&dst_made);
	s = utf8_ಬಫರ್;
	len = dst_made;
    }

    Pg_Draw_ಪಠ್ಯ(s, len, &pos, 0);

    ಆದರೆ (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(ಲಂಬಸಾಲು) + 1;
	rect.ul.y = FILL_Y(ಅಡ್ಡಸಾಲು);
	rect.lr.x = FILL_X(ಲಂಬಸಾಲು + len) - 1;
	rect.lr.y = FILL_Y(ಅಡ್ಡಸಾಲು + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#ಆದರೆ 0
	pos.x++;

	Pg_ಗಣ_User_Clip(&rect);
	Pg_Draw_ಪಠ್ಯ(s, len, &pos, 0);
	Pg_ಗಣ_User_Clip(ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	Pg_Draw_ಪಠ್ಯ_Area(s, len, &rect, Pg_ಪಠ್ಯ_BOTTOM);
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (flags & DRAW_UNDERL)
	Pg_ಗಣ_Underline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_ಕೊನೆ;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    PhRect_t r;

    // FIXME: Double width characters

    r.ul.x = FILL_X(gui.ಲಂಬಸಾಲು);
    r.ul.y = FILL_Y(gui.ಅಡ್ಡಸಾಲು);
    r.lr.x = r.ul.x + gui.char_ಅಗಲ - 1;
    r.lr.y = r.ul.y + gui.char_ಎತ್ತರ - 1;

    DRAW_ಆರಂಭ;
    Pg_ಗಣ_Stroke_Color(color);
    PgDrawRect(&r, Pg_DRAW_STROKE);
    DRAW_ಕೊನೆ;
}

    ಶೂನ್ಯ
gui_mch_draw_part_cursor(ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h, guicolor_T color)
{
    PhRect_t r;

    r.ul.x = FILL_X(gui.ಲಂಬಸಾಲು);
    r.ul.y = FILL_Y(gui.ಅಡ್ಡಸಾಲು) + gui.char_ಎತ್ತರ - h;
    r.lr.x = r.ul.x + w - 1;
    r.lr.y = r.ul.y + h - 1;

    DRAW_ಆರಂಭ;
    gui_mch_ಗಣ_bg_color(color);
    PgDrawRect(&r, Pg_DRAW_FILL);
    DRAW_ಕೊನೆ;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_blinking(ದೀರ್ಘ wait, ದೀರ್ಘ on, ದೀರ್ಘ off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ಶೂನ್ಯ
gui_mch_ಆರಂಭ_blink(ಶೂನ್ಯ)
{
    // Only turn on the timer on if none of the times are zero
    ಆದರೆ (blink_waittime && blink_ontime && blink_offtime && gui.ಒಳ_focus)
    {
	Pt_ಗಣ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಂಕ may_call_gui_update_cursor)
{
    Pt_ಗಣ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    ಆದರೆ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    PtBeep();
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಂಕ msec)
{
    Pg_ಗಣ_Fill_XORColor(Pg_BLACK, Pg_WHITE);
    Pg_ಗಣ_Draw_ಬಹುಲಕ(Pg_DRAWMODE_XOR);
    gui_mch_clear_all();
    gui_mch_flush();

    ui_delay((ದೀರ್ಘ) msec, TRUE);

    gui_mch_clear_all();
    Pg_ಗಣ_Draw_ಬಹುಲಕ(Pg_DRAWMODE_OPAQUE);
    gui_mch_flush();
}

    ಶೂನ್ಯ
gui_mch_flush(ಶೂನ್ಯ)
{
    PgFlush();
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_ಪಠ್ಯ_area_pos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    PhArea_t area = {{x, y}, {w, h}};

    Pt_ಗಣ_Resource(gui.vim_ಪಠ್ಯ_Area, Pt_ARG_AREA, &area, 0);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	ಆದರೆ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0 &&
		 name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
# include "toolbar.phi"

ಸ್ಥಿರ PhImage_t *gui_ph_toolbar_images[] = {
    &tb_new_phi,
    &tb_open_phi,
    &tb_save_phi,
    &tb_undo_phi,
    &tb_redo_phi,
    &tb_cut_phi,
    &tb_copy_phi,
    &tb_paste_phi,
    &tb_print_phi,
    &tb_help_phi,
    &tb_ಕಂಡುಹಿಡಿ_phi,
    &tb_save_all_phi,
    &tb_save_session_phi,
    &tb_new_session_phi,
    &tb_ತುಂಬು_session_phi,
    &tb_macro_phi,
    &tb_replace_phi,
    &tb_close_phi,
    &tb_maximize_phi,
    &tb_minimize_phi,
    &tb_ವಿಭಜಿಸು_phi,
    &tb_shell_phi,
    &tb_ಕಂಡುಹಿಡಿ_prev_phi,
    &tb_ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_phi,
    &tb_ಕಂಡುಹಿಡಿ_help_phi,
    &tb_make_phi,
    &tb_jump_phi,
    &tb_ctags_phi,
    &tb_vsplit_phi,
    &tb_maxwidth_phi,
    &tb_minwidth_phi
};

ಸ್ಥಿರ PhImage_t *
gui_ph_toolbar_ತುಂಬು_icon(char_u *iconfile)
{
    ಸ್ಥಿರ PhImage_t external_icon;
    PhImage_t *ತಾತ್ಕಾಲಿಕ_phi = ಖಾಲಿ;

    ತಾತ್ಕಾಲಿಕ_phi = Px_ತುಂಬು_Image(iconfile, ಖಾಲಿ);
    ಆದರೆ (ತಾತ್ಕಾಲಿಕ_phi != ಖಾಲಿ)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	ತಾತ್ಕಾಲಿಕ_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	ಸ್ಮರಣೆ_ನಕಲು(&external_icon, ತಾತ್ಕಾಲಿಕ_phi, ಗಾತ್ರ(external_icon));
	ಬಿಡುಗಡೆ(ತಾತ್ಕಾಲಿಕ_phi);

	ತಾತ್ಕಾಲಿಕ_phi = &external_icon;
    }
    ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ಸ್ಥಿರ PhImage_t *
gui_ph_toolbar_ಕಂಡುಹಿಡಿ_icon(vimmenu_T *menu)
{
    char_u ತುಂಬಿದ_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = ಖಾಲಿ;

    ಆದರೆ (menu->icon_builtin == FALSE)
    {
	ಆದರೆ (menu->iconfile != ಖಾಲಿ)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_ತುಂಬು_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	ಆದರೆ ((icon == ಖಾಲಿ) &&
	    ((gui_ಕಂಡುಹಿಡಿ_bitmap(menu->name, ತುಂಬಿದ_pathname, "gif") == OK) ||
	      (gui_ಕಂಡುಹಿಡಿ_bitmap(menu->name, ತುಂಬಿದ_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_ತುಂಬು_icon(ತುಂಬಿದ_pathname);

	ಆದರೆ (icon != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ icon;
    }

    ಆದರೆ (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_ಉದ್ದಳತೆ(gui_ph_toolbar_images)))
	ಹಿಂತಿರುಗಿ gui_ph_toolbar_images[menu->iconidx];

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_MENU)
    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಂಕ flag)
{
    ಆದರೆ (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    ಇಲ್ಲದಿದ್ದರೆ
	PtUnrealizeWidget(gui.vimMenuBar);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_menu_pos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_position_menu(PtWidget_t *widget, ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = Pt_Widget_ಮಗು_ಹಿಂಭಾಗ(Pt_Widget_ಪೋಷಕ(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    ಆಗ (traverse != ಖಾಲಿ)
    {
	PtGetResource(traverse, Pt_ARG_ಸೂಚಕ, &menu, 0);

	ಆದರೆ (menu != ಖಾಲಿ &&
		ಆದ್ಯತೆ < menu->ಆದ್ಯತೆ &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_ಸೇರಿಸು(widget, traverse, 1);
	    ಹಿಂತಿರುಗಿ;
	}

	traverse = Pt_Widget_Brother_ಒಳ_Front(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;
    char_u	*accel_ಕೀಲಿ;
    char_u	mnemonic_str[MB_LEN_MAX];
    ಪೂರ್ಣಾಂಕ	    n;
    PtArg_t ವಾದಗಳು[5];

    menu->submenu_id = menu->id = ಖಾಲಿ;

    ಆದರೆ (menu_is_menubar(menu->name))
    {

	accel_ಕೀಲಿ = vim_strchr(menu->name, '&');
	ಆದರೆ (accel_ಕೀಲಿ != ಖಾಲಿ)
	{
	    mnemonic_str[0] = accel_ಕೀಲಿ[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಪಠ್ಯ_STRING, menu->dname, 0);
	Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ACCEL_ಪಠ್ಯ, menu->actext, 0);
	ಆದರೆ (accel_ಕೀಲಿ != ಖಾಲಿ)
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ACCEL_ಕೀಲಿ, mnemonic_str, 0);
	Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಸೂಚಕ, menu, 0);

	ಆದರೆ (ಪೋಷಕ != ಖಾಲಿ)
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_ಬಲ, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(ಪೋಷಕ == ಖಾಲಿ) ? gui.vimMenuBar : ಪೋಷಕ->submenu_id,
		n, ವಾದಗಳು);

	Pt_ಕೂಡಿಸು_Callback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	ಆದರೆ (ಪೋಷಕ != ಖಾಲಿ)
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_ಮಗು);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, ವಾದಗಳು);

	ಆದರೆ (ಪೋಷಕ == ಖಾಲಿ)
	{
	    Pt_ಕೂಡಿಸು_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    ಆದರೆ (menu->mnemonic != 0)
	    {
		Pt_ಕೂಡಿಸು_Hotkey_Handler(gui.vim_ಕಿಟಕಿ, ಸುರಕ್ಷಿತ_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->ಆದ್ಯತೆ);

	// Redraw menubar here instead of gui_mch_draw_menubar
	ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	    PtRealizeWidget(menu->id);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vim_ಕಿಟಕಿ, 0, ಖಾಲಿ);
	Pt_ಕೂಡಿಸು_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;
    char_u	*accel_ಕೀಲಿ;
    char_u	mnemonic_str[MB_LEN_MAX];
    ಪೂರ್ಣಾಂಕ	    n;
    PtArg_t ವಾದಗಳು[13];

    n = 0;
    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಸೂಚಕ, menu, 0);

# ifdef FEAT_TOOLBAR
    ಆದರೆ (menu_is_toolbar(ಪೋಷಕ->name))
    {
	ಆದರೆ (menu_is_separator(menu->name))
	{
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_ಒಳ, 0);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_ತುದಿ_BOTTOM);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಅಗಲ, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, ವಾದಗಳು);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) p_toolbar, "text") != ಖಾಲಿ)
	    {
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಪಠ್ಯ_STRING, menu->dname, 0);
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಪಠ್ಯ_FONT, "TextFont08", 0);
	    }
	    ಆದರೆ ((ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) p_toolbar, "icons") != ಖಾಲಿ) &&
		    (gui_ph_toolbar_images != ಖಾಲಿ))
	    {
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_ಕಂಡುಹಿಡಿ_icon(menu), 0);
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_LABEL_TYPE, Pt_ಪಠ್ಯ_IMAGE, 0);
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಪಠ್ಯ_IMAGE_SPACING, 0, 0);
	    }
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) p_toolbar, "tooltips") != ಖಾಲಿ)
	    {
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_LABEL_BALLOON,
			gui_ph_show_tooltip, 0);
		Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_LABEL_FLAGS,
			Pt_TRUE, Pt_SHOW_BALLOON);
	    }
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_MARGIN_ಎತ್ತರ, 1, 0);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_MARGIN_ಅಗಲ, 1, 0);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_FLAGS, Pt_FALSE,
		    Pt_HIGHLIGHTED | Pt_GETS_FOCUS);
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_FILL_COLOR, Pg_TRANSPARENT, 0);
	    menu->id = PtCreateWidget(PtButton, gui.vimToolBar, n, ವಾದಗಳು);

	    Pt_ಕೂಡಿಸು_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);
	}
	// Update toolbar if it's open
	ಆದರೆ (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	ಆದರೆ (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, ಪೋಷಕ->submenu_id, n, ವಾದಗಳು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	accel_ಕೀಲಿ = vim_strchr(menu->name, '&');
	ಆದರೆ (accel_ಕೀಲಿ != ಖಾಲಿ)
	{
	    mnemonic_str[0] = accel_ಕೀಲಿ[1];
	    mnemonic_str[1] = NUL;
	}

	Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ಪಠ್ಯ_STRING, menu->dname, 0);
	ಆದರೆ (accel_ಕೀಲಿ != ಖಾಲಿ)
	    Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ACCEL_ಕೀಲಿ, mnemonic_str,
		    0);

	Pt_ಗಣ_Arg(&ವಾದಗಳು[ n++ ], Pt_ARG_ACCEL_ಪಠ್ಯ, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, ಪೋಷಕ->submenu_id, n, ವಾದಗಳು);

	Pt_ಕೂಡಿಸು_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_ಗುಂಪು
	ಆದರೆ (gui_ph_is_ಬಫರ್_ಅಂಶ(menu, ಪೋಷಕ) == TRUE)
	{
	    Pt_ಕೂಡಿಸು_Callback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_ಬಫರ್_remove, menu);
	    gui_ph_pg_ಕೂಡಿಸು_ಬಫರ್(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->ಆದ್ಯತೆ);
}

    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    ಆದರೆ (menu->submenu_id != ಖಾಲಿ)
	PtDestroyWidget(menu->submenu_id);
    ಆದರೆ (menu->id != ಖಾಲಿ)
	PtDestroyWidget(menu->id);

    menu->submenu_id = ಖಾಲಿ;
    menu->id = ಖಾಲಿ;
}

    ಶೂನ್ಯ
gui_mch_menu_grey(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ grey)
{
    ದೀರ್ಘ    flags, ಮುಖವಾಡ, fields;

    ಆದರೆ (menu->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    flags = PtWidgetFlags(menu->id);
    ಆದರೆ (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(Pt_Widget_ಪೋಷಕ(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	ಮುಖವಾಡ = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fields = Pt_TRUE;
	ಮುಖವಾಡ = Pt_BLOCKED | Pt_GHOST;
    }

    ಆದರೆ (! grey)
	fields = ~fields;

    Pt_ಗಣ_Resource(menu->id, Pt_ARG_FLAGS, fields,
	    ಮುಖವಾಡ);
}

    ಶೂನ್ಯ
gui_mch_menu_hidden(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ hidden)
{
    // TODO: [un]realize the widget?
}

    ಶೂನ್ಯ
gui_mch_draw_menubar(ಶೂನ್ಯ)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    Pt_ಗಣ_Resource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಂಕ enable)
{
    // No tearoffs yet
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (showit)
	PtRealizeWidget(gui.vimToolBar);
    ಇಲ್ಲದಿದ್ದರೆ
	PtUnrealizeWidget(gui.vimToolBar);
}
#ಆದರೆ_ಅಂತ್ಯ

////////////////////////////////////////////////////////////////////////////
// Fonts

    ಸ್ಥಿರ GuiFont
gui_ph_get_font(
	char_u	*font_name,
	int_u	font_flags,
	int_u	font_ಗಾತ್ರ_2,
	// Check whether the resulting font has the font flags and size that
	// was asked for
	int_u	enforce
	)
{
    char_u	    *font_ಟ್ಯಾಗ್;
    FontQueryInfo   info;
    int_u	    style;

    font_ಟ್ಯಾಗ್ = alloc(MAX_FONT_ಟ್ಯಾಗ್);
    ಆದರೆ (font_ಟ್ಯಾಗ್ != ಖಾಲಿ)
    {
	ಆದರೆ (PfGenerateFontName(font_name, font_flags, font_ಗಾತ್ರ_2,
		    font_ಟ್ಯಾಗ್) != ಖಾಲಿ)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    ಆದರೆ (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    ಆದರೆ (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    ಆದರೆ (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_ಟ್ಯಾಗ್, &info);

	    ಆದರೆ (info.ಗಾತ್ರ_2 == 0)
		font_ಗಾತ್ರ_2 = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    ಆದರೆ (font_ಗಾತ್ರ_2 == info.ಗಾತ್ರ_2 &&
		    style == (info.style & style))
		ಹಿಂತಿರುಗಿ (GuiFont)font_ಟ್ಯಾಗ್;
	}
	vim_free(font_ಟ್ಯಾಗ್);
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Split up the vim font name
 *
 * vim_font is in the form of
 * <name>:s<height>:a:b:i
 *
 * a = antialias
 * b = bold
 * i = italic
 *
 */

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_ಗಾತ್ರ_2)
{
    char_u  *mark;
    int_u   name_len, ಗಾತ್ರ_2;

    mark = vim_strchr(vim_font, ':');
    ಆದರೆ (mark == ಖಾಲಿ)
	name_len = STRLEN(vim_font);
    ಇಲ್ಲದಿದ್ದರೆ
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    ಆದರೆ (*font_name != ಖಾಲಿ)
    {
	ಆದರೆ (mark != ಖಾಲಿ)
	{
	    ಆಗ (*mark != NUL && *mark++ == ':')
	    {
		ಆಯ್ಕೆ (ಸುರಕ್ಷಿತ_tolower(*mark++))
		{
		    ಸ್ಥಿತಿ 'a': *font_flags |= PF_STYLE_ANTIALIAS; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'b': *font_flags |= PF_STYLE_BOLD; ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'i': *font_flags |= PF_STYLE_ITALIC; ನಿಲ್ಲಿಸು;

		    ಸ್ಥಿತಿ 's':
			ಗಾತ್ರ_2 = getdigits(&mark);
			// Restrict the size to some vague limits
			ಆದರೆ (ಗಾತ್ರ_2 < 1 || ಗಾತ್ರ_2 > 100)
			    ಗಾತ್ರ_2 = 8;

			*font_ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
			ನಿಲ್ಲಿಸು;

		    ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_init_font(char_u *vim_font_name, ಪೂರ್ಣಾಂಕ fontset)
{
    char_u  *font_ಟ್ಯಾಗ್;
    char_u  *font_name = ಖಾಲಿ;
    int_u   font_flags = 0;
    int_u   font_ಗಾತ್ರ_2  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    ಆದರೆ (vim_font_name == ಖಾಲಿ)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    ಆದರೆ (STRCMP(vim_font_name, "*") == 0)
    {
	font_ಟ್ಯಾಗ್ = PtFontSelection(gui.vim_ಕಿಟಕಿ, ಖಾಲಿ, ಖಾಲಿ,
		"pcterm12", -1, PHFONT_FIXED, ಖಾಲಿ);

	ಆದರೆ (font_ಟ್ಯಾಗ್ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ಟ್ಯಾಗ್;

	PfQueryFontInfo(font_ಟ್ಯಾಗ್, &info);
	font_name = vim_strsave(info.font);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_ಗಾತ್ರ_2) == FALSE)
	    ಹಿಂತಿರುಗಿ FAIL;

	font_ಟ್ಯಾಗ್ = gui_ph_get_font(font_name, font_flags, font_ಗಾತ್ರ_2, 0);
	ಆದರೆ (font_ಟ್ಯಾಗ್ == ಖಾಲಿ)
	{
	    vim_free(font_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ಟ್ಯಾಗ್;
    }

    gui_mch_free_font(gui.bold_font);
    gui.bold_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_BOLD,
	    font_ಗಾತ್ರ_2, PF_STYLE_BOLD);

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_ITALIC,
	    font_ಗಾತ್ರ_2, PF_STYLE_ITALIC);

    // This extent was brought to you by the letter 'g'
    Pf_Extent_ಪಠ್ಯ(&extent, ಖಾಲಿ, font_ಟ್ಯಾಗ್, "g", 1);

    gui.char_ಅಗಲ = extent.lr.x - extent.ul.x + 1;
    gui.char_ಎತ್ತರ = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_ಎತ್ತರ = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    ಹಿಂತಿರುಗಿ OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, ಪೂರ್ಣಾಂಕ report_error)
{
    char_u  *font_name;
    char_u  *font_ಟ್ಯಾಗ್;
    int_u   font_ಗಾತ್ರ_2 = 12;
    int_u   font_flags = 0;

    ಆದರೆ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_ಗಾತ್ರ_2) != FALSE)
    {
	font_ಟ್ಯಾಗ್ = gui_ph_get_font(font_name, font_flags, font_ಗಾತ್ರ_2, -1);
	vim_free(font_name);

	ಆದರೆ (font_ಟ್ಯಾಗ್ != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ (GuiFont)font_ಟ್ಯಾಗ್;
    }

    ಆದರೆ (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strsave(name);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_ಗಣ_font(GuiFont font)
{
    Pg_ಗಣ_Font(font);
}

    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

