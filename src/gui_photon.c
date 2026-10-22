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

#اگر_تعریف_شدہ FEAT_TOOLBAR
# include <photon/PxImage.h>
#اگر_ختم

#اگر !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	صحیح
# define PhEvent_t	صحیح
# define Ph_نقطہ_t	صحیح
# define PtWidget_t	صحیح
# define Pg_BLACK	0
# define PtCallbackF_t	صحیح
# define PtCallbackInfo_t صحیح
# define PhTile_t	صحیح
# define PtWidget_t	صحیح
# define PhImage_t	صحیح
#اگر_ختم

#تعریف_کرو RGB(r, g, b) PgRGB(r, g, b)

#تعریف_کرو EVENT_بفر_سائز حجم(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#تعریف_کرو MOUSE_HIDE		TRUE
#تعریف_کرو MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#تعریف_منسوخ USE_PANEL_گروہ

#اگر_تعریف_شدہ USE_PANEL_گروہ
ساکن حرف	*خالی_خانے_title = "    ";
ساکن حرف	**panel_titles = تہی;
ساکن ushort_t	عدد_panels = 0;
ساکن مختصر pg_margin_بایاں, pg_margin_دایاں, pg_margin_چوٹی, pg_margin_bottom;
#اگر_ختم

#تعریف_کرو GUI_PH_MARGIN		4	// Size of the bevel

