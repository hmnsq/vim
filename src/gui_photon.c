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

#অন্তর্ভাৱ "vim.h"

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#যদি !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	পূর্ণাঙ্ক
# define PhEvent_t	পূর্ণাঙ্ক
# define PhPoint_t	পূর্ণাঙ্ক
# define PtWidget_t	পূর্ণাঙ্ক
# define Pg_BLACK	0
# define PtCallbackF_t	পূর্ণাঙ্ক
# define PtCallbackInfo_t পূর্ণাঙ্ক
# define PhTile_t	পূর্ণাঙ্ক
# define PtWidget_t	পূর্ণাঙ্ক
# define PhImage_t	পূর্ণাঙ্ক
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE পরিমাণ(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
স্থির ৱর্ণ	*empty_title = "    ";
স্থির ৱর্ণ	**panel_titles = রিক্ত;
স্থির ushort_t	অঙ্ক_panels = 0;
স্থির short pg_margin_বাম, pg_margin_ডান, pg_margin_শীৰ্ষ, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_ঢোকাও
স্থির PgColor_t gui_ph_mouse_color =	Pg_BLACK;

স্থির PhPoint_t    gui_ph_raw_offset;
স্থির PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
স্থির PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
স্থির short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
স্থির PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
স্থির পূর্ণাঙ্ক is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
স্থির সংরচনা PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
স্থির গণনা {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

স্থির long_u	blink_waittime	= 700;
স্থির long_u	blink_ontime	= 400;
স্থির long_u	blink_offtime	= 250;

স্থির সংরচনা
{
    পূর্ণাঙ্ক	    চাবি_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_চাবিগুলি[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_বাম,	    'k', 'l'},
    {Pk_ডান,	    'k', 'r'},

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
    {Pk_ঢোকাও,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //ঢোকাও
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

স্থির PtCallbackF_t gui_ph_handle_timer_cursor;
স্থির PtCallbackF_t gui_ph_handle_timer_timeout;

স্থির PtCallbackF_t gui_ph_handle_window_cb;

স্থির PtCallbackF_t gui_ph_handle_scrollbar;
স্থির PtCallbackF_t gui_ph_handle_keyboard;
স্থির PtCallbackF_t gui_ph_handle_mouse;
স্থির PtCallbackF_t gui_ph_handle_pulldown_menu;
স্থির PtCallbackF_t gui_ph_handle_menu;
স্থির PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

স্থির PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
স্থির PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
স্থির শূন্য gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

স্থির শূন্য gui_ph_draw_start(শূন্য);
স্থির শূন্য gui_ph_draw_end(শূন্য);

// Set the text for the balloon
স্থির PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     পূর্ণাঙ্ক position,
			     ৱর্ণ *text,
			     ৱর্ণ *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

স্থির PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     পূর্ণাঙ্ক position,
			     ৱর্ণ *text,
			     ৱর্ণ *font,
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
    যদি (menu != রিক্ত)
    {
	পূর্ণাঙ্ক index = MENU_INDEX_TIP;
	যদি (menu->strings[ index ] != রিক্ত)
	    tooltip = menu->strings[ index ];
    }

    ঘূৰাই PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_শীৰ্ষ,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    স্থির শূন্য
gui_ph_resize_container(শূন্য)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	শূন্য *other,
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
    যদি (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    অন্যথা
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#অন্যথা
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_শীৰ্ষ, &pg_margin_bottom,
	    &pg_margin_বাম, &pg_margin_ডান);
#endif
    ঘূৰাই Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    স্থির পূর্ণাঙ্ক
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	শূন্য *data,
	PtCallbackInfo_t *info)
{
    যদি (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    অন্যথা
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    ঘূৰাই Pt_CONTINUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    শূন্য		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    ঘূৰাই Pt_CONTINUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_window_cb(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    ৱিকল্প (we->event_f)
    {
	পক্ষ Ph_WM_CLOSE:
	    gui_shell_closed();
	    ভঙ্গ;

	পক্ষ Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    যদি (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    অন্যথা
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    ভঙ্গ;

	পক্ষ Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_বাম + pg_margin_ডান);
	    height -= (pg_margin_শীৰ্ষ + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    ভঙ্গ;

	স্ৱভাৱ:
	    ভঙ্গ;
    }

    ঘূৰাই Pt_CONTINUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_scrollbar(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    পূর্ণাঙ্ক	    মান, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    যদি (sb != রিক্ত)
    {
	মান = scroll->position;
	ৱিকল্প (scroll->action)
	{
	    পক্ষ Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		ভঙ্গ;

	    পক্ষ Pt_SCROLL_SET:
		// FIXME: return straight away here?
		ঘূৰাই Pt_CONTINUE;
		ভঙ্গ;
	}

	gui_drag_scrollbar(sb, মান, dragging);
    }
    ঘূৰাই Pt_CONTINUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_keyboard(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    Ph_চাবি_Event_t    *চাবি;
    unsigned ৱর্ণ   string[6];
    পূর্ণাঙ্ক		    len, i;
    পূর্ণাঙ্ক		    ch, modifiers;

    চাবি = PhGetData(info->event);

    ch = modifiers = len = 0;

    যদি (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    যদি (চাবি->চাবি_flags & Pk_KF_Compose)
	ঘূৰাই Pt_CONTINUE;

    যদি ((চাবি->চাবি_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_চাবি_Down(চাবি->চাবি_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	যদি (((চাবি->চাবি_mods & Pk_KM_Alt) &&
		    !(চাবি->চাবি_mods & Pk_KM_Shift) &&
		    !(চাবি->চাবি_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(চাবি->চাবি_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    ঘূৰাই Pt_CONTINUE;
	}
#endif

	বাবে (i = 0; special_চাবিগুলি[i].চাবি_sym != 0; i++)
	{
	    যদি (special_চাবিগুলি[i].চাবি_sym == চাবি->চাবি_cap)
	    {
		len = 0;
		যদি (special_চাবিগুলি[i].vim_code1 == NUL)
		    ch = special_চাবিগুলি[i].vim_code0;
		অন্যথা
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    যদি (চাবি->চাবি_cap >= Pk_KP_Enter && চাবি->চাবি_cap <= Pk_KP_9
			    && (চাবি->চাবি_mods & Pk_KM_অঙ্ক_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = চাবি->চাবি_cap - 0xf080;
		    }
		    অন্যথা
			ch = TO_SPECIAL(special_চাবিগুলি[i].vim_code0,
				special_চাবিগুলি[i].vim_code1);
		}
		ভঙ্গ;
	    }
	}

	যদি (চাবি->চাবি_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	যদি (চাবি->চাবি_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	যদি (চাবি->চাবি_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	যদি (special_চাবিগুলি[i].চাবি_sym == 0)
	{
	    ch = PhTo8859_1(চাবি);
	    যদি (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_চাবি_To_Mb(string, চাবি);
		যদি (len > 0)
		{
		    স্থির ৱর্ণ buf[6];
		    পূর্ণাঙ্ক src_taken, dst_made;
		    যদি (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_প্ৰবেশ_buf(buf, dst_made);
		    }
		    অন্যথা
		    {
			add_to_প্ৰবেশ_buf(string, len);
		    }

		    ঘূৰাই Pt_CONSUME;
		}
		len = 0;
		ch = চাবি->চাবি_cap;
		যদি (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    যদি (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			যদি ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			অন্যথা যদি (ch == '2')
			    ch = NUL;
			অন্যথা যদি (ch == '6')
			    ch = 0x1e;
			অন্যথা যদি (ch == '-')
			    ch = 0x1f;
			অন্যথা
			    modifiers |= MOD_MASK_CTRL;
		    }

		    যদি (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		অন্যথা
		{
		    ঘূৰাই Pt_CONTINUE;
		}
	    }
	    অন্যথা
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_চাবি(ch, &modifiers);
	যদি (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	যদি (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	অন্যথা
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    পূর্ণাঙ্ক int_ch = check_for_interrupt(ch, modifiers);

	    যদি (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_প্ৰবেশ_buf();
	    }
	}

	যদি (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	যদি (len > 0)
	{
	    add_to_প্ৰবেশ_buf(string, len);
	    ঘূৰাই Pt_CONSUME;
	}
    }

    ঘূৰাই Pt_CONTINUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_mouse(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    পূর্ণাঙ্ক		     button = 0, repeated_click, modifiers = 0x0;
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

    যদি (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	ঘূৰাই Pt_CONTINUE;
    }

    যদি (pointer->চাবি_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    যদি (pointer->চাবি_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    যদি (pointer->চাবি_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    যদি (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_বাম;

    যদি (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_ডান;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    যদি (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_মধ্য;

    // Catch a real release (not phantom or other releases
    যদি (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    যদি (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#যদি 0
    // Vim doesn't use button repeats
    যদি (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    যদি ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_গুনতি >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    ঘূৰাই Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    স্থির পূর্ণাঙ্ক
gui_ph_handle_focus(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    যদি (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, রিক্ত);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    অন্যথা
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, রিক্ত);
    }
    ঘূৰাই Pt_CONTINUE;
}

    স্থির শূন্য
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    যদি (is_ignore_draw == TRUE)
	ঘূৰাই;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#যদি 0
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
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, রিক্ত));

#যদি 1
    // Redraw individual damage regions
    যদি (damage->next != রিক্ত)
	damage = damage->next;

    যেতিয়ালৈ (damage != রিক্ত)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#অন্যথা
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	শূন্য *data,
	PtCallbackInfo_t *info)
{
    যদি (data != রিক্ত)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, রিক্ত);
	PtRealizeWidget(menu->submenu_id);
    }

    ঘূৰাই Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    স্থির পূর্ণাঙ্ক
gui_ph_handle_menu(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    যদি (data != রিক্ত)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    ঘূৰাই Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    স্থির পূর্ণাঙ্ক
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	শূন্য *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, রিক্ত);
    ঘূৰাই Pt_CONTINUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_window_open(
	PtWidget_t *widget,
	শূন্য *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ঘূৰাই Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    স্থির শূন্য
gui_ph_draw_start(শূন্য)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, রিক্ত));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    স্থির শূন্য
gui_ph_draw_end(শূন্য)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    স্থির vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *শীৰ্ষ_level = মূল_menu;
    vimmenu_T *বস্তুসমূহ = রিক্ত;

    যেতিয়ালৈ (শীৰ্ষ_level != রিক্ত &&
	    (STRCMP(শীৰ্ষ_level->dname, "Buffers") != 0))
	শীৰ্ষ_level = শীৰ্ষ_level->next;

    যদি (শীৰ্ষ_level != রিক্ত)
    {
	বস্তুসমূহ = শীৰ্ষ_level->children;

	যেতিয়ালৈ (বস্তুসমূহ != রিক্ত &&
		(STRCMP(বস্তুসমূহ->dname, name) != 0))
	    বস্তুসমূহ = বস্তুসমূহ->next;
    }
    ঘূৰাই বস্তুসমূহ;
}

    স্থির শূন্য
gui_ph_pg_set_buffer_অঙ্ক(int_u buf_অঙ্ক)
{
    পূর্ণাঙ্ক i;
    ৱর্ণ অনুসন্ধান[16];
    ৱর্ণ *mark;

    যদি (gui.vimTextArea == রিক্ত || buf_অঙ্ক == 0)
	ঘূৰাই;

    অনুসন্ধান[0] = '(';
    ultoa(buf_অঙ্ক, &অনুসন্ধান[1], 10);
    STRCAT(অনুসন্ধান, ")");

    বাবে (i = 0; i < অঙ্ক_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	যদি (mark != রিক্ত && STRCMP(mark, অনুসন্ধান) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    স্থির পূর্ণাঙ্ক
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	শূন্য *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    যদি (info->event != রিক্ত)
    {
	panel = info->cbdata;
	যদি (panel->new_panel != রিক্ত)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    যদি (menu)
		gui_menu_cb(menu);
	}
    }
    ঘূৰাই Pt_CONTINUE;
}

    স্থির শূন্য
gui_ph_get_panelgroup_margins(
	short *শীৰ্ষ,
	short *bottom,
	short *বাম,
	short *ডান)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ধ্রুৱ unsigned short *margin_শীৰ্ষ, *margin_bottom;
    ধ্রুৱ unsigned short *margin_বাম, *margin_ডান;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_ডান, &margin_ডান, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *শীৰ্ষ    = abs_raw_y;
    *bottom = *margin_bottom;

    *বাম  = abs_raw_x;
    *ডান = *margin_ডান;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    স্থির পূর্ণাঙ্ক
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    ৱর্ণ *mark;

    যদি (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	যদি (mark != রিক্ত)
	{
	    mark++;
	    যেতিয়ালৈ (SAFE_isdigit(*mark))
		mark++;

	    যদি (*mark == ')')
		ঘূৰাই TRUE;
	}
    }
    ঘূৰাই FALSE;
}

    স্থির শূন্য
gui_ph_pg_add_buffer(ৱর্ণ *name)
{
    ৱর্ণ **new_titles = রিক্ত;

    new_titles = ALLOC_MULT(ৱর্ণ *, (অঙ্ক_panels + 1));
    যদি (new_titles == রিক্ত)
	ঘূৰাই;

    যদি (অঙ্ক_panels > 0)
	memcpy(new_titles, panel_titles, অঙ্ক_panels * পরিমাণ(ৱর্ণ **));

    new_titles[ অঙ্ক_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    অঙ্ক_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    স্থির শূন্য
gui_ph_pg_remove_buffer(ৱর্ণ *name)
{
    পূর্ণাঙ্ক i;
    ৱর্ণ **new_titles = রিক্ত;

    // If there is only 1 panel, we just use the temporary place holder
    যদি (অঙ্ক_panels > 1)
    {
	new_titles = ALLOC_MULT(ৱর্ণ *, অঙ্ক_panels - 1);
	যদি (new_titles != রিক্ত)
	{
	    ৱর্ণ **s = new_titles;
	    // Copy all the titles except the one we're removing
	    বাবে (i = 0; i < অঙ্ক_panels; i++)
	    {
		যদি (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    অঙ্ক_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    অঙ্ক_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    অন্যথা
    {
	অঙ্ক_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    স্থির পূর্ণাঙ্ক
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	শূন্য *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    যদি (data != রিক্ত)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    ঘূৰাই Pt_CONTINUE;
}
#endif

    স্থির পূর্ণাঙ্ক
gui_ph_pane_resize(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    যদি (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    ঘূৰাই Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    শূন্য
gui_ph_encoding_changed(পূর্ণাঙ্ক new_encoding)
{
    // Default encoding is latin1
    ৱর্ণ *charset = "latin1";
    পূর্ণাঙ্ক i;

    সংরচনা {
	পূর্ণাঙ্ক encoding;
	ৱর্ণ *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    বাবে (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	যদি (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    শূন্য
gui_mch_prepare(পূর্ণাঙ্ক *argc, ৱর্ণ **argv)
{
    PtInit(রিক্ত);
}

    পূর্ণাঙ্ক
gui_mch_init(শূন্য)
{
    PtArg_t args[10];
    পূর্ণাঙ্ক	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    যদি (gui.event_buffer == রিক্ত)
	ঘূৰাই FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(রিক্ত, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, রিক্ত, n, args);
    যদি (gui.vimWindow == রিক্ত)
	ঘূৰাই FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, রিক্ত);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, রিক্ত);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    যদি (gui.vimPanelGroup == রিক্ত)
	ঘূৰাই FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, রিক্ত);
#অন্যথা
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    যদি (gui.vimContainer == রিক্ত)
	ঘূৰাই FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, রিক্ত);
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
    যদি (gui.vimTextArea == রিক্ত)
	ঘূৰাই FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, রিক্ত);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_চাবি,
	    gui_ph_handle_keyboard, রিক্ত);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, রিক্ত);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, রিক্ত);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, রিক্ত);
    যদি (gui_ph_timer_cursor == রিক্ত)
	ঘূৰাই FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, রিক্ত);
    যদি (gui_ph_timer_timeout == রিক্ত)
	ঘূৰাই FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, রিক্ত);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, রিক্ত);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_বাম_ডান,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    যদি (gui.vimToolBarGroup == রিক্ত)
	ঘূৰাই FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, রিক্ত);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    যদি (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    যদি (gui.vimMenuBar == রিক্ত)
	ঘূৰাই FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_বাম_ডান |Pt_শীৰ্ষ_ANCHORED_শীৰ্ষ, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    যদি (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    যদি (gui.vimToolBar == রিক্ত)
	ঘূৰাই FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
gui_mch_init_check(শূন্য)
{
    ঘূৰাই (is_photon_available == TRUE) ? OK : FAIL;
}

    পূর্ণাঙ্ক
gui_mch_open(শূন্য)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    যদি (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    ঘূৰাই (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    শূন্য
gui_mch_exit(পূর্ণাঙ্ক rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, রিক্ত);

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
    স্থির পূর্ণাঙ্ক
exit_gui_mch_update(শূন্য *data)
{
    *(পূর্ণাঙ্ক *)data = FALSE;
    ঘূৰাই Pt_END;
}

    শূন্য
gui_mch_update(শূন্য)
{
    পূর্ণাঙ্ক working = TRUE;

    PtAppAddWorkProc(রিক্ত, exit_gui_mch_update, &working);
    যেতিয়ালৈ ((working == TRUE) && !vim_is_প্ৰবেশ_buf_full())
	PtProcessEvent();
}

    পূর্ণাঙ্ক
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    যদি (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    যেতিয়ালৈ (1)
    {
	PtProcessEvent();
	যদি (প্ৰবেশ_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    ঘূৰাই OK;
	}
	অন্যথা যদি (is_timeout == TRUE)
	    ঘূৰাই FAIL;
    }
}

#যদি defined(FEAT_BROWSE)
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
	পূর্ণাঙ্ক saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    পূর্ণাঙ্ক	    flags;
    char_u  *default_path;
    char_u  *open_text = রিক্ত;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    যদি (default_path != রিক্ত)
    {
	যদি (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	যদি (initdir == রিক্ত || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	অন্যথা
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	যদি (default_name != রিক্ত)
	{
	    যদি (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		রিক্ত,
		title,
		default_path,
		রিক্ত,
		open_text,
		রিক্ত,
		রিক্ত,
		&file,
		flags);

	vim_free(default_path);

	যদি (file.ret == Pt_FSDIALOG_BTN1)
	    ঘূৰাই vim_strsave(file.path);
    }
    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_GUI_DIALOG)
স্থির PtWidget_t *gui_ph_dialog_text = রিক্ত;

    স্থির পূর্ণাঙ্ক
gui_ph_dialog_close(পূর্ণাঙ্ক button, শূন্য *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    যদি (gui_ph_dialog_text != রিক্ত)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (শূন্য *) button);

    ঘূৰাই Pt_TRUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    শূন্য		*data,
    PtCallbackInfo_t	*info)
{
    যদি (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    ঘূৰাই Pt_CONTINUE;
}

    স্থির পূর্ণাঙ্ক
gui_ph_dialog_esc(PtWidget_t *widget, শূন্য *data, PtCallbackInfo_t *info)
{
    Ph_চাবি_Event_t *চাবি;

    চাবি = PhGetData(info->event);
    যদি ((চাবি->চাবি_flags & Pk_KF_Cap_Valid) && (চাবি->চাবি_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	ঘূৰাই Pt_CONSUME;
    }
    ঘূৰাই Pt_PROCESS;
}

    পূর্ণাঙ্ক
gui_mch_dialog(
	পূর্ণাঙ্ক	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	পূর্ণাঙ্ক	default_button,
	char_u	*textfield,
	পূর্ণাঙ্ক	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    পূর্ণাঙ্ক		button_গুনতি;
    পূর্ণাঙ্ক		i, len;
    পূর্ণাঙ্ক		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_গুনতি = len = i = 0;

    যদি (buttons == রিক্ত || *buttons == NUL)
	ঘূৰাই -1;

    // There is one less separator than buttons, so bump up the button count
    button_গুনতি = 1;

    // Count string length and number of separators
    বাবে (str = buttons; *str; str++)
    {
	len++;
	যদি (*str == DLG_BUTTON_SEP)
	    button_গুনতি++;
    }

    যদি (title == রিক্ত)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_গুনতি);
    যদি (buttons_copy != রিক্ত && button_array != রিক্ত)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	বাবে (i = 0; i < button_গুনতি; i++)
	{
	    button_array[ i ] = str;
	    বাবে (; *str; str++)
	    {
		যদি (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    ভঙ্গ;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, রিক্ত,
		title,
		রিক্ত,
		message, রিক্ত,
		button_গুনতি, (ধ্রুৱ ৱর্ণ **) button_array, রিক্ত,
		default_button, 0, Pt_MODAL);
# অন্যথা
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    পূর্ণাঙ্ক n;
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
		    Pt_TRUE, Pt_GROUP_NO_চাবিগুলি | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, রিক্ত, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    যদি (textfield != রিক্ত)
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
	    di.buttons = (ধ্রুৱ ৱর্ণ **) button_array;
	    di.nbtns = button_গুনতি;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_গুনতি;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_চাবি,
		    gui_ph_dialog_esc, &modal_ctrl);

	    যদি (gui_ph_dialog_text != রিক্ত)
		PtGiveFocus(gui_ph_dialog_text, রিক্ত);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (পূর্ণাঙ্ক) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = রিক্ত;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    ঘূৰাই dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    পূর্ণাঙ্ক
gui_mch_get_winpos(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, রিক্ত);

    *x = pos->x;
    *y = pos->y;

    ঘূৰাই OK;
}

    শূন্য
gui_mch_set_winpos(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    শূন্য
gui_mch_set_shellsize(পূর্ণাঙ্ক width, পূর্ণাঙ্ক height,
	পূর্ণাঙ্ক min_width, পূর্ণাঙ্ক min_height, পূর্ণাঙ্ক base_width, পূর্ণাঙ্ক base_height,
	পূর্ণাঙ্ক direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_বাম + pg_margin_ডান;
    window_size.h += pg_margin_শীৰ্ষ + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    যদি (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    শূন্য
gui_mch_get_screen_dimensions(পূর্ণাঙ্ক *screen_w, পূর্ণাঙ্ক *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_প্ৰবেশ_Group(রিক্ত), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    শূন্য
gui_mch_iconify(শূন্য)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#যদি defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    শূন্য
gui_mch_set_foreground(শূন্য)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    শূন্য
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_অঙ্ক(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    শূন্য
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, পূর্ণাঙ্ক val, পূর্ণাঙ্ক size, পূর্ণাঙ্ক max)
{
    পূর্ণাঙ্ক	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_মান, val, 0);
    PtSetResources(sb->id, n, args);
}

    শূন্য
gui_mch_set_scrollbar_pos(scrollbar_T *sb, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    পূর্ণাঙ্ক
gui_mch_get_scrollbar_xpadding(শূন্য)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ঘূৰাই 0;
}

    পূর্ণাঙ্ক
gui_mch_get_scrollbar_ypadding(শূন্য)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ঘূৰাই 0;
}

    শূন্য
gui_mch_create_scrollbar(scrollbar_T *sb, পূর্ণাঙ্ক orient)
{
    পূর্ণাঙ্ক	    n = 0;
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
#যদি 0
    // Don't need this anchoring for the scrollbars
    যদি (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_বাম_ANCHORED_বাম | Pt_ডান_ANCHORED_ডান;
    }
    অন্যথা
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_শীৰ্ষ_ANCHORED_শীৰ্ষ;
	যদি (sb->wp != রিক্ত)
	{
	    যদি (sb == &sb->wp->w_scrollbars[ SBAR_বাম ])
		anchor_flags |= Pt_বাম_ANCHORED_বাম;
	    অন্যথা
		anchor_flags |= Pt_ডান_ANCHORED_ডান;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#অন্যথা
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    শূন্য
gui_mch_enable_scrollbar(scrollbar_T *sb, পূর্ণাঙ্ক flag)
{
    যদি (flag != 0)
	PtRealizeWidget(sb->id);
    অন্যথা
	PtUnrealizeWidget(sb->id);
}

    শূন্য
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = রিক্ত;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#যদি defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
স্থির পূর্ণাঙ্ক last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
স্থির পূর্ণাঙ্ক mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_ঢোকাও,		// beam
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

    শূন্য
mch_set_mouse_shape(পূর্ণাঙ্ক shape)
{
    পূর্ণাঙ্ক	    id;

    যদি (!gui.in_use)
	ঘূৰাই;

    যদি (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    অন্যথা
    {
	যদি (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	অন্যথা
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    যদি (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    শূন্য
gui_mch_mousehide(পূর্ণাঙ্ক hide)
{
    যদি (gui.pointer_hidden == hide)
	ঘূৰাই;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    যদি (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    অন্যথা
	mch_set_mouse_shape(last_shape);
#অন্যথা
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    শূন্য
gui_mch_getmouse(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_প্ৰবেশ_Group(রিক্ত), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    শূন্য
gui_mch_setmouse(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_প্ৰবেশ_Group(রিক্ত), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ঘূৰাই (guicolor_T)(PgRGB(Pg_Red_মান(pixel),
				     Pg_Green_মান(pixel), Pg_Blue_মান(pixel)));
}

    শূন্য
gui_mch_new_colors(শূন্য)
{
#যদি 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    যদি (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_মান(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_মান(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_মান(gui_ph_mouse_color) ^ 255;

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
    ঘূৰাই gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(পূর্ণাঙ্ক r, পূর্ণাঙ্ক g, পূর্ণাঙ্ক b)
{
    ঘূৰাই gui_get_rgb_color_cmn(r, g, b);
}

    শূন্য
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    শূন্য
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    শূন্য
gui_mch_set_sp_color(guicolor_T color)
{
}

    শূন্য
gui_mch_invert_rectangle(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক nr, পূর্ণাঙ্ক nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    যদি (nc > 0)
	rect.lr.x -= 1;
    যদি (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    শূন্য
gui_mch_clear_block(পূর্ণাঙ্ক row1, পূর্ণাঙ্ক col1, পূর্ণাঙ্ক row2, পূর্ণাঙ্ক col2)
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

    শূন্য
gui_mch_clear_all(শূন্য)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    যদি (is_ignore_draw == TRUE)
	ঘূৰাই;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    শূন্য
gui_mch_delete_সাৰিগুলি(পূর্ণাঙ্ক row, পূর্ণাঙ্ক অঙ্ক_সাৰিগুলি)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_বাম);
    rect.ul.y = FILL_Y(row + অঙ্ক_সাৰিগুলি);

    rect.lr.x = FILL_X(gui.scroll_region_ডান + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, রিক্ত));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -অঙ্ক_সাৰিগুলি * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - অঙ্ক_সাৰিগুলি + 1,
	gui.scroll_region_বাম,
	gui.scroll_region_bot,
	gui.scroll_region_ডান);
}

    শূন্য
gui_mch_ঢোকাও_সাৰিগুলি(পূর্ণাঙ্ক row, পূর্ণাঙ্ক অঙ্ক_সাৰিগুলি)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_বাম);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_ডান + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - অঙ্ক_সাৰিগুলি + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, রিক্ত));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = অঙ্ক_সাৰিগুলি * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_বাম,
	    row + অঙ্ক_সাৰিগুলি - 1, gui.scroll_region_ডান);
}

    শূন্য
gui_mch_draw_string(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, char_u *s, পূর্ণাঙ্ক len, পূর্ণাঙ্ক flags)
{
    স্থির ৱর্ণ *utf8_buffer = রিক্ত;
    স্থির পূর্ণাঙ্ক	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    যদি (is_ignore_draw == TRUE)
	ঘূৰাই;

    DRAW_START;

    যদি (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    যদি (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    যদি (charset_translate != রিক্ত && enc_utf8 == 0)
    {
	পূর্ণাঙ্ক src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	যদি (utf8_len < len)
	{
	    ৱর্ণ *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    যদি (new_buffer == রিক্ত)
		ঘূৰাই;
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

    যদি (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#যদি 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(রিক্ত);
#অন্যথা
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    যদি (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    শূন্য
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

    শূন্য
gui_mch_draw_part_cursor(পূর্ণাঙ্ক w, পূর্ণাঙ্ক h, guicolor_T color)
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

    পূর্ণাঙ্ক
gui_mch_is_blinking(শূন্য)
{
    ঘূৰাই blink_state != BLINK_NONE;
}

    পূর্ণাঙ্ক
gui_mch_is_blink_off(শূন্য)
{
    ঘূৰাই blink_state == BLINK_OFF;
}

    শূন্য
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    শূন্য
gui_mch_start_blink(শূন্য)
{
    // Only turn on the timer on if none of the times are zero
    যদি (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    শূন্য
gui_mch_stop_blink(পূর্ণাঙ্ক may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    যদি (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    শূন্য
gui_mch_beep(শূন্য)
{
    PtBeep();
}

    শূন্য
gui_mch_flash(পূর্ণাঙ্ক msec)
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

    শূন্য
gui_mch_flush(শূন্য)
{
    PgFlush();
}

    শূন্য
gui_mch_set_text_area_pos(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    পূর্ণাঙ্ক
gui_mch_haskey(char_u *name)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; special_চাবিগুলি[i].চাবি_sym != 0; i++)
	যদি (name[0] == special_চাবিগুলি[i].vim_code0 &&
		 name[1] == special_চাবিগুলি[i].vim_code1)
	    ঘূৰাই OK;
    ঘূৰাই FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
# include "toolbar.phi"

স্থির PhImage_t *gui_ph_toolbar_images[] = {
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

স্থির PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    স্থির PhImage_t external_icon;
    PhImage_t *temp_phi = রিক্ত;

    temp_phi = PxLoadImage(iconfile, রিক্ত);
    যদি (temp_phi != রিক্ত)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, পরিমাণ(external_icon));
	মুক্ত(temp_phi);

	temp_phi = &external_icon;
    }
    ঘূৰাই temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
স্থির PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = রিক্ত;

    যদি (menu->icon_builtin == FALSE)
    {
	যদি (menu->iconfile != রিক্ত)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	যদি ((icon == রিক্ত) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	যদি (icon != রিক্ত)
	    ঘূৰাই icon;
    }

    যদি (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	ঘূৰাই gui_ph_toolbar_images[menu->iconidx];

    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_MENU)
    শূন্য
gui_mch_enable_menu(পূর্ণাঙ্ক flag)
{
    যদি (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    অন্যথা
	PtUnrealizeWidget(gui.vimMenuBar);
}

    শূন্য
gui_mch_set_menu_pos(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    স্থির শূন্য
gui_ph_position_menu(PtWidget_t *widget, পূর্ণাঙ্ক priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    যেতিয়ালৈ (traverse != রিক্ত)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	যদি (menu != রিক্ত &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_ঢোকাও(widget, traverse, 1);
	    ঘূৰাই;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    শূন্য
gui_mch_add_menu(vimmenu_T *menu, পূর্ণাঙ্ক index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_চাবি;
    char_u	mnemonic_str[MB_LEN_MAX];
    পূর্ণাঙ্ক	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = রিক্ত;

    যদি (menu_is_menubar(menu->name))
    {

	accel_চাবি = vim_strchr(menu->name, '&');
	যদি (accel_চাবি != রিক্ত)
	{
	    mnemonic_str[0] = accel_চাবি[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	যদি (accel_চাবি != রিক্ত)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_চাবি, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	যদি (parent != রিক্ত)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_ডান, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == রিক্ত) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	যদি (parent != রিক্ত)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	যদি (parent == রিক্ত)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    যদি (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	যদি (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    অন্যথা যদি (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, রিক্ত);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    শূন্য
gui_mch_add_menu_item(vimmenu_T *menu, পূর্ণাঙ্ক index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_চাবি;
    char_u	mnemonic_str[MB_LEN_MAX];
    পূর্ণাঙ্ক	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    যদি (menu_is_toolbar(parent->name))
    {
	যদি (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_শীৰ্ষ_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	অন্যথা
	{
	    যদি (strstr((ধ্রুৱ ৱর্ণ *) p_toolbar, "text") != রিক্ত)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    যদি ((strstr((ধ্রুৱ ৱর্ণ *) p_toolbar, "icons") != রিক্ত) &&
		    (gui_ph_toolbar_images != রিক্ত))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    যদি (strstr((ধ্রুৱ ৱর্ণ *) p_toolbar, "tooltips") != রিক্ত)
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
	যদি (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    অন্যথা
# endif
	যদি (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    অন্যথা
    {
	accel_চাবি = vim_strchr(menu->name, '&');
	যদি (accel_চাবি != রিক্ত)
	{
	    mnemonic_str[0] = accel_চাবি[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	যদি (accel_চাবি != রিক্ত)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_চাবি, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	যদি (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    শূন্য
gui_mch_destroy_menu(vimmenu_T *menu)
{
    যদি (menu->submenu_id != রিক্ত)
	PtDestroyWidget(menu->submenu_id);
    যদি (menu->id != রিক্ত)
	PtDestroyWidget(menu->id);

    menu->submenu_id = রিক্ত;
    menu->id = রিক্ত;
}

    শূন্য
gui_mch_menu_grey(vimmenu_T *menu, পূর্ণাঙ্ক grey)
{
    long    flags, mask, fields;

    যদি (menu->id == রিক্ত)
	ঘূৰাই;

    flags = PtWidgetFlags(menu->id);
    যদি (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    অন্যথা
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    যদি (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    শূন্য
gui_mch_menu_hidden(vimmenu_T *menu, পূর্ণাঙ্ক hidden)
{
    // TODO: [un]realize the widget?
}

    শূন্য
gui_mch_draw_menubar(শূন্য)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    শূন্য
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    শূন্য
gui_mch_toggle_tearoffs(পূর্ণাঙ্ক enable)
{
    // No tearoffs yet
}

#endif

#যদি defined(FEAT_TOOLBAR)
    শূন্য
gui_mch_show_toolbar(পূর্ণাঙ্ক showit)
{
    যদি (showit)
	PtRealizeWidget(gui.vimToolBar);
    অন্যথা
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    স্থির GuiFont
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
    যদি (font_tag != রিক্ত)
    {
	যদি (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != রিক্ত)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    যদি (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    যদি (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    যদি (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    যদি (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    যদি (font_size == info.size &&
		    style == (info.style & style))
		ঘূৰাই (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    ঘূৰাই রিক্ত;
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

    স্থির পূর্ণাঙ্ক
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    যদি (mark == রিক্ত)
	name_len = STRLEN(vim_font);
    অন্যথা
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    যদি (*font_name != রিক্ত)
    {
	যদি (mark != রিক্ত)
	{
	    যেতিয়ালৈ (*mark != NUL && *mark++ == ':')
	    {
		ৱিকল্প (SAFE_tolower(*mark++))
		{
		    পক্ষ 'a': *font_flags |= PF_STYLE_ANTIALIAS; ভঙ্গ;
		    পক্ষ 'b': *font_flags |= PF_STYLE_BOLD; ভঙ্গ;
		    পক্ষ 'i': *font_flags |= PF_STYLE_ITALIC; ভঙ্গ;

		    পক্ষ 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			যদি (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			ভঙ্গ;

		    স্ৱভাৱ:
			ভঙ্গ;
		}
	    }
	}
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

    পূর্ণাঙ্ক
gui_mch_init_font(char_u *vim_font_name, পূর্ণাঙ্ক fontset)
{
    char_u  *font_tag;
    char_u  *font_name = রিক্ত;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    যদি (vim_font_name == রিক্ত)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    যদি (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, রিক্ত, রিক্ত,
		"pcterm12", -1, PHFONT_FIXED, রিক্ত);

	যদি (font_tag == রিক্ত)
	    ঘূৰাই FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    অন্যথা
    {
	যদি (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    ঘূৰাই FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	যদি (font_tag == রিক্ত)
	{
	    vim_free(font_name);
	    ঘূৰাই FAIL;
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
    PfExtentText(&extent, রিক্ত, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    ঘূৰাই OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    পূর্ণাঙ্ক
gui_mch_adjust_charheight(শূন্য)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    ঘূৰাই OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, পূর্ণাঙ্ক report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    যদি (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	যদি (font_tag != রিক্ত)
	    ঘূৰাই (GuiFont)font_tag;
    }

    যদি (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    ঘূৰাই FAIL;
}

#যদি defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    যদি (name == রিক্ত)
	ঘূৰাই রিক্ত;
    ঘূৰাই vim_strsave(name);
}
#endif

    শূন্য
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    শূন্য
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

