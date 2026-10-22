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

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#എങ്കിൽ !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	പൂർണ്ണസംഖ്യ
# define PhEvent_t	പൂർണ്ണസംഖ്യ
# define PhPoint_t	പൂർണ്ണസംഖ്യ
# define PtWidget_t	പൂർണ്ണസംഖ്യ
# define Pg_BLACK	0
# define PtCallbackF_t	പൂർണ്ണസംഖ്യ
# define PtCallbackInfo_t പൂർണ്ണസംഖ്യ
# define PhTile_t	പൂർണ്ണസംഖ്യ
# define PtWidget_t	പൂർണ്ണസംഖ്യ
# define PhImage_t	പൂർണ്ണസംഖ്യ
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE വലുപ്പം(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
സ്ഥിതം അക്ഷരം	*empty_title = "    ";
സ്ഥിതം അക്ഷരം	**panel_titles = ശൂന്യ;
സ്ഥിതം ushort_t	അക്കം_panels = 0;
സ്ഥിതം short pg_margin_ഇടത്, pg_margin_വലത്, pg_margin_മുകൾ, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_തിരുകുക
സ്ഥിതം PgColor_t gui_ph_mouse_color =	Pg_BLACK;

സ്ഥിതം PhPoint_t    gui_ph_raw_offset;
സ്ഥിതം PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
സ്ഥിതം PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
സ്ഥിതം short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
സ്ഥിതം PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
സ്ഥിതം പൂർണ്ണസംഖ്യ is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
സ്ഥിതം ഘടന PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
സ്ഥിതം എണ്ണൽ {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

സ്ഥിതം long_u	blink_waittime	= 700;
സ്ഥിതം long_u	blink_ontime	= 400;
സ്ഥിതം long_u	blink_offtime	= 250;

സ്ഥിതം ഘടന
{
    പൂർണ്ണസംഖ്യ	    താക്കോൽ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_താക്കോലുകൾ[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_ഇടത്,	    'k', 'l'},
    {Pk_വലത്,	    'k', 'r'},

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
    {Pk_തിരുകുക,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //തിരുകുക
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

സ്ഥിതം PtCallbackF_t gui_ph_handle_timer_cursor;
സ്ഥിതം PtCallbackF_t gui_ph_handle_timer_timeout;

സ്ഥിതം PtCallbackF_t gui_ph_handle_window_cb;

സ്ഥിതം PtCallbackF_t gui_ph_handle_scrollbar;
സ്ഥിതം PtCallbackF_t gui_ph_handle_keyboard;
സ്ഥിതം PtCallbackF_t gui_ph_handle_mouse;
സ്ഥിതം PtCallbackF_t gui_ph_handle_pulldown_menu;
സ്ഥിതം PtCallbackF_t gui_ph_handle_menu;
സ്ഥിതം PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

സ്ഥിതം PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
സ്ഥിതം PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
സ്ഥിതം ശൂന്യം gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

സ്ഥിതം ശൂന്യം gui_ph_draw_start(ശൂന്യം);
സ്ഥിതം ശൂന്യം gui_ph_draw_end(ശൂന്യം);

// Set the text for the balloon
സ്ഥിതം PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     പൂർണ്ണസംഖ്യ position,
			     അക്ഷരം *text,
			     അക്ഷരം *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

സ്ഥിതം PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     പൂർണ്ണസംഖ്യ position,
			     അക്ഷരം *text,
			     അക്ഷരം *font,
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
    എങ്കിൽ (menu != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ index = MENU_INDEX_TIP;
	എങ്കിൽ (menu->strings[ index ] != ശൂന്യ)
	    tooltip = menu->strings[ index ];
    }

    തിരികെ PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_മുകൾ,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    സ്ഥിതം ശൂന്യം
gui_ph_resize_container(ശൂന്യം)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	ശൂന്യം *other,
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
    എങ്കിൽ (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    അല്ലെങ്കിൽ
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#അല്ലെങ്കിൽ
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_മുകൾ, &pg_margin_bottom,
	    &pg_margin_ഇടത്, &pg_margin_വലത്);
#endif
    തിരികെ Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	ശൂന്യം *data,
	PtCallbackInfo_t *info)
{
    എങ്കിൽ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    അല്ലെങ്കിൽ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    ശൂന്യം		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_window_cb(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    തിരഞ്ഞെടുപ്പ് (we->event_f)
    {
	അവസ്ഥ Ph_WM_CLOSE:
	    gui_shell_closed();
	    നിർത്തുക;

	അവസ്ഥ Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    എങ്കിൽ (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    അല്ലെങ്കിൽ
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    നിർത്തുക;

	അവസ്ഥ Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_ഇടത് + pg_margin_വലത്);
	    height -= (pg_margin_മുകൾ + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }

    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_scrollbar(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    പൂർണ്ണസംഖ്യ	    മൂല്യം, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    എങ്കിൽ (sb != ശൂന്യ)
    {
	മൂല്യം = scroll->position;
	തിരഞ്ഞെടുപ്പ് (scroll->action)
	{
	    അവസ്ഥ Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		നിർത്തുക;

	    അവസ്ഥ Pt_SCROLL_SET:
		// FIXME: return straight away here?
		തിരികെ Pt_CONTINUE;
		നിർത്തുക;
	}

	gui_drag_scrollbar(sb, മൂല്യം, dragging);
    }
    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_keyboard(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    Ph_താക്കോൽ_Event_t    *താക്കോൽ;
    unsigned അക്ഷരം   string[6];
    പൂർണ്ണസംഖ്യ		    len, i;
    പൂർണ്ണസംഖ്യ		    ch, modifiers;

    താക്കോൽ = PhGetData(info->event);

    ch = modifiers = len = 0;

    എങ്കിൽ (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    എങ്കിൽ (താക്കോൽ->താക്കോൽ_flags & Pk_KF_Compose)
	തിരികെ Pt_CONTINUE;

    എങ്കിൽ ((താക്കോൽ->താക്കോൽ_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_താക്കോൽ_Down(താക്കോൽ->താക്കോൽ_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	എങ്കിൽ (((താക്കോൽ->താക്കോൽ_mods & Pk_KM_Alt) &&
		    !(താക്കോൽ->താക്കോൽ_mods & Pk_KM_Shift) &&
		    !(താക്കോൽ->താക്കോൽ_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(താക്കോൽ->താക്കോൽ_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    തിരികെ Pt_CONTINUE;
	}
#endif

	വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].താക്കോൽ_sym != 0; i++)
	{
	    എങ്കിൽ (special_താക്കോലുകൾ[i].താക്കോൽ_sym == താക്കോൽ->താക്കോൽ_cap)
	    {
		len = 0;
		എങ്കിൽ (special_താക്കോലുകൾ[i].vim_code1 == NUL)
		    ch = special_താക്കോലുകൾ[i].vim_code0;
		അല്ലെങ്കിൽ
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    എങ്കിൽ (താക്കോൽ->താക്കോൽ_cap >= Pk_KP_Enter && താക്കോൽ->താക്കോൽ_cap <= Pk_KP_9
			    && (താക്കോൽ->താക്കോൽ_mods & Pk_KM_അക്കം_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = താക്കോൽ->താക്കോൽ_cap - 0xf080;
		    }
		    അല്ലെങ്കിൽ
			ch = TO_SPECIAL(special_താക്കോലുകൾ[i].vim_code0,
				special_താക്കോലുകൾ[i].vim_code1);
		}
		നിർത്തുക;
	    }
	}

	എങ്കിൽ (താക്കോൽ->താക്കോൽ_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	എങ്കിൽ (താക്കോൽ->താക്കോൽ_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	എങ്കിൽ (താക്കോൽ->താക്കോൽ_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	എങ്കിൽ (special_താക്കോലുകൾ[i].താക്കോൽ_sym == 0)
	{
	    ch = PhTo8859_1(താക്കോൽ);
	    എങ്കിൽ (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_താക്കോൽ_To_Mb(string, താക്കോൽ);
		എങ്കിൽ (len > 0)
		{
		    സ്ഥിതം അക്ഷരം buf[6];
		    പൂർണ്ണസംഖ്യ src_taken, dst_made;
		    എങ്കിൽ (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_നിവേശം_buf(buf, dst_made);
		    }
		    അല്ലെങ്കിൽ
		    {
			add_to_നിവേശം_buf(string, len);
		    }

		    തിരികെ Pt_CONSUME;
		}
		len = 0;
		ch = താക്കോൽ->താക്കോൽ_cap;
		എങ്കിൽ (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    എങ്കിൽ (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			എങ്കിൽ ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			അല്ലെങ്കിൽ എങ്കിൽ (ch == '2')
			    ch = NUL;
			അല്ലെങ്കിൽ എങ്കിൽ (ch == '6')
			    ch = 0x1e;
			അല്ലെങ്കിൽ എങ്കിൽ (ch == '-')
			    ch = 0x1f;
			അല്ലെങ്കിൽ
			    modifiers |= MOD_MASK_CTRL;
		    }

		    എങ്കിൽ (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    തിരികെ Pt_CONTINUE;
		}
	    }
	    അല്ലെങ്കിൽ
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_താക്കോൽ(ch, &modifiers);
	എങ്കിൽ (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	എങ്കിൽ (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	അല്ലെങ്കിൽ
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    പൂർണ്ണസംഖ്യ int_ch = check_for_interrupt(ch, modifiers);

	    എങ്കിൽ (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_നിവേശം_buf();
	    }
	}

	എങ്കിൽ (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	എങ്കിൽ (len > 0)
	{
	    add_to_നിവേശം_buf(string, len);
	    തിരികെ Pt_CONSUME;
	}
    }

    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_mouse(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    പൂർണ്ണസംഖ്യ		     button = 0, repeated_click, modifiers = 0x0;
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

    എങ്കിൽ (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	തിരികെ Pt_CONTINUE;
    }

    എങ്കിൽ (pointer->താക്കോൽ_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    എങ്കിൽ (pointer->താക്കോൽ_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    എങ്കിൽ (pointer->താക്കോൽ_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    എങ്കിൽ (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_ഇടത്;

    എങ്കിൽ (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_വലത്;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    എങ്കിൽ (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_നടുവ്;

    // Catch a real release (not phantom or other releases
    എങ്കിൽ (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    എങ്കിൽ (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#എങ്കിൽ 0
    // Vim doesn't use button repeats
    എങ്കിൽ (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    എങ്കിൽ ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_എണ്ണം >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    തിരികെ Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_focus(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    എങ്കിൽ (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ശൂന്യ);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    അല്ലെങ്കിൽ
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ശൂന്യ);
    }
    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം ശൂന്യം
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    എങ്കിൽ (is_ignore_draw == TRUE)
	തിരികെ;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#എങ്കിൽ 0
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
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, ശൂന്യ));

#എങ്കിൽ 1
    // Redraw individual damage regions
    എങ്കിൽ (damage->next != ശൂന്യ)
	damage = damage->next;

    ആകുമ്പോൾ (damage != ശൂന്യ)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#അല്ലെങ്കിൽ
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	ശൂന്യം *data,
	PtCallbackInfo_t *info)
{
    എങ്കിൽ (data != ശൂന്യ)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, ശൂന്യ);
	PtRealizeWidget(menu->submenu_id);
    }

    തിരികെ Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_menu(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    എങ്കിൽ (data != ശൂന്യ)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    തിരികെ Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	ശൂന്യം *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, ശൂന്യ);
    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_window_open(
	PtWidget_t *widget,
	ശൂന്യം *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    തിരികെ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    സ്ഥിതം ശൂന്യം
gui_ph_draw_start(ശൂന്യം)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ശൂന്യ));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    സ്ഥിതം ശൂന്യം
gui_ph_draw_end(ശൂന്യം)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    സ്ഥിതം vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *മുകൾ_level = വേര്_menu;
    vimmenu_T *ഇനങ്ങൾ = ശൂന്യ;

    ആകുമ്പോൾ (മുകൾ_level != ശൂന്യ &&
	    (STRCMP(മുകൾ_level->dname, "Buffers") != 0))
	മുകൾ_level = മുകൾ_level->next;

    എങ്കിൽ (മുകൾ_level != ശൂന്യ)
    {
	ഇനങ്ങൾ = മുകൾ_level->children;

	ആകുമ്പോൾ (ഇനങ്ങൾ != ശൂന്യ &&
		(STRCMP(ഇനങ്ങൾ->dname, name) != 0))
	    ഇനങ്ങൾ = ഇനങ്ങൾ->next;
    }
    തിരികെ ഇനങ്ങൾ;
}

    സ്ഥിതം ശൂന്യം
gui_ph_pg_set_buffer_അക്കം(int_u buf_അക്കം)
{
    പൂർണ്ണസംഖ്യ i;
    അക്ഷരം തിരയൽ[16];
    അക്ഷരം *mark;

    എങ്കിൽ (gui.vimTextArea == ശൂന്യ || buf_അക്കം == 0)
	തിരികെ;

    തിരയൽ[0] = '(';
    ultoa(buf_അക്കം, &തിരയൽ[1], 10);
    STRCAT(തിരയൽ, ")");

    വേണ്ടി (i = 0; i < അക്കം_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	എങ്കിൽ (mark != ശൂന്യ && STRCMP(mark, തിരയൽ) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	ശൂന്യം *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    എങ്കിൽ (info->event != ശൂന്യ)
    {
	panel = info->cbdata;
	എങ്കിൽ (panel->new_panel != ശൂന്യ)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    എങ്കിൽ (menu)
		gui_menu_cb(menu);
	}
    }
    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം ശൂന്യം
gui_ph_get_panelgroup_margins(
	short *മുകൾ,
	short *bottom,
	short *ഇടത്,
	short *വലത്)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    സ്ഥിരം unsigned short *margin_മുകൾ, *margin_bottom;
    സ്ഥിരം unsigned short *margin_ഇടത്, *margin_വലത്;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_വലത്, &margin_വലത്, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *മുകൾ    = abs_raw_y;
    *bottom = *margin_bottom;

    *ഇടത്  = abs_raw_x;
    *വലത് = *margin_വലത്;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    അക്ഷരം *mark;

    എങ്കിൽ (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	എങ്കിൽ (mark != ശൂന്യ)
	{
	    mark++;
	    ആകുമ്പോൾ (SAFE_isdigit(*mark))
		mark++;

	    എങ്കിൽ (*mark == ')')
		തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

    സ്ഥിതം ശൂന്യം
gui_ph_pg_add_buffer(അക്ഷരം *name)
{
    അക്ഷരം **new_titles = ശൂന്യ;

    new_titles = ALLOC_MULT(അക്ഷരം *, (അക്കം_panels + 1));
    എങ്കിൽ (new_titles == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (അക്കം_panels > 0)
	memcpy(new_titles, panel_titles, അക്കം_panels * വലുപ്പം(അക്ഷരം **));

    new_titles[ അക്കം_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    അക്കം_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    സ്ഥിതം ശൂന്യം
gui_ph_pg_remove_buffer(അക്ഷരം *name)
{
    പൂർണ്ണസംഖ്യ i;
    അക്ഷരം **new_titles = ശൂന്യ;

    // If there is only 1 panel, we just use the temporary place holder
    എങ്കിൽ (അക്കം_panels > 1)
    {
	new_titles = ALLOC_MULT(അക്ഷരം *, അക്കം_panels - 1);
	എങ്കിൽ (new_titles != ശൂന്യ)
	{
	    അക്ഷരം **s = new_titles;
	    // Copy all the titles except the one we're removing
	    വേണ്ടി (i = 0; i < അക്കം_panels; i++)
	    {
		എങ്കിൽ (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    അക്കം_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    അക്കം_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    അല്ലെങ്കിൽ
    {
	അക്കം_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	ശൂന്യം *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    എങ്കിൽ (data != ശൂന്യ)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    തിരികെ Pt_CONTINUE;
}
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_pane_resize(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    എങ്കിൽ (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    തിരികെ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    ശൂന്യം
gui_ph_encoding_changed(പൂർണ്ണസംഖ്യ new_encoding)
{
    // Default encoding is latin1
    അക്ഷരം *charset = "latin1";
    പൂർണ്ണസംഖ്യ i;

    ഘടന {
	പൂർണ്ണസംഖ്യ encoding;
	അക്ഷരം *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    വേണ്ടി (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	എങ്കിൽ (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    ശൂന്യം
gui_mch_prepare(പൂർണ്ണസംഖ്യ *argc, അക്ഷരം **argv)
{
    PtInit(ശൂന്യ);
}

    പൂർണ്ണസംഖ്യ
gui_mch_init(ശൂന്യം)
{
    PtArg_t args[10];
    പൂർണ്ണസംഖ്യ	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    എങ്കിൽ (gui.event_buffer == ശൂന്യ)
	തിരികെ FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(ശൂന്യ, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, ശൂന്യ, n, args);
    എങ്കിൽ (gui.vimWindow == ശൂന്യ)
	തിരികെ FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, ശൂന്യ);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, ശൂന്യ);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    എങ്കിൽ (gui.vimPanelGroup == ശൂന്യ)
	തിരികെ FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, ശൂന്യ);
#അല്ലെങ്കിൽ
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    എങ്കിൽ (gui.vimContainer == ശൂന്യ)
	തിരികെ FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, ശൂന്യ);
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
    എങ്കിൽ (gui.vimTextArea == ശൂന്യ)
	തിരികെ FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, ശൂന്യ);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_താക്കോൽ,
	    gui_ph_handle_keyboard, ശൂന്യ);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, ശൂന്യ);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, ശൂന്യ);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, ശൂന്യ);
    എങ്കിൽ (gui_ph_timer_cursor == ശൂന്യ)
	തിരികെ FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, ശൂന്യ);
    എങ്കിൽ (gui_ph_timer_timeout == ശൂന്യ)
	തിരികെ FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, ശൂന്യ);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, ശൂന്യ);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ഇടത്_വലത്,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    എങ്കിൽ (gui.vimToolBarGroup == ശൂന്യ)
	തിരികെ FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, ശൂന്യ);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    എങ്കിൽ (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    എങ്കിൽ (gui.vimMenuBar == ശൂന്യ)
	തിരികെ FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_ഇടത്_വലത് |Pt_മുകൾ_ANCHORED_മുകൾ, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    എങ്കിൽ (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    എങ്കിൽ (gui.vimToolBar == ശൂന്യ)
	തിരികെ FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
gui_mch_init_check(ശൂന്യം)
{
    തിരികെ (is_photon_available == TRUE) ? OK : FAIL;
}

    പൂർണ്ണസംഖ്യ
gui_mch_open(ശൂന്യം)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    എങ്കിൽ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    തിരികെ (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    ശൂന്യം
gui_mch_exit(പൂർണ്ണസംഖ്യ rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, ശൂന്യ);

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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exit_gui_mch_update(ശൂന്യം *data)
{
    *(പൂർണ്ണസംഖ്യ *)data = FALSE;
    തിരികെ Pt_END;
}

    ശൂന്യം
gui_mch_update(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ working = TRUE;

    PtAppAddWorkProc(ശൂന്യ, exit_gui_mch_update, &working);
    ആകുമ്പോൾ ((working == TRUE) && !vim_is_നിവേശം_buf_full())
	PtProcessEvent();
}

    പൂർണ്ണസംഖ്യ
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    എങ്കിൽ (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    ആകുമ്പോൾ (1)
    {
	PtProcessEvent();
	എങ്കിൽ (നിവേശം_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    തിരികെ OK;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (is_timeout == TRUE)
	    തിരികെ FAIL;
    }
}

#എങ്കിൽ defined(FEAT_BROWSE)
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
	പൂർണ്ണസംഖ്യ saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    പൂർണ്ണസംഖ്യ	    flags;
    char_u  *default_path;
    char_u  *open_text = ശൂന്യ;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    എങ്കിൽ (default_path != ശൂന്യ)
    {
	എങ്കിൽ (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	എങ്കിൽ (initdir == ശൂന്യ || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	അല്ലെങ്കിൽ
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	എങ്കിൽ (default_name != ശൂന്യ)
	{
	    എങ്കിൽ (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		ശൂന്യ,
		title,
		default_path,
		ശൂന്യ,
		open_text,
		ശൂന്യ,
		ശൂന്യ,
		&file,
		flags);

	vim_free(default_path);

	എങ്കിൽ (file.ret == Pt_FSDIALOG_BTN1)
	    തിരികെ vim_strsave(file.path);
    }
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_GUI_DIALOG)
സ്ഥിതം PtWidget_t *gui_ph_dialog_text = ശൂന്യ;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_dialog_close(പൂർണ്ണസംഖ്യ button, ശൂന്യം *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    എങ്കിൽ (gui_ph_dialog_text != ശൂന്യ)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (ശൂന്യം *) button);

    തിരികെ Pt_TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    ശൂന്യം		*data,
    PtCallbackInfo_t	*info)
{
    എങ്കിൽ (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    തിരികെ Pt_CONTINUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_dialog_esc(PtWidget_t *widget, ശൂന്യം *data, PtCallbackInfo_t *info)
{
    Ph_താക്കോൽ_Event_t *താക്കോൽ;

    താക്കോൽ = PhGetData(info->event);
    എങ്കിൽ ((താക്കോൽ->താക്കോൽ_flags & Pk_KF_Cap_Valid) && (താക്കോൽ->താക്കോൽ_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	തിരികെ Pt_CONSUME;
    }
    തിരികെ Pt_PROCESS;
}

    പൂർണ്ണസംഖ്യ
gui_mch_dialog(
	പൂർണ്ണസംഖ്യ	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	പൂർണ്ണസംഖ്യ	default_button,
	char_u	*textfield,
	പൂർണ്ണസംഖ്യ	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    പൂർണ്ണസംഖ്യ		button_എണ്ണം;
    പൂർണ്ണസംഖ്യ		i, len;
    പൂർണ്ണസംഖ്യ		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_എണ്ണം = len = i = 0;

    എങ്കിൽ (buttons == ശൂന്യ || *buttons == NUL)
	തിരികെ -1;

    // There is one less separator than buttons, so bump up the button count
    button_എണ്ണം = 1;

    // Count string length and number of separators
    വേണ്ടി (str = buttons; *str; str++)
    {
	len++;
	എങ്കിൽ (*str == DLG_BUTTON_SEP)
	    button_എണ്ണം++;
    }

    എങ്കിൽ (title == ശൂന്യ)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_എണ്ണം);
    എങ്കിൽ (buttons_copy != ശൂന്യ && button_array != ശൂന്യ)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	വേണ്ടി (i = 0; i < button_എണ്ണം; i++)
	{
	    button_array[ i ] = str;
	    വേണ്ടി (; *str; str++)
	    {
		എങ്കിൽ (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    നിർത്തുക;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, ശൂന്യ,
		title,
		ശൂന്യ,
		message, ശൂന്യ,
		button_എണ്ണം, (സ്ഥിരം അക്ഷരം **) button_array, ശൂന്യ,
		default_button, 0, Pt_MODAL);
# അല്ലെങ്കിൽ
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    പൂർണ്ണസംഖ്യ n;
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
		    Pt_TRUE, Pt_GROUP_NO_താക്കോലുകൾ | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, ശൂന്യ, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    എങ്കിൽ (textfield != ശൂന്യ)
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
	    di.buttons = (സ്ഥിരം അക്ഷരം **) button_array;
	    di.nbtns = button_എണ്ണം;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_എണ്ണം;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_താക്കോൽ,
		    gui_ph_dialog_esc, &modal_ctrl);

	    എങ്കിൽ (gui_ph_dialog_text != ശൂന്യ)
		PtGiveFocus(gui_ph_dialog_text, ശൂന്യ);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (പൂർണ്ണസംഖ്യ) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = ശൂന്യ;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    തിരികെ dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    പൂർണ്ണസംഖ്യ
gui_mch_get_winpos(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, ശൂന്യ);

    *x = pos->x;
    *y = pos->y;

    തിരികെ OK;
}

    ശൂന്യം
gui_mch_set_winpos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    ശൂന്യം
gui_mch_set_shellsize(പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height,
	പൂർണ്ണസംഖ്യ min_width, പൂർണ്ണസംഖ്യ min_height, പൂർണ്ണസംഖ്യ base_width, പൂർണ്ണസംഖ്യ base_height,
	പൂർണ്ണസംഖ്യ direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_ഇടത് + pg_margin_വലത്;
    window_size.h += pg_margin_മുകൾ + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    എങ്കിൽ (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    ശൂന്യം
gui_mch_get_screen_dimensions(പൂർണ്ണസംഖ്യ *screen_w, പൂർണ്ണസംഖ്യ *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_നിവേശം_Group(ശൂന്യ), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    ശൂന്യം
gui_mch_iconify(ശൂന്യം)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ശൂന്യം
gui_mch_set_foreground(ശൂന്യം)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    ശൂന്യം
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_അക്കം(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    ശൂന്യം
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, പൂർണ്ണസംഖ്യ val, പൂർണ്ണസംഖ്യ size, പൂർണ്ണസംഖ്യ max)
{
    പൂർണ്ണസംഖ്യ	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_മൂല്യം, val, 0);
    PtSetResources(sb->id, n, args);
}

    ശൂന്യം
gui_mch_set_scrollbar_pos(scrollbar_T *sb, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_xpadding(ശൂന്യം)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    തിരികെ 0;
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_ypadding(ശൂന്യം)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    തിരികെ 0;
}

    ശൂന്യം
gui_mch_create_scrollbar(scrollbar_T *sb, പൂർണ്ണസംഖ്യ orient)
{
    പൂർണ്ണസംഖ്യ	    n = 0;
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
#എങ്കിൽ 0
    // Don't need this anchoring for the scrollbars
    എങ്കിൽ (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_ഇടത്_ANCHORED_ഇടത് | Pt_വലത്_ANCHORED_വലത്;
    }
    അല്ലെങ്കിൽ
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_മുകൾ_ANCHORED_മുകൾ;
	എങ്കിൽ (sb->wp != ശൂന്യ)
	{
	    എങ്കിൽ (sb == &sb->wp->w_scrollbars[ SBAR_ഇടത് ])
		anchor_flags |= Pt_ഇടത്_ANCHORED_ഇടത്;
	    അല്ലെങ്കിൽ
		anchor_flags |= Pt_വലത്_ANCHORED_വലത്;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#അല്ലെങ്കിൽ
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    ശൂന്യം
gui_mch_enable_scrollbar(scrollbar_T *sb, പൂർണ്ണസംഖ്യ flag)
{
    എങ്കിൽ (flag != 0)
	PtRealizeWidget(sb->id);
    അല്ലെങ്കിൽ
	PtUnrealizeWidget(sb->id);
}

    ശൂന്യം
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = ശൂന്യ;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#എങ്കിൽ defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
സ്ഥിതം പൂർണ്ണസംഖ്യ last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
സ്ഥിതം പൂർണ്ണസംഖ്യ mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_തിരുകുക,		// beam
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

    ശൂന്യം
mch_set_mouse_shape(പൂർണ്ണസംഖ്യ shape)
{
    പൂർണ്ണസംഖ്യ	    id;

    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    എങ്കിൽ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	അല്ലെങ്കിൽ
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    എങ്കിൽ (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    ശൂന്യം
gui_mch_mousehide(പൂർണ്ണസംഖ്യ hide)
{
    എങ്കിൽ (gui.pointer_hidden == hide)
	തിരികെ;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    എങ്കിൽ (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    അല്ലെങ്കിൽ
	mch_set_mouse_shape(last_shape);
#അല്ലെങ്കിൽ
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    ശൂന്യം
gui_mch_getmouse(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_നിവേശം_Group(ശൂന്യ), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    ശൂന്യം
gui_mch_setmouse(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_നിവേശം_Group(ശൂന്യ), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    തിരികെ (guicolor_T)(PgRGB(Pg_Red_മൂല്യം(pixel),
				     Pg_Green_മൂല്യം(pixel), Pg_Blue_മൂല്യം(pixel)));
}

    ശൂന്യം
gui_mch_new_colors(ശൂന്യം)
{
#എങ്കിൽ 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    എങ്കിൽ (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_മൂല്യം(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_മൂല്യം(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_മൂല്യം(gui_ph_mouse_color) ^ 255;

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
    തിരികെ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ g, പൂർണ്ണസംഖ്യ b)
{
    തിരികെ gui_get_rgb_color_cmn(r, g, b);
}

    ശൂന്യം
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    ശൂന്യം
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    ശൂന്യം
gui_mch_set_sp_color(guicolor_T color)
{
}

    ശൂന്യം
gui_mch_invert_rectangle(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ nr, പൂർണ്ണസംഖ്യ nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    എങ്കിൽ (nc > 0)
	rect.lr.x -= 1;
    എങ്കിൽ (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    ശൂന്യം
gui_mch_clear_block(പൂർണ്ണസംഖ്യ row1, പൂർണ്ണസംഖ്യ col1, പൂർണ്ണസംഖ്യ row2, പൂർണ്ണസംഖ്യ col2)
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

    ശൂന്യം
gui_mch_clear_all(ശൂന്യം)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    എങ്കിൽ (is_ignore_draw == TRUE)
	തിരികെ;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    ശൂന്യം
gui_mch_delete_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ഇടത്);
    rect.ul.y = FILL_Y(row + അക്കം_വരികൾ);

    rect.lr.x = FILL_X(gui.scroll_region_വലത് + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ശൂന്യ));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -അക്കം_വരികൾ * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - അക്കം_വരികൾ + 1,
	gui.scroll_region_ഇടത്,
	gui.scroll_region_bot,
	gui.scroll_region_വലത്);
}

    ശൂന്യം
gui_mch_തിരുകുക_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ഇടത്);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_വലത് + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - അക്കം_വരികൾ + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ശൂന്യ));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = അക്കം_വരികൾ * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_ഇടത്,
	    row + അക്കം_വരികൾ - 1, gui.scroll_region_വലത്);
}

    ശൂന്യം
gui_mch_draw_string(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ flags)
{
    സ്ഥിതം അക്ഷരം *utf8_buffer = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    എങ്കിൽ (is_ignore_draw == TRUE)
	തിരികെ;

    DRAW_START;

    എങ്കിൽ (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    എങ്കിൽ (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    എങ്കിൽ (charset_translate != ശൂന്യ && enc_utf8 == 0)
    {
	പൂർണ്ണസംഖ്യ src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	എങ്കിൽ (utf8_len < len)
	{
	    അക്ഷരം *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    എങ്കിൽ (new_buffer == ശൂന്യ)
		തിരികെ;
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

    എങ്കിൽ (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#എങ്കിൽ 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(ശൂന്യ);
#അല്ലെങ്കിൽ
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    എങ്കിൽ (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    ശൂന്യം
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

    ശൂന്യം
gui_mch_draw_part_cursor(പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h, guicolor_T color)
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

    പൂർണ്ണസംഖ്യ
gui_mch_is_blinking(ശൂന്യം)
{
    തിരികെ blink_state != BLINK_NONE;
}

    പൂർണ്ണസംഖ്യ
gui_mch_is_blink_off(ശൂന്യം)
{
    തിരികെ blink_state == BLINK_OFF;
}

    ശൂന്യം
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ശൂന്യം
gui_mch_start_blink(ശൂന്യം)
{
    // Only turn on the timer on if none of the times are zero
    എങ്കിൽ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ശൂന്യം
gui_mch_stop_blink(പൂർണ്ണസംഖ്യ may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    എങ്കിൽ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    ശൂന്യം
gui_mch_beep(ശൂന്യം)
{
    PtBeep();
}

    ശൂന്യം
gui_mch_flash(പൂർണ്ണസംഖ്യ msec)
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

    ശൂന്യം
gui_mch_flush(ശൂന്യം)
{
    PgFlush();
}

    ശൂന്യം
gui_mch_set_text_area_pos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    പൂർണ്ണസംഖ്യ
gui_mch_haskey(char_u *name)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].താക്കോൽ_sym != 0; i++)
	എങ്കിൽ (name[0] == special_താക്കോലുകൾ[i].vim_code0 &&
		 name[1] == special_താക്കോലുകൾ[i].vim_code1)
	    തിരികെ OK;
    തിരികെ FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
# include "toolbar.phi"

സ്ഥിതം PhImage_t *gui_ph_toolbar_images[] = {
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

സ്ഥിതം PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    സ്ഥിതം PhImage_t external_icon;
    PhImage_t *temp_phi = ശൂന്യ;

    temp_phi = PxLoadImage(iconfile, ശൂന്യ);
    എങ്കിൽ (temp_phi != ശൂന്യ)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, വലുപ്പം(external_icon));
	സ്വതന്ത്രം(temp_phi);

	temp_phi = &external_icon;
    }
    തിരികെ temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
സ്ഥിതം PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = ശൂന്യ;

    എങ്കിൽ (menu->icon_builtin == FALSE)
    {
	എങ്കിൽ (menu->iconfile != ശൂന്യ)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	എങ്കിൽ ((icon == ശൂന്യ) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	എങ്കിൽ (icon != ശൂന്യ)
	    തിരികെ icon;
    }

    എങ്കിൽ (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	തിരികെ gui_ph_toolbar_images[menu->iconidx];

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_MENU)
    ശൂന്യം
gui_mch_enable_menu(പൂർണ്ണസംഖ്യ flag)
{
    എങ്കിൽ (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    അല്ലെങ്കിൽ
	PtUnrealizeWidget(gui.vimMenuBar);
}

    ശൂന്യം
gui_mch_set_menu_pos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    സ്ഥിതം ശൂന്യം
gui_ph_position_menu(PtWidget_t *widget, പൂർണ്ണസംഖ്യ priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    ആകുമ്പോൾ (traverse != ശൂന്യ)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	എങ്കിൽ (menu != ശൂന്യ &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_തിരുകുക(widget, traverse, 1);
	    തിരികെ;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    ശൂന്യം
gui_mch_add_menu(vimmenu_T *menu, പൂർണ്ണസംഖ്യ index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_താക്കോൽ;
    char_u	mnemonic_str[MB_LEN_MAX];
    പൂർണ്ണസംഖ്യ	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = ശൂന്യ;

    എങ്കിൽ (menu_is_menubar(menu->name))
    {

	accel_താക്കോൽ = vim_strchr(menu->name, '&');
	എങ്കിൽ (accel_താക്കോൽ != ശൂന്യ)
	{
	    mnemonic_str[0] = accel_താക്കോൽ[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	എങ്കിൽ (accel_താക്കോൽ != ശൂന്യ)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_താക്കോൽ, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	എങ്കിൽ (parent != ശൂന്യ)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_വലത്, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == ശൂന്യ) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	എങ്കിൽ (parent != ശൂന്യ)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	എങ്കിൽ (parent == ശൂന്യ)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    എങ്കിൽ (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	എങ്കിൽ (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, ശൂന്യ);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    ശൂന്യം
gui_mch_add_menu_item(vimmenu_T *menu, പൂർണ്ണസംഖ്യ index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_താക്കോൽ;
    char_u	mnemonic_str[MB_LEN_MAX];
    പൂർണ്ണസംഖ്യ	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    എങ്കിൽ (menu_is_toolbar(parent->name))
    {
	എങ്കിൽ (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_മുകൾ_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (strstr((സ്ഥിരം അക്ഷരം *) p_toolbar, "text") != ശൂന്യ)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    എങ്കിൽ ((strstr((സ്ഥിരം അക്ഷരം *) p_toolbar, "icons") != ശൂന്യ) &&
		    (gui_ph_toolbar_images != ശൂന്യ))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    എങ്കിൽ (strstr((സ്ഥിരം അക്ഷരം *) p_toolbar, "tooltips") != ശൂന്യ)
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
	എങ്കിൽ (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    അല്ലെങ്കിൽ
# endif
	എങ്കിൽ (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    അല്ലെങ്കിൽ
    {
	accel_താക്കോൽ = vim_strchr(menu->name, '&');
	എങ്കിൽ (accel_താക്കോൽ != ശൂന്യ)
	{
	    mnemonic_str[0] = accel_താക്കോൽ[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	എങ്കിൽ (accel_താക്കോൽ != ശൂന്യ)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_താക്കോൽ, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	എങ്കിൽ (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    ശൂന്യം
gui_mch_destroy_menu(vimmenu_T *menu)
{
    എങ്കിൽ (menu->submenu_id != ശൂന്യ)
	PtDestroyWidget(menu->submenu_id);
    എങ്കിൽ (menu->id != ശൂന്യ)
	PtDestroyWidget(menu->id);

    menu->submenu_id = ശൂന്യ;
    menu->id = ശൂന്യ;
}

    ശൂന്യം
gui_mch_menu_grey(vimmenu_T *menu, പൂർണ്ണസംഖ്യ grey)
{
    long    flags, mask, fields;

    എങ്കിൽ (menu->id == ശൂന്യ)
	തിരികെ;

    flags = PtWidgetFlags(menu->id);
    എങ്കിൽ (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    അല്ലെങ്കിൽ
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    എങ്കിൽ (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    ശൂന്യം
gui_mch_menu_hidden(vimmenu_T *menu, പൂർണ്ണസംഖ്യ hidden)
{
    // TODO: [un]realize the widget?
}

    ശൂന്യം
gui_mch_draw_menubar(ശൂന്യം)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    ശൂന്യം
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    ശൂന്യം
gui_mch_toggle_tearoffs(പൂർണ്ണസംഖ്യ enable)
{
    // No tearoffs yet
}

#endif

#എങ്കിൽ defined(FEAT_TOOLBAR)
    ശൂന്യം
gui_mch_show_toolbar(പൂർണ്ണസംഖ്യ showit)
{
    എങ്കിൽ (showit)
	PtRealizeWidget(gui.vimToolBar);
    അല്ലെങ്കിൽ
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    സ്ഥിതം GuiFont
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
    എങ്കിൽ (font_tag != ശൂന്യ)
    {
	എങ്കിൽ (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != ശൂന്യ)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    എങ്കിൽ (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    എങ്കിൽ (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    എങ്കിൽ (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    എങ്കിൽ (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    എങ്കിൽ (font_size == info.size &&
		    style == (info.style & style))
		തിരികെ (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    തിരികെ ശൂന്യ;
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

    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    എങ്കിൽ (mark == ശൂന്യ)
	name_len = STRLEN(vim_font);
    അല്ലെങ്കിൽ
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    എങ്കിൽ (*font_name != ശൂന്യ)
    {
	എങ്കിൽ (mark != ശൂന്യ)
	{
	    ആകുമ്പോൾ (*mark != NUL && *mark++ == ':')
	    {
		തിരഞ്ഞെടുപ്പ് (SAFE_tolower(*mark++))
		{
		    അവസ്ഥ 'a': *font_flags |= PF_STYLE_ANTIALIAS; നിർത്തുക;
		    അവസ്ഥ 'b': *font_flags |= PF_STYLE_BOLD; നിർത്തുക;
		    അവസ്ഥ 'i': *font_flags |= PF_STYLE_ITALIC; നിർത്തുക;

		    അവസ്ഥ 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			എങ്കിൽ (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			നിർത്തുക;

		    സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
	    }
	}
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

    പൂർണ്ണസംഖ്യ
gui_mch_init_font(char_u *vim_font_name, പൂർണ്ണസംഖ്യ fontset)
{
    char_u  *font_tag;
    char_u  *font_name = ശൂന്യ;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    എങ്കിൽ (vim_font_name == ശൂന്യ)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    എങ്കിൽ (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, ശൂന്യ, ശൂന്യ,
		"pcterm12", -1, PHFONT_FIXED, ശൂന്യ);

	എങ്കിൽ (font_tag == ശൂന്യ)
	    തിരികെ FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    തിരികെ FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	എങ്കിൽ (font_tag == ശൂന്യ)
	{
	    vim_free(font_name);
	    തിരികെ FAIL;
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
    PfExtentText(&extent, ശൂന്യ, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    തിരികെ OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    പൂർണ്ണസംഖ്യ
gui_mch_adjust_charheight(ശൂന്യം)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    തിരികെ OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, പൂർണ്ണസംഖ്യ report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    എങ്കിൽ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	എങ്കിൽ (font_tag != ശൂന്യ)
	    തിരികെ (GuiFont)font_tag;
    }

    എങ്കിൽ (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    തിരികെ FAIL;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strsave(name);
}
#endif

    ശൂന്യം
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    ശൂന്യം
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

