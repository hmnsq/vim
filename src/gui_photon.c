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

#अन्तर्भाव "vim.h"

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#जइ !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	पूर्णाङ्क
# define PhEvent_t	पूर्णाङ्क
# define PhPoint_t	पूर्णाङ्क
# define PtWidget_t	पूर्णाङ्क
# define Pg_BLACK	0
# define PtCallbackF_t	पूर्णाङ्क
# define PtCallbackInfo_t पूर्णाङ्क
# define PhTile_t	पूर्णाङ्क
# define PtWidget_t	पूर्णाङ्क
# define PhImage_t	पूर्णाङ्क
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE परिमाण(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
स्थिर वर्ण	*empty_title = "    ";
स्थिर वर्ण	**panel_titles = रिक्त;
स्थिर ushort_t	अङ्कः_panels = 0;
स्थिर short pg_margin_वामः, pg_margin_दक्षिणः, pg_margin_शिखरम्, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_निवेशय
स्थिर PgColor_t gui_ph_mouse_color =	Pg_BLACK;

स्थिर PhPoint_t    gui_ph_raw_offset;
स्थिर PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
स्थिर PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
स्थिर short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
स्थिर PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
स्थिर पूर्णाङ्क is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
स्थिर संरचना PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
स्थिर गणना {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

स्थिर long_u	blink_waittime	= 700;
स्थिर long_u	blink_ontime	= 400;
स्थिर long_u	blink_offtime	= 250;

स्थिर संरचना
{
    पूर्णाङ्क	    कुञ्चिका_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_कुञ्चिकाः[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_वामः,	    'k', 'l'},
    {Pk_दक्षिणः,	    'k', 'r'},

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
    {Pk_निवेशय,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //निवेशय
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

स्थिर PtCallbackF_t gui_ph_handle_timer_cursor;
स्थिर PtCallbackF_t gui_ph_handle_timer_timeout;

स्थिर PtCallbackF_t gui_ph_handle_window_cb;

स्थिर PtCallbackF_t gui_ph_handle_scrollbar;
स्थिर PtCallbackF_t gui_ph_handle_keyboard;
स्थिर PtCallbackF_t gui_ph_handle_mouse;
स्थिर PtCallbackF_t gui_ph_handle_pulldown_menu;
स्थिर PtCallbackF_t gui_ph_handle_menu;
स्थिर PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

स्थिर PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
स्थिर PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
स्थिर शून्य gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

स्थिर शून्य gui_ph_draw_start(शून्य);
स्थिर शून्य gui_ph_draw_end(शून्य);

// Set the text for the balloon
स्थिर PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     पूर्णाङ्क position,
			     वर्ण *text,
			     वर्ण *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

स्थिर PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     पूर्णाङ्क position,
			     वर्ण *text,
			     वर्ण *font,
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
    जइ (menu != रिक्त)
    {
	पूर्णाङ्क index = MENU_INDEX_TIP;
	जइ (menu->strings[ index ] != रिक्त)
	    tooltip = menu->strings[ index ];
    }

    प्रत्यागम PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_शिखरम्,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    स्थिर शून्य
gui_ph_resize_container(शून्य)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    स्थिर पूर्णाङ्क
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	शून्य *other,
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
    जइ (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    अण्णहा
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#अण्णहा
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_शिखरम्, &pg_margin_bottom,
	    &pg_margin_वामः, &pg_margin_दक्षिणः);
#endif
    प्रत्यागम Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    स्थिर पूर्णाङ्क
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    जइ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    अण्णहा
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    प्रत्यागम Pt_CONTINUE;
}

    स्थिर पूर्णाङ्क
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    शून्य		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    प्रत्यागम Pt_CONTINUE;
}

    स्थिर पूर्णाङ्क
gui_ph_handle_window_cb(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    विकल्प (we->event_f)
    {
	पक्ष Ph_WM_CLOSE:
	    gui_shell_closed();
	    भङ्ग;

	पक्ष Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    जइ (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    अण्णहा
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    भङ्ग;

	पक्ष Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_वामः + pg_margin_दक्षिणः);
	    height -= (pg_margin_शिखरम् + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    भङ्ग;

	स्वभाव:
	    भङ्ग;
    }

    प्रत्यागम Pt_CONTINUE;
}

    स्थिर पूर्णाङ्क
gui_ph_handle_scrollbar(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    पूर्णाङ्क	    मानम्, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    जइ (sb != रिक्त)
    {
	मानम् = scroll->position;
	विकल्प (scroll->action)
	{
	    पक्ष Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		भङ्ग;

	    पक्ष Pt_SCROLL_SET:
		// FIXME: return straight away here?
		प्रत्यागम Pt_CONTINUE;
		भङ्ग;
	}

	gui_drag_scrollbar(sb, मानम्, dragging);
    }
    प्रत्यागम Pt_CONTINUE;
}

    स्थिर पूर्णाङ्क
gui_ph_handle_keyboard(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    Ph_कुञ्चिका_Event_t    *कुञ्चिका;
    unsigned वर्ण   string[6];
    पूर्णाङ्क		    len, i;
    पूर्णाङ्क		    ch, modifiers;

    कुञ्चिका = PhGetData(info->event);

    ch = modifiers = len = 0;

    जइ (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    जइ (कुञ्चिका->कुञ्चिका_flags & Pk_KF_Compose)
	प्रत्यागम Pt_CONTINUE;

    जइ ((कुञ्चिका->कुञ्चिका_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_कुञ्चिका_Down(कुञ्चिका->कुञ्चिका_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	जइ (((कुञ्चिका->कुञ्चिका_mods & Pk_KM_Alt) &&
		    !(कुञ्चिका->कुञ्चिका_mods & Pk_KM_Shift) &&
		    !(कुञ्चिका->कुञ्चिका_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(कुञ्चिका->कुञ्चिका_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    प्रत्यागम Pt_CONTINUE;
	}
#endif

	प्रति (i = 0; special_कुञ्चिकाः[i].कुञ्चिका_sym != 0; i++)
	{
	    जइ (special_कुञ्चिकाः[i].कुञ्चिका_sym == कुञ्चिका->कुञ्चिका_cap)
	    {
		len = 0;
		जइ (special_कुञ्चिकाः[i].vim_code1 == NUL)
		    ch = special_कुञ्चिकाः[i].vim_code0;
		अण्णहा
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    जइ (कुञ्चिका->कुञ्चिका_cap >= Pk_KP_Enter && कुञ्चिका->कुञ्चिका_cap <= Pk_KP_9
			    && (कुञ्चिका->कुञ्चिका_mods & Pk_KM_अङ्कः_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = कुञ्चिका->कुञ्चिका_cap - 0xf080;
		    }
		    अण्णहा
			ch = TO_SPECIAL(special_कुञ्चिकाः[i].vim_code0,
				special_कुञ्चिकाः[i].vim_code1);
		}
		भङ्ग;
	    }
	}

	जइ (कुञ्चिका->कुञ्चिका_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	जइ (कुञ्चिका->कुञ्चिका_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	जइ (कुञ्चिका->कुञ्चिका_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	जइ (special_कुञ्चिकाः[i].कुञ्चिका_sym == 0)
	{
	    ch = PhTo8859_1(कुञ्चिका);
	    जइ (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_कुञ्चिका_To_Mb(string, कुञ्चिका);
		जइ (len > 0)
		{
		    स्थिर वर्ण buf[6];
		    पूर्णाङ्क src_taken, dst_made;
		    जइ (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_निवेशः_buf(buf, dst_made);
		    }
		    अण्णहा
		    {
			add_to_निवेशः_buf(string, len);
		    }

		    प्रत्यागम Pt_CONSUME;
		}
		len = 0;
		ch = कुञ्चिका->कुञ्चिका_cap;
		जइ (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    जइ (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			जइ ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			अण्णहा जइ (ch == '2')
			    ch = NUL;
			अण्णहा जइ (ch == '6')
			    ch = 0x1e;
			अण्णहा जइ (ch == '-')
			    ch = 0x1f;
			अण्णहा
			    modifiers |= MOD_MASK_CTRL;
		    }

		    जइ (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		अण्णहा
		{
		    प्रत्यागम Pt_CONTINUE;
		}
	    }
	    अण्णहा
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_कुञ्चिका(ch, &modifiers);
	जइ (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	जइ (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	अण्णहा
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    पूर्णाङ्क int_ch = check_for_interrupt(ch, modifiers);

	    जइ (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_निवेशः_buf();
	    }
	}

	जइ (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	जइ (len > 0)
	{
	    add_to_निवेशः_buf(string, len);
	    प्रत्यागम Pt_CONSUME;
	}
    }

    प्रत्यागम Pt_CONTINUE;
}

    स्थिर पूर्णाङ्क
gui_ph_handle_mouse(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    पूर्णाङ्क		     button = 0, repeated_click, modifiers = 0x0;
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

    जइ (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	प्रत्यागम Pt_CONTINUE;
    }

    जइ (pointer->कुञ्चिका_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    जइ (pointer->कुञ्चिका_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    जइ (pointer->कुञ्चिका_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    जइ (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_वामः;

    जइ (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_दक्षिणः;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    जइ (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_मध्यः;

    // Catch a real release (not phantom or other releases
    जइ (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    जइ (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#जइ 0
    // Vim doesn't use button repeats
    जइ (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    जइ ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_गणनम् >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    प्रत्यागम Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    स्थिर पूर्णाङ्क
gui_ph_handle_focus(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    जइ (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, रिक्त);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    अण्णहा
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, रिक्त);
    }
    प्रत्यागम Pt_CONTINUE;
}

    स्थिर शून्य
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    जइ (is_ignore_draw == TRUE)
	प्रत्यागम;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#जइ 0
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
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, रिक्त));

#जइ 1
    // Redraw individual damage regions
    जइ (damage->next != रिक्त)
	damage = damage->next;

    जाव (damage != रिक्त)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#अण्णहा
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    स्थिर पूर्णाङ्क
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    जइ (data != रिक्त)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, रिक्त);
	PtRealizeWidget(menu->submenu_id);
    }

    प्रत्यागम Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    स्थिर पूर्णाङ्क
gui_ph_handle_menu(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    जइ (data != रिक्त)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    प्रत्यागम Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    स्थिर पूर्णाङ्क
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, रिक्त);
    प्रत्यागम Pt_CONTINUE;
}

    स्थिर पूर्णाङ्क
gui_ph_handle_window_open(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    प्रत्यागम Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    स्थिर शून्य
gui_ph_draw_start(शून्य)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, रिक्त));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    स्थिर शून्य
gui_ph_draw_end(शून्य)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    स्थिर vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *शिखरम्_level = मूलम्_menu;
    vimmenu_T *वस्तूनि = रिक्त;

    जाव (शिखरम्_level != रिक्त &&
	    (STRCMP(शिखरम्_level->dname, "Buffers") != 0))
	शिखरम्_level = शिखरम्_level->next;

    जइ (शिखरम्_level != रिक्त)
    {
	वस्तूनि = शिखरम्_level->children;

	जाव (वस्तूनि != रिक्त &&
		(STRCMP(वस्तूनि->dname, name) != 0))
	    वस्तूनि = वस्तूनि->next;
    }
    प्रत्यागम वस्तूनि;
}

    स्थिर शून्य
gui_ph_pg_set_buffer_अङ्कः(int_u buf_अङ्कः)
{
    पूर्णाङ्क i;
    वर्ण अन्वेषणम्[16];
    वर्ण *mark;

    जइ (gui.vimTextArea == रिक्त || buf_अङ्कः == 0)
	प्रत्यागम;

    अन्वेषणम्[0] = '(';
    ultoa(buf_अङ्कः, &अन्वेषणम्[1], 10);
    STRCAT(अन्वेषणम्, ")");

    प्रति (i = 0; i < अङ्कः_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	जइ (mark != रिक्त && STRCMP(mark, अन्वेषणम्) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    स्थिर पूर्णाङ्क
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    जइ (info->event != रिक्त)
    {
	panel = info->cbdata;
	जइ (panel->new_panel != रिक्त)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    जइ (menu)
		gui_menu_cb(menu);
	}
    }
    प्रत्यागम Pt_CONTINUE;
}

    स्थिर शून्य
gui_ph_get_panelgroup_margins(
	short *शिखरम्,
	short *bottom,
	short *वामः,
	short *दक्षिणः)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ध्रुव unsigned short *margin_शिखरम्, *margin_bottom;
    ध्रुव unsigned short *margin_वामः, *margin_दक्षिणः;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_दक्षिणः, &margin_दक्षिणः, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *शिखरम्    = abs_raw_y;
    *bottom = *margin_bottom;

    *वामः  = abs_raw_x;
    *दक्षिणः = *margin_दक्षिणः;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    स्थिर पूर्णाङ्क
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    वर्ण *mark;

    जइ (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	जइ (mark != रिक्त)
	{
	    mark++;
	    जाव (SAFE_isdigit(*mark))
		mark++;

	    जइ (*mark == ')')
		प्रत्यागम TRUE;
	}
    }
    प्रत्यागम FALSE;
}

    स्थिर शून्य
gui_ph_pg_add_buffer(वर्ण *name)
{
    वर्ण **new_titles = रिक्त;

    new_titles = ALLOC_MULT(वर्ण *, (अङ्कः_panels + 1));
    जइ (new_titles == रिक्त)
	प्रत्यागम;

    जइ (अङ्कः_panels > 0)
	memcpy(new_titles, panel_titles, अङ्कः_panels * परिमाण(वर्ण **));

    new_titles[ अङ्कः_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    अङ्कः_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    स्थिर शून्य
gui_ph_pg_remove_buffer(वर्ण *name)
{
    पूर्णाङ्क i;
    वर्ण **new_titles = रिक्त;

    // If there is only 1 panel, we just use the temporary place holder
    जइ (अङ्कः_panels > 1)
    {
	new_titles = ALLOC_MULT(वर्ण *, अङ्कः_panels - 1);
	जइ (new_titles != रिक्त)
	{
	    वर्ण **s = new_titles;
	    // Copy all the titles except the one we're removing
	    प्रति (i = 0; i < अङ्कः_panels; i++)
	    {
		जइ (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    अङ्कः_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    अङ्कः_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    अण्णहा
    {
	अङ्कः_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    स्थिर पूर्णाङ्क
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    जइ (data != रिक्त)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    प्रत्यागम Pt_CONTINUE;
}
#endif

    स्थिर पूर्णाङ्क
gui_ph_pane_resize(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    जइ (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    प्रत्यागम Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    शून्य
gui_ph_encoding_changed(पूर्णाङ्क new_encoding)
{
    // Default encoding is latin1
    वर्ण *charset = "latin1";
    पूर्णाङ्क i;

    संरचना {
	पूर्णाङ्क encoding;
	वर्ण *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    प्रति (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	जइ (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    शून्य
gui_mch_prepare(पूर्णाङ्क *argc, वर्ण **argv)
{
    PtInit(रिक्त);
}

    पूर्णाङ्क
gui_mch_init(शून्य)
{
    PtArg_t args[10];
    पूर्णाङ्क	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    जइ (gui.event_buffer == रिक्त)
	प्रत्यागम FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(रिक्त, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, रिक्त, n, args);
    जइ (gui.vimWindow == रिक्त)
	प्रत्यागम FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, रिक्त);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, रिक्त);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    जइ (gui.vimPanelGroup == रिक्त)
	प्रत्यागम FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, रिक्त);
#अण्णहा
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    जइ (gui.vimContainer == रिक्त)
	प्रत्यागम FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, रिक्त);
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
    जइ (gui.vimTextArea == रिक्त)
	प्रत्यागम FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, रिक्त);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_कुञ्चिका,
	    gui_ph_handle_keyboard, रिक्त);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, रिक्त);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, रिक्त);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, रिक्त);
    जइ (gui_ph_timer_cursor == रिक्त)
	प्रत्यागम FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, रिक्त);
    जइ (gui_ph_timer_timeout == रिक्त)
	प्रत्यागम FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, रिक्त);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, रिक्त);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_वामः_दक्षिणः,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    जइ (gui.vimToolBarGroup == रिक्त)
	प्रत्यागम FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, रिक्त);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    जइ (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    जइ (gui.vimMenuBar == रिक्त)
	प्रत्यागम FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_वामः_दक्षिणः |Pt_शिखरम्_ANCHORED_शिखरम्, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    जइ (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    जइ (gui.vimToolBar == रिक्त)
	प्रत्यागम FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    प्रत्यागम OK;
}

    पूर्णाङ्क
gui_mch_init_check(शून्य)
{
    प्रत्यागम (is_photon_available == TRUE) ? OK : FAIL;
}

    पूर्णाङ्क
gui_mch_open(शून्य)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    जइ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    प्रत्यागम (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    शून्य
gui_mch_exit(पूर्णाङ्क rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, रिक्त);

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
    स्थिर पूर्णाङ्क
exit_gui_mch_update(शून्य *data)
{
    *(पूर्णाङ्क *)data = FALSE;
    प्रत्यागम Pt_END;
}

    शून्य
gui_mch_update(शून्य)
{
    पूर्णाङ्क working = TRUE;

    PtAppAddWorkProc(रिक्त, exit_gui_mch_update, &working);
    जाव ((working == TRUE) && !vim_is_निवेशः_buf_full())
	PtProcessEvent();
}

    पूर्णाङ्क
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    जइ (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    जाव (1)
    {
	PtProcessEvent();
	जइ (निवेशः_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    प्रत्यागम OK;
	}
	अण्णहा जइ (is_timeout == TRUE)
	    प्रत्यागम FAIL;
    }
}

#जइ defined(FEAT_BROWSE)
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
	पूर्णाङ्क saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    पूर्णाङ्क	    flags;
    char_u  *default_path;
    char_u  *open_text = रिक्त;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    जइ (default_path != रिक्त)
    {
	जइ (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	जइ (initdir == रिक्त || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	अण्णहा
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	जइ (default_name != रिक्त)
	{
	    जइ (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		रिक्त,
		title,
		default_path,
		रिक्त,
		open_text,
		रिक्त,
		रिक्त,
		&file,
		flags);

	vim_free(default_path);

	जइ (file.ret == Pt_FSDIALOG_BTN1)
	    प्रत्यागम vim_strsave(file.path);
    }
    प्रत्यागम रिक्त;
}
#endif

#जइ defined(FEAT_GUI_DIALOG)
स्थिर PtWidget_t *gui_ph_dialog_text = रिक्त;

    स्थिर पूर्णाङ्क
gui_ph_dialog_close(पूर्णाङ्क button, शून्य *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    जइ (gui_ph_dialog_text != रिक्त)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (शून्य *) button);

    प्रत्यागम Pt_TRUE;
}

    स्थिर पूर्णाङ्क
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    शून्य		*data,
    PtCallbackInfo_t	*info)
{
    जइ (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    प्रत्यागम Pt_CONTINUE;
}

    स्थिर पूर्णाङ्क
gui_ph_dialog_esc(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    Ph_कुञ्चिका_Event_t *कुञ्चिका;

    कुञ्चिका = PhGetData(info->event);
    जइ ((कुञ्चिका->कुञ्चिका_flags & Pk_KF_Cap_Valid) && (कुञ्चिका->कुञ्चिका_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	प्रत्यागम Pt_CONSUME;
    }
    प्रत्यागम Pt_PROCESS;
}

    पूर्णाङ्क
gui_mch_dialog(
	पूर्णाङ्क	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	पूर्णाङ्क	default_button,
	char_u	*textfield,
	पूर्णाङ्क	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    पूर्णाङ्क		button_गणनम्;
    पूर्णाङ्क		i, len;
    पूर्णाङ्क		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_गणनम् = len = i = 0;

    जइ (buttons == रिक्त || *buttons == NUL)
	प्रत्यागम -1;

    // There is one less separator than buttons, so bump up the button count
    button_गणनम् = 1;

    // Count string length and number of separators
    प्रति (str = buttons; *str; str++)
    {
	len++;
	जइ (*str == DLG_BUTTON_SEP)
	    button_गणनम्++;
    }

    जइ (title == रिक्त)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_गणनम्);
    जइ (buttons_copy != रिक्त && button_array != रिक्त)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	प्रति (i = 0; i < button_गणनम्; i++)
	{
	    button_array[ i ] = str;
	    प्रति (; *str; str++)
	    {
		जइ (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    भङ्ग;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, रिक्त,
		title,
		रिक्त,
		message, रिक्त,
		button_गणनम्, (ध्रुव वर्ण **) button_array, रिक्त,
		default_button, 0, Pt_MODAL);
# अण्णहा
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    पूर्णाङ्क n;
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
		    Pt_TRUE, Pt_GROUP_NO_कुञ्चिकाः | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, रिक्त, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    जइ (textfield != रिक्त)
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
	    di.buttons = (ध्रुव वर्ण **) button_array;
	    di.nbtns = button_गणनम्;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_गणनम्;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_कुञ्चिका,
		    gui_ph_dialog_esc, &modal_ctrl);

	    जइ (gui_ph_dialog_text != रिक्त)
		PtGiveFocus(gui_ph_dialog_text, रिक्त);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (पूर्णाङ्क) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = रिक्त;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    प्रत्यागम dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    पूर्णाङ्क
gui_mch_get_winpos(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, रिक्त);

    *x = pos->x;
    *y = pos->y;

    प्रत्यागम OK;
}

    शून्य
gui_mch_set_winpos(पूर्णाङ्क x, पूर्णाङ्क y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    शून्य
gui_mch_set_shellsize(पूर्णाङ्क width, पूर्णाङ्क height,
	पूर्णाङ्क min_width, पूर्णाङ्क min_height, पूर्णाङ्क base_width, पूर्णाङ्क base_height,
	पूर्णाङ्क direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_वामः + pg_margin_दक्षिणः;
    window_size.h += pg_margin_शिखरम् + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    जइ (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    शून्य
gui_mch_get_screen_dimensions(पूर्णाङ्क *screen_w, पूर्णाङ्क *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_निवेशः_Group(रिक्त), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    शून्य
gui_mch_iconify(शून्य)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#जइ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    शून्य
gui_mch_set_foreground(शून्य)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    शून्य
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_अङ्कः(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    शून्य
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, पूर्णाङ्क val, पूर्णाङ्क size, पूर्णाङ्क max)
{
    पूर्णाङ्क	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_मानम्, val, 0);
    PtSetResources(sb->id, n, args);
}

    शून्य
gui_mch_set_scrollbar_pos(scrollbar_T *sb, पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    पूर्णाङ्क
gui_mch_get_scrollbar_xpadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    प्रत्यागम 0;
}

    पूर्णाङ्क
gui_mch_get_scrollbar_ypadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    प्रत्यागम 0;
}

    शून्य
gui_mch_create_scrollbar(scrollbar_T *sb, पूर्णाङ्क orient)
{
    पूर्णाङ्क	    n = 0;
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
#जइ 0
    // Don't need this anchoring for the scrollbars
    जइ (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_वामः_ANCHORED_वामः | Pt_दक्षिणः_ANCHORED_दक्षिणः;
    }
    अण्णहा
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_शिखरम्_ANCHORED_शिखरम्;
	जइ (sb->wp != रिक्त)
	{
	    जइ (sb == &sb->wp->w_scrollbars[ SBAR_वामः ])
		anchor_flags |= Pt_वामः_ANCHORED_वामः;
	    अण्णहा
		anchor_flags |= Pt_दक्षिणः_ANCHORED_दक्षिणः;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#अण्णहा
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    शून्य
gui_mch_enable_scrollbar(scrollbar_T *sb, पूर्णाङ्क flag)
{
    जइ (flag != 0)
	PtRealizeWidget(sb->id);
    अण्णहा
	PtUnrealizeWidget(sb->id);
}

    शून्य
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = रिक्त;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#जइ defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
स्थिर पूर्णाङ्क last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
स्थिर पूर्णाङ्क mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_निवेशय,		// beam
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

    शून्य
mch_set_mouse_shape(पूर्णाङ्क shape)
{
    पूर्णाङ्क	    id;

    जइ (!gui.in_use)
	प्रत्यागम;

    जइ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    अण्णहा
    {
	जइ (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	अण्णहा
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    जइ (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    शून्य
gui_mch_mousehide(पूर्णाङ्क hide)
{
    जइ (gui.pointer_hidden == hide)
	प्रत्यागम;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    जइ (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    अण्णहा
	mch_set_mouse_shape(last_shape);
#अण्णहा
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    शून्य
gui_mch_getmouse(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_निवेशः_Group(रिक्त), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    शून्य
gui_mch_setmouse(पूर्णाङ्क x, पूर्णाङ्क y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_निवेशः_Group(रिक्त), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    प्रत्यागम (guicolor_T)(PgRGB(Pg_Red_मानम्(pixel),
				     Pg_Green_मानम्(pixel), Pg_Blue_मानम्(pixel)));
}

    शून्य
gui_mch_new_colors(शून्य)
{
#जइ 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    जइ (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_मानम्(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_मानम्(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_मानम्(gui_ph_mouse_color) ^ 255;

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
    प्रत्यागम gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(पूर्णाङ्क r, पूर्णाङ्क g, पूर्णाङ्क b)
{
    प्रत्यागम gui_get_rgb_color_cmn(r, g, b);
}

    शून्य
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    शून्य
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    शून्य
gui_mch_set_sp_color(guicolor_T color)
{
}

    शून्य
gui_mch_invert_rectangle(पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क nr, पूर्णाङ्क nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    जइ (nc > 0)
	rect.lr.x -= 1;
    जइ (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    शून्य
gui_mch_clear_block(पूर्णाङ्क row1, पूर्णाङ्क col1, पूर्णाङ्क row2, पूर्णाङ्क col2)
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

    शून्य
gui_mch_clear_all(शून्य)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    जइ (is_ignore_draw == TRUE)
	प्रत्यागम;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    शून्य
gui_mch_delete_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_वामः);
    rect.ul.y = FILL_Y(row + अङ्कः_पङ्क्तयः);

    rect.lr.x = FILL_X(gui.scroll_region_दक्षिणः + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, रिक्त));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -अङ्कः_पङ्क्तयः * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - अङ्कः_पङ्क्तयः + 1,
	gui.scroll_region_वामः,
	gui.scroll_region_bot,
	gui.scroll_region_दक्षिणः);
}

    शून्य
gui_mch_निवेशय_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_वामः);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_दक्षिणः + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - अङ्कः_पङ्क्तयः + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, रिक्त));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = अङ्कः_पङ्क्तयः * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_वामः,
	    row + अङ्कः_पङ्क्तयः - 1, gui.scroll_region_दक्षिणः);
}

    शून्य
gui_mch_draw_string(पूर्णाङ्क row, पूर्णाङ्क col, char_u *s, पूर्णाङ्क len, पूर्णाङ्क flags)
{
    स्थिर वर्ण *utf8_buffer = रिक्त;
    स्थिर पूर्णाङ्क	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    जइ (is_ignore_draw == TRUE)
	प्रत्यागम;

    DRAW_START;

    जइ (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    जइ (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    जइ (charset_translate != रिक्त && enc_utf8 == 0)
    {
	पूर्णाङ्क src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	जइ (utf8_len < len)
	{
	    वर्ण *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    जइ (new_buffer == रिक्त)
		प्रत्यागम;
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

    जइ (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#जइ 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(रिक्त);
#अण्णहा
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    जइ (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    शून्य
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

    शून्य
gui_mch_draw_part_cursor(पूर्णाङ्क w, पूर्णाङ्क h, guicolor_T color)
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

    पूर्णाङ्क
gui_mch_is_blinking(शून्य)
{
    प्रत्यागम blink_state != BLINK_NONE;
}

    पूर्णाङ्क
gui_mch_is_blink_off(शून्य)
{
    प्रत्यागम blink_state == BLINK_OFF;
}

    शून्य
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    शून्य
gui_mch_start_blink(शून्य)
{
    // Only turn on the timer on if none of the times are zero
    जइ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    शून्य
gui_mch_stop_blink(पूर्णाङ्क may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    जइ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    शून्य
gui_mch_beep(शून्य)
{
    PtBeep();
}

    शून्य
gui_mch_flash(पूर्णाङ्क msec)
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

    शून्य
gui_mch_flush(शून्य)
{
    PgFlush();
}

    शून्य
gui_mch_set_text_area_pos(पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    पूर्णाङ्क
gui_mch_haskey(char_u *name)
{
    पूर्णाङ्क i;

    प्रति (i = 0; special_कुञ्चिकाः[i].कुञ्चिका_sym != 0; i++)
	जइ (name[0] == special_कुञ्चिकाः[i].vim_code0 &&
		 name[1] == special_कुञ्चिकाः[i].vim_code1)
	    प्रत्यागम OK;
    प्रत्यागम FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
# include "toolbar.phi"

स्थिर PhImage_t *gui_ph_toolbar_images[] = {
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

स्थिर PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    स्थिर PhImage_t external_icon;
    PhImage_t *temp_phi = रिक्त;

    temp_phi = PxLoadImage(iconfile, रिक्त);
    जइ (temp_phi != रिक्त)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, परिमाण(external_icon));
	मुक्त(temp_phi);

	temp_phi = &external_icon;
    }
    प्रत्यागम temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
स्थिर PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = रिक्त;

    जइ (menu->icon_builtin == FALSE)
    {
	जइ (menu->iconfile != रिक्त)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	जइ ((icon == रिक्त) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	जइ (icon != रिक्त)
	    प्रत्यागम icon;
    }

    जइ (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	प्रत्यागम gui_ph_toolbar_images[menu->iconidx];

    प्रत्यागम रिक्त;
}
#endif

#जइ defined(FEAT_MENU)
    शून्य
gui_mch_enable_menu(पूर्णाङ्क flag)
{
    जइ (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    अण्णहा
	PtUnrealizeWidget(gui.vimMenuBar);
}

    शून्य
gui_mch_set_menu_pos(पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    स्थिर शून्य
gui_ph_position_menu(PtWidget_t *widget, पूर्णाङ्क priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    जाव (traverse != रिक्त)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	जइ (menu != रिक्त &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_निवेशय(widget, traverse, 1);
	    प्रत्यागम;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    शून्य
gui_mch_add_menu(vimmenu_T *menu, पूर्णाङ्क index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_कुञ्चिका;
    char_u	mnemonic_str[MB_LEN_MAX];
    पूर्णाङ्क	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = रिक्त;

    जइ (menu_is_menubar(menu->name))
    {

	accel_कुञ्चिका = vim_strchr(menu->name, '&');
	जइ (accel_कुञ्चिका != रिक्त)
	{
	    mnemonic_str[0] = accel_कुञ्चिका[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	जइ (accel_कुञ्चिका != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_कुञ्चिका, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	जइ (parent != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_दक्षिणः, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == रिक्त) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	जइ (parent != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	जइ (parent == रिक्त)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    जइ (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	जइ (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    अण्णहा जइ (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, रिक्त);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    शून्य
gui_mch_add_menu_item(vimmenu_T *menu, पूर्णाङ्क index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_कुञ्चिका;
    char_u	mnemonic_str[MB_LEN_MAX];
    पूर्णाङ्क	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    जइ (menu_is_toolbar(parent->name))
    {
	जइ (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_शिखरम्_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	अण्णहा
	{
	    जइ (strstr((ध्रुव वर्ण *) p_toolbar, "text") != रिक्त)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    जइ ((strstr((ध्रुव वर्ण *) p_toolbar, "icons") != रिक्त) &&
		    (gui_ph_toolbar_images != रिक्त))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    जइ (strstr((ध्रुव वर्ण *) p_toolbar, "tooltips") != रिक्त)
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
	जइ (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    अण्णहा
# endif
	जइ (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    अण्णहा
    {
	accel_कुञ्चिका = vim_strchr(menu->name, '&');
	जइ (accel_कुञ्चिका != रिक्त)
	{
	    mnemonic_str[0] = accel_कुञ्चिका[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	जइ (accel_कुञ्चिका != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_कुञ्चिका, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	जइ (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    शून्य
gui_mch_destroy_menu(vimmenu_T *menu)
{
    जइ (menu->submenu_id != रिक्त)
	PtDestroyWidget(menu->submenu_id);
    जइ (menu->id != रिक्त)
	PtDestroyWidget(menu->id);

    menu->submenu_id = रिक्त;
    menu->id = रिक्त;
}

    शून्य
gui_mch_menu_grey(vimmenu_T *menu, पूर्णाङ्क grey)
{
    long    flags, mask, fields;

    जइ (menu->id == रिक्त)
	प्रत्यागम;

    flags = PtWidgetFlags(menu->id);
    जइ (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    अण्णहा
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    जइ (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    शून्य
gui_mch_menu_hidden(vimmenu_T *menu, पूर्णाङ्क hidden)
{
    // TODO: [un]realize the widget?
}

    शून्य
gui_mch_draw_menubar(शून्य)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    शून्य
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    शून्य
gui_mch_toggle_tearoffs(पूर्णाङ्क enable)
{
    // No tearoffs yet
}

#endif

#जइ defined(FEAT_TOOLBAR)
    शून्य
gui_mch_show_toolbar(पूर्णाङ्क showit)
{
    जइ (showit)
	PtRealizeWidget(gui.vimToolBar);
    अण्णहा
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    स्थिर GuiFont
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
    जइ (font_tag != रिक्त)
    {
	जइ (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != रिक्त)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    जइ (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    जइ (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    जइ (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    जइ (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    जइ (font_size == info.size &&
		    style == (info.style & style))
		प्रत्यागम (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    प्रत्यागम रिक्त;
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

    स्थिर पूर्णाङ्क
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    जइ (mark == रिक्त)
	name_len = STRLEN(vim_font);
    अण्णहा
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    जइ (*font_name != रिक्त)
    {
	जइ (mark != रिक्त)
	{
	    जाव (*mark != NUL && *mark++ == ':')
	    {
		विकल्प (SAFE_tolower(*mark++))
		{
		    पक्ष 'a': *font_flags |= PF_STYLE_ANTIALIAS; भङ्ग;
		    पक्ष 'b': *font_flags |= PF_STYLE_BOLD; भङ्ग;
		    पक्ष 'i': *font_flags |= PF_STYLE_ITALIC; भङ्ग;

		    पक्ष 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			जइ (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			भङ्ग;

		    स्वभाव:
			भङ्ग;
		}
	    }
	}
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

    पूर्णाङ्क
gui_mch_init_font(char_u *vim_font_name, पूर्णाङ्क fontset)
{
    char_u  *font_tag;
    char_u  *font_name = रिक्त;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    जइ (vim_font_name == रिक्त)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    जइ (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, रिक्त, रिक्त,
		"pcterm12", -1, PHFONT_FIXED, रिक्त);

	जइ (font_tag == रिक्त)
	    प्रत्यागम FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    अण्णहा
    {
	जइ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    प्रत्यागम FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	जइ (font_tag == रिक्त)
	{
	    vim_free(font_name);
	    प्रत्यागम FAIL;
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
    PfExtentText(&extent, रिक्त, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    प्रत्यागम OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    पूर्णाङ्क
gui_mch_adjust_charheight(शून्य)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    प्रत्यागम OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, पूर्णाङ्क report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    जइ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	जइ (font_tag != रिक्त)
	    प्रत्यागम (GuiFont)font_tag;
    }

    जइ (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    प्रत्यागम FAIL;
}

#जइ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    जइ (name == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम vim_strsave(name);
}
#endif

    शून्य
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    शून्य
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

