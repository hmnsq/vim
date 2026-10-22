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

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#ಯದಿ !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	ಪೂರ್ಣಾಙ್ಕ
# define PhEvent_t	ಪೂರ್ಣಾಙ್ಕ
# define PhPoint_t	ಪೂರ್ಣಾಙ್ಕ
# define PtWidget_t	ಪೂರ್ಣಾಙ್ಕ
# define Pg_BLACK	0
# define PtCallbackF_t	ಪೂರ್ಣಾಙ್ಕ
# define PtCallbackInfo_t ಪೂರ್ಣಾಙ್ಕ
# define PhTile_t	ಪೂರ್ಣಾಙ್ಕ
# define PtWidget_t	ಪೂರ್ಣಾಙ್ಕ
# define PhImage_t	ಪೂರ್ಣಾಙ್ಕ
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE ಪರಿಮಾಣ(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
ಸ್ಥಿರ ವರ್ಣ	*empty_title = "    ";
ಸ್ಥಿರ ವರ್ಣ	**panel_titles = ರಿಕ್ತ;
ಸ್ಥಿರ ushort_t	ಅಂಕೆ_panels = 0;
ಸ್ಥಿರ short pg_margin_ಎಡ, pg_margin_ಬಲ, pg_margin_ತುದಿ, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_ಸೇರಿಸು
ಸ್ಥಿರ PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ಸ್ಥಿರ PhPoint_t    gui_ph_raw_offset;
ಸ್ಥಿರ PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ಸ್ಥಿರ PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ಸ್ಥಿರ short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ಸ್ಥಿರ PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
ಸ್ಥಿರ ಸಂರಚನಾ PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
ಸ್ಥಿರ ಗಣನಾ {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

ಸ್ಥಿರ long_u	blink_waittime	= 700;
ಸ್ಥಿರ long_u	blink_ontime	= 400;
ಸ್ಥಿರ long_u	blink_offtime	= 250;

ಸ್ಥಿರ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ	    ಕೀಲಿ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
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
    {Pk_BackSpace,  'k', 'b'},
    {Pk_ಸೇರಿಸು,	    'k', 'I'},
    {Pk_Delete,	    'k', 'D'},
    {Pk_Home,	    'k', 'h'},
    {Pk_End,	    '@', '7'},
    {Pk_Prior,	    'k', 'P'},
    {Pk_Next,	    'k', 'N'},
    {Pk_Print,	    '%', '9'},

    {Pk_KP_Add,	    'K', '6'},
    {Pk_KP_Subtract,'K', '7'},
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
    {Pk_KP_1,	    'K', '4'}, // End

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

ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_window_cb;

ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_scrollbar;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_keyboard;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_mouse;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_pulldown_menu;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_menu;
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ಸ್ಥಿರ PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
ಸ್ಥಿರ ಶೂನ್ಯ gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

ಸ್ಥಿರ ಶೂನ್ಯ gui_ph_draw_start(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ gui_ph_draw_end(ಶೂನ್ಯ);

// Set the text for the balloon
ಸ್ಥಿರ PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     ಪೂರ್ಣಾಙ್ಕ position,
			     ವರ್ಣ *text,
			     ವರ್ಣ *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

ಸ್ಥಿರ PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     ಪೂರ್ಣಾಙ್ಕ position,
			     ವರ್ಣ *text,
			     ವರ್ಣ *font,
			     PgColor_t fill_color,
			     PgColor_t text_color)
{
    PtArg_t arg;
    vimmenu_T *menu;
    char_u  *tooltip;

    PtSetArg(&arg, Pt_ARG_POINTER, &menu, 0);
    PtGetResources(widget, 1, &arg);

    // Override the text and position

    tooltip = text;
    ಯದಿ (menu != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ index = MENU_INDEX_TIP;
	ಯದಿ (menu->strings[ index ] != ರಿಕ್ತ)
	    tooltip = menu->strings[ index ];
    }

    ಪ್ರತ್ಯಾಗಮ PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_ತುದಿ,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_resize_container(ಶೂನ್ಯ)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	ಶೂನ್ಯ *other,
	PtCallbackInfo_t *info)
{
    PtContainerCallback_t *sizes = info->cbdata;
    PtWidget_t		*container;
    PhPoint_t		below_menu;
    int_u		height;

    height = sizes->new_dim.h;

    // Because vim treats the toolbar and menubar separately,
    // and here they're lumped together into a PtToolbarGroup,
    // we only need either menu_height or toolbar_height set at once
    ಯದಿ (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    ಅನ್ಯಥಾ
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#ಅನ್ಯಥಾ
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_ತುದಿ, &pg_margin_bottom,
	    &pg_margin_ಎಡ, &pg_margin_ಬಲ);
#endif
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	ಶೂನ್ಯ *data,
	PtCallbackInfo_t *info)
{
    ಯದಿ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    ಅನ್ಯಥಾ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    ಶೂನ್ಯ		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_window_cb(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    ವಿಕಲ್ಪ (we->event_f)
    {
	ಪಕ್ಷ Ph_WM_CLOSE:
	    gui_shell_closed();
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    ಯದಿ (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    ಅನ್ಯಥಾ
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_ಎಡ + pg_margin_ಬಲ);
	    height -= (pg_margin_ತುದಿ + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_scrollbar(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    ಪೂರ್ಣಾಙ್ಕ	    ಮೌಲ್ಯ, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    ಯದಿ (sb != ರಿಕ್ತ)
    {
	ಮೌಲ್ಯ = scroll->position;
	ವಿಕಲ್ಪ (scroll->action)
	{
	    ಪಕ್ಷ Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ Pt_SCROLL_SET:
		// FIXME: return straight away here?
		ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
		ಭಙ್ಗ;
	}

	gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
    }
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_keyboard(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    Ph_ಕೀಲಿ_Event_t    *ಕೀಲಿ;
    unsigned ವರ್ಣ   string[6];
    ಪೂರ್ಣಾಙ್ಕ		    len, i;
    ಪೂರ್ಣಾಙ್ಕ		    ch, modifiers;

    ಕೀಲಿ = PhGetData(info->event);

    ch = modifiers = len = 0;

    ಯದಿ (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    ಯದಿ (ಕೀಲಿ->ಕೀಲಿ_flags & Pk_KF_Compose)
	ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;

    ಯದಿ ((ಕೀಲಿ->ಕೀಲಿ_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_ಕೀಲಿ_Down(ಕೀಲಿ->ಕೀಲಿ_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	ಯದಿ (((ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Alt) &&
		    !(ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Shift) &&
		    !(ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(ಕೀಲಿ->ಕೀಲಿ_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
	}
#endif

	ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	{
	    ಯದಿ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == ಕೀಲಿ->ಕೀಲಿ_cap)
	    {
		len = 0;
		ಯದಿ (special_ಕೀಲಿಗಳು[i].vim_code1 == NUL)
		    ch = special_ಕೀಲಿಗಳು[i].vim_code0;
		ಅನ್ಯಥಾ
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    ಯದಿ (ಕೀಲಿ->ಕೀಲಿ_cap >= Pk_KP_Enter && ಕೀಲಿ->ಕೀಲಿ_cap <= Pk_KP_9
			    && (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_ಅಂಕೆ_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = ಕೀಲಿ->ಕೀಲಿ_cap - 0xf080;
		    }
		    ಅನ್ಯಥಾ
			ch = TO_SPECIAL(special_ಕೀಲಿಗಳು[i].vim_code0,
				special_ಕೀಲಿಗಳು[i].vim_code1);
		}
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	ಯದಿ (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	ಯದಿ (ಕೀಲಿ->ಕೀಲಿ_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	ಯದಿ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == 0)
	{
	    ch = PhTo8859_1(ಕೀಲಿ);
	    ಯದಿ (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_ಕೀಲಿ_To_Mb(string, ಕೀಲಿ);
		ಯದಿ (len > 0)
		{
		    ಸ್ಥಿರ ವರ್ಣ buf[6];
		    ಪೂರ್ಣಾಙ್ಕ src_taken, dst_made;
		    ಯದಿ (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_ಒಳಹರಿವು_buf(buf, dst_made);
		    }
		    ಅನ್ಯಥಾ
		    {
			add_to_ಒಳಹರಿವು_buf(string, len);
		    }

		    ಪ್ರತ್ಯಾಗಮ Pt_CONSUME;
		}
		len = 0;
		ch = ಕೀಲಿ->ಕೀಲಿ_cap;
		ಯದಿ (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    ಯದಿ (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			ಯದಿ ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			ಅನ್ಯಥಾ ಯದಿ (ch == '2')
			    ch = NUL;
			ಅನ್ಯಥಾ ಯದಿ (ch == '6')
			    ch = 0x1e;
			ಅನ್ಯಥಾ ಯದಿ (ch == '-')
			    ch = 0x1f;
			ಅನ್ಯಥಾ
			    modifiers |= MOD_MASK_CTRL;
		    }

		    ಯದಿ (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
		}
	    }
	    ಅನ್ಯಥಾ
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_ಕೀಲಿ(ch, &modifiers);
	ಯದಿ (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	ಯದಿ (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	ಅನ್ಯಥಾ
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    ಪೂರ್ಣಾಙ್ಕ int_ch = check_for_interrupt(ch, modifiers);

	    ಯದಿ (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_ಒಳಹರಿವು_buf();
	    }
	}

	ಯದಿ (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	ಯದಿ (len > 0)
	{
	    add_to_ಒಳಹರಿವು_buf(string, len);
	    ಪ್ರತ್ಯಾಗಮ Pt_CONSUME;
	}
    }

    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_mouse(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    ಪೂರ್ಣಾಙ್ಕ		     button = 0, repeated_click, modifiers = 0x0;
    short	     mouse_x, mouse_y;

    pointer = PhGetData(info->event);
    pos = PhGetRects(info->event);

    gui_mch_mousehide(MOUSE_SHOW);

    /*
     * Coordinates need to be relative to the base window,
     * not relative to the vimTextArea widget
     */
    mouse_x = pos->ul.x + gui.border_width;
    mouse_y = pos->ul.y + gui.border_width;

    ಯದಿ (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
    }

    ಯದಿ (pointer->ಕೀಲಿ_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    ಯದಿ (pointer->ಕೀಲಿ_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    ಯದಿ (pointer->ಕೀಲಿ_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    ಯದಿ (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_ಎಡ;

    ಯದಿ (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_ಬಲ;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    ಯದಿ (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_ಮಧ್ಯ;

    // Catch a real release (not phantom or other releases
    ಯದಿ (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    ಯದಿ (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#ಯದಿ 0
    // Vim doesn't use button repeats
    ಯದಿ (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    ಯದಿ ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_ಎಣಿಕೆ >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_focus(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    ಯದಿ (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ರಿಕ್ತ);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    ಅನ್ಯಥಾ
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    ಯದಿ (is_ignore_draw == TRUE)
	ಪ್ರತ್ಯಾಗಮ;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#ಯದಿ 0
    /*
     * This causes some weird problems, with drawing being done from
     * within this raw drawing function (rather than just simple clearing
     * and text drawing done by gui_redraw)
     *
     * The main problem is when PhBlit is used, and the cursor appearing
     * in places where it shouldn't
     */
    out_flush();
#endif

    PtWidgetOffset(widget, &offset);
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, ರಿಕ್ತ));

#ಯದಿ 1
    // Redraw individual damage regions
    ಯದಿ (damage->next != ರಿಕ್ತ)
	damage = damage->next;

    ಯಾವತ್ (damage != ರಿಕ್ತ)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#ಅನ್ಯಥಾ
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	ಶೂನ್ಯ *data,
	PtCallbackInfo_t *info)
{
    ಯದಿ (data != ರಿಕ್ತ)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, ರಿಕ್ತ);
	PtRealizeWidget(menu->submenu_id);
    }

    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_menu(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    ಯದಿ (data != ರಿಕ್ತ)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	ಶೂನ್ಯ *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_window_open(
	PtWidget_t *widget,
	ಶೂನ್ಯ *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_draw_start(ಶೂನ್ಯ)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ರಿಕ್ತ));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_draw_end(ಶೂನ್ಯ)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    ಸ್ಥಿರ vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *ತುದಿ_level = ಬೇರು_menu;
    vimmenu_T *ವಸ್ತುಗಳು = ರಿಕ್ತ;

    ಯಾವತ್ (ತುದಿ_level != ರಿಕ್ತ &&
	    (STRCMP(ತುದಿ_level->dname, "Buffers") != 0))
	ತುದಿ_level = ತುದಿ_level->next;

    ಯದಿ (ತುದಿ_level != ರಿಕ್ತ)
    {
	ವಸ್ತುಗಳು = ತುದಿ_level->children;

	ಯಾವತ್ (ವಸ್ತುಗಳು != ರಿಕ್ತ &&
		(STRCMP(ವಸ್ತುಗಳು->dname, name) != 0))
	    ವಸ್ತುಗಳು = ವಸ್ತುಗಳು->next;
    }
    ಪ್ರತ್ಯಾಗಮ ವಸ್ತುಗಳು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_pg_set_buffer_ಅಂಕೆ(int_u buf_ಅಂಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ವರ್ಣ ಹುಡುಕು[16];
    ವರ್ಣ *mark;

    ಯದಿ (gui.vimTextArea == ರಿಕ್ತ || buf_ಅಂಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಹುಡುಕು[0] = '(';
    ultoa(buf_ಅಂಕೆ, &ಹುಡುಕು[1], 10);
    STRCAT(ಹುಡುಕು, ")");

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	ಯದಿ (mark != ರಿಕ್ತ && STRCMP(mark, ಹುಡುಕು) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	ಶೂನ್ಯ *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    ಯದಿ (info->event != ರಿಕ್ತ)
    {
	panel = info->cbdata;
	ಯದಿ (panel->new_panel != ರಿಕ್ತ)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    ಯದಿ (menu)
		gui_menu_cb(menu);
	}
    }
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_get_panelgroup_margins(
	short *ತುದಿ,
	short *bottom,
	short *ಎಡ,
	short *ಬಲ)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ಧ್ರುವ unsigned short *margin_ತುದಿ, *margin_bottom;
    ಧ್ರುವ unsigned short *margin_ಎಡ, *margin_ಬಲ;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_ಬಲ, &margin_ಬಲ, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *ತುದಿ    = abs_raw_y;
    *bottom = *margin_bottom;

    *ಎಡ  = abs_raw_x;
    *ಬಲ = *margin_ಬಲ;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    ವರ್ಣ *mark;

    ಯದಿ (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	ಯದಿ (mark != ರಿಕ್ತ)
	{
	    mark++;
	    ಯಾವತ್ (SAFE_isdigit(*mark))
		mark++;

	    ಯದಿ (*mark == ')')
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_pg_add_buffer(ವರ್ಣ *name)
{
    ವರ್ಣ **new_titles = ರಿಕ್ತ;

    new_titles = ALLOC_MULT(ವರ್ಣ *, (ಅಂಕೆ_panels + 1));
    ಯದಿ (new_titles == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (ಅಂಕೆ_panels > 0)
	memcpy(new_titles, panel_titles, ಅಂಕೆ_panels * ಪರಿಮಾಣ(ವರ್ಣ **));

    new_titles[ ಅಂಕೆ_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    ಅಂಕೆ_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_pg_remove_buffer(ವರ್ಣ *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ವರ್ಣ **new_titles = ರಿಕ್ತ;

    // If there is only 1 panel, we just use the temporary place holder
    ಯದಿ (ಅಂಕೆ_panels > 1)
    {
	new_titles = ALLOC_MULT(ವರ್ಣ *, ಅಂಕೆ_panels - 1);
	ಯದಿ (new_titles != ರಿಕ್ತ)
	{
	    ವರ್ಣ **s = new_titles;
	    // Copy all the titles except the one we're removing
	    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_panels; i++)
	    {
		ಯದಿ (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    ಅಂಕೆ_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    ಅಂಕೆ_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಅಂಕೆ_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	ಶೂನ್ಯ *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    ಯದಿ (data != ರಿಕ್ತ)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_pane_resize(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    ಯದಿ (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    ಶೂನ್ಯ
gui_ph_encoding_changed(ಪೂರ್ಣಾಙ್ಕ new_encoding)
{
    // Default encoding is latin1
    ವರ್ಣ *charset = "latin1";
    ಪೂರ್ಣಾಙ್ಕ i;

    ಸಂರಚನಾ {
	ಪೂರ್ಣಾಙ್ಕ encoding;
	ವರ್ಣ *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    ಪ್ರತಿ (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	ಯದಿ (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಙ್ಕ *argc, ವರ್ಣ **argv)
{
    PtInit(ರಿಕ್ತ);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init(ಶೂನ್ಯ)
{
    PtArg_t args[10];
    ಪೂರ್ಣಾಙ್ಕ	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    ಯದಿ (gui.event_buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(ರಿಕ್ತ, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, ರಿಕ್ತ, n, args);
    ಯದಿ (gui.vimWindow == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, ರಿಕ್ತ);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, ರಿಕ್ತ);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    ಯದಿ (gui.vimPanelGroup == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    ಯದಿ (gui.vimContainer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, ರಿಕ್ತ);
#endif

    // Size for the text area is set in gui_mch_set_text_area_pos
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_RAW_DRAW_F, gui_ph_handle_raw_draw, 1);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, GUI_PH_MARGIN, 0);
    /*
     * Using focus render also causes the whole widget to be redrawn
     * whenever it changes focus, which is very annoying :p
     */
    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE,
	    Pt_GETS_FOCUS | Pt_HIGHLIGHTED);
#ifndef FEAT_MOUSESHAPE
    PtSetArg(&args[ n++ ], Pt_ARG_CURSOR_TYPE, GUI_PH_MOUSE_TYPE, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CURSOR_COLOR, gui_ph_mouse_color, 0);
#endif

    gui.vimTextArea = PtCreateWidget(PtRaw, Pt_DFLT_PARENT, n, args);
    ಯದಿ (gui.vimTextArea == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, ರಿಕ್ತ);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_ಕೀಲಿ,
	    gui_ph_handle_keyboard, ರಿಕ್ತ);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, ರಿಕ್ತ);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, ರಿಕ್ತ);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, ರಿಕ್ತ);
    ಯದಿ (gui_ph_timer_cursor == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, ರಿಕ್ತ);
    ಯದಿ (gui_ph_timer_timeout == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, ರಿಕ್ತ);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, ರಿಕ್ತ);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ಎಡ_ಬಲ,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    ಯದಿ (gui.vimToolBarGroup == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, ರಿಕ್ತ);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    ಯದಿ (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    ಯದಿ (gui.vimMenuBar == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_ಎಡ_ಬಲ |Pt_ತುದಿ_ANCHORED_ತುದಿ, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    ಯದಿ (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    ಯದಿ (gui.vimToolBar == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (is_photon_available == TRUE) ? OK : FAIL;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_open(ಶೂನ್ಯ)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    ಯದಿ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    ಪ್ರತ್ಯಾಗಮ (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಙ್ಕ rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, ರಿಕ್ತ);

    vim_free(gui.event_buffer);

#ifdef USE_PANEL_GROUPS
    vim_free(panel_titles);
#endif
}

////////////////////////////////////////////////////////////////////////////
// events

/*
 * When no events are available, photon will call this function, working is
 * set to FALSE, and the gui_mch_update loop will exit.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exit_gui_mch_update(ಶೂನ್ಯ *data)
{
    *(ಪೂರ್ಣಾಙ್ಕ *)data = FALSE;
    ಪ್ರತ್ಯಾಗಮ Pt_END;
}

    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ working = TRUE;

    PtAppAddWorkProc(ರಿಕ್ತ, exit_gui_mch_update, &working);
    ಯಾವತ್ ((working == TRUE) && !vim_is_ಒಳಹರಿವು_buf_full())
	PtProcessEvent();
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    ಯದಿ (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    ಯಾವತ್ (1)
    {
	PtProcessEvent();
	ಯದಿ (ಒಳಹರಿವು_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಅನ್ಯಥಾ ಯದಿ (is_timeout == TRUE)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
}

#ಯದಿ defined(FEAT_BROWSE)
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
	ಪೂರ್ಣಾಙ್ಕ saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    ಪೂರ್ಣಾಙ್ಕ	    flags;
    char_u  *default_path;
    char_u  *open_text = ರಿಕ್ತ;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    ಯದಿ (default_path != ರಿಕ್ತ)
    {
	ಯದಿ (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	ಯದಿ (initdir == ರಿಕ್ತ || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	ಅನ್ಯಥಾ
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	ಯದಿ (default_name != ರಿಕ್ತ)
	{
	    ಯದಿ (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		ರಿಕ್ತ,
		title,
		default_path,
		ರಿಕ್ತ,
		open_text,
		ರಿಕ್ತ,
		ರಿಕ್ತ,
		&file,
		flags);

	vim_free(default_path);

	ಯದಿ (file.ret == Pt_FSDIALOG_BTN1)
	    ಪ್ರತ್ಯಾಗಮ vim_strsave(file.path);
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_GUI_DIALOG)
ಸ್ಥಿರ PtWidget_t *gui_ph_dialog_text = ರಿಕ್ತ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_dialog_close(ಪೂರ್ಣಾಙ್ಕ button, ಶೂನ್ಯ *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    ಯದಿ (gui_ph_dialog_text != ರಿಕ್ತ)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (ಶೂನ್ಯ *) button);

    ಪ್ರತ್ಯಾಗಮ Pt_TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    ಶೂನ್ಯ		*data,
    PtCallbackInfo_t	*info)
{
    ಯದಿ (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    ಪ್ರತ್ಯಾಗಮ Pt_CONTINUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_dialog_esc(PtWidget_t *widget, ಶೂನ್ಯ *data, PtCallbackInfo_t *info)
{
    Ph_ಕೀಲಿ_Event_t *ಕೀಲಿ;

    ಕೀಲಿ = PhGetData(info->event);
    ಯದಿ ((ಕೀಲಿ->ಕೀಲಿ_flags & Pk_KF_Cap_Valid) && (ಕೀಲಿ->ಕೀಲಿ_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	ಪ್ರತ್ಯಾಗಮ Pt_CONSUME;
    }
    ಪ್ರತ್ಯಾಗಮ Pt_PROCESS;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_dialog(
	ಪೂರ್ಣಾಙ್ಕ	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	ಪೂರ್ಣಾಙ್ಕ	default_button,
	char_u	*textfield,
	ಪೂರ್ಣಾಙ್ಕ	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    ಪೂರ್ಣಾಙ್ಕ		button_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		i, len;
    ಪೂರ್ಣಾಙ್ಕ		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_ಎಣಿಕೆ = len = i = 0;

    ಯದಿ (buttons == ರಿಕ್ತ || *buttons == NUL)
	ಪ್ರತ್ಯಾಗಮ -1;

    // There is one less separator than buttons, so bump up the button count
    button_ಎಣಿಕೆ = 1;

    // Count string length and number of separators
    ಪ್ರತಿ (str = buttons; *str; str++)
    {
	len++;
	ಯದಿ (*str == DLG_BUTTON_SEP)
	    button_ಎಣಿಕೆ++;
    }

    ಯದಿ (title == ರಿಕ್ತ)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_ಎಣಿಕೆ);
    ಯದಿ (buttons_copy != ರಿಕ್ತ && button_array != ರಿಕ್ತ)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	ಪ್ರತಿ (i = 0; i < button_ಎಣಿಕೆ; i++)
	{
	    button_array[ i ] = str;
	    ಪ್ರತಿ (; *str; str++)
	    {
		ಯದಿ (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    ಭಙ್ಗ;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, ರಿಕ್ತ,
		title,
		ರಿಕ್ತ,
		message, ರಿಕ್ತ,
		button_ಎಣಿಕೆ, (ಧ್ರುವ ವರ್ಣ **) button_array, ರಿಕ್ತ,
		default_button, 0, Pt_MODAL);
# ಅನ್ಯಥಾ
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    ಪೂರ್ಣಾಙ್ಕ n;
	    PtArg_t args[5];
	    PtWidget_t *dialog, *pane;
	    PtModalCtrl_t modal_ctrl;
	    PtDialogInfo_t di;

	    CLEAR_FIELD(di);
	    CLEAR_FIELD(modal_ctrl);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_GROUP_ROWS_COLS, 0, 0);
	    PtSetArg(&args[n++], Pt_ARG_WIDTH, 350, 0);
	    PtSetArg(&args[n++], Pt_ARG_GROUP_ORIENTATION,
		    Pt_GROUP_VERTICAL, 0);
	    PtSetArg(&args[n++], Pt_ARG_GROUP_FLAGS,
		    Pt_TRUE, Pt_GROUP_NO_ಕೀಲಿಗಳು | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, ರಿಕ್ತ, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    ಯದಿ (textfield != ರಿಕ್ತ)
	    {
		n = 0;
		PtSetArg(&args[n++], Pt_ARG_MAX_LENGTH, IOSIZE - 1, 0);
		PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, textfield, 0);
		PtSetArg(&args[n++], Pt_ARG_POINTER, textfield, 0);
		gui_ph_dialog_text = PtCreateWidget(PtText, pane, n, args);
		PtAddCallback(gui_ph_dialog_text, Pt_CB_ACTIVATE,
			gui_ph_dialog_text_enter, &modal_ctrl);
	    }

	    di.parent = gui.vimWindow;
	    di.pane = pane;
	    di.title = title;
	    di.buttons = (ಧ್ರುವ ವರ್ಣ **) button_array;
	    di.nbtns = button_ಎಣಿಕೆ;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_ಎಣಿಕೆ;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_ಕೀಲಿ,
		    gui_ph_dialog_esc, &modal_ctrl);

	    ಯದಿ (gui_ph_dialog_text != ರಿಕ್ತ)
		PtGiveFocus(gui_ph_dialog_text, ರಿಕ್ತ);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (ಪೂರ್ಣಾಙ್ಕ) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = ರಿಕ್ತ;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    ಪ್ರತ್ಯಾಗಮ dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, ರಿಕ್ತ);

    *x = pos->x;
    *y = pos->y;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
gui_mch_set_winpos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    ಶೂನ್ಯ
gui_mch_set_shellsize(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height,
	ಪೂರ್ಣಾಙ್ಕ min_width, ಪೂರ್ಣಾಙ್ಕ min_height, ಪೂರ್ಣಾಙ್ಕ base_width, ಪೂರ್ಣಾಙ್ಕ base_height,
	ಪೂರ್ಣಾಙ್ಕ direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_ಎಡ + pg_margin_ಬಲ;
    window_size.h += pg_margin_ತುದಿ + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    ಯದಿ (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(ಪೂರ್ಣಾಙ್ಕ *screen_w, ಪೂರ್ಣಾಙ್ಕ *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_ಒಳಹರಿವು_Group(ರಿಕ್ತ), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_set_foreground(ಶೂನ್ಯ)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    ಶೂನ್ಯ
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_ಅಂಕೆ(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    ಶೂನ್ಯ
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ val, ಪೂರ್ಣಾಙ್ಕ size, ಪೂರ್ಣಾಙ್ಕ max)
{
    ಪೂರ್ಣಾಙ್ಕ	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_ಮೌಲ್ಯ, val, 0);
    PtSetResources(sb->id, n, args);
}

    ಶೂನ್ಯ
gui_mch_set_scrollbar_pos(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ orient)
{
    ಪೂರ್ಣಾಙ್ಕ	    n = 0;
//    int	    anchor_flags = 0;
    PtArg_t args[4];

    /*
     * Stop the scrollbar from being realized when the parent
     * is realized, so it can be explicitly realized by vim.
     *
     * Also, don't let the scrollbar get focus
     */
    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE,
	    Pt_DELAY_REALIZE | Pt_GETS_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_SCROLLBAR_FLAGS, Pt_SCROLLBAR_SHOW_ARROWS, 0);
#ಯದಿ 0
    // Don't need this anchoring for the scrollbars
    ಯದಿ (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_ಎಡ_ANCHORED_ಎಡ | Pt_ಬಲ_ANCHORED_ಬಲ;
    }
    ಅನ್ಯಥಾ
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_ತುದಿ_ANCHORED_ತುದಿ;
	ಯದಿ (sb->wp != ರಿಕ್ತ)
	{
	    ಯದಿ (sb == &sb->wp->w_scrollbars[ SBAR_ಎಡ ])
		anchor_flags |= Pt_ಎಡ_ANCHORED_ಎಡ;
	    ಅನ್ಯಥಾ
		anchor_flags |= Pt_ಬಲ_ANCHORED_ಬಲ;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#ಅನ್ಯಥಾ
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    ಶೂನ್ಯ
gui_mch_enable_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಯದಿ (flag != 0)
	PtRealizeWidget(sb->id);
    ಅನ್ಯಥಾ
	PtUnrealizeWidget(sb->id);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = ರಿಕ್ತ;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#ಯದಿ defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
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
    Ph_CURSOR_QUESTION_POINT,	// question
    Ph_CURSOR_POINTER,		// right-arrow
    Ph_CURSOR_POINTER,		// up-arrow
    Ph_CURSOR_POINTER		// last one
};

    ಶೂನ್ಯ
mch_set_mouse_shape(ಪೂರ್ಣಾಙ್ಕ shape)
{
    ಪೂರ್ಣಾಙ್ಕ	    id;

    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	ಅನ್ಯಥಾ
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    ಯದಿ (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಙ್ಕ hide)
{
    ಯದಿ (gui.pointer_hidden == hide)
	ಪ್ರತ್ಯಾಗಮ;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    ಯದಿ (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    ಅನ್ಯಥಾ
	mch_set_mouse_shape(last_shape);
#ಅನ್ಯಥಾ
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_ಒಳಹರಿವು_Group(ರಿಕ್ತ), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_ಒಳಹರಿವು_Group(ರಿಕ್ತ), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ಪ್ರತ್ಯಾಗಮ (guicolor_T)(PgRGB(Pg_Red_ಮೌಲ್ಯ(pixel),
				     Pg_Green_ಮೌಲ್ಯ(pixel), Pg_Blue_ಮೌಲ್ಯ(pixel)));
}

    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
#ಯದಿ 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    ಯದಿ (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_ಮೌಲ್ಯ(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_ಮೌಲ್ಯ(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_ಮೌಲ್ಯ(gui_ph_mouse_color) ^ 255;

# ifndef FEAT_MOUSESHAPE
	gui_ph_mouse_color = PgRGB(r, g, b);
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_COLOR,
		gui_ph_mouse_color, 0);
# endif
    }
#endif

    PtSetResource(gui.vimTextArea, Pt_ARG_FILL_COLOR, gui.back_pixel, 0);
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
    ಪ್ರತ್ಯಾಗಮ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ g, ಪೂರ್ಣಾಙ್ಕ b)
{
    ಪ್ರತ್ಯಾಗಮ gui_get_rgb_color_cmn(r, g, b);
}

    ಶೂನ್ಯ
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    ಶೂನ್ಯ
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    ಶೂನ್ಯ
gui_mch_set_sp_color(guicolor_T color)
{
}

    ಶೂನ್ಯ
gui_mch_invert_rectangle(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ nr, ಪೂರ್ಣಾಙ್ಕ nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    ಯದಿ (nc > 0)
	rect.lr.x -= 1;
    ಯದಿ (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    ಶೂನ್ಯ
gui_mch_clear_block(ಪೂರ್ಣಾಙ್ಕ row1, ಪೂರ್ಣಾಙ್ಕ col1, ಪೂರ್ಣಾಙ್ಕ row2, ಪೂರ್ಣಾಙ್ಕ col2)
{
    PhRect_t block = {
	{ FILL_X(col1), FILL_Y(row1) },
	{ FILL_X(col2 + 1) - 1, FILL_Y(row2 + 1) - 1}
    };

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&block, Pg_DRAW_FILL);
    DRAW_END;
}

    ಶೂನ್ಯ
gui_mch_clear_all(ಶೂನ್ಯ)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    ಯದಿ (is_ignore_draw == TRUE)
	ಪ್ರತ್ಯಾಗಮ;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ಎಡ);
    rect.ul.y = FILL_Y(row + ಅಂಕೆ_ಸಾಲುಗಳು);

    rect.lr.x = FILL_X(gui.scroll_region_ಬಲ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ರಿಕ್ತ));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
	gui.scroll_region_ಎಡ,
	gui.scroll_region_bot,
	gui.scroll_region_ಬಲ);
}

    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ಎಡ);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_ಬಲ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ರಿಕ್ತ));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_ಎಡ,
	    row + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
}

    ಶೂನ್ಯ
gui_mch_draw_string(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, char_u *s, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಸ್ಥಿರ ವರ್ಣ *utf8_buffer = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    ಯದಿ (is_ignore_draw == TRUE)
	ಪ್ರತ್ಯಾಗಮ;

    DRAW_START;

    ಯದಿ (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    ಯದಿ (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    ಯದಿ (charset_translate != ರಿಕ್ತ && enc_utf8 == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	ಯದಿ (utf8_len < len)
	{
	    ವರ್ಣ *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    ಯದಿ (new_buffer == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    utf8_buffer = new_buffer;
	    utf8_len = len;
	}

	PxTranslateToUTF(
		charset_translate,
		s,
		len,
		&src_taken,
		utf8_buffer,
		utf8_len,
		&dst_made);
	s = utf8_buffer;
	len = dst_made;
    }

    PgDrawText(s, len, &pos, 0);

    ಯದಿ (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#ಯದಿ 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(ರಿಕ್ತ);
#ಅನ್ಯಥಾ
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    ಯದಿ (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    PhRect_t r;

    // FIXME: Double width characters

    r.ul.x = FILL_X(gui.col);
    r.ul.y = FILL_Y(gui.row);
    r.lr.x = r.ul.x + gui.char_width - 1;
    r.lr.y = r.ul.y + gui.char_height - 1;

    DRAW_START;
    PgSetStrokeColor(color);
    PgDrawRect(&r, Pg_DRAW_STROKE);
    DRAW_END;
}

    ಶೂನ್ಯ
gui_mch_draw_part_cursor(ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h, guicolor_T color)
{
    PhRect_t r;

    r.ul.x = FILL_X(gui.col);
    r.ul.y = FILL_Y(gui.row) + gui.char_height - h;
    r.lr.x = r.ul.x + w - 1;
    r.lr.y = r.ul.y + h - 1;

    DRAW_START;
    gui_mch_set_bg_color(color);
    PgDrawRect(&r, Pg_DRAW_FILL);
    DRAW_END;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ಶೂನ್ಯ
gui_mch_start_blink(ಶೂನ್ಯ)
{
    // Only turn on the timer on if none of the times are zero
    ಯದಿ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಙ್ಕ may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    ಯದಿ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
gui_mch_flash(ಪೂರ್ಣಾಙ್ಕ msec)
{
    PgSetFillXORColor(Pg_BLACK, Pg_WHITE);
    PgSetDrawMode(Pg_DRAWMODE_XOR);
    gui_mch_clear_all();
    gui_mch_flush();

    ui_delay((long) msec, TRUE);

    gui_mch_clear_all();
    PgSetDrawMode(Pg_DRAWMODE_OPAQUE);
    gui_mch_flush();
}

    ಶೂನ್ಯ
gui_mch_flush(ಶೂನ್ಯ)
{
    PgFlush();
}

    ಶೂನ್ಯ
gui_mch_set_text_area_pos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	ಯದಿ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0 &&
		 name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
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
    &tb_find_phi,
    &tb_save_all_phi,
    &tb_save_session_phi,
    &tb_new_session_phi,
    &tb_load_session_phi,
    &tb_macro_phi,
    &tb_replace_phi,
    &tb_close_phi,
    &tb_maximize_phi,
    &tb_minimize_phi,
    &tb_split_phi,
    &tb_shell_phi,
    &tb_find_prev_phi,
    &tb_find_next_phi,
    &tb_find_help_phi,
    &tb_make_phi,
    &tb_jump_phi,
    &tb_ctags_phi,
    &tb_vsplit_phi,
    &tb_maxwidth_phi,
    &tb_minwidth_phi
};

ಸ್ಥಿರ PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    ಸ್ಥಿರ PhImage_t external_icon;
    PhImage_t *temp_phi = ರಿಕ್ತ;

    temp_phi = PxLoadImage(iconfile, ರಿಕ್ತ);
    ಯದಿ (temp_phi != ರಿಕ್ತ)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, ಪರಿಮಾಣ(external_icon));
	ಮುಕ್ತ(temp_phi);

	temp_phi = &external_icon;
    }
    ಪ್ರತ್ಯಾಗಮ temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ಸ್ಥಿರ PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = ರಿಕ್ತ;

    ಯದಿ (menu->icon_builtin == FALSE)
    {
	ಯದಿ (menu->iconfile != ರಿಕ್ತ)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	ಯದಿ ((icon == ರಿಕ್ತ) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	ಯದಿ (icon != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ icon;
    }

    ಯದಿ (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	ಪ್ರತ್ಯಾಗಮ gui_ph_toolbar_images[menu->iconidx];

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_MENU)
    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಯದಿ (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    ಅನ್ಯಥಾ
	PtUnrealizeWidget(gui.vimMenuBar);
}

    ಶೂನ್ಯ
gui_mch_set_menu_pos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ph_position_menu(PtWidget_t *widget, ಪೂರ್ಣಾಙ್ಕ priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    ಯಾವತ್ (traverse != ರಿಕ್ತ)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	ಯದಿ (menu != ರಿಕ್ತ &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_ಸೇರಿಸು(widget, traverse, 1);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    ಶೂನ್ಯ
gui_mch_add_menu(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_ಕೀಲಿ;
    char_u	mnemonic_str[MB_LEN_MAX];
    ಪೂರ್ಣಾಙ್ಕ	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = ರಿಕ್ತ;

    ಯದಿ (menu_is_menubar(menu->name))
    {

	accel_ಕೀಲಿ = vim_strchr(menu->name, '&');
	ಯದಿ (accel_ಕೀಲಿ != ರಿಕ್ತ)
	{
	    mnemonic_str[0] = accel_ಕೀಲಿ[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	ಯದಿ (accel_ಕೀಲಿ != ರಿಕ್ತ)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_ಕೀಲಿ, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	ಯದಿ (parent != ರಿಕ್ತ)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_ಬಲ, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == ರಿಕ್ತ) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	ಯದಿ (parent != ರಿಕ್ತ)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	ಯದಿ (parent == ರಿಕ್ತ)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    ಯದಿ (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	ಯದಿ (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    ಅನ್ಯಥಾ ಯದಿ (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, ರಿಕ್ತ);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    ಶೂನ್ಯ
gui_mch_add_menu_item(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_ಕೀಲಿ;
    char_u	mnemonic_str[MB_LEN_MAX];
    ಪೂರ್ಣಾಙ್ಕ	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    ಯದಿ (menu_is_toolbar(parent->name))
    {
	ಯದಿ (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_ತುದಿ_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (strstr((ಧ್ರುವ ವರ್ಣ *) p_toolbar, "text") != ರಿಕ್ತ)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    ಯದಿ ((strstr((ಧ್ರುವ ವರ್ಣ *) p_toolbar, "icons") != ರಿಕ್ತ) &&
		    (gui_ph_toolbar_images != ರಿಕ್ತ))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    ಯದಿ (strstr((ಧ್ರುವ ವರ್ಣ *) p_toolbar, "tooltips") != ರಿಕ್ತ)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_BALLOON,
			gui_ph_show_tooltip, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_FLAGS,
			Pt_TRUE, Pt_SHOW_BALLOON);
	    }
	    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 1, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 1, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_FALSE,
		    Pt_HIGHLIGHTED | Pt_GETS_FOCUS);
	    PtSetArg(&args[ n++ ], Pt_ARG_FILL_COLOR, Pg_TRANSPARENT, 0);
	    menu->id = PtCreateWidget(PtButton, gui.vimToolBar, n, args);

	    PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);
	}
	// Update toolbar if it's open
	ಯದಿ (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    ಅನ್ಯಥಾ
# endif
	ಯದಿ (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    ಅನ್ಯಥಾ
    {
	accel_ಕೀಲಿ = vim_strchr(menu->name, '&');
	ಯದಿ (accel_ಕೀಲಿ != ರಿಕ್ತ)
	{
	    mnemonic_str[0] = accel_ಕೀಲಿ[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	ಯದಿ (accel_ಕೀಲಿ != ರಿಕ್ತ)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_ಕೀಲಿ, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	ಯದಿ (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    ಯದಿ (menu->submenu_id != ರಿಕ್ತ)
	PtDestroyWidget(menu->submenu_id);
    ಯದಿ (menu->id != ರಿಕ್ತ)
	PtDestroyWidget(menu->id);

    menu->submenu_id = ರಿಕ್ತ;
    menu->id = ರಿಕ್ತ;
}

    ಶೂನ್ಯ
gui_mch_menu_grey(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ grey)
{
    long    flags, mask, fields;

    ಯದಿ (menu->id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    flags = PtWidgetFlags(menu->id);
    ಯದಿ (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    ಅನ್ಯಥಾ
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    ಯದಿ (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    ಶೂನ್ಯ
gui_mch_menu_hidden(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ hidden)
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
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಙ್ಕ enable)
{
    // No tearoffs yet
}

#endif

#ಯದಿ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಙ್ಕ showit)
{
    ಯದಿ (showit)
	PtRealizeWidget(gui.vimToolBar);
    ಅನ್ಯಥಾ
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    ಸ್ಥಿರ GuiFont
gui_ph_get_font(
	char_u	*font_name,
	int_u	font_flags,
	int_u	font_size,
	// Check whether the resulting font has the font flags and size that
	// was asked for
	int_u	enforce
	)
{
    char_u	    *font_tag;
    FontQueryInfo   info;
    int_u	    style;

    font_tag = alloc(MAX_FONT_TAG);
    ಯದಿ (font_tag != ರಿಕ್ತ)
    {
	ಯದಿ (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != ರಿಕ್ತ)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    ಯದಿ (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    ಯದಿ (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    ಯದಿ (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    ಯದಿ (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    ಯದಿ (font_size == info.size &&
		    style == (info.style & style))
		ಪ್ರತ್ಯಾಗಮ (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    ಯದಿ (mark == ರಿಕ್ತ)
	name_len = STRLEN(vim_font);
    ಅನ್ಯಥಾ
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    ಯದಿ (*font_name != ರಿಕ್ತ)
    {
	ಯದಿ (mark != ರಿಕ್ತ)
	{
	    ಯಾವತ್ (*mark != NUL && *mark++ == ':')
	    {
		ವಿಕಲ್ಪ (SAFE_tolower(*mark++))
		{
		    ಪಕ್ಷ 'a': *font_flags |= PF_STYLE_ANTIALIAS; ಭಙ್ಗ;
		    ಪಕ್ಷ 'b': *font_flags |= PF_STYLE_BOLD; ಭಙ್ಗ;
		    ಪಕ್ಷ 'i': *font_flags |= PF_STYLE_ITALIC; ಭಙ್ಗ;

		    ಪಕ್ಷ 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			ಯದಿ (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			ಭಙ್ಗ;

		    ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
	    }
	}
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_font(char_u *vim_font_name, ಪೂರ್ಣಾಙ್ಕ fontset)
{
    char_u  *font_tag;
    char_u  *font_name = ರಿಕ್ತ;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    ಯದಿ (vim_font_name == ರಿಕ್ತ)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    ಯದಿ (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, ರಿಕ್ತ, ರಿಕ್ತ,
		"pcterm12", -1, PHFONT_FIXED, ರಿಕ್ತ);

	ಯದಿ (font_tag == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	ಯದಿ (font_tag == ರಿಕ್ತ)
	{
	    vim_free(font_name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;
    }

    gui_mch_free_font(gui.bold_font);
    gui.bold_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_BOLD,
	    font_size, PF_STYLE_BOLD);

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_ITALIC,
	    font_size, PF_STYLE_ITALIC);

    // This extent was brought to you by the letter 'g'
    PfExtentText(&extent, ರಿಕ್ತ, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, ಪೂರ್ಣಾಙ್ಕ report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    ಯದಿ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	ಯದಿ (font_tag != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ (GuiFont)font_tag;
    }

    ಯದಿ (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    ಪ್ರತ್ಯಾಗಮ FAIL;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ vim_strsave(name);
}
#endif

    ಶೂನ್ಯ
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