#تعریف_کرو GUI_PH_MOUSE_TYPE		Ph_CURSOR_داخل_کرو
ساکن PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ساکن Ph_نقطہ_t    gui_ph_raw_انحراف;
ساکن PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ساکن PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ساکن مختصر	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ساکن Ph_نقطہ_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
ساکن صحیح is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
ساکن ساخت PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
ساکن شمار {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

ساکن long_u	blink_waittime	= 700;
ساکن long_u	blink_ontime	= 400;
ساکن long_u	blink_offtime	= 250;

ساکن ساخت
{
    صحیح	    کلید_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدیں[] =
{
    {Pk_اوپر,	    'k', 'u'},
    {Pk_نیچے_کو,	    'k', 'd'},
    {Pk_بایاں,	    'k', 'l'},
    {Pk_دایاں,	    'k', 'r'},

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
    {Pk_عقب_Space,  'k', 'b'},
    {Pk_داخل_کرو,	    'k', 'I'},
    {Pk_Delete,	    'k', 'D'},
    {Pk_Home,	    'k', 'h'},
    {Pk_انتہا,	    '@', '7'},
    {Pk_پیشگی,	    'k', 'P'},
    {Pk_اگلا,	    'k', 'N'},
    {Pk_Print,	    '%', '9'},

    {Pk_KP_جوڑو,	    'K', '6'},
    {Pk_KP_تفریق,'K', '7'},
    {Pk_KP_Divide,  'K', '8'},
    {Pk_KP_Multiply,'K', '9'},
    {Pk_KP_Enter,   'K', 'A'},

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //داخل کرو
    {Pk_KP_Decimal, KS_EXTRA, KE_KDEL}, // Delete

    {Pk_KP_4,	    'k', 'l'}, // Left
    {Pk_KP_6,	    'k', 'r'}, // Right
    {Pk_KP_8,	    'k', 'u'}, // Up
    {Pk_KP_2,	    'k', 'd'}, // Down

    {Pk_KP_7,	    'K', '1'}, // Home
    {Pk_KP_1,	    'K', '4'}, //انتہا

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

ساکن PtCallbackF_t gui_ph_handle_timer_cursor;
ساکن PtCallbackF_t gui_ph_handle_timer_timeout;

ساکن PtCallbackF_t gui_ph_handle_کھڑکی_cb;

ساکن PtCallbackF_t gui_ph_handle_scrollbar;
ساکن PtCallbackF_t gui_ph_handle_keyboard;
ساکن PtCallbackF_t gui_ph_handle_mouse;
ساکن PtCallbackF_t gui_ph_handle_pulldown_menu;
ساکن PtCallbackF_t gui_ph_handle_menu;
ساکن PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ساکن PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ساکن PtCallbackF_t gui_ph_handle_menu_unrealized;

#اگر_تعریف_شدہ USE_PANEL_گروہ
ساکن خالی gui_ph_get_panelgroup_margins(مختصر*, مختصر*, مختصر*, مختصر*);
#اگر_ختم

ساکن خالی gui_ph_draw_آغاز(خالی);
ساکن خالی gui_ph_draw_انتہا(خالی);

// Set the text for the balloon
ساکن PtWidget_t * gui_ph_show_tooltip(PtWidget_t *کھڑکی_2,
			     PtWidget_t *widget,
			     صحیح position,
			     حرف *متن,
			     حرف *font,
			     PgColor_t fill_color,
			     PgColor_t متن_color);

////////////////////////////////////////////////////////////////////////////

ساکن PtWidget_t * gui_ph_show_tooltip(PtWidget_t *کھڑکی_2,
			     PtWidget_t *widget,
			     صحیح position,
			     حرف *متن,
			     حرف *font,
			     PgColor_t fill_color,
			     PgColor_t متن_color)
{
    PtArg_t arg;
    vimmenu_T *menu;
    char_u  *tooltip;

    Pt_سیٹ_Arg(&arg, Pt_ARG_اشارہ_گر, &menu, 0);
    PtGetResources(widget, 1, &arg);

    // Override the text and position

    tooltip = متن;
    اگر (menu != تہی)
    {
	صحیح اشاریہ = MENU_اشاریہ_TIP;
	اگر (menu->لڑیاں[ اشاریہ ] != تہی)
	    tooltip = menu->لڑیاں[ اشاریہ ];
    }

    واپس PtInflateBalloon(
	    کھڑکی_2,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_چوٹی,
	    tooltip,
	    font,
	    fill_color,
	    متن_color);
}

    ساکن خالی
gui_ph_resize_container(خالی)
{
    PhArea_t area;

    PtWidgetArea(gui.vim_کھڑکی, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    Pt_سیٹ_Resource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ساکن صحیح
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	خالی *other,
	PtCallbackInfo_t *info)
{
    PtContainerCallback_t *sizes = info->cbdata;
    PtWidget_t		*container;
    Ph_نقطہ_t		نیچے_menu;
    int_u		اونچائی;

    اونچائی = sizes->new_dim.h;

    // Because vim treats the toolbar and menubar separately,
    // and here they're lumped together into a PtToolbarGroup,
    // we only need either menu_height or toolbar_height set at once
    اگر (gui.menu_is_فعال)
    {
	gui.menu_اونچائی = اونچائی;
	gui.toolbar_اونچائی = 0;
    }
#اگر_تعریف_شدہ FEAT_TOOLBAR
    ورنہ
	gui.toolbar_اونچائی = اونچائی;
#اگر_ختم

    نیچے_menu.x = 0;
    نیچے_menu.y = اونچائی;

#اگر_تعریف_شدہ USE_PANEL_گروہ
    container = gui.vim_Panel_گروہ;
#ورنہ
    container = gui.vimContainer;
#اگر_ختم

    Pt_سیٹ_Resource(container, Pt_ARG_POS, &نیچے_menu, 0);

    gui_ph_resize_container();

#اگر_تعریف_شدہ USE_PANEL_گروہ
    gui_ph_get_panelgroup_margins(
	    &pg_margin_چوٹی, &pg_margin_bottom,
	    &pg_margin_بایاں, &pg_margin_دایاں);
#اگر_ختم
    واپس Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ساکن صحیح
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	خالی *ڈیٹا,
	PtCallbackInfo_t *info)
{
    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	Pt_سیٹ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    ورنہ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	Pt_سیٹ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    واپس Pt_CONTINUE;
}

    ساکن صحیح
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    خالی		*ڈیٹا,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    واپس Pt_CONTINUE;
}

    ساکن صحیح
gui_ph_handle_کھڑکی_cb(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    Ph_کھڑکی_Event_t *we = info->cbdata;
    ushort_t *چوڑائی, *اونچائی;

    انتخاب (we->event_f)
    {
	حالت Ph_WM_CLOSE:
	    gui_shell_closed();
	    توڑو;

	حالت Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    اگر (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_آغاز_blink();
	    }
	    ورنہ
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    توڑو;

	حالت Ph_WM_RESIZE:
	    PtGetResource(gui.vim_کھڑکی, Pt_ARG_چوڑائی, &چوڑائی, 0);
	    PtGetResource(gui.vim_کھڑکی, Pt_ARG_اونچائی, &اونچائی, 0);
#اگر_تعریف_شدہ USE_PANEL_گروہ
	    چوڑائی  -= (pg_margin_بایاں + pg_margin_دایاں);
	    اونچائی -= (pg_margin_چوٹی + pg_margin_bottom);
#اگر_ختم
	    gui_resize_shell(*چوڑائی, *اونچائی);
	    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    Pt_انتہا_Flux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    توڑو;

	طےشدہ:
	    توڑو;
    }

    واپس Pt_CONTINUE;
}

    ساکن صحیح
gui_ph_handle_scrollbar(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    صحیح	    قدر, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) ڈیٹا;
    اگر (sb != تہی)
    {
	قدر = scroll->position;
	انتخاب (scroll->action)
	{
	    حالت Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		توڑو;

	    حالت Pt_SCROLL_سیٹ:
		// FIXME: return straight away here?
		واپس Pt_CONTINUE;
		توڑو;
	}

	gui_drag_scrollbar(sb, قدر, dragging);
    }
    واپس Pt_CONTINUE;
}

    ساکن صحیح
gui_ph_handle_keyboard(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    Ph_کلید_Event_t    *کلید;
    غیرعلامتی حرف   string[6];
    صحیح		    len, i;
    صحیح		    ch, modifiers;

    کلید = Ph_Get_ڈیٹا(info->event);

    ch = modifiers = len = 0;

    اگر (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    اگر (کلید->کلید_flags & Pk_KF_Compose)
	واپس Pt_CONTINUE;

    اگر ((کلید->کلید_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_کلید_نیچے_کو(کلید->کلید_flags))
    {
#اگر_تعریف_شدہ FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	اگر (((کلید->کلید_mods & Pk_KM_Alt) &&
		    !(کلید->کلید_mods & Pk_KM_سرکاؤ) &&
		    !(کلید->کلید_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_فعال &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(کلید->کلید_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    واپس Pt_CONTINUE;
	}
#اگر_ختم

	برائے (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	{
	    اگر (special_کلیدیں[i].کلید_sym == کلید->کلید_cap)
	    {
		len = 0;
		اگر (special_کلیدیں[i].vim_code1 == NUL)
		    ch = special_کلیدیں[i].vim_code0;
		ورنہ
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    اگر (کلید->کلید_cap >= Pk_KP_Enter && کلید->کلید_cap <= Pk_KP_9
			    && (کلید->کلید_mods & Pk_KM_عدد_تالا))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = کلید->کلید_cap - 0xf080;
		    }
		    ورنہ
			ch = مقصد_SPECIAL(special_کلیدیں[i].vim_code0,
				special_کلیدیں[i].vim_code1);
		}
		توڑو;
	    }
	}

	اگر (کلید->کلید_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_نقاب_CTRL;
	اگر (کلید->کلید_mods & Pk_KM_Alt)
	    modifiers |= MOD_نقاب_ALT;
	اگر (کلید->کلید_mods & Pk_KM_سرکاؤ)
	    modifiers |= MOD_نقاب_سرکاؤ;

	// Is this not a special key?
	اگر (special_کلیدیں[i].کلید_sym == 0)
	{
	    ch = PhTo8859_1(کلید);
	    اگر (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_کلید_مقصد_Mb(string, کلید);
		اگر (len > 0)
		{
		    ساکن حرف buf[6];
		    صحیح src_لیا_گیا, dst_made;
		    اگر (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_لیا_گیا,
				buf,
				6,
				&dst_made);

			جوڑو_مقصد_داخلہ_buf(buf, dst_made);
		    }
		    ورنہ
		    {
			جوڑو_مقصد_داخلہ_buf(string, len);
		    }

		    واپس Pt_CONSUME;
		}
		len = 0;
		ch = کلید->کلید_cap;
		اگر (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    اگر (modifiers & MOD_نقاب_CTRL)
		    {
			modifiers &= ~MOD_نقاب_CTRL;

			اگر ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			ورنہ اگر (ch == '2')
			    ch = NUL;
			ورنہ اگر (ch == '6')
			    ch = 0x1e;
			ورنہ اگر (ch == '-')
			    ch = 0x1f;
			ورنہ
			    modifiers |= MOD_نقاب_CTRL;
		    }

		    اگر (modifiers & MOD_نقاب_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_نقاب_ALT;
		    }
		}
		ورنہ
		{
		    واپس Pt_CONTINUE;
		}
	    }
	    ورنہ
		modifiers &= ~MOD_نقاب_سرکاؤ;
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
	    string[ len++ ] = K_دوسرا(ch);
	    string[ len++ ] = K_تیسرا(ch);
	}
	ورنہ
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    صحیح int_ch = check_for_مداخلت(ch, modifiers);

	    اگر (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_داخلہ_buf();
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
	    جوڑو_مقصد_داخلہ_buf(string, len);
	    واپس Pt_CONSUME;
	}
    }

    واپس Pt_CONTINUE;
}

    ساکن صحیح
gui_ph_handle_mouse(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    Ph_اشارہ_گر_Event_t *اشارہ_گر;
    PhRect_t	     *pos;
    صحیح		     button = 0, repeated_click, modifiers = 0x0;
    مختصر	     mouse_x, mouse_y;

    اشارہ_گر = Ph_Get_ڈیٹا(info->event);
    pos = PhGetRects(info->event);

    gui_mch_mousehide(MOUSE_SHOW);

    /*
     * Coordinates need to be relative to the base window,
     * not relative to the vimTextArea widget
     */
    mouse_x = pos->ul.x + gui.border_چوڑائی;
    mouse_y = pos->ul.y + gui.border_چوڑائی;

    اگر (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	واپس Pt_CONTINUE;
    }

    اگر (اشارہ_گر->کلید_mods & Pk_KM_سرکاؤ)
	modifiers |= MOUSE_سرکاؤ;
    اگر (اشارہ_گر->کلید_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    اگر (اشارہ_گر->کلید_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    اگر (اشارہ_گر->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_بایاں;

    اگر (اشارہ_گر->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_دایاں;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = اشارہ_گر->pos.x;
	abs_mouse.y = اشارہ_گر->pos.y;
    }

    اگر (اشارہ_گر->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_درمیان;

    // Catch a real release (not phantom or other releases
    اگر (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    اگر (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#اگر 0
    // Vim doesn't use button repeats
    اگر (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#اگر_ختم

    // Don't do anything if it is one of the phantom mouse release events
    اگر ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (اشارہ_گر->click_گنتی >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    واپس Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ساکن صحیح
gui_ph_handle_focus(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    اگر (info->reason == Pt_CB_گمشدہ_FOCUS)
    {
	PtRemoveEventHandler(gui.vim_متن_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, تہی);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    ورنہ
    {
	Pt_جوڑو_Event_Handler(gui.vim_متن_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, تہی);
    }
    واپس Pt_CONTINUE;
}

    ساکن خالی
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    Ph_نقطہ_t	انحراف;
    Ph_نقطہ_t	translation;

    اگر (is_ignore_draw == TRUE)
	واپس;

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
    برآمد_flush();
#اگر_ختم

    Pt_Widget_انحراف(widget, &انحراف);
    Ph_Translate_نقطہ(&انحراف, PtWidgetPos(gui.vim_متن_Area, تہی));

#اگر 1
    // Redraw individual damage regions
    اگر (damage->اگلا != تہی)
	damage = damage->اگلا;

    جب (damage != تہی)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - انحراف.x, r->ul.y - انحراف.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->اگلا;
    }
#ورنہ
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - انحراف.x, r->ul.y - انحراف.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#اگر_ختم

    Pg_سیٹ_Translation(&translation, 0);
}

    ساکن صحیح
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	خالی *ڈیٹا,
	PtCallbackInfo_t *info)
{
    اگر (ڈیٹا != تہی)
    {
	vimmenu_T *menu = (vimmenu_T *) ڈیٹا;

	PtPositionMenu(menu->submenu_id, تہی);
	PtRealizeWidget(menu->submenu_id);
    }

    واپس Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ساکن صحیح
gui_ph_handle_menu(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    اگر (ڈیٹا != تہی)
    {
	vimmenu_T *menu = (vimmenu_T *) ڈیٹا;
	gui_menu_cb(menu);
    }
    واپس Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    ساکن صحیح
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	خالی *ڈیٹا,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vim_متن_Area, تہی);
    واپس Pt_CONTINUE;
}

    ساکن صحیح
gui_ph_handle_کھڑکی_open(
	PtWidget_t *widget,
	خالی *ڈیٹا,
	PtCallbackInfo_t *info)
{
    gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    واپس Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#تعریف_کرو DRAW_آغاز  gui_ph_draw_آغاز()
#تعریف_کرو DRAW_انتہا    gui_ph_draw_انتہا()

/*
 * TODO: Set a clipping rect?
 */
    ساکن خالی
gui_ph_draw_آغاز(خالی)
{
    PhGC_t *gc;

    gc = PgGetGC();
    Pg_سیٹ_Region(PtWidgetRid(Pt_ڈھونڈو_Disjoint(gui.vim_متن_Area)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    Pt_Widget_انحراف(gui.vim_متن_Area, &gui_ph_raw_انحراف);
    Ph_Translate_نقطہ(&gui_ph_raw_انحراف, PtWidgetPos(gui.vim_متن_Area, تہی));

    Pg_سیٹ_Translation(&gui_ph_raw_انحراف, Pg_RELATIVE);
}

    ساکن خالی
gui_ph_draw_انتہا(خالی)
{
    gui_ph_raw_انحراف.x = -gui_ph_raw_انحراف.x;
    gui_ph_raw_انحراف.y = -gui_ph_raw_انحراف.y;
    Pg_سیٹ_Translation(&gui_ph_raw_انحراف, Pg_RELATIVE);
}

#اگر_تعریف_شدہ USE_PANEL_گروہ
    ساکن vimmenu_T *
gui_ph_ڈھونڈو_بفر_شے(char_u *name)
{
    vimmenu_T *چوٹی_level = جڑ_menu;
    vimmenu_T *اشیاء = تہی;

    جب (چوٹی_level != تہی &&
	    (STRCMP(چوٹی_level->dname, "Buffers") != 0))
	چوٹی_level = چوٹی_level->اگلا;

    اگر (چوٹی_level != تہی)
    {
	اشیاء = چوٹی_level->children;

	جب (اشیاء != تہی &&
		(STRCMP(اشیاء->dname, name) != 0))
	    اشیاء = اشیاء->اگلا;
    }
    واپس اشیاء;
}

    ساکن خالی
gui_ph_pg_سیٹ_بفر_عدد(int_u buf_عدد)
{
    صحیح i;
    حرف تلاش[16];
    حرف *mark;

    اگر (gui.vim_متن_Area == تہی || buf_عدد == 0)
	واپس;

    تلاش[0] = '(';
    ultoa(buf_عدد, &تلاش[1], 10);
    STRCAT(تلاش, ")");

    برائے (i = 0; i < عدد_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	اگر (mark != تہی && STRCMP(mark, تلاش) == 0)
	{
	    Pt_سیٹ_Resource(gui.vim_Panel_گروہ, Pt_ARG_PG_CURRENT_اشاریہ,
		    i, 0);
	}
    }
}

    ساکن صحیح
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	خالی *ڈیٹا,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    Pt_Panel_گروہ_Callback_t *panel;

    اگر (info->event != تہی)
    {
	panel = info->cbdata;
	اگر (panel->new_panel != تہی)
	{
	    menu = gui_ph_ڈھونڈو_بفر_شے(panel->new_panel);
	    اگر (menu)
		gui_menu_cb(menu);
	}
    }
    واپس Pt_CONTINUE;
}

    ساکن خالی
gui_ph_get_panelgroup_margins(
	مختصر *چوٹی,
	مختصر *bottom,
	مختصر *بایاں,
	مختصر *دایاں)
{
    غیرعلامتی مختصر abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ثابت غیرعلامتی مختصر *margin_چوٹی, *margin_bottom;
    ثابت غیرعلامتی مختصر *margin_بایاں, *margin_دایاں;

    PtGetAbsPosition(gui.vim_متن_Area, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vim_Panel_گروہ, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vim_Panel_گروہ, Pt_ARG_MARGIN_دایاں, &margin_دایاں, 0);
    PtGetResource(gui.vim_Panel_گروہ, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *چوٹی    = abs_raw_y;
    *bottom = *margin_bottom;

    *بایاں  = abs_raw_x;
    *دایاں = *margin_دایاں;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ساکن صحیح
gui_ph_is_بفر_شے(vimmenu_T *menu, vimmenu_T *والد)
{
    حرف *mark;

    اگر (STRCMP(والد->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	اگر (mark != تہی)
	{
	    mark++;
	    جب (محفوظ_isdigit(*mark))
		mark++;

	    اگر (*mark == ')')
		واپس TRUE;
	}
    }
    واپس FALSE;
}

    ساکن خالی
gui_ph_pg_جوڑو_بفر(حرف *name)
{
    حرف **new_titles = تہی;

    new_titles = ALLOC_MULT(حرف *, (عدد_panels + 1));
    اگر (new_titles == تہی)
	واپس;

    اگر (عدد_panels > 0)
	حافظہ_نقل(new_titles, panel_titles, عدد_panels * حجم(حرف **));

    new_titles[ عدد_panels++ ] = name;

    Pt_سیٹ_Resource(gui.vim_Panel_گروہ, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    عدد_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ساکن خالی
gui_ph_pg_remove_بفر(حرف *name)
{
    صحیح i;
    حرف **new_titles = تہی;

    // If there is only 1 panel, we just use the temporary place holder
    اگر (عدد_panels > 1)
    {
	new_titles = ALLOC_MULT(حرف *, عدد_panels - 1);
	اگر (new_titles != تہی)
	{
	    حرف **s = new_titles;
	    // Copy all the titles except the one we're removing
	    برائے (i = 0; i < عدد_panels; i++)
	    {
		اگر (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    عدد_panels--;

	    Pt_سیٹ_Resource(gui.vim_Panel_گروہ, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    عدد_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    ورنہ
    {
	عدد_panels--;
	Pt_سیٹ_Resource(gui.vim_Panel_گروہ, Pt_ARG_PG_PANEL_TITLES, &خالی_خانے_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ساکن صحیح
gui_ph_handle_بفر_remove(
	PtWidget_t *widget,
	خالی *ڈیٹا,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    اگر (ڈیٹا != تہی)
    {
	menu = (vimmenu_T *) ڈیٹا;
	gui_ph_pg_remove_بفر(menu->dname);
    }

    واپس Pt_CONTINUE;
}
#اگر_ختم

    ساکن صحیح
gui_ph_pane_resize(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    اگر (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	Pt_آغاز_Flux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    واپس Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    خالی
gui_ph_encoding_بدلا(صحیح new_encoding)
{
    // Default encoding is latin1
    حرف *charset = "latin1";
    صحیح i;

    ساخت {
	صحیح encoding;
	حرف *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    برائے (i = 0; i < ARRAY_لمبائی(charsets); i++)
    {
	اگر (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = Px_Translate_سیٹ(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    خالی
gui_mch_prepare(صحیح *argc, حرف **argv)
{
    PtInit(تہی);
}

    صحیح
gui_mch_init(خالی)
{
    PtArg_t دلائل[10];
    صحیح	    flags = 0, n = 0;

    PhDim_t	کھڑکی_سائز = {100, 100}; // Arbitrary values
    Ph_نقطہ_t	pos = {0, 0};

    gui.event_بفر = alloc(EVENT_بفر_سائز);
    اگر (gui.event_بفر == تہی)
	واپس FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = Px_Translate_سیٹ(تہی, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_انحراف = gui.border_چوڑائی = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_کھڑکی_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_کھڑکی_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_DIM, &کھڑکی_سائز, 0);
    gui.vim_کھڑکی = PtCreateWidget(Pt_کھڑکی, تہی, n, دلائل);
    اگر (gui.vim_کھڑکی == تہی)
	واپس FAIL;

    Pt_جوڑو_Callback(gui.vim_کھڑکی, Pt_CB_کھڑکی, gui_ph_handle_کھڑکی_cb, تہی);
    Pt_جوڑو_Callback(gui.vim_کھڑکی, Pt_CB_کھڑکی_OPENING,
	    gui_ph_handle_کھڑکی_open, تہی);

    n = 0;
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_DIM, &کھڑکی_سائز, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_POS, &pos, 0);

#اگر_تعریف_شدہ USE_PANEL_گروہ
    // Put in a temporary place holder title
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_PG_PANEL_TITLES, &خالی_خانے_title, 1);

    gui.vim_Panel_گروہ = PtCreateWidget(Pt_Panel_گروہ, gui.vim_کھڑکی, n, دلائل);
    اگر (gui.vim_Panel_گروہ == تہی)
	واپس FAIL;

    Pt_جوڑو_Callback(gui.vim_Panel_گروہ, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, تہی);
#ورنہ
    // Turn off all edge decorations
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_BEVEL_چوڑائی, 0, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_MARGIN_چوڑائی, 0, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_MARGIN_اونچائی, 0, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vim_کھڑکی, n, دلائل);
    اگر (gui.vimContainer == تہی)
	واپس FAIL;

    Pt_جوڑو_Callback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, تہی);
#اگر_ختم

    // Size for the text area is set in gui_mch_set_text_area_pos
    n = 0;

    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_RAW_DRAW_F, gui_ph_handle_raw_draw, 1);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_BEVEL_چوڑائی, GUI_PH_MARGIN, 0);
    /*
     * Using focus render also causes the whole widget to be redrawn
     * whenever it changes focus, which is very annoying :p
     */
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_FLAGS, Pt_TRUE,
	    Pt_GETS_FOCUS | Pt_HIGHLIGHTED);
#اگر_غیر_تعریف_شدہ FEAT_MOUSESHAPE
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_CURSOR_TYPE, GUI_PH_MOUSE_TYPE, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_CURSOR_COLOR, gui_ph_mouse_color, 0);
#اگر_ختم

    gui.vim_متن_Area = PtCreateWidget(PtRaw, Pt_DFLT_والد, n, دلائل);
    اگر (gui.vim_متن_Area == تہی)
	واپس FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    Pt_جوڑو_Event_Handler(gui.vim_متن_Area,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, تہی);
    Pt_جوڑو_Event_Handler(gui.vim_متن_Area, Ph_EV_کلید,
	    gui_ph_handle_keyboard, تہی);
    Pt_جوڑو_Callback(gui.vim_متن_Area, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, تہی);
    Pt_جوڑو_Callback(gui.vim_متن_Area, Pt_CB_گمشدہ_FOCUS,
	    gui_ph_handle_focus, تہی);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vim_کھڑکی, 0, تہی);
    اگر (gui_ph_timer_cursor == تہی)
	واپس FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vim_کھڑکی, 0, تہی);
    اگر (gui_ph_timer_timeout == تہی)
	واپس FAIL;

    Pt_جوڑو_Callback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, تہی);
    Pt_جوڑو_Callback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, تہی);

#اگر_تعریف_شدہ FEAT_MENU
    n = 0;
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_چوڑائی, کھڑکی_سائز.w, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_بایاں_دایاں,
	    Pt_IS_ANCHORED);
    gui.vim_Tool_Bar_گروہ = PtCreateWidget(Pt_Toolbar_گروہ, gui.vim_کھڑکی,
	    n, دلائل);
    اگر (gui.vim_Tool_Bar_گروہ == تہی)
	واپس FAIL;

    Pt_جوڑو_Callback(gui.vim_Tool_Bar_گروہ, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, تہی);

    n = 0;
    flags = 0;
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_چوڑائی, کھڑکی_سائز.w, 0);
    اگر (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vim_Tool_Bar_گروہ, n, دلائل);
    اگر (gui.vimMenuBar == تہی)
	واپس FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_بایاں_دایاں |Pt_چوٹی_ANCHORED_چوٹی, Pt_IS_ANCHORED);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_چوڑائی, کھڑکی_سائز.w, 0);

    flags = Pt_GETS_FOCUS;
    اگر (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vim_Tool_Bar_گروہ, n, دلائل);
    اگر (gui.vimToolBar == تہی)
	واپس FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#اگر_ختم

    واپس OK;
}

    صحیح
gui_mch_init_check(خالی)
{
    واپس (is_photon_دستیاب == TRUE) ? OK : FAIL;
}

    صحیح
gui_mch_open(خالی)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.عقب_pixel =  Pg_WHITE;

    سیٹ_معمول_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_عقب_pixel = gui.عقب_pixel;

    highlight_gui_started();

    اگر (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_سیٹ_winpos(gui_win_x, gui_win_y);

    واپس (PtRealizeWidget(gui.vim_کھڑکی) == 0) ? OK : FAIL;
}

    خالی
gui_mch_exit(صحیح rc)
{
    PtDestroyWidget(gui.vim_کھڑکی);

    Px_Translate_سیٹ(charset_translate, تہی);

    vim_free(gui.event_بفر);

#اگر_تعریف_شدہ USE_PANEL_GROUPS
    vim_free(panel_titles);
#اگر_ختم
}

////////////////////////////////////////////////////////////////////////////
// events

/*
 * When no events are available, photon will call this function, working is
 * set to FALSE, and the gui_mch_update loop will exit.
 */
    ساکن صحیح
exit_gui_mch_update(خالی *ڈیٹا)
{
    *(صحیح *)ڈیٹا = FALSE;
    واپس Pt_انتہا;
}

    خالی
gui_mch_update(خالی)
{
    صحیح working = TRUE;

    Pt_App_جوڑو_Work_Proc(تہی, exit_gui_mch_update, &working);
    جب ((working == TRUE) && !vim_is_داخلہ_buf_بھرا())
	PtProcessEvent();
}

    صحیح
gui_mch_wait_for_chars(طویل wtime)
{
    is_timeout = FALSE;

    اگر (wtime >= 0)
	Pt_سیٹ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    جب (1)
    {
	PtProcessEvent();
	اگر (داخلہ_دستیاب())
	{
	    Pt_سیٹ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    واپس OK;
	}
	ورنہ اگر (is_timeout == TRUE)
	    واپس FAIL;
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
    char_u  *default_راستہ;
    char_u  *open_متن = تہی;

    flags = 0;
    CLEAR_میدان(file);

    default_راستہ = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    اگر (default_راستہ != تہی)
    {
	اگر (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_متن = "&Save";
	}

	// combine the directory and filename into a single path
	اگر (initdir == تہی || *initdir == NUL)
	{
	    mch_dirname(default_راستہ, MAXPATHL);
	    initdir = default_راستہ;
	}
	ورنہ
	{
	    STRCPY(default_راستہ, initdir);
	    initdir = default_راستہ;
	}

	اگر (default_name != تہی)
	{
	    اگر (default_راستہ[ STRLEN(default_راستہ) - 1 ] != '/')
		STRCAT(default_راستہ, "/");

	    STRCAT(default_راستہ, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vim_کھڑکی,
		تہی,
		title,
		default_راستہ,
		تہی,
		open_متن,
		تہی,
		تہی,
		&file,
		flags);

	vim_free(default_راستہ);

	اگر (file.ret == Pt_FSDIALOG_BTN1)
	    واپس vim_strsave(file.راستہ);
    }
    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_GUI_DIALOG)
ساکن PtWidget_t *gui_ph_dialog_متن = تہی;

    ساکن صحیح
gui_ph_dialog_close(صحیح button, خالی *ڈیٹا)
{
    PtModalCtrl_t *modal_ctrl = ڈیٹا;
    char_u *dialog_متن, *vim_متن;

    اگر (gui_ph_dialog_متن != تہی)
    {
	PtGetResource(gui_ph_dialog_متن, Pt_ARG_متن_STRING, &dialog_متن, 0);
	PtGetResource(gui_ph_dialog_متن, Pt_ARG_اشارہ_گر, &vim_متن, 0);
	STRNCPY(vim_متن, dialog_متن, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (خالی *) button);

    واپس Pt_TRUE;
}

    ساکن صحیح
gui_ph_dialog_متن_enter(
    PtWidget_t		*widget,
    خالی		*ڈیٹا,
    PtCallbackInfo_t	*info)
{
    اگر (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, ڈیٹا);
    واپس Pt_CONTINUE;
}

    ساکن صحیح
gui_ph_dialog_esc(PtWidget_t *widget, خالی *ڈیٹا, PtCallbackInfo_t *info)
{
    Ph_کلید_Event_t *کلید;

    کلید = Ph_Get_ڈیٹا(info->event);
    اگر ((کلید->کلید_flags & Pk_KF_Cap_Valid) && (کلید->کلید_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, ڈیٹا);
	واپس Pt_CONSUME;
    }
    واپس Pt_PROCESS;
}

    صحیح
gui_mch_dialog(
	صحیح	type,
	char_u	*title,
	char_u	*پیغام_2,
	char_u	*buttons,
	صحیح	default_button,
	char_u	*textfield,
	صحیح	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    صحیح		button_گنتی;
    صحیح		i, len;
    صحیح		dialog_نتیجہ = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_گنتی = len = i = 0;

    اگر (buttons == تہی || *buttons == NUL)
	واپس -1;

    // There is one less separator than buttons, so bump up the button count
    button_گنتی = 1;

    // Count string length and number of separators
    برائے (str = buttons; *str; str++)
    {
	len++;
	اگر (*str == DLG_BUTTON_SEP)
	    button_گنتی++;
    }

    اگر (title == تہی)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_گنتی);
    اگر (buttons_copy != تہی && button_array != تہی)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	برائے (i = 0; i < button_گنتی; i++)
	{
	    button_array[ i ] = str;
	    برائے (; *str; str++)
	    {
		اگر (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    توڑو;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_نتیجہ = PtAlert(
		gui.vim_کھڑکی, تہی,
		title,
		تہی,
		پیغام_2, تہی,
		button_گنتی, (ثابت حرف **) button_array, تہی,
		default_button, 0, Pt_MODAL);
# ورنہ
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    صحیح n;
	    PtArg_t دلائل[5];
	    PtWidget_t *dialog, *pane;
	    PtModalCtrl_t modal_ctrl;
	    PtDialogInfo_t di;

	    CLEAR_میدان(di);
	    CLEAR_میدان(modal_ctrl);

	    n = 0;
	    Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_گروہ_صفیں_ستون_تعداد, 0, 0);
	    Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_چوڑائی, 350, 0);
	    Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_گروہ_ORIENTATION,
		    Pt_گروہ_VERTICAL, 0);
	    Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_گروہ_FLAGS,
		    Pt_TRUE, Pt_گروہ_NO_کلیدیں | Pt_گروہ_STRETCH_HORIZONTAL);
	    Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(Pt_گروہ, تہی, n, دلائل);

	    n = 0;
	    Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_متن_STRING, پیغام_2, 0);
	    PtCreateWidget(PtLabel, pane, n, دلائل);

	    اگر (textfield != تہی)
	    {
		n = 0;
		Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_MAX_لمبائی, IOSIZE - 1, 0);
		Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_متن_STRING, textfield, 0);
		Pt_سیٹ_Arg(&دلائل[n++], Pt_ARG_اشارہ_گر, textfield, 0);
		gui_ph_dialog_متن = PtCreateWidget(Pt_متن, pane, n, دلائل);
		Pt_جوڑو_Callback(gui_ph_dialog_متن, Pt_CB_ACTIVATE,
			gui_ph_dialog_متن_enter, &modal_ctrl);
	    }

	    di.والد = gui.vim_کھڑکی;
	    di.pane = pane;
	    di.title = title;
	    di.buttons = (ثابت حرف **) button_array;
	    di.nbtns = button_گنتی;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_گنتی;
	    di.callback = gui_ph_dialog_close;
	    di.ڈیٹا = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    Pt_جوڑو_Filter_Callback(dialog, Ph_EV_کلید,
		    gui_ph_dialog_esc, &modal_ctrl);

	    اگر (gui_ph_dialog_متن != تہی)
		PtGiveFocus(gui_ph_dialog_متن, تہی);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_نتیجہ = (صحیح) Pt_Modal_بلاک(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_متن = تہی;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    واپس dialog_نتیجہ;
}
#اگر_ختم
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    Ph_نقطہ_t *pos;

    pos = PtWidgetPos(gui.vim_کھڑکی, تہی);

    *x = pos->x;
    *y = pos->y;

    واپس OK;
}

    خالی
gui_mch_سیٹ_winpos(صحیح x, صحیح y)
{
    Ph_نقطہ_t pos = { x, y };

    Pt_سیٹ_Resource(gui.vim_کھڑکی, Pt_ARG_POS, &pos, 0);
}

    خالی
gui_mch_سیٹ_shellsize(صحیح چوڑائی, صحیح اونچائی,
	صحیح min_چوڑائی, صحیح min_اونچائی, صحیح بنیاد_چوڑائی, صحیح بنیاد_اونچائی,
	صحیح direction)
{
    PhDim_t کھڑکی_سائز = { چوڑائی, اونچائی };
    PhDim_t min_سائز = { min_چوڑائی, min_اونچائی };

#اگر_تعریف_شدہ USE_PANEL_گروہ
    کھڑکی_سائز.w += pg_margin_بایاں + pg_margin_دایاں;
    کھڑکی_سائز.h += pg_margin_چوٹی + pg_margin_bottom;
#اگر_ختم

    Pt_سیٹ_Resource(gui.vim_کھڑکی, Pt_ARG_MINIMUM_DIM, &min_سائز, 0);
    Pt_سیٹ_Resource(gui.vim_کھڑکی, Pt_ARG_DIM, &کھڑکی_سائز, 0);

    اگر (! PtWidgetIsRealized(gui.vim_کھڑکی))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    خالی
gui_mch_get_screen_dimensions(صحیح *screen_w, صحیح *screen_h)
{
    PhRect_t console;

    Ph_کھڑکی_Query_Visible(Ph_QUERY_WORKSPACE, 0,
	    Ph_داخلہ_گروہ(تہی), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    خالی
gui_mch_iconify(خالی)
{
    Ph_کھڑکی_Event_t event;

    CLEAR_میدان(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vim_کھڑکی);
    Pt_Forward_کھڑکی_Event(&event);
}

#اگر defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    خالی
gui_mch_سیٹ_foreground(خالی)
{
    Ph_کھڑکی_Event_t event;

    CLEAR_میدان(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vim_کھڑکی);
    Pt_Forward_کھڑکی_Event(&event);
}
#اگر_ختم

    خالی
gui_mch_settitle(char_u *title,	char_u *icon)
{
#اگر_تعریف_شدہ USE_PANEL_گروہ
    gui_ph_pg_سیٹ_بفر_عدد(curwin->w_بفر->b_fnum);
#اگر_ختم
    Pt_سیٹ_Resource(gui.vim_کھڑکی, Pt_ARG_کھڑکی_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    خالی
gui_mch_سیٹ_scrollbar_thumb(scrollbar_T *sb, صحیح val, صحیح سائز, صحیح max)
{
    صحیح	    n = 0;
    PtArg_t دلائل[3];

    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_زیادہ_سے_زیادہ, max, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_SLIDER_سائز, سائز, 0);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_GAUGE_قدر, val, 0);
    Pt_سیٹ_Resources(sb->id, n, دلائل);
}

    خالی
gui_mch_سیٹ_scrollbar_pos(scrollbar_T *sb, صحیح x, صحیح y, صحیح w, صحیح h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    Pt_سیٹ_Resource(sb->id, Pt_ARG_AREA, &area, 0);
}

    صحیح
gui_mch_get_scrollbar_xpadding(خالی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    واپس 0;
}

    صحیح
gui_mch_get_scrollbar_ypadding(خالی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    واپس 0;
}

    خالی
gui_mch_create_scrollbar(scrollbar_T *sb, صحیح orient)
{
    صحیح	    n = 0;
//    int	    anchor_flags = 0;
    PtArg_t دلائل[4];

    /*
     * Stop the scrollbar from being realized when the parent
     * is realized, so it can be explicitly realized by vim.
     *
     * Also, don't let the scrollbar get focus
     */
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE,
	    Pt_DELAY_REALIZE | Pt_GETS_FOCUS);
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_SCROLLBAR_FLAGS, Pt_SCROLLBAR_SHOW_ARROWS, 0);
#اگر 0
    // Don't need this anchoring for the scrollbars
    اگر (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_بایاں_ANCHORED_بایاں | Pt_دایاں_ANCHORED_دایاں;
    }
    ورنہ
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_چوٹی_ANCHORED_چوٹی;
	اگر (sb->wp != تہی)
	{
	    اگر (sb == &sb->wp->w_scrollbars[ SBAR_بایاں ])
		anchor_flags |= Pt_بایاں_ANCHORED_بایاں;
	    ورنہ
		anchor_flags |= Pt_دایاں_ANCHORED_دایاں;
	}
    }
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#اگر_ختم
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#اگر_تعریف_شدہ USE_PANEL_گروہ
    sb->id = PtCreateWidget(PtScrollbar, gui.vim_Panel_گروہ, n, دلائل);
#ورنہ
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, دلائل);
#اگر_ختم

    Pt_جوڑو_Callback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    خالی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    اگر (flag != 0)
	PtRealizeWidget(sb->id);
    ورنہ
	PtUnrealizeWidget(sb->id);
}

    خالی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = تہی;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#اگر defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساکن صحیح آخری_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساکن صحیح mshape_ids[] =
{
    Ph_CURSOR_اشارہ_گر,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_داخل_کرو,		// beam
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
    Ph_CURSOR_QUESTION_نقطہ,	// question
    Ph_CURSOR_اشارہ_گر,		// right-arrow
    Ph_CURSOR_اشارہ_گر,		// up-arrow
    Ph_CURSOR_اشارہ_گر		// last one
};

    خالی
mch_سیٹ_mouse_shape(صحیح shape)
{
    صحیح	    id;

    اگر (!gui.اندر_use)
	واپس;

    اگر (shape == MSHAPE_HIDE || gui.اشارہ_گر_hidden)
	Pt_سیٹ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    ورنہ
    {
	اگر (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_اشارہ_گر;
	ورنہ
	    id = mshape_ids[shape];

	Pt_سیٹ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    اگر (shape != MSHAPE_HIDE)
	آخری_shape = shape;
}
#اگر_ختم

    خالی
gui_mch_mousehide(صحیح hide)
{
    اگر (gui.اشارہ_گر_hidden == hide)
	واپس;

    gui.اشارہ_گر_hidden = hide;
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
    اگر (hide)
	Pt_سیٹ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    ورنہ
	mch_سیٹ_mouse_shape(آخری_shape);
#ورنہ
    Pt_سیٹ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#اگر_ختم
}

    خالی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    PhCursorInfo_t info;
    مختصر ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_داخلہ_گروہ(تہی), &info);
    PtGetAbsPosition(gui.vim_متن_Area , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    خالی
gui_mch_setmouse(صحیح x, صحیح y)
{
    مختصر abs_x, abs_y;

    PtGetAbsPosition(gui.vim_متن_Area, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_داخلہ_گروہ(تہی), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    واپس (guicolor_T)(PgRGB(Pg_Red_قدر(pixel),
				     Pg_Green_قدر(pixel), Pg_Blue_قدر(pixel)));
}

    خالی
gui_mch_new_colors(خالی)
{
#اگر 0 // Don't bother changing the cursor colour
    مختصر color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.عقب_pixel);

    اگر (مطلق_قدر(color_diff) < 64)
    {
	مختصر r, g, b;
	// not a great algorithm...
	r = Pg_Red_قدر(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_قدر(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_قدر(gui_ph_mouse_color) ^ 255;

# ifndef FEAT_MOUSESHAPE
	gui_ph_mouse_color = PgRGB(r, g, b);
	Pt_سیٹ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_COLOR,
		gui_ph_mouse_color, 0);
# endif
    }
#اگر_ختم

    Pt_سیٹ_Resource(gui.vim_متن_Area, Pt_ARG_FILL_COLOR, gui.عقب_pixel, 0);
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
    واپس gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    واپس gui_get_rgb_color_cmn(r, g, b);
}

    خالی
gui_mch_سیٹ_fg_color(guicolor_T color)
{
    Pg_سیٹ_متن_Color(color);
}

    خالی
gui_mch_سیٹ_bg_color(guicolor_T color)
{
    Pg_سیٹ_Fill_Color(color);
}

    خالی
gui_mch_سیٹ_sp_color(guicolor_T color)
{
}

    خالی
gui_mch_invert_rectangle(صحیح صف, صحیح ستون, صحیح nr, صحیح nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(ستون);
    rect.ul.y = FILL_Y(صف);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_چوڑائی;
    rect.lr.y = rect.ul.y + nr * gui.char_اونچائی;
    اگر (nc > 0)
	rect.lr.x -= 1;
    اگر (nr > 0)
	rect.lr.y -= 1;

    DRAW_آغاز;
    Pg_سیٹ_Draw_منوال(Pg_Draw_منوال_DSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    Pg_سیٹ_Draw_منوال(Pg_Draw_منوال_SRCCOPY);
    DRAW_انتہا;
}

    خالی
gui_mch_clear_بلاک(صحیح row1, صحیح col1, صحیح row2, صحیح col2)
{
    PhRect_t بلاک = {
	{ FILL_X(col1), FILL_Y(row1) },
	{ FILL_X(col2 + 1) - 1, FILL_Y(row2 + 1) - 1}
    };

    DRAW_آغاز;
    gui_mch_سیٹ_bg_color(gui.عقب_pixel);
    PgDrawRect(&بلاک, Pg_DRAW_FILL);
    DRAW_انتہا;
}

    خالی
gui_mch_clear_all(خالی)
{
    PhRect_t متن_rect = {
	{ gui.border_چوڑائی, gui.border_چوڑائی },
	{ Columns * gui.char_چوڑائی + gui.border_چوڑائی - 1 ,
	    صفیں * gui.char_اونچائی + gui.border_چوڑائی - 1 }
    };

    اگر (is_ignore_draw == TRUE)
	واپس;

    DRAW_آغاز;
    gui_mch_سیٹ_bg_color(gui.عقب_pixel);
    PgDrawRect(&متن_rect, Pg_DRAW_FILL);
    DRAW_انتہا;
}

    خالی
gui_mch_delete_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    PhRect_t    rect;
    Ph_نقطہ_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_بایاں);
    rect.ul.y = FILL_Y(صف + عدد_سطریں);

    rect.lr.x = FILL_X(gui.scroll_region_دایاں + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    Pt_Widget_انحراف(gui.vim_متن_Area, &gui_ph_raw_انحراف);
    Ph_Translate_نقطہ(&gui_ph_raw_انحراف, PtWidgetPos(gui.vim_متن_Area, تہی));
    PhTranslateRect(&rect, &gui_ph_raw_انحراف);

    delta.x = 0;
    delta.y = -عدد_سطریں * gui.char_اونچائی;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ڈھونڈو_Disjoint(gui.vim_متن_Area)), &rect, &delta);

    gui_clear_بلاک(
	gui.scroll_region_bot - عدد_سطریں + 1,
	gui.scroll_region_بایاں,
	gui.scroll_region_bot,
	gui.scroll_region_دایاں);
}

    خالی
gui_mch_داخل_کرو_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    PhRect_t    rect;
    Ph_نقطہ_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_بایاں);
    rect.ul.y = FILL_Y(صف);

    rect.lr.x = FILL_X(gui.scroll_region_دایاں + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - عدد_سطریں + 1) - 1;

    Pt_Widget_انحراف(gui.vim_متن_Area, &gui_ph_raw_انحراف);
    Ph_Translate_نقطہ(&gui_ph_raw_انحراف, PtWidgetPos(gui.vim_متن_Area, تہی));
    PhTranslateRect(&rect, &gui_ph_raw_انحراف);

    delta.x = 0;
    delta.y = عدد_سطریں * gui.char_اونچائی;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ڈھونڈو_Disjoint(gui.vim_متن_Area)) , &rect, &delta);

    gui_clear_بلاک(صف, gui.scroll_region_بایاں,
	    صف + عدد_سطریں - 1, gui.scroll_region_دایاں);
}

    خالی
gui_mch_draw_string(صحیح صف, صحیح ستون, char_u *s, صحیح len, صحیح flags)
{
    ساکن حرف *utf8_بفر = تہی;
    ساکن صحیح	utf8_len = 0;

    Ph_نقطہ_t	pos = { متن_X(ستون), متن_Y(صف) };
    PhRect_t	rect;

    اگر (is_ignore_draw == TRUE)
	واپس;

    DRAW_آغاز;

    اگر (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(ستون), FILL_Y(صف),
		FILL_X(ستون + len) - 1, FILL_Y(صف + 1) - 1,
		Pg_DRAW_FILL);
    }

    اگر (flags & DRAW_UNDERL)
	Pg_سیٹ_Underline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    اگر (charset_translate != تہی && enc_utf8 == 0)
    {
	صحیح src_لیا_گیا, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	اگر (utf8_len < len)
	{
	    حرف *new_بفر = realloc(utf8_بفر, len * MB_LEN_MAX);

	    اگر (new_بفر == تہی)
		واپس;
	    utf8_بفر = new_بفر;
	    utf8_len = len;
	}

	Px_Translate_مقصد_UTF(
		charset_translate,
		s,
		len,
		&src_لیا_گیا,
		utf8_بفر,
		utf8_len,
		&dst_made);
	s = utf8_بفر;
	len = dst_made;
    }

    Pg_Draw_متن(s, len, &pos, 0);

    اگر (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(ستون) + 1;
	rect.ul.y = FILL_Y(صف);
	rect.lr.x = FILL_X(ستون + len) - 1;
	rect.lr.y = FILL_Y(صف + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#اگر 0
	pos.x++;

	Pg_سیٹ_User_Clip(&rect);
	Pg_Draw_متن(s, len, &pos, 0);
	Pg_سیٹ_User_Clip(تہی);
#ورنہ
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	Pg_Draw_متن_Area(s, len, &rect, Pg_متن_BOTTOM);
#اگر_ختم
    }

    اگر (flags & DRAW_UNDERL)
	Pg_سیٹ_Underline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_انتہا;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    خالی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    PhRect_t r;

    // FIXME: Double width characters

    r.ul.x = FILL_X(gui.ستون);
    r.ul.y = FILL_Y(gui.صف);
    r.lr.x = r.ul.x + gui.char_چوڑائی - 1;
    r.lr.y = r.ul.y + gui.char_اونچائی - 1;

    DRAW_آغاز;
    Pg_سیٹ_Stroke_Color(color);
    PgDrawRect(&r, Pg_DRAW_STROKE);
    DRAW_انتہا;
}

    خالی
gui_mch_draw_part_cursor(صحیح w, صحیح h, guicolor_T color)
{
    PhRect_t r;

    r.ul.x = FILL_X(gui.ستون);
    r.ul.y = FILL_Y(gui.صف) + gui.char_اونچائی - h;
    r.lr.x = r.ul.x + w - 1;
    r.lr.y = r.ul.y + h - 1;

    DRAW_آغاز;
    gui_mch_سیٹ_bg_color(color);
    PgDrawRect(&r, Pg_DRAW_FILL);
    DRAW_انتہا;
}

    صحیح
gui_mch_is_blinking(خالی)
{
    واپس blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(خالی)
{
    واپس blink_state == BLINK_OFF;
}

    خالی
gui_mch_سیٹ_blinking(طویل wait, طویل on, طویل off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    خالی
gui_mch_آغاز_blink(خالی)
{
    // Only turn on the timer on if none of the times are zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.اندر_focus)
    {
	Pt_سیٹ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    خالی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    Pt_سیٹ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    اگر (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    خالی
gui_mch_beep(خالی)
{
    PtBeep();
}

    خالی
gui_mch_flash(صحیح msec)
{
    Pg_سیٹ_Fill_XORColor(Pg_BLACK, Pg_WHITE);
    Pg_سیٹ_Draw_منوال(Pg_DRAWMODE_XOR);
    gui_mch_clear_all();
    gui_mch_flush();

    ui_delay((طویل) msec, TRUE);

    gui_mch_clear_all();
    Pg_سیٹ_Draw_منوال(Pg_DRAWMODE_OPAQUE);
    gui_mch_flush();
}

    خالی
gui_mch_flush(خالی)
{
    PgFlush();
}

    خالی
gui_mch_سیٹ_متن_area_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    PhArea_t area = {{x, y}, {w, h}};

    Pt_سیٹ_Resource(gui.vim_متن_Area, Pt_ARG_AREA, &area, 0);
}

    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برائے (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	اگر (name[0] == special_کلیدیں[i].vim_code0 &&
		 name[1] == special_کلیدیں[i].vim_code1)
	    واپس OK;
    واپس FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#اگر_تعریف_شدہ FEAT_TOOLBAR
# include "toolbar.phi"

ساکن PhImage_t *gui_ph_toolbar_images[] = {
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
    &tb_ڈھونڈو_phi,
    &tb_save_all_phi,
    &tb_save_session_phi,
    &tb_new_session_phi,
    &tb_بھرو_session_phi,
    &tb_macro_phi,
    &tb_replace_phi,
    &tb_close_phi,
    &tb_maximize_phi,
    &tb_minimize_phi,
    &tb_بانٹو_phi,
    &tb_shell_phi,
    &tb_ڈھونڈو_پچھلا_phi,
    &tb_ڈھونڈو_اگلا_phi,
    &tb_ڈھونڈو_help_phi,
    &tb_make_phi,
    &tb_jump_phi,
    &tb_ctags_phi,
    &tb_vsplit_phi,
    &tb_maxwidth_phi,
    &tb_minwidth_phi
};

ساکن PhImage_t *
gui_ph_toolbar_بھرو_icon(char_u *iconfile)
{
    ساکن PhImage_t external_icon;
    PhImage_t *عارضی_phi = تہی;

    عارضی_phi = Px_بھرو_Image(iconfile, تہی);
    اگر (عارضی_phi != تہی)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	عارضی_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	حافظہ_نقل(&external_icon, عارضی_phi, حجم(external_icon));
	آزاد(عارضی_phi);

	عارضی_phi = &external_icon;
    }
    واپس عارضی_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ساکن PhImage_t *
gui_ph_toolbar_ڈھونڈو_icon(vimmenu_T *menu)
{
    char_u بھرا_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = تہی;

    اگر (menu->icon_builtin == FALSE)
    {
	اگر (menu->iconfile != تہی)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_بھرو_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	اگر ((icon == تہی) &&
	    ((gui_ڈھونڈو_bitmap(menu->name, بھرا_pathname, "gif") == OK) ||
	      (gui_ڈھونڈو_bitmap(menu->name, بھرا_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_بھرو_icon(بھرا_pathname);

	اگر (icon != تہی)
	    واپس icon;
    }

    اگر (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_لمبائی(gui_ph_toolbar_images)))
	واپس gui_ph_toolbar_images[menu->iconidx];

    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_MENU)
    خالی
gui_mch_enable_menu(صحیح flag)
{
    اگر (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    ورنہ
	PtUnrealizeWidget(gui.vimMenuBar);
}

    خالی
gui_mch_سیٹ_menu_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ساکن خالی
gui_ph_position_menu(PtWidget_t *widget, صحیح ترجیح)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = Pt_Widget_بچہ_عقب(Pt_Widget_والد(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    جب (traverse != تہی)
    {
	PtGetResource(traverse, Pt_ARG_اشارہ_گر, &menu, 0);

	اگر (menu != تہی &&
		ترجیح < menu->ترجیح &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_داخل_کرو(widget, traverse, 1);
	    واپس;
	}

	traverse = Pt_Widget_Brother_اندر_Front(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    خالی
gui_mch_جوڑو_menu(vimmenu_T *menu, صحیح اشاریہ)
{
    vimmenu_T	*والد = menu->والد;
    char_u	*accel_کلید;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحیح	    n;
    PtArg_t دلائل[5];

    menu->submenu_id = menu->id = تہی;

    اگر (menu_is_menubar(menu->name))
    {

	accel_کلید = vim_strchr(menu->name, '&');
	اگر (accel_کلید != تہی)
	{
	    mnemonic_str[0] = accel_کلید[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_متن_STRING, menu->dname, 0);
	Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ACCEL_متن, menu->actext, 0);
	اگر (accel_کلید != تہی)
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ACCEL_کلید, mnemonic_str, 0);
	Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_اشارہ_گر, menu, 0);

	اگر (والد != تہی)
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_دایاں, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(والد == تہی) ? gui.vimMenuBar : والد->submenu_id,
		n, دلائل);

	Pt_جوڑو_Callback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	اگر (والد != تہی)
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_بچہ);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, دلائل);

	اگر (والد == تہی)
	{
	    Pt_جوڑو_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    اگر (menu->mnemonic != 0)
	    {
		Pt_جوڑو_Hotkey_Handler(gui.vim_کھڑکی, محفوظ_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->ترجیح);

	// Redraw menubar here instead of gui_mch_draw_menubar
	اگر (gui.menu_is_فعال)
	    PtRealizeWidget(menu->id);
    }
    ورنہ اگر (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vim_کھڑکی, 0, تہی);
	Pt_جوڑو_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    خالی
gui_mch_جوڑو_menu_شے(vimmenu_T *menu, صحیح اشاریہ)
{
    vimmenu_T	*والد = menu->والد;
    char_u	*accel_کلید;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحیح	    n;
    PtArg_t دلائل[13];

    n = 0;
    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_اشارہ_گر, menu, 0);

# ifdef FEAT_TOOLBAR
    اگر (menu_is_toolbar(والد->name))
    {
	اگر (menu_is_separator(menu->name))
	{
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_اندر, 0);
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_چوٹی_BOTTOM);
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_چوڑائی, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, دلائل);
	}
	ورنہ
	{
	    اگر (لڑی_تلاش((ثابت حرف *) p_toolbar, "text") != تہی)
	    {
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_متن_STRING, menu->dname, 0);
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_متن_FONT, "TextFont08", 0);
	    }
	    اگر ((لڑی_تلاش((ثابت حرف *) p_toolbar, "icons") != تہی) &&
		    (gui_ph_toolbar_images != تہی))
	    {
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_ڈھونڈو_icon(menu), 0);
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_LABEL_TYPE, Pt_متن_IMAGE, 0);
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_متن_IMAGE_SPACING, 0, 0);
	    }
	    اگر (لڑی_تلاش((ثابت حرف *) p_toolbar, "tooltips") != تہی)
	    {
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_LABEL_BALLOON,
			gui_ph_show_tooltip, 0);
		Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_LABEL_FLAGS,
			Pt_TRUE, Pt_SHOW_BALLOON);
	    }
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_MARGIN_اونچائی, 1, 0);
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_MARGIN_چوڑائی, 1, 0);
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_FLAGS, Pt_FALSE,
		    Pt_HIGHLIGHTED | Pt_GETS_FOCUS);
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_FILL_COLOR, Pg_TRANSPARENT, 0);
	    menu->id = PtCreateWidget(PtButton, gui.vimToolBar, n, دلائل);

	    Pt_جوڑو_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);
	}
	// Update toolbar if it's open
	اگر (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    ورنہ
# endif
	اگر (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, والد->submenu_id, n, دلائل);
    }
    ورنہ
    {
	accel_کلید = vim_strchr(menu->name, '&');
	اگر (accel_کلید != تہی)
	{
	    mnemonic_str[0] = accel_کلید[1];
	    mnemonic_str[1] = NUL;
	}

	Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_متن_STRING, menu->dname, 0);
	اگر (accel_کلید != تہی)
	    Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ACCEL_کلید, mnemonic_str,
		    0);

	Pt_سیٹ_Arg(&دلائل[ n++ ], Pt_ARG_ACCEL_متن, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, والد->submenu_id, n, دلائل);

	Pt_جوڑو_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_گروہ
	اگر (gui_ph_is_بفر_شے(menu, والد) == TRUE)
	{
	    Pt_جوڑو_Callback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_بفر_remove, menu);
	    gui_ph_pg_جوڑو_بفر(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->ترجیح);
}

    خالی
gui_mch_destroy_menu(vimmenu_T *menu)
{
    اگر (menu->submenu_id != تہی)
	PtDestroyWidget(menu->submenu_id);
    اگر (menu->id != تہی)
	PtDestroyWidget(menu->id);

    menu->submenu_id = تہی;
    menu->id = تہی;
}

    خالی
gui_mch_menu_grey(vimmenu_T *menu, صحیح grey)
{
    طویل    flags, نقاب, fields;

    اگر (menu->id == تہی)
	واپس;

    flags = PtWidgetFlags(menu->id);
    اگر (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(Pt_Widget_والد(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	نقاب = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    ورنہ
    {
	fields = Pt_TRUE;
	نقاب = Pt_BLOCKED | Pt_GHOST;
    }

    اگر (! grey)
	fields = ~fields;

    Pt_سیٹ_Resource(menu->id, Pt_ARG_FLAGS, fields,
	    نقاب);
}

    خالی
gui_mch_menu_hidden(vimmenu_T *menu, صحیح hidden)
{
    // TODO: [un]realize the widget?
}

    خالی
gui_mch_draw_menubar(خالی)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    خالی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    Pt_سیٹ_Resource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    خالی
gui_mch_toggle_tearoffs(صحیح enable)
{
    // No tearoffs yet
}

#اگر_ختم

#اگر defined(FEAT_TOOLBAR)
    خالی
gui_mch_show_toolbar(صحیح showit)
{
    اگر (showit)
	PtRealizeWidget(gui.vimToolBar);
    ورنہ
	PtUnrealizeWidget(gui.vimToolBar);
}
#اگر_ختم

////////////////////////////////////////////////////////////////////////////
// Fonts

    ساکن GuiFont
gui_ph_get_font(
	char_u	*font_name,
	int_u	font_flags,
	int_u	font_سائز_2,
	// Check whether the resulting font has the font flags and size that
	// was asked for
	int_u	enforce
	)
{
    char_u	    *font_لیبل;
    FontQueryInfo   info;
    int_u	    style;

    font_لیبل = alloc(MAX_FONT_لیبل);
    اگر (font_لیبل != تہی)
    {
	اگر (PfGenerateFontName(font_name, font_flags, font_سائز_2,
		    font_لیبل) != تہی)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    اگر (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    اگر (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    اگر (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_لیبل, &info);

	    اگر (info.سائز == 0)
		font_سائز_2 = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    اگر (font_سائز_2 == info.سائز &&
		    style == (info.style & style))
		واپس (GuiFont)font_لیبل;
	}
	vim_free(font_لیبل);
    }
    واپس تہی;
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

    ساکن صحیح
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_سائز_2)
{
    char_u  *mark;
    int_u   name_len, سائز;

    mark = vim_strchr(vim_font, ':');
    اگر (mark == تہی)
	name_len = STRLEN(vim_font);
    ورنہ
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    اگر (*font_name != تہی)
    {
	اگر (mark != تہی)
	{
	    جب (*mark != NUL && *mark++ == ':')
	    {
		انتخاب (محفوظ_tolower(*mark++))
		{
		    حالت 'a': *font_flags |= PF_STYLE_ANTIALIAS; توڑو;
		    حالت 'b': *font_flags |= PF_STYLE_BOLD; توڑو;
		    حالت 'i': *font_flags |= PF_STYLE_ITALIC; توڑو;

		    حالت 's':
			سائز = getdigits(&mark);
			// Restrict the size to some vague limits
			اگر (سائز < 1 || سائز > 100)
			    سائز = 8;

			*font_سائز_2 = سائز;
			توڑو;

		    طےشدہ:
			توڑو;
		}
	    }
	}
	واپس TRUE;
    }
    واپس FALSE;
}

    صحیح
gui_mch_init_font(char_u *vim_font_name, صحیح fontset)
{
    char_u  *font_لیبل;
    char_u  *font_name = تہی;
    int_u   font_flags = 0;
    int_u   font_سائز_2  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    اگر (vim_font_name == تہی)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    اگر (STRCMP(vim_font_name, "*") == 0)
    {
	font_لیبل = PtFontSelection(gui.vim_کھڑکی, تہی, تہی,
		"pcterm12", -1, PHFONT_FIXED, تہی);

	اگر (font_لیبل == تہی)
	    واپس FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_لیبل;

	PfQueryFontInfo(font_لیبل, &info);
	font_name = vim_strsave(info.font);
    }
    ورنہ
    {
	اگر (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_سائز_2) == FALSE)
	    واپس FAIL;

	font_لیبل = gui_ph_get_font(font_name, font_flags, font_سائز_2, 0);
	اگر (font_لیبل == تہی)
	{
	    vim_free(font_name);
	    واپس FAIL;
	}

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_لیبل;
    }

    gui_mch_free_font(gui.bold_font);
    gui.bold_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_BOLD,
	    font_سائز_2, PF_STYLE_BOLD);

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_ITALIC,
	    font_سائز_2, PF_STYLE_ITALIC);

    // This extent was brought to you by the letter 'g'
    Pf_Extent_متن(&extent, تہی, font_لیبل, "g", 1);

    gui.char_چوڑائی = extent.lr.x - extent.ul.x + 1;
    gui.char_اونچائی = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    واپس OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
gui_mch_adjust_charheight(خالی)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_اونچائی = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    واپس OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, صحیح report_error)
{
    char_u  *font_name;
    char_u  *font_لیبل;
    int_u   font_سائز_2 = 12;
    int_u   font_flags = 0;

    اگر (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_سائز_2) != FALSE)
    {
	font_لیبل = gui_ph_get_font(font_name, font_flags, font_سائز_2, -1);
	vim_free(font_name);

	اگر (font_لیبل != تہی)
	    واپس (GuiFont)font_لیبل;
    }

    اگر (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    واپس FAIL;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    اگر (name == تہی)
	واپس تہی;
    واپس vim_strsave(name);
}
#اگر_ختم

    خالی
gui_mch_سیٹ_font(GuiFont font)
{
    Pg_سیٹ_Font(font);
}

    خالی
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

