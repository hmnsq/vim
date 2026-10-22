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

#ضمن "vim.h"

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#إذا !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	صحيح
# define PhEvent_t	صحيح
# define PhPoint_t	صحيح
# define PtWidget_t	صحيح
# define Pg_BLACK	0
# define PtCallbackF_t	صحيح
# define PtCallbackInfo_t صحيح
# define PhTile_t	صحيح
# define PtWidget_t	صحيح
# define PhImage_t	صحيح
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE حجم(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
ساكن حرف	*empty_title = "    ";
ساكن حرف	**panel_titles = عدم;
ساكن ushort_t	اڠک_panels = 0;
ساكن short pg_margin_کيري, pg_margin_کانن, pg_margin_اتس, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_سيسيڤ
ساكن PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ساكن PhPoint_t    gui_ph_raw_offset;
ساكن PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ساكن PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ساكن short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ساكن PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
ساكن صحيح is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
ساكن بنية PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
ساكن تعداد {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

ساكن long_u	blink_waittime	= 700;
ساكن long_u	blink_ontime	= 400;
ساكن long_u	blink_offtime	= 250;

ساكن بنية
{
    صحيح	    کونچي_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کونچي_سموا[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_کيري,	    'k', 'l'},
    {Pk_کانن,	    'k', 'r'},

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
    {Pk_سيسيڤ,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //سيسيڤ
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

ساكن PtCallbackF_t gui_ph_handle_timer_cursor;
ساكن PtCallbackF_t gui_ph_handle_timer_timeout;

ساكن PtCallbackF_t gui_ph_handle_window_cb;

ساكن PtCallbackF_t gui_ph_handle_scrollbar;
ساكن PtCallbackF_t gui_ph_handle_keyboard;
ساكن PtCallbackF_t gui_ph_handle_mouse;
ساكن PtCallbackF_t gui_ph_handle_pulldown_menu;
ساكن PtCallbackF_t gui_ph_handle_menu;
ساكن PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ساكن PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ساكن PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
ساكن فراغ gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

ساكن فراغ gui_ph_draw_start(فراغ);
ساكن فراغ gui_ph_draw_end(فراغ);

// Set the text for the balloon
ساكن PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     صحيح position,
			     حرف *text,
			     حرف *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

ساكن PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     صحيح position,
			     حرف *text,
			     حرف *font,
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
    إذا (menu != عدم)
    {
	صحيح index = MENU_INDEX_TIP;
	إذا (menu->strings[ index ] != عدم)
	    tooltip = menu->strings[ index ];
    }

    أرجع PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_اتس,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    ساكن فراغ
gui_ph_resize_container(فراغ)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ساكن صحيح
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	فراغ *other,
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
    إذا (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    وإلا
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#وإلا
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_اتس, &pg_margin_bottom,
	    &pg_margin_کيري, &pg_margin_کانن);
#endif
    أرجع Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ساكن صحيح
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	فراغ *data,
	PtCallbackInfo_t *info)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    فراغ		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_window_cb(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    بدل (we->event_f)
    {
	حالة Ph_WM_CLOSE:
	    gui_shell_closed();
	    اكسر;

	حالة Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    إذا (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    وإلا
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    اكسر;

	حالة Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_کيري + pg_margin_کانن);
	    height -= (pg_margin_اتس + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    اكسر;

	افتراضي:
	    اكسر;
    }

    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_scrollbar(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    صحيح	    نيلاي, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    إذا (sb != عدم)
    {
	نيلاي = scroll->position;
	بدل (scroll->action)
	{
	    حالة Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		اكسر;

	    حالة Pt_SCROLL_SET:
		// FIXME: return straight away here?
		أرجع Pt_CONTINUE;
		اكسر;
	}

	gui_drag_scrollbar(sb, نيلاي, dragging);
    }
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_keyboard(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    Ph_کونچي_Event_t    *کونچي;
    unsigned حرف   string[6];
    صحيح		    len, i;
    صحيح		    ch, modifiers;

    کونچي = PhGetData(info->event);

    ch = modifiers = len = 0;

    إذا (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    إذا (کونچي->کونچي_flags & Pk_KF_Compose)
	أرجع Pt_CONTINUE;

    إذا ((کونچي->کونچي_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_کونچي_Down(کونچي->کونچي_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	إذا (((کونچي->کونچي_mods & Pk_KM_Alt) &&
		    !(کونچي->کونچي_mods & Pk_KM_Shift) &&
		    !(کونچي->کونچي_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(کونچي->کونچي_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    أرجع Pt_CONTINUE;
	}
#endif

	لكل (i = 0; special_کونچي_سموا[i].کونچي_sym != 0; i++)
	{
	    إذا (special_کونچي_سموا[i].کونچي_sym == کونچي->کونچي_cap)
	    {
		len = 0;
		إذا (special_کونچي_سموا[i].vim_code1 == NUL)
		    ch = special_کونچي_سموا[i].vim_code0;
		وإلا
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    إذا (کونچي->کونچي_cap >= Pk_KP_Enter && کونچي->کونچي_cap <= Pk_KP_9
			    && (کونچي->کونچي_mods & Pk_KM_اڠک_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = کونچي->کونچي_cap - 0xf080;
		    }
		    وإلا
			ch = TO_SPECIAL(special_کونچي_سموا[i].vim_code0,
				special_کونچي_سموا[i].vim_code1);
		}
		اكسر;
	    }
	}

	إذا (کونچي->کونچي_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	إذا (کونچي->کونچي_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	إذا (کونچي->کونچي_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	إذا (special_کونچي_سموا[i].کونچي_sym == 0)
	{
	    ch = PhTo8859_1(کونچي);
	    إذا (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_کونچي_To_Mb(string, کونچي);
		إذا (len > 0)
		{
		    ساكن حرف buf[6];
		    صحيح src_taken, dst_made;
		    إذا (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_ماسوقن_buf(buf, dst_made);
		    }
		    وإلا
		    {
			add_to_ماسوقن_buf(string, len);
		    }

		    أرجع Pt_CONSUME;
		}
		len = 0;
		ch = کونچي->کونچي_cap;
		إذا (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    إذا (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			إذا ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			وإلا إذا (ch == '2')
			    ch = NUL;
			وإلا إذا (ch == '6')
			    ch = 0x1e;
			وإلا إذا (ch == '-')
			    ch = 0x1f;
			وإلا
			    modifiers |= MOD_MASK_CTRL;
		    }

		    إذا (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		وإلا
		{
		    أرجع Pt_CONTINUE;
		}
	    }
	    وإلا
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_کونچي(ch, &modifiers);
	إذا (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	إذا (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	وإلا
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    صحيح int_ch = check_for_interrupt(ch, modifiers);

	    إذا (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_ماسوقن_buf();
	    }
	}

	إذا (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	إذا (len > 0)
	{
	    add_to_ماسوقن_buf(string, len);
	    أرجع Pt_CONSUME;
	}
    }

    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_mouse(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    صحيح		     button = 0, repeated_click, modifiers = 0x0;
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

    إذا (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	أرجع Pt_CONTINUE;
    }

    إذا (pointer->کونچي_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    إذا (pointer->کونچي_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    إذا (pointer->کونچي_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    إذا (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_کيري;

    إذا (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_کانن;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    إذا (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_تڠه;

    // Catch a real release (not phantom or other releases
    إذا (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    إذا (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#إذا 0
    // Vim doesn't use button repeats
    إذا (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    إذا ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_کيراءن >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    أرجع Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ساكن صحيح
gui_ph_handle_focus(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    إذا (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, عدم);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    وإلا
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, عدم);
    }
    أرجع Pt_CONTINUE;
}

    ساكن فراغ
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    إذا (is_ignore_draw == TRUE)
	أرجع;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#إذا 0
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
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, عدم));

#إذا 1
    // Redraw individual damage regions
    إذا (damage->next != عدم)
	damage = damage->next;

    طالما (damage != عدم)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#وإلا
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    ساكن صحيح
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	فراغ *data,
	PtCallbackInfo_t *info)
{
    إذا (data != عدم)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, عدم);
	PtRealizeWidget(menu->submenu_id);
    }

    أرجع Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ساكن صحيح
gui_ph_handle_menu(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    إذا (data != عدم)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    أرجع Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    ساكن صحيح
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	فراغ *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, عدم);
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_window_open(
	PtWidget_t *widget,
	فراغ *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    أرجع Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    ساكن فراغ
gui_ph_draw_start(فراغ)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, عدم));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    ساكن فراغ
gui_ph_draw_end(فراغ)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    ساكن vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *اتس_level = اکر_menu;
    vimmenu_T *بارڠن = عدم;

    طالما (اتس_level != عدم &&
	    (STRCMP(اتس_level->dname, "Buffers") != 0))
	اتس_level = اتس_level->next;

    إذا (اتس_level != عدم)
    {
	بارڠن = اتس_level->children;

	طالما (بارڠن != عدم &&
		(STRCMP(بارڠن->dname, name) != 0))
	    بارڠن = بارڠن->next;
    }
    أرجع بارڠن;
}

    ساكن فراغ
gui_ph_pg_set_buffer_اڠک(int_u buf_اڠک)
{
    صحيح i;
    حرف چارين[16];
    حرف *mark;

    إذا (gui.vimTextArea == عدم || buf_اڠک == 0)
	أرجع;

    چارين[0] = '(';
    ultoa(buf_اڠک, &چارين[1], 10);
    STRCAT(چارين, ")");

    لكل (i = 0; i < اڠک_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	إذا (mark != عدم && STRCMP(mark, چارين) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    ساكن صحيح
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	فراغ *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    إذا (info->event != عدم)
    {
	panel = info->cbdata;
	إذا (panel->new_panel != عدم)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    إذا (menu)
		gui_menu_cb(menu);
	}
    }
    أرجع Pt_CONTINUE;
}

    ساكن فراغ
gui_ph_get_panelgroup_margins(
	short *اتس,
	short *bottom,
	short *کيري,
	short *کانن)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ثابت unsigned short *margin_اتس, *margin_bottom;
    ثابت unsigned short *margin_کيري, *margin_کانن;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_کانن, &margin_کانن, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *اتس    = abs_raw_y;
    *bottom = *margin_bottom;

    *کيري  = abs_raw_x;
    *کانن = *margin_کانن;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ساكن صحيح
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    حرف *mark;

    إذا (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	إذا (mark != عدم)
	{
	    mark++;
	    طالما (SAFE_isdigit(*mark))
		mark++;

	    إذا (*mark == ')')
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

    ساكن فراغ
gui_ph_pg_add_buffer(حرف *name)
{
    حرف **new_titles = عدم;

    new_titles = ALLOC_MULT(حرف *, (اڠک_panels + 1));
    إذا (new_titles == عدم)
	أرجع;

    إذا (اڠک_panels > 0)
	memcpy(new_titles, panel_titles, اڠک_panels * حجم(حرف **));

    new_titles[ اڠک_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    اڠک_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ساكن فراغ
gui_ph_pg_remove_buffer(حرف *name)
{
    صحيح i;
    حرف **new_titles = عدم;

    // If there is only 1 panel, we just use the temporary place holder
    إذا (اڠک_panels > 1)
    {
	new_titles = ALLOC_MULT(حرف *, اڠک_panels - 1);
	إذا (new_titles != عدم)
	{
	    حرف **s = new_titles;
	    // Copy all the titles except the one we're removing
	    لكل (i = 0; i < اڠک_panels; i++)
	    {
		إذا (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    اڠک_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    اڠک_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    وإلا
    {
	اڠک_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ساكن صحيح
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	فراغ *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    إذا (data != عدم)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    أرجع Pt_CONTINUE;
}
#endif

    ساكن صحيح
gui_ph_pane_resize(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    إذا (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    أرجع Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    فراغ
gui_ph_encoding_changed(صحيح new_encoding)
{
    // Default encoding is latin1
    حرف *charset = "latin1";
    صحيح i;

    بنية {
	صحيح encoding;
	حرف *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    لكل (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	إذا (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    فراغ
gui_mch_prepare(صحيح *argc, حرف **argv)
{
    PtInit(عدم);
}

    صحيح
gui_mch_init(فراغ)
{
    PtArg_t args[10];
    صحيح	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    إذا (gui.event_buffer == عدم)
	أرجع FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(عدم, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, عدم, n, args);
    إذا (gui.vimWindow == عدم)
	أرجع FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, عدم);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, عدم);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    إذا (gui.vimPanelGroup == عدم)
	أرجع FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, عدم);
#وإلا
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    إذا (gui.vimContainer == عدم)
	أرجع FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, عدم);
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
    إذا (gui.vimTextArea == عدم)
	أرجع FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, عدم);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_کونچي,
	    gui_ph_handle_keyboard, عدم);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, عدم);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, عدم);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, عدم);
    إذا (gui_ph_timer_cursor == عدم)
	أرجع FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, عدم);
    إذا (gui_ph_timer_timeout == عدم)
	أرجع FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, عدم);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, عدم);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_کيري_کانن,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    إذا (gui.vimToolBarGroup == عدم)
	أرجع FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, عدم);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    إذا (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    إذا (gui.vimMenuBar == عدم)
	أرجع FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_کيري_کانن |Pt_اتس_ANCHORED_اتس, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    إذا (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    إذا (gui.vimToolBar == عدم)
	أرجع FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    أرجع OK;
}

    صحيح
gui_mch_init_check(فراغ)
{
    أرجع (is_photon_available == TRUE) ? OK : FAIL;
}

    صحيح
gui_mch_open(فراغ)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    أرجع (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    فراغ
gui_mch_exit(صحيح rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, عدم);

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
    ساكن صحيح
exit_gui_mch_update(فراغ *data)
{
    *(صحيح *)data = FALSE;
    أرجع Pt_END;
}

    فراغ
gui_mch_update(فراغ)
{
    صحيح working = TRUE;

    PtAppAddWorkProc(عدم, exit_gui_mch_update, &working);
    طالما ((working == TRUE) && !vim_is_ماسوقن_buf_full())
	PtProcessEvent();
}

    صحيح
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    إذا (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    طالما (1)
    {
	PtProcessEvent();
	إذا (ماسوقن_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    أرجع OK;
	}
	وإلا إذا (is_timeout == TRUE)
	    أرجع FAIL;
    }
}

#إذا defined(FEAT_BROWSE)
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
	صحيح saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    صحيح	    flags;
    char_u  *default_path;
    char_u  *open_text = عدم;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    إذا (default_path != عدم)
    {
	إذا (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	إذا (initdir == عدم || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	وإلا
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	إذا (default_name != عدم)
	{
	    إذا (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		عدم,
		title,
		default_path,
		عدم,
		open_text,
		عدم,
		عدم,
		&file,
		flags);

	vim_free(default_path);

	إذا (file.ret == Pt_FSDIALOG_BTN1)
	    أرجع vim_strsave(file.path);
    }
    أرجع عدم;
}
#endif

#إذا defined(FEAT_GUI_DIALOG)
ساكن PtWidget_t *gui_ph_dialog_text = عدم;

    ساكن صحيح
gui_ph_dialog_close(صحيح button, فراغ *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    إذا (gui_ph_dialog_text != عدم)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (فراغ *) button);

    أرجع Pt_TRUE;
}

    ساكن صحيح
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    فراغ		*data,
    PtCallbackInfo_t	*info)
{
    إذا (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_dialog_esc(PtWidget_t *widget, فراغ *data, PtCallbackInfo_t *info)
{
    Ph_کونچي_Event_t *کونچي;

    کونچي = PhGetData(info->event);
    إذا ((کونچي->کونچي_flags & Pk_KF_Cap_Valid) && (کونچي->کونچي_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	أرجع Pt_CONSUME;
    }
    أرجع Pt_PROCESS;
}

    صحيح
gui_mch_dialog(
	صحيح	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	صحيح	default_button,
	char_u	*textfield,
	صحيح	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    صحيح		button_کيراءن;
    صحيح		i, len;
    صحيح		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_کيراءن = len = i = 0;

    إذا (buttons == عدم || *buttons == NUL)
	أرجع -1;

    // There is one less separator than buttons, so bump up the button count
    button_کيراءن = 1;

    // Count string length and number of separators
    لكل (str = buttons; *str; str++)
    {
	len++;
	إذا (*str == DLG_BUTTON_SEP)
	    button_کيراءن++;
    }

    إذا (title == عدم)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_کيراءن);
    إذا (buttons_copy != عدم && button_array != عدم)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	لكل (i = 0; i < button_کيراءن; i++)
	{
	    button_array[ i ] = str;
	    لكل (; *str; str++)
	    {
		إذا (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    اكسر;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, عدم,
		title,
		عدم,
		message, عدم,
		button_کيراءن, (ثابت حرف **) button_array, عدم,
		default_button, 0, Pt_MODAL);
# وإلا
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    صحيح n;
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
		    Pt_TRUE, Pt_GROUP_NO_کونچي_سموا | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, عدم, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    إذا (textfield != عدم)
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
	    di.buttons = (ثابت حرف **) button_array;
	    di.nbtns = button_کيراءن;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_کيراءن;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_کونچي,
		    gui_ph_dialog_esc, &modal_ctrl);

	    إذا (gui_ph_dialog_text != عدم)
		PtGiveFocus(gui_ph_dialog_text, عدم);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (صحيح) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = عدم;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    أرجع dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, عدم);

    *x = pos->x;
    *y = pos->y;

    أرجع OK;
}

    فراغ
gui_mch_set_winpos(صحيح x, صحيح y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    فراغ
gui_mch_set_shellsize(صحيح width, صحيح height,
	صحيح min_width, صحيح min_height, صحيح base_width, صحيح base_height,
	صحيح direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_کيري + pg_margin_کانن;
    window_size.h += pg_margin_اتس + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    إذا (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    فراغ
gui_mch_get_screen_dimensions(صحيح *screen_w, صحيح *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_ماسوقن_Group(عدم), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    فراغ
gui_mch_iconify(فراغ)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_set_foreground(فراغ)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    فراغ
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_اڠک(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    فراغ
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, صحيح val, صحيح size, صحيح max)
{
    صحيح	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_نيلاي, val, 0);
    PtSetResources(sb->id, n, args);
}

    فراغ
gui_mch_set_scrollbar_pos(scrollbar_T *sb, صحيح x, صحيح y, صحيح w, صحيح h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    أرجع 0;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    أرجع 0;
}

    فراغ
gui_mch_create_scrollbar(scrollbar_T *sb, صحيح orient)
{
    صحيح	    n = 0;
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
#إذا 0
    // Don't need this anchoring for the scrollbars
    إذا (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_کيري_ANCHORED_کيري | Pt_کانن_ANCHORED_کانن;
    }
    وإلا
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_اتس_ANCHORED_اتس;
	إذا (sb->wp != عدم)
	{
	    إذا (sb == &sb->wp->w_scrollbars[ SBAR_کيري ])
		anchor_flags |= Pt_کيري_ANCHORED_کيري;
	    وإلا
		anchor_flags |= Pt_کانن_ANCHORED_کانن;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#وإلا
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    إذا (flag != 0)
	PtRealizeWidget(sb->id);
    وإلا
	PtUnrealizeWidget(sb->id);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = عدم;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#إذا defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساكن صحيح last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساكن صحيح mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_سيسيڤ,		// beam
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

    فراغ
mch_set_mouse_shape(صحيح shape)
{
    صحيح	    id;

    إذا (!gui.in_use)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	وإلا
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    إذا (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.pointer_hidden == hide)
	أرجع;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    إذا (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    وإلا
	mch_set_mouse_shape(last_shape);
#وإلا
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_ماسوقن_Group(عدم), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_ماسوقن_Group(عدم), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    أرجع (guicolor_T)(PgRGB(Pg_Red_نيلاي(pixel),
				     Pg_Green_نيلاي(pixel), Pg_Blue_نيلاي(pixel)));
}

    فراغ
gui_mch_new_colors(فراغ)
{
#إذا 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    إذا (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_نيلاي(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_نيلاي(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_نيلاي(gui_ph_mouse_color) ^ 255;

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
    أرجع gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
    أرجع gui_get_rgb_color_cmn(r, g, b);
}

    فراغ
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    فراغ
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    فراغ
gui_mch_set_sp_color(guicolor_T color)
{
}

    فراغ
gui_mch_invert_rectangle(صحيح row, صحيح col, صحيح nr, صحيح nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    إذا (nc > 0)
	rect.lr.x -= 1;
    إذا (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    فراغ
gui_mch_clear_block(صحيح row1, صحيح col1, صحيح row2, صحيح col2)
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

    فراغ
gui_mch_clear_all(فراغ)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    إذا (is_ignore_draw == TRUE)
	أرجع;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    فراغ
gui_mch_delete_باريس_سموا(صحيح row, صحيح اڠک_باريس_سموا)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_کيري);
    rect.ul.y = FILL_Y(row + اڠک_باريس_سموا);

    rect.lr.x = FILL_X(gui.scroll_region_کانن + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, عدم));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -اڠک_باريس_سموا * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - اڠک_باريس_سموا + 1,
	gui.scroll_region_کيري,
	gui.scroll_region_bot,
	gui.scroll_region_کانن);
}

    فراغ
gui_mch_سيسيڤ_باريس_سموا(صحيح row, صحيح اڠک_باريس_سموا)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_کيري);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_کانن + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - اڠک_باريس_سموا + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, عدم));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = اڠک_باريس_سموا * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_کيري,
	    row + اڠک_باريس_سموا - 1, gui.scroll_region_کانن);
}

    فراغ
gui_mch_draw_string(صحيح row, صحيح col, char_u *s, صحيح len, صحيح flags)
{
    ساكن حرف *utf8_buffer = عدم;
    ساكن صحيح	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    إذا (is_ignore_draw == TRUE)
	أرجع;

    DRAW_START;

    إذا (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    إذا (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    إذا (charset_translate != عدم && enc_utf8 == 0)
    {
	صحيح src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	إذا (utf8_len < len)
	{
	    حرف *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    إذا (new_buffer == عدم)
		أرجع;
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

    إذا (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#إذا 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(عدم);
#وإلا
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    إذا (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    فراغ
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

    فراغ
gui_mch_draw_part_cursor(صحيح w, صحيح h, guicolor_T color)
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

    صحيح
gui_mch_is_blinking(فراغ)
{
    أرجع blink_state != BLINK_NONE;
}

    صحيح
gui_mch_is_blink_off(فراغ)
{
    أرجع blink_state == BLINK_OFF;
}

    فراغ
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    فراغ
gui_mch_start_blink(فراغ)
{
    // Only turn on the timer on if none of the times are zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    إذا (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    فراغ
gui_mch_beep(فراغ)
{
    PtBeep();
}

    فراغ
gui_mch_flash(صحيح msec)
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

    فراغ
gui_mch_flush(فراغ)
{
    PgFlush();
}

    فراغ
gui_mch_set_text_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کونچي_سموا[i].کونچي_sym != 0; i++)
	إذا (name[0] == special_کونچي_سموا[i].vim_code0 &&
		 name[1] == special_کونچي_سموا[i].vim_code1)
	    أرجع OK;
    أرجع FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
# include "toolbar.phi"

ساكن PhImage_t *gui_ph_toolbar_images[] = {
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

ساكن PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    ساكن PhImage_t external_icon;
    PhImage_t *temp_phi = عدم;

    temp_phi = PxLoadImage(iconfile, عدم);
    إذا (temp_phi != عدم)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, حجم(external_icon));
	حرر(temp_phi);

	temp_phi = &external_icon;
    }
    أرجع temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ساكن PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = عدم;

    إذا (menu->icon_builtin == FALSE)
    {
	إذا (menu->iconfile != عدم)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	إذا ((icon == عدم) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	إذا (icon != عدم)
	    أرجع icon;
    }

    إذا (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	أرجع gui_ph_toolbar_images[menu->iconidx];

    أرجع عدم;
}
#endif

#إذا defined(FEAT_MENU)
    فراغ
gui_mch_enable_menu(صحيح flag)
{
    إذا (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    وإلا
	PtUnrealizeWidget(gui.vimMenuBar);
}

    فراغ
gui_mch_set_menu_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ساكن فراغ
gui_ph_position_menu(PtWidget_t *widget, صحيح priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    طالما (traverse != عدم)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	إذا (menu != عدم &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_سيسيڤ(widget, traverse, 1);
	    أرجع;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    فراغ
gui_mch_add_menu(vimmenu_T *menu, صحيح index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_کونچي;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحيح	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = عدم;

    إذا (menu_is_menubar(menu->name))
    {

	accel_کونچي = vim_strchr(menu->name, '&');
	إذا (accel_کونچي != عدم)
	{
	    mnemonic_str[0] = accel_کونچي[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	إذا (accel_کونچي != عدم)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_کونچي, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	إذا (parent != عدم)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_کانن, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == عدم) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	إذا (parent != عدم)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	إذا (parent == عدم)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    إذا (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	إذا (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    وإلا إذا (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, عدم);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    فراغ
gui_mch_add_menu_item(vimmenu_T *menu, صحيح index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_کونچي;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحيح	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(parent->name))
    {
	إذا (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_اتس_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	وإلا
	{
	    إذا (strstr((ثابت حرف *) p_toolbar, "text") != عدم)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    إذا ((strstr((ثابت حرف *) p_toolbar, "icons") != عدم) &&
		    (gui_ph_toolbar_images != عدم))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    إذا (strstr((ثابت حرف *) p_toolbar, "tooltips") != عدم)
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
	إذا (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    وإلا
# endif
	إذا (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    وإلا
    {
	accel_کونچي = vim_strchr(menu->name, '&');
	إذا (accel_کونچي != عدم)
	{
	    mnemonic_str[0] = accel_کونچي[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	إذا (accel_کونچي != عدم)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_کونچي, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	إذا (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    فراغ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    إذا (menu->submenu_id != عدم)
	PtDestroyWidget(menu->submenu_id);
    إذا (menu->id != عدم)
	PtDestroyWidget(menu->id);

    menu->submenu_id = عدم;
    menu->id = عدم;
}

    فراغ
gui_mch_menu_grey(vimmenu_T *menu, صحيح grey)
{
    long    flags, mask, fields;

    إذا (menu->id == عدم)
	أرجع;

    flags = PtWidgetFlags(menu->id);
    إذا (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    وإلا
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    إذا (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    فراغ
gui_mch_menu_hidden(vimmenu_T *menu, صحيح hidden)
{
    // TODO: [un]realize the widget?
}

    فراغ
gui_mch_draw_menubar(فراغ)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    // No tearoffs yet
}

#endif

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    إذا (showit)
	PtRealizeWidget(gui.vimToolBar);
    وإلا
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    ساكن GuiFont
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
    إذا (font_tag != عدم)
    {
	إذا (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != عدم)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    إذا (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    إذا (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    إذا (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    إذا (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    إذا (font_size == info.size &&
		    style == (info.style & style))
		أرجع (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    أرجع عدم;
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

    ساكن صحيح
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    إذا (mark == عدم)
	name_len = STRLEN(vim_font);
    وإلا
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    إذا (*font_name != عدم)
    {
	إذا (mark != عدم)
	{
	    طالما (*mark != NUL && *mark++ == ':')
	    {
		بدل (SAFE_tolower(*mark++))
		{
		    حالة 'a': *font_flags |= PF_STYLE_ANTIALIAS; اكسر;
		    حالة 'b': *font_flags |= PF_STYLE_BOLD; اكسر;
		    حالة 'i': *font_flags |= PF_STYLE_ITALIC; اكسر;

		    حالة 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			إذا (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			اكسر;

		    افتراضي:
			اكسر;
		}
	    }
	}
	أرجع TRUE;
    }
    أرجع FALSE;
}

    صحيح
gui_mch_init_font(char_u *vim_font_name, صحيح fontset)
{
    char_u  *font_tag;
    char_u  *font_name = عدم;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    إذا (vim_font_name == عدم)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    إذا (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, عدم, عدم,
		"pcterm12", -1, PHFONT_FIXED, عدم);

	إذا (font_tag == عدم)
	    أرجع FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    وإلا
    {
	إذا (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    أرجع FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	إذا (font_tag == عدم)
	{
	    vim_free(font_name);
	    أرجع FAIL;
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
    PfExtentText(&extent, عدم, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    أرجع OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحيح
gui_mch_adjust_charheight(فراغ)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    أرجع OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, صحيح report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    إذا (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	إذا (font_tag != عدم)
	    أرجع (GuiFont)font_tag;
    }

    إذا (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    أرجع FAIL;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    إذا (name == عدم)
	أرجع عدم;
    أرجع vim_strsave(name);
}
#endif

    فراغ
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    فراغ
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

