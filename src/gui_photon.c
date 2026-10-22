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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_TOOLBAR
# include <photon/PxImage.h>
#endif

#ଯଦି !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define PhEvent_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define PhPoint_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define PtWidget_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define Pg_BLACK	0
# define PtCallbackF_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define PtCallbackInfo_t ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define PhTile_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define PtWidget_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
# define PhImage_t	ପୂର୍ଣ୍ଣସଂଖ୍ୟା
#endif

#define RGB(r, g, b) PgRGB(r, g, b)

#define EVENT_BUFFER_SIZE ଆକାର(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#define MOUSE_HIDE		TRUE
#define MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#undef USE_PANEL_GROUP

#ifdef USE_PANEL_GROUP
ସ୍ଥିର ଅକ୍ଷର	*empty_title = "    ";
ସ୍ଥିର ଅକ୍ଷର	**panel_titles = ଖାଲି;
ସ୍ଥିର ushort_t	ଅଙ୍କ_panels = 0;
ସ୍ଥିର short pg_margin_ବାମ, pg_margin_ଡାହାଣ, pg_margin_ଶୀର୍ଷ, pg_margin_bottom;
#endif

#define GUI_PH_MARGIN		4	// Size of the bevel

#define GUI_PH_MOUSE_TYPE		Ph_CURSOR_ଭର୍ତ୍ତି
ସ୍ଥିର PgColor_t gui_ph_mouse_color =	Pg_BLACK;

