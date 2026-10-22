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

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
# include <photon/PxImage.h>
#جيڪڏهن_پورو

#إذا !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	صحيح
# define PhEvent_t	صحيح
# define Ph_نقطو_t	صحيح
# define PtWidget_t	صحيح
# define Pg_BLACK	0
# define PtCallbackF_t	صحيح
# define PtCallbackInfo_t صحيح
# define PhTile_t	صحيح
# define PtWidget_t	صحيح
# define PhImage_t	صحيح
#جيڪڏهن_پورو

#وصف_ڪريو RGB(r, g, b) PgRGB(r, g, b)

#وصف_ڪريو EVENT_بفر_سائيز حجم(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#وصف_ڪريو MOUSE_HIDE		TRUE
#وصف_ڪريو MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#وصف_رد USE_PANEL_گروهه

#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
ساكن حرف	*خالي_جايون_title = "    ";
ساكن حرف	**panel_titles = عدم;
ساكن ushort_t	انگ_panels = 0;
ساكن ننڍو pg_margin_کاٻو, pg_margin_ساڄو, pg_margin_چوٽي, pg_margin_bottom;
#جيڪڏهن_پورو

#وصف_ڪريو GUI_PH_MARGIN		4	// Size of the bevel

#وصف_ڪريو GUI_PH_MOUSE_TYPE		Ph_CURSOR_وجهو
ساكن PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ساكن Ph_نقطو_t    gui_ph_raw_وٿي;
ساكن PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ساكن PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ساكن ننڍو	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ساكن Ph_نقطو_t    abs_mouse;

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
    صحيح	    ڪنجي_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ڪنجيون[] =
{
    {Pk_مٿي,	    'k', 'u'},
    {Pk_هيٺ_ڏانهن,	    'k', 'd'},
    {Pk_کاٻو,	    'k', 'l'},
    {Pk_ساڄو,	    'k', 'r'},

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
    {Pk_پٺ_Space,  'k', 'b'},
    {Pk_وجهو,	    'k', 'I'},
    {Pk_Delete,	    'k', 'D'},
    {Pk_Home,	    'k', 'h'},
    {Pk_پڇاڙي,	    '@', '7'},
    {Pk_اڳواٽ,	    'k', 'P'},
    {Pk_ايندڙ,	    'k', 'N'},
    {Pk_Print,	    '%', '9'},

    {Pk_KP_جوڙيو,	    'K', '6'},
    {Pk_KP_ڪٽيو,'K', '7'},
    {Pk_KP_Divide,  'K', '8'},
    {Pk_KP_Multiply,'K', '9'},
    {Pk_KP_Enter,   'K', 'A'},

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //وجهو
    {Pk_KP_Decimal, KS_EXTRA, KE_KDEL}, // Delete

    {Pk_KP_4,	    'k', 'l'}, // Left
    {Pk_KP_6,	    'k', 'r'}, // Right
    {Pk_KP_8,	    'k', 'u'}, // Up
    {Pk_KP_2,	    'k', 'd'}, // Down

    {Pk_KP_7,	    'K', '1'}, // Home
    {Pk_KP_1,	    'K', '4'}, //پڇاڙي

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

ساكن PtCallbackF_t gui_ph_handle_دري_cb;

ساكن PtCallbackF_t gui_ph_handle_scrollbar;
ساكن PtCallbackF_t gui_ph_handle_keyboard;
ساكن PtCallbackF_t gui_ph_handle_mouse;
ساكن PtCallbackF_t gui_ph_handle_pulldown_menu;
ساكن PtCallbackF_t gui_ph_handle_menu;
ساكن PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ساكن PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ساكن PtCallbackF_t gui_ph_handle_menu_unrealized;

#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
ساكن فراغ gui_ph_get_panelgroup_margins(ننڍو*, ننڍو*, ننڍو*, ننڍو*);
#جيڪڏهن_پورو

ساكن فراغ gui_ph_draw_شروعات(فراغ);
ساكن فراغ gui_ph_draw_پڇاڙي(فراغ);

// Set the text for the balloon
ساكن PtWidget_t * gui_ph_show_tooltip(PtWidget_t *دري_2,
			     PtWidget_t *widget,
			     صحيح position,
			     حرف *متن,
			     حرف *font,
			     PgColor_t fill_color,
			     PgColor_t متن_color);

////////////////////////////////////////////////////////////////////////////

ساكن PtWidget_t * gui_ph_show_tooltip(PtWidget_t *دري_2,
			     PtWidget_t *widget,
			     صحيح position,
			     حرف *متن,
			     حرف *font,
			     PgColor_t fill_color,
			     PgColor_t متن_color)
{
    PtArg_t arg;
    vimmenu_T *menu;
    char_u  *tooltip;

    Pt_سيٽ_Arg(&arg, Pt_ARG_اشارو, &menu, 0);
    PtGetResources(widget, 1, &arg);

    // Override the text and position

    tooltip = متن;
    إذا (menu != عدم)
    {
	صحيح اشاريو = MENU_اشاريو_TIP;
	إذا (menu->لڙيون[ اشاريو ] != عدم)
	    tooltip = menu->لڙيون[ اشاريو ];
    }

    أرجع PtInflateBalloon(
	    دري_2,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_چوٽي,
	    tooltip,
	    font,
	    fill_color,
	    متن_color);
}

    ساكن فراغ
gui_ph_resize_container(فراغ)
{
    PhArea_t area;

    PtWidgetArea(gui.vim_دري, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    Pt_سيٽ_Resource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ساكن صحيح
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	فراغ *other,
	PtCallbackInfo_t *info)
{
    PtContainerCallback_t *sizes = info->cbdata;
    PtWidget_t		*container;
    Ph_نقطو_t		هيٺ_menu;
    int_u		اوچائي;

    اوچائي = sizes->new_dim.h;

    // Because vim treats the toolbar and menubar separately,
    // and here they're lumped together into a PtToolbarGroup,
    // we only need either menu_height or toolbar_height set at once
    إذا (gui.menu_is_سرگرم)
    {
	gui.menu_اوچائي = اوچائي;
	gui.toolbar_اوچائي = 0;
    }
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    وإلا
	gui.toolbar_اوچائي = اوچائي;
#جيڪڏهن_پورو

    هيٺ_menu.x = 0;
    هيٺ_menu.y = اوچائي;

#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
    container = gui.vim_Panel_گروهه;
#وإلا
    container = gui.vimContainer;
#جيڪڏهن_پورو

    Pt_سيٽ_Resource(container, Pt_ARG_POS, &هيٺ_menu, 0);

    gui_ph_resize_container();

#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
    gui_ph_get_panelgroup_margins(
	    &pg_margin_چوٽي, &pg_margin_bottom,
	    &pg_margin_کاٻو, &pg_margin_ساڄو);
#جيڪڏهن_پورو
    أرجع Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ساكن صحيح
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	فراغ *معلومات,
	PtCallbackInfo_t *info)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	Pt_سيٽ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	Pt_سيٽ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    فراغ		*معلومات,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_دري_cb(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    Ph_دري_Event_t *we = info->cbdata;
    ushort_t *ويڪر, *اوچائي;

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
		gui_mch_شروعات_blink();
	    }
	    وإلا
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    اكسر;

	حالة Ph_WM_RESIZE:
	    PtGetResource(gui.vim_دري, Pt_ARG_ويڪر, &ويڪر, 0);
	    PtGetResource(gui.vim_دري, Pt_ARG_اوچائي, &اوچائي, 0);
#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
	    ويڪر  -= (pg_margin_کاٻو + pg_margin_ساڄو);
	    اوچائي -= (pg_margin_چوٽي + pg_margin_bottom);
#جيڪڏهن_پورو
	    gui_resize_shell(*ويڪر, *اوچائي);
	    gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    Pt_پڇاڙي_Flux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    اكسر;

	افتراضي:
	    اكسر;
    }

    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_scrollbar(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    صحيح	    قدر, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) معلومات;
    إذا (sb != عدم)
    {
	قدر = scroll->position;
	بدل (scroll->action)
	{
	    حالة Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		اكسر;

	    حالة Pt_SCROLL_سيٽ:
		// FIXME: return straight away here?
		أرجع Pt_CONTINUE;
		اكسر;
	}

	gui_drag_scrollbar(sb, قدر, dragging);
    }
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_keyboard(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    Ph_ڪنجي_Event_t    *ڪنجي;
    بنانشان حرف   string[6];
    صحيح		    len, i;
    صحيح		    ch, modifiers;

    ڪنجي = Ph_Get_معلومات(info->event);

    ch = modifiers = len = 0;

    إذا (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    إذا (ڪنجي->ڪنجي_flags & Pk_KF_Compose)
	أرجع Pt_CONTINUE;

    إذا ((ڪنجي->ڪنجي_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_ڪنجي_هيٺ_ڏانهن(ڪنجي->ڪنجي_flags))
    {
#جيڪڏهن_وصف_ٿيل FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	إذا (((ڪنجي->ڪنجي_mods & Pk_KM_Alt) &&
		    !(ڪنجي->ڪنجي_mods & Pk_KM_سرڪايو) &&
		    !(ڪنجي->ڪنجي_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_سرگرم &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(ڪنجي->ڪنجي_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    أرجع Pt_CONTINUE;
	}
#جيڪڏهن_پورو

	لكل (i = 0; special_ڪنجيون[i].ڪنجي_sym != 0; i++)
	{
	    إذا (special_ڪنجيون[i].ڪنجي_sym == ڪنجي->ڪنجي_cap)
	    {
		len = 0;
		إذا (special_ڪنجيون[i].vim_code1 == NUL)
		    ch = special_ڪنجيون[i].vim_code0;
		وإلا
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    إذا (ڪنجي->ڪنجي_cap >= Pk_KP_Enter && ڪنجي->ڪنجي_cap <= Pk_KP_9
			    && (ڪنجي->ڪنجي_mods & Pk_KM_انگ_تالو))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = ڪنجي->ڪنجي_cap - 0xf080;
		    }
		    وإلا
			ch = منزل_SPECIAL(special_ڪنجيون[i].vim_code0,
				special_ڪنجيون[i].vim_code1);
		}
		اكسر;
	    }
	}

	إذا (ڪنجي->ڪنجي_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_نقاب_CTRL;
	إذا (ڪنجي->ڪنجي_mods & Pk_KM_Alt)
	    modifiers |= MOD_نقاب_ALT;
	إذا (ڪنجي->ڪنجي_mods & Pk_KM_سرڪايو)
	    modifiers |= MOD_نقاب_سرڪايو;

	// Is this not a special key?
	إذا (special_ڪنجيون[i].ڪنجي_sym == 0)
	{
	    ch = PhTo8859_1(ڪنجي);
	    إذا (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_ڪنجي_منزل_Mb(string, ڪنجي);
		إذا (len > 0)
		{
		    ساكن حرف buf[6];
		    صحيح src_ورتل, dst_made;
		    إذا (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_ورتل,
				buf,
				6,
				&dst_made);

			جوڙيو_منزل_داخلا_buf(buf, dst_made);
		    }
		    وإلا
		    {
			جوڙيو_منزل_داخلا_buf(string, len);
		    }

		    أرجع Pt_CONSUME;
		}
		len = 0;
		ch = ڪنجي->ڪنجي_cap;
		إذا (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    إذا (modifiers & MOD_نقاب_CTRL)
		    {
			modifiers &= ~MOD_نقاب_CTRL;

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
			    modifiers |= MOD_نقاب_CTRL;
		    }

		    إذا (modifiers & MOD_نقاب_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_نقاب_ALT;
		    }
		}
		وإلا
		{
		    أرجع Pt_CONTINUE;
		}
	    }
	    وإلا
		modifiers &= ~MOD_نقاب_سرڪايو;
	}

	ch = simplify_ڪنجي(ch, &modifiers);
	إذا (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	إذا (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_ٻيو(ch);
	    string[ len++ ] = K_ٽيون(ch);
	}
	وإلا
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    صحيح int_ch = check_for_مداخلت(ch, modifiers);

	    إذا (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_داخلا_buf();
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
	    جوڙيو_منزل_داخلا_buf(string, len);
	    أرجع Pt_CONSUME;
	}
    }

    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_mouse(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    Ph_اشارو_Event_t *اشارو;
    PhRect_t	     *pos;
    صحيح		     button = 0, repeated_click, modifiers = 0x0;
    ننڍو	     mouse_x, mouse_y;

    اشارو = Ph_Get_معلومات(info->event);
    pos = PhGetRects(info->event);

    gui_mch_mousehide(MOUSE_SHOW);

    /*
     * Coordinates need to be relative to the base window,
     * not relative to the vimTextArea widget
     */
    mouse_x = pos->ul.x + gui.border_ويڪر;
    mouse_y = pos->ul.y + gui.border_ويڪر;

    إذا (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	أرجع Pt_CONTINUE;
    }

    إذا (اشارو->ڪنجي_mods & Pk_KM_سرڪايو)
	modifiers |= MOUSE_سرڪايو;
    إذا (اشارو->ڪنجي_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    إذا (اشارو->ڪنجي_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    إذا (اشارو->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_کاٻو;

    إذا (اشارو->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_ساڄو;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = اشارو->pos.x;
	abs_mouse.y = اشارو->pos.y;
    }

    إذا (اشارو->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_وچ;

    // Catch a real release (not phantom or other releases
    إذا (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    إذا (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#إذا 0
    // Vim doesn't use button repeats
    إذا (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#جيڪڏهن_پورو

    // Don't do anything if it is one of the phantom mouse release events
    إذا ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (اشارو->click_ڳڻپ >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    أرجع Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ساكن صحيح
gui_ph_handle_focus(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    إذا (info->reason == Pt_CB_گم_FOCUS)
    {
	PtRemoveEventHandler(gui.vim_متن_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, عدم);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    وإلا
    {
	Pt_جوڙيو_Event_Handler(gui.vim_متن_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, عدم);
    }
    أرجع Pt_CONTINUE;
}

    ساكن فراغ
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    Ph_نقطو_t	وٿي;
    Ph_نقطو_t	translation;

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
    ٻاهريون_flush();
#جيڪڏهن_پورو

    Pt_Widget_وٿي(widget, &وٿي);
    Ph_Translate_نقطو(&وٿي, PtWidgetPos(gui.vim_متن_Area, عدم));

#إذا 1
    // Redraw individual damage regions
    إذا (damage->ايندڙ != عدم)
	damage = damage->ايندڙ;

    طالما (damage != عدم)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - وٿي.x, r->ul.y - وٿي.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->ايندڙ;
    }
#وإلا
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - وٿي.x, r->ul.y - وٿي.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#جيڪڏهن_پورو

    Pg_سيٽ_Translation(&translation, 0);
}

    ساكن صحيح
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	فراغ *معلومات,
	PtCallbackInfo_t *info)
{
    إذا (معلومات != عدم)
    {
	vimmenu_T *menu = (vimmenu_T *) معلومات;

	PtPositionMenu(menu->submenu_id, عدم);
	PtRealizeWidget(menu->submenu_id);
    }

    أرجع Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ساكن صحيح
gui_ph_handle_menu(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    إذا (معلومات != عدم)
    {
	vimmenu_T *menu = (vimmenu_T *) معلومات;
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
	فراغ *معلومات,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vim_متن_Area, عدم);
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_handle_دري_open(
	PtWidget_t *widget,
	فراغ *معلومات,
	PtCallbackInfo_t *info)
{
    gui_سيٽ_shellsize(FALSE, TRUE, RESIZE_BOTH);
    أرجع Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#وصف_ڪريو DRAW_شروعات  gui_ph_draw_شروعات()
#وصف_ڪريو DRAW_پڇاڙي    gui_ph_draw_پڇاڙي()

/*
 * TODO: Set a clipping rect?
 */
    ساكن فراغ
gui_ph_draw_شروعات(فراغ)
{
    PhGC_t *gc;

    gc = PgGetGC();
    Pg_سيٽ_Region(PtWidgetRid(Pt_ڳوليو_Disjoint(gui.vim_متن_Area)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    Pt_Widget_وٿي(gui.vim_متن_Area, &gui_ph_raw_وٿي);
    Ph_Translate_نقطو(&gui_ph_raw_وٿي, PtWidgetPos(gui.vim_متن_Area, عدم));

    Pg_سيٽ_Translation(&gui_ph_raw_وٿي, Pg_RELATIVE);
}

    ساكن فراغ
gui_ph_draw_پڇاڙي(فراغ)
{
    gui_ph_raw_وٿي.x = -gui_ph_raw_وٿي.x;
    gui_ph_raw_وٿي.y = -gui_ph_raw_وٿي.y;
    Pg_سيٽ_Translation(&gui_ph_raw_وٿي, Pg_RELATIVE);
}

#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
    ساكن vimmenu_T *
gui_ph_ڳوليو_بفر_شي(char_u *name)
{
    vimmenu_T *چوٽي_level = پاڙ_menu;
    vimmenu_T *شيون = عدم;

    طالما (چوٽي_level != عدم &&
	    (STRCMP(چوٽي_level->dname, "Buffers") != 0))
	چوٽي_level = چوٽي_level->ايندڙ;

    إذا (چوٽي_level != عدم)
    {
	شيون = چوٽي_level->children;

	طالما (شيون != عدم &&
		(STRCMP(شيون->dname, name) != 0))
	    شيون = شيون->ايندڙ;
    }
    أرجع شيون;
}

    ساكن فراغ
gui_ph_pg_سيٽ_بفر_انگ(int_u buf_انگ)
{
    صحيح i;
    حرف ڳولا[16];
    حرف *mark;

    إذا (gui.vim_متن_Area == عدم || buf_انگ == 0)
	أرجع;

    ڳولا[0] = '(';
    ultoa(buf_انگ, &ڳولا[1], 10);
    STRCAT(ڳولا, ")");

    لكل (i = 0; i < انگ_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	إذا (mark != عدم && STRCMP(mark, ڳولا) == 0)
	{
	    Pt_سيٽ_Resource(gui.vim_Panel_گروهه, Pt_ARG_PG_CURRENT_اشاريو,
		    i, 0);
	}
    }
}

    ساكن صحيح
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	فراغ *معلومات,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    Pt_Panel_گروهه_Callback_t *panel;

    إذا (info->event != عدم)
    {
	panel = info->cbdata;
	إذا (panel->new_panel != عدم)
	{
	    menu = gui_ph_ڳوليو_بفر_شي(panel->new_panel);
	    إذا (menu)
		gui_menu_cb(menu);
	}
    }
    أرجع Pt_CONTINUE;
}

    ساكن فراغ
gui_ph_get_panelgroup_margins(
	ننڍو *چوٽي,
	ننڍو *bottom,
	ننڍو *کاٻو,
	ننڍو *ساڄو)
{
    بنانشان ننڍو abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ثابت بنانشان ننڍو *margin_چوٽي, *margin_bottom;
    ثابت بنانشان ننڍو *margin_کاٻو, *margin_ساڄو;

    PtGetAbsPosition(gui.vim_متن_Area, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vim_Panel_گروهه, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vim_Panel_گروهه, Pt_ARG_MARGIN_ساڄو, &margin_ساڄو, 0);
    PtGetResource(gui.vim_Panel_گروهه, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *چوٽي    = abs_raw_y;
    *bottom = *margin_bottom;

    *کاٻو  = abs_raw_x;
    *ساڄو = *margin_ساڄو;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ساكن صحيح
gui_ph_is_بفر_شي(vimmenu_T *menu, vimmenu_T *والد)
{
    حرف *mark;

    إذا (STRCMP(والد->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	إذا (mark != عدم)
	{
	    mark++;
	    طالما (محفوظ_isdigit(*mark))
		mark++;

	    إذا (*mark == ')')
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

    ساكن فراغ
gui_ph_pg_جوڙيو_بفر(حرف *name)
{
    حرف **new_titles = عدم;

    new_titles = ALLOC_MULT(حرف *, (انگ_panels + 1));
    إذا (new_titles == عدم)
	أرجع;

    إذا (انگ_panels > 0)
	يادگيري_نقل(new_titles, panel_titles, انگ_panels * حجم(حرف **));

    new_titles[ انگ_panels++ ] = name;

    Pt_سيٽ_Resource(gui.vim_Panel_گروهه, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    انگ_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ساكن فراغ
gui_ph_pg_remove_بفر(حرف *name)
{
    صحيح i;
    حرف **new_titles = عدم;

    // If there is only 1 panel, we just use the temporary place holder
    إذا (انگ_panels > 1)
    {
	new_titles = ALLOC_MULT(حرف *, انگ_panels - 1);
	إذا (new_titles != عدم)
	{
	    حرف **s = new_titles;
	    // Copy all the titles except the one we're removing
	    لكل (i = 0; i < انگ_panels; i++)
	    {
		إذا (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    انگ_panels--;

	    Pt_سيٽ_Resource(gui.vim_Panel_گروهه, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    انگ_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    وإلا
    {
	انگ_panels--;
	Pt_سيٽ_Resource(gui.vim_Panel_گروهه, Pt_ARG_PG_PANEL_TITLES, &خالي_جايون_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ساكن صحيح
gui_ph_handle_بفر_remove(
	PtWidget_t *widget,
	فراغ *معلومات,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    إذا (معلومات != عدم)
    {
	menu = (vimmenu_T *) معلومات;
	gui_ph_pg_remove_بفر(menu->dname);
    }

    أرجع Pt_CONTINUE;
}
#جيڪڏهن_پورو

    ساكن صحيح
gui_ph_pane_resize(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    إذا (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	Pt_شروعات_Flux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    أرجع Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    فراغ
gui_ph_encoding_بدليو(صحيح new_encoding)
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

    لكل (i = 0; i < ARRAY_ڊيگهائي(charsets); i++)
    {
	إذا (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = Px_Translate_سيٽ(charset_translate, charset);
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
    PtArg_t دليلون[10];
    صحيح	    flags = 0, n = 0;

    PhDim_t	دري_سائيز = {100, 100}; // Arbitrary values
    Ph_نقطو_t	pos = {0, 0};

    gui.event_بفر = alloc(EVENT_بفر_سائيز);
    إذا (gui.event_بفر == عدم)
	أرجع FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = Px_Translate_سيٽ(عدم, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_وٿي = gui.border_ويڪر = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_دري_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_دري_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_DIM, &دري_سائيز, 0);
    gui.vim_دري = PtCreateWidget(Pt_دري, عدم, n, دليلون);
    إذا (gui.vim_دري == عدم)
	أرجع FAIL;

    Pt_جوڙيو_Callback(gui.vim_دري, Pt_CB_دري, gui_ph_handle_دري_cb, عدم);
    Pt_جوڙيو_Callback(gui.vim_دري, Pt_CB_دري_OPENING,
	    gui_ph_handle_دري_open, عدم);

    n = 0;
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_DIM, &دري_سائيز, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_POS, &pos, 0);

#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
    // Put in a temporary place holder title
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_PG_PANEL_TITLES, &خالي_جايون_title, 1);

    gui.vim_Panel_گروهه = PtCreateWidget(Pt_Panel_گروهه, gui.vim_دري, n, دليلون);
    إذا (gui.vim_Panel_گروهه == عدم)
	أرجع FAIL;

    Pt_جوڙيو_Callback(gui.vim_Panel_گروهه, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, عدم);
#وإلا
    // Turn off all edge decorations
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_BEVEL_ويڪر, 0, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_MARGIN_ويڪر, 0, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_MARGIN_اوچائي, 0, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vim_دري, n, دليلون);
    إذا (gui.vimContainer == عدم)
	أرجع FAIL;

    Pt_جوڙيو_Callback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, عدم);
#جيڪڏهن_پورو

    // Size for the text area is set in gui_mch_set_text_area_pos
    n = 0;

    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_RAW_DRAW_F, gui_ph_handle_raw_draw, 1);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_BEVEL_ويڪر, GUI_PH_MARGIN, 0);
    /*
     * Using focus render also causes the whole widget to be redrawn
     * whenever it changes focus, which is very annoying :p
     */
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_FLAGS, Pt_TRUE,
	    Pt_GETS_FOCUS | Pt_HIGHLIGHTED);
#جيڪڏهن_وصف_نه_ٿيل FEAT_MOUSESHAPE
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_CURSOR_TYPE, GUI_PH_MOUSE_TYPE, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_CURSOR_COLOR, gui_ph_mouse_color, 0);
#جيڪڏهن_پورو

    gui.vim_متن_Area = PtCreateWidget(PtRaw, Pt_DFLT_والد, n, دليلون);
    إذا (gui.vim_متن_Area == عدم)
	أرجع FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    Pt_جوڙيو_Event_Handler(gui.vim_متن_Area,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, عدم);
    Pt_جوڙيو_Event_Handler(gui.vim_متن_Area, Ph_EV_ڪنجي,
	    gui_ph_handle_keyboard, عدم);
    Pt_جوڙيو_Callback(gui.vim_متن_Area, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, عدم);
    Pt_جوڙيو_Callback(gui.vim_متن_Area, Pt_CB_گم_FOCUS,
	    gui_ph_handle_focus, عدم);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vim_دري, 0, عدم);
    إذا (gui_ph_timer_cursor == عدم)
	أرجع FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vim_دري, 0, عدم);
    إذا (gui_ph_timer_timeout == عدم)
	أرجع FAIL;

    Pt_جوڙيو_Callback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, عدم);
    Pt_جوڙيو_Callback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, عدم);

#جيڪڏهن_وصف_ٿيل FEAT_MENU
    n = 0;
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ويڪر, دري_سائيز.w, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_کاٻو_ساڄو,
	    Pt_IS_ANCHORED);
    gui.vim_Tool_Bar_گروهه = PtCreateWidget(Pt_Toolbar_گروهه, gui.vim_دري,
	    n, دليلون);
    إذا (gui.vim_Tool_Bar_گروهه == عدم)
	أرجع FAIL;

    Pt_جوڙيو_Callback(gui.vim_Tool_Bar_گروهه, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, عدم);

    n = 0;
    flags = 0;
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ويڪر, دري_سائيز.w, 0);
    إذا (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vim_Tool_Bar_گروهه, n, دليلون);
    إذا (gui.vimMenuBar == عدم)
	أرجع FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_کاٻو_ساڄو |Pt_چوٽي_ANCHORED_چوٽي, Pt_IS_ANCHORED);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ويڪر, دري_سائيز.w, 0);

    flags = Pt_GETS_FOCUS;
    إذا (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vim_Tool_Bar_گروهه, n, دليلون);
    إذا (gui.vimToolBar == عدم)
	أرجع FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#جيڪڏهن_پورو

    أرجع OK;
}

    صحيح
gui_mch_init_check(فراغ)
{
    أرجع (is_photon_موجود == TRUE) ? OK : FAIL;
}

    صحيح
gui_mch_open(فراغ)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.پٺ_pixel =  Pg_WHITE;

    سيٽ_عام_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_پٺ_pixel = gui.پٺ_pixel;

    highlight_gui_started();

    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_سيٽ_winpos(gui_win_x, gui_win_y);

    أرجع (PtRealizeWidget(gui.vim_دري) == 0) ? OK : FAIL;
}

    فراغ
gui_mch_exit(صحيح rc)
{
    PtDestroyWidget(gui.vim_دري);

    Px_Translate_سيٽ(charset_translate, عدم);

    vim_free(gui.event_بفر);

#جيڪڏهن_وصف_ٿيل USE_PANEL_GROUPS
    vim_free(panel_titles);
#جيڪڏهن_پورو
}

////////////////////////////////////////////////////////////////////////////
// events

/*
 * When no events are available, photon will call this function, working is
 * set to FALSE, and the gui_mch_update loop will exit.
 */
    ساكن صحيح
exit_gui_mch_update(فراغ *معلومات)
{
    *(صحيح *)معلومات = FALSE;
    أرجع Pt_پڇاڙي;
}

    فراغ
gui_mch_update(فراغ)
{
    صحيح working = TRUE;

    Pt_App_جوڙيو_Work_Proc(عدم, exit_gui_mch_update, &working);
    طالما ((working == TRUE) && !vim_is_داخلا_buf_ڀريل())
	PtProcessEvent();
}

    صحيح
gui_mch_wait_for_chars(ڊگهو wtime)
{
    is_timeout = FALSE;

    إذا (wtime >= 0)
	Pt_سيٽ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    طالما (1)
    {
	PtProcessEvent();
	إذا (داخلا_موجود())
	{
	    Pt_سيٽ_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
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
    char_u  *default_رستو;
    char_u  *open_متن = عدم;

    flags = 0;
    CLEAR_ميدان(file);

    default_رستو = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    إذا (default_رستو != عدم)
    {
	إذا (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_متن = "&Save";
	}

	// combine the directory and filename into a single path
	إذا (initdir == عدم || *initdir == NUL)
	{
	    mch_dirname(default_رستو, MAXPATHL);
	    initdir = default_رستو;
	}
	وإلا
	{
	    STRCPY(default_رستو, initdir);
	    initdir = default_رستو;
	}

	إذا (default_name != عدم)
	{
	    إذا (default_رستو[ STRLEN(default_رستو) - 1 ] != '/')
		STRCAT(default_رستو, "/");

	    STRCAT(default_رستو, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vim_دري,
		عدم,
		title,
		default_رستو,
		عدم,
		open_متن,
		عدم,
		عدم,
		&file,
		flags);

	vim_free(default_رستو);

	إذا (file.ret == Pt_FSDIALOG_BTN1)
	    أرجع vim_strsave(file.رستو);
    }
    أرجع عدم;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_DIALOG)
ساكن PtWidget_t *gui_ph_dialog_متن = عدم;

    ساكن صحيح
gui_ph_dialog_close(صحيح button, فراغ *معلومات)
{
    PtModalCtrl_t *modal_ctrl = معلومات;
    char_u *dialog_متن, *vim_متن;

    إذا (gui_ph_dialog_متن != عدم)
    {
	PtGetResource(gui_ph_dialog_متن, Pt_ARG_متن_STRING, &dialog_متن, 0);
	PtGetResource(gui_ph_dialog_متن, Pt_ARG_اشارو, &vim_متن, 0);
	STRNCPY(vim_متن, dialog_متن, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (فراغ *) button);

    أرجع Pt_TRUE;
}

    ساكن صحيح
gui_ph_dialog_متن_enter(
    PtWidget_t		*widget,
    فراغ		*معلومات,
    PtCallbackInfo_t	*info)
{
    إذا (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, معلومات);
    أرجع Pt_CONTINUE;
}

    ساكن صحيح
gui_ph_dialog_esc(PtWidget_t *widget, فراغ *معلومات, PtCallbackInfo_t *info)
{
    Ph_ڪنجي_Event_t *ڪنجي;

    ڪنجي = Ph_Get_معلومات(info->event);
    إذا ((ڪنجي->ڪنجي_flags & Pk_KF_Cap_Valid) && (ڪنجي->ڪنجي_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, معلومات);
	أرجع Pt_CONSUME;
    }
    أرجع Pt_PROCESS;
}

    صحيح
gui_mch_dialog(
	صحيح	type,
	char_u	*title,
	char_u	*پيغام,
	char_u	*buttons,
	صحيح	default_button,
	char_u	*textfield,
	صحيح	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    صحيح		button_ڳڻپ;
    صحيح		i, len;
    صحيح		dialog_نتيجو = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_ڳڻپ = len = i = 0;

    إذا (buttons == عدم || *buttons == NUL)
	أرجع -1;

    // There is one less separator than buttons, so bump up the button count
    button_ڳڻپ = 1;

    // Count string length and number of separators
    لكل (str = buttons; *str; str++)
    {
	len++;
	إذا (*str == DLG_BUTTON_SEP)
	    button_ڳڻپ++;
    }

    إذا (title == عدم)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_ڳڻپ);
    إذا (buttons_copy != عدم && button_array != عدم)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	لكل (i = 0; i < button_ڳڻپ; i++)
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
	dialog_نتيجو = PtAlert(
		gui.vim_دري, عدم,
		title,
		عدم,
		پيغام, عدم,
		button_ڳڻپ, (ثابت حرف **) button_array, عدم,
		default_button, 0, Pt_MODAL);
# وإلا
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    صحيح n;
	    PtArg_t دليلون[5];
	    PtWidget_t *dialog, *pane;
	    PtModalCtrl_t modal_ctrl;
	    PtDialogInfo_t di;

	    CLEAR_ميدان(di);
	    CLEAR_ميدان(modal_ctrl);

	    n = 0;
	    Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_گروهه_صفون_کانن_ڳڻپ, 0, 0);
	    Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_ويڪر, 350, 0);
	    Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_گروهه_ORIENTATION,
		    Pt_گروهه_VERTICAL, 0);
	    Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_گروهه_FLAGS,
		    Pt_TRUE, Pt_گروهه_NO_ڪنجيون | Pt_گروهه_STRETCH_HORIZONTAL);
	    Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(Pt_گروهه, عدم, n, دليلون);

	    n = 0;
	    Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_متن_STRING, پيغام, 0);
	    PtCreateWidget(PtLabel, pane, n, دليلون);

	    إذا (textfield != عدم)
	    {
		n = 0;
		Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_MAX_ڊيگهائي, IOSIZE - 1, 0);
		Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_متن_STRING, textfield, 0);
		Pt_سيٽ_Arg(&دليلون[n++], Pt_ARG_اشارو, textfield, 0);
		gui_ph_dialog_متن = PtCreateWidget(Pt_متن, pane, n, دليلون);
		Pt_جوڙيو_Callback(gui_ph_dialog_متن, Pt_CB_ACTIVATE,
			gui_ph_dialog_متن_enter, &modal_ctrl);
	    }

	    di.والد = gui.vim_دري;
	    di.pane = pane;
	    di.title = title;
	    di.buttons = (ثابت حرف **) button_array;
	    di.nbtns = button_ڳڻپ;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_ڳڻپ;
	    di.callback = gui_ph_dialog_close;
	    di.معلومات = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    Pt_جوڙيو_Filter_Callback(dialog, Ph_EV_ڪنجي,
		    gui_ph_dialog_esc, &modal_ctrl);

	    إذا (gui_ph_dialog_متن != عدم)
		PtGiveFocus(gui_ph_dialog_متن, عدم);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_نتيجو = (صحيح) Pt_Modal_بلاڪ(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_متن = عدم;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    أرجع dialog_نتيجو;
}
#جيڪڏهن_پورو
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    Ph_نقطو_t *pos;

    pos = PtWidgetPos(gui.vim_دري, عدم);

    *x = pos->x;
    *y = pos->y;

    أرجع OK;
}

    فراغ
gui_mch_سيٽ_winpos(صحيح x, صحيح y)
{
    Ph_نقطو_t pos = { x, y };

    Pt_سيٽ_Resource(gui.vim_دري, Pt_ARG_POS, &pos, 0);
}

    فراغ
gui_mch_سيٽ_shellsize(صحيح ويڪر, صحيح اوچائي,
	صحيح min_ويڪر, صحيح min_اوچائي, صحيح بنياد_ويڪر, صحيح بنياد_اوچائي,
	صحيح direction)
{
    PhDim_t دري_سائيز = { ويڪر, اوچائي };
    PhDim_t min_سائيز = { min_ويڪر, min_اوچائي };

#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
    دري_سائيز.w += pg_margin_کاٻو + pg_margin_ساڄو;
    دري_سائيز.h += pg_margin_چوٽي + pg_margin_bottom;
#جيڪڏهن_پورو

    Pt_سيٽ_Resource(gui.vim_دري, Pt_ARG_MINIMUM_DIM, &min_سائيز, 0);
    Pt_سيٽ_Resource(gui.vim_دري, Pt_ARG_DIM, &دري_سائيز, 0);

    إذا (! PtWidgetIsRealized(gui.vim_دري))
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

    Ph_دري_Query_Visible(Ph_QUERY_WORKSPACE, 0,
	    Ph_داخلا_گروهه(عدم), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    فراغ
gui_mch_iconify(فراغ)
{
    Ph_دري_Event_t event;

    CLEAR_ميدان(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vim_دري);
    Pt_Forward_دري_Event(&event);
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_سيٽ_foreground(فراغ)
{
    Ph_دري_Event_t event;

    CLEAR_ميدان(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vim_دري);
    Pt_Forward_دري_Event(&event);
}
#جيڪڏهن_پورو

    فراغ
gui_mch_settitle(char_u *title,	char_u *icon)
{
#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
    gui_ph_pg_سيٽ_بفر_انگ(curwin->w_بفر->b_fnum);
#جيڪڏهن_پورو
    Pt_سيٽ_Resource(gui.vim_دري, Pt_ARG_دري_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    فراغ
gui_mch_سيٽ_scrollbar_thumb(scrollbar_T *sb, صحيح val, صحيح سائيز, صحيح max)
{
    صحيح	    n = 0;
    PtArg_t دليلون[3];

    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_وڌ_کان_وڌ, max, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_SLIDER_سائيز, سائيز, 0);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_GAUGE_قدر, val, 0);
    Pt_سيٽ_Resources(sb->id, n, دليلون);
}

    فراغ
gui_mch_سيٽ_scrollbar_pos(scrollbar_T *sb, صحيح x, صحيح y, صحيح w, صحيح h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    Pt_سيٽ_Resource(sb->id, Pt_ARG_AREA, &area, 0);
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
    PtArg_t دليلون[4];

    /*
     * Stop the scrollbar from being realized when the parent
     * is realized, so it can be explicitly realized by vim.
     *
     * Also, don't let the scrollbar get focus
     */
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE,
	    Pt_DELAY_REALIZE | Pt_GETS_FOCUS);
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_SCROLLBAR_FLAGS, Pt_SCROLLBAR_SHOW_ARROWS, 0);
#إذا 0
    // Don't need this anchoring for the scrollbars
    إذا (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_کاٻو_ANCHORED_کاٻو | Pt_ساڄو_ANCHORED_ساڄو;
    }
    وإلا
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_چوٽي_ANCHORED_چوٽي;
	إذا (sb->wp != عدم)
	{
	    إذا (sb == &sb->wp->w_scrollbars[ SBAR_کاٻو ])
		anchor_flags |= Pt_کاٻو_ANCHORED_کاٻو;
	    وإلا
		anchor_flags |= Pt_ساڄو_ANCHORED_ساڄو;
	}
    }
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#جيڪڏهن_پورو
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#جيڪڏهن_وصف_ٿيل USE_PANEL_گروهه
    sb->id = PtCreateWidget(PtScrollbar, gui.vim_Panel_گروهه, n, دليلون);
#وإلا
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, دليلون);
#جيڪڏهن_پورو

    Pt_جوڙيو_Callback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
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
ساكن صحيح آخري_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساكن صحيح mshape_ids[] =
{
    Ph_CURSOR_اشارو,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_وجهو,		// beam
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
    Ph_CURSOR_QUESTION_نقطو,	// question
    Ph_CURSOR_اشارو,		// right-arrow
    Ph_CURSOR_اشارو,		// up-arrow
    Ph_CURSOR_اشارو		// last one
};

    فراغ
mch_سيٽ_mouse_shape(صحيح shape)
{
    صحيح	    id;

    إذا (!gui.اندريون_use)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.اشارو_hidden)
	Pt_سيٽ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_اشارو;
	وإلا
	    id = mshape_ids[shape];

	Pt_سيٽ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    إذا (shape != MSHAPE_HIDE)
	آخري_shape = shape;
}
#جيڪڏهن_پورو

    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.اشارو_hidden == hide)
	أرجع;

    gui.اشارو_hidden = hide;
#جيڪڏهن_وصف_ٿيل FEAT_MOUSESHAPE
    إذا (hide)
	Pt_سيٽ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    وإلا
	mch_سيٽ_mouse_shape(آخري_shape);
#وإلا
    Pt_سيٽ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#جيڪڏهن_پورو
}

    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    PhCursorInfo_t info;
    ننڍو ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_داخلا_گروهه(عدم), &info);
    PtGetAbsPosition(gui.vim_متن_Area , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    ننڍو abs_x, abs_y;

    PtGetAbsPosition(gui.vim_متن_Area, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_داخلا_گروهه(عدم), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    أرجع (guicolor_T)(PgRGB(Pg_Red_قدر(pixel),
				     Pg_Green_قدر(pixel), Pg_Blue_قدر(pixel)));
}

    فراغ
gui_mch_new_colors(فراغ)
{
#إذا 0 // Don't bother changing the cursor colour
    ننڍو color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.پٺ_pixel);

    إذا (مطلق_قدر(color_diff) < 64)
    {
	ننڍو r, g, b;
	// not a great algorithm...
	r = Pg_Red_قدر(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_قدر(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_قدر(gui_ph_mouse_color) ^ 255;

# ifndef FEAT_MOUSESHAPE
	gui_ph_mouse_color = PgRGB(r, g, b);
	Pt_سيٽ_Resource(gui.vim_متن_Area, Pt_ARG_CURSOR_COLOR,
		gui_ph_mouse_color, 0);
# endif
    }
#جيڪڏهن_پورو

    Pt_سيٽ_Resource(gui.vim_متن_Area, Pt_ARG_FILL_COLOR, gui.پٺ_pixel, 0);
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
gui_mch_سيٽ_fg_color(guicolor_T color)
{
    Pg_سيٽ_متن_Color(color);
}

    فراغ
gui_mch_سيٽ_bg_color(guicolor_T color)
{
    Pg_سيٽ_Fill_Color(color);
}

    فراغ
gui_mch_سيٽ_sp_color(guicolor_T color)
{
}

    فراغ
gui_mch_invert_rectangle(صحيح صف, صحيح کانو, صحيح nr, صحيح nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(کانو);
    rect.ul.y = FILL_Y(صف);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_ويڪر;
    rect.lr.y = rect.ul.y + nr * gui.char_اوچائي;
    إذا (nc > 0)
	rect.lr.x -= 1;
    إذا (nr > 0)
	rect.lr.y -= 1;

    DRAW_شروعات;
    Pg_سيٽ_Draw_گهڻائي(Pg_Draw_گهڻائي_DSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    Pg_سيٽ_Draw_گهڻائي(Pg_Draw_گهڻائي_SRCCOPY);
    DRAW_پڇاڙي;
}

    فراغ
gui_mch_clear_بلاڪ(صحيح row1, صحيح col1, صحيح row2, صحيح col2)
{
    PhRect_t بلاڪ = {
	{ FILL_X(col1), FILL_Y(row1) },
	{ FILL_X(col2 + 1) - 1, FILL_Y(row2 + 1) - 1}
    };

    DRAW_شروعات;
    gui_mch_سيٽ_bg_color(gui.پٺ_pixel);
    PgDrawRect(&بلاڪ, Pg_DRAW_FILL);
    DRAW_پڇاڙي;
}

    فراغ
gui_mch_clear_all(فراغ)
{
    PhRect_t متن_rect = {
	{ gui.border_ويڪر, gui.border_ويڪر },
	{ Columns * gui.char_ويڪر + gui.border_ويڪر - 1 ,
	    صفون * gui.char_اوچائي + gui.border_ويڪر - 1 }
    };

    إذا (is_ignore_draw == TRUE)
	أرجع;

    DRAW_شروعات;
    gui_mch_سيٽ_bg_color(gui.پٺ_pixel);
    PgDrawRect(&متن_rect, Pg_DRAW_FILL);
    DRAW_پڇاڙي;
}

    فراغ
gui_mch_delete_سٽون(صحيح صف, صحيح انگ_سٽون)
{
    PhRect_t    rect;
    Ph_نقطو_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_کاٻو);
    rect.ul.y = FILL_Y(صف + انگ_سٽون);

    rect.lr.x = FILL_X(gui.scroll_region_ساڄو + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    Pt_Widget_وٿي(gui.vim_متن_Area, &gui_ph_raw_وٿي);
    Ph_Translate_نقطو(&gui_ph_raw_وٿي, PtWidgetPos(gui.vim_متن_Area, عدم));
    PhTranslateRect(&rect, &gui_ph_raw_وٿي);

    delta.x = 0;
    delta.y = -انگ_سٽون * gui.char_اوچائي;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ڳوليو_Disjoint(gui.vim_متن_Area)), &rect, &delta);

    gui_clear_بلاڪ(
	gui.scroll_region_bot - انگ_سٽون + 1,
	gui.scroll_region_کاٻو,
	gui.scroll_region_bot,
	gui.scroll_region_ساڄو);
}

    فراغ
gui_mch_وجهو_سٽون(صحيح صف, صحيح انگ_سٽون)
{
    PhRect_t    rect;
    Ph_نقطو_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_کاٻو);
    rect.ul.y = FILL_Y(صف);

    rect.lr.x = FILL_X(gui.scroll_region_ساڄو + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - انگ_سٽون + 1) - 1;

    Pt_Widget_وٿي(gui.vim_متن_Area, &gui_ph_raw_وٿي);
    Ph_Translate_نقطو(&gui_ph_raw_وٿي, PtWidgetPos(gui.vim_متن_Area, عدم));
    PhTranslateRect(&rect, &gui_ph_raw_وٿي);

    delta.x = 0;
    delta.y = انگ_سٽون * gui.char_اوچائي;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_ڳوليو_Disjoint(gui.vim_متن_Area)) , &rect, &delta);

    gui_clear_بلاڪ(صف, gui.scroll_region_کاٻو,
	    صف + انگ_سٽون - 1, gui.scroll_region_ساڄو);
}

    فراغ
gui_mch_draw_string(صحيح صف, صحيح کانو, char_u *s, صحيح len, صحيح flags)
{
    ساكن حرف *utf8_بفر = عدم;
    ساكن صحيح	utf8_len = 0;

    Ph_نقطو_t	pos = { متن_X(کانو), متن_Y(صف) };
    PhRect_t	rect;

    إذا (is_ignore_draw == TRUE)
	أرجع;

    DRAW_شروعات;

    إذا (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(کانو), FILL_Y(صف),
		FILL_X(کانو + len) - 1, FILL_Y(صف + 1) - 1,
		Pg_DRAW_FILL);
    }

    إذا (flags & DRAW_UNDERL)
	Pg_سيٽ_Underline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    إذا (charset_translate != عدم && enc_utf8 == 0)
    {
	صحيح src_ورتل, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	إذا (utf8_len < len)
	{
	    حرف *new_بفر = realloc(utf8_بفر, len * MB_LEN_MAX);

	    إذا (new_بفر == عدم)
		أرجع;
	    utf8_بفر = new_بفر;
	    utf8_len = len;
	}

	Px_Translate_منزل_UTF(
		charset_translate,
		s,
		len,
		&src_ورتل,
		utf8_بفر,
		utf8_len,
		&dst_made);
	s = utf8_بفر;
	len = dst_made;
    }

    Pg_Draw_متن(s, len, &pos, 0);

    إذا (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(کانو) + 1;
	rect.ul.y = FILL_Y(صف);
	rect.lr.x = FILL_X(کانو + len) - 1;
	rect.lr.y = FILL_Y(صف + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#إذا 0
	pos.x++;

	Pg_سيٽ_User_Clip(&rect);
	Pg_Draw_متن(s, len, &pos, 0);
	Pg_سيٽ_User_Clip(عدم);
#وإلا
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	Pg_Draw_متن_Area(s, len, &rect, Pg_متن_BOTTOM);
#جيڪڏهن_پورو
    }

    إذا (flags & DRAW_UNDERL)
	Pg_سيٽ_Underline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_پڇاڙي;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    PhRect_t r;

    // FIXME: Double width characters

    r.ul.x = FILL_X(gui.کانو);
    r.ul.y = FILL_Y(gui.صف);
    r.lr.x = r.ul.x + gui.char_ويڪر - 1;
    r.lr.y = r.ul.y + gui.char_اوچائي - 1;

    DRAW_شروعات;
    Pg_سيٽ_Stroke_Color(color);
    PgDrawRect(&r, Pg_DRAW_STROKE);
    DRAW_پڇاڙي;
}

    فراغ
gui_mch_draw_part_cursor(صحيح w, صحيح h, guicolor_T color)
{
    PhRect_t r;

    r.ul.x = FILL_X(gui.کانو);
    r.ul.y = FILL_Y(gui.صف) + gui.char_اوچائي - h;
    r.lr.x = r.ul.x + w - 1;
    r.lr.y = r.ul.y + h - 1;

    DRAW_شروعات;
    gui_mch_سيٽ_bg_color(color);
    PgDrawRect(&r, Pg_DRAW_FILL);
    DRAW_پڇاڙي;
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
gui_mch_سيٽ_blinking(ڊگهو wait, ڊگهو on, ڊگهو off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    فراغ
gui_mch_شروعات_blink(فراغ)
{
    // Only turn on the timer on if none of the times are zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.اندريون_focus)
    {
	Pt_سيٽ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    Pt_سيٽ_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

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
    Pg_سيٽ_Fill_XORColor(Pg_BLACK, Pg_WHITE);
    Pg_سيٽ_Draw_گهڻائي(Pg_DRAWMODE_XOR);
    gui_mch_clear_all();
    gui_mch_flush();

    ui_delay((ڊگهو) msec, TRUE);

    gui_mch_clear_all();
    Pg_سيٽ_Draw_گهڻائي(Pg_DRAWMODE_OPAQUE);
    gui_mch_flush();
}

    فراغ
gui_mch_flush(فراغ)
{
    PgFlush();
}

    فراغ
gui_mch_سيٽ_متن_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    PhArea_t area = {{x, y}, {w, h}};

    Pt_سيٽ_Resource(gui.vim_متن_Area, Pt_ARG_AREA, &area, 0);
}

    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_ڪنجيون[i].ڪنجي_sym != 0; i++)
	إذا (name[0] == special_ڪنجيون[i].vim_code0 &&
		 name[1] == special_ڪنجيون[i].vim_code1)
	    أرجع OK;
    أرجع FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
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
    &tb_ڳوليو_phi,
    &tb_save_all_phi,
    &tb_save_session_phi,
    &tb_new_session_phi,
    &tb_ڀريو_session_phi,
    &tb_macro_phi,
    &tb_replace_phi,
    &tb_close_phi,
    &tb_maximize_phi,
    &tb_minimize_phi,
    &tb_ورهايو_phi,
    &tb_shell_phi,
    &tb_ڳوليو_prev_phi,
    &tb_ڳوليو_ايندڙ_phi,
    &tb_ڳوليو_help_phi,
    &tb_make_phi,
    &tb_jump_phi,
    &tb_ctags_phi,
    &tb_vsplit_phi,
    &tb_maxwidth_phi,
    &tb_minwidth_phi
};

ساكن PhImage_t *
gui_ph_toolbar_ڀريو_icon(char_u *iconfile)
{
    ساكن PhImage_t external_icon;
    PhImage_t *عارضي_phi = عدم;

    عارضي_phi = Px_ڀريو_Image(iconfile, عدم);
    إذا (عارضي_phi != عدم)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	عارضي_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	يادگيري_نقل(&external_icon, عارضي_phi, حجم(external_icon));
	حرر(عارضي_phi);

	عارضي_phi = &external_icon;
    }
    أرجع عارضي_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ساكن PhImage_t *
gui_ph_toolbar_ڳوليو_icon(vimmenu_T *menu)
{
    char_u ڀريل_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = عدم;

    إذا (menu->icon_builtin == FALSE)
    {
	إذا (menu->iconfile != عدم)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_ڀريو_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	إذا ((icon == عدم) &&
	    ((gui_ڳوليو_bitmap(menu->name, ڀريل_pathname, "gif") == OK) ||
	      (gui_ڳوليو_bitmap(menu->name, ڀريل_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_ڀريو_icon(ڀريل_pathname);

	إذا (icon != عدم)
	    أرجع icon;
    }

    إذا (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_ڊيگهائي(gui_ph_toolbar_images)))
	أرجع gui_ph_toolbar_images[menu->iconidx];

    أرجع عدم;
}
#جيڪڏهن_پورو

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
gui_mch_سيٽ_menu_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ساكن فراغ
gui_ph_position_menu(PtWidget_t *widget, صحيح ترجيح)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = Pt_Widget_ٻار_پٺ(Pt_Widget_والد(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    طالما (traverse != عدم)
    {
	PtGetResource(traverse, Pt_ARG_اشارو, &menu, 0);

	إذا (menu != عدم &&
		ترجيح < menu->ترجيح &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_وجهو(widget, traverse, 1);
	    أرجع;
	}

	traverse = Pt_Widget_Brother_اندريون_Front(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    فراغ
gui_mch_جوڙيو_menu(vimmenu_T *menu, صحيح اشاريو)
{
    vimmenu_T	*والد = menu->والد;
    char_u	*accel_ڪنجي;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحيح	    n;
    PtArg_t دليلون[5];

    menu->submenu_id = menu->id = عدم;

    إذا (menu_is_menubar(menu->name))
    {

	accel_ڪنجي = vim_strchr(menu->name, '&');
	إذا (accel_ڪنجي != عدم)
	{
	    mnemonic_str[0] = accel_ڪنجي[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_متن_STRING, menu->dname, 0);
	Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ACCEL_متن, menu->actext, 0);
	إذا (accel_ڪنجي != عدم)
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ACCEL_ڪنجي, mnemonic_str, 0);
	Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_اشارو, menu, 0);

	إذا (والد != عدم)
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_ساڄو, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(والد == عدم) ? gui.vimMenuBar : والد->submenu_id,
		n, دليلون);

	Pt_جوڙيو_Callback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	إذا (والد != عدم)
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_ٻار);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, دليلون);

	إذا (والد == عدم)
	{
	    Pt_جوڙيو_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    إذا (menu->mnemonic != 0)
	    {
		Pt_جوڙيو_Hotkey_Handler(gui.vim_دري, محفوظ_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->ترجيح);

	// Redraw menubar here instead of gui_mch_draw_menubar
	إذا (gui.menu_is_سرگرم)
	    PtRealizeWidget(menu->id);
    }
    وإلا إذا (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vim_دري, 0, عدم);
	Pt_جوڙيو_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    فراغ
gui_mch_جوڙيو_menu_شي(vimmenu_T *menu, صحيح اشاريو)
{
    vimmenu_T	*والد = menu->والد;
    char_u	*accel_ڪنجي;
    char_u	mnemonic_str[MB_LEN_MAX];
    صحيح	    n;
    PtArg_t دليلون[13];

    n = 0;
    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_اشارو, menu, 0);

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(والد->name))
    {
	إذا (menu_is_separator(menu->name))
	{
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_اندريون, 0);
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_چوٽي_BOTTOM);
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ويڪر, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, دليلون);
	}
	وإلا
	{
	    إذا (لڙي_ڳولا((ثابت حرف *) p_toolbar, "text") != عدم)
	    {
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_متن_STRING, menu->dname, 0);
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_متن_FONT, "TextFont08", 0);
	    }
	    إذا ((لڙي_ڳولا((ثابت حرف *) p_toolbar, "icons") != عدم) &&
		    (gui_ph_toolbar_images != عدم))
	    {
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_ڳوليو_icon(menu), 0);
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_LABEL_TYPE, Pt_متن_IMAGE, 0);
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_متن_IMAGE_SPACING, 0, 0);
	    }
	    إذا (لڙي_ڳولا((ثابت حرف *) p_toolbar, "tooltips") != عدم)
	    {
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_LABEL_BALLOON,
			gui_ph_show_tooltip, 0);
		Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_LABEL_FLAGS,
			Pt_TRUE, Pt_SHOW_BALLOON);
	    }
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_MARGIN_اوچائي, 1, 0);
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_MARGIN_ويڪر, 1, 0);
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_FLAGS, Pt_FALSE,
		    Pt_HIGHLIGHTED | Pt_GETS_FOCUS);
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_FILL_COLOR, Pg_TRANSPARENT, 0);
	    menu->id = PtCreateWidget(PtButton, gui.vimToolBar, n, دليلون);

	    Pt_جوڙيو_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);
	}
	// Update toolbar if it's open
	إذا (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    وإلا
# endif
	إذا (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, والد->submenu_id, n, دليلون);
    }
    وإلا
    {
	accel_ڪنجي = vim_strchr(menu->name, '&');
	إذا (accel_ڪنجي != عدم)
	{
	    mnemonic_str[0] = accel_ڪنجي[1];
	    mnemonic_str[1] = NUL;
	}

	Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_متن_STRING, menu->dname, 0);
	إذا (accel_ڪنجي != عدم)
	    Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ACCEL_ڪنجي, mnemonic_str,
		    0);

	Pt_سيٽ_Arg(&دليلون[ n++ ], Pt_ARG_ACCEL_متن, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, والد->submenu_id, n, دليلون);

	Pt_جوڙيو_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_گروهه
	إذا (gui_ph_is_بفر_شي(menu, والد) == TRUE)
	{
	    Pt_جوڙيو_Callback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_بفر_remove, menu);
	    gui_ph_pg_جوڙيو_بفر(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->ترجيح);
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
    ڊگهو    flags, نقاب, fields;

    إذا (menu->id == عدم)
	أرجع;

    flags = PtWidgetFlags(menu->id);
    إذا (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(Pt_Widget_والد(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	نقاب = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    وإلا
    {
	fields = Pt_TRUE;
	نقاب = Pt_BLOCKED | Pt_GHOST;
    }

    إذا (! grey)
	fields = ~fields;

    Pt_سيٽ_Resource(menu->id, Pt_ARG_FLAGS, fields,
	    نقاب);
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
    Pt_سيٽ_Resource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    // No tearoffs yet
}

#جيڪڏهن_پورو

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    إذا (showit)
	PtRealizeWidget(gui.vimToolBar);
    وإلا
	PtUnrealizeWidget(gui.vimToolBar);
}
#جيڪڏهن_پورو

////////////////////////////////////////////////////////////////////////////
// Fonts

    ساكن GuiFont
gui_ph_get_font(
	char_u	*font_name,
	int_u	font_flags,
	int_u	font_سائيز_2,
	// Check whether the resulting font has the font flags and size that
	// was asked for
	int_u	enforce
	)
{
    char_u	    *font_ٽيگ;
    FontQueryInfo   info;
    int_u	    style;

    font_ٽيگ = alloc(MAX_FONT_ٽيگ);
    إذا (font_ٽيگ != عدم)
    {
	إذا (PfGenerateFontName(font_name, font_flags, font_سائيز_2,
		    font_ٽيگ) != عدم)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    إذا (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    إذا (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    إذا (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_ٽيگ, &info);

	    إذا (info.سائيز == 0)
		font_سائيز_2 = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    إذا (font_سائيز_2 == info.سائيز &&
		    style == (info.style & style))
		أرجع (GuiFont)font_ٽيگ;
	}
	vim_free(font_ٽيگ);
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
	int_u *font_سائيز_2)
{
    char_u  *mark;
    int_u   name_len, سائيز;

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
		بدل (محفوظ_tolower(*mark++))
		{
		    حالة 'a': *font_flags |= PF_STYLE_ANTIALIAS; اكسر;
		    حالة 'b': *font_flags |= PF_STYLE_BOLD; اكسر;
		    حالة 'i': *font_flags |= PF_STYLE_ITALIC; اكسر;

		    حالة 's':
			سائيز = getdigits(&mark);
			// Restrict the size to some vague limits
			إذا (سائيز < 1 || سائيز > 100)
			    سائيز = 8;

			*font_سائيز_2 = سائيز;
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
    char_u  *font_ٽيگ;
    char_u  *font_name = عدم;
    int_u   font_flags = 0;
    int_u   font_سائيز_2  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    إذا (vim_font_name == عدم)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    إذا (STRCMP(vim_font_name, "*") == 0)
    {
	font_ٽيگ = PtFontSelection(gui.vim_دري, عدم, عدم,
		"pcterm12", -1, PHFONT_FIXED, عدم);

	إذا (font_ٽيگ == عدم)
	    أرجع FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ٽيگ;

	PfQueryFontInfo(font_ٽيگ, &info);
	font_name = vim_strsave(info.font);
    }
    وإلا
    {
	إذا (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_سائيز_2) == FALSE)
	    أرجع FAIL;

	font_ٽيگ = gui_ph_get_font(font_name, font_flags, font_سائيز_2, 0);
	إذا (font_ٽيگ == عدم)
	{
	    vim_free(font_name);
	    أرجع FAIL;
	}

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ٽيگ;
    }

    gui_mch_free_font(gui.bold_font);
    gui.bold_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_BOLD,
	    font_سائيز_2, PF_STYLE_BOLD);

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_ITALIC,
	    font_سائيز_2, PF_STYLE_ITALIC);

    // This extent was brought to you by the letter 'g'
    Pf_Extent_متن(&extent, عدم, font_ٽيگ, "g", 1);

    gui.char_ويڪر = extent.lr.x - extent.ul.x + 1;
    gui.char_اوچائي = (- extent.ul.y) + extent.lr.y + 1;
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

    gui.char_اوچائي = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    أرجع OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, صحيح report_error)
{
    char_u  *font_name;
    char_u  *font_ٽيگ;
    int_u   font_سائيز_2 = 12;
    int_u   font_flags = 0;

    إذا (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_سائيز_2) != FALSE)
    {
	font_ٽيگ = gui_ph_get_font(font_name, font_flags, font_سائيز_2, -1);
	vim_free(font_name);

	إذا (font_ٽيگ != عدم)
	    أرجع (GuiFont)font_ٽيگ;
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
#جيڪڏهن_پورو

    فراغ
gui_mch_سيٽ_font(GuiFont font)
{
    Pg_سيٽ_Font(font);
}

    فراغ
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

