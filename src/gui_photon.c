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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
# include <photon/PxImage.h>
#ਜੇ_ਅੰਤ

#ਜੇ !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	ਪੂਰਨ
# define PhEvent_t	ਪੂਰਨ
# define Ph_ਬਿੰਦੂ_t	ਪੂਰਨ
# define PtWidget_t	ਪੂਰਨ
# define Pg_BLACK	0
# define PtCallbackF_t	ਪੂਰਨ
# define PtCallbackInfo_t ਪੂਰਨ
# define PhTile_t	ਪੂਰਨ
# define PtWidget_t	ਪੂਰਨ
# define PhImage_t	ਪੂਰਨ
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RGB(r, g, b) PgRGB(r, g, b)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVENT_ਬਫਰ_ਮਾਪ ਆਕਾਰ(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_HIDE		TRUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ USE_PANEL_ਸਮੂਹ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
ਸਥਿਰ ਅੱਖਰ	*ਖਾਲੀ_ਥਾਵਾਂ_title = "    ";
ਸਥਿਰ ਅੱਖਰ	**panel_titles = ਨਿਰਾ;
ਸਥਿਰ ushort_t	ਅੰਕ_panels = 0;
ਸਥਿਰ ਛੋਟਾ pg_margin_ਖੱਬਾ, pg_margin_ਸੱਜਾ, pg_margin_ਸਿਖਰ, pg_margin_bottom;
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_PH_MARGIN		4	// Size of the bevel

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_PH_MOUSE_TYPE		Ph_CURSOR_ਪਾਓ
ਸਥਿਰ PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ਸਥਿਰ Ph_ਬਿੰਦੂ_t    gui_ph_raw_ਵਿੱਥ;
ਸਥਿਰ PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ਸਥਿਰ PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ਸਥਿਰ ਛੋਟਾ	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ਸਥਿਰ Ph_ਬਿੰਦੂ_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
ਸਥਿਰ ਪੂਰਨ is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
ਸਥਿਰ ਬਣਤਰ PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
ਸਥਿਰ ਗਿਣਤੀ {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

ਸਥਿਰ long_u	blink_waittime	= 700;
ਸਥਿਰ long_u	blink_ontime	= 400;
ਸਥਿਰ long_u	blink_offtime	= 250;

ਸਥਿਰ ਬਣਤਰ
{
    ਪੂਰਨ	    ਕੁੰਜੀ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ਕੁੰਜੀਆਂ[] =
{
    {Pk_ਉੱਪਰ,	    'k', 'u'},
    {Pk_ਥੱਲੇ,	    'k', 'd'},
    {Pk_ਖੱਬਾ,	    'k', 'l'},
    {Pk_ਸੱਜਾ,	    'k', 'r'},

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
    {Pk_ਪਿੱਠ_Space,  'k', 'b'},
    {Pk_ਪਾਓ,	    'k', 'I'},
    {Pk_Delete,	    'k', 'D'},
    {Pk_Home,	    'k', 'h'},
    {Pk_ਸਿਰਾ,	    '@', '7'},
    {Pk_ਪੂਰਵ,	    'k', 'P'},
    {Pk_ਅਗਲਾ,	    'k', 'N'},
    {Pk_Print,	    '%', '9'},

    {Pk_KP_ਜੋੜੋ,	    'K', '6'},
    {Pk_KP_ਘਟਾਓ,'K', '7'},
    {Pk_KP_Divide,  'K', '8'},
    {Pk_KP_Multiply,'K', '9'},
    {Pk_KP_Enter,   'K', 'A'},

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //ਪਾਓ
    {Pk_KP_Decimal, KS_EXTRA, KE_KDEL}, // Delete

    {Pk_KP_4,	    'k', 'l'}, // Left
    {Pk_KP_6,	    'k', 'r'}, // Right
    {Pk_KP_8,	    'k', 'u'}, // Up
    {Pk_KP_2,	    'k', 'd'}, // Down

    {Pk_KP_7,	    'K', '1'}, // Home
    {Pk_KP_1,	    'K', '4'}, //ਸਿਰਾ

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

ਸਥਿਰ PtCallbackF_t gui_ph_handle_timer_cursor;
ਸਥਿਰ PtCallbackF_t gui_ph_handle_timer_timeout;

ਸਥਿਰ PtCallbackF_t gui_ph_handle_ਖਿੜਕੀ_cb;

ਸਥਿਰ PtCallbackF_t gui_ph_handle_scrollbar;
ਸਥਿਰ PtCallbackF_t gui_ph_handle_keyboard;
ਸਥਿਰ PtCallbackF_t gui_ph_handle_mouse;
ਸਥਿਰ PtCallbackF_t gui_ph_handle_pulldown_menu;
ਸਥਿਰ PtCallbackF_t gui_ph_handle_menu;
ਸਥਿਰ PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ਸਥਿਰ PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ਸਥਿਰ PtCallbackF_t gui_ph_handle_menu_unrealized;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
ਸਥਿਰ ਖਾਲੀ gui_ph_get_panelgroup_margins(ਛੋਟਾ*, ਛੋਟਾ*, ਛੋਟਾ*, ਛੋਟਾ*);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ gui_ph_draw_ਸ਼ੁਰੂ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ gui_ph_draw_ਸਿਰਾ(ਖਾਲੀ);

// Set the text for the balloon
ਸਥਿਰ PtWidget_t * gui_ph_show_tooltip(PtWidget_t *ਖਿੜਕੀ_2,
			     PtWidget_t *widget,
			     ਪੂਰਨ position,
			     ਅੱਖਰ *ਪਾਠ,
			     ਅੱਖਰ *font,
			     PgColor_t fill_color,
			     PgColor_t ਪਾਠ_color);

////////////////////////////////////////////////////////////////////////////

ਸਥਿਰ PtWidget_t * gui_ph_show_tooltip(PtWidget_t *ਖਿੜਕੀ_2,
			     PtWidget_t *widget,
			     ਪੂਰਨ position,
			     ਅੱਖਰ *ਪਾਠ,
			     ਅੱਖਰ *font,
			     PgColor_t fill_color,
			     PgColor_t ਪਾਠ_color)
{
    PtArg_t arg;
    vimmenu_T *menu;
    char_u  *tooltip;

    Pt_ਸੈੱਟ_Arg(&arg, Pt_ARG_ਸੂਚਕ, &menu, 0);
    PtGetResources(widget, 1, &arg);

    // Override the text and position

    tooltip = ਪਾਠ;
    ਜੇ (menu != ਨਿਰਾ)
    {
	ਪੂਰਨ ਸੂਚਕਾਂਕ = MENU_ਸੂਚਕਾਂਕ_TIP;
	ਜੇ (menu->ਲੜੀਆਂ[ ਸੂਚਕਾਂਕ ] != ਨਿਰਾ)
	    tooltip = menu->ਲੜੀਆਂ[ ਸੂਚਕਾਂਕ ];
    }

    ਵਾਪਸ PtInflateBalloon(
	    ਖਿੜਕੀ_2,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_ਸਿਖਰ,
	    tooltip,
	    font,
	    fill_color,
	    ਪਾਠ_color);
}

    ਸਥਿਰ ਖਾਲੀ
gui_ph_resize_container(ਖਾਲੀ)
{
    PhArea_t area;

    PtWidgetArea(gui.vim_ਖਿੜਕੀ, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    Pt_ਸੈੱਟ_Resource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	ਖਾਲੀ *other,
	PtCallbackInfo_t *info)
{
    PtContainerCallback_t *sizes = info->cbdata;
    PtWidget_t		*container;
    Ph_ਬਿੰਦੂ_t		ਹੇਠਾਂ_menu;
    int_u		ਉਚਾਈ;

    ਉਚਾਈ = sizes->new_dim.h;

    // Because vim treats the toolbar and menubar separately,
    // and here they're lumped together into a PtToolbarGroup,
    // we only need either menu_height or toolbar_height set at once
    ਜੇ (gui.menu_is_ਸਰਗਰਮ)
    {
	gui.menu_ਉਚਾਈ = ਉਚਾਈ;
	gui.toolbar_ਉਚਾਈ = 0;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਨਹੀਂਤਾਂ
	gui.toolbar_ਉਚਾਈ = ਉਚਾਈ;
#ਜੇ_ਅੰਤ

    ਹੇਠਾਂ_menu.x = 0;
    ਹੇਠਾਂ_menu.y = ਉਚਾਈ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
    container = gui.vim_Panel_ਸਮੂਹ;
#ਨਹੀਂਤਾਂ
    container = gui.vimContainer;
#ਜੇ_ਅੰਤ

    Pt_ਸੈੱਟ_Resource(container, Pt_ARG_POS, &ਹੇਠਾਂ_menu, 0);

    gui_ph_resize_container();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
    gui_ph_get_panelgroup_margins(
	    &pg_margin_ਸਿਖਰ, &pg_margin_bottom,
	    &pg_margin_ਖੱਬਾ, &pg_margin_ਸੱਜਾ);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	ਖਾਲੀ *ਅੰਕੜੇ,
	PtCallbackInfo_t *info)
{
    ਜੇ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	Pt_ਸੈੱਟ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    ਨਹੀਂਤਾਂ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	Pt_ਸੈੱਟ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    ਖਾਲੀ		*ਅੰਕੜੇ,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_ਖਿੜਕੀ_cb(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    Ph_ਖਿੜਕੀ_Event_t *we = info->cbdata;
    ushort_t *ਚੌੜਾਈ, *ਉਚਾਈ;

    ਚੋਣ (we->event_f)
    {
	ਹਾਲਤ Ph_WM_CLOSE:
	    gui_shell_closed();
	    ਰੋਕੋ;

	ਹਾਲਤ Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    ਜੇ (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_ਸ਼ੁਰੂ_blink();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ Ph_WM_RESIZE:
	    PtGetResource(gui.vim_ਖਿੜਕੀ, Pt_ARG_ਚੌੜਾਈ, &ਚੌੜਾਈ, 0);
	    PtGetResource(gui.vim_ਖਿੜਕੀ, Pt_ARG_ਉਚਾਈ, &ਉਚਾਈ, 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
	    ਚੌੜਾਈ  -= (pg_margin_ਖੱਬਾ + pg_margin_ਸੱਜਾ);
	    ਉਚਾਈ -= (pg_margin_ਸਿਖਰ + pg_margin_bottom);
#ਜੇ_ਅੰਤ
	    gui_resize_shell(*ਚੌੜਾਈ, *ਉਚਾਈ);
	    gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    Pt_ਸਿਰਾ_Flux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    ਰੋਕੋ;

	ਮੂਲ:
	    ਰੋਕੋ;
    }

    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_scrollbar(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    ਪੂਰਨ	    ਮੁੱਲ, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) ਅੰਕੜੇ;
    ਜੇ (sb != ਨਿਰਾ)
    {
	ਮੁੱਲ = scroll->position;
	ਚੋਣ (scroll->action)
	{
	    ਹਾਲਤ Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ Pt_SCROLL_ਸੈੱਟ:
		// FIXME: return straight away here?
		ਵਾਪਸ Pt_CONTINUE;
		ਰੋਕੋ;
	}

	gui_drag_scrollbar(sb, ਮੁੱਲ, dragging);
    }
    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_keyboard(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    Ph_ਕੁੰਜੀ_Event_t    *ਕੁੰਜੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ   string[6];
    ਪੂਰਨ		    len, i;
    ਪੂਰਨ		    ch, modifiers;

    ਕੁੰਜੀ = Ph_Get_ਅੰਕੜੇ(info->event);

    ch = modifiers = len = 0;

    ਜੇ (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    ਜੇ (ਕੁੰਜੀ->ਕੁੰਜੀ_flags & Pk_KF_Compose)
	ਵਾਪਸ Pt_CONTINUE;

    ਜੇ ((ਕੁੰਜੀ->ਕੁੰਜੀ_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_ਕੁੰਜੀ_ਥੱਲੇ(ਕੁੰਜੀ->ਕੁੰਜੀ_flags))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	ਜੇ (((ਕੁੰਜੀ->ਕੁੰਜੀ_mods & Pk_KM_Alt) &&
		    !(ਕੁੰਜੀ->ਕੁੰਜੀ_mods & Pk_KM_ਖਿਸਕਾਓ) &&
		    !(ਕੁੰਜੀ->ਕੁੰਜੀ_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_ਸਰਗਰਮ &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(ਕੁੰਜੀ->ਕੁੰਜੀ_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    ਵਾਪਸ Pt_CONTINUE;
	}
#ਜੇ_ਅੰਤ

	ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != 0; i++)
	{
	    ਜੇ (special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym == ਕੁੰਜੀ->ਕੁੰਜੀ_cap)
	    {
		len = 0;
		ਜੇ (special_ਕੁੰਜੀਆਂ[i].vim_code1 == NUL)
		    ch = special_ਕੁੰਜੀਆਂ[i].vim_code0;
		ਨਹੀਂਤਾਂ
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    ਜੇ (ਕੁੰਜੀ->ਕੁੰਜੀ_cap >= Pk_KP_Enter && ਕੁੰਜੀ->ਕੁੰਜੀ_cap <= Pk_KP_9
			    && (ਕੁੰਜੀ->ਕੁੰਜੀ_mods & Pk_KM_ਅੰਕ_ਤਾਲਾ))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = ਕੁੰਜੀ->ਕੁੰਜੀ_cap - 0xf080;
		    }
		    ਨਹੀਂਤਾਂ
			ch = ਮੰਜ਼ਿਲ_SPECIAL(special_ਕੁੰਜੀਆਂ[i].vim_code0,
				special_ਕੁੰਜੀਆਂ[i].vim_code1);
		}
		ਰੋਕੋ;
	    }
	}

	ਜੇ (ਕੁੰਜੀ->ਕੁੰਜੀ_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_ਨਕਾਬ_CTRL;
	ਜੇ (ਕੁੰਜੀ->ਕੁੰਜੀ_mods & Pk_KM_Alt)
	    modifiers |= MOD_ਨਕਾਬ_ALT;
	ਜੇ (ਕੁੰਜੀ->ਕੁੰਜੀ_mods & Pk_KM_ਖਿਸਕਾਓ)
	    modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;

	// Is this not a special key?
	ਜੇ (special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym == 0)
	{
	    ch = PhTo8859_1(ਕੁੰਜੀ);
	    ਜੇ (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_Mb(string, ਕੁੰਜੀ);
		ਜੇ (len > 0)
		{
		    ਸਥਿਰ ਅੱਖਰ buf[6];
		    ਪੂਰਨ src_ਲਿਆ, dst_made;
		    ਜੇ (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_ਲਿਆ,
				buf,
				6,
				&dst_made);

			ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(buf, dst_made);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
		    }

		    ਵਾਪਸ Pt_CONSUME;
		}
		len = 0;
		ch = ਕੁੰਜੀ->ਕੁੰਜੀ_cap;
		ਜੇ (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    ਜੇ (modifiers & MOD_ਨਕਾਬ_CTRL)
		    {
			modifiers &= ~MOD_ਨਕਾਬ_CTRL;

			ਜੇ ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			ਨਹੀਂਤਾਂ ਜੇ (ch == '2')
			    ch = NUL;
			ਨਹੀਂਤਾਂ ਜੇ (ch == '6')
			    ch = 0x1e;
			ਨਹੀਂਤਾਂ ਜੇ (ch == '-')
			    ch = 0x1f;
			ਨਹੀਂਤਾਂ
			    modifiers |= MOD_ਨਕਾਬ_CTRL;
		    }

		    ਜੇ (modifiers & MOD_ਨਕਾਬ_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_ਨਕਾਬ_ALT;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਵਾਪਸ Pt_CONTINUE;
		}
	    }
	    ਨਹੀਂਤਾਂ
		modifiers &= ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
	}

	ch = simplify_ਕੁੰਜੀ(ch, &modifiers);
	ਜੇ (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	ਜੇ (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_ਦੂਜਾ(ch);
	    string[ len++ ] = K_ਤੀਜਾ(ch);
	}
	ਨਹੀਂਤਾਂ
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    ਪੂਰਨ int_ch = check_for_ਰੁਕਾਵਟ(ch, modifiers);

	    ਜੇ (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_ਨਿਵੇਸ਼_buf();
	    }
	}

	ਜੇ (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	ਜੇ (len > 0)
	{
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
	    ਵਾਪਸ Pt_CONSUME;
	}
    }

    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_mouse(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    Ph_ਸੂਚਕ_Event_t *ਸੂਚਕ;
    PhRect_t	     *pos;
    ਪੂਰਨ		     button = 0, repeated_click, modifiers = 0x0;
    ਛੋਟਾ	     mouse_x, mouse_y;

    ਸੂਚਕ = Ph_Get_ਅੰਕੜੇ(info->event);
    pos = PhGetRects(info->event);

    gui_mch_mousehide(MOUSE_SHOW);

    /*
     * Coordinates need to be relative to the base window,
     * not relative to the vimTextArea widget
     */
    mouse_x = pos->ul.x + gui.border_ਚੌੜਾਈ;
    mouse_y = pos->ul.y + gui.border_ਚੌੜਾਈ;

    ਜੇ (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	ਵਾਪਸ Pt_CONTINUE;
    }

    ਜੇ (ਸੂਚਕ->ਕੁੰਜੀ_mods & Pk_KM_ਖਿਸਕਾਓ)
	modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ (ਸੂਚਕ->ਕੁੰਜੀ_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    ਜੇ (ਸੂਚਕ->ਕੁੰਜੀ_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    ਜੇ (ਸੂਚਕ->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_ਖੱਬਾ;

    ਜੇ (ਸੂਚਕ->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_ਸੱਜਾ;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = ਸੂਚਕ->pos.x;
	abs_mouse.y = ਸੂਚਕ->pos.y;
    }

    ਜੇ (ਸੂਚਕ->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_ਵਿਚਕਾਰ;

    // Catch a real release (not phantom or other releases
    ਜੇ (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    ਜੇ (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#ਜੇ 0
    // Vim doesn't use button repeats
    ਜੇ (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#ਜੇ_ਅੰਤ

    // Don't do anything if it is one of the phantom mouse release events
    ਜੇ ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (ਸੂਚਕ->click_ਗਿਣਤੀ >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    ਵਾਪਸ Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_focus(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    ਜੇ (info->reason == Pt_CB_ਗੁਆਚਾ_FOCUS)
    {
	PtRemoveEventHandler(gui.vim_ਪਾਠ_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ਨਿਰਾ);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    ਨਹੀਂਤਾਂ
    {
	Pt_ਜੋੜੋ_Event_Handler(gui.vim_ਪਾਠ_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ਨਿਰਾ);
    }
    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਖਾਲੀ
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    Ph_ਬਿੰਦੂ_t	ਵਿੱਥ;
    Ph_ਬਿੰਦੂ_t	translation;

    ਜੇ (is_ignore_draw == TRUE)
	ਵਾਪਸ;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#ਜੇ 0
    /*
     * This causes some weird problems, with drawing being done from
     * within this raw drawing function (rather than just simple clearing
     * and text drawing done by gui_redraw)
     *
     * The main problem is when PhBlit is used, and the cursor appearing
     * in places where it shouldn't
     */
    ਬਾਹਰਲਾ_flush();
#ਜੇ_ਅੰਤ

    Pt_Widget_ਵਿੱਥ(widget, &ਵਿੱਥ);
    Ph_Translate_ਬਿੰਦੂ(&ਵਿੱਥ, PtWidgetPos(gui.vim_ਪਾਠ_Area, ਨਿਰਾ));

#ਜੇ 1
    // Redraw individual damage regions
    ਜੇ (damage->ਅਗਲਾ != ਨਿਰਾ)
	damage = damage->ਅਗਲਾ;

    ਜਦੋਂ (damage != ਨਿਰਾ)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - ਵਿੱਥ.x, r->ul.y - ਵਿੱਥ.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->ਅਗਲਾ;
    }
#ਨਹੀਂਤਾਂ
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - ਵਿੱਥ.x, r->ul.y - ਵਿੱਥ.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#ਜੇ_ਅੰਤ

    Pg_ਸੈੱਟ_Translation(&translation, 0);
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	ਖਾਲੀ *ਅੰਕੜੇ,
	PtCallbackInfo_t *info)
{
    ਜੇ (ਅੰਕੜੇ != ਨਿਰਾ)
    {
	vimmenu_T *menu = (vimmenu_T *) ਅੰਕੜੇ;

	PtPositionMenu(menu->submenu_id, ਨਿਰਾ);
	PtRealizeWidget(menu->submenu_id);
    }

    ਵਾਪਸ Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_menu(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    ਜੇ (ਅੰਕੜੇ != ਨਿਰਾ)
    {
	vimmenu_T *menu = (vimmenu_T *) ਅੰਕੜੇ;
	gui_menu_cb(menu);
    }
    ਵਾਪਸ Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	ਖਾਲੀ *ਅੰਕੜੇ,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vim_ਪਾਠ_Area, ਨਿਰਾ);
    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_ਖਿੜਕੀ_open(
	PtWidget_t *widget,
	ਖਾਲੀ *ਅੰਕੜੇ,
	PtCallbackInfo_t *info)
{
    gui_ਸੈੱਟ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ਵਾਪਸ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_ਸ਼ੁਰੂ  gui_ph_draw_ਸ਼ੁਰੂ()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAW_ਸਿਰਾ    gui_ph_draw_ਸਿਰਾ()

/*
 * TODO: Set a clipping rect?
 */
    ਸਥਿਰ ਖਾਲੀ
gui_ph_draw_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    PhGC_t *gc;

    gc = PgGetGC();
    Pg_ਸੈੱਟ_Region(PtWidgetRid(Pt_ਲੱਭੋ_Disjoint(gui.vim_ਪਾਠ_Area)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    Pt_Widget_ਵਿੱਥ(gui.vim_ਪਾਠ_Area, &gui_ph_raw_ਵਿੱਥ);
    Ph_Translate_ਬਿੰਦੂ(&gui_ph_raw_ਵਿੱਥ, PtWidgetPos(gui.vim_ਪਾਠ_Area, ਨਿਰਾ));

    Pg_ਸੈੱਟ_Translation(&gui_ph_raw_ਵਿੱਥ, Pg_RELATIVE);
}

    ਸਥਿਰ ਖਾਲੀ
gui_ph_draw_ਸਿਰਾ(ਖਾਲੀ)
{
    gui_ph_raw_ਵਿੱਥ.x = -gui_ph_raw_ਵਿੱਥ.x;
    gui_ph_raw_ਵਿੱਥ.y = -gui_ph_raw_ਵਿੱਥ.y;
    Pg_ਸੈੱਟ_Translation(&gui_ph_raw_ਵਿੱਥ, Pg_RELATIVE);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
    ਸਥਿਰ vimmenu_T *
gui_ph_ਲੱਭੋ_ਬਫਰ_ਵਸਤੂ(char_u *name)
{
    vimmenu_T *ਸਿਖਰ_level = ਜੜ੍ਹ_menu;
    vimmenu_T *ਚੀਜ਼ਾਂ = ਨਿਰਾ;

    ਜਦੋਂ (ਸਿਖਰ_level != ਨਿਰਾ &&
	    (STRCMP(ਸਿਖਰ_level->dname, "Buffers") != 0))
	ਸਿਖਰ_level = ਸਿਖਰ_level->ਅਗਲਾ;

    ਜੇ (ਸਿਖਰ_level != ਨਿਰਾ)
    {
	ਚੀਜ਼ਾਂ = ਸਿਖਰ_level->children;

	ਜਦੋਂ (ਚੀਜ਼ਾਂ != ਨਿਰਾ &&
		(STRCMP(ਚੀਜ਼ਾਂ->dname, name) != 0))
	    ਚੀਜ਼ਾਂ = ਚੀਜ਼ਾਂ->ਅਗਲਾ;
    }
    ਵਾਪਸ ਚੀਜ਼ਾਂ;
}

    ਸਥਿਰ ਖਾਲੀ
gui_ph_pg_ਸੈੱਟ_ਬਫਰ_ਅੰਕ(int_u buf_ਅੰਕ)
{
    ਪੂਰਨ i;
    ਅੱਖਰ ਖੋਜ[16];
    ਅੱਖਰ *mark;

    ਜੇ (gui.vim_ਪਾਠ_Area == ਨਿਰਾ || buf_ਅੰਕ == 0)
	ਵਾਪਸ;

    ਖੋਜ[0] = '(';
    ultoa(buf_ਅੰਕ, &ਖੋਜ[1], 10);
    STRCAT(ਖੋਜ, ")");

    ਲਈ (i = 0; i < ਅੰਕ_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	ਜੇ (mark != ਨਿਰਾ && STRCMP(mark, ਖੋਜ) == 0)
	{
	    Pt_ਸੈੱਟ_Resource(gui.vim_Panel_ਸਮੂਹ, Pt_ARG_PG_CURRENT_ਸੂਚਕਾਂਕ,
		    i, 0);
	}
    }
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	ਖਾਲੀ *ਅੰਕੜੇ,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    Pt_Panel_ਸਮੂਹ_Callback_t *panel;

    ਜੇ (info->event != ਨਿਰਾ)
    {
	panel = info->cbdata;
	ਜੇ (panel->new_panel != ਨਿਰਾ)
	{
	    menu = gui_ph_ਲੱਭੋ_ਬਫਰ_ਵਸਤੂ(panel->new_panel);
	    ਜੇ (menu)
		gui_menu_cb(menu);
	}
    }
    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਖਾਲੀ
gui_ph_get_panelgroup_margins(
	ਛੋਟਾ *ਸਿਖਰ,
	ਛੋਟਾ *bottom,
	ਛੋਟਾ *ਖੱਬਾ,
	ਛੋਟਾ *ਸੱਜਾ)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *margin_ਸਿਖਰ, *margin_bottom;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *margin_ਖੱਬਾ, *margin_ਸੱਜਾ;

    PtGetAbsPosition(gui.vim_ਪਾਠ_Area, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vim_Panel_ਸਮੂਹ, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vim_Panel_ਸਮੂਹ, Pt_ARG_MARGIN_ਸੱਜਾ, &margin_ਸੱਜਾ, 0);
    PtGetResource(gui.vim_Panel_ਸਮੂਹ, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *ਸਿਖਰ    = abs_raw_y;
    *bottom = *margin_bottom;

    *ਖੱਬਾ  = abs_raw_x;
    *ਸੱਜਾ = *margin_ਸੱਜਾ;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ਸਥਿਰ ਪੂਰਨ
gui_ph_is_ਬਫਰ_ਵਸਤੂ(vimmenu_T *menu, vimmenu_T *ਮਾਪਾ)
{
    ਅੱਖਰ *mark;

    ਜੇ (STRCMP(ਮਾਪਾ->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	ਜੇ (mark != ਨਿਰਾ)
	{
	    mark++;
	    ਜਦੋਂ (ਸੁਰੱਖਿਅਤ_isdigit(*mark))
		mark++;

	    ਜੇ (*mark == ')')
		ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
gui_ph_pg_ਜੋੜੋ_ਬਫਰ(ਅੱਖਰ *name)
{
    ਅੱਖਰ **new_titles = ਨਿਰਾ;

    new_titles = ALLOC_MULT(ਅੱਖਰ *, (ਅੰਕ_panels + 1));
    ਜੇ (new_titles == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਅੰਕ_panels > 0)
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_titles, panel_titles, ਅੰਕ_panels * ਆਕਾਰ(ਅੱਖਰ **));

    new_titles[ ਅੰਕ_panels++ ] = name;

    Pt_ਸੈੱਟ_Resource(gui.vim_Panel_ਸਮੂਹ, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    ਅੰਕ_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ਸਥਿਰ ਖਾਲੀ
gui_ph_pg_remove_ਬਫਰ(ਅੱਖਰ *name)
{
    ਪੂਰਨ i;
    ਅੱਖਰ **new_titles = ਨਿਰਾ;

    // If there is only 1 panel, we just use the temporary place holder
    ਜੇ (ਅੰਕ_panels > 1)
    {
	new_titles = ALLOC_MULT(ਅੱਖਰ *, ਅੰਕ_panels - 1);
	ਜੇ (new_titles != ਨਿਰਾ)
	{
	    ਅੱਖਰ **s = new_titles;
	    // Copy all the titles except the one we're removing
	    ਲਈ (i = 0; i < ਅੰਕ_panels; i++)
	    {
		ਜੇ (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    ਅੰਕ_panels--;

	    Pt_ਸੈੱਟ_Resource(gui.vim_Panel_ਸਮੂਹ, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    ਅੰਕ_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਅੰਕ_panels--;
	Pt_ਸੈੱਟ_Resource(gui.vim_Panel_ਸਮੂਹ, Pt_ARG_PG_PANEL_TITLES, &ਖਾਲੀ_ਥਾਵਾਂ_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ਸਥਿਰ ਪੂਰਨ
gui_ph_handle_ਬਫਰ_remove(
	PtWidget_t *widget,
	ਖਾਲੀ *ਅੰਕੜੇ,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    ਜੇ (ਅੰਕੜੇ != ਨਿਰਾ)
    {
	menu = (vimmenu_T *) ਅੰਕੜੇ;
	gui_ph_pg_remove_ਬਫਰ(menu->dname);
    }

    ਵਾਪਸ Pt_CONTINUE;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
gui_ph_pane_resize(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    ਜੇ (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	Pt_ਸ਼ੁਰੂ_Flux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    ਵਾਪਸ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    ਖਾਲੀ
gui_ph_encoding_ਬਦਲਿਆ(ਪੂਰਨ new_encoding)
{
    // Default encoding is latin1
    ਅੱਖਰ *charset = "latin1";
    ਪੂਰਨ i;

    ਬਣਤਰ {
	ਪੂਰਨ encoding;
	ਅੱਖਰ *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    ਲਈ (i = 0; i < ARRAY_ਲੰਮਾਈ(charsets); i++)
    {
	ਜੇ (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = Px_Translate_ਸੈੱਟ(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    ਖਾਲੀ
gui_mch_prepare(ਪੂਰਨ *argc, ਅੱਖਰ **argv)
{
    PtInit(ਨਿਰਾ);
}

    ਪੂਰਨ
gui_mch_init(ਖਾਲੀ)
{
    PtArg_t ਦਲੀਲਾਂ[10];
    ਪੂਰਨ	    flags = 0, n = 0;

    PhDim_t	ਖਿੜਕੀ_ਮਾਪ = {100, 100}; // Arbitrary values
    Ph_ਬਿੰਦੂ_t	pos = {0, 0};

    gui.event_ਬਫਰ = alloc(EVENT_ਬਫਰ_ਮਾਪ);
    ਜੇ (gui.event_ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = Px_Translate_ਸੈੱਟ(ਨਿਰਾ, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_ਵਿੱਥ = gui.border_ਚੌੜਾਈ = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਖਿੜਕੀ_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਖਿੜਕੀ_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_DIM, &ਖਿੜਕੀ_ਮਾਪ, 0);
    gui.vim_ਖਿੜਕੀ = PtCreateWidget(Pt_ਖਿੜਕੀ, ਨਿਰਾ, n, ਦਲੀਲਾਂ);
    ਜੇ (gui.vim_ਖਿੜਕੀ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    Pt_ਜੋੜੋ_Callback(gui.vim_ਖਿੜਕੀ, Pt_CB_ਖਿੜਕੀ, gui_ph_handle_ਖਿੜਕੀ_cb, ਨਿਰਾ);
    Pt_ਜੋੜੋ_Callback(gui.vim_ਖਿੜਕੀ, Pt_CB_ਖਿੜਕੀ_OPENING,
	    gui_ph_handle_ਖਿੜਕੀ_open, ਨਿਰਾ);

    n = 0;
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_DIM, &ਖਿੜਕੀ_ਮਾਪ, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_POS, &pos, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
    // Put in a temporary place holder title
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_PG_PANEL_TITLES, &ਖਾਲੀ_ਥਾਵਾਂ_title, 1);

    gui.vim_Panel_ਸਮੂਹ = PtCreateWidget(Pt_Panel_ਸਮੂਹ, gui.vim_ਖਿੜਕੀ, n, ਦਲੀਲਾਂ);
    ਜੇ (gui.vim_Panel_ਸਮੂਹ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    Pt_ਜੋੜੋ_Callback(gui.vim_Panel_ਸਮੂਹ, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    // Turn off all edge decorations
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_BEVEL_ਚੌੜਾਈ, 0, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_MARGIN_ਚੌੜਾਈ, 0, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_MARGIN_ਉਚਾਈ, 0, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vim_ਖਿੜਕੀ, n, ਦਲੀਲਾਂ);
    ਜੇ (gui.vimContainer == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    Pt_ਜੋੜੋ_Callback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    // Size for the text area is set in gui_mch_set_text_area_pos
    n = 0;

    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_RAW_DRAW_F, gui_ph_handle_raw_draw, 1);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_BEVEL_ਚੌੜਾਈ, GUI_PH_MARGIN, 0);
    /*
     * Using focus render also causes the whole widget to be redrawn
     * whenever it changes focus, which is very annoying :p
     */
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_FLAGS, Pt_TRUE,
	    Pt_GETS_FOCUS | Pt_HIGHLIGHTED);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_CURSOR_TYPE, GUI_PH_MOUSE_TYPE, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_CURSOR_COLOR, gui_ph_mouse_color, 0);
#ਜੇ_ਅੰਤ

    gui.vim_ਪਾਠ_Area = PtCreateWidget(PtRaw, Pt_DFLT_ਮਾਪਾ, n, ਦਲੀਲਾਂ);
    ਜੇ (gui.vim_ਪਾਠ_Area == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    Pt_ਜੋੜੋ_Event_Handler(gui.vim_ਪਾਠ_Area,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, ਨਿਰਾ);
    Pt_ਜੋੜੋ_Event_Handler(gui.vim_ਪਾਠ_Area, Ph_EV_ਕੁੰਜੀ,
	    gui_ph_handle_keyboard, ਨਿਰਾ);
    Pt_ਜੋੜੋ_Callback(gui.vim_ਪਾਠ_Area, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, ਨਿਰਾ);
    Pt_ਜੋੜੋ_Callback(gui.vim_ਪਾਠ_Area, Pt_CB_ਗੁਆਚਾ_FOCUS,
	    gui_ph_handle_focus, ਨਿਰਾ);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vim_ਖਿੜਕੀ, 0, ਨਿਰਾ);
    ਜੇ (gui_ph_timer_cursor == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vim_ਖਿੜਕੀ, 0, ਨਿਰਾ);
    ਜੇ (gui_ph_timer_timeout == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    Pt_ਜੋੜੋ_Callback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, ਨਿਰਾ);
    Pt_ਜੋੜੋ_Callback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    n = 0;
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਚੌੜਾਈ, ਖਿੜਕੀ_ਮਾਪ.w, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ਖੱਬਾ_ਸੱਜਾ,
	    Pt_IS_ANCHORED);
    gui.vim_Tool_Bar_ਸਮੂਹ = PtCreateWidget(Pt_Toolbar_ਸਮੂਹ, gui.vim_ਖਿੜਕੀ,
	    n, ਦਲੀਲਾਂ);
    ਜੇ (gui.vim_Tool_Bar_ਸਮੂਹ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    Pt_ਜੋੜੋ_Callback(gui.vim_Tool_Bar_ਸਮੂਹ, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, ਨਿਰਾ);

    n = 0;
    flags = 0;
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਚੌੜਾਈ, ਖਿੜਕੀ_ਮਾਪ.w, 0);
    ਜੇ (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vim_Tool_Bar_ਸਮੂਹ, n, ਦਲੀਲਾਂ);
    ਜੇ (gui.vimMenuBar == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_ਖੱਬਾ_ਸੱਜਾ |Pt_ਸਿਖਰ_ANCHORED_ਸਿਖਰ, Pt_IS_ANCHORED);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਚੌੜਾਈ, ਖਿੜਕੀ_ਮਾਪ.w, 0);

    flags = Pt_GETS_FOCUS;
    ਜੇ (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vim_Tool_Bar_ਸਮੂਹ, n, ਦਲੀਲਾਂ);
    ਜੇ (gui.vimToolBar == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

    ਪੂਰਨ
gui_mch_init_check(ਖਾਲੀ)
{
    ਵਾਪਸ (is_photon_ਉਪਲਬਧ == TRUE) ? OK : FAIL;
}

    ਪੂਰਨ
gui_mch_open(ਖਾਲੀ)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.ਪਿੱਠ_pixel =  Pg_WHITE;

    ਸੈੱਟ_ਸਾਧਾਰਨ_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_ਪਿੱਠ_pixel = gui.ਪਿੱਠ_pixel;

    highlight_gui_started();

    ਜੇ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_ਸੈੱਟ_winpos(gui_win_x, gui_win_y);

    ਵਾਪਸ (PtRealizeWidget(gui.vim_ਖਿੜਕੀ) == 0) ? OK : FAIL;
}

    ਖਾਲੀ
gui_mch_exit(ਪੂਰਨ rc)
{
    PtDestroyWidget(gui.vim_ਖਿੜਕੀ);

    Px_Translate_ਸੈੱਟ(charset_translate, ਨਿਰਾ);

    vim_free(gui.event_ਬਫਰ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_GROUPS
    vim_free(panel_titles);
#ਜੇ_ਅੰਤ
}

////////////////////////////////////////////////////////////////////////////
// events

/*
 * When no events are available, photon will call this function, working is
 * set to FALSE, and the gui_mch_update loop will exit.
 */
    ਸਥਿਰ ਪੂਰਨ
exit_gui_mch_update(ਖਾਲੀ *ਅੰਕੜੇ)
{
    *(ਪੂਰਨ *)ਅੰਕੜੇ = FALSE;
    ਵਾਪਸ Pt_ਸਿਰਾ;
}

    ਖਾਲੀ
gui_mch_update(ਖਾਲੀ)
{
    ਪੂਰਨ working = TRUE;

    Pt_App_ਜੋੜੋ_Work_Proc(ਨਿਰਾ, exit_gui_mch_update, &working);
    ਜਦੋਂ ((working == TRUE) && !vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ())
	PtProcessEvent();
}

    ਪੂਰਨ
gui_mch_wait_for_chars(ਲੰਮਾ wtime)
{
    is_timeout = FALSE;

    ਜੇ (wtime >= 0)
	Pt_ਸੈੱਟ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    ਜਦੋਂ (1)
    {
	PtProcessEvent();
	ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	{
	    Pt_ਸੈੱਟ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    ਵਾਪਸ OK;
	}
	ਨਹੀਂਤਾਂ ਜੇ (is_timeout == TRUE)
	    ਵਾਪਸ FAIL;
    }
}

#ਜੇ defined(FEAT_BROWSE)
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
	ਪੂਰਨ saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    ਪੂਰਨ	    flags;
    char_u  *default_ਰਸਤਾ;
    char_u  *open_ਪਾਠ = ਨਿਰਾ;

    flags = 0;
    CLEAR_ਖੇਤਰ(file);

    default_ਰਸਤਾ = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    ਜੇ (default_ਰਸਤਾ != ਨਿਰਾ)
    {
	ਜੇ (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_ਪਾਠ = "&Save";
	}

	// combine the directory and filename into a single path
	ਜੇ (initdir == ਨਿਰਾ || *initdir == NUL)
	{
	    mch_dirname(default_ਰਸਤਾ, MAXPATHL);
	    initdir = default_ਰਸਤਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    STRCPY(default_ਰਸਤਾ, initdir);
	    initdir = default_ਰਸਤਾ;
	}

	ਜੇ (default_name != ਨਿਰਾ)
	{
	    ਜੇ (default_ਰਸਤਾ[ STRLEN(default_ਰਸਤਾ) - 1 ] != '/')
		STRCAT(default_ਰਸਤਾ, "/");

	    STRCAT(default_ਰਸਤਾ, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vim_ਖਿੜਕੀ,
		ਨਿਰਾ,
		title,
		default_ਰਸਤਾ,
		ਨਿਰਾ,
		open_ਪਾਠ,
		ਨਿਰਾ,
		ਨਿਰਾ,
		&file,
		flags);

	vim_free(default_ਰਸਤਾ);

	ਜੇ (file.ret == Pt_FSDIALOG_BTN1)
	    ਵਾਪਸ vim_strsave(file.ਰਸਤਾ);
    }
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_DIALOG)
ਸਥਿਰ PtWidget_t *gui_ph_dialog_ਪਾਠ = ਨਿਰਾ;

    ਸਥਿਰ ਪੂਰਨ
gui_ph_dialog_close(ਪੂਰਨ button, ਖਾਲੀ *ਅੰਕੜੇ)
{
    PtModalCtrl_t *modal_ctrl = ਅੰਕੜੇ;
    char_u *dialog_ਪਾਠ, *vim_ਪਾਠ;

    ਜੇ (gui_ph_dialog_ਪਾਠ != ਨਿਰਾ)
    {
	PtGetResource(gui_ph_dialog_ਪਾਠ, Pt_ARG_ਪਾਠ_STRING, &dialog_ਪਾਠ, 0);
	PtGetResource(gui_ph_dialog_ਪਾਠ, Pt_ARG_ਸੂਚਕ, &vim_ਪਾਠ, 0);
	STRNCPY(vim_ਪਾਠ, dialog_ਪਾਠ, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (ਖਾਲੀ *) button);

    ਵਾਪਸ Pt_TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_dialog_ਪਾਠ_enter(
    PtWidget_t		*widget,
    ਖਾਲੀ		*ਅੰਕੜੇ,
    PtCallbackInfo_t	*info)
{
    ਜੇ (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, ਅੰਕੜੇ);
    ਵਾਪਸ Pt_CONTINUE;
}

    ਸਥਿਰ ਪੂਰਨ
gui_ph_dialog_esc(PtWidget_t *widget, ਖਾਲੀ *ਅੰਕੜੇ, PtCallbackInfo_t *info)
{
    Ph_ਕੁੰਜੀ_Event_t *ਕੁੰਜੀ;

    ਕੁੰਜੀ = Ph_Get_ਅੰਕੜੇ(info->event);
    ਜੇ ((ਕੁੰਜੀ->ਕੁੰਜੀ_flags & Pk_KF_Cap_Valid) && (ਕੁੰਜੀ->ਕੁੰਜੀ_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, ਅੰਕੜੇ);
	ਵਾਪਸ Pt_CONSUME;
    }
    ਵਾਪਸ Pt_PROCESS;
}

    ਪੂਰਨ
gui_mch_dialog(
	ਪੂਰਨ	type,
	char_u	*title,
	char_u	*ਸੁਨੇਹਾ_2,
	char_u	*buttons,
	ਪੂਰਨ	default_button,
	char_u	*textfield,
	ਪੂਰਨ	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    ਪੂਰਨ		button_ਗਿਣਤੀ;
    ਪੂਰਨ		i, len;
    ਪੂਰਨ		dialog_ਨਤੀਜਾ = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_ਗਿਣਤੀ = len = i = 0;

    ਜੇ (buttons == ਨਿਰਾ || *buttons == NUL)
	ਵਾਪਸ -1;

    // There is one less separator than buttons, so bump up the button count
    button_ਗਿਣਤੀ = 1;

    // Count string length and number of separators
    ਲਈ (str = buttons; *str; str++)
    {
	len++;
	ਜੇ (*str == DLG_BUTTON_SEP)
	    button_ਗਿਣਤੀ++;
    }

    ਜੇ (title == ਨਿਰਾ)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_ਗਿਣਤੀ);
    ਜੇ (buttons_copy != ਨਿਰਾ && button_array != ਨਿਰਾ)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	ਲਈ (i = 0; i < button_ਗਿਣਤੀ; i++)
	{
	    button_array[ i ] = str;
	    ਲਈ (; *str; str++)
	    {
		ਜੇ (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    ਰੋਕੋ;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_ਨਤੀਜਾ = PtAlert(
		gui.vim_ਖਿੜਕੀ, ਨਿਰਾ,
		title,
		ਨਿਰਾ,
		ਸੁਨੇਹਾ_2, ਨਿਰਾ,
		button_ਗਿਣਤੀ, (ਅਚਲ ਅੱਖਰ **) button_array, ਨਿਰਾ,
		default_button, 0, Pt_MODAL);
# ਨਹੀਂਤਾਂ
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    ਪੂਰਨ n;
	    PtArg_t ਦਲੀਲਾਂ[5];
	    PtWidget_t *dialog, *pane;
	    PtModalCtrl_t modal_ctrl;
	    PtDialogInfo_t di;

	    CLEAR_ਖੇਤਰ(di);
	    CLEAR_ਖੇਤਰ(modal_ctrl);

	    n = 0;
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_ਸਮੂਹ_ਪੰਕਤੀਆਂ_ਖਾਨੇ_ਗਿਣਤੀ, 0, 0);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_ਚੌੜਾਈ, 350, 0);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_ਸਮੂਹ_ORIENTATION,
		    Pt_ਸਮੂਹ_VERTICAL, 0);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_ਸਮੂਹ_FLAGS,
		    Pt_TRUE, Pt_ਸਮੂਹ_NO_ਕੁੰਜੀਆਂ | Pt_ਸਮੂਹ_STRETCH_HORIZONTAL);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(Pt_ਸਮੂਹ, ਨਿਰਾ, n, ਦਲੀਲਾਂ);

	    n = 0;
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_ਪਾਠ_STRING, ਸੁਨੇਹਾ_2, 0);
	    PtCreateWidget(PtLabel, pane, n, ਦਲੀਲਾਂ);

	    ਜੇ (textfield != ਨਿਰਾ)
	    {
		n = 0;
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_MAX_ਲੰਮਾਈ, IOSIZE - 1, 0);
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_ਪਾਠ_STRING, textfield, 0);
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[n++], Pt_ARG_ਸੂਚਕ, textfield, 0);
		gui_ph_dialog_ਪਾਠ = PtCreateWidget(Pt_ਪਾਠ, pane, n, ਦਲੀਲਾਂ);
		Pt_ਜੋੜੋ_Callback(gui_ph_dialog_ਪਾਠ, Pt_CB_ACTIVATE,
			gui_ph_dialog_ਪਾਠ_enter, &modal_ctrl);
	    }

	    di.ਮਾਪਾ = gui.vim_ਖਿੜਕੀ;
	    di.pane = pane;
	    di.title = title;
	    di.buttons = (ਅਚਲ ਅੱਖਰ **) button_array;
	    di.nbtns = button_ਗਿਣਤੀ;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_ਗਿਣਤੀ;
	    di.callback = gui_ph_dialog_close;
	    di.ਅੰਕੜੇ = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    Pt_ਜੋੜੋ_Filter_Callback(dialog, Ph_EV_ਕੁੰਜੀ,
		    gui_ph_dialog_esc, &modal_ctrl);

	    ਜੇ (gui_ph_dialog_ਪਾਠ != ਨਿਰਾ)
		PtGiveFocus(gui_ph_dialog_ਪਾਠ, ਨਿਰਾ);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_ਨਤੀਜਾ = (ਪੂਰਨ) Pt_Modal_ਖੰਡ(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_ਪਾਠ = ਨਿਰਾ;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    ਵਾਪਸ dialog_ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    ਪੂਰਨ
gui_mch_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    Ph_ਬਿੰਦੂ_t *pos;

    pos = PtWidgetPos(gui.vim_ਖਿੜਕੀ, ਨਿਰਾ);

    *x = pos->x;
    *y = pos->y;

    ਵਾਪਸ OK;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_winpos(ਪੂਰਨ x, ਪੂਰਨ y)
{
    Ph_ਬਿੰਦੂ_t pos = { x, y };

    Pt_ਸੈੱਟ_Resource(gui.vim_ਖਿੜਕੀ, Pt_ARG_POS, &pos, 0);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_shellsize(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ,
	ਪੂਰਨ min_ਚੌੜਾਈ, ਪੂਰਨ min_ਉਚਾਈ, ਪੂਰਨ ਅਧਾਰ_ਚੌੜਾਈ, ਪੂਰਨ ਅਧਾਰ_ਉਚਾਈ,
	ਪੂਰਨ direction)
{
    PhDim_t ਖਿੜਕੀ_ਮਾਪ = { ਚੌੜਾਈ, ਉਚਾਈ };
    PhDim_t min_ਮਾਪ = { min_ਚੌੜਾਈ, min_ਉਚਾਈ };

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
    ਖਿੜਕੀ_ਮਾਪ.w += pg_margin_ਖੱਬਾ + pg_margin_ਸੱਜਾ;
    ਖਿੜਕੀ_ਮਾਪ.h += pg_margin_ਸਿਖਰ + pg_margin_bottom;
#ਜੇ_ਅੰਤ

    Pt_ਸੈੱਟ_Resource(gui.vim_ਖਿੜਕੀ, Pt_ARG_MINIMUM_DIM, &min_ਮਾਪ, 0);
    Pt_ਸੈੱਟ_Resource(gui.vim_ਖਿੜਕੀ, Pt_ARG_DIM, &ਖਿੜਕੀ_ਮਾਪ, 0);

    ਜੇ (! PtWidgetIsRealized(gui.vim_ਖਿੜਕੀ))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    ਖਾਲੀ
gui_mch_get_screen_dimensions(ਪੂਰਨ *screen_w, ਪੂਰਨ *screen_h)
{
    PhRect_t console;

    Ph_ਖਿੜਕੀ_Query_Visible(Ph_QUERY_WORKSPACE, 0,
	    Ph_ਨਿਵੇਸ਼_ਸਮੂਹ(ਨਿਰਾ), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    ਖਾਲੀ
gui_mch_iconify(ਖਾਲੀ)
{
    Ph_ਖਿੜਕੀ_Event_t event;

    CLEAR_ਖੇਤਰ(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vim_ਖਿੜਕੀ);
    Pt_Forward_ਖਿੜਕੀ_Event(&event);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_foreground(ਖਾਲੀ)
{
    Ph_ਖਿੜਕੀ_Event_t event;

    CLEAR_ਖੇਤਰ(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vim_ਖਿੜਕੀ);
    Pt_Forward_ਖਿੜਕੀ_Event(&event);
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
    gui_ph_pg_ਸੈੱਟ_ਬਫਰ_ਅੰਕ(curwin->w_ਬਫਰ->b_fnum);
#ਜੇ_ਅੰਤ
    Pt_ਸੈੱਟ_Resource(gui.vim_ਖਿੜਕੀ, Pt_ARG_ਖਿੜਕੀ_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_thumb(scrollbar_T *sb, ਪੂਰਨ val, ਪੂਰਨ ਮਾਪ, ਪੂਰਨ max)
{
    ਪੂਰਨ	    n = 0;
    PtArg_t ਦਲੀਲਾਂ[3];

    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਵੱਧ_ਤੋਂ_ਵੱਧ, max, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_SLIDER_ਮਾਪ, ਮਾਪ, 0);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_GAUGE_ਮੁੱਲ, val, 0);
    Pt_ਸੈੱਟ_Resources(sb->id, n, ਦਲੀਲਾਂ);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_pos(scrollbar_T *sb, ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    Pt_ਸੈੱਟ_Resource(sb->id, Pt_ARG_AREA, &area, 0);
}

    ਪੂਰਨ
gui_mch_get_scrollbar_xpadding(ਖਾਲੀ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ਵਾਪਸ 0;
}

    ਪੂਰਨ
gui_mch_get_scrollbar_ypadding(ਖਾਲੀ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ਵਾਪਸ 0;
}

    ਖਾਲੀ
gui_mch_create_scrollbar(scrollbar_T *sb, ਪੂਰਨ orient)
{
    ਪੂਰਨ	    n = 0;
//    int	    anchor_flags = 0;
    PtArg_t ਦਲੀਲਾਂ[4];

    /*
     * Stop the scrollbar from being realized when the parent
     * is realized, so it can be explicitly realized by vim.
     *
     * Also, don't let the scrollbar get focus
     */
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE,
	    Pt_DELAY_REALIZE | Pt_GETS_FOCUS);
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_SCROLLBAR_FLAGS, Pt_SCROLLBAR_SHOW_ARROWS, 0);
#ਜੇ 0
    // Don't need this anchoring for the scrollbars
    ਜੇ (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_ਖੱਬਾ_ANCHORED_ਖੱਬਾ | Pt_ਸੱਜਾ_ANCHORED_ਸੱਜਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_ਸਿਖਰ_ANCHORED_ਸਿਖਰ;
	ਜੇ (sb->wp != ਨਿਰਾ)
	{
	    ਜੇ (sb == &sb->wp->w_scrollbars[ SBAR_ਖੱਬਾ ])
		anchor_flags |= Pt_ਖੱਬਾ_ANCHORED_ਖੱਬਾ;
	    ਨਹੀਂਤਾਂ
		anchor_flags |= Pt_ਸੱਜਾ_ANCHORED_ਸੱਜਾ;
	}
    }
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#ਜੇ_ਅੰਤ
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PANEL_ਸਮੂਹ
    sb->id = PtCreateWidget(PtScrollbar, gui.vim_Panel_ਸਮੂਹ, n, ਦਲੀਲਾਂ);
#ਨਹੀਂਤਾਂ
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, ਦਲੀਲਾਂ);
#ਜੇ_ਅੰਤ

    Pt_ਜੋੜੋ_Callback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    ਖਾਲੀ
gui_mch_enable_scrollbar(scrollbar_T *sb, ਪੂਰਨ flag)
{
    ਜੇ (flag != 0)
	PtRealizeWidget(sb->id);
    ਨਹੀਂਤਾਂ
	PtUnrealizeWidget(sb->id);
}

    ਖਾਲੀ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = ਨਿਰਾ;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#ਜੇ defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ਸਥਿਰ ਪੂਰਨ mshape_ids[] =
{
    Ph_CURSOR_ਸੂਚਕ,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_ਪਾਓ,		// beam
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
    Ph_CURSOR_QUESTION_ਬਿੰਦੂ,	// question
    Ph_CURSOR_ਸੂਚਕ,		// right-arrow
    Ph_CURSOR_ਸੂਚਕ,		// up-arrow
    Ph_CURSOR_ਸੂਚਕ		// last one
};

    ਖਾਲੀ
mch_ਸੈੱਟ_mouse_shape(ਪੂਰਨ shape)
{
    ਪੂਰਨ	    id;

    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    ਜੇ (shape == MSHAPE_HIDE || gui.ਸੂਚਕ_hidden)
	Pt_ਸੈੱਟ_Resource(gui.vim_ਪਾਠ_Area, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_ਸੂਚਕ;
	ਨਹੀਂਤਾਂ
	    id = mshape_ids[shape];

	Pt_ਸੈੱਟ_Resource(gui.vim_ਪਾਠ_Area, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    ਜੇ (shape != MSHAPE_HIDE)
	ਆਖਰੀ_shape = shape;
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_mousehide(ਪੂਰਨ hide)
{
    ਜੇ (gui.ਸੂਚਕ_hidden == hide)
	ਵਾਪਸ;

    gui.ਸੂਚਕ_hidden = hide;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
    ਜੇ (hide)
	Pt_ਸੈੱਟ_Resource(gui.vim_ਪਾਠ_Area, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    ਨਹੀਂਤਾਂ
	mch_ਸੈੱਟ_mouse_shape(ਆਖਰੀ_shape);
#ਨਹੀਂਤਾਂ
    Pt_ਸੈੱਟ_Resource(gui.vim_ਪਾਠ_Area, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
gui_mch_getmouse(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    PhCursorInfo_t info;
    ਛੋਟਾ ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_ਨਿਵੇਸ਼_ਸਮੂਹ(ਨਿਰਾ), &info);
    PtGetAbsPosition(gui.vim_ਪਾਠ_Area , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    ਖਾਲੀ
gui_mch_setmouse(ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਛੋਟਾ abs_x, abs_y;

    PtGetAbsPosition(gui.vim_ਪਾਠ_Area, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_ਨਿਵੇਸ਼_ਸਮੂਹ(ਨਿਰਾ), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ਵਾਪਸ (guicolor_T)(PgRGB(Pg_Red_ਮੁੱਲ(pixel),
				     Pg_Green_ਮੁੱਲ(pixel), Pg_Blue_ਮੁੱਲ(pixel)));
}

    ਖਾਲੀ
gui_mch_new_colors(ਖਾਲੀ)
{
#ਜੇ 0 // Don't bother changing the cursor colour
    ਛੋਟਾ color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.ਪਿੱਠ_pixel);

    ਜੇ (ਨਿਰਪੇਖ(color_diff) < 64)
    {
	ਛੋਟਾ r, g, b;
	// not a great algorithm...
	r = Pg_Red_ਮੁੱਲ(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_ਮੁੱਲ(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_ਮੁੱਲ(gui_ph_mouse_color) ^ 255;

# ifndef FEAT_MOUSESHAPE
	gui_ph_mouse_color = PgRGB(r, g, b);
	Pt_ਸੈੱਟ_Resource(gui.vim_ਪਾਠ_Area, Pt_ARG_CURSOR_COLOR,
		gui_ph_mouse_color, 0);
# endif
    }
#ਜੇ_ਅੰਤ

    Pt_ਸੈੱਟ_Resource(gui.vim_ਪਾਠ_Area, Pt_ARG_FILL_COLOR, gui.ਪਿੱਠ_pixel, 0);
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
    ਵਾਪਸ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
    ਵਾਪਸ gui_get_rgb_color_cmn(r, g, b);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fg_color(guicolor_T color)
{
    Pg_ਸੈੱਟ_ਪਾਠ_Color(color);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_bg_color(guicolor_T color)
{
    Pg_ਸੈੱਟ_Fill_Color(color);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_sp_color(guicolor_T color)
{
}

    ਖਾਲੀ
gui_mch_invert_rectangle(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ nr, ਪੂਰਨ nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(ਖਾਨਾ);
    rect.ul.y = FILL_Y(ਪੰਕਤੀ);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_ਚੌੜਾਈ;
    rect.lr.y = rect.ul.y + nr * gui.char_ਉਚਾਈ;
    ਜੇ (nc > 0)
	rect.lr.x -= 1;
    ਜੇ (nr > 0)
	rect.lr.y -= 1;

    DRAW_ਸ਼ੁਰੂ;
    Pg_ਸੈੱਟ_Draw_ਬਹੁਲਕ(Pg_Draw_ਬਹੁਲਕ_DSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    Pg_ਸੈੱਟ_Draw_ਬਹੁਲਕ(Pg_Draw_ਬਹੁਲਕ_SRCCOPY);
    DRAW_ਸਿਰਾ;
}

    ਖਾਲੀ
gui_mch_clear_ਖੰਡ(ਪੂਰਨ row1, ਪੂਰਨ col1, ਪੂਰਨ row2, ਪੂਰਨ col2)
{
    PhRect_t ਖੰਡ = {
	{ FILL_X(col1), FILL_Y(row1) },
	{ FILL_X(col2 + 1) - 1, FILL_Y(row2 + 1) - 1}
    };

    DRAW_ਸ਼ੁਰੂ;
    gui_mch_ਸੈੱਟ_bg_color(gui.ਪਿੱਠ_pixel);
    PgDrawRect(&ਖੰਡ, Pg_DRAW_FILL);
    DRAW_ਸਿਰਾ;
}

    ਖਾਲੀ
gui_mch_clear_all(ਖਾਲੀ)
{
    PhRect_t ਪਾਠ_rect = {
	{ gui.border_ਚੌੜਾਈ, gui.border_ਚੌੜਾਈ },
	{ Columns * gui.char_ਚੌੜਾਈ + gui.border_ਚੌੜਾਈ - 1 ,
	    ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ + gui.border_ਚੌੜਾਈ - 1 }
    };

    ਜੇ (is_ignore_draw == TRUE)
	ਵਾਪਸ;

    DRAW_ਸ਼ੁਰੂ;
    gui_mch_ਸੈੱਟ_bg_color(gui.ਪਿੱਠ_pixel);
    PgDrawRect(&ਪਾਠ_rect, Pg_DRAW_FILL);
    DRAW_ਸਿਰਾ;
}

    ਖਾਲੀ
gui_mch_delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    PhRect_t    rect;
    Ph_ਬਿੰਦੂ_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ਖੱਬਾ);
    rect.ul.y = FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ);

    rect.lr.x = FILL_X(gui.scroll_region_ਸੱਜਾ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    Pt_Widget_ਵਿੱਥ(gui.vim_ਪਾਠ_Area, &gui_ph_raw_ਵਿੱਥ);
    Ph_Translate_ਬਿੰਦੂ(&gui_ph_raw_ਵਿੱਥ, PtWidgetPos(gui.vim_ਪਾਠ_Area, ਨਿਰਾ));
    PhTranslateRect(&rect, &gui_ph_raw_ਵਿੱਥ);

    delta.x = 0;
    delta.y = -ਅੰਕ_ਸਤਰਾਂ * gui.char_ਉਚਾਈ;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ਲੱਭੋ_Disjoint(gui.vim_ਪਾਠ_Area)), &rect, &delta);

    gui_clear_ਖੰਡ(
	gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1,
	gui.scroll_region_ਖੱਬਾ,
	gui.scroll_region_bot,
	gui.scroll_region_ਸੱਜਾ);
}

    ਖਾਲੀ
gui_mch_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    PhRect_t    rect;
    Ph_ਬਿੰਦੂ_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ਖੱਬਾ);
    rect.ul.y = FILL_Y(ਪੰਕਤੀ);

    rect.lr.x = FILL_X(gui.scroll_region_ਸੱਜਾ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1) - 1;

    Pt_Widget_ਵਿੱਥ(gui.vim_ਪਾਠ_Area, &gui_ph_raw_ਵਿੱਥ);
    Ph_Translate_ਬਿੰਦੂ(&gui_ph_raw_ਵਿੱਥ, PtWidgetPos(gui.vim_ਪਾਠ_Area, ਨਿਰਾ));
    PhTranslateRect(&rect, &gui_ph_raw_ਵਿੱਥ);

    delta.x = 0;
    delta.y = ਅੰਕ_ਸਤਰਾਂ * gui.char_ਉਚਾਈ;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ਲੱਭੋ_Disjoint(gui.vim_ਪਾਠ_Area)) , &rect, &delta);

    gui_clear_ਖੰਡ(ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
	    ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ - 1, gui.scroll_region_ਸੱਜਾ);
}

    ਖਾਲੀ
gui_mch_draw_string(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, char_u *s, ਪੂਰਨ len, ਪੂਰਨ flags)
{
    ਸਥਿਰ ਅੱਖਰ *utf8_ਬਫਰ = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	utf8_len = 0;

    Ph_ਬਿੰਦੂ_t	pos = { ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ) };
    PhRect_t	rect;

    ਜੇ (is_ignore_draw == TRUE)
	ਵਾਪਸ;

    DRAW_ਸ਼ੁਰੂ;

    ਜੇ (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(ਖਾਨਾ), FILL_Y(ਪੰਕਤੀ),
		FILL_X(ਖਾਨਾ + len) - 1, FILL_Y(ਪੰਕਤੀ + 1) - 1,
		Pg_DRAW_FILL);
    }

    ਜੇ (flags & DRAW_UNDERL)
	Pg_ਸੈੱਟ_Underline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    ਜੇ (charset_translate != ਨਿਰਾ && enc_utf8 == 0)
    {
	ਪੂਰਨ src_ਲਿਆ, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	ਜੇ (utf8_len < len)
	{
	    ਅੱਖਰ *new_ਬਫਰ = realloc(utf8_ਬਫਰ, len * MB_LEN_MAX);

	    ਜੇ (new_ਬਫਰ == ਨਿਰਾ)
		ਵਾਪਸ;
	    utf8_ਬਫਰ = new_ਬਫਰ;
	    utf8_len = len;
	}

	Px_Translate_ਮੰਜ਼ਿਲ_UTF(
		charset_translate,
		s,
		len,
		&src_ਲਿਆ,
		utf8_ਬਫਰ,
		utf8_len,
		&dst_made);
	s = utf8_ਬਫਰ;
	len = dst_made;
    }

    Pg_Draw_ਪਾਠ(s, len, &pos, 0);

    ਜੇ (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(ਖਾਨਾ) + 1;
	rect.ul.y = FILL_Y(ਪੰਕਤੀ);
	rect.lr.x = FILL_X(ਖਾਨਾ + len) - 1;
	rect.lr.y = FILL_Y(ਪੰਕਤੀ + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#ਜੇ 0
	pos.x++;

	Pg_ਸੈੱਟ_User_Clip(&rect);
	Pg_Draw_ਪਾਠ(s, len, &pos, 0);
	Pg_ਸੈੱਟ_User_Clip(ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	Pg_Draw_ਪਾਠ_Area(s, len, &rect, Pg_ਪਾਠ_BOTTOM);
#ਜੇ_ਅੰਤ
    }

    ਜੇ (flags & DRAW_UNDERL)
	Pg_ਸੈੱਟ_Underline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_ਸਿਰਾ;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    ਖਾਲੀ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    PhRect_t r;

    // FIXME: Double width characters

    r.ul.x = FILL_X(gui.ਖਾਨਾ);
    r.ul.y = FILL_Y(gui.ਪੰਕਤੀ);
    r.lr.x = r.ul.x + gui.char_ਚੌੜਾਈ - 1;
    r.lr.y = r.ul.y + gui.char_ਉਚਾਈ - 1;

    DRAW_ਸ਼ੁਰੂ;
    Pg_ਸੈੱਟ_Stroke_Color(color);
    PgDrawRect(&r, Pg_DRAW_STROKE);
    DRAW_ਸਿਰਾ;
}

    ਖਾਲੀ
gui_mch_draw_part_cursor(ਪੂਰਨ w, ਪੂਰਨ h, guicolor_T color)
{
    PhRect_t r;

    r.ul.x = FILL_X(gui.ਖਾਨਾ);
    r.ul.y = FILL_Y(gui.ਪੰਕਤੀ) + gui.char_ਉਚਾਈ - h;
    r.lr.x = r.ul.x + w - 1;
    r.lr.y = r.ul.y + h - 1;

    DRAW_ਸ਼ੁਰੂ;
    gui_mch_ਸੈੱਟ_bg_color(color);
    PgDrawRect(&r, Pg_DRAW_FILL);
    DRAW_ਸਿਰਾ;
}

    ਪੂਰਨ
gui_mch_is_blinking(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state != BLINK_NONE;
}

    ਪੂਰਨ
gui_mch_is_blink_off(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state == BLINK_OFF;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_blinking(ਲੰਮਾ wait, ਲੰਮਾ on, ਲੰਮਾ off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ਖਾਲੀ
gui_mch_ਸ਼ੁਰੂ_blink(ਖਾਲੀ)
{
    // Only turn on the timer on if none of the times are zero
    ਜੇ (blink_waittime && blink_ontime && blink_offtime && gui.ਵਿੱਚ_focus)
    {
	Pt_ਸੈੱਟ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ਖਾਲੀ
gui_mch_stop_blink(ਪੂਰਨ may_call_gui_update_cursor)
{
    Pt_ਸੈੱਟ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    ਜੇ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    ਖਾਲੀ
gui_mch_beep(ਖਾਲੀ)
{
    PtBeep();
}

    ਖਾਲੀ
gui_mch_flash(ਪੂਰਨ msec)
{
    Pg_ਸੈੱਟ_Fill_XORColor(Pg_BLACK, Pg_WHITE);
    Pg_ਸੈੱਟ_Draw_ਬਹੁਲਕ(Pg_DRAWMODE_XOR);
    gui_mch_clear_all();
    gui_mch_flush();

    ui_delay((ਲੰਮਾ) msec, TRUE);

    gui_mch_clear_all();
    Pg_ਸੈੱਟ_Draw_ਬਹੁਲਕ(Pg_DRAWMODE_OPAQUE);
    gui_mch_flush();
}

    ਖਾਲੀ
gui_mch_flush(ਖਾਲੀ)
{
    PgFlush();
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_ਪਾਠ_area_pos(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    PhArea_t area = {{x, y}, {w, h}};

    Pt_ਸੈੱਟ_Resource(gui.vim_ਪਾਠ_Area, Pt_ARG_AREA, &area, 0);
}

    ਪੂਰਨ
gui_mch_haskey(char_u *name)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != 0; i++)
	ਜੇ (name[0] == special_ਕੁੰਜੀਆਂ[i].vim_code0 &&
		 name[1] == special_ਕੁੰਜੀਆਂ[i].vim_code1)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
# include "toolbar.phi"

ਸਥਿਰ PhImage_t *gui_ph_toolbar_images[] = {
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
    &tb_ਲੱਭੋ_phi,
    &tb_save_all_phi,
    &tb_save_session_phi,
    &tb_new_session_phi,
    &tb_ਲੱਦੋ_session_phi,
    &tb_macro_phi,
    &tb_replace_phi,
    &tb_close_phi,
    &tb_maximize_phi,
    &tb_minimize_phi,
    &tb_ਵੰਡੋ_phi,
    &tb_shell_phi,
    &tb_ਲੱਭੋ_prev_phi,
    &tb_ਲੱਭੋ_ਅਗਲਾ_phi,
    &tb_ਲੱਭੋ_help_phi,
    &tb_make_phi,
    &tb_jump_phi,
    &tb_ctags_phi,
    &tb_vsplit_phi,
    &tb_maxwidth_phi,
    &tb_minwidth_phi
};

ਸਥਿਰ PhImage_t *
gui_ph_toolbar_ਲੱਦੋ_icon(char_u *iconfile)
{
    ਸਥਿਰ PhImage_t external_icon;
    PhImage_t *ਆਰਜ਼ੀ_phi = ਨਿਰਾ;

    ਆਰਜ਼ੀ_phi = Px_ਲੱਦੋ_Image(iconfile, ਨਿਰਾ);
    ਜੇ (ਆਰਜ਼ੀ_phi != ਨਿਰਾ)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	ਆਰਜ਼ੀ_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&external_icon, ਆਰਜ਼ੀ_phi, ਆਕਾਰ(external_icon));
	ਮੁਕਤ(ਆਰਜ਼ੀ_phi);

	ਆਰਜ਼ੀ_phi = &external_icon;
    }
    ਵਾਪਸ ਆਰਜ਼ੀ_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ਸਥਿਰ PhImage_t *
gui_ph_toolbar_ਲੱਭੋ_icon(vimmenu_T *menu)
{
    char_u ਭਰਿਆ_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = ਨਿਰਾ;

    ਜੇ (menu->icon_builtin == FALSE)
    {
	ਜੇ (menu->iconfile != ਨਿਰਾ)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_ਲੱਦੋ_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	ਜੇ ((icon == ਨਿਰਾ) &&
	    ((gui_ਲੱਭੋ_bitmap(menu->name, ਭਰਿਆ_pathname, "gif") == OK) ||
	      (gui_ਲੱਭੋ_bitmap(menu->name, ਭਰਿਆ_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_ਲੱਦੋ_icon(ਭਰਿਆ_pathname);

	ਜੇ (icon != ਨਿਰਾ)
	    ਵਾਪਸ icon;
    }

    ਜੇ (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_ਲੰਮਾਈ(gui_ph_toolbar_images)))
	ਵਾਪਸ gui_ph_toolbar_images[menu->iconidx];

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_MENU)
    ਖਾਲੀ
gui_mch_enable_menu(ਪੂਰਨ flag)
{
    ਜੇ (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    ਨਹੀਂਤਾਂ
	PtUnrealizeWidget(gui.vimMenuBar);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_menu_pos(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ਸਥਿਰ ਖਾਲੀ
gui_ph_position_menu(PtWidget_t *widget, ਪੂਰਨ ਤਰਜੀਹ)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = Pt_Widget_ਬੱਚਾ_ਪਿੱਠ(Pt_Widget_ਮਾਪਾ(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    ਜਦੋਂ (traverse != ਨਿਰਾ)
    {
	PtGetResource(traverse, Pt_ARG_ਸੂਚਕ, &menu, 0);

	ਜੇ (menu != ਨਿਰਾ &&
		ਤਰਜੀਹ < menu->ਤਰਜੀਹ &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_ਪਾਓ(widget, traverse, 1);
	    ਵਾਪਸ;
	}

	traverse = Pt_Widget_Brother_ਵਿੱਚ_Front(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu(vimmenu_T *menu, ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;
    char_u	*accel_ਕੁੰਜੀ;
    char_u	mnemonic_str[MB_LEN_MAX];
    ਪੂਰਨ	    n;
    PtArg_t ਦਲੀਲਾਂ[5];

    menu->submenu_id = menu->id = ਨਿਰਾ;

    ਜੇ (menu_is_menubar(menu->name))
    {

	accel_ਕੁੰਜੀ = vim_strchr(menu->name, '&');
	ਜੇ (accel_ਕੁੰਜੀ != ਨਿਰਾ)
	{
	    mnemonic_str[0] = accel_ਕੁੰਜੀ[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਪਾਠ_STRING, menu->dname, 0);
	Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ACCEL_ਪਾਠ, menu->actext, 0);
	ਜੇ (accel_ਕੁੰਜੀ != ਨਿਰਾ)
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ACCEL_ਕੁੰਜੀ, mnemonic_str, 0);
	Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਸੂਚਕ, menu, 0);

	ਜੇ (ਮਾਪਾ != ਨਿਰਾ)
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_ਸੱਜਾ, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(ਮਾਪਾ == ਨਿਰਾ) ? gui.vimMenuBar : ਮਾਪਾ->submenu_id,
		n, ਦਲੀਲਾਂ);

	Pt_ਜੋੜੋ_Callback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	ਜੇ (ਮਾਪਾ != ਨਿਰਾ)
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_ਬੱਚਾ);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, ਦਲੀਲਾਂ);

	ਜੇ (ਮਾਪਾ == ਨਿਰਾ)
	{
	    Pt_ਜੋੜੋ_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    ਜੇ (menu->mnemonic != 0)
	    {
		Pt_ਜੋੜੋ_Hotkey_Handler(gui.vim_ਖਿੜਕੀ, ਸੁਰੱਖਿਅਤ_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->ਤਰਜੀਹ);

	// Redraw menubar here instead of gui_mch_draw_menubar
	ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	    PtRealizeWidget(menu->id);
    }
    ਨਹੀਂਤਾਂ ਜੇ (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vim_ਖਿੜਕੀ, 0, ਨਿਰਾ);
	Pt_ਜੋੜੋ_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(vimmenu_T *menu, ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;
    char_u	*accel_ਕੁੰਜੀ;
    char_u	mnemonic_str[MB_LEN_MAX];
    ਪੂਰਨ	    n;
    PtArg_t ਦਲੀਲਾਂ[13];

    n = 0;
    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਸੂਚਕ, menu, 0);

# ifdef FEAT_TOOLBAR
    ਜੇ (menu_is_toolbar(ਮਾਪਾ->name))
    {
	ਜੇ (menu_is_separator(menu->name))
	{
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_ਵਿੱਚ, 0);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_ਸਿਖਰ_BOTTOM);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਚੌੜਾਈ, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, ਦਲੀਲਾਂ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *) p_toolbar, "text") != ਨਿਰਾ)
	    {
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਪਾਠ_STRING, menu->dname, 0);
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਪਾਠ_FONT, "TextFont08", 0);
	    }
	    ਜੇ ((ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *) p_toolbar, "icons") != ਨਿਰਾ) &&
		    (gui_ph_toolbar_images != ਨਿਰਾ))
	    {
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_ਲੱਭੋ_icon(menu), 0);
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_LABEL_TYPE, Pt_ਪਾਠ_IMAGE, 0);
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਪਾਠ_IMAGE_SPACING, 0, 0);
	    }
	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *) p_toolbar, "tooltips") != ਨਿਰਾ)
	    {
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_LABEL_BALLOON,
			gui_ph_show_tooltip, 0);
		Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_LABEL_FLAGS,
			Pt_TRUE, Pt_SHOW_BALLOON);
	    }
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_MARGIN_ਉਚਾਈ, 1, 0);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_MARGIN_ਚੌੜਾਈ, 1, 0);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_FLAGS, Pt_FALSE,
		    Pt_HIGHLIGHTED | Pt_GETS_FOCUS);
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_FILL_COLOR, Pg_TRANSPARENT, 0);
	    menu->id = PtCreateWidget(PtButton, gui.vimToolBar, n, ਦਲੀਲਾਂ);

	    Pt_ਜੋੜੋ_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);
	}
	// Update toolbar if it's open
	ਜੇ (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    ਨਹੀਂਤਾਂ
# endif
	ਜੇ (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, ਮਾਪਾ->submenu_id, n, ਦਲੀਲਾਂ);
    }
    ਨਹੀਂਤਾਂ
    {
	accel_ਕੁੰਜੀ = vim_strchr(menu->name, '&');
	ਜੇ (accel_ਕੁੰਜੀ != ਨਿਰਾ)
	{
	    mnemonic_str[0] = accel_ਕੁੰਜੀ[1];
	    mnemonic_str[1] = NUL;
	}

	Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ਪਾਠ_STRING, menu->dname, 0);
	ਜੇ (accel_ਕੁੰਜੀ != ਨਿਰਾ)
	    Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ACCEL_ਕੁੰਜੀ, mnemonic_str,
		    0);

	Pt_ਸੈੱਟ_Arg(&ਦਲੀਲਾਂ[ n++ ], Pt_ARG_ACCEL_ਪਾਠ, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, ਮਾਪਾ->submenu_id, n, ਦਲੀਲਾਂ);

	Pt_ਜੋੜੋ_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_ਸਮੂਹ
	ਜੇ (gui_ph_is_ਬਫਰ_ਵਸਤੂ(menu, ਮਾਪਾ) == TRUE)
	{
	    Pt_ਜੋੜੋ_Callback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_ਬਫਰ_remove, menu);
	    gui_ph_pg_ਜੋੜੋ_ਬਫਰ(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->ਤਰਜੀਹ);
}

    ਖਾਲੀ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    ਜੇ (menu->submenu_id != ਨਿਰਾ)
	PtDestroyWidget(menu->submenu_id);
    ਜੇ (menu->id != ਨਿਰਾ)
	PtDestroyWidget(menu->id);

    menu->submenu_id = ਨਿਰਾ;
    menu->id = ਨਿਰਾ;
}

    ਖਾਲੀ
gui_mch_menu_grey(vimmenu_T *menu, ਪੂਰਨ grey)
{
    ਲੰਮਾ    flags, ਨਕਾਬ, fields;

    ਜੇ (menu->id == ਨਿਰਾ)
	ਵਾਪਸ;

    flags = PtWidgetFlags(menu->id);
    ਜੇ (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(Pt_Widget_ਮਾਪਾ(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	ਨਕਾਬ = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    ਨਹੀਂਤਾਂ
    {
	fields = Pt_TRUE;
	ਨਕਾਬ = Pt_BLOCKED | Pt_GHOST;
    }

    ਜੇ (! grey)
	fields = ~fields;

    Pt_ਸੈੱਟ_Resource(menu->id, Pt_ARG_FLAGS, fields,
	    ਨਕਾਬ);
}

    ਖਾਲੀ
gui_mch_menu_hidden(vimmenu_T *menu, ਪੂਰਨ hidden)
{
    // TODO: [un]realize the widget?
}

    ਖਾਲੀ
gui_mch_draw_menubar(ਖਾਲੀ)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    ਖਾਲੀ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    Pt_ਸੈੱਟ_Resource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    ਖਾਲੀ
gui_mch_toggle_tearoffs(ਪੂਰਨ enable)
{
    // No tearoffs yet
}

#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR)
    ਖਾਲੀ
gui_mch_show_toolbar(ਪੂਰਨ showit)
{
    ਜੇ (showit)
	PtRealizeWidget(gui.vimToolBar);
    ਨਹੀਂਤਾਂ
	PtUnrealizeWidget(gui.vimToolBar);
}
#ਜੇ_ਅੰਤ

////////////////////////////////////////////////////////////////////////////
// Fonts

    ਸਥਿਰ GuiFont
gui_ph_get_font(
	char_u	*font_name,
	int_u	font_flags,
	int_u	font_ਮਾਪ_2,
	// Check whether the resulting font has the font flags and size that
	// was asked for
	int_u	enforce
	)
{
    char_u	    *font_ਟੈਗ;
    FontQueryInfo   info;
    int_u	    style;

    font_ਟੈਗ = alloc(MAX_FONT_ਟੈਗ);
    ਜੇ (font_ਟੈਗ != ਨਿਰਾ)
    {
	ਜੇ (PfGenerateFontName(font_name, font_flags, font_ਮਾਪ_2,
		    font_ਟੈਗ) != ਨਿਰਾ)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    ਜੇ (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    ਜੇ (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    ਜੇ (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_ਟੈਗ, &info);

	    ਜੇ (info.ਮਾਪ == 0)
		font_ਮਾਪ_2 = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    ਜੇ (font_ਮਾਪ_2 == info.ਮਾਪ &&
		    style == (info.style & style))
		ਵਾਪਸ (GuiFont)font_ਟੈਗ;
	}
	vim_free(font_ਟੈਗ);
    }
    ਵਾਪਸ ਨਿਰਾ;
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

    ਸਥਿਰ ਪੂਰਨ
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_ਮਾਪ_2)
{
    char_u  *mark;
    int_u   name_len, ਮਾਪ;

    mark = vim_strchr(vim_font, ':');
    ਜੇ (mark == ਨਿਰਾ)
	name_len = STRLEN(vim_font);
    ਨਹੀਂਤਾਂ
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    ਜੇ (*font_name != ਨਿਰਾ)
    {
	ਜੇ (mark != ਨਿਰਾ)
	{
	    ਜਦੋਂ (*mark != NUL && *mark++ == ':')
	    {
		ਚੋਣ (ਸੁਰੱਖਿਅਤ_tolower(*mark++))
		{
		    ਹਾਲਤ 'a': *font_flags |= PF_STYLE_ANTIALIAS; ਰੋਕੋ;
		    ਹਾਲਤ 'b': *font_flags |= PF_STYLE_BOLD; ਰੋਕੋ;
		    ਹਾਲਤ 'i': *font_flags |= PF_STYLE_ITALIC; ਰੋਕੋ;

		    ਹਾਲਤ 's':
			ਮਾਪ = getdigits(&mark);
			// Restrict the size to some vague limits
			ਜੇ (ਮਾਪ < 1 || ਮਾਪ > 100)
			    ਮਾਪ = 8;

			*font_ਮਾਪ_2 = ਮਾਪ;
			ਰੋਕੋ;

		    ਮੂਲ:
			ਰੋਕੋ;
		}
	    }
	}
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
gui_mch_init_font(char_u *vim_font_name, ਪੂਰਨ fontset)
{
    char_u  *font_ਟੈਗ;
    char_u  *font_name = ਨਿਰਾ;
    int_u   font_flags = 0;
    int_u   font_ਮਾਪ_2  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    ਜੇ (vim_font_name == ਨਿਰਾ)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    ਜੇ (STRCMP(vim_font_name, "*") == 0)
    {
	font_ਟੈਗ = PtFontSelection(gui.vim_ਖਿੜਕੀ, ਨਿਰਾ, ਨਿਰਾ,
		"pcterm12", -1, PHFONT_FIXED, ਨਿਰਾ);

	ਜੇ (font_ਟੈਗ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ਟੈਗ;

	PfQueryFontInfo(font_ਟੈਗ, &info);
	font_name = vim_strsave(info.font);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_ਮਾਪ_2) == FALSE)
	    ਵਾਪਸ FAIL;

	font_ਟੈਗ = gui_ph_get_font(font_name, font_flags, font_ਮਾਪ_2, 0);
	ਜੇ (font_ਟੈਗ == ਨਿਰਾ)
	{
	    vim_free(font_name);
	    ਵਾਪਸ FAIL;
	}

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ਟੈਗ;
    }

    gui_mch_free_font(gui.bold_font);
    gui.bold_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_BOLD,
	    font_ਮਾਪ_2, PF_STYLE_BOLD);

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_ITALIC,
	    font_ਮਾਪ_2, PF_STYLE_ITALIC);

    // This extent was brought to you by the letter 'g'
    Pf_Extent_ਪਾਠ(&extent, ਨਿਰਾ, font_ਟੈਗ, "g", 1);

    gui.char_ਚੌੜਾਈ = extent.lr.x - extent.ul.x + 1;
    gui.char_ਉਚਾਈ = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    ਵਾਪਸ OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ਪੂਰਨ
gui_mch_adjust_charheight(ਖਾਲੀ)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_ਉਚਾਈ = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    ਵਾਪਸ OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, ਪੂਰਨ report_error)
{
    char_u  *font_name;
    char_u  *font_ਟੈਗ;
    int_u   font_ਮਾਪ_2 = 12;
    int_u   font_flags = 0;

    ਜੇ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_ਮਾਪ_2) != FALSE)
    {
	font_ਟੈਗ = gui_ph_get_font(font_name, font_flags, font_ਮਾਪ_2, -1);
	vim_free(font_name);

	ਜੇ (font_ਟੈਗ != ਨਿਰਾ)
	    ਵਾਪਸ (GuiFont)font_ਟੈਗ;
    }

    ਜੇ (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    ਵਾਪਸ FAIL;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strsave(name);
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_font(GuiFont font)
{
    Pg_ਸੈੱਟ_Font(font);
}

    ਖਾਲੀ
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

