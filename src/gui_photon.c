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

#जर !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	पूर्णांक
# define PhEvent_t	पूर्णांक
# define PhPoint_t	पूर्णांक
# define PtWidget_t	पूर्णांक
# define Pg_BLACK	0
# define PtCallbackF_t	पूर्णांक
# define PtCallbackInfo_t पूर्णांक
# define PhTile_t	पूर्णांक
# define PtWidget_t	पूर्णांक
# define PhImage_t	पूर्णांक
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE आकार(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
स्थिर अक्षर	*empty_title = "    ";
स्थिर अक्षर	**panel_titles = रिक्त;
स्थिर ushort_t	अंक_panels = 0;
स्थिर लघु pg_margin_डावा, pg_margin_उजवा, pg_margin_शीर्ष, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_घाला
स्थिर PgColor_t gui_ph_mouse_color =	Pg_BLACK;

स्थिर PhPoint_t    gui_ph_raw_offset;
स्थिर PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
स्थिर PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
स्थिर लघु	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
स्थिर PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
स्थिर पूर्णांक is_ignore_draw = FALSE;

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
    पूर्णांक	    किल्ली_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_किल्ल्या[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_डावा,	    'k', 'l'},
    {Pk_उजवा,	    'k', 'r'},

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
    {Pk_घाला,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //घाला
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
स्थिर शून्य gui_ph_get_panelgroup_margins(लघु*, लघु*, लघु*, लघु*);
#endif

स्थिर शून्य gui_ph_draw_start(शून्य);
स्थिर शून्य gui_ph_draw_end(शून्य);

// Set the text for the balloon
स्थिर PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     पूर्णांक position,
			     अक्षर *text,
			     अक्षर *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

स्थिर PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     पूर्णांक position,
			     अक्षर *text,
			     अक्षर *font,
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
    जर (menu != रिक्त)
    {
	पूर्णांक index = MENU_INDEX_TIP;
	जर (menu->strings[ index ] != रिक्त)
	    tooltip = menu->strings[ index ];
    }

    परत PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_शीर्ष,
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

    स्थिर पूर्णांक
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
    जर (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    अन्यथा
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#अन्यथा
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_शीर्ष, &pg_margin_bottom,
	    &pg_margin_डावा, &pg_margin_उजवा);
#endif
    परत Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    स्थिर पूर्णांक
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    जर (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    अन्यथा
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    परत Pt_CONTINUE;
}

    स्थिर पूर्णांक
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    शून्य		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    परत Pt_CONTINUE;
}

    स्थिर पूर्णांक
gui_ph_handle_window_cb(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    निवड (we->event_f)
    {
	स्थिती Ph_WM_CLOSE:
	    gui_shell_closed();
	    थांबा;

	स्थिती Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    जर (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    अन्यथा
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    थांबा;

	स्थिती Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_डावा + pg_margin_उजवा);
	    height -= (pg_margin_शीर्ष + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    थांबा;

	पूर्वनिर्धारित:
	    थांबा;
    }

    परत Pt_CONTINUE;
}

    स्थिर पूर्णांक
gui_ph_handle_scrollbar(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    पूर्णांक	    मूल्य, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    जर (sb != रिक्त)
    {
	मूल्य = scroll->position;
	निवड (scroll->action)
	{
	    स्थिती Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		थांबा;

	    स्थिती Pt_SCROLL_SET:
		// FIXME: return straight away here?
		परत Pt_CONTINUE;
		थांबा;
	}

	gui_drag_scrollbar(sb, मूल्य, dragging);
    }
    परत Pt_CONTINUE;
}

    स्थिर पूर्णांक
gui_ph_handle_keyboard(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    Ph_किल्ली_Event_t    *किल्ली;
    अचिन्हित अक्षर   string[6];
    पूर्णांक		    len, i;
    पूर्णांक		    ch, modifiers;

    किल्ली = PhGetData(info->event);

    ch = modifiers = len = 0;

    जर (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    जर (किल्ली->किल्ली_flags & Pk_KF_Compose)
	परत Pt_CONTINUE;

    जर ((किल्ली->किल्ली_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_किल्ली_Down(किल्ली->किल्ली_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	जर (((किल्ली->किल्ली_mods & Pk_KM_Alt) &&
		    !(किल्ली->किल्ली_mods & Pk_KM_Shift) &&
		    !(किल्ली->किल्ली_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(किल्ली->किल्ली_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    परत Pt_CONTINUE;
	}
#endif

	साठी (i = 0; special_किल्ल्या[i].किल्ली_sym != 0; i++)
	{
	    जर (special_किल्ल्या[i].किल्ली_sym == किल्ली->किल्ली_cap)
	    {
		len = 0;
		जर (special_किल्ल्या[i].vim_code1 == NUL)
		    ch = special_किल्ल्या[i].vim_code0;
		अन्यथा
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    जर (किल्ली->किल्ली_cap >= Pk_KP_Enter && किल्ली->किल्ली_cap <= Pk_KP_9
			    && (किल्ली->किल्ली_mods & Pk_KM_अंक_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = किल्ली->किल्ली_cap - 0xf080;
		    }
		    अन्यथा
			ch = TO_SPECIAL(special_किल्ल्या[i].vim_code0,
				special_किल्ल्या[i].vim_code1);
		}
		थांबा;
	    }
	}

	जर (किल्ली->किल्ली_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	जर (किल्ली->किल्ली_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	जर (किल्ली->किल्ली_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	जर (special_किल्ल्या[i].किल्ली_sym == 0)
	{
	    ch = PhTo8859_1(किल्ली);
	    जर (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_किल्ली_To_Mb(string, किल्ली);
		जर (len > 0)
		{
		    स्थिर अक्षर buf[6];
		    पूर्णांक src_taken, dst_made;
		    जर (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_आदान_buf(buf, dst_made);
		    }
		    अन्यथा
		    {
			add_to_आदान_buf(string, len);
		    }

		    परत Pt_CONSUME;
		}
		len = 0;
		ch = किल्ली->किल्ली_cap;
		जर (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    जर (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			जर ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			अन्यथा जर (ch == '2')
			    ch = NUL;
			अन्यथा जर (ch == '6')
			    ch = 0x1e;
			अन्यथा जर (ch == '-')
			    ch = 0x1f;
			अन्यथा
			    modifiers |= MOD_MASK_CTRL;
		    }

		    जर (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		अन्यथा
		{
		    परत Pt_CONTINUE;
		}
	    }
	    अन्यथा
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_किल्ली(ch, &modifiers);
	जर (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	जर (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	अन्यथा
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    पूर्णांक int_ch = check_for_interrupt(ch, modifiers);

	    जर (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_आदान_buf();
	    }
	}

	जर (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	जर (len > 0)
	{
	    add_to_आदान_buf(string, len);
	    परत Pt_CONSUME;
	}
    }

    परत Pt_CONTINUE;
}

    स्थिर पूर्णांक
gui_ph_handle_mouse(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    पूर्णांक		     button = 0, repeated_click, modifiers = 0x0;
    लघु	     mouse_x, mouse_y;

    pointer = PhGetData(info->event);
    pos = PhGetRects(info->event);

    gui_mch_mousehide(MOUSE_SHOW);

    /*
     * Coordinates need to be relative to the base window,
     * not relative to the vimTextArea widget
     */
    mouse_x = pos->ul.x + gui.border_width;
    mouse_y = pos->ul.y + gui.border_width;

    जर (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	परत Pt_CONTINUE;
    }

    जर (pointer->किल्ली_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    जर (pointer->किल्ली_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    जर (pointer->किल्ली_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    जर (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_डावा;

    जर (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_उजवा;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    जर (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_मधला;

    // Catch a real release (not phantom or other releases
    जर (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    जर (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#जर 0
    // Vim doesn't use button repeats
    जर (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    जर ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_मोजणी >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    परत Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    स्थिर पूर्णांक
gui_ph_handle_focus(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    जर (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, रिक्त);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    अन्यथा
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, रिक्त);
    }
    परत Pt_CONTINUE;
}

    स्थिर शून्य
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    जर (is_ignore_draw == TRUE)
	परत;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#जर 0
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

#जर 1
    // Redraw individual damage regions
    जर (damage->next != रिक्त)
	damage = damage->next;

    पर्यंत (damage != रिक्त)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#अन्यथा
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    स्थिर पूर्णांक
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    जर (data != रिक्त)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, रिक्त);
	PtRealizeWidget(menu->submenu_id);
    }

    परत Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    स्थिर पूर्णांक
gui_ph_handle_menu(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    जर (data != रिक्त)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    परत Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    स्थिर पूर्णांक
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, रिक्त);
    परत Pt_CONTINUE;
}

    स्थिर पूर्णांक
gui_ph_handle_window_open(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    परत Pt_CONTINUE;
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
    vimmenu_T *शीर्ष_level = मूळ_menu;
    vimmenu_T *वस्तुसंच = रिक्त;

    पर्यंत (शीर्ष_level != रिक्त &&
	    (STRCMP(शीर्ष_level->dname, "Buffers") != 0))
	शीर्ष_level = शीर्ष_level->next;

    जर (शीर्ष_level != रिक्त)
    {
	वस्तुसंच = शीर्ष_level->children;

	पर्यंत (वस्तुसंच != रिक्त &&
		(STRCMP(वस्तुसंच->dname, name) != 0))
	    वस्तुसंच = वस्तुसंच->next;
    }
    परत वस्तुसंच;
}

    स्थिर शून्य
gui_ph_pg_set_buffer_अंक(int_u buf_अंक)
{
    पूर्णांक i;
    अक्षर शोध[16];
    अक्षर *mark;

    जर (gui.vimTextArea == रिक्त || buf_अंक == 0)
	परत;

    शोध[0] = '(';
    ultoa(buf_अंक, &शोध[1], 10);
    STRCAT(शोध, ")");

    साठी (i = 0; i < अंक_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	जर (mark != रिक्त && STRCMP(mark, शोध) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    स्थिर पूर्णांक
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    जर (info->event != रिक्त)
    {
	panel = info->cbdata;
	जर (panel->new_panel != रिक्त)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    जर (menu)
		gui_menu_cb(menu);
	}
    }
    परत Pt_CONTINUE;
}

    स्थिर शून्य
gui_ph_get_panelgroup_margins(
	लघु *शीर्ष,
	लघु *bottom,
	लघु *डावा,
	लघु *उजवा)
{
    अचिन्हित लघु abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    स्थिरांक अचिन्हित लघु *margin_शीर्ष, *margin_bottom;
    स्थिरांक अचिन्हित लघु *margin_डावा, *margin_उजवा;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_उजवा, &margin_उजवा, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *शीर्ष    = abs_raw_y;
    *bottom = *margin_bottom;

    *डावा  = abs_raw_x;
    *उजवा = *margin_उजवा;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    स्थिर पूर्णांक
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    अक्षर *mark;

    जर (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	जर (mark != रिक्त)
	{
	    mark++;
	    पर्यंत (SAFE_isdigit(*mark))
		mark++;

	    जर (*mark == ')')
		परत TRUE;
	}
    }
    परत FALSE;
}

    स्थिर शून्य
gui_ph_pg_add_buffer(अक्षर *name)
{
    अक्षर **new_titles = रिक्त;

    new_titles = ALLOC_MULT(अक्षर *, (अंक_panels + 1));
    जर (new_titles == रिक्त)
	परत;

    जर (अंक_panels > 0)
	स_नकल(new_titles, panel_titles, अंक_panels * आकार(अक्षर **));

    new_titles[ अंक_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    अंक_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    स्थिर शून्य
gui_ph_pg_remove_buffer(अक्षर *name)
{
    पूर्णांक i;
    अक्षर **new_titles = रिक्त;

    // If there is only 1 panel, we just use the temporary place holder
    जर (अंक_panels > 1)
    {
	new_titles = ALLOC_MULT(अक्षर *, अंक_panels - 1);
	जर (new_titles != रिक्त)
	{
	    अक्षर **s = new_titles;
	    // Copy all the titles except the one we're removing
	    साठी (i = 0; i < अंक_panels; i++)
	    {
		जर (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    अंक_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    अंक_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    अन्यथा
    {
	अंक_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    स्थिर पूर्णांक
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	शून्य *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    जर (data != रिक्त)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    परत Pt_CONTINUE;
}
#endif

    स्थिर पूर्णांक
gui_ph_pane_resize(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    जर (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    परत Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    शून्य
gui_ph_encoding_changed(पूर्णांक new_encoding)
{
    // Default encoding is latin1
    अक्षर *charset = "latin1";
    पूर्णांक i;

    संरचना {
	पूर्णांक encoding;
	अक्षर *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    साठी (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	जर (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    शून्य
gui_mch_prepare(पूर्णांक *argc, अक्षर **argv)
{
    PtInit(रिक्त);
}

    पूर्णांक
gui_mch_init(शून्य)
{
    PtArg_t args[10];
    पूर्णांक	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    जर (gui.event_buffer == रिक्त)
	परत FAIL;

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
    जर (gui.vimWindow == रिक्त)
	परत FAIL;

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
    जर (gui.vimPanelGroup == रिक्त)
	परत FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, रिक्त);
#अन्यथा
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    जर (gui.vimContainer == रिक्त)
	परत FAIL;

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
    जर (gui.vimTextArea == रिक्त)
	परत FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, रिक्त);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_किल्ली,
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
    जर (gui_ph_timer_cursor == रिक्त)
	परत FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, रिक्त);
    जर (gui_ph_timer_timeout == रिक्त)
	परत FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, रिक्त);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, रिक्त);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_डावा_उजवा,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    जर (gui.vimToolBarGroup == रिक्त)
	परत FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, रिक्त);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    जर (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    जर (gui.vimMenuBar == रिक्त)
	परत FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_डावा_उजवा |Pt_शीर्ष_ANCHORED_शीर्ष, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    जर (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    जर (gui.vimToolBar == रिक्त)
	परत FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    परत OK;
}

    पूर्णांक
gui_mch_init_check(शून्य)
{
    परत (is_photon_available == TRUE) ? OK : FAIL;
}

    पूर्णांक
gui_mch_open(शून्य)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    जर (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    परत (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    शून्य
gui_mch_exit(पूर्णांक rc)
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
    स्थिर पूर्णांक
exit_gui_mch_update(शून्य *data)
{
    *(पूर्णांक *)data = FALSE;
    परत Pt_END;
}

    शून्य
gui_mch_update(शून्य)
{
    पूर्णांक working = TRUE;

    PtAppAddWorkProc(रिक्त, exit_gui_mch_update, &working);
    पर्यंत ((working == TRUE) && !vim_is_आदान_buf_full())
	PtProcessEvent();
}

    पूर्णांक
gui_mch_wait_for_chars(दीर्घ wtime)
{
    is_timeout = FALSE;

    जर (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    पर्यंत (1)
    {
	PtProcessEvent();
	जर (आदान_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    परत OK;
	}
	अन्यथा जर (is_timeout == TRUE)
	    परत FAIL;
    }
}

#जर defined(FEAT_BROWSE)
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
	पूर्णांक saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    पूर्णांक	    flags;
    char_u  *default_path;
    char_u  *open_text = रिक्त;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    जर (default_path != रिक्त)
    {
	जर (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	जर (initdir == रिक्त || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	अन्यथा
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	जर (default_name != रिक्त)
	{
	    जर (default_path[ STRLEN(default_path) - 1 ] != '/')
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

	जर (file.ret == Pt_FSDIALOG_BTN1)
	    परत vim_strsave(file.path);
    }
    परत रिक्त;
}
#endif

#जर defined(FEAT_GUI_DIALOG)
स्थिर PtWidget_t *gui_ph_dialog_text = रिक्त;

    स्थिर पूर्णांक
gui_ph_dialog_close(पूर्णांक button, शून्य *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    जर (gui_ph_dialog_text != रिक्त)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (शून्य *) button);

    परत Pt_TRUE;
}

    स्थिर पूर्णांक
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    शून्य		*data,
    PtCallbackInfo_t	*info)
{
    जर (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    परत Pt_CONTINUE;
}

    स्थिर पूर्णांक
gui_ph_dialog_esc(PtWidget_t *widget, शून्य *data, PtCallbackInfo_t *info)
{
    Ph_किल्ली_Event_t *किल्ली;

    किल्ली = PhGetData(info->event);
    जर ((किल्ली->किल्ली_flags & Pk_KF_Cap_Valid) && (किल्ली->किल्ली_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	परत Pt_CONSUME;
    }
    परत Pt_PROCESS;
}

    पूर्णांक
gui_mch_dialog(
	पूर्णांक	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	पूर्णांक	default_button,
	char_u	*textfield,
	पूर्णांक	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    पूर्णांक		button_मोजणी;
    पूर्णांक		i, len;
    पूर्णांक		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_मोजणी = len = i = 0;

    जर (buttons == रिक्त || *buttons == NUL)
	परत -1;

    // There is one less separator than buttons, so bump up the button count
    button_मोजणी = 1;

    // Count string length and number of separators
    साठी (str = buttons; *str; str++)
    {
	len++;
	जर (*str == DLG_BUTTON_SEP)
	    button_मोजणी++;
    }

    जर (title == रिक्त)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_मोजणी);
    जर (buttons_copy != रिक्त && button_array != रिक्त)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	साठी (i = 0; i < button_मोजणी; i++)
	{
	    button_array[ i ] = str;
	    साठी (; *str; str++)
	    {
		जर (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    थांबा;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, रिक्त,
		title,
		रिक्त,
		message, रिक्त,
		button_मोजणी, (स्थिरांक अक्षर **) button_array, रिक्त,
		default_button, 0, Pt_MODAL);
# अन्यथा
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    पूर्णांक n;
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
		    Pt_TRUE, Pt_GROUP_NO_किल्ल्या | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, रिक्त, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    जर (textfield != रिक्त)
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
	    di.buttons = (स्थिरांक अक्षर **) button_array;
	    di.nbtns = button_मोजणी;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_मोजणी;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_किल्ली,
		    gui_ph_dialog_esc, &modal_ctrl);

	    जर (gui_ph_dialog_text != रिक्त)
		PtGiveFocus(gui_ph_dialog_text, रिक्त);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (पूर्णांक) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = रिक्त;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    परत dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    पूर्णांक
gui_mch_get_winpos(पूर्णांक *x, पूर्णांक *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, रिक्त);

    *x = pos->x;
    *y = pos->y;

    परत OK;
}

    शून्य
gui_mch_set_winpos(पूर्णांक x, पूर्णांक y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    शून्य
gui_mch_set_shellsize(पूर्णांक width, पूर्णांक height,
	पूर्णांक min_width, पूर्णांक min_height, पूर्णांक base_width, पूर्णांक base_height,
	पूर्णांक direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_डावा + pg_margin_उजवा;
    window_size.h += pg_margin_शीर्ष + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    जर (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    शून्य
gui_mch_get_screen_dimensions(पूर्णांक *screen_w, पूर्णांक *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_आदान_Group(रिक्त), &console);

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

#जर defined(FEAT_EVAL)
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
    gui_ph_pg_set_buffer_अंक(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    शून्य
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, पूर्णांक val, पूर्णांक size, पूर्णांक max)
{
    पूर्णांक	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_मूल्य, val, 0);
    PtSetResources(sb->id, n, args);
}

    शून्य
gui_mch_set_scrollbar_pos(scrollbar_T *sb, पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    पूर्णांक
gui_mch_get_scrollbar_xpadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    परत 0;
}

    पूर्णांक
gui_mch_get_scrollbar_ypadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    परत 0;
}

    शून्य
gui_mch_create_scrollbar(scrollbar_T *sb, पूर्णांक orient)
{
    पूर्णांक	    n = 0;
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
#जर 0
    // Don't need this anchoring for the scrollbars
    जर (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_डावा_ANCHORED_डावा | Pt_उजवा_ANCHORED_उजवा;
    }
    अन्यथा
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_शीर्ष_ANCHORED_शीर्ष;
	जर (sb->wp != रिक्त)
	{
	    जर (sb == &sb->wp->w_scrollbars[ SBAR_डावा ])
		anchor_flags |= Pt_डावा_ANCHORED_डावा;
	    अन्यथा
		anchor_flags |= Pt_उजवा_ANCHORED_उजवा;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#अन्यथा
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    शून्य
gui_mch_enable_scrollbar(scrollbar_T *sb, पूर्णांक flag)
{
    जर (flag != 0)
	PtRealizeWidget(sb->id);
    अन्यथा
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

#जर defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
स्थिर पूर्णांक last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
स्थिर पूर्णांक mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_घाला,		// beam
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
mch_set_mouse_shape(पूर्णांक shape)
{
    पूर्णांक	    id;

    जर (!gui.in_use)
	परत;

    जर (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    अन्यथा
    {
	जर (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	अन्यथा
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    जर (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    शून्य
gui_mch_mousehide(पूर्णांक hide)
{
    जर (gui.pointer_hidden == hide)
	परत;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    जर (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    अन्यथा
	mch_set_mouse_shape(last_shape);
#अन्यथा
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    शून्य
gui_mch_getmouse(पूर्णांक *x, पूर्णांक *y)
{
    PhCursorInfo_t info;
    लघु ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_आदान_Group(रिक्त), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    शून्य
gui_mch_setmouse(पूर्णांक x, पूर्णांक y)
{
    लघु abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_आदान_Group(रिक्त), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    परत (guicolor_T)(PgRGB(Pg_Red_मूल्य(pixel),
				     Pg_Green_मूल्य(pixel), Pg_Blue_मूल्य(pixel)));
}

    शून्य
gui_mch_new_colors(शून्य)
{
#जर 0 // Don't bother changing the cursor colour
    लघु color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    जर (असल(color_diff) < 64)
    {
	लघु r, g, b;
	// not a great algorithm...
	r = Pg_Red_मूल्य(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_मूल्य(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_मूल्य(gui_ph_mouse_color) ^ 255;

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
    परत gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(पूर्णांक r, पूर्णांक g, पूर्णांक b)
{
    परत gui_get_rgb_color_cmn(r, g, b);
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
gui_mch_invert_rectangle(पूर्णांक row, पूर्णांक col, पूर्णांक nr, पूर्णांक nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    जर (nc > 0)
	rect.lr.x -= 1;
    जर (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    शून्य
gui_mch_clear_block(पूर्णांक row1, पूर्णांक col1, पूर्णांक row2, पूर्णांक col2)
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

    जर (is_ignore_draw == TRUE)
	परत;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    शून्य
gui_mch_delete_ओळी(पूर्णांक row, पूर्णांक अंक_ओळी)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_डावा);
    rect.ul.y = FILL_Y(row + अंक_ओळी);

    rect.lr.x = FILL_X(gui.scroll_region_उजवा + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, रिक्त));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -अंक_ओळी * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - अंक_ओळी + 1,
	gui.scroll_region_डावा,
	gui.scroll_region_bot,
	gui.scroll_region_उजवा);
}

    शून्य
gui_mch_घाला_ओळी(पूर्णांक row, पूर्णांक अंक_ओळी)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_डावा);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_उजवा + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - अंक_ओळी + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, रिक्त));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = अंक_ओळी * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_डावा,
	    row + अंक_ओळी - 1, gui.scroll_region_उजवा);
}

    शून्य
gui_mch_draw_string(पूर्णांक row, पूर्णांक col, char_u *s, पूर्णांक len, पूर्णांक flags)
{
    स्थिर अक्षर *utf8_buffer = रिक्त;
    स्थिर पूर्णांक	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    जर (is_ignore_draw == TRUE)
	परत;

    DRAW_START;

    जर (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    जर (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    जर (charset_translate != रिक्त && enc_utf8 == 0)
    {
	पूर्णांक src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	जर (utf8_len < len)
	{
	    अक्षर *new_buffer = पुनः_स्मृति(utf8_buffer, len * MB_LEN_MAX);

	    जर (new_buffer == रिक्त)
		परत;
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

    जर (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#जर 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(रिक्त);
#अन्यथा
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    जर (flags & DRAW_UNDERL)
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
gui_mch_draw_part_cursor(पूर्णांक w, पूर्णांक h, guicolor_T color)
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

    पूर्णांक
gui_mch_is_blinking(शून्य)
{
    परत blink_state != BLINK_NONE;
}

    पूर्णांक
gui_mch_is_blink_off(शून्य)
{
    परत blink_state == BLINK_OFF;
}

    शून्य
gui_mch_set_blinking(दीर्घ रुको, दीर्घ on, दीर्घ off)
{
    blink_waittime = रुको;
    blink_ontime = on;
    blink_offtime = off;
}

    शून्य
gui_mch_start_blink(शून्य)
{
    // Only turn on the timer on if none of the times are zero
    जर (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    शून्य
gui_mch_stop_blink(पूर्णांक may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    जर (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
gui_mch_flash(पूर्णांक msec)
{
    PgSetFillXORColor(Pg_BLACK, Pg_WHITE);
    PgSetDrawMode(Pg_DRAWMODE_XOR);
    gui_mch_clear_all();
    gui_mch_flush();

    ui_delay((दीर्घ) msec, TRUE);

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
gui_mch_set_text_area_pos(पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    पूर्णांक
gui_mch_haskey(char_u *name)
{
    पूर्णांक i;

    साठी (i = 0; special_किल्ल्या[i].किल्ली_sym != 0; i++)
	जर (name[0] == special_किल्ल्या[i].vim_code0 &&
		 name[1] == special_किल्ल्या[i].vim_code1)
	    परत OK;
    परत FAIL;
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
    जर (temp_phi != रिक्त)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	स_नकल(&external_icon, temp_phi, आकार(external_icon));
	मुक्त(temp_phi);

	temp_phi = &external_icon;
    }
    परत temp_phi;
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

    जर (menu->icon_builtin == FALSE)
    {
	जर (menu->iconfile != रिक्त)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	जर ((icon == रिक्त) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	जर (icon != रिक्त)
	    परत icon;
    }

    जर (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	परत gui_ph_toolbar_images[menu->iconidx];

    परत रिक्त;
}
#endif

#जर defined(FEAT_MENU)
    शून्य
gui_mch_enable_menu(पूर्णांक flag)
{
    जर (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    अन्यथा
	PtUnrealizeWidget(gui.vimMenuBar);
}

    शून्य
gui_mch_set_menu_pos(पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    स्थिर शून्य
gui_ph_position_menu(PtWidget_t *widget, पूर्णांक priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    पर्यंत (traverse != रिक्त)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	जर (menu != रिक्त &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_घाला(widget, traverse, 1);
	    परत;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    शून्य
gui_mch_add_menu(vimmenu_T *menu, पूर्णांक index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_किल्ली;
    char_u	mnemonic_str[MB_LEN_MAX];
    पूर्णांक	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = रिक्त;

    जर (menu_is_menubar(menu->name))
    {

	accel_किल्ली = vim_strchr(menu->name, '&');
	जर (accel_किल्ली != रिक्त)
	{
	    mnemonic_str[0] = accel_किल्ली[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	जर (accel_किल्ली != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_किल्ली, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	जर (parent != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_उजवा, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == रिक्त) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	जर (parent != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	जर (parent == रिक्त)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    जर (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	जर (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    अन्यथा जर (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, रिक्त);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    शून्य
gui_mch_add_menu_item(vimmenu_T *menu, पूर्णांक index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_किल्ली;
    char_u	mnemonic_str[MB_LEN_MAX];
    पूर्णांक	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    जर (menu_is_toolbar(parent->name))
    {
	जर (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_शीर्ष_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	अन्यथा
	{
	    जर (म_माला((स्थिरांक अक्षर *) p_toolbar, "text") != रिक्त)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    जर ((म_माला((स्थिरांक अक्षर *) p_toolbar, "icons") != रिक्त) &&
		    (gui_ph_toolbar_images != रिक्त))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    जर (म_माला((स्थिरांक अक्षर *) p_toolbar, "tooltips") != रिक्त)
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
	जर (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    अन्यथा
# endif
	जर (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    अन्यथा
    {
	accel_किल्ली = vim_strchr(menu->name, '&');
	जर (accel_किल्ली != रिक्त)
	{
	    mnemonic_str[0] = accel_किल्ली[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	जर (accel_किल्ली != रिक्त)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_किल्ली, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	जर (gui_ph_is_buffer_item(menu, parent) == TRUE)
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
    जर (menu->submenu_id != रिक्त)
	PtDestroyWidget(menu->submenu_id);
    जर (menu->id != रिक्त)
	PtDestroyWidget(menu->id);

    menu->submenu_id = रिक्त;
    menu->id = रिक्त;
}

    शून्य
gui_mch_menu_grey(vimmenu_T *menu, पूर्णांक grey)
{
    दीर्घ    flags, mask, fields;

    जर (menu->id == रिक्त)
	परत;

    flags = PtWidgetFlags(menu->id);
    जर (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    अन्यथा
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    जर (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    शून्य
gui_mch_menu_hidden(vimmenu_T *menu, पूर्णांक hidden)
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
gui_mch_toggle_tearoffs(पूर्णांक enable)
{
    // No tearoffs yet
}

#endif

#जर defined(FEAT_TOOLBAR)
    शून्य
gui_mch_show_toolbar(पूर्णांक showit)
{
    जर (showit)
	PtRealizeWidget(gui.vimToolBar);
    अन्यथा
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
    जर (font_tag != रिक्त)
    {
	जर (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != रिक्त)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    जर (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    जर (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    जर (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    जर (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    जर (font_size == info.size &&
		    style == (info.style & style))
		परत (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    परत रिक्त;
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

    स्थिर पूर्णांक
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    जर (mark == रिक्त)
	name_len = STRLEN(vim_font);
    अन्यथा
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    जर (*font_name != रिक्त)
    {
	जर (mark != रिक्त)
	{
	    पर्यंत (*mark != NUL && *mark++ == ':')
	    {
		निवड (SAFE_tolower(*mark++))
		{
		    स्थिती 'a': *font_flags |= PF_STYLE_ANTIALIAS; थांबा;
		    स्थिती 'b': *font_flags |= PF_STYLE_BOLD; थांबा;
		    स्थिती 'i': *font_flags |= PF_STYLE_ITALIC; थांबा;

		    स्थिती 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			जर (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			थांबा;

		    पूर्वनिर्धारित:
			थांबा;
		}
	    }
	}
	परत TRUE;
    }
    परत FALSE;
}

    पूर्णांक
gui_mch_init_font(char_u *vim_font_name, पूर्णांक fontset)
{
    char_u  *font_tag;
    char_u  *font_name = रिक्त;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    जर (vim_font_name == रिक्त)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    जर (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, रिक्त, रिक्त,
		"pcterm12", -1, PHFONT_FIXED, रिक्त);

	जर (font_tag == रिक्त)
	    परत FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    अन्यथा
    {
	जर (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    परत FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	जर (font_tag == रिक्त)
	{
	    vim_free(font_name);
	    परत FAIL;
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
    परत OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    पूर्णांक
gui_mch_adjust_charheight(शून्य)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    परत OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, पूर्णांक report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    जर (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	जर (font_tag != रिक्त)
	    परत (GuiFont)font_tag;
    }

    जर (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    परत FAIL;
}

#जर defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    जर (name == रिक्त)
	परत रिक्त;
    परत vim_strsave(name);
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

