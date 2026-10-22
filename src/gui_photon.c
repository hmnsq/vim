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

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#જો !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	પૂર્ણાંક
# define PhEvent_t	પૂર્ણાંક
# define PhPoint_t	પૂર્ણાંક
# define PtWidget_t	પૂર્ણાંક
# define Pg_BLACK	0
# define PtCallbackF_t	પૂર્ણાંક
# define PtCallbackInfo_t પૂર્ણાંક
# define PhTile_t	પૂર્ણાંક
# define PtWidget_t	પૂર્ણાંક
# define PhImage_t	પૂર્ણાંક
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE કદ(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
સ્થિર અક્ષર	*empty_title = "    ";
સ્થિર અક્ષર	**panel_titles = ખાલી;
સ્થિર ushort_t	અંક_panels = 0;
સ્થિર short pg_margin_ડાબું, pg_margin_જમણું, pg_margin_ટોચ, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_ઉમેરો
સ્થિર PgColor_t gui_ph_mouse_color =	Pg_BLACK;

સ્થિર PhPoint_t    gui_ph_raw_offset;
સ્થિર PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
સ્થિર PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
સ્થિર short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
સ્થિર PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
સ્થિર પૂર્ણાંક is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
સ્થિર સંરચના PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
સ્થિર ગણના {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

સ્થિર long_u	blink_waittime	= 700;
સ્થિર long_u	blink_ontime	= 400;
સ્થિર long_u	blink_offtime	= 250;

સ્થિર સંરચના
{
    પૂર્ણાંક	    ચાવી_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ચાવીઓ[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_ડાબું,	    'k', 'l'},
    {Pk_જમણું,	    'k', 'r'},

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
    {Pk_ઉમેરો,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //ઉમેરો
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

સ્થિર PtCallbackF_t gui_ph_handle_timer_cursor;
સ્થિર PtCallbackF_t gui_ph_handle_timer_timeout;

સ્થિર PtCallbackF_t gui_ph_handle_window_cb;

સ્થિર PtCallbackF_t gui_ph_handle_scrollbar;
સ્થિર PtCallbackF_t gui_ph_handle_keyboard;
સ્થિર PtCallbackF_t gui_ph_handle_mouse;
સ્થિર PtCallbackF_t gui_ph_handle_pulldown_menu;
સ્થિર PtCallbackF_t gui_ph_handle_menu;
સ્થિર PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

સ્થિર PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
સ્થિર PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
સ્થિર શૂન્ય gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

સ્થિર શૂન્ય gui_ph_draw_start(શૂન્ય);
સ્થિર શૂન્ય gui_ph_draw_end(શૂન્ય);

// Set the text for the balloon
સ્થિર PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     પૂર્ણાંક position,
			     અક્ષર *text,
			     અક્ષર *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

સ્થિર PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     પૂર્ણાંક position,
			     અક્ષર *text,
			     અક્ષર *font,
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
    જો (menu != ખાલી)
    {
	પૂર્ણાંક index = MENU_INDEX_TIP;
	જો (menu->strings[ index ] != ખાલી)
	    tooltip = menu->strings[ index ];
    }

    પાછા PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_ટોચ,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    સ્થિર શૂન્ય
gui_ph_resize_container(શૂન્ય)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	શૂન્ય *other,
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
    જો (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    નહીંતર
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#નહીંતર
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_ટોચ, &pg_margin_bottom,
	    &pg_margin_ડાબું, &pg_margin_જમણું);
#endif
    પાછા Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    સ્થિર પૂર્ણાંક
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	શૂન્ય *data,
	PtCallbackInfo_t *info)
{
    જો (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    નહીંતર
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    પાછા Pt_CONTINUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    શૂન્ય		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    પાછા Pt_CONTINUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_window_cb(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    પસંદગી (we->event_f)
    {
	સ્થિતિ Ph_WM_CLOSE:
	    gui_shell_closed();
	    તોડો;

	સ્થિતિ Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    જો (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    નહીંતર
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    તોડો;

	સ્થિતિ Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_ડાબું + pg_margin_જમણું);
	    height -= (pg_margin_ટોચ + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    તોડો;

	મૂળભૂત:
	    તોડો;
    }

    પાછા Pt_CONTINUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_scrollbar(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    પૂર્ણાંક	    કિંમત, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    જો (sb != ખાલી)
    {
	કિંમત = scroll->position;
	પસંદગી (scroll->action)
	{
	    સ્થિતિ Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		તોડો;

	    સ્થિતિ Pt_SCROLL_SET:
		// FIXME: return straight away here?
		પાછા Pt_CONTINUE;
		તોડો;
	}

	gui_drag_scrollbar(sb, કિંમત, dragging);
    }
    પાછા Pt_CONTINUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_keyboard(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    Ph_ચાવી_Event_t    *ચાવી;
    unsigned અક્ષર   string[6];
    પૂર્ણાંક		    len, i;
    પૂર્ણાંક		    ch, modifiers;

    ચાવી = PhGetData(info->event);

    ch = modifiers = len = 0;

    જો (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    જો (ચાવી->ચાવી_flags & Pk_KF_Compose)
	પાછા Pt_CONTINUE;

    જો ((ચાવી->ચાવી_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_ચાવી_Down(ચાવી->ચાવી_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	જો (((ચાવી->ચાવી_mods & Pk_KM_Alt) &&
		    !(ચાવી->ચાવી_mods & Pk_KM_Shift) &&
		    !(ચાવી->ચાવી_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(ચાવી->ચાવી_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    પાછા Pt_CONTINUE;
	}
#endif

	માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != 0; i++)
	{
	    જો (special_ચાવીઓ[i].ચાવી_sym == ચાવી->ચાવી_cap)
	    {
		len = 0;
		જો (special_ચાવીઓ[i].vim_code1 == NUL)
		    ch = special_ચાવીઓ[i].vim_code0;
		નહીંતર
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    જો (ચાવી->ચાવી_cap >= Pk_KP_Enter && ચાવી->ચાવી_cap <= Pk_KP_9
			    && (ચાવી->ચાવી_mods & Pk_KM_અંક_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = ચાવી->ચાવી_cap - 0xf080;
		    }
		    નહીંતર
			ch = TO_SPECIAL(special_ચાવીઓ[i].vim_code0,
				special_ચાવીઓ[i].vim_code1);
		}
		તોડો;
	    }
	}

	જો (ચાવી->ચાવી_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	જો (ચાવી->ચાવી_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	જો (ચાવી->ચાવી_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	જો (special_ચાવીઓ[i].ચાવી_sym == 0)
	{
	    ch = PhTo8859_1(ચાવી);
	    જો (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_ચાવી_To_Mb(string, ચાવી);
		જો (len > 0)
		{
		    સ્થિર અક્ષર buf[6];
		    પૂર્ણાંક src_taken, dst_made;
		    જો (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_આગત_buf(buf, dst_made);
		    }
		    નહીંતર
		    {
			add_to_આગત_buf(string, len);
		    }

		    પાછા Pt_CONSUME;
		}
		len = 0;
		ch = ચાવી->ચાવી_cap;
		જો (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    જો (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			જો ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			નહીંતર જો (ch == '2')
			    ch = NUL;
			નહીંતર જો (ch == '6')
			    ch = 0x1e;
			નહીંતર જો (ch == '-')
			    ch = 0x1f;
			નહીંતર
			    modifiers |= MOD_MASK_CTRL;
		    }

		    જો (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		નહીંતર
		{
		    પાછા Pt_CONTINUE;
		}
	    }
	    નહીંતર
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_ચાવી(ch, &modifiers);
	જો (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	જો (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	નહીંતર
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    પૂર્ણાંક int_ch = check_for_interrupt(ch, modifiers);

	    જો (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_આગત_buf();
	    }
	}

	જો (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	જો (len > 0)
	{
	    add_to_આગત_buf(string, len);
	    પાછા Pt_CONSUME;
	}
    }

    પાછા Pt_CONTINUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_mouse(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    પૂર્ણાંક		     button = 0, repeated_click, modifiers = 0x0;
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

    જો (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	પાછા Pt_CONTINUE;
    }

    જો (pointer->ચાવી_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    જો (pointer->ચાવી_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    જો (pointer->ચાવી_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    જો (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_ડાબું;

    જો (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_જમણું;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    જો (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_મધ્ય;

    // Catch a real release (not phantom or other releases
    જો (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    જો (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#જો 0
    // Vim doesn't use button repeats
    જો (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    જો ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_ગણતરી >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    પાછા Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    સ્થિર પૂર્ણાંક
gui_ph_handle_focus(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    જો (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ખાલી);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    નહીંતર
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ખાલી);
    }
    પાછા Pt_CONTINUE;
}

    સ્થિર શૂન્ય
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    જો (is_ignore_draw == TRUE)
	પાછા;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#જો 0
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
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, ખાલી));

#જો 1
    // Redraw individual damage regions
    જો (damage->next != ખાલી)
	damage = damage->next;

    જ્યારે (damage != ખાલી)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#નહીંતર
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	શૂન્ય *data,
	PtCallbackInfo_t *info)
{
    જો (data != ખાલી)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, ખાલી);
	PtRealizeWidget(menu->submenu_id);
    }

    પાછા Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    સ્થિર પૂર્ણાંક
gui_ph_handle_menu(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    જો (data != ખાલી)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    પાછા Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    સ્થિર પૂર્ણાંક
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	શૂન્ય *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, ખાલી);
    પાછા Pt_CONTINUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_window_open(
	PtWidget_t *widget,
	શૂન્ય *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    પાછા Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    સ્થિર શૂન્ય
gui_ph_draw_start(શૂન્ય)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ખાલી));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    સ્થિર શૂન્ય
gui_ph_draw_end(શૂન્ય)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    સ્થિર vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *ટોચ_level = મૂળ_menu;
    vimmenu_T *વસ્તુઓ = ખાલી;

    જ્યારે (ટોચ_level != ખાલી &&
	    (STRCMP(ટોચ_level->dname, "Buffers") != 0))
	ટોચ_level = ટોચ_level->next;

    જો (ટોચ_level != ખાલી)
    {
	વસ્તુઓ = ટોચ_level->children;

	જ્યારે (વસ્તુઓ != ખાલી &&
		(STRCMP(વસ્તુઓ->dname, name) != 0))
	    વસ્તુઓ = વસ્તુઓ->next;
    }
    પાછા વસ્તુઓ;
}

    સ્થિર શૂન્ય
gui_ph_pg_set_buffer_અંક(int_u buf_અંક)
{
    પૂર્ણાંક i;
    અક્ષર શોધ[16];
    અક્ષર *mark;

    જો (gui.vimTextArea == ખાલી || buf_અંક == 0)
	પાછા;

    શોધ[0] = '(';
    ultoa(buf_અંક, &શોધ[1], 10);
    STRCAT(શોધ, ")");

    માટે (i = 0; i < અંક_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	જો (mark != ખાલી && STRCMP(mark, શોધ) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    સ્થિર પૂર્ણાંક
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	શૂન્ય *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    જો (info->event != ખાલી)
    {
	panel = info->cbdata;
	જો (panel->new_panel != ખાલી)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    જો (menu)
		gui_menu_cb(menu);
	}
    }
    પાછા Pt_CONTINUE;
}

    સ્થિર શૂન્ય
gui_ph_get_panelgroup_margins(
	short *ટોચ,
	short *bottom,
	short *ડાબું,
	short *જમણું)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    અચલ unsigned short *margin_ટોચ, *margin_bottom;
    અચલ unsigned short *margin_ડાબું, *margin_જમણું;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_જમણું, &margin_જમણું, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *ટોચ    = abs_raw_y;
    *bottom = *margin_bottom;

    *ડાબું  = abs_raw_x;
    *જમણું = *margin_જમણું;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    સ્થિર પૂર્ણાંક
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    અક્ષર *mark;

    જો (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	જો (mark != ખાલી)
	{
	    mark++;
	    જ્યારે (SAFE_isdigit(*mark))
		mark++;

	    જો (*mark == ')')
		પાછા TRUE;
	}
    }
    પાછા FALSE;
}

    સ્થિર શૂન્ય
gui_ph_pg_add_buffer(અક્ષર *name)
{
    અક્ષર **new_titles = ખાલી;

    new_titles = ALLOC_MULT(અક્ષર *, (અંક_panels + 1));
    જો (new_titles == ખાલી)
	પાછા;

    જો (અંક_panels > 0)
	memcpy(new_titles, panel_titles, અંક_panels * કદ(અક્ષર **));

    new_titles[ અંક_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    અંક_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    સ્થિર શૂન્ય
gui_ph_pg_remove_buffer(અક્ષર *name)
{
    પૂર્ણાંક i;
    અક્ષર **new_titles = ખાલી;

    // If there is only 1 panel, we just use the temporary place holder
    જો (અંક_panels > 1)
    {
	new_titles = ALLOC_MULT(અક્ષર *, અંક_panels - 1);
	જો (new_titles != ખાલી)
	{
	    અક્ષર **s = new_titles;
	    // Copy all the titles except the one we're removing
	    માટે (i = 0; i < અંક_panels; i++)
	    {
		જો (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    અંક_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    અંક_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    નહીંતર
    {
	અંક_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    સ્થિર પૂર્ણાંક
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	શૂન્ય *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    જો (data != ખાલી)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    પાછા Pt_CONTINUE;
}
#endif

    સ્થિર પૂર્ણાંક
gui_ph_pane_resize(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    જો (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    પાછા Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    શૂન્ય
gui_ph_encoding_changed(પૂર્ણાંક new_encoding)
{
    // Default encoding is latin1
    અક્ષર *charset = "latin1";
    પૂર્ણાંક i;

    સંરચના {
	પૂર્ણાંક encoding;
	અક્ષર *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    માટે (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	જો (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    શૂન્ય
gui_mch_prepare(પૂર્ણાંક *argc, અક્ષર **argv)
{
    PtInit(ખાલી);
}

    પૂર્ણાંક
gui_mch_init(શૂન્ય)
{
    PtArg_t args[10];
    પૂર્ણાંક	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    જો (gui.event_buffer == ખાલી)
	પાછા FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(ખાલી, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, ખાલી, n, args);
    જો (gui.vimWindow == ખાલી)
	પાછા FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, ખાલી);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, ખાલી);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    જો (gui.vimPanelGroup == ખાલી)
	પાછા FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, ખાલી);
#નહીંતર
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    જો (gui.vimContainer == ખાલી)
	પાછા FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, ખાલી);
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
    જો (gui.vimTextArea == ખાલી)
	પાછા FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, ખાલી);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_ચાવી,
	    gui_ph_handle_keyboard, ખાલી);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, ખાલી);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, ખાલી);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, ખાલી);
    જો (gui_ph_timer_cursor == ખાલી)
	પાછા FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, ખાલી);
    જો (gui_ph_timer_timeout == ખાલી)
	પાછા FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, ખાલી);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, ખાલી);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ડાબું_જમણું,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    જો (gui.vimToolBarGroup == ખાલી)
	પાછા FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, ખાલી);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    જો (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    જો (gui.vimMenuBar == ખાલી)
	પાછા FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_ડાબું_જમણું |Pt_ટોચ_ANCHORED_ટોચ, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    જો (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    જો (gui.vimToolBar == ખાલી)
	પાછા FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    પાછા OK;
}

    પૂર્ણાંક
gui_mch_init_check(શૂન્ય)
{
    પાછા (is_photon_available == TRUE) ? OK : FAIL;
}

    પૂર્ણાંક
gui_mch_open(શૂન્ય)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    જો (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    પાછા (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    શૂન્ય
gui_mch_exit(પૂર્ણાંક rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, ખાલી);

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
    સ્થિર પૂર્ણાંક
exit_gui_mch_update(શૂન્ય *data)
{
    *(પૂર્ણાંક *)data = FALSE;
    પાછા Pt_END;
}

    શૂન્ય
gui_mch_update(શૂન્ય)
{
    પૂર્ણાંક working = TRUE;

    PtAppAddWorkProc(ખાલી, exit_gui_mch_update, &working);
    જ્યારે ((working == TRUE) && !vim_is_આગત_buf_full())
	PtProcessEvent();
}

    પૂર્ણાંક
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    જો (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    જ્યારે (1)
    {
	PtProcessEvent();
	જો (આગત_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    પાછા OK;
	}
	નહીંતર જો (is_timeout == TRUE)
	    પાછા FAIL;
    }
}

#જો defined(FEAT_BROWSE)
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
	પૂર્ણાંક saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    પૂર્ણાંક	    flags;
    char_u  *default_path;
    char_u  *open_text = ખાલી;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    જો (default_path != ખાલી)
    {
	જો (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	જો (initdir == ખાલી || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	નહીંતર
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	જો (default_name != ખાલી)
	{
	    જો (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		ખાલી,
		title,
		default_path,
		ખાલી,
		open_text,
		ખાલી,
		ખાલી,
		&file,
		flags);

	vim_free(default_path);

	જો (file.ret == Pt_FSDIALOG_BTN1)
	    પાછા vim_strsave(file.path);
    }
    પાછા ખાલી;
}
#endif

#જો defined(FEAT_GUI_DIALOG)
સ્થિર PtWidget_t *gui_ph_dialog_text = ખાલી;

    સ્થિર પૂર્ણાંક
gui_ph_dialog_close(પૂર્ણાંક button, શૂન્ય *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    જો (gui_ph_dialog_text != ખાલી)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (શૂન્ય *) button);

    પાછા Pt_TRUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    શૂન્ય		*data,
    PtCallbackInfo_t	*info)
{
    જો (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    પાછા Pt_CONTINUE;
}

    સ્થિર પૂર્ણાંક
gui_ph_dialog_esc(PtWidget_t *widget, શૂન્ય *data, PtCallbackInfo_t *info)
{
    Ph_ચાવી_Event_t *ચાવી;

    ચાવી = PhGetData(info->event);
    જો ((ચાવી->ચાવી_flags & Pk_KF_Cap_Valid) && (ચાવી->ચાવી_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	પાછા Pt_CONSUME;
    }
    પાછા Pt_PROCESS;
}

    પૂર્ણાંક
gui_mch_dialog(
	પૂર્ણાંક	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	પૂર્ણાંક	default_button,
	char_u	*textfield,
	પૂર્ણાંક	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    પૂર્ણાંક		button_ગણતરી;
    પૂર્ણાંક		i, len;
    પૂર્ણાંક		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_ગણતરી = len = i = 0;

    જો (buttons == ખાલી || *buttons == NUL)
	પાછા -1;

    // There is one less separator than buttons, so bump up the button count
    button_ગણતરી = 1;

    // Count string length and number of separators
    માટે (str = buttons; *str; str++)
    {
	len++;
	જો (*str == DLG_BUTTON_SEP)
	    button_ગણતરી++;
    }

    જો (title == ખાલી)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_ગણતરી);
    જો (buttons_copy != ખાલી && button_array != ખાલી)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	માટે (i = 0; i < button_ગણતરી; i++)
	{
	    button_array[ i ] = str;
	    માટે (; *str; str++)
	    {
		જો (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    તોડો;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, ખાલી,
		title,
		ખાલી,
		message, ખાલી,
		button_ગણતરી, (અચલ અક્ષર **) button_array, ખાલી,
		default_button, 0, Pt_MODAL);
# નહીંતર
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    પૂર્ણાંક n;
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
		    Pt_TRUE, Pt_GROUP_NO_ચાવીઓ | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, ખાલી, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    જો (textfield != ખાલી)
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
	    di.buttons = (અચલ અક્ષર **) button_array;
	    di.nbtns = button_ગણતરી;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_ગણતરી;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_ચાવી,
		    gui_ph_dialog_esc, &modal_ctrl);

	    જો (gui_ph_dialog_text != ખાલી)
		PtGiveFocus(gui_ph_dialog_text, ખાલી);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (પૂર્ણાંક) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = ખાલી;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    પાછા dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    પૂર્ણાંક
gui_mch_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, ખાલી);

    *x = pos->x;
    *y = pos->y;

    પાછા OK;
}

    શૂન્ય
gui_mch_set_winpos(પૂર્ણાંક x, પૂર્ણાંક y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    શૂન્ય
gui_mch_set_shellsize(પૂર્ણાંક width, પૂર્ણાંક height,
	પૂર્ણાંક min_width, પૂર્ણાંક min_height, પૂર્ણાંક base_width, પૂર્ણાંક base_height,
	પૂર્ણાંક direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_ડાબું + pg_margin_જમણું;
    window_size.h += pg_margin_ટોચ + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    જો (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    શૂન્ય
gui_mch_get_screen_dimensions(પૂર્ણાંક *screen_w, પૂર્ણાંક *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_આગત_Group(ખાલી), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    શૂન્ય
gui_mch_iconify(શૂન્ય)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#જો defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    શૂન્ય
gui_mch_set_foreground(શૂન્ય)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    શૂન્ય
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_અંક(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    શૂન્ય
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, પૂર્ણાંક val, પૂર્ણાંક size, પૂર્ણાંક max)
{
    પૂર્ણાંક	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_કિંમત, val, 0);
    PtSetResources(sb->id, n, args);
}

    શૂન્ય
gui_mch_set_scrollbar_pos(scrollbar_T *sb, પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    પૂર્ણાંક
gui_mch_get_scrollbar_xpadding(શૂન્ય)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    પાછા 0;
}

    પૂર્ણાંક
gui_mch_get_scrollbar_ypadding(શૂન્ય)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    પાછા 0;
}

    શૂન્ય
gui_mch_create_scrollbar(scrollbar_T *sb, પૂર્ણાંક orient)
{
    પૂર્ણાંક	    n = 0;
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
#જો 0
    // Don't need this anchoring for the scrollbars
    જો (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_ડાબું_ANCHORED_ડાબું | Pt_જમણું_ANCHORED_જમણું;
    }
    નહીંતર
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_ટોચ_ANCHORED_ટોચ;
	જો (sb->wp != ખાલી)
	{
	    જો (sb == &sb->wp->w_scrollbars[ SBAR_ડાબું ])
		anchor_flags |= Pt_ડાબું_ANCHORED_ડાબું;
	    નહીંતર
		anchor_flags |= Pt_જમણું_ANCHORED_જમણું;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#નહીંતર
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    શૂન્ય
gui_mch_enable_scrollbar(scrollbar_T *sb, પૂર્ણાંક flag)
{
    જો (flag != 0)
	PtRealizeWidget(sb->id);
    નહીંતર
	PtUnrealizeWidget(sb->id);
}

    શૂન્ય
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = ખાલી;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#જો defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
સ્થિર પૂર્ણાંક last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
સ્થિર પૂર્ણાંક mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_ઉમેરો,		// beam
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

    શૂન્ય
mch_set_mouse_shape(પૂર્ણાંક shape)
{
    પૂર્ણાંક	    id;

    જો (!gui.in_use)
	પાછા;

    જો (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    નહીંતર
    {
	જો (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	નહીંતર
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    જો (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    શૂન્ય
gui_mch_mousehide(પૂર્ણાંક hide)
{
    જો (gui.pointer_hidden == hide)
	પાછા;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    જો (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    નહીંતર
	mch_set_mouse_shape(last_shape);
#નહીંતર
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    શૂન્ય
gui_mch_getmouse(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_આગત_Group(ખાલી), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    શૂન્ય
gui_mch_setmouse(પૂર્ણાંક x, પૂર્ણાંક y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_આગત_Group(ખાલી), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    પાછા (guicolor_T)(PgRGB(Pg_Red_કિંમત(pixel),
				     Pg_Green_કિંમત(pixel), Pg_Blue_કિંમત(pixel)));
}

    શૂન્ય
gui_mch_new_colors(શૂન્ય)
{
#જો 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    જો (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_કિંમત(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_કિંમત(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_કિંમત(gui_ph_mouse_color) ^ 255;

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
    પાછા gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
    પાછા gui_get_rgb_color_cmn(r, g, b);
}

    શૂન્ય
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    શૂન્ય
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    શૂન્ય
gui_mch_set_sp_color(guicolor_T color)
{
}

    શૂન્ય
gui_mch_invert_rectangle(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક nr, પૂર્ણાંક nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    જો (nc > 0)
	rect.lr.x -= 1;
    જો (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    શૂન્ય
gui_mch_clear_block(પૂર્ણાંક row1, પૂર્ણાંક col1, પૂર્ણાંક row2, પૂર્ણાંક col2)
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

    શૂન્ય
gui_mch_clear_all(શૂન્ય)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    જો (is_ignore_draw == TRUE)
	પાછા;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    શૂન્ય
gui_mch_delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ડાબું);
    rect.ul.y = FILL_Y(row + અંક_લીટીઓ);

    rect.lr.x = FILL_X(gui.scroll_region_જમણું + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ખાલી));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -અંક_લીટીઓ * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - અંક_લીટીઓ + 1,
	gui.scroll_region_ડાબું,
	gui.scroll_region_bot,
	gui.scroll_region_જમણું);
}

    શૂન્ય
gui_mch_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ડાબું);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_જમણું + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - અંક_લીટીઓ + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ખાલી));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = અંક_લીટીઓ * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_ડાબું,
	    row + અંક_લીટીઓ - 1, gui.scroll_region_જમણું);
}

    શૂન્ય
gui_mch_draw_string(પૂર્ણાંક row, પૂર્ણાંક col, char_u *s, પૂર્ણાંક len, પૂર્ણાંક flags)
{
    સ્થિર અક્ષર *utf8_buffer = ખાલી;
    સ્થિર પૂર્ણાંક	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    જો (is_ignore_draw == TRUE)
	પાછા;

    DRAW_START;

    જો (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    જો (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    જો (charset_translate != ખાલી && enc_utf8 == 0)
    {
	પૂર્ણાંક src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	જો (utf8_len < len)
	{
	    અક્ષર *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    જો (new_buffer == ખાલી)
		પાછા;
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

    જો (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#જો 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(ખાલી);
#નહીંતર
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    જો (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    શૂન્ય
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

    શૂન્ય
gui_mch_draw_part_cursor(પૂર્ણાંક w, પૂર્ણાંક h, guicolor_T color)
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

    પૂર્ણાંક
gui_mch_is_blinking(શૂન્ય)
{
    પાછા blink_state != BLINK_NONE;
}

    પૂર્ણાંક
gui_mch_is_blink_off(શૂન્ય)
{
    પાછા blink_state == BLINK_OFF;
}

    શૂન્ય
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    શૂન્ય
gui_mch_start_blink(શૂન્ય)
{
    // Only turn on the timer on if none of the times are zero
    જો (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    શૂન્ય
gui_mch_stop_blink(પૂર્ણાંક may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    જો (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    શૂન્ય
gui_mch_beep(શૂન્ય)
{
    PtBeep();
}

    શૂન્ય
gui_mch_flash(પૂર્ણાંક msec)
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

    શૂન્ય
gui_mch_flush(શૂન્ય)
{
    PgFlush();
}

    શૂન્ય
gui_mch_set_text_area_pos(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    પૂર્ણાંક
gui_mch_haskey(char_u *name)
{
    પૂર્ણાંક i;

    માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != 0; i++)
	જો (name[0] == special_ચાવીઓ[i].vim_code0 &&
		 name[1] == special_ચાવીઓ[i].vim_code1)
	    પાછા OK;
    પાછા FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
# include "toolbar.phi"

સ્થિર PhImage_t *gui_ph_toolbar_images[] = {
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

સ્થિર PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    સ્થિર PhImage_t external_icon;
    PhImage_t *temp_phi = ખાલી;

    temp_phi = PxLoadImage(iconfile, ખાલી);
    જો (temp_phi != ખાલી)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, કદ(external_icon));
	મુક્ત(temp_phi);

	temp_phi = &external_icon;
    }
    પાછા temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
સ્થિર PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = ખાલી;

    જો (menu->icon_builtin == FALSE)
    {
	જો (menu->iconfile != ખાલી)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	જો ((icon == ખાલી) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	જો (icon != ખાલી)
	    પાછા icon;
    }

    જો (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	પાછા gui_ph_toolbar_images[menu->iconidx];

    પાછા ખાલી;
}
#endif

#જો defined(FEAT_MENU)
    શૂન્ય
gui_mch_enable_menu(પૂર્ણાંક flag)
{
    જો (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    નહીંતર
	PtUnrealizeWidget(gui.vimMenuBar);
}

    શૂન્ય
gui_mch_set_menu_pos(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    સ્થિર શૂન્ય
gui_ph_position_menu(PtWidget_t *widget, પૂર્ણાંક priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    જ્યારે (traverse != ખાલી)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	જો (menu != ખાલી &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_ઉમેરો(widget, traverse, 1);
	    પાછા;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    શૂન્ય
gui_mch_add_menu(vimmenu_T *menu, પૂર્ણાંક index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_ચાવી;
    char_u	mnemonic_str[MB_LEN_MAX];
    પૂર્ણાંક	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = ખાલી;

    જો (menu_is_menubar(menu->name))
    {

	accel_ચાવી = vim_strchr(menu->name, '&');
	જો (accel_ચાવી != ખાલી)
	{
	    mnemonic_str[0] = accel_ચાવી[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	જો (accel_ચાવી != ખાલી)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_ચાવી, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	જો (parent != ખાલી)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_જમણું, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == ખાલી) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	જો (parent != ખાલી)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	જો (parent == ખાલી)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    જો (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	જો (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    નહીંતર જો (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, ખાલી);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    શૂન્ય
gui_mch_add_menu_item(vimmenu_T *menu, પૂર્ણાંક index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_ચાવી;
    char_u	mnemonic_str[MB_LEN_MAX];
    પૂર્ણાંક	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    જો (menu_is_toolbar(parent->name))
    {
	જો (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_ટોચ_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	નહીંતર
	{
	    જો (strstr((અચલ અક્ષર *) p_toolbar, "text") != ખાલી)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    જો ((strstr((અચલ અક્ષર *) p_toolbar, "icons") != ખાલી) &&
		    (gui_ph_toolbar_images != ખાલી))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    જો (strstr((અચલ અક્ષર *) p_toolbar, "tooltips") != ખાલી)
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
	જો (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    નહીંતર
# endif
	જો (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    નહીંતર
    {
	accel_ચાવી = vim_strchr(menu->name, '&');
	જો (accel_ચાવી != ખાલી)
	{
	    mnemonic_str[0] = accel_ચાવી[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	જો (accel_ચાવી != ખાલી)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_ચાવી, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	જો (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    શૂન્ય
gui_mch_destroy_menu(vimmenu_T *menu)
{
    જો (menu->submenu_id != ખાલી)
	PtDestroyWidget(menu->submenu_id);
    જો (menu->id != ખાલી)
	PtDestroyWidget(menu->id);

    menu->submenu_id = ખાલી;
    menu->id = ખાલી;
}

    શૂન્ય
gui_mch_menu_grey(vimmenu_T *menu, પૂર્ણાંક grey)
{
    long    flags, mask, fields;

    જો (menu->id == ખાલી)
	પાછા;

    flags = PtWidgetFlags(menu->id);
    જો (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    નહીંતર
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    જો (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    શૂન્ય
gui_mch_menu_hidden(vimmenu_T *menu, પૂર્ણાંક hidden)
{
    // TODO: [un]realize the widget?
}

    શૂન્ય
gui_mch_draw_menubar(શૂન્ય)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    શૂન્ય
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    શૂન્ય
gui_mch_toggle_tearoffs(પૂર્ણાંક enable)
{
    // No tearoffs yet
}

#endif

#જો defined(FEAT_TOOLBAR)
    શૂન્ય
gui_mch_show_toolbar(પૂર્ણાંક showit)
{
    જો (showit)
	PtRealizeWidget(gui.vimToolBar);
    નહીંતર
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    સ્થિર GuiFont
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
    જો (font_tag != ખાલી)
    {
	જો (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != ખાલી)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    જો (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    જો (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    જો (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    જો (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    જો (font_size == info.size &&
		    style == (info.style & style))
		પાછા (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    પાછા ખાલી;
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

    સ્થિર પૂર્ણાંક
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    જો (mark == ખાલી)
	name_len = STRLEN(vim_font);
    નહીંતર
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    જો (*font_name != ખાલી)
    {
	જો (mark != ખાલી)
	{
	    જ્યારે (*mark != NUL && *mark++ == ':')
	    {
		પસંદગી (SAFE_tolower(*mark++))
		{
		    સ્થિતિ 'a': *font_flags |= PF_STYLE_ANTIALIAS; તોડો;
		    સ્થિતિ 'b': *font_flags |= PF_STYLE_BOLD; તોડો;
		    સ્થિતિ 'i': *font_flags |= PF_STYLE_ITALIC; તોડો;

		    સ્થિતિ 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			જો (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			તોડો;

		    મૂળભૂત:
			તોડો;
		}
	    }
	}
	પાછા TRUE;
    }
    પાછા FALSE;
}

    પૂર્ણાંક
gui_mch_init_font(char_u *vim_font_name, પૂર્ણાંક fontset)
{
    char_u  *font_tag;
    char_u  *font_name = ખાલી;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    જો (vim_font_name == ખાલી)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    જો (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, ખાલી, ખાલી,
		"pcterm12", -1, PHFONT_FIXED, ખાલી);

	જો (font_tag == ખાલી)
	    પાછા FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    નહીંતર
    {
	જો (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    પાછા FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	જો (font_tag == ખાલી)
	{
	    vim_free(font_name);
	    પાછા FAIL;
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
    PfExtentText(&extent, ખાલી, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    પાછા OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    પૂર્ણાંક
gui_mch_adjust_charheight(શૂન્ય)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    પાછા OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, પૂર્ણાંક report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    જો (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	જો (font_tag != ખાલી)
	    પાછા (GuiFont)font_tag;
    }

    જો (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    પાછા FAIL;
}

#જો defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    જો (name == ખાલી)
	પાછા ખાલી;
    પાછા vim_strsave(name);
}
#endif

    શૂન્ય
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    શૂન્ય
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

