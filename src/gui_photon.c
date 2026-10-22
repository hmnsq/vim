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

#شامل "vim.h"

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#اگر !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	صحیح
# define PhEvent_t	صحیح
# define PhPoint_t	صحیح
# define PtWidget_t	صحیح
# define Pg_BLACK	0
# define PtCallbackF_t	صحیح
# define PtCallbackInfo_t صحیح
# define PhTile_t	صحیح
# define PtWidget_t	صحیح
# define PhImage_t	صحیح
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE اندازه(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
ایستا نویسه	*empty_title = "    ";
ایستا نویسه	**panel_titles = پوچ;
ایستا ushort_t	رقم_panels = 0;
ایستا short pg_margin_چپ, pg_margin_راست, pg_margin_بالاترین, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_درج
ایستا PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ایستا PhPoint_t    gui_ph_raw_offset;
ایستا PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ایستا PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ایستا short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ایستا PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
ایستا صحیح is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
ایستا ساختار PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
ایستا شمارش {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

ایستا long_u	blink_waittime	= 700;
ایستا long_u	blink_ontime	= 400;
ایستا long_u	blink_offtime	= 250;

ایستا ساختار
{
    صحیح	    کلید_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدها[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_چپ,	    'k', 'l'},
    {Pk_راست,	    'k', 'r'},

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
    {Pk_درج,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //درج
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

ایستا PtCallbackF_t gui_ph_handle_timer_cursor;
ایستا PtCallbackF_t gui_ph_handle_timer_timeout;

ایستا PtCallbackF_t gui_ph_handle_window_cb;

ایستا PtCallbackF_t gui_ph_handle_scrollbar;
ایستا PtCallbackF_t gui_ph_handle_keyboard;
ایستا PtCallbackF_t gui_ph_handle_mouse;
ایستا PtCallbackF_t gui_ph_handle_pulldown_menu;
ایستا PtCallbackF_t gui_ph_handle_menu;
ایستا PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ایستا PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ایستا PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
ایستا تهی gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

ایستا تهی gui_ph_draw_start(تهی);
ایستا تهی gui_ph_draw_end(تهی);

// Set the text for the balloon
ایستا PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     صحیح position,
			     نویسه *text,
			     نویسه *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

ایستا PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     صحیح position,
			     نویسه *text,
			     نویسه *font,
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
    اگر (menu != پوچ)
    {
	صحیح index = MENU_INDEX_TIP;
	اگر (menu->strings[ index ] != پوچ)
	    tooltip = menu->strings[ index ];
    }

    بازگشت PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_بالاترین,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    ایستا تهی
gui_ph_resize_container(تهی)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ایستا صحیح
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	تهی *other,
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
    اگر (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    وگرنه
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#وگرنه
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_بالاترین, &pg_margin_bottom,
	    &pg_margin_چپ, &pg_margin_راست);
#endif
    بازگشت Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ایستا صحیح
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	تهی *data,
	PtCallbackInfo_t *info)
{
    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    وگرنه
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    بازگشت Pt_CONTINUE;
}

    ایستا صحیح
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    تهی		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    بازگشت Pt_CONTINUE;
}

    ایستا صحیح
gui_ph_handle_window_cb(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    انتخاب (we->event_f)
    {
	حالت Ph_WM_CLOSE:
	    gui_shell_closed();
	    بشکن;

	حالت Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    اگر (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    وگرنه
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    بشکن;

	حالت Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_چپ + pg_margin_راست);
	    height -= (pg_margin_بالاترین + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    بشکن;

	پیشفرض:
	    بشکن;
    }

    بازگشت Pt_CONTINUE;
}

    ایستا صحیح
gui_ph_handle_scrollbar(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    صحیح	    مقدار, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    اگر (sb != پوچ)
    {
	مقدار = scroll->position;
	انتخاب (scroll->action)
	{
	    حالت Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		بشکن;

	    حالت Pt_SCROLL_SET:
		// FIXME: return straight away here?
		بازگشت Pt_CONTINUE;
		بشکن;
	}

	gui_drag_scrollbar(sb, مقدار, dragging);
    }
    بازگشت Pt_CONTINUE;
}

    ایستا صحیح
gui_ph_handle_keyboard(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    Ph_کلید_Event_t    *کلید;
    unsigned نویسه   string[6];
    صحیح		    len, i;
    صحیح		    ch, modifiers;

    کلید = PhGetData(info->event);

    ch = modifiers = len = 0;

    اگر (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    اگر (کلید->کلید_flags & Pk_KF_Compose)
	بازگشت Pt_CONTINUE;

    اگر ((کلید->کلید_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_کلید_Down(کلید->کلید_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	اگر (((کلید->کلید_mods & Pk_KM_Alt) &&
		    !(کلید->کلید_mods & Pk_KM_Shift) &&
		    !(کلید->کلید_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(کلید->کلید_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    بازگشت Pt_CONTINUE;
	}
#endif

	برای (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	{
	    اگر (special_کلیدها[i].کلید_sym == کلید->کلید_cap)
	    {
		len = 0;
		اگر (special_کلیدها[i].vim_code1 == NUL)
		    ch = special_کلیدها[i].vim_code0;
		وگرنه
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    اگر (کلید->کلید_cap >= Pk_KP_Enter && کلید->کلید_cap <= Pk_KP_9
			    && (کلید->کلید_mods & Pk_KM_رقم_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = کلید->کلید_cap - 0xf080;
		    }
		    وگرنه
			ch = TO_SPECIAL(special_کلیدها[i].vim_code0,
				special_کلیدها[i].vim_code1);
		}
		بشکن;
	    }
	}

	اگر (کلید->کلید_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	اگر (کلید->کلید_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	اگر (کلید->کلید_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	اگر (special_کلیدها[i].کلید_sym == 0)
	{
	    ch = PhTo8859_1(کلید);
	    اگر (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_کلید_To_Mb(string, کلید);
		اگر (len > 0)
		{
		    ایستا نویسه buf[6];
		    صحیح src_taken, dst_made;
		    اگر (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_درون_داد_buf(buf, dst_made);
		    }
		    وگرنه
		    {
			add_to_درون_داد_buf(string, len);
		    }

		    بازگشت Pt_CONSUME;
		}
		len = 0;
		ch = کلید->کلید_cap;
		اگر (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    اگر (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			اگر ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			وگرنه اگر (ch == '2')
			    ch = NUL;
			وگرنه اگر (ch == '6')
			    ch = 0x1e;
			وگرنه اگر (ch == '-')
			    ch = 0x1f;
			وگرنه
			    modifiers |= MOD_MASK_CTRL;
		    }

		    اگر (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		وگرنه
		{
		    بازگشت Pt_CONTINUE;
		}
	    }
	    وگرنه
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_کلید(ch, &modifiers);
	اگر (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	اگر (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	وگرنه
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    صحیح int_ch = check_for_interrupt(ch, modifiers);

	    اگر (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_درون_داد_buf();
	    }
	}

	اگر (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	اگر (len > 0)
	{
	    add_to_درون_داد_buf(string, len);
	    بازگشت Pt_CONSUME;
	}
    }

    بازگشت Pt_CONTINUE;
}

    ایستا صحیح
gui_ph_handle_mouse(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    صحیح		     button = 0, repeated_click, modifiers = 0x0;
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

    اگر (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	بازگشت Pt_CONTINUE;
    }

    اگر (pointer->کلید_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    اگر (pointer->کلید_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    اگر (pointer->کلید_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    اگر (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_چپ;

    اگر (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_راست;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    اگر (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_میانه;

    // Catch a real release (not phantom or other releases
    اگر (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    اگر (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#اگر 0
    // Vim doesn't use button repeats
    اگر (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    اگر ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_شمار >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    بازگشت Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ایستا صحیح
gui_ph_handle_focus(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    اگر (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, پوچ);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    وگرنه
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, پوچ);
    }
    بازگشت Pt_CONTINUE;
}

    ایستا تهی
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    اگر (is_ignore_draw == TRUE)
	بازگشت;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#اگر 0
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
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, پوچ));

#اگر 1
    // Redraw individual damage regions
    اگر (damage->next != پوچ)
	damage = damage->next;

    وقتی (damage != پوچ)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#وگرنه
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    ایستا صحیح
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	تهی *data,
	PtCallbackInfo_t *info)
{
    اگر (data != پوچ)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, پوچ);
	PtRealizeWidget(menu->submenu_id);
    }

    بازگشت Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ایستا صحیح
gui_ph_handle_menu(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    اگر (data != پوچ)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    بازگشت Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    ایستا صحیح
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	تهی *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, پوچ);
    بازگشت Pt_CONTINUE;
}

    ایستا صحیح
gui_ph_handle_window_open(
	PtWidget_t *widget,
	تهی *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    بازگشت Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    ایستا تهی
gui_ph_draw_start(تهی)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, پوچ));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    ایستا تهی
gui_ph_draw_end(تهی)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    ایستا vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *بالاترین_level = ریشه_menu;
    vimmenu_T *اقلام = پوچ;

    وقتی (بالاترین_level != پوچ &&
	    (STRCMP(بالاترین_level->dname, "Buffers") != 0))
	بالاترین_level = بالاترین_level->next;

    اگر (بالاترین_level != پوچ)
    {
	اقلام = بالاترین_level->children;

	وقتی (اقلام != پوچ &&
		(STRCMP(اقلام->dname, name) != 0))
	    اقلام = اقلام->next;
    }
    بازگشت اقلام;
}

    ایستا تهی
gui_ph_pg_set_buffer_رقم(int_u buf_رقم)
{
    صحیح i;
    نویسه جستجو[16];
    نویسه *mark;

    اگر (gui.vimTextArea == پوچ || buf_رقم == 0)
	بازگشت;

    جستجو[0] = '(';
    ultoa(buf_رقم, &جستجو[1], 10);
    STRCAT(جستجو, ")");

    برای (i = 0; i < رقم_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	اگر (mark != پوچ && STRCMP(mark, جستجو) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    ایستا صحیح
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	تهی *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    اگر (info->event != پوچ)
    {
	panel = info->cbdata;
	اگر (panel->new_panel != پوچ)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    اگر (menu)
		gui_menu_cb(menu);
	}
    }
    بازگشت Pt_CONTINUE;
}

    ایستا تهی
gui_ph_get_panelgroup_margins(
	short *بالاترین,
	short *bottom,
	short *چپ,
	short *راست)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ثابت unsigned short *margin_بالاترین, *margin_bottom;
    ثابت unsigned short *margin_چپ, *margin_راست;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_راست, &margin_راست, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *بالاترین    = abs_raw_y;
    *bottom = *margin_bottom;

    *چپ  = abs_raw_x;
    *راست = *margin_راست;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ایستا صحیح
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    نویسه *mark;

    اگر (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	اگر (mark != پوچ)
	{
	    mark++;
	    وقتی (SAFE_isdigit(*mark))
		mark++;

	    اگر (*mark == ')')
		بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

    ایستا تهی
gui_ph_pg_add_buffer(نویسه *name)
{
    نویسه **new_titles = پوچ;

    new_titles = ALLOC_MULT(نویسه *, (رقم_panels + 1));
    اگر (new_titles == پوچ)
	بازگشت;

    اگر (رقم_panels > 0)
	memcpy(new_titles, panel_titles, رقم_panels * اندازه(نویسه **));

    new_titles[ رقم_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    رقم_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ایستا تهی
gui_ph_pg_remove_buffer(نویسه *name)
{
    صحیح i;
    نویسه **new_titles = پوچ;

    // If there is only 1 panel, we just use the temporary place holder
    اگر (رقم_panels > 1)
    {
	new_titles = ALLOC_MULT(نویسه *, رقم_panels - 1);
	اگر (new_titles != پوچ)
	{
	    نویسه **s = new_titles;
	    // Copy all the titles except the one we're removing
	    برای (i = 0; i < رقم_panels; i++)
	    {
		اگر (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    رقم_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    رقم_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    وگرنه
    {
	رقم_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ایستا صحیح
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	تهی *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    اگر (data != پوچ)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    بازگشت Pt_CONTINUE;
}
#endif

    ایستا صحیح
gui_ph_pane_resize(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    اگر (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    بازگشت Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    تهی
gui_ph_encoding_changed(صحیح new_encoding)
{
    // Default encoding is latin1
    نویسه *charset = "latin1";
    صحیح i;

    ساختار {
	صحیح encoding;
	نویسه *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    برای (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	اگر (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    تهی
gui_mch_prepare(صحیح *argc, نویسه **argv)
{
    PtInit(پوچ);
}

    صحیح
gui_mch_init(تهی)
{
    PtArg_t args[10];
    صحیح	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    اگر (gui.event_buffer == پوچ)
	بازگشت FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(پوچ, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, پوچ, n, args);
    اگر (gui.vimWindow == پوچ)
	بازگشت FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, پوچ);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, پوچ);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    اگر (gui.vimPanelGroup == پوچ)
	بازگشت FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, پوچ);
#وگرنه
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    اگر (gui.vimContainer == پوچ)
	بازگشت FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, پوچ);
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
    اگر (gui.vimTextArea == پوچ)
	بازگشت FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, پوچ);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_کلید,
	    gui_ph_handle_keyboard, پوچ);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, پوچ);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, پوچ);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, پوچ);
    اگر (gui_ph_timer_cursor == پوچ)
	بازگشت FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, پوچ);
    اگر (gui_ph_timer_timeout == پوچ)
	بازگشت FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, پوچ);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, پوچ);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_چپ_راست,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    اگر (gui.vimToolBarGroup == پوچ)
	بازگشت FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, پوچ);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    اگر (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    اگر (gui.vimMenuBar == پوچ)
	بازگشت FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_چپ_راست |Pt_بالاترین_ANCHORED_بالاترین, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    اگر (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    اگر (gui.vimToolBar == پوچ)
	بازگشت FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    بازگشت OK;
}

    صحیح
gui_mch_init_check(تهی)
{
    بازگشت (is_photon_available == TRUE) ? OK : FAIL;
}

    صحیح
gui_mch_open(تهی)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    اگر (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    بازگشت (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    تهی
gui_mch_exit(صحیح rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, پوچ);

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
    ایستا صحیح
exit_gui_mch_update(تهی *data)
{
    *(صحیح *)data = FALSE;
    بازگشت Pt_END;
}

    تهی
gui_mch_update(تهی)
{
    صحیح working = TRUE;

    PtAppAddWorkProc(پوچ, exit_gui_mch_update, &working);
    وقتی ((working == TRUE) && !vim_is_درون_داد_buf_full())
	PtProcessEvent();
}

    صحیح
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    اگر (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    وقتی (1)
    {
	PtProcessEvent();
	اگر (درون_داد_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    بازگشت OK;
	}
	وگرنه اگر (is_timeout == TRUE)
	    بازگشت FAIL;
    }
}

#اگر defined(FEAT_BROWSE)
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
	صحیح saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    صحیح	    flags;
    char_u  *default_path;
    char_u  *open_text = پوچ;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    اگر (default_path != پوچ)
    {
	اگر (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	اگر (initdir == پوچ || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	وگرنه
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	اگر (default_name != پوچ)
	{
	    اگر (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		پوچ,
		title,
		default_path,
		پوچ,
		open_text,
		پوچ,
		پوچ,
		&file,
		flags);

	vim_free(default_path);

	اگر (file.ret == Pt_FSDIALOG_BTN1)
	    بازگشت vim_strsave(file.path);
    }
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_GUI_DIALOG)
ایستا PtWidget_t *gui_ph_dialog_text = پوچ;

    ایستا صحیح
gui_ph_dialog_close(صحیح button, تهی *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    اگر (gui_ph_dialog_text != پوچ)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (تهی *) button);

    بازگشت Pt_TRUE;
}

    ایستا صحیح
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    تهی		*data,
    PtCallbackInfo_t	*info)
{
    اگر (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    بازگشت Pt_CONTINUE;
}

    ایستا صحیح
gui_ph_dialog_esc(PtWidget_t *widget, تهی *data, PtCallbackInfo_t *info)
{
    Ph_کلید_Event_t *کلید;

    کلید = PhGetData(info->event);
    اگر ((کلید->کلید_flags & Pk_KF_Cap_Valid) && (کلید->کلید_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	بازگشت Pt_CONSUME;
    }
    بازگشت Pt_PROCESS;
}

    صحیح
gui_mch_dialog(
	صحیح	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	صحیح	default_button,
	char_u	*textfield,
	صحیح	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    صحیح		button_شمار;
    صحیح		i, len;
    صحیح		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_شمار = len = i = 0;

    اگر (buttons == پوچ || *buttons == NUL)
	بازگشت -1;

    // There is one less separator than buttons, so bump up the button count
    button_شمار = 1;

    // Count string length and number of separators
    برای (str = buttons; *str; str++)
    {
	len++;
	اگر (*str == DLG_BUTTON_SEP)
	    button_شمار++;
    }

    اگر (title == پوچ)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_شمار);
    اگر (buttons_copy != پوچ && button_array != پوچ)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	برای (i = 0; i < button_شمار; i++)
	{
	    button_array[ i ] = str;
	    برای (; *str; str++)
	    {
		اگر (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    بشکن;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, پوچ,
		title,
		پوچ,
		message, پوچ,
		button_شمار, (ثابت نویسه **) button_array, پوچ,
		default_button, 0, Pt_MODAL);
# وگرنه
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    صحیح n;
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
		    Pt_TRUE, Pt_GROUP_NO_کلیدها | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, پوچ, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    اگر (textfield != پوچ)
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
	    di.buttons = (ثابت نویسه **) button_array;
	    di.nbtns = button_شمار;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_شمار;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_کلید,
		    gui_ph_dialog_esc, &modal_ctrl);

	    اگر (gui_ph_dialog_text != پوچ)
		PtGiveFocus(gui_ph_dialog_text, پوچ);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (صحیح) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = پوچ;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    بازگشت dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, پوچ);

    *x = pos->x;
    *y = pos->y;

    بازگشت OK;
}

    تهی
gui_mch_set_winpos(صحیح x, صحیح y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    تهی
gui_mch_set_shellsize(صحیح width, صحیح height,
	صحیح min_width, صحیح min_height, صحیح base_width, صحیح base_height,
	صحیح direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_چپ + pg_margin_راست;
    window_size.h += pg_margin_بالاترین + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    اگر (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    تهی
gui_mch_get_screen_dimensions(صحیح *screen_w, صحیح *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_درون_داد_Group(پوچ), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    تهی
gui_mch_iconify(تهی)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#اگر defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    تهی
gui_mch_set_foreground(تهی)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    تهی
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_رقم(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    تهی
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, صحیح val, صحیح size, صحیح max)
{
    صحیح	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_مقدار, val, 0);
    PtSetResources(sb->id, n, args);
}

    تهی
gui_mch_set_scrollbar_pos(scrollbar_T *sb, صحیح x, صحیح y, صحیح w, صحیح h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    صحیح
gui_mch_get_scrollbar_xpadding(تهی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    بازگشت 0;
}

    صحیح
gui_mch_get_scrollbar_ypadding(تهی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    بازگشت 0;
}

    تهی
gui_mch_create_scrollbar(scrollbar_T *sb, صحیح orient)
{
    صحیح	    n = 0;
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
#اگر 0
    // Don't need this anchoring for the scrollbars
    اگر (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_چپ_ANCHORED_چپ | Pt_راست_ANCHORED_راست;
    }
    وگرنه
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_بالاترین_ANCHORED_بالاترین;
	اگر (sb->wp != پوچ)
	{
	    اگر (sb == &sb->wp->w_scrollbars[ SBAR_چپ ])
		anchor_flags |= Pt_چپ_ANCHORED_چپ;
	    وگرنه
		anchor_flags |= Pt_راست_ANCHORED_راست;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#وگرنه
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    تهی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    اگر (flag != 0)
	PtRealizeWidget(sb->id);
    وگرنه
	PtUnrealizeWidget(sb->id);
}

    تهی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = پوچ;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#اگر defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ایستا صحیح last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ایستا صحیح mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_درج,		// beam
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

    تهی
mch_set_mouse_shape(صحیح shape)
{
    صحیح	    id;

    اگر (!gui.in_use)
	بازگشت;

    اگر (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    وگرنه
    {
	اگر (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	وگرنه
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    اگر (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    تهی
gui_mch_mousehide(صحیح hide)
{
    اگر (gui.pointer_hidden == hide)
	بازگشت;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    اگر (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    وگرنه
	mch_set_mouse_shape(last_shape);
#وگرنه
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    تهی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_درون_داد_Group(پوچ), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    تهی
gui_mch_setmouse(صحیح x, صحیح y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_درون_داد_Group(پوچ), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    بازگشت (guicolor_T)(PgRGB(Pg_Red_مقدار(pixel),
				     Pg_Green_مقدار(pixel), Pg_Blue_مقدار(pixel)));
}

    تهی
gui_mch_new_colors(تهی)
{
#اگر 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    اگر (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_مقدار(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_مقدار(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_مقدار(gui_ph_mouse_color) ^ 255;

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
    بازگشت gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    بازگشت gui_get_rgb_color_cmn(r, g, b);
}

    تهی
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    تهی
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    تهی
gui_mch_set_sp_color(guicolor_T color)
{
}

    تهی
gui_mch_invert_rectangle(صحیح row, صحیح col, صحیح nr, صحیح nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    اگر (nc > 0)
	rect.lr.x -= 1;
    اگر (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    تهی
gui_mch_clear_block(صحیح row1, صحیح col1, صحیح row2, صحیح col2)
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

    تهی
gui_mch_clear_all(تهی)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    اگر (is_ignore_draw == TRUE)
	بازگشت;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    تهی
gui_mch_delete_سطرها(صحیح row, صحیح رقم_سطرها)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_چپ);
    rect.ul.y = FILL_Y(row + رقم_سطرها);

    rect.lr.x = FILL_X(gui.scroll_region_راست + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, پوچ));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -رقم_سطرها * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - رقم_سطرها + 1,
	gui.scroll_region_چپ,
	gui.scroll_region_bot,
	gui.scroll_region_راست);
}

    تهی
gui_mch_درج_سطرها(صحیح row, صحیح رقم_سطرها)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_چپ);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_راست + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - رقم_سطرها + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, پوچ));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = رقم_سطرها * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_چپ,
	    row + رقم_سطرها - 1, gui.scroll_region_راست);
}

    تهی
gui_mch_draw_string(صحیح row, صحیح col, char_u *s, صحیح len, صحیح flags)
{
    ایستا نویسه *utf8_buffer = پوچ;
    ایستا صحیح	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    اگر (is_ignore_draw == TRUE)
	بازگشت;

    DRAW_START;

    اگر (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    اگر (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    اگر (charset_translate != پوچ && enc_utf8 == 0)
    {
	صحیح src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	اگر (utf8_len < len)
	{
	    نویسه *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    اگر (new_buffer == پوچ)
		بازگشت;
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

    اگر (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#اگر 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(پوچ);
#وگرنه
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    اگر (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    تهی
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

    تهی
gui_mch_draw_part_cursor(صحیح w, صحیح h, guicolor_T color)
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

    صحیح
gui_mch_is_blinking(تهی)
{
    بازگشت blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(تهی)
{
    بازگشت blink_state == BLINK_OFF;
}

    تهی
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    تهی
gui_mch_start_blink(تهی)
{
    // Only turn on the timer on if none of the times are zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    تهی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    اگر (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    تهی
gui_mch_beep(تهی)
{
    PtBeep();
}

    تهی
gui_mch_flash(صحیح msec)
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

    تهی
gui_mch_flush(تهی)
{
    PgFlush();
}

    تهی
gui_mch_set_text_area_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برای (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	اگر (name[0] == special_کلیدها[i].vim_code0 &&
		 name[1] == special_کلیدها[i].vim_code1)
	    بازگشت OK;
    بازگشت FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
# include "toolbar.phi"

ایستا PhImage_t *gui_ph_toolbar_images[] = {
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

ایستا PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    ایستا PhImage_t external_icon;
    PhImage_t *temp_phi = پوچ;

    temp_phi = PxLoadImage(iconfile, پوچ);
    اگر (temp_phi != پوچ)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, اندازه(external_icon));
	آزاد(temp_phi);

	temp_phi = &external_icon;
    }
    بازگشت temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ایستا PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = پوچ;

    اگر (menu->icon_builtin == FALSE)
    {
	اگر (menu->iconfile != پوچ)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	اگر ((icon == پوچ) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	اگر (icon != پوچ)
	    بازگشت icon;
    }

    اگر (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	بازگشت gui_ph_toolbar_images[menu->iconidx];

    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_MENU)
    تهی
gui_mch_enable_menu(صحیح flag)
{
    اگر (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    وگرنه
	PtUnrealizeWidget(gui.vimMenuBar);
}

    تهی
gui_mch_set_menu_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ایستا تهی
gui_ph_position_menu(PtWidget_t *widget, صحیح priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    وقتی (traverse != پوچ)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	اگر (menu != پوچ &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_درج(widget, traverse, 1);
	    بازگشت;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    تهی
gui_mch_add_menu(vimmenu_T *menu, صحیح index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_کلید;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحیح	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = پوچ;

    اگر (menu_is_menubar(menu->name))
    {

	accel_کلید = vim_strchr(menu->name, '&');
	اگر (accel_کلید != پوچ)
	{
	    mnemonic_str[0] = accel_کلید[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	اگر (accel_کلید != پوچ)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_کلید, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	اگر (parent != پوچ)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_راست, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == پوچ) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	اگر (parent != پوچ)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	اگر (parent == پوچ)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    اگر (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	اگر (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    وگرنه اگر (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, پوچ);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    تهی
gui_mch_add_menu_item(vimmenu_T *menu, صحیح index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_کلید;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحیح	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    اگر (menu_is_toolbar(parent->name))
    {
	اگر (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_بالاترین_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	وگرنه
	{
	    اگر (strstr((ثابت نویسه *) p_toolbar, "text") != پوچ)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    اگر ((strstr((ثابت نویسه *) p_toolbar, "icons") != پوچ) &&
		    (gui_ph_toolbar_images != پوچ))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    اگر (strstr((ثابت نویسه *) p_toolbar, "tooltips") != پوچ)
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
	اگر (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    وگرنه
# endif
	اگر (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    وگرنه
    {
	accel_کلید = vim_strchr(menu->name, '&');
	اگر (accel_کلید != پوچ)
	{
	    mnemonic_str[0] = accel_کلید[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	اگر (accel_کلید != پوچ)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_کلید, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	اگر (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    تهی
gui_mch_destroy_menu(vimmenu_T *menu)
{
    اگر (menu->submenu_id != پوچ)
	PtDestroyWidget(menu->submenu_id);
    اگر (menu->id != پوچ)
	PtDestroyWidget(menu->id);

    menu->submenu_id = پوچ;
    menu->id = پوچ;
}

    تهی
gui_mch_menu_grey(vimmenu_T *menu, صحیح grey)
{
    long    flags, mask, fields;

    اگر (menu->id == پوچ)
	بازگشت;

    flags = PtWidgetFlags(menu->id);
    اگر (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    وگرنه
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    اگر (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    تهی
gui_mch_menu_hidden(vimmenu_T *menu, صحیح hidden)
{
    // TODO: [un]realize the widget?
}

    تهی
gui_mch_draw_menubar(تهی)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    تهی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    تهی
gui_mch_toggle_tearoffs(صحیح enable)
{
    // No tearoffs yet
}

#endif

#اگر defined(FEAT_TOOLBAR)
    تهی
gui_mch_show_toolbar(صحیح showit)
{
    اگر (showit)
	PtRealizeWidget(gui.vimToolBar);
    وگرنه
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    ایستا GuiFont
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
    اگر (font_tag != پوچ)
    {
	اگر (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != پوچ)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    اگر (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    اگر (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    اگر (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    اگر (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    اگر (font_size == info.size &&
		    style == (info.style & style))
		بازگشت (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    بازگشت پوچ;
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

    ایستا صحیح
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    اگر (mark == پوچ)
	name_len = STRLEN(vim_font);
    وگرنه
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    اگر (*font_name != پوچ)
    {
	اگر (mark != پوچ)
	{
	    وقتی (*mark != NUL && *mark++ == ':')
	    {
		انتخاب (SAFE_tolower(*mark++))
		{
		    حالت 'a': *font_flags |= PF_STYLE_ANTIALIAS; بشکن;
		    حالت 'b': *font_flags |= PF_STYLE_BOLD; بشکن;
		    حالت 'i': *font_flags |= PF_STYLE_ITALIC; بشکن;

		    حالت 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			اگر (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			بشکن;

		    پیشفرض:
			بشکن;
		}
	    }
	}
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

    صحیح
gui_mch_init_font(char_u *vim_font_name, صحیح fontset)
{
    char_u  *font_tag;
    char_u  *font_name = پوچ;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    اگر (vim_font_name == پوچ)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    اگر (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, پوچ, پوچ,
		"pcterm12", -1, PHFONT_FIXED, پوچ);

	اگر (font_tag == پوچ)
	    بازگشت FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    وگرنه
    {
	اگر (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    بازگشت FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	اگر (font_tag == پوچ)
	{
	    vim_free(font_name);
	    بازگشت FAIL;
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
    PfExtentText(&extent, پوچ, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    بازگشت OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
gui_mch_adjust_charheight(تهی)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    بازگشت OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, صحیح report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    اگر (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	اگر (font_tag != پوچ)
	    بازگشت (GuiFont)font_tag;
    }

    اگر (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    بازگشت FAIL;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    اگر (name == پوچ)
	بازگشت پوچ;
    بازگشت vim_strsave(name);
}
#endif

    تهی
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    تهی
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