ସ୍ଥିର PhPoint_t    gui_ph_raw_offset;
ସ୍ଥିର PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
ସ୍ଥିର PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
ସ୍ଥିର short	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
ସ୍ଥିର PhPoint_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
ସ୍ଥିର ସଂରଚନା PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
ସ୍ଥିର ଗଣନା {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

ସ୍ଥିର long_u	blink_waittime	= 700;
ସ୍ଥିର long_u	blink_ontime	= 400;
ସ୍ଥିର long_u	blink_offtime	= 250;

ସ୍ଥିର ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଚାବି_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ଚାବିଗୁଡ଼ିକ[] =
{
    {Pk_Up,	    'k', 'u'},
    {Pk_Down,	    'k', 'd'},
    {Pk_ବାମ,	    'k', 'l'},
    {Pk_ଡାହାଣ,	    'k', 'r'},

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
    {Pk_ଭର୍ତ୍ତି,	    'k', 'I'},
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

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //ଭର୍ତ୍ତି
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

ସ୍ଥିର PtCallbackF_t gui_ph_handle_timer_cursor;
ସ୍ଥିର PtCallbackF_t gui_ph_handle_timer_timeout;

ସ୍ଥିର PtCallbackF_t gui_ph_handle_window_cb;

ସ୍ଥିର PtCallbackF_t gui_ph_handle_scrollbar;
ସ୍ଥିର PtCallbackF_t gui_ph_handle_keyboard;
ସ୍ଥିର PtCallbackF_t gui_ph_handle_mouse;
ସ୍ଥିର PtCallbackF_t gui_ph_handle_pulldown_menu;
ସ୍ଥିର PtCallbackF_t gui_ph_handle_menu;
ସ୍ଥିର PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

ସ୍ଥିର PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
ସ୍ଥିର PtCallbackF_t gui_ph_handle_menu_unrealized;

#ifdef USE_PANEL_GROUP
ସ୍ଥିର ଶୂନ୍ୟ gui_ph_get_panelgroup_margins(short*, short*, short*, short*);
#endif

ସ୍ଥିର ଶୂନ୍ୟ gui_ph_draw_start(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ gui_ph_draw_end(ଶୂନ୍ୟ);

// Set the text for the balloon
ସ୍ଥିର PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     ପୂର୍ଣ୍ଣସଂଖ୍ୟା position,
			     ଅକ୍ଷର *text,
			     ଅକ୍ଷର *font,
			     PgColor_t fill_color,
			     PgColor_t text_color);

////////////////////////////////////////////////////////////////////////////

ସ୍ଥିର PtWidget_t * gui_ph_show_tooltip(PtWidget_t *window,
			     PtWidget_t *widget,
			     ପୂର୍ଣ୍ଣସଂଖ୍ୟା position,
			     ଅକ୍ଷର *text,
			     ଅକ୍ଷର *font,
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
    ଯଦି (menu != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = MENU_INDEX_TIP;
	ଯଦି (menu->strings[ index ] != ଖାଲି)
	    tooltip = menu->strings[ index ];
    }

    ଫେରନ୍ତୁ PtInflateBalloon(
	    window,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_ଶୀର୍ଷ,
	    tooltip,
	    font,
	    fill_color,
	    text_color);
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_resize_container(ଶୂନ୍ୟ)
{
    PhArea_t area;

    PtWidgetArea(gui.vimWindow, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    PtSetResource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	ଶୂନ୍ୟ *other,
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
    ଯଦି (gui.menu_is_active)
    {
	gui.menu_height = height;
	gui.toolbar_height = 0;
    }
#ifdef FEAT_TOOLBAR
    ଅନ୍ୟଥା
	gui.toolbar_height = height;
#endif

    below_menu.x = 0;
    below_menu.y = height;

#ifdef USE_PANEL_GROUP
    container = gui.vimPanelGroup;
#ଅନ୍ୟଥା
    container = gui.vimContainer;
#endif

    PtSetResource(container, Pt_ARG_POS, &below_menu, 0);

    gui_ph_resize_container();

#ifdef USE_PANEL_GROUP
    gui_ph_get_panelgroup_margins(
	    &pg_margin_ଶୀର୍ଷ, &pg_margin_bottom,
	    &pg_margin_ବାମ, &pg_margin_ଡାହାଣ);
#endif
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	ଶୂନ୍ୟ *data,
	PtCallbackInfo_t *info)
{
    ଯଦି (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    ଅନ୍ୟଥା
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    ଶୂନ୍ୟ		*data,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_window_cb(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    PhWindowEvent_t *we = info->cbdata;
    ushort_t *width, *height;

    ବାଛ (we->event_f)
    {
	ସ୍ଥିତି Ph_WM_CLOSE:
	    gui_shell_closed();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    ଯଦି (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_start_blink();
	    }
	    ଅନ୍ୟଥା
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ph_WM_RESIZE:
	    PtGetResource(gui.vimWindow, Pt_ARG_WIDTH, &width, 0);
	    PtGetResource(gui.vimWindow, Pt_ARG_HEIGHT, &height, 0);
#ifdef USE_PANEL_GROUP
	    width  -= (pg_margin_ବାମ + pg_margin_ଡାହାଣ);
	    height -= (pg_margin_ଶୀର୍ଷ + pg_margin_bottom);
#endif
	    gui_resize_shell(*width, *height);
	    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    PtEndFlux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_scrollbar(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ମୂଲ୍ୟ, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) data;
    ଯଦି (sb != ଖାଲି)
    {
	ମୂଲ୍ୟ = scroll->position;
	ବାଛ (scroll->action)
	{
	    ସ୍ଥିତି Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି Pt_SCROLL_SET:
		// FIXME: return straight away here?
		ଫେରନ୍ତୁ Pt_CONTINUE;
		ଭାଙ୍ଗ;
	}

	gui_drag_scrollbar(sb, ମୂଲ୍ୟ, dragging);
    }
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_keyboard(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    Ph_ଚାବି_Event_t    *ଚାବି;
    unsigned ଅକ୍ଷର   string[6];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len, i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ch, modifiers;

    ଚାବି = PhGetData(info->event);

    ch = modifiers = len = 0;

    ଯଦି (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    ଯଦି (ଚାବି->ଚାବି_flags & Pk_KF_Compose)
	ଫେରନ୍ତୁ Pt_CONTINUE;

    ଯଦି ((ଚାବି->ଚାବି_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_ଚାବି_Down(ଚାବି->ଚାବି_flags))
    {
#ifdef FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	ଯଦି (((ଚାବି->ଚାବି_mods & Pk_KM_Alt) &&
		    !(ଚାବି->ଚାବି_mods & Pk_KM_Shift) &&
		    !(ଚାବି->ଚାବି_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_active &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(ଚାବି->ଚାବି_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    ଫେରନ୍ତୁ Pt_CONTINUE;
	}
#endif

	ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym != 0; i++)
	{
	    ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym == ଚାବି->ଚାବି_cap)
	    {
		len = 0;
		ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].vim_code1 == NUL)
		    ch = special_ଚାବିଗୁଡ଼ିକ[i].vim_code0;
		ଅନ୍ୟଥା
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    ଯଦି (ଚାବି->ଚାବି_cap >= Pk_KP_Enter && ଚାବି->ଚାବି_cap <= Pk_KP_9
			    && (ଚାବି->ଚାବି_mods & Pk_KM_ଅଙ୍କ_Lock))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = ଚାବି->ଚାବି_cap - 0xf080;
		    }
		    ଅନ୍ୟଥା
			ch = TO_SPECIAL(special_ଚାବିଗୁଡ଼ିକ[i].vim_code0,
				special_ଚାବିଗୁଡ଼ିକ[i].vim_code1);
		}
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (ଚାବି->ଚାବି_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_MASK_CTRL;
	ଯଦି (ଚାବି->ଚାବି_mods & Pk_KM_Alt)
	    modifiers |= MOD_MASK_ALT;
	ଯଦି (ଚାବି->ଚାବି_mods & Pk_KM_Shift)
	    modifiers |= MOD_MASK_SHIFT;

	// Is this not a special key?
	ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym == 0)
	{
	    ch = PhTo8859_1(ଚାବି);
	    ଯଦି (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_ଚାବି_To_Mb(string, ଚାବି);
		ଯଦି (len > 0)
		{
		    ସ୍ଥିର ଅକ୍ଷର buf[6];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା src_taken, dst_made;
		    ଯଦି (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_taken,
				buf,
				6,
				&dst_made);

			add_to_ନିବେଶ_buf(buf, dst_made);
		    }
		    ଅନ୍ୟଥା
		    {
			add_to_ନିବେଶ_buf(string, len);
		    }

		    ଫେରନ୍ତୁ Pt_CONSUME;
		}
		len = 0;
		ch = ଚାବି->ଚାବି_cap;
		ଯଦି (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    ଯଦି (modifiers & MOD_MASK_CTRL)
		    {
			modifiers &= ~MOD_MASK_CTRL;

			ଯଦି ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			ଅନ୍ୟଥା ଯଦି (ch == '2')
			    ch = NUL;
			ଅନ୍ୟଥା ଯଦି (ch == '6')
			    ch = 0x1e;
			ଅନ୍ୟଥା ଯଦି (ch == '-')
			    ch = 0x1f;
			ଅନ୍ୟଥା
			    modifiers |= MOD_MASK_CTRL;
		    }

		    ଯଦି (modifiers & MOD_MASK_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_MASK_ALT;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଫେରନ୍ତୁ Pt_CONTINUE;
		}
	    }
	    ଅନ୍ୟଥା
		modifiers &= ~MOD_MASK_SHIFT;
	}

	ch = simplify_ଚାବି(ch, &modifiers);
	ଯଦି (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	ଯଦି (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_SECOND(ch);
	    string[ len++ ] = K_THIRD(ch);
	}
	ଅନ୍ୟଥା
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା int_ch = check_for_interrupt(ch, modifiers);

	    ଯଦି (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_ନିବେଶ_buf();
	    }
	}

	ଯଦି (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	ଯଦି (len > 0)
	{
	    add_to_ନିବେଶ_buf(string, len);
	    ଫେରନ୍ତୁ Pt_CONSUME;
	}
    }

    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_mouse(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    PhPointerEvent_t *pointer;
    PhRect_t	     *pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		     button = 0, repeated_click, modifiers = 0x0;
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

    ଯଦି (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	ଫେରନ୍ତୁ Pt_CONTINUE;
    }

    ଯଦି (pointer->ଚାବି_mods & Pk_KM_Shift)
	modifiers |= MOUSE_SHIFT;
    ଯଦି (pointer->ଚାବି_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    ଯଦି (pointer->ଚାବି_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    ଯଦି (pointer->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_ବାମ;

    ଯଦି (pointer->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_ଡାହାଣ;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = pointer->pos.x;
	abs_mouse.y = pointer->pos.y;
    }

    ଯଦି (pointer->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_ମଧ୍ୟ;

    // Catch a real release (not phantom or other releases
    ଯଦି (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    ଯଦି (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#ଯଦି 0
    // Vim doesn't use button repeats
    ଯଦି (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#endif

    // Don't do anything if it is one of the phantom mouse release events
    ଯଦି ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (pointer->click_ଗଣନା >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    ଫେରନ୍ତୁ Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_focus(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    ଯଦି (info->reason == Pt_CB_LOST_FOCUS)
    {
	PtRemoveEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ଖାଲି);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    ଅନ୍ୟଥା
    {
	PtAddEventHandler(gui.vimTextArea, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ଖାଲି);
    }
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    PhPoint_t	offset;
    PhPoint_t	translation;

    ଯଦି (is_ignore_draw == TRUE)
	ଫେରନ୍ତୁ;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#ଯଦି 0
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
    PhTranslatePoint(&offset, PtWidgetPos(gui.vimTextArea, ଖାଲି));

#ଯଦି 1
    // Redraw individual damage regions
    ଯଦି (damage->next != ଖାଲି)
	damage = damage->next;

    ଯେତେବେଳେ (damage != ଖାଲି)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - offset.x, r->ul.y - offset.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->next;
    }
#ଅନ୍ୟଥା
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - offset.x, r->ul.y - offset.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#endif

    PgSetTranslation(&translation, 0);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	ଶୂନ୍ୟ *data,
	PtCallbackInfo_t *info)
{
    ଯଦି (data != ଖାଲି)
    {
	vimmenu_T *menu = (vimmenu_T *) data;

	PtPositionMenu(menu->submenu_id, ଖାଲି);
	PtRealizeWidget(menu->submenu_id);
    }

    ଫେରନ୍ତୁ Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_menu(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    ଯଦି (data != ଖାଲି)
    {
	vimmenu_T *menu = (vimmenu_T *) data;
	gui_menu_cb(menu);
    }
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	ଶୂନ୍ୟ *data,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vimTextArea, ଖାଲି);
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_window_open(
	PtWidget_t *widget,
	ଶୂନ୍ୟ *data,
	PtCallbackInfo_t *info)
{
    gui_set_shellsize(FALSE, TRUE, RESIZE_BOTH);
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#define DRAW_START  gui_ph_draw_start()
#define DRAW_END    gui_ph_draw_end()

/*
 * TODO: Set a clipping rect?
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_draw_start(ଶୂନ୍ୟ)
{
    PhGC_t *gc;

    gc = PgGetGC();
    PgSetRegion(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ଖାଲି));

    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_draw_end(ଶୂନ୍ୟ)
{
    gui_ph_raw_offset.x = -gui_ph_raw_offset.x;
    gui_ph_raw_offset.y = -gui_ph_raw_offset.y;
    PgSetTranslation(&gui_ph_raw_offset, Pg_RELATIVE);
}

#ifdef USE_PANEL_GROUP
    ସ୍ଥିର vimmenu_T *
gui_ph_find_buffer_item(char_u *name)
{
    vimmenu_T *ଶୀର୍ଷ_level = ମୂଳ_menu;
    vimmenu_T *ବସ୍ତୁଗୁଡ଼ିକ = ଖାଲି;

    ଯେତେବେଳେ (ଶୀର୍ଷ_level != ଖାଲି &&
	    (STRCMP(ଶୀର୍ଷ_level->dname, "Buffers") != 0))
	ଶୀର୍ଷ_level = ଶୀର୍ଷ_level->next;

    ଯଦି (ଶୀର୍ଷ_level != ଖାଲି)
    {
	ବସ୍ତୁଗୁଡ଼ିକ = ଶୀର୍ଷ_level->children;

	ଯେତେବେଳେ (ବସ୍ତୁଗୁଡ଼ିକ != ଖାଲି &&
		(STRCMP(ବସ୍ତୁଗୁଡ଼ିକ->dname, name) != 0))
	    ବସ୍ତୁଗୁଡ଼ିକ = ବସ୍ତୁଗୁଡ଼ିକ->next;
    }
    ଫେରନ୍ତୁ ବସ୍ତୁଗୁଡ଼ିକ;
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_pg_set_buffer_ଅଙ୍କ(int_u buf_ଅଙ୍କ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଅକ୍ଷର ସନ୍ଧାନ[16];
    ଅକ୍ଷର *mark;

    ଯଦି (gui.vimTextArea == ଖାଲି || buf_ଅଙ୍କ == 0)
	ଫେରନ୍ତୁ;

    ସନ୍ଧାନ[0] = '(';
    ultoa(buf_ଅଙ୍କ, &ସନ୍ଧାନ[1], 10);
    STRCAT(ସନ୍ଧାନ, ")");

    ପାଇଁ (i = 0; i < ଅଙ୍କ_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	ଯଦି (mark != ଖାଲି && STRCMP(mark, ସନ୍ଧାନ) == 0)
	{
	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_CURRENT_INDEX,
		    i, 0);
	}
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	ଶୂନ୍ୟ *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    PtPanelGroupCallback_t *panel;

    ଯଦି (info->event != ଖାଲି)
    {
	panel = info->cbdata;
	ଯଦି (panel->new_panel != ଖାଲି)
	{
	    menu = gui_ph_find_buffer_item(panel->new_panel);
	    ଯଦି (menu)
		gui_menu_cb(menu);
	}
    }
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_get_panelgroup_margins(
	short *ଶୀର୍ଷ,
	short *bottom,
	short *ବାମ,
	short *ଡାହାଣ)
{
    unsigned short abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    ଅଚଳ unsigned short *margin_ଶୀର୍ଷ, *margin_bottom;
    ଅଚଳ unsigned short *margin_ବାମ, *margin_ଡାହାଣ;

    PtGetAbsPosition(gui.vimTextArea, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vimPanelGroup, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_ଡାହାଣ, &margin_ଡାହାଣ, 0);
    PtGetResource(gui.vimPanelGroup, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *ଶୀର୍ଷ    = abs_raw_y;
    *bottom = *margin_bottom;

    *ବାମ  = abs_raw_x;
    *ଡାହାଣ = *margin_ଡାହାଣ;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_is_buffer_item(vimmenu_T *menu, vimmenu_T *parent)
{
    ଅକ୍ଷର *mark;

    ଯଦି (STRCMP(parent->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	ଯଦି (mark != ଖାଲି)
	{
	    mark++;
	    ଯେତେବେଳେ (SAFE_isdigit(*mark))
		mark++;

	    ଯଦି (*mark == ')')
		ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_pg_add_buffer(ଅକ୍ଷର *name)
{
    ଅକ୍ଷର **new_titles = ଖାଲି;

    new_titles = ALLOC_MULT(ଅକ୍ଷର *, (ଅଙ୍କ_panels + 1));
    ଯଦି (new_titles == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (ଅଙ୍କ_panels > 0)
	memcpy(new_titles, panel_titles, ଅଙ୍କ_panels * ଆକାର(ଅକ୍ଷର **));

    new_titles[ ଅଙ୍କ_panels++ ] = name;

    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    ଅଙ୍କ_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_pg_remove_buffer(ଅକ୍ଷର *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଅକ୍ଷର **new_titles = ଖାଲି;

    // If there is only 1 panel, we just use the temporary place holder
    ଯଦି (ଅଙ୍କ_panels > 1)
    {
	new_titles = ALLOC_MULT(ଅକ୍ଷର *, ଅଙ୍କ_panels - 1);
	ଯଦି (new_titles != ଖାଲି)
	{
	    ଅକ୍ଷର **s = new_titles;
	    // Copy all the titles except the one we're removing
	    ପାଇଁ (i = 0; i < ଅଙ୍କ_panels; i++)
	    {
		ଯଦି (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    ଅଙ୍କ_panels--;

	    PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    ଅଙ୍କ_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    ଅନ୍ୟଥା
    {
	ଅଙ୍କ_panels--;
	PtSetResource(gui.vimPanelGroup, Pt_ARG_PG_PANEL_TITLES, &empty_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_handle_buffer_remove(
	PtWidget_t *widget,
	ଶୂନ୍ୟ *data,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    ଯଦି (data != ଖାଲି)
    {
	menu = (vimmenu_T *) data;
	gui_ph_pg_remove_buffer(menu->dname);
    }

    ଫେରନ୍ତୁ Pt_CONTINUE;
}
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_pane_resize(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    ଯଦି (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	PtStartFlux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    ଫେରନ୍ତୁ Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    ଶୂନ୍ୟ
gui_ph_encoding_changed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_encoding)
{
    // Default encoding is latin1
    ଅକ୍ଷର *charset = "latin1";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ସଂରଚନା {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା encoding;
	ଅକ୍ଷର *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    ପାଇଁ (i = 0; i < ARRAY_LENGTH(charsets); i++)
    {
	ଯଦି (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = PxTranslateSet(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    ଶୂନ୍ୟ
gui_mch_prepare(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *argc, ଅକ୍ଷର **argv)
{
    PtInit(ଖାଲି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init(ଶୂନ୍ୟ)
{
    PtArg_t args[10];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags = 0, n = 0;

    PhDim_t	window_size = {100, 100}; // Arbitrary values
    PhPoint_t	pos = {0, 0};

    gui.event_buffer = alloc(EVENT_BUFFER_SIZE);
    ଯଦି (gui.event_buffer == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = PxTranslateSet(ଖାଲି, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_offset = gui.border_width = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    PtSetArg(&args[ n++ ], Pt_ARG_WINDOW_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    gui.vimWindow = PtCreateWidget(PtWindow, ଖାଲି, n, args);
    ଯଦି (gui.vimWindow == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW, gui_ph_handle_window_cb, ଖାଲି);
    PtAddCallback(gui.vimWindow, Pt_CB_WINDOW_OPENING,
	    gui_ph_handle_window_open, ଖାଲି);

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_DIM, &window_size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_POS, &pos, 0);

#ifdef USE_PANEL_GROUP
    // Put in a temporary place holder title
    PtSetArg(&args[ n++ ], Pt_ARG_PG_PANEL_TITLES, &empty_title, 1);

    gui.vimPanelGroup = PtCreateWidget(PtPanelGroup, gui.vimWindow, n, args);
    ଯଦି (gui.vimPanelGroup == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    PtAddCallback(gui.vimPanelGroup, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, ଖାଲି);
#ଅନ୍ୟଥା
    // Turn off all edge decorations
    PtSetArg(&args[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    PtSetArg(&args[ n++ ], Pt_ARG_BEVEL_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_WIDTH, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_MARGIN_HEIGHT, 0, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vimWindow, n, args);
    ଯଦି (gui.vimContainer == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    PtAddCallback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, ଖାଲି);
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
    ଯଦି (gui.vimTextArea == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    PtAddEventHandler(gui.vimTextArea,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, ଖାଲି);
    PtAddEventHandler(gui.vimTextArea, Ph_EV_ଚାବି,
	    gui_ph_handle_keyboard, ଖାଲି);
    PtAddCallback(gui.vimTextArea, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, ଖାଲି);
    PtAddCallback(gui.vimTextArea, Pt_CB_LOST_FOCUS,
	    gui_ph_handle_focus, ଖାଲି);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vimWindow, 0, ଖାଲି);
    ଯଦି (gui_ph_timer_cursor == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vimWindow, 0, ଖାଲି);
    ଯଦି (gui_ph_timer_timeout == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    PtAddCallback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, ଖାଲି);
    PtAddCallback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, ଖାଲି);

#ifdef FEAT_MENU
    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ବାମ_ଡାହାଣ,
	    Pt_IS_ANCHORED);
    gui.vimToolBarGroup = PtCreateWidget(PtToolbarGroup, gui.vimWindow,
	    n, args);
    ଯଦି (gui.vimToolBarGroup == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    PtAddCallback(gui.vimToolBarGroup, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, ଖାଲି);

    n = 0;
    flags = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);
    ଯଦି (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vimToolBarGroup, n, args);
    ଯଦି (gui.vimMenuBar == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_ବାମ_ଡାହାଣ |Pt_ଶୀର୍ଷ_ANCHORED_ଶୀର୍ଷ, Pt_IS_ANCHORED);
    PtSetArg(&args[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, window_size.w, 0);

    flags = Pt_GETS_FOCUS;
    ଯଦି (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    PtSetArg(&args[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vimToolBarGroup, n, args);
    ଯଦି (gui.vimToolBar == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#endif

    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_check(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (is_photon_available == TRUE) ? OK : FAIL;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_open(ଶୂନ୍ୟ)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.back_pixel =  Pg_WHITE;

    set_normal_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    highlight_gui_started();

    ଯଦି (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    ଫେରନ୍ତୁ (PtRealizeWidget(gui.vimWindow) == 0) ? OK : FAIL;
}

    ଶୂନ୍ୟ
gui_mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc)
{
    PtDestroyWidget(gui.vimWindow);

    PxTranslateSet(charset_translate, ଖାଲି);

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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exit_gui_mch_update(ଶୂନ୍ୟ *data)
{
    *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)data = FALSE;
    ଫେରନ୍ତୁ Pt_END;
}

    ଶୂନ୍ୟ
gui_mch_update(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା working = TRUE;

    PtAppAddWorkProc(ଖାଲି, exit_gui_mch_update, &working);
    ଯେତେବେଳେ ((working == TRUE) && !vim_is_ନିବେଶ_buf_full())
	PtProcessEvent();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_wait_for_chars(long wtime)
{
    is_timeout = FALSE;

    ଯଦି (wtime >= 0)
	PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    ଯେତେବେଳେ (1)
    {
	PtProcessEvent();
	ଯଦି (ନିବେଶ_available())
	{
	    PtSetResource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    ଫେରନ୍ତୁ OK;
	}
	ଅନ୍ୟଥା ଯଦି (is_timeout == TRUE)
	    ଫେରନ୍ତୁ FAIL;
    }
}

#ଯଦି defined(FEAT_BROWSE)
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags;
    char_u  *default_path;
    char_u  *open_text = ଖାଲି;

    flags = 0;
    CLEAR_FIELD(file);

    default_path = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    ଯଦି (default_path != ଖାଲି)
    {
	ଯଦି (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_text = "&Save";
	}

	// combine the directory and filename into a single path
	ଯଦି (initdir == ଖାଲି || *initdir == NUL)
	{
	    mch_dirname(default_path, MAXPATHL);
	    initdir = default_path;
	}
	ଅନ୍ୟଥା
	{
	    STRCPY(default_path, initdir);
	    initdir = default_path;
	}

	ଯଦି (default_name != ଖାଲି)
	{
	    ଯଦି (default_path[ STRLEN(default_path) - 1 ] != '/')
		STRCAT(default_path, "/");

	    STRCAT(default_path, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vimWindow,
		ଖାଲି,
		title,
		default_path,
		ଖାଲି,
		open_text,
		ଖାଲି,
		ଖାଲି,
		&file,
		flags);

	vim_free(default_path);

	ଯଦି (file.ret == Pt_FSDIALOG_BTN1)
	    ଫେରନ୍ତୁ vim_strsave(file.path);
    }
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_GUI_DIALOG)
ସ୍ଥିର PtWidget_t *gui_ph_dialog_text = ଖାଲି;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_dialog_close(ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, ଶୂନ୍ୟ *data)
{
    PtModalCtrl_t *modal_ctrl = data;
    char_u *dialog_text, *vim_text;

    ଯଦି (gui_ph_dialog_text != ଖାଲି)
    {
	PtGetResource(gui_ph_dialog_text, Pt_ARG_TEXT_STRING, &dialog_text, 0);
	PtGetResource(gui_ph_dialog_text, Pt_ARG_POINTER, &vim_text, 0);
	STRNCPY(vim_text, dialog_text, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (ଶୂନ୍ୟ *) button);

    ଫେରନ୍ତୁ Pt_TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_dialog_text_enter(
    PtWidget_t		*widget,
    ଶୂନ୍ୟ		*data,
    PtCallbackInfo_t	*info)
{
    ଯଦି (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, data);
    ଫେରନ୍ତୁ Pt_CONTINUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_dialog_esc(PtWidget_t *widget, ଶୂନ୍ୟ *data, PtCallbackInfo_t *info)
{
    Ph_ଚାବି_Event_t *ଚାବି;

    ଚାବି = PhGetData(info->event);
    ଯଦି ((ଚାବି->ଚାବି_flags & Pk_KF_Cap_Valid) && (ଚାବି->ଚାବି_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, data);
	ଫେରନ୍ତୁ Pt_CONSUME;
    }
    ଫେରନ୍ତୁ Pt_PROCESS;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_dialog(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	default_button,
	char_u	*textfield,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		button_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dialog_result = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_ଗଣନା = len = i = 0;

    ଯଦି (buttons == ଖାଲି || *buttons == NUL)
	ଫେରନ୍ତୁ -1;

    // There is one less separator than buttons, so bump up the button count
    button_ଗଣନା = 1;

    // Count string length and number of separators
    ପାଇଁ (str = buttons; *str; str++)
    {
	len++;
	ଯଦି (*str == DLG_BUTTON_SEP)
	    button_ଗଣନା++;
    }

    ଯଦି (title == ଖାଲି)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_ଗଣନା);
    ଯଦି (buttons_copy != ଖାଲି && button_array != ଖାଲି)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	ପାଇଁ (i = 0; i < button_ଗଣନା; i++)
	{
	    button_array[ i ] = str;
	    ପାଇଁ (; *str; str++)
	    {
		ଯଦି (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    ଭାଙ୍ଗ;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_result = PtAlert(
		gui.vimWindow, ଖାଲି,
		title,
		ଖାଲି,
		message, ଖାଲି,
		button_ଗଣନା, (ଅଚଳ ଅକ୍ଷର **) button_array, ଖାଲି,
		default_button, 0, Pt_MODAL);
# ଅନ୍ୟଥା
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
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
		    Pt_TRUE, Pt_GROUP_NO_ଚାବିଗୁଡ଼ିକ | Pt_GROUP_STRETCH_HORIZONTAL);
	    PtSetArg(&args[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(PtGroup, ଖାଲି, n, args);

	    n = 0;
	    PtSetArg(&args[n++], Pt_ARG_TEXT_STRING, message, 0);
	    PtCreateWidget(PtLabel, pane, n, args);

	    ଯଦି (textfield != ଖାଲି)
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
	    di.buttons = (ଅଚଳ ଅକ୍ଷର **) button_array;
	    di.nbtns = button_ଗଣନା;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_ଗଣନା;
	    di.callback = gui_ph_dialog_close;
	    di.data = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    PtAddFilterCallback(dialog, Ph_EV_ଚାବି,
		    gui_ph_dialog_esc, &modal_ctrl);

	    ଯଦି (gui_ph_dialog_text != ଖାଲି)
		PtGiveFocus(gui_ph_dialog_text, ଖାଲି);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_result = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) PtModalBlock(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_text = ଖାଲି;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    ଫେରନ୍ତୁ dialog_result;
}
#endif
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    PhPoint_t *pos;

    pos = PtWidgetPos(gui.vimWindow, ଖାଲି);

    *x = pos->x;
    *y = pos->y;

    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
gui_mch_set_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    PhPoint_t pos = { x, y };

    PtSetResource(gui.vimWindow, Pt_ARG_POS, &pos, 0);
}

    ଶୂନ୍ୟ
gui_mch_set_shellsize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_height, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction)
{
    PhDim_t window_size = { width, height };
    PhDim_t min_size = { min_width, min_height };

#ifdef USE_PANEL_GROUP
    window_size.w += pg_margin_ବାମ + pg_margin_ଡାହାଣ;
    window_size.h += pg_margin_ଶୀର୍ଷ + pg_margin_bottom;
#endif

    PtSetResource(gui.vimWindow, Pt_ARG_MINIMUM_DIM, &min_size, 0);
    PtSetResource(gui.vimWindow, Pt_ARG_DIM, &window_size, 0);

    ଯଦି (! PtWidgetIsRealized(gui.vimWindow))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    ଶୂନ୍ୟ
gui_mch_get_screen_dimensions(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *screen_w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *screen_h)
{
    PhRect_t console;

    PhWindowQueryVisible(Ph_QUERY_WORKSPACE, 0,
	    Ph_ନିବେଶ_Group(ଖାଲି), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    ଶୂନ୍ୟ
gui_mch_iconify(ଶୂନ୍ୟ)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ଶୂନ୍ୟ
gui_mch_set_foreground(ଶୂନ୍ୟ)
{
    PhWindowEvent_t event;

    CLEAR_FIELD(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vimWindow);
    PtForwardWindowEvent(&event);
}
#endif

    ଶୂନ୍ୟ
gui_mch_settitle(char_u *title,	char_u *icon)
{
#ifdef USE_PANEL_GROUP
    gui_ph_pg_set_buffer_ଅଙ୍କ(curwin->w_buffer->b_fnum);
#endif
    PtSetResource(gui.vimWindow, Pt_ARG_WINDOW_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = 0;
    PtArg_t args[3];

    PtSetArg(&args[ n++ ], Pt_ARG_MAXIMUM, max, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_SLIDER_SIZE, size, 0);
    PtSetArg(&args[ n++ ], Pt_ARG_GAUGE_ମୂଲ୍ୟ, val, 0);
    PtSetResources(sb->id, n, args);
}

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_pos(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    PtSetResource(sb->id, Pt_ARG_AREA, &area, 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_xpadding(ଶୂନ୍ୟ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ଫେରନ୍ତୁ 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_ypadding(ଶୂନ୍ୟ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ଫେରନ୍ତୁ 0;
}

    ଶୂନ୍ୟ
gui_mch_create_scrollbar(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା orient)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = 0;
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
#ଯଦି 0
    // Don't need this anchoring for the scrollbars
    ଯଦି (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_ବାମ_ANCHORED_ବାମ | Pt_ଡାହାଣ_ANCHORED_ଡାହାଣ;
    }
    ଅନ୍ୟଥା
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_ଶୀର୍ଷ_ANCHORED_ଶୀର୍ଷ;
	ଯଦି (sb->wp != ଖାଲି)
	{
	    ଯଦି (sb == &sb->wp->w_scrollbars[ SBAR_ବାମ ])
		anchor_flags |= Pt_ବାମ_ANCHORED_ବାମ;
	    ଅନ୍ୟଥା
		anchor_flags |= Pt_ଡାହାଣ_ANCHORED_ଡାହାଣ;
	}
    }
    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#endif
    PtSetArg(&args[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#ifdef USE_PANEL_GROUP
    sb->id = PtCreateWidget(PtScrollbar, gui.vimPanelGroup, n, args);
#ଅନ୍ୟଥା
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, args);
#endif

    PtAddCallback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    ଶୂନ୍ୟ
gui_mch_enable_scrollbar(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    ଯଦି (flag != 0)
	PtRealizeWidget(sb->id);
    ଅନ୍ୟଥା
	PtUnrealizeWidget(sb->id);
}

    ଶୂନ୍ୟ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = ଖାଲି;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#ଯଦି defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mshape_ids[] =
{
    Ph_CURSOR_POINTER,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_ଭର୍ତ୍ତି,		// beam
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

    ଶୂନ୍ୟ
mch_set_mouse_shape(ପୂର୍ଣ୍ଣସଂଖ୍ୟା shape)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;

    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ;

    ଯଦି (shape == MSHAPE_HIDE || gui.pointer_hidden)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    ଅନ୍ୟଥା
    {
	ଯଦି (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_POINTER;
	ଅନ୍ୟଥା
	    id = mshape_ids[shape];

	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    ଯଦି (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

    ଶୂନ୍ୟ
gui_mch_mousehide(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hide)
{
    ଯଦି (gui.pointer_hidden == hide)
	ଫେରନ୍ତୁ;

    gui.pointer_hidden = hide;
#ifdef FEAT_MOUSESHAPE
    ଯଦି (hide)
	PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    ଅନ୍ୟଥା
	mch_set_mouse_shape(last_shape);
#ଅନ୍ୟଥା
    PtSetResource(gui.vimTextArea, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#endif
}

    ଶୂନ୍ୟ
gui_mch_getmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    PhCursorInfo_t info;
    short ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_ନିବେଶ_Group(ଖାଲି), &info);
    PtGetAbsPosition(gui.vimTextArea , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    ଶୂନ୍ୟ
gui_mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    short abs_x, abs_y;

    PtGetAbsPosition(gui.vimTextArea, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_ନିବେଶ_Group(ଖାଲି), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ଫେରନ୍ତୁ (guicolor_T)(PgRGB(Pg_Red_ମୂଲ୍ୟ(pixel),
				     Pg_Green_ମୂଲ୍ୟ(pixel), Pg_Blue_ମୂଲ୍ୟ(pixel)));
}

    ଶୂନ୍ୟ
gui_mch_new_colors(ଶୂନ୍ୟ)
{
#ଯଦି 0 // Don't bother changing the cursor colour
    short color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.back_pixel);

    ଯଦି (abs(color_diff) < 64)
    {
	short r, g, b;
	// not a great algorithm...
	r = Pg_Red_ମୂଲ୍ୟ(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_ମୂଲ୍ୟ(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_ମୂଲ୍ୟ(gui_ph_mouse_color) ^ 255;

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
    ଫେରନ୍ତୁ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    ଫେରନ୍ତୁ gui_get_rgb_color_cmn(r, g, b);
}

    ଶୂନ୍ୟ
gui_mch_set_fg_color(guicolor_T color)
{
    PgSetTextColor(color);
}

    ଶୂନ୍ୟ
gui_mch_set_bg_color(guicolor_T color)
{
    PgSetFillColor(color);
}

    ଶୂନ୍ୟ
gui_mch_set_sp_color(guicolor_T color)
{
}

    ଶୂନ୍ୟ
gui_mch_invert_rectangle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(col);
    rect.ul.y = FILL_Y(row);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_width;
    rect.lr.y = rect.ul.y + nr * gui.char_height;
    ଯଦି (nc > 0)
	rect.lr.x -= 1;
    ଯଦି (nr > 0)
	rect.lr.y -= 1;

    DRAW_START;
    PgSetDrawMode(Pg_DrawModeDSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    PgSetDrawMode(Pg_DrawModeSRCCOPY);
    DRAW_END;
}

    ଶୂନ୍ୟ
gui_mch_clear_block(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col2)
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

    ଶୂନ୍ୟ
gui_mch_clear_all(ଶୂନ୍ୟ)
{
    PhRect_t text_rect = {
	{ gui.border_width, gui.border_width },
	{ Columns * gui.char_width + gui.border_width - 1 ,
	    Rows * gui.char_height + gui.border_width - 1 }
    };

    ଯଦି (is_ignore_draw == TRUE)
	ଫେରନ୍ତୁ;

    DRAW_START;
    gui_mch_set_bg_color(gui.back_pixel);
    PgDrawRect(&text_rect, Pg_DRAW_FILL);
    DRAW_END;
}

    ଶୂନ୍ୟ
gui_mch_delete_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ବାମ);
    rect.ul.y = FILL_Y(row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ);

    rect.lr.x = FILL_X(gui.scroll_region_ଡାହାଣ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ଖାଲି));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = -ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)), &rect, &delta);

    gui_clear_block(
	gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1,
	gui.scroll_region_ବାମ,
	gui.scroll_region_bot,
	gui.scroll_region_ଡାହାଣ);
}

    ଶୂନ୍ୟ
gui_mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    PhRect_t    rect;
    PhPoint_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ବାମ);
    rect.ul.y = FILL_Y(row);

    rect.lr.x = FILL_X(gui.scroll_region_ଡାହାଣ + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1) - 1;

    PtWidgetOffset(gui.vimTextArea, &gui_ph_raw_offset);
    PhTranslatePoint(&gui_ph_raw_offset, PtWidgetPos(gui.vimTextArea, ଖାଲି));
    PhTranslateRect(&rect, &gui_ph_raw_offset);

    delta.x = 0;
    delta.y = ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ * gui.char_height;

    PgFlush();

    PhBlit(PtWidgetRid(PtFindDisjoint(gui.vimTextArea)) , &rect, &delta);

    gui_clear_block(row, gui.scroll_region_ବାମ,
	    row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ - 1, gui.scroll_region_ଡାହାଣ);
}

    ଶୂନ୍ୟ
gui_mch_draw_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ସ୍ଥିର ଅକ୍ଷର *utf8_buffer = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	utf8_len = 0;

    PhPoint_t	pos = { TEXT_X(col), TEXT_Y(row) };
    PhRect_t	rect;

    ଯଦି (is_ignore_draw == TRUE)
	ଫେରନ୍ତୁ;

    DRAW_START;

    ଯଦି (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(col), FILL_Y(row),
		FILL_X(col + len) - 1, FILL_Y(row + 1) - 1,
		Pg_DRAW_FILL);
    }

    ଯଦି (flags & DRAW_UNDERL)
	PgSetUnderline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    ଯଦି (charset_translate != ଖାଲି && enc_utf8 == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା src_taken, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	ଯଦି (utf8_len < len)
	{
	    ଅକ୍ଷର *new_buffer = realloc(utf8_buffer, len * MB_LEN_MAX);

	    ଯଦି (new_buffer == ଖାଲି)
		ଫେରନ୍ତୁ;
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

    ଯଦି (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(col) + 1;
	rect.ul.y = FILL_Y(row);
	rect.lr.x = FILL_X(col + len) - 1;
	rect.lr.y = FILL_Y(row + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#ଯଦି 0
	pos.x++;

	PgSetUserClip(&rect);
	PgDrawText(s, len, &pos, 0);
	PgSetUserClip(ଖାଲି);
#ଅନ୍ୟଥା
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	PgDrawTextArea(s, len, &rect, Pg_TEXT_BOTTOM);
#endif
    }

    ଯଦି (flags & DRAW_UNDERL)
	PgSetUnderline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_END;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    ଶୂନ୍ୟ
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

    ଶୂନ୍ୟ
gui_mch_draw_part_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h, guicolor_T color)
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blinking(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state != BLINK_NONE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blink_off(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state == BLINK_OFF;
}

    ଶୂନ୍ୟ
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ଶୂନ୍ୟ
gui_mch_start_blink(ଶୂନ୍ୟ)
{
    // Only turn on the timer on if none of the times are zero
    ଯଦି (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ଶୂନ୍ୟ
gui_mch_stop_blink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_call_gui_update_cursor)
{
    PtSetResource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    ଯଦି (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    ଶୂନ୍ୟ
gui_mch_beep(ଶୂନ୍ୟ)
{
    PtBeep();
}

    ଶୂନ୍ୟ
gui_mch_flash(ପୂର୍ଣ୍ଣସଂଖ୍ୟା msec)
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

    ଶୂନ୍ୟ
gui_mch_flush(ଶୂନ୍ୟ)
{
    PgFlush();
}

    ଶୂନ୍ୟ
gui_mch_set_text_area_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    PhArea_t area = {{x, y}, {w, h}};

    PtSetResource(gui.vimTextArea, Pt_ARG_AREA, &area, 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_haskey(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym != 0; i++)
	ଯଦି (name[0] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code0 &&
		 name[1] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code1)
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#ifdef FEAT_TOOLBAR
# include "toolbar.phi"

ସ୍ଥିର PhImage_t *gui_ph_toolbar_images[] = {
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

ସ୍ଥିର PhImage_t *
gui_ph_toolbar_load_icon(char_u *iconfile)
{
    ସ୍ଥିର PhImage_t external_icon;
    PhImage_t *temp_phi = ଖାଲି;

    temp_phi = PxLoadImage(iconfile, ଖାଲି);
    ଯଦି (temp_phi != ଖାଲି)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	temp_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	memcpy(&external_icon, temp_phi, ଆକାର(external_icon));
	ମୁକ୍ତ(temp_phi);

	temp_phi = &external_icon;
    }
    ଫେରନ୍ତୁ temp_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
ସ୍ଥିର PhImage_t *
gui_ph_toolbar_find_icon(vimmenu_T *menu)
{
    char_u full_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = ଖାଲି;

    ଯଦି (menu->icon_builtin == FALSE)
    {
	ଯଦି (menu->iconfile != ଖାଲି)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_load_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	ଯଦି ((icon == ଖାଲି) &&
	    ((gui_find_bitmap(menu->name, full_pathname, "gif") == OK) ||
	      (gui_find_bitmap(menu->name, full_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_load_icon(full_pathname);

	ଯଦି (icon != ଖାଲି)
	    ଫେରନ୍ତୁ icon;
    }

    ଯଦି (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_LENGTH(gui_ph_toolbar_images)))
	ଫେରନ୍ତୁ gui_ph_toolbar_images[menu->iconidx];

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_MENU)
    ଶୂନ୍ୟ
gui_mch_enable_menu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    ଯଦି (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    ଅନ୍ୟଥା
	PtUnrealizeWidget(gui.vimMenuBar);
}

    ଶୂନ୍ୟ
gui_mch_set_menu_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_ph_position_menu(PtWidget_t *widget, ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = PtWidgetChildBack(PtWidgetParent(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    ଯେତେବେଳେ (traverse != ଖାଲି)
    {
	PtGetResource(traverse, Pt_ARG_POINTER, &menu, 0);

	ଯଦି (menu != ଖାଲି &&
		priority < menu->priority &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_ଭର୍ତ୍ତି(widget, traverse, 1);
	    ଫେରନ୍ତୁ;
	}

	traverse = PtWidgetBrotherInFront(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    ଶୂନ୍ୟ
gui_mch_add_menu(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_ଚାବି;
    char_u	mnemonic_str[MB_LEN_MAX];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n;
    PtArg_t args[5];

    menu->submenu_id = menu->id = ଖାଲି;

    ଯଦି (menu_is_menubar(menu->name))
    {

	accel_ଚାବି = vim_strchr(menu->name, '&');
	ଯଦି (accel_ଚାବି != ଖାଲି)
	{
	    mnemonic_str[0] = accel_ଚାବି[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);
	ଯଦି (accel_ଚାବି != ଖାଲି)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_ଚାବି, mnemonic_str, 0);
	PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

	ଯଦି (parent != ଖାଲି)
	    PtSetArg(&args[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_ଡାହାଣ, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(parent == ଖାଲି) ? gui.vimMenuBar : parent->submenu_id,
		n, args);

	PtAddCallback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	ଯଦି (parent != ଖାଲି)
	    PtSetArg(&args[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_CHILD);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, args);

	ଯଦି (parent == ଖାଲି)
	{
	    PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    ଯଦି (menu->mnemonic != 0)
	    {
		PtAddHotkeyHandler(gui.vimWindow, SAFE_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->priority);

	// Redraw menubar here instead of gui_mch_draw_menubar
	ଯଦି (gui.menu_is_active)
	    PtRealizeWidget(menu->id);
    }
    ଅନ୍ୟଥା ଯଦି (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vimWindow, 0, ଖାଲି);
	PtAddCallback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    ଶୂନ୍ୟ
gui_mch_add_menu_item(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
    vimmenu_T	*parent = menu->parent;
    char_u	*accel_ଚାବି;
    char_u	mnemonic_str[MB_LEN_MAX];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n;
    PtArg_t args[13];

    n = 0;
    PtSetArg(&args[ n++ ], Pt_ARG_POINTER, menu, 0);

# ifdef FEAT_TOOLBAR
    ଯଦି (menu_is_toolbar(parent->name))
    {
	ଯଦି (menu_is_separator(menu->name))
	{
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    PtSetArg(&args[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_IN, 0);
	    PtSetArg(&args[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_ଶୀର୍ଷ_BOTTOM);
	    PtSetArg(&args[ n++ ], Pt_ARG_WIDTH, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, args);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (strstr((ଅଚଳ ଅକ୍ଷର *) p_toolbar, "text") != ଖାଲି)
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_FONT, "TextFont08", 0);
	    }
	    ଯଦି ((strstr((ଅଚଳ ଅକ୍ଷର *) p_toolbar, "icons") != ଖାଲି) &&
		    (gui_ph_toolbar_images != ଖାଲି))
	    {
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_find_icon(menu), 0);
		PtSetArg(&args[ n++ ], Pt_ARG_LABEL_TYPE, Pt_TEXT_IMAGE, 0);
		PtSetArg(&args[ n++ ], Pt_ARG_TEXT_IMAGE_SPACING, 0, 0);
	    }
	    ଯଦି (strstr((ଅଚଳ ଅକ୍ଷର *) p_toolbar, "tooltips") != ଖାଲି)
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
	ଯଦି (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    ଅନ୍ୟଥା
# endif
	ଯଦି (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, parent->submenu_id, n, args);
    }
    ଅନ୍ୟଥା
    {
	accel_ଚାବି = vim_strchr(menu->name, '&');
	ଯଦି (accel_ଚାବି != ଖାଲି)
	{
	    mnemonic_str[0] = accel_ଚାବି[1];
	    mnemonic_str[1] = NUL;
	}

	PtSetArg(&args[ n++ ], Pt_ARG_TEXT_STRING, menu->dname, 0);
	ଯଦି (accel_ଚାବି != ଖାଲି)
	    PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_ଚାବି, mnemonic_str,
		    0);

	PtSetArg(&args[ n++ ], Pt_ARG_ACCEL_TEXT, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, parent->submenu_id, n, args);

	PtAddCallback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_GROUP
	ଯଦି (gui_ph_is_buffer_item(menu, parent) == TRUE)
	{
	    PtAddCallback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_buffer_remove, menu);
	    gui_ph_pg_add_buffer(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->priority);
}

    ଶୂନ୍ୟ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    ଯଦି (menu->submenu_id != ଖାଲି)
	PtDestroyWidget(menu->submenu_id);
    ଯଦି (menu->id != ଖାଲି)
	PtDestroyWidget(menu->id);

    menu->submenu_id = ଖାଲି;
    menu->id = ଖାଲି;
}

    ଶୂନ୍ୟ
gui_mch_menu_grey(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା grey)
{
    long    flags, mask, fields;

    ଯଦି (menu->id == ଖାଲି)
	ଫେରନ୍ତୁ;

    flags = PtWidgetFlags(menu->id);
    ଯଦି (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(PtWidgetParent(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	mask = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    ଅନ୍ୟଥା
    {
	fields = Pt_TRUE;
	mask = Pt_BLOCKED | Pt_GHOST;
    }

    ଯଦି (! grey)
	fields = ~fields;

    PtSetResource(menu->id, Pt_ARG_FLAGS, fields,
	    mask);
}

    ଶୂନ୍ୟ
gui_mch_menu_hidden(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hidden)
{
    // TODO: [un]realize the widget?
}

    ଶୂନ୍ୟ
gui_mch_draw_menubar(ଶୂନ୍ୟ)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    ଶୂନ୍ୟ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    PtSetResource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    ଶୂନ୍ୟ
gui_mch_toggle_tearoffs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable)
{
    // No tearoffs yet
}

#endif

#ଯଦି defined(FEAT_TOOLBAR)
    ଶୂନ୍ୟ
gui_mch_show_toolbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    ଯଦି (showit)
	PtRealizeWidget(gui.vimToolBar);
    ଅନ୍ୟଥା
	PtUnrealizeWidget(gui.vimToolBar);
}
#endif

////////////////////////////////////////////////////////////////////////////
// Fonts

    ସ୍ଥିର GuiFont
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
    ଯଦି (font_tag != ଖାଲି)
    {
	ଯଦି (PfGenerateFontName(font_name, font_flags, font_size,
		    font_tag) != ଖାଲି)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    ଯଦି (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    ଯଦି (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    ଯଦି (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_tag, &info);

	    ଯଦି (info.size == 0)
		font_size = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    ଯଦି (font_size == info.size &&
		    style == (info.style & style))
		ଫେରନ୍ତୁ (GuiFont)font_tag;
	}
	vim_free(font_tag);
    }
    ଫେରନ୍ତୁ ଖାଲି;
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

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_size)
{
    char_u  *mark;
    int_u   name_len, size;

    mark = vim_strchr(vim_font, ':');
    ଯଦି (mark == ଖାଲି)
	name_len = STRLEN(vim_font);
    ଅନ୍ୟଥା
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    ଯଦି (*font_name != ଖାଲି)
    {
	ଯଦି (mark != ଖାଲି)
	{
	    ଯେତେବେଳେ (*mark != NUL && *mark++ == ':')
	    {
		ବାଛ (SAFE_tolower(*mark++))
		{
		    ସ୍ଥିତି 'a': *font_flags |= PF_STYLE_ANTIALIAS; ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'b': *font_flags |= PF_STYLE_BOLD; ଭାଙ୍ଗ;
		    ସ୍ଥିତି 'i': *font_flags |= PF_STYLE_ITALIC; ଭାଙ୍ଗ;

		    ସ୍ଥିତି 's':
			size = getdigits(&mark);
			// Restrict the size to some vague limits
			ଯଦି (size < 1 || size > 100)
			    size = 8;

			*font_size = size;
			ଭାଙ୍ଗ;

		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
	    }
	}
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_font(char_u *vim_font_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fontset)
{
    char_u  *font_tag;
    char_u  *font_name = ଖାଲି;
    int_u   font_flags = 0;
    int_u   font_size  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    ଯଦି (vim_font_name == ଖାଲି)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    ଯଦି (STRCMP(vim_font_name, "*") == 0)
    {
	font_tag = PtFontSelection(gui.vimWindow, ଖାଲି, ଖାଲି,
		"pcterm12", -1, PHFONT_FIXED, ଖାଲି);

	ଯଦି (font_tag == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_tag;

	PfQueryFontInfo(font_tag, &info);
	font_name = vim_strsave(info.font);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_size) == FALSE)
	    ଫେରନ୍ତୁ FAIL;

	font_tag = gui_ph_get_font(font_name, font_flags, font_size, 0);
	ଯଦି (font_tag == ଖାଲି)
	{
	    vim_free(font_name);
	    ଫେରନ୍ତୁ FAIL;
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
    PfExtentText(&extent, ଖାଲି, font_tag, "g", 1);

    gui.char_width = extent.lr.x - extent.ul.x + 1;
    gui.char_height = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    ଫେରନ୍ତୁ OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_adjust_charheight(ଶୂନ୍ୟ)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_height = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    ଫେରନ୍ତୁ OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା report_error)
{
    char_u  *font_name;
    char_u  *font_tag;
    int_u   font_size = 12;
    int_u   font_flags = 0;

    ଯଦି (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_size) != FALSE)
    {
	font_tag = gui_ph_get_font(font_name, font_flags, font_size, -1);
	vim_free(font_name);

	ଯଦି (font_tag != ଖାଲି)
	    ଫେରନ୍ତୁ (GuiFont)font_tag;
    }

    ଯଦି (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    ଫେରନ୍ତୁ FAIL;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strsave(name);
}
#endif

    ଶୂନ୍ୟ
gui_mch_set_font(GuiFont font)
{
    PgSetFont(font);
}

    ଶୂନ୍ୟ
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

