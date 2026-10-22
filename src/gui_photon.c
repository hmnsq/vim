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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_TOOLBAR
# include <photon/PxImage.h>
#అయితే_ముగింపు

#ఒకవేళ !defined(__QNX__)
// Used when generating prototypes.
# define PgColor_t	పూర్ణాంకం
# define PhEvent_t	పూర్ణాంకం
# define Ph_బిందువు_t	పూర్ణాంకం
# define PtWidget_t	పూర్ణాంకం
# define Pg_BLACK	0
# define PtCallbackF_t	పూర్ణాంకం
# define PtCallbackInfo_t పూర్ణాంకం
# define PhTile_t	పూర్ణాంకం
# define PtWidget_t	పూర్ణాంకం
# define PhImage_t	పూర్ణాంకం
#అయితే_ముగింపు

#నిర్వచించు RGB(r, g, b) PgRGB(r, g, b)

#నిర్వచించు EVENT_బఫరు_కొలత పరిమాణం(PhEvent_t) + 1000

// Some defines for gui_mch_mousehide()
#నిర్వచించు MOUSE_HIDE		TRUE
#నిర్వచించు MOUSE_SHOW		FALSE

// Optional support for using a PtPanelGroup widget, needs work
#నిర్వచనం_రద్దు USE_PANEL_సమూహం

#నిర్వచితమైతే USE_PANEL_సమూహం
స్థిర అక్షరం	*ఖాళీలు_title = "    ";
స్థిర అక్షరం	**panel_titles = ఖాళీ;
స్థిర ushort_t	అంకె_panels = 0;
స్థిర పొట్టి pg_margin_ఎడమ, pg_margin_కుడి, pg_margin_శిఖరం, pg_margin_bottom;
#అయితే_ముగింపు

#నిర్వచించు GUI_PH_MARGIN		4	// Size of the bevel

#నిర్వచించు GUI_PH_MOUSE_TYPE		Ph_CURSOR_చొప్పించు
స్థిర PgColor_t gui_ph_mouse_color =	Pg_BLACK;

స్థిర Ph_బిందువు_t    gui_ph_raw_స్థానభ్రంశం;
స్థిర PtWidget_t   *gui_ph_timer_cursor;   // handle cursor blinking
స్థిర PtWidget_t   *gui_ph_timer_timeout;  // used in gui_mch_wait_for_chars
స్థిర పొట్టి	    is_timeout;		    // Has the timeout occurred?

/*
 * This is set inside the mouse callback for a right mouse
 * button click, and used for the popup menus
 */
స్థిర Ph_బిందువు_t    abs_mouse;

// Try and avoid redraws while a resize is in progress
స్థిర పూర్ణాంకం is_ignore_draw = FALSE;

// Used for converting to/from utf-8 and other charsets
స్థిర నిర్మాణం PxTransCtrl *charset_translate;

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */
స్థిర గణన {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

స్థిర long_u	blink_waittime	= 700;
స్థిర long_u	blink_ontime	= 400;
స్థిర long_u	blink_offtime	= 250;

స్థిర నిర్మాణం
{
    పూర్ణాంకం	    కీలకం_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_కీలకాలు[] =
{
    {Pk_పైకి,	    'k', 'u'},
    {Pk_కిందికి,	    'k', 'd'},
    {Pk_ఎడమ,	    'k', 'l'},
    {Pk_కుడి,	    'k', 'r'},

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
    {Pk_వెనుక_Space,  'k', 'b'},
    {Pk_చొప్పించు,	    'k', 'I'},
    {Pk_Delete,	    'k', 'D'},
    {Pk_Home,	    'k', 'h'},
    {Pk_చివర,	    '@', '7'},
    {Pk_పూర్వ,	    'k', 'P'},
    {Pk_తదుపరి,	    'k', 'N'},
    {Pk_Print,	    '%', '9'},

    {Pk_KP_కూడు,	    'K', '6'},
    {Pk_KP_వ్యవకలనం,'K', '7'},
    {Pk_KP_Divide,  'K', '8'},
    {Pk_KP_Multiply,'K', '9'},
    {Pk_KP_Enter,   'K', 'A'},

    {Pk_KP_0,	    KS_EXTRA, KE_KINS}, //చొప్పించు
    {Pk_KP_Decimal, KS_EXTRA, KE_KDEL}, // Delete

    {Pk_KP_4,	    'k', 'l'}, // Left
    {Pk_KP_6,	    'k', 'r'}, // Right
    {Pk_KP_8,	    'k', 'u'}, // Up
    {Pk_KP_2,	    'k', 'd'}, // Down

    {Pk_KP_7,	    'K', '1'}, // Home
    {Pk_KP_1,	    'K', '4'}, //చివర

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

స్థిర PtCallbackF_t gui_ph_handle_timer_cursor;
స్థిర PtCallbackF_t gui_ph_handle_timer_timeout;

స్థిర PtCallbackF_t gui_ph_handle_కిటికీ_cb;

స్థిర PtCallbackF_t gui_ph_handle_scrollbar;
స్థిర PtCallbackF_t gui_ph_handle_keyboard;
స్థిర PtCallbackF_t gui_ph_handle_mouse;
స్థిర PtCallbackF_t gui_ph_handle_pulldown_menu;
స్థిర PtCallbackF_t gui_ph_handle_menu;
స్థిర PtCallbackF_t gui_ph_handle_focus;	// focus change of text area

స్థిర PtCallbackF_t gui_ph_handle_menu_resize;

// When a menu is unrealized, give focus back to vimTextArea
స్థిర PtCallbackF_t gui_ph_handle_menu_unrealized;

#నిర్వచితమైతే USE_PANEL_సమూహం
స్థిర శూన్య gui_ph_get_panelgroup_margins(పొట్టి*, పొట్టి*, పొట్టి*, పొట్టి*);
#అయితే_ముగింపు

స్థిర శూన్య gui_ph_draw_ప్రారంభం(శూన్య);
స్థిర శూన్య gui_ph_draw_చివర(శూన్య);

// Set the text for the balloon
స్థిర PtWidget_t * gui_ph_show_tooltip(PtWidget_t *కిటికీ_2,
			     PtWidget_t *widget,
			     పూర్ణాంకం position,
			     అక్షరం *పాఠ్యం,
			     అక్షరం *font,
			     PgColor_t fill_color,
			     PgColor_t పాఠ్యం_color);

////////////////////////////////////////////////////////////////////////////

స్థిర PtWidget_t * gui_ph_show_tooltip(PtWidget_t *కిటికీ_2,
			     PtWidget_t *widget,
			     పూర్ణాంకం position,
			     అక్షరం *పాఠ్యం,
			     అక్షరం *font,
			     PgColor_t fill_color,
			     PgColor_t పాఠ్యం_color)
{
    PtArg_t arg;
    vimmenu_T *menu;
    char_u  *tooltip;

    Pt_సమితి_Arg(&arg, Pt_ARG_సూచిక, &menu, 0);
    PtGetResources(widget, 1, &arg);

    // Override the text and position

    tooltip = పాఠ్యం;
    ఒకవేళ (menu != ఖాళీ)
    {
	పూర్ణాంకం సూచీ = MENU_సూచీ_TIP;
	ఒకవేళ (menu->అక్షరశ్రేణులు[ సూచీ ] != ఖాళీ)
	    tooltip = menu->అక్షరశ్రేణులు[ సూచీ ];
    }

    తిరిగి PtInflateBalloon(
	    కిటికీ_2,
	    widget,
	    // Don't put the balloon at the bottom,
	    // it gets drawn over by gfx done in the PtRaw
	    Pt_BALLOON_శిఖరం,
	    tooltip,
	    font,
	    fill_color,
	    పాఠ్యం_color);
}

    స్థిర శూన్య
gui_ph_resize_container(శూన్య)
{
    PhArea_t area;

    PtWidgetArea(gui.vim_కిటికీ, &area);
    PtWidgetPos (gui.vimContainer, &area.pos);

    Pt_సమితి_Resource(gui.vimContainer, Pt_ARG_AREA, &area, 0);
}

    స్థిర పూర్ణాంకం
gui_ph_handle_menu_resize(
	PtWidget_t *widget,
	శూన్య *other,
	PtCallbackInfo_t *info)
{
    PtContainerCallback_t *sizes = info->cbdata;
    PtWidget_t		*container;
    Ph_బిందువు_t		క్రింద_menu;
    int_u		ఎత్తు;

    ఎత్తు = sizes->new_dim.h;

    // Because vim treats the toolbar and menubar separately,
    // and here they're lumped together into a PtToolbarGroup,
    // we only need either menu_height or toolbar_height set at once
    ఒకవేళ (gui.menu_is_చురుకైన)
    {
	gui.menu_ఎత్తు = ఎత్తు;
	gui.toolbar_ఎత్తు = 0;
    }
#నిర్వచితమైతే FEAT_TOOLBAR
    లేకపోతే
	gui.toolbar_ఎత్తు = ఎత్తు;
#అయితే_ముగింపు

    క్రింద_menu.x = 0;
    క్రింద_menu.y = ఎత్తు;

#నిర్వచితమైతే USE_PANEL_సమూహం
    container = gui.vim_Panel_సమూహం;
#లేకపోతే
    container = gui.vimContainer;
#అయితే_ముగింపు

    Pt_సమితి_Resource(container, Pt_ARG_POS, &క్రింద_menu, 0);

    gui_ph_resize_container();

#నిర్వచితమైతే USE_PANEL_సమూహం
    gui_ph_get_panelgroup_margins(
	    &pg_margin_శిఖరం, &pg_margin_bottom,
	    &pg_margin_ఎడమ, &pg_margin_కుడి);
#అయితే_ముగింపు
    తిరిగి Pt_CONTINUE;
}

/*
 * Pt_ARG_TIMER_REPEAT isn't used because the on & off times
 * are different
 */
    స్థిర పూర్ణాంకం
gui_ph_handle_timer_cursor(
	PtWidget_t *widget,
	శూన్య *దత్తాంశం,
	PtCallbackInfo_t *info)
{
    ఒకవేళ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	Pt_సమితి_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_offtime, 0);
    }
    లేకపోతే
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	Pt_సమితి_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_ontime, 0);
    }
    తిరిగి Pt_CONTINUE;
}

    స్థిర పూర్ణాంకం
gui_ph_handle_timer_timeout(
    PtWidget_t		*widget,
    శూన్య		*దత్తాంశం,
    PtCallbackInfo_t	*info)
{
    is_timeout = TRUE;

    తిరిగి Pt_CONTINUE;
}

    స్థిర పూర్ణాంకం
gui_ph_handle_కిటికీ_cb(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    Ph_కిటికీ_Event_t *we = info->cbdata;
    ushort_t *వెడల్పు, *ఎత్తు;

    ఎంపిక (we->event_f)
    {
	స్థితి Ph_WM_CLOSE:
	    gui_shell_closed();
	    ఆపు;

	స్థితి Ph_WM_FOCUS:
	    // Just in case it's hidden and needs to be shown
	    gui_mch_mousehide(MOUSE_SHOW);

	    ఒకవేళ (we->event_state == Ph_WM_EVSTATE_FOCUS)
	    {
		gui_focus_change(TRUE);
		gui_mch_ప్రారంభం_blink();
	    }
	    లేకపోతే
	    {
		gui_focus_change(FALSE);
		gui_mch_stop_blink(TRUE);
	    }
	    ఆపు;

	స్థితి Ph_WM_RESIZE:
	    PtGetResource(gui.vim_కిటికీ, Pt_ARG_వెడల్పు, &వెడల్పు, 0);
	    PtGetResource(gui.vim_కిటికీ, Pt_ARG_ఎత్తు, &ఎత్తు, 0);
#నిర్వచితమైతే USE_PANEL_సమూహం
	    వెడల్పు  -= (pg_margin_ఎడమ + pg_margin_కుడి);
	    ఎత్తు -= (pg_margin_శిఖరం + pg_margin_bottom);
#అయితే_ముగింపు
	    gui_resize_shell(*వెడల్పు, *ఎత్తు);
	    gui_సమితి_shellsize(FALSE, FALSE, RESIZE_BOTH);
	    is_ignore_draw = FALSE;
	    Pt_చివర_Flux(gui.vimContainer);
	    PtContainerRelease(gui.vimContainer);
	    ఆపు;

	అప్రమేయ:
	    ఆపు;
    }

    తిరిగి Pt_CONTINUE;
}

    స్థిర పూర్ణాంకం
gui_ph_handle_scrollbar(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    PtScrollbarCallback_t *scroll;
    scrollbar_T *sb;
    పూర్ణాంకం	    విలువ, dragging = FALSE;

    scroll = info->cbdata;

    sb = (scrollbar_T *) దత్తాంశం;
    ఒకవేళ (sb != ఖాళీ)
    {
	విలువ = scroll->position;
	ఎంపిక (scroll->action)
	{
	    స్థితి Pt_SCROLL_DRAGGED:
		dragging = TRUE;
		ఆపు;

	    స్థితి Pt_SCROLL_సమితి:
		// FIXME: return straight away here?
		తిరిగి Pt_CONTINUE;
		ఆపు;
	}

	gui_drag_scrollbar(sb, విలువ, dragging);
    }
    తిరిగి Pt_CONTINUE;
}

    స్థిర పూర్ణాంకం
gui_ph_handle_keyboard(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    Ph_కీలకం_Event_t    *కీలకం;
    గుర్తులేని అక్షరం   string[6];
    పూర్ణాంకం		    len, i;
    పూర్ణాంకం		    ch, modifiers;

    కీలకం = Ph_Get_దత్తాంశం(info->event);

    ch = modifiers = len = 0;

    ఒకవేళ (p_mh)
	gui_mch_mousehide(MOUSE_HIDE);

    // We're a good lil photon program, aren't we? yes we are, yeess wee arrr
    ఒకవేళ (కీలకం->కీలకం_flags & Pk_KF_Compose)
	తిరిగి Pt_CONTINUE;

    ఒకవేళ ((కీలకం->కీలకం_flags & Pk_KF_Cap_Valid) &&
	    Pk_Is_కీలకం_కిందికి(కీలకం->కీలకం_flags))
    {
#నిర్వచితమైతే FEAT_MENU
	/*
	 * Only show the menu if the Alt key is down, and the Shift & Ctrl
	 * keys aren't down, as well as the other conditions
	 */
	ఒకవేళ (((కీలకం->కీలకం_mods & Pk_KM_Alt) &&
		    !(కీలకం->కీలకం_mods & Pk_KM_జరుపు) &&
		    !(కీలకం->కీలకం_mods & Pk_KM_Ctrl)) &&
	    gui.menu_is_చురుకైన &&
	    (*p_wak == 'y' ||
	      (*p_wak == 'm' &&
		gui_is_menu_shortcut(కీలకం->కీలకం_cap))))
	{
	    // Fallthrough and let photon look for the hotkey
	    తిరిగి Pt_CONTINUE;
	}
#అయితే_ముగింపు

	కొరకు (i = 0; special_కీలకాలు[i].కీలకం_sym != 0; i++)
	{
	    ఒకవేళ (special_కీలకాలు[i].కీలకం_sym == కీలకం->కీలకం_cap)
	    {
		len = 0;
		ఒకవేళ (special_కీలకాలు[i].vim_code1 == NUL)
		    ch = special_కీలకాలు[i].vim_code0;
		లేకపోతే
		{
		    // Detect if a keypad number key has been pressed
		    // and change the key if Num Lock is on
		    ఒకవేళ (కీలకం->కీలకం_cap >= Pk_KP_Enter && కీలకం->కీలకం_cap <= Pk_KP_9
			    && (కీలకం->కీలకం_mods & Pk_KM_అంకె_తాళం))
		    {
			// FIXME: For now, just map the key to an ascii value
			// (see <photon/PkKeyDef.h>)
			ch = కీలకం->కీలకం_cap - 0xf080;
		    }
		    లేకపోతే
			ch = గమ్యం_SPECIAL(special_కీలకాలు[i].vim_code0,
				special_కీలకాలు[i].vim_code1);
		}
		ఆపు;
	    }
	}

	ఒకవేళ (కీలకం->కీలకం_mods & Pk_KM_Ctrl)
	    modifiers |= MOD_ముసుగు_CTRL;
	ఒకవేళ (కీలకం->కీలకం_mods & Pk_KM_Alt)
	    modifiers |= MOD_ముసుగు_ALT;
	ఒకవేళ (కీలకం->కీలకం_mods & Pk_KM_జరుపు)
	    modifiers |= MOD_ముసుగు_జరుపు;

	// Is this not a special key?
	ఒకవేళ (special_కీలకాలు[i].కీలకం_sym == 0)
	{
	    ch = PhTo8859_1(కీలకం);
	    ఒకవేళ (ch == -1 || (enc_utf8 && ch > 127))
	    {
		len = Ph_కీలకం_గమ్యం_Mb(string, కీలకం);
		ఒకవేళ (len > 0)
		{
		    స్థిర అక్షరం buf[6];
		    పూర్ణాంకం src_తీసుకున్న, dst_made;
		    ఒకవేళ (enc_utf8 != TRUE)
		    {
			PxTranslateFromUTF(
				charset_translate,
				string,
				len,
				&src_తీసుకున్న,
				buf,
				6,
				&dst_made);

			కూడు_గమ్యం_ఆదానం_buf(buf, dst_made);
		    }
		    లేకపోతే
		    {
			కూడు_గమ్యం_ఆదానం_buf(string, len);
		    }

		    తిరిగి Pt_CONSUME;
		}
		len = 0;
		ch = కీలకం->కీలకం_cap;
		ఒకవేళ (ch < 0xff)
		{
		    // FIXME: is this the right thing to do?
		    ఒకవేళ (modifiers & MOD_ముసుగు_CTRL)
		    {
			modifiers &= ~MOD_ముసుగు_CTRL;

			ఒకవేళ ((ch >= 'a' && ch <= 'z') ||
				ch == '[' ||
				ch == ']' ||
				ch == '\\')
			    ch = Ctrl_chr(ch);
			లేకపోతే ఒకవేళ (ch == '2')
			    ch = NUL;
			లేకపోతే ఒకవేళ (ch == '6')
			    ch = 0x1e;
			లేకపోతే ఒకవేళ (ch == '-')
			    ch = 0x1f;
			లేకపోతే
			    modifiers |= MOD_ముసుగు_CTRL;
		    }

		    ఒకవేళ (modifiers & MOD_ముసుగు_ALT)
		    {
			ch = Meta(ch);
			modifiers &= ~MOD_ముసుగు_ALT;
		    }
		}
		లేకపోతే
		{
		    తిరిగి Pt_CONTINUE;
		}
	    }
	    లేకపోతే
		modifiers &= ~MOD_ముసుగు_జరుపు;
	}

	ch = simplify_కీలకం(ch, &modifiers);
	ఒకవేళ (modifiers)
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = KS_MODIFIER;
	    string[ len++ ] = modifiers;
	}

	ఒకవేళ (IS_SPECIAL(ch))
	{
	    string[ len++ ] = CSI;
	    string[ len++ ] = K_రెండవ(ch);
	    string[ len++ ] = K_మూడవ(ch);
	}
	లేకపోతే
	{
	    string[ len++ ] = ch;
	}

	// Check if the key interrupts.
	{
	    పూర్ణాంకం int_ch = check_for_అంతరాయం(ch, modifiers);

	    ఒకవేళ (int_ch != NUL)
	    {
		ch = int_ch;
		string[0] = ch;
		len = 1;
		trash_ఆదానం_buf();
	    }
	}

	ఒకవేళ (len == 1 && string[0] == CSI)
	{
	    // Turn CSI into K_CSI.
	    string[ len++ ] = KS_EXTRA;
	    string[ len++ ] = KE_CSI;
	}

	ఒకవేళ (len > 0)
	{
	    కూడు_గమ్యం_ఆదానం_buf(string, len);
	    తిరిగి Pt_CONSUME;
	}
    }

    తిరిగి Pt_CONTINUE;
}

    స్థిర పూర్ణాంకం
gui_ph_handle_mouse(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    Ph_సూచిక_Event_t *సూచిక;
    PhRect_t	     *pos;
    పూర్ణాంకం		     button = 0, repeated_click, modifiers = 0x0;
    పొట్టి	     mouse_x, mouse_y;

    సూచిక = Ph_Get_దత్తాంశం(info->event);
    pos = PhGetRects(info->event);

    gui_mch_mousehide(MOUSE_SHOW);

    /*
     * Coordinates need to be relative to the base window,
     * not relative to the vimTextArea widget
     */
    mouse_x = pos->ul.x + gui.border_వెడల్పు;
    mouse_y = pos->ul.y + gui.border_వెడల్పు;

    ఒకవేళ (info->event->type == Ph_EV_PTR_MOTION_NOBUTTON)
    {
	gui_mouse_moved(mouse_x, mouse_y);
	తిరిగి Pt_CONTINUE;
    }

    ఒకవేళ (సూచిక->కీలకం_mods & Pk_KM_జరుపు)
	modifiers |= MOUSE_జరుపు;
    ఒకవేళ (సూచిక->కీలకం_mods & Pk_KM_Ctrl)
	modifiers |= MOUSE_CTRL;
    ఒకవేళ (సూచిక->కీలకం_mods & Pk_KM_Alt)
	modifiers |= MOUSE_ALT;

    /*
     * FIXME More than one button may be involved, but for
     * now just deal with one
     */
    ఒకవేళ (సూచిక->buttons & Ph_BUTTON_SELECT)
	button = MOUSE_ఎడమ;

    ఒకవేళ (సూచిక->buttons & Ph_BUTTON_MENU)
    {
	button = MOUSE_కుడి;
	// Need the absolute coordinates for the popup menu
	abs_mouse.x = సూచిక->pos.x;
	abs_mouse.y = సూచిక->pos.y;
    }

    ఒకవేళ (సూచిక->buttons & Ph_BUTTON_ADJUST)
	button = MOUSE_మధ్య;

    // Catch a real release (not phantom or other releases
    ఒకవేళ (info->event->type == Ph_EV_BUT_RELEASE)
	button = MOUSE_RELEASE;

    ఒకవేళ (info->event->type & Ph_EV_PTR_MOTION_BUTTON)
	button = MOUSE_DRAG;

#ఒకవేళ 0
    // Vim doesn't use button repeats
    ఒకవేళ (info->event->type & Ph_EV_BUT_REPEAT)
	button = MOUSE_DRAG;
#అయితే_ముగింపు

    // Don't do anything if it is one of the phantom mouse release events
    ఒకవేళ ((button != MOUSE_RELEASE) ||
	    (info->event->subtype == Ph_EV_RELEASE_REAL))
    {
	repeated_click = (సూచిక->click_లెక్క >= 2) ? TRUE : FALSE;

	gui_send_mouse_event(button , mouse_x, mouse_y, repeated_click, modifiers);
    }

    తిరిగి Pt_CONTINUE;
}

/*
 * Handle a focus change of the PtRaw widget
 */
    స్థిర పూర్ణాంకం
gui_ph_handle_focus(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    ఒకవేళ (info->reason == Pt_CB_కోల్పోయిన_FOCUS)
    {
	PtRemoveEventHandler(gui.vim_పాఠ్యం_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ఖాళీ);

	gui_mch_mousehide(MOUSE_SHOW);
    }
    లేకపోతే
    {
	Pt_కూడు_Event_Handler(gui.vim_పాఠ్యం_Area, Ph_EV_PTR_MOTION_NOBUTTON,
		gui_ph_handle_mouse, ఖాళీ);
    }
    తిరిగి Pt_CONTINUE;
}

    స్థిర శూన్య
gui_ph_handle_raw_draw(PtWidget_t *widget, PhTile_t *damage)
{
    PhRect_t	*r;
    Ph_బిందువు_t	స్థానభ్రంశం;
    Ph_బిందువు_t	translation;

    ఒకవేళ (is_ignore_draw == TRUE)
	తిరిగి;

    PtSuperClassDraw(PtBasic, widget, damage);
    PgGetTranslation(&translation);
    PgClearTranslation();

#ఒకవేళ 0
    /*
     * This causes some weird problems, with drawing being done from
     * within this raw drawing function (rather than just simple clearing
     * and text drawing done by gui_redraw)
     *
     * The main problem is when PhBlit is used, and the cursor appearing
     * in places where it shouldn't
     */
    బహిర్గతం_flush();
#అయితే_ముగింపు

    Pt_Widget_స్థానభ్రంశం(widget, &స్థానభ్రంశం);
    Ph_Translate_బిందువు(&స్థానభ్రంశం, PtWidgetPos(gui.vim_పాఠ్యం_Area, ఖాళీ));

#ఒకవేళ 1
    // Redraw individual damage regions
    ఒకవేళ (damage->తదుపరి != ఖాళీ)
	damage = damage->తదుపరి;

    అయితే (damage != ఖాళీ)
    {
	r = &damage->rect;
	gui_redraw(
		r->ul.x - స్థానభ్రంశం.x, r->ul.y - స్థానభ్రంశం.y,
		r->lr.x - r->ul.x + 1,
		r->lr.y - r->ul.y + 1);
	damage = damage->తదుపరి;
    }
#లేకపోతే
    // Redraw the rectangle that covers all the damaged regions
    r = &damage->rect;
    gui_redraw(
	    r->ul.x - స్థానభ్రంశం.x, r->ul.y - స్థానభ్రంశం.y,
	    r->lr.x - r->ul.x + 1,
	    r->lr.y - r->ul.y + 1);
#అయితే_ముగింపు

    Pg_సమితి_Translation(&translation, 0);
}

    స్థిర పూర్ణాంకం
gui_ph_handle_pulldown_menu(
	PtWidget_t *widget,
	శూన్య *దత్తాంశం,
	PtCallbackInfo_t *info)
{
    ఒకవేళ (దత్తాంశం != ఖాళీ)
    {
	vimmenu_T *menu = (vimmenu_T *) దత్తాంశం;

	PtPositionMenu(menu->submenu_id, ఖాళీ);
	PtRealizeWidget(menu->submenu_id);
    }

    తిరిగి Pt_CONTINUE;
}

/*
 * This is used for pulldown/popup menus and also toolbar buttons
 */
    స్థిర పూర్ణాంకం
gui_ph_handle_menu(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    ఒకవేళ (దత్తాంశం != ఖాళీ)
    {
	vimmenu_T *menu = (vimmenu_T *) దత్తాంశం;
	gui_menu_cb(menu);
    }
    తిరిగి Pt_CONTINUE;
}

/*
 * Stop focus from disappearing into the menubar...
 */
    స్థిర పూర్ణాంకం
gui_ph_handle_menu_unrealized(
	PtWidget_t *widget,
	శూన్య *దత్తాంశం,
	PtCallbackInfo_t *info)
{
    PtGiveFocus(gui.vim_పాఠ్యం_Area, ఖాళీ);
    తిరిగి Pt_CONTINUE;
}

    స్థిర పూర్ణాంకం
gui_ph_handle_కిటికీ_open(
	PtWidget_t *widget,
	శూన్య *దత్తాంశం,
	PtCallbackInfo_t *info)
{
    gui_సమితి_shellsize(FALSE, TRUE, RESIZE_BOTH);
    తిరిగి Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

#నిర్వచించు DRAW_ప్రారంభం  gui_ph_draw_ప్రారంభం()
#నిర్వచించు DRAW_చివర    gui_ph_draw_చివర()

/*
 * TODO: Set a clipping rect?
 */
    స్థిర శూన్య
gui_ph_draw_ప్రారంభం(శూన్య)
{
    PhGC_t *gc;

    gc = PgGetGC();
    Pg_సమితి_Region(PtWidgetRid(Pt_కనుగొను_Disjoint(gui.vim_పాఠ్యం_Area)));
    PgClearClippingsCx(gc);
    PgClearTranslationCx(gc);

    Pt_Widget_స్థానభ్రంశం(gui.vim_పాఠ్యం_Area, &gui_ph_raw_స్థానభ్రంశం);
    Ph_Translate_బిందువు(&gui_ph_raw_స్థానభ్రంశం, PtWidgetPos(gui.vim_పాఠ్యం_Area, ఖాళీ));

    Pg_సమితి_Translation(&gui_ph_raw_స్థానభ్రంశం, Pg_RELATIVE);
}

    స్థిర శూన్య
gui_ph_draw_చివర(శూన్య)
{
    gui_ph_raw_స్థానభ్రంశం.x = -gui_ph_raw_స్థానభ్రంశం.x;
    gui_ph_raw_స్థానభ్రంశం.y = -gui_ph_raw_స్థానభ్రంశం.y;
    Pg_సమితి_Translation(&gui_ph_raw_స్థానభ్రంశం, Pg_RELATIVE);
}

#నిర్వచితమైతే USE_PANEL_సమూహం
    స్థిర vimmenu_T *
gui_ph_కనుగొను_బఫరు_అంశం(char_u *name)
{
    vimmenu_T *శిఖరం_level = మూలం_menu;
    vimmenu_T *వస్తువులు = ఖాళీ;

    అయితే (శిఖరం_level != ఖాళీ &&
	    (STRCMP(శిఖరం_level->dname, "Buffers") != 0))
	శిఖరం_level = శిఖరం_level->తదుపరి;

    ఒకవేళ (శిఖరం_level != ఖాళీ)
    {
	వస్తువులు = శిఖరం_level->children;

	అయితే (వస్తువులు != ఖాళీ &&
		(STRCMP(వస్తువులు->dname, name) != 0))
	    వస్తువులు = వస్తువులు->తదుపరి;
    }
    తిరిగి వస్తువులు;
}

    స్థిర శూన్య
gui_ph_pg_సమితి_బఫరు_అంకె(int_u buf_అంకె)
{
    పూర్ణాంకం i;
    అక్షరం శోధన[16];
    అక్షరం *mark;

    ఒకవేళ (gui.vim_పాఠ్యం_Area == ఖాళీ || buf_అంకె == 0)
	తిరిగి;

    శోధన[0] = '(';
    ultoa(buf_అంకె, &శోధన[1], 10);
    STRCAT(శోధన, ")");

    కొరకు (i = 0; i < అంకె_panels; i++)
    {
	// find the last "(" in the panel title and see if the buffer
	// number in the title matches the one we're looking for
	mark = STRRCHR(panel_titles[ i ], '(');
	ఒకవేళ (mark != ఖాళీ && STRCMP(mark, శోధన) == 0)
	{
	    Pt_సమితి_Resource(gui.vim_Panel_సమూహం, Pt_ARG_PG_CURRENT_సూచీ,
		    i, 0);
	}
    }
}

    స్థిర పూర్ణాంకం
gui_ph_handle_pg_change(
	PtWidget_t *widget,
	శూన్య *దత్తాంశం,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;
    Pt_Panel_సమూహం_Callback_t *panel;

    ఒకవేళ (info->event != ఖాళీ)
    {
	panel = info->cbdata;
	ఒకవేళ (panel->new_panel != ఖాళీ)
	{
	    menu = gui_ph_కనుగొను_బఫరు_అంశం(panel->new_panel);
	    ఒకవేళ (menu)
		gui_menu_cb(menu);
	}
    }
    తిరిగి Pt_CONTINUE;
}

    స్థిర శూన్య
gui_ph_get_panelgroup_margins(
	పొట్టి *శిఖరం,
	పొట్టి *bottom,
	పొట్టి *ఎడమ_2,
	పొట్టి *కుడి_2)
{
    గుర్తులేని పొట్టి abs_raw_x, abs_raw_y, abs_panel_x, abs_panel_y;
    స్థిరరాశి గుర్తులేని పొట్టి *margin_శిఖరం, *margin_bottom;
    స్థిరరాశి గుర్తులేని పొట్టి *margin_ఎడమ, *margin_కుడి;

    PtGetAbsPosition(gui.vim_పాఠ్యం_Area, &abs_raw_x, &abs_raw_y);
    PtGetAbsPosition(gui.vim_Panel_సమూహం, &abs_panel_x, &abs_panel_y);

    PtGetResource(gui.vim_Panel_సమూహం, Pt_ARG_MARGIN_కుడి, &margin_కుడి, 0);
    PtGetResource(gui.vim_Panel_సమూహం, Pt_ARG_MARGIN_BOTTOM, &margin_bottom, 0);

    abs_raw_x -= abs_panel_x;
    abs_raw_y -= abs_panel_y;

    *శిఖరం    = abs_raw_y;
    *bottom = *margin_bottom;

    *ఎడమ_2  = abs_raw_x;
    *కుడి_2 = *margin_కుడి;
}

/*
 * Used for the tabs for PtPanelGroup
 */
    స్థిర పూర్ణాంకం
gui_ph_is_బఫరు_అంశం(vimmenu_T *menu, vimmenu_T *మాతృ)
{
    అక్షరం *mark;

    ఒకవేళ (STRCMP(మాతృ->dname, "Buffers") == 0)
    {
	// Look for '(' digits ')'
	mark = vim_strchr(menu->dname, '(');
	ఒకవేళ (mark != ఖాళీ)
	{
	    mark++;
	    అయితే (సురక్షిత_isdigit(*mark))
		mark++;

	    ఒకవేళ (*mark == ')')
		తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

    స్థిర శూన్య
gui_ph_pg_కూడు_బఫరు(అక్షరం *name)
{
    అక్షరం **new_titles = ఖాళీ;

    new_titles = ALLOC_MULT(అక్షరం *, (అంకె_panels + 1));
    ఒకవేళ (new_titles == ఖాళీ)
	తిరిగి;

    ఒకవేళ (అంకె_panels > 0)
	జ్ఞాపకం_నకలు(new_titles, panel_titles, అంకె_panels * పరిమాణం(అక్షరం **));

    new_titles[ అంకె_panels++ ] = name;

    Pt_సమితి_Resource(gui.vim_Panel_సమూహం, Pt_ARG_PG_PANEL_TITLES, new_titles,
	    అంకె_panels);

    vim_free(panel_titles);
    panel_titles = new_titles;
}

    స్థిర శూన్య
gui_ph_pg_remove_బఫరు(అక్షరం *name)
{
    పూర్ణాంకం i;
    అక్షరం **new_titles = ఖాళీ;

    // If there is only 1 panel, we just use the temporary place holder
    ఒకవేళ (అంకె_panels > 1)
    {
	new_titles = ALLOC_MULT(అక్షరం *, అంకె_panels - 1);
	ఒకవేళ (new_titles != ఖాళీ)
	{
	    అక్షరం **s = new_titles;
	    // Copy all the titles except the one we're removing
	    కొరకు (i = 0; i < అంకె_panels; i++)
	    {
		ఒకవేళ (STRCMP(panel_titles[ i ], name) != 0)
		    *s++ = panel_titles[ i ];
	    }
	    అంకె_panels--;

	    Pt_సమితి_Resource(gui.vim_Panel_సమూహం, Pt_ARG_PG_PANEL_TITLES, new_titles,
		    అంకె_panels);

	    vim_free(panel_titles);
	    panel_titles = new_titles;
	}
    }
    లేకపోతే
    {
	అంకె_panels--;
	Pt_సమితి_Resource(gui.vim_Panel_సమూహం, Pt_ARG_PG_PANEL_TITLES, &ఖాళీలు_title,
		1);

	VIM_CLEAR(panel_titles);
    }
}

/*
 * When a buffer item is deleted from the buffer menu
 */
    స్థిర పూర్ణాంకం
gui_ph_handle_బఫరు_remove(
	PtWidget_t *widget,
	శూన్య *దత్తాంశం,
	PtCallbackInfo_t *info)
{
    vimmenu_T *menu;

    ఒకవేళ (దత్తాంశం != ఖాళీ)
    {
	menu = (vimmenu_T *) దత్తాంశం;
	gui_ph_pg_remove_బఫరు(menu->dname);
    }

    తిరిగి Pt_CONTINUE;
}
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
gui_ph_pane_resize(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    ఒకవేళ (PtWidgetIsRealized(widget))
    {
	is_ignore_draw = TRUE;
	Pt_ప్రారంభం_Flux(gui.vimContainer);
	PtContainerHold(gui.vimContainer);
    }

    తిరిగి Pt_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////

    శూన్య
gui_ph_encoding_మారింది(పూర్ణాంకం new_encoding)
{
    // Default encoding is latin1
    అక్షరం *charset = "latin1";
    పూర్ణాంకం i;

    నిర్మాణం {
	పూర్ణాంకం encoding;
	అక్షరం *name;
    } charsets[] = {
	{ DBCS_JPN, "SHIFT_JIS" },
	{ DBCS_KOR, "csEUCKR" },
	{ DBCS_CHT, "big5" },
	{ DBCS_CHS, "gb" }
    };

    కొరకు (i = 0; i < ARRAY_నిడివి(charsets); i++)
    {
	ఒకవేళ (new_encoding == charsets[ i ].encoding)
	    charset = charsets[ i ].name;
    }

    charset_translate = Px_Translate_సమితి(charset_translate, charset);
}

////////////////////////////////////////////////////////////////////////////

    శూన్య
gui_mch_prepare(పూర్ణాంకం *argc, అక్షరం **argv)
{
    PtInit(ఖాళీ);
}

    పూర్ణాంకం
gui_mch_init(శూన్య)
{
    PtArg_t వాదనలు[10];
    పూర్ణాంకం	    flags = 0, n = 0;

    PhDim_t	కిటికీ_కొలత = {100, 100}; // Arbitrary values
    Ph_బిందువు_t	pos = {0, 0};

    gui.event_బఫరు = alloc(EVENT_బఫరు_కొలత);
    ఒకవేళ (gui.event_బఫరు == ఖాళీ)
	తిరిగి FAIL;

    // Get a translation so we can convert from ISO Latin-1 to UTF
    charset_translate = Px_Translate_సమితి(ఖాళీ, "latin1");

    // The +2 is for the 1 pixel dark line on each side
    gui.border_స్థానభ్రంశం = gui.border_వెడల్పు = GUI_PH_MARGIN + 2;

    // Handle close events ourselves
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_కిటికీ_MANAGED_FLAGS, Pt_FALSE, Ph_WM_CLOSE);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_కిటికీ_NOTIFY_FLAGS, Pt_TRUE,
	    Ph_WM_CLOSE | Ph_WM_RESIZE | Ph_WM_FOCUS);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_DIM, &కిటికీ_కొలత, 0);
    gui.vim_కిటికీ = PtCreateWidget(Pt_కిటికీ, ఖాళీ, n, వాదనలు);
    ఒకవేళ (gui.vim_కిటికీ == ఖాళీ)
	తిరిగి FAIL;

    Pt_కూడు_Callback(gui.vim_కిటికీ, Pt_CB_కిటికీ, gui_ph_handle_కిటికీ_cb, ఖాళీ);
    Pt_కూడు_Callback(gui.vim_కిటికీ, Pt_CB_కిటికీ_OPENING,
	    gui_ph_handle_కిటికీ_open, ఖాళీ);

    n = 0;
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ALL, Pt_IS_ANCHORED);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_DIM, &కిటికీ_కొలత, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_POS, &pos, 0);

#నిర్వచితమైతే USE_PANEL_సమూహం
    // Put in a temporary place holder title
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_PG_PANEL_TITLES, &ఖాళీలు_title, 1);

    gui.vim_Panel_సమూహం = PtCreateWidget(Pt_Panel_సమూహం, gui.vim_కిటికీ, n, వాదనలు);
    ఒకవేళ (gui.vim_Panel_సమూహం == ఖాళీ)
	తిరిగి FAIL;

    Pt_కూడు_Callback(gui.vim_Panel_సమూహం, Pt_CB_PG_PANEL_SWITCHING,
	    gui_ph_handle_pg_change, ఖాళీ);
#లేకపోతే
    // Turn off all edge decorations
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_BASIC_FLAGS, Pt_FALSE, Pt_ALL);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_BEVEL_వెడల్పు, 0, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_MARGIN_వెడల్పు, 0, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_MARGIN_ఎత్తు, 0, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_CONTAINER_FLAGS, Pt_TRUE, Pt_AUTO_EXTENT);

    gui.vimContainer = PtCreateWidget(PtPane, gui.vim_కిటికీ, n, వాదనలు);
    ఒకవేళ (gui.vimContainer == ఖాళీ)
	తిరిగి FAIL;

    Pt_కూడు_Callback(gui.vimContainer, Pt_CB_RESIZE, gui_ph_pane_resize, ఖాళీ);
#అయితే_ముగింపు

    // Size for the text area is set in gui_mch_set_text_area_pos
    n = 0;

    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_RAW_DRAW_F, gui_ph_handle_raw_draw, 1);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_BEVEL_వెడల్పు, GUI_PH_MARGIN, 0);
    /*
     * Using focus render also causes the whole widget to be redrawn
     * whenever it changes focus, which is very annoying :p
     */
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_FLAGS, Pt_TRUE,
	    Pt_GETS_FOCUS | Pt_HIGHLIGHTED);
#నిర్వచితం_కాకపోతే FEAT_MOUSESHAPE
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_CURSOR_TYPE, GUI_PH_MOUSE_TYPE, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_CURSOR_COLOR, gui_ph_mouse_color, 0);
#అయితే_ముగింపు

    gui.vim_పాఠ్యం_Area = PtCreateWidget(PtRaw, Pt_DFLT_మాతృ, n, వాదనలు);
    ఒకవేళ (gui.vim_పాఠ్యం_Area == ఖాళీ)
	తిరిగి FAIL;

    // TODO: use PtAddEventHandlers instead?
    // Not using Ph_EV_BUT_REPEAT because vim wouldn't use it anyway
    Pt_కూడు_Event_Handler(gui.vim_పాఠ్యం_Area,
	    Ph_EV_BUT_PRESS | Ph_EV_BUT_RELEASE | Ph_EV_PTR_MOTION_BUTTON,
	    gui_ph_handle_mouse, ఖాళీ);
    Pt_కూడు_Event_Handler(gui.vim_పాఠ్యం_Area, Ph_EV_కీలకం,
	    gui_ph_handle_keyboard, ఖాళీ);
    Pt_కూడు_Callback(gui.vim_పాఠ్యం_Area, Pt_CB_GOT_FOCUS,
	    gui_ph_handle_focus, ఖాళీ);
    Pt_కూడు_Callback(gui.vim_పాఠ్యం_Area, Pt_CB_కోల్పోయిన_FOCUS,
	    gui_ph_handle_focus, ఖాళీ);

    /*
     * Now that the text area widget has been created, set up the colours,
     * which will call PtSetResource from gui_mch_new_colors
     */

    /*
     * Create the two timers, not as accurate as using the kernel timer
     * functions, but good enough
     */
    gui_ph_timer_cursor  = PtCreateWidget(PtTimer, gui.vim_కిటికీ, 0, ఖాళీ);
    ఒకవేళ (gui_ph_timer_cursor == ఖాళీ)
	తిరిగి FAIL;

    gui_ph_timer_timeout = PtCreateWidget(PtTimer, gui.vim_కిటికీ, 0, ఖాళీ);
    ఒకవేళ (gui_ph_timer_timeout == ఖాళీ)
	తిరిగి FAIL;

    Pt_కూడు_Callback(gui_ph_timer_cursor,  Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_cursor, ఖాళీ);
    Pt_కూడు_Callback(gui_ph_timer_timeout, Pt_CB_TIMER_ACTIVATE,
	    gui_ph_handle_timer_timeout, ఖాళీ);

#నిర్వచితమైతే FEAT_MENU
    n = 0;
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_వెడల్పు, కిటికీ_కొలత.w, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ANCHOR_FLAGS, Pt_ANCHOR_ఎడమ_కుడి,
	    Pt_IS_ANCHORED);
    gui.vim_Tool_Bar_సమూహం = PtCreateWidget(Pt_Toolbar_సమూహం, gui.vim_కిటికీ,
	    n, వాదనలు);
    ఒకవేళ (gui.vim_Tool_Bar_సమూహం == ఖాళీ)
	తిరిగి FAIL;

    Pt_కూడు_Callback(gui.vim_Tool_Bar_సమూహం, Pt_CB_RESIZE,
	    gui_ph_handle_menu_resize, ఖాళీ);

    n = 0;
    flags = 0;
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_వెడల్పు, కిటికీ_కొలత.w, 0);
    ఒకవేళ (! vim_strchr(p_go, GO_MENUS))
    {
	flags |= Pt_DELAY_REALIZE;
	Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_FLAGS, Pt_TRUE, flags);
    }
    gui.vimMenuBar = PtCreateWidget(PtMenuBar, gui.vim_Tool_Bar_సమూహం, n, వాదనలు);
    ఒకవేళ (gui.vimMenuBar == ఖాళీ)
	తిరిగి FAIL;

# ifdef FEAT_TOOLBAR
    n = 0;

    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ANCHOR_FLAGS,
	    Pt_ANCHOR_ఎడమ_కుడి |Pt_శిఖరం_ANCHORED_శిఖరం, Pt_IS_ANCHORED);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_RESIZE_FLAGS, Pt_TRUE,
	    Pt_RESIZE_Y_AS_REQUIRED);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_వెడల్పు, కిటికీ_కొలత.w, 0);

    flags = Pt_GETS_FOCUS;
    ఒకవేళ (! vim_strchr(p_go, GO_TOOLBAR))
	flags |= Pt_DELAY_REALIZE;

    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE, flags);

    gui.vimToolBar = PtCreateWidget(PtToolbar, gui.vim_Tool_Bar_సమూహం, n, వాదనలు);
    ఒకవేళ (gui.vimToolBar == ఖాళీ)
	తిరిగి FAIL;

    /*
     * Size for the toolbar is fetched in gui_mch_show_toolbar, after
     * the buttons have been added and the toolbar has resized it's height
     * for the buttons to fit
     */
# endif

#అయితే_ముగింపు

    తిరిగి OK;
}

    పూర్ణాంకం
gui_mch_init_check(శూన్య)
{
    తిరిగి (is_photon_అందుబాటు == TRUE) ? OK : FAIL;
}

    పూర్ణాంకం
gui_mch_open(శూన్య)
{
    gui.norm_pixel =  Pg_BLACK;
    gui.వెనుక_pixel =  Pg_WHITE;

    సమితి_సాధారణ_colors();

    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_వెనుక_pixel = gui.వెనుక_pixel;

    highlight_gui_started();

    ఒకవేళ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_సమితి_winpos(gui_win_x, gui_win_y);

    తిరిగి (PtRealizeWidget(gui.vim_కిటికీ) == 0) ? OK : FAIL;
}

    శూన్య
gui_mch_exit(పూర్ణాంకం rc)
{
    PtDestroyWidget(gui.vim_కిటికీ);

    Px_Translate_సమితి(charset_translate, ఖాళీ);

    vim_free(gui.event_బఫరు);

#నిర్వచితమైతే USE_PANEL_GROUPS
    vim_free(panel_titles);
#అయితే_ముగింపు
}

////////////////////////////////////////////////////////////////////////////
// events

/*
 * When no events are available, photon will call this function, working is
 * set to FALSE, and the gui_mch_update loop will exit.
 */
    స్థిర పూర్ణాంకం
exit_gui_mch_update(శూన్య *దత్తాంశం)
{
    *(పూర్ణాంకం *)దత్తాంశం = FALSE;
    తిరిగి Pt_చివర;
}

    శూన్య
gui_mch_update(శూన్య)
{
    పూర్ణాంకం working = TRUE;

    Pt_App_కూడు_Work_Proc(ఖాళీ, exit_gui_mch_update, &working);
    అయితే ((working == TRUE) && !vim_is_ఆదానం_buf_నిండిన())
	PtProcessEvent();
}

    పూర్ణాంకం
gui_mch_wait_for_chars(దీర్ఘ wtime)
{
    is_timeout = FALSE;

    ఒకవేళ (wtime >= 0)
	Pt_సమితి_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL,
						    wtime == 0 ? 1 : wtime, 0);

    అయితే (1)
    {
	PtProcessEvent();
	ఒకవేళ (ఆదానం_అందుబాటు())
	{
	    Pt_సమితి_Resource(gui_ph_timer_timeout, Pt_ARG_TIMER_INITIAL, 0, 0);
	    తిరిగి OK;
	}
	లేకపోతే ఒకవేళ (is_timeout == TRUE)
	    తిరిగి FAIL;
    }
}

#ఒకవేళ defined(FEAT_BROWSE)
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
	పూర్ణాంకం saving,
	char_u *title,
	char_u *default_name,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    PtFileSelectionInfo_t file;
    పూర్ణాంకం	    flags;
    char_u  *default_మార్గం;
    char_u  *open_పాఠ్యం = ఖాళీ;

    flags = 0;
    CLEAR_క్షేత్రం(file);

    default_మార్గం = alloc(MAXPATHL + 1 + NAME_MAX + 1);
    ఒకవేళ (default_మార్గం != ఖాళీ)
    {
	ఒకవేళ (saving == TRUE)
	{
	    // Don't need Pt_FSR_CONFIRM_EXISTING, vim will ask anyway
	    flags |= Pt_FSR_NO_FCHECK;
	    open_పాఠ్యం = "&Save";
	}

	// combine the directory and filename into a single path
	ఒకవేళ (initdir == ఖాళీ || *initdir == NUL)
	{
	    mch_dirname(default_మార్గం, MAXPATHL);
	    initdir = default_మార్గం;
	}
	లేకపోతే
	{
	    STRCPY(default_మార్గం, initdir);
	    initdir = default_మార్గం;
	}

	ఒకవేళ (default_name != ఖాళీ)
	{
	    ఒకవేళ (default_మార్గం[ STRLEN(default_మార్గం) - 1 ] != '/')
		STRCAT(default_మార్గం, "/");

	    STRCAT(default_మార్గం, default_name);
	}

	// TODO: add a filter?
	PtFileSelection(
		gui.vim_కిటికీ,
		ఖాళీ,
		title,
		default_మార్గం,
		ఖాళీ,
		open_పాఠ్యం,
		ఖాళీ,
		ఖాళీ,
		&file,
		flags);

	vim_free(default_మార్గం);

	ఒకవేళ (file.ret == Pt_FSDIALOG_BTN1)
	    తిరిగి vim_strsave(file.మార్గం);
    }
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_DIALOG)
స్థిర PtWidget_t *gui_ph_dialog_పాఠ్యం = ఖాళీ;

    స్థిర పూర్ణాంకం
gui_ph_dialog_close(పూర్ణాంకం button, శూన్య *దత్తాంశం)
{
    PtModalCtrl_t *modal_ctrl = దత్తాంశం;
    char_u *dialog_పాఠ్యం, *vim_పాఠ్యం;

    ఒకవేళ (gui_ph_dialog_పాఠ్యం != ఖాళీ)
    {
	PtGetResource(gui_ph_dialog_పాఠ్యం, Pt_ARG_పాఠ్యం_STRING, &dialog_పాఠ్యం, 0);
	PtGetResource(gui_ph_dialog_పాఠ్యం, Pt_ARG_సూచిక, &vim_పాఠ్యం, 0);
	STRNCPY(vim_పాఠ్యం, dialog_పాఠ్యం, IOSIZE - 1);
    }

    PtModalUnblock(modal_ctrl, (శూన్య *) button);

    తిరిగి Pt_TRUE;
}

    స్థిర పూర్ణాంకం
gui_ph_dialog_పాఠ్యం_enter(
    PtWidget_t		*widget,
    శూన్య		*దత్తాంశం,
    PtCallbackInfo_t	*info)
{
    ఒకవేళ (info->reason_subtype == Pt_EDIT_ACTIVATE)
	gui_ph_dialog_close(1, దత్తాంశం);
    తిరిగి Pt_CONTINUE;
}

    స్థిర పూర్ణాంకం
gui_ph_dialog_esc(PtWidget_t *widget, శూన్య *దత్తాంశం, PtCallbackInfo_t *info)
{
    Ph_కీలకం_Event_t *కీలకం;

    కీలకం = Ph_Get_దత్తాంశం(info->event);
    ఒకవేళ ((కీలకం->కీలకం_flags & Pk_KF_Cap_Valid) && (కీలకం->కీలకం_cap == Pk_Escape))
    {
	gui_ph_dialog_close(0, దత్తాంశం);
	తిరిగి Pt_CONSUME;
    }
    తిరిగి Pt_PROCESS;
}

    పూర్ణాంకం
gui_mch_dialog(
	పూర్ణాంకం	type,
	char_u	*title,
	char_u	*సందేశం_2,
	char_u	*buttons,
	పూర్ణాంకం	default_button,
	char_u	*textfield,
	పూర్ణాంకం	ex_cmd)
{
    char_u	*str;
    char_u	**button_array;
    char_u	*buttons_copy;

    పూర్ణాంకం		button_లెక్క;
    పూర్ణాంకం		i, len;
    పూర్ణాంకం		dialog_ఫలితం = -1;

    // FIXME: the vertical option in guioptions is blatantly ignored
    // FIXME: so is the type

    button_లెక్క = len = i = 0;

    ఒకవేళ (buttons == ఖాళీ || *buttons == NUL)
	తిరిగి -1;

    // There is one less separator than buttons, so bump up the button count
    button_లెక్క = 1;

    // Count string length and number of separators
    కొరకు (str = buttons; *str; str++)
    {
	len++;
	ఒకవేళ (*str == DLG_BUTTON_SEP)
	    button_లెక్క++;
    }

    ఒకవేళ (title == ఖాళీ)
	title = "Vim";

    buttons_copy = alloc(len + 1);
    button_array = ALLOC_MULT(char_u *, button_లెక్క);
    ఒకవేళ (buttons_copy != ఖాళీ && button_array != ఖాళీ)
    {
	STRCPY(buttons_copy, buttons);

	/*
	 * Convert DLG_BUTTON_SEP into NUL's and fill in
	 * button_array with the pointer to each NUL terminated string
	 */
	str = buttons_copy;
	కొరకు (i = 0; i < button_లెక్క; i++)
	{
	    button_array[ i ] = str;
	    కొరకు (; *str; str++)
	    {
		ఒకవేళ (*str == DLG_BUTTON_SEP)
		{
		    *str++ = NUL;
		    ఆపు;
		}
	    }
	}
# ifndef FEAT_GUI_TEXTDIALOG
	dialog_ఫలితం = PtAlert(
		gui.vim_కిటికీ, ఖాళీ,
		title,
		ఖాళీ,
		సందేశం_2, ఖాళీ,
		button_లెక్క, (స్థిరరాశి అక్షరం **) button_array, ఖాళీ,
		default_button, 0, Pt_MODAL);
# లేకపోతే
	// Writing the dialog ourselves lets us add extra features, like
	// trapping the escape key and returning 0 to vim
	{
	    పూర్ణాంకం n;
	    PtArg_t వాదనలు[5];
	    PtWidget_t *dialog, *pane;
	    PtModalCtrl_t modal_ctrl;
	    PtDialogInfo_t di;

	    CLEAR_క్షేత్రం(di);
	    CLEAR_క్షేత్రం(modal_ctrl);

	    n = 0;
	    Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_సమూహం_అడ్డువరుసలు_నిలువుల_సంఖ్య, 0, 0);
	    Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_వెడల్పు, 350, 0);
	    Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_సమూహం_ORIENTATION,
		    Pt_సమూహం_VERTICAL, 0);
	    Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_సమూహం_FLAGS,
		    Pt_TRUE, Pt_సమూహం_NO_కీలకాలు | Pt_సమూహం_STRETCH_HORIZONTAL);
	    Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_CONTAINER_FLAGS, Pt_FALSE, Pt_TRUE);
	    pane = PtCreateWidget(Pt_సమూహం, ఖాళీ, n, వాదనలు);

	    n = 0;
	    Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_పాఠ్యం_STRING, సందేశం_2, 0);
	    PtCreateWidget(PtLabel, pane, n, వాదనలు);

	    ఒకవేళ (textfield != ఖాళీ)
	    {
		n = 0;
		Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_MAX_నిడివి, IOSIZE - 1, 0);
		Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_పాఠ్యం_STRING, textfield, 0);
		Pt_సమితి_Arg(&వాదనలు[n++], Pt_ARG_సూచిక, textfield, 0);
		gui_ph_dialog_పాఠ్యం = PtCreateWidget(Pt_పాఠ్యం, pane, n, వాదనలు);
		Pt_కూడు_Callback(gui_ph_dialog_పాఠ్యం, Pt_CB_ACTIVATE,
			gui_ph_dialog_పాఠ్యం_enter, &modal_ctrl);
	    }

	    di.మాతృ = gui.vim_కిటికీ;
	    di.pane = pane;
	    di.title = title;
	    di.buttons = (స్థిరరాశి అక్షరం **) button_array;
	    di.nbtns = button_లెక్క;
	    di.def_btn = default_button;
	    // This is just to give the dialog the close button.
	    // We check for the Escape key ourselves and return 0
	    di.esc_btn = button_లెక్క;
	    di.callback = gui_ph_dialog_close;
	    di.దత్తాంశం = &modal_ctrl;

	    dialog = PtCreateDialog(&di);
	    Pt_కూడు_Filter_Callback(dialog, Ph_EV_కీలకం,
		    gui_ph_dialog_esc, &modal_ctrl);

	    ఒకవేళ (gui_ph_dialog_పాఠ్యం != ఖాళీ)
		PtGiveFocus(gui_ph_dialog_పాఠ్యం, ఖాళీ);

	    // Open dialog, block the vim window and wait for the dialog to close
	    PtRealizeWidget(dialog);
	    PtMakeModal(dialog, Ph_CURSOR_NOINPUT, Ph_CURSOR_DEFAULT_COLOR);
	    dialog_ఫలితం = (పూర్ణాంకం) Pt_Modal_ఖండం(&modal_ctrl, 0);

	    PtDestroyWidget(dialog);
	    gui_ph_dialog_పాఠ్యం = ఖాళీ;
	}
# endif
    }

    vim_free(button_array);
    vim_free(buttons_copy);

    తిరిగి dialog_ఫలితం;
}
#అయితే_ముగింపు
////////////////////////////////////////////////////////////////////////////
// window size/position/state

    పూర్ణాంకం
gui_mch_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    Ph_బిందువు_t *pos;

    pos = PtWidgetPos(gui.vim_కిటికీ, ఖాళీ);

    *x = pos->x;
    *y = pos->y;

    తిరిగి OK;
}

    శూన్య
gui_mch_సమితి_winpos(పూర్ణాంకం x, పూర్ణాంకం y)
{
    Ph_బిందువు_t pos = { x, y };

    Pt_సమితి_Resource(gui.vim_కిటికీ, Pt_ARG_POS, &pos, 0);
}

    శూన్య
gui_mch_సమితి_shellsize(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు,
	పూర్ణాంకం min_వెడల్పు, పూర్ణాంకం min_ఎత్తు, పూర్ణాంకం పునాది_వెడల్పు, పూర్ణాంకం పునాది_ఎత్తు,
	పూర్ణాంకం direction)
{
    PhDim_t కిటికీ_కొలత = { వెడల్పు, ఎత్తు };
    PhDim_t min_కొలత = { min_వెడల్పు, min_ఎత్తు };

#నిర్వచితమైతే USE_PANEL_సమూహం
    కిటికీ_కొలత.w += pg_margin_ఎడమ + pg_margin_కుడి;
    కిటికీ_కొలత.h += pg_margin_శిఖరం + pg_margin_bottom;
#అయితే_ముగింపు

    Pt_సమితి_Resource(gui.vim_కిటికీ, Pt_ARG_MINIMUM_DIM, &min_కొలత, 0);
    Pt_సమితి_Resource(gui.vim_కిటికీ, Pt_ARG_DIM, &కిటికీ_కొలత, 0);

    ఒకవేళ (! PtWidgetIsRealized(gui.vim_కిటికీ))
	gui_ph_resize_container();
}

/*
 * Return the amount of screen space that hasn't been allocated (such as
 * by the shelf).
 */
    శూన్య
gui_mch_get_screen_dimensions(పూర్ణాంకం *screen_w, పూర్ణాంకం *screen_h)
{
    PhRect_t console;

    Ph_కిటికీ_Query_Visible(Ph_QUERY_WORKSPACE, 0,
	    Ph_ఆదానం_సమూహం(ఖాళీ), &console);

    *screen_w = console.lr.x - console.ul.x + 1;
    *screen_h = console.lr.y - console.ul.y + 1;
}

    శూన్య
gui_mch_iconify(శూన్య)
{
    Ph_కిటికీ_Event_t event;

    CLEAR_క్షేత్రం(event);
    event.event_f = Ph_WM_HIDE;
    event.event_state = Ph_WM_EVSTATE_HIDE;
    event.rid = PtWidgetRid(gui.vim_కిటికీ);
    Pt_Forward_కిటికీ_Event(&event);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    శూన్య
gui_mch_సమితి_foreground(శూన్య)
{
    Ph_కిటికీ_Event_t event;

    CLEAR_క్షేత్రం(event);
    event.event_f = Ph_WM_TOFRONT;
    event.event_state = Ph_WM_EVSTATE_FFRONT;
    event.rid = PtWidgetRid(gui.vim_కిటికీ);
    Pt_Forward_కిటికీ_Event(&event);
}
#అయితే_ముగింపు

    శూన్య
gui_mch_settitle(char_u *title,	char_u *icon)
{
#నిర్వచితమైతే USE_PANEL_సమూహం
    gui_ph_pg_సమితి_బఫరు_అంకె(curwin->w_బఫరు->b_fnum);
#అయితే_ముగింపు
    Pt_సమితి_Resource(gui.vim_కిటికీ, Pt_ARG_కిటికీ_TITLE, title, 0);
    // Not sure what to do with the icon text, set balloon text somehow?
}

////////////////////////////////////////////////////////////////////////////
// Scrollbar

    శూన్య
gui_mch_సమితి_scrollbar_thumb(scrollbar_T *sb, పూర్ణాంకం val, పూర్ణాంకం కొలత, పూర్ణాంకం max)
{
    పూర్ణాంకం	    n = 0;
    PtArg_t వాదనలు[3];

    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_గరిష్ఠ, max, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_SLIDER_కొలత, కొలత, 0);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_GAUGE_విలువ, val, 0);
    Pt_సమితి_Resources(sb->id, n, వాదనలు);
}

    శూన్య
gui_mch_సమితి_scrollbar_pos(scrollbar_T *sb, పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    PhArea_t area = {{ x, y }, { w, h }};

    Pt_సమితి_Resource(sb->id, Pt_ARG_AREA, &area, 0);
}

    పూర్ణాంకం
gui_mch_get_scrollbar_xpadding(శూన్య)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    తిరిగి 0;
}

    పూర్ణాంకం
gui_mch_get_scrollbar_ypadding(శూన్య)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    తిరిగి 0;
}

    శూన్య
gui_mch_create_scrollbar(scrollbar_T *sb, పూర్ణాంకం orient)
{
    పూర్ణాంకం	    n = 0;
//    int	    anchor_flags = 0;
    PtArg_t వాదనలు[4];

    /*
     * Stop the scrollbar from being realized when the parent
     * is realized, so it can be explicitly realized by vim.
     *
     * Also, don't let the scrollbar get focus
     */
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_FLAGS, Pt_DELAY_REALIZE,
	    Pt_DELAY_REALIZE | Pt_GETS_FOCUS);
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_SCROLLBAR_FLAGS, Pt_SCROLLBAR_SHOW_ARROWS, 0);
#ఒకవేళ 0
    // Don't need this anchoring for the scrollbars
    ఒకవేళ (orient == SBAR_HORIZ)
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM |
	    Pt_ఎడమ_ANCHORED_ఎడమ | Pt_కుడి_ANCHORED_కుడి;
    }
    లేకపోతే
    {
	anchor_flags = Pt_BOTTOM_ANCHORED_BOTTOM | Pt_శిఖరం_ANCHORED_శిఖరం;
	ఒకవేళ (sb->wp != ఖాళీ)
	{
	    ఒకవేళ (sb == &sb->wp->w_scrollbars[ SBAR_ఎడమ ])
		anchor_flags |= Pt_ఎడమ_ANCHORED_ఎడమ;
	    లేకపోతే
		anchor_flags |= Pt_కుడి_ANCHORED_కుడి;
	}
    }
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ANCHOR_FLAGS, anchor_flags, Pt_IS_ANCHORED);
#అయితే_ముగింపు
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ORIENTATION,
	    (orient == SBAR_HORIZ) ? Pt_HORIZONTAL : Pt_VERTICAL, 0);
#నిర్వచితమైతే USE_PANEL_సమూహం
    sb->id = PtCreateWidget(PtScrollbar, gui.vim_Panel_సమూహం, n, వాదనలు);
#లేకపోతే
    sb->id = PtCreateWidget(PtScrollbar, gui.vimContainer, n, వాదనలు);
#అయితే_ముగింపు

    Pt_కూడు_Callback(sb->id, Pt_CB_SCROLLBAR_MOVE, gui_ph_handle_scrollbar, sb);
}

    శూన్య
gui_mch_enable_scrollbar(scrollbar_T *sb, పూర్ణాంకం flag)
{
    ఒకవేళ (flag != 0)
	PtRealizeWidget(sb->id);
    లేకపోతే
	PtUnrealizeWidget(sb->id);
}

    శూన్య
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    PtDestroyWidget(sb->id);
    sb->id = ఖాళీ;
}

////////////////////////////////////////////////////////////////////////////
// Mouse functions

#ఒకవేళ defined(FEAT_MOUSESHAPE)
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
స్థిర పూర్ణాంకం చివరి_shape = 0;

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
స్థిర పూర్ణాంకం mshape_ids[] =
{
    Ph_CURSOR_సూచిక,		// arrow
    Ph_CURSOR_NONE,		// blank
    Ph_CURSOR_చొప్పించు,		// beam
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
    Ph_CURSOR_QUESTION_బిందువు,	// question
    Ph_CURSOR_సూచిక,		// right-arrow
    Ph_CURSOR_సూచిక,		// up-arrow
    Ph_CURSOR_సూచిక		// last one
};

    శూన్య
mch_సమితి_mouse_shape(పూర్ణాంకం shape)
{
    పూర్ణాంకం	    id;

    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    ఒకవేళ (shape == MSHAPE_HIDE || gui.సూచిక_hidden)
	Pt_సమితి_Resource(gui.vim_పాఠ్యం_Area, Pt_ARG_CURSOR_TYPE, Ph_CURSOR_NONE,
		0);
    లేకపోతే
    {
	ఒకవేళ (shape >= MSHAPE_NUMBERED)
	    id = Ph_CURSOR_సూచిక;
	లేకపోతే
	    id = mshape_ids[shape];

	Pt_సమితి_Resource(gui.vim_పాఠ్యం_Area, Pt_ARG_CURSOR_TYPE, id,	0);
    }
    ఒకవేళ (shape != MSHAPE_HIDE)
	చివరి_shape = shape;
}
#అయితే_ముగింపు

    శూన్య
gui_mch_mousehide(పూర్ణాంకం hide)
{
    ఒకవేళ (gui.సూచిక_hidden == hide)
	తిరిగి;

    gui.సూచిక_hidden = hide;
#నిర్వచితమైతే FEAT_MOUSESHAPE
    ఒకవేళ (hide)
	Pt_సమితి_Resource(gui.vim_పాఠ్యం_Area, Pt_ARG_CURSOR_TYPE,
		Ph_CURSOR_NONE, 0);
    లేకపోతే
	mch_సమితి_mouse_shape(చివరి_shape);
#లేకపోతే
    Pt_సమితి_Resource(gui.vim_పాఠ్యం_Area, Pt_ARG_CURSOR_TYPE,
	    (hide == MOUSE_SHOW) ? GUI_PH_MOUSE_TYPE : Ph_CURSOR_NONE,
	    0);
#అయితే_ముగింపు
}

    శూన్య
gui_mch_getmouse(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    PhCursorInfo_t info;
    పొట్టి ix, iy;

    // FIXME: does this return the correct position,
    // with respect to the border?
    PhQueryCursor(Ph_ఆదానం_సమూహం(ఖాళీ), &info);
    PtGetAbsPosition(gui.vim_పాఠ్యం_Area , &ix, &iy);

    *x = info.pos.x - ix;
    *y = info.pos.y - iy;
}

    శూన్య
gui_mch_setmouse(పూర్ణాంకం x, పూర్ణాంకం y)
{
    పొట్టి abs_x, abs_y;

    PtGetAbsPosition(gui.vim_పాఠ్యం_Area, &abs_x, &abs_y);
    // Add the border offset?
    PhMoveCursorAbs(Ph_ఆదానం_సమూహం(ఖాళీ), abs_x + x, abs_y + y);
}

////////////////////////////////////////////////////////////////////////////
// Colours

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    తిరిగి (guicolor_T)(PgRGB(Pg_Red_విలువ(pixel),
				     Pg_Green_విలువ(pixel), Pg_Blue_విలువ(pixel)));
}

    శూన్య
gui_mch_new_colors(శూన్య)
{
#ఒకవేళ 0 // Don't bother changing the cursor colour
    పొట్టి color_diff;

    /*
     * If there isn't enough difference between the background colour and
     * the mouse pointer colour then change the mouse pointer colour
     */
    color_diff = gui_get_lightness(gui_ph_mouse_color)
					  - gui_get_lightness(gui.వెనుక_pixel);

    ఒకవేళ (నిరపేక్ష(color_diff) < 64)
    {
	పొట్టి r, g, b;
	// not a great algorithm...
	r = Pg_Red_విలువ(gui_ph_mouse_color) ^ 255;
	g = Pg_Green_విలువ(gui_ph_mouse_color) ^ 255;
	b = Pg_Blue_విలువ(gui_ph_mouse_color) ^ 255;

# ifndef FEAT_MOUSESHAPE
	gui_ph_mouse_color = PgRGB(r, g, b);
	Pt_సమితి_Resource(gui.vim_పాఠ్యం_Area, Pt_ARG_CURSOR_COLOR,
		gui_ph_mouse_color, 0);
# endif
    }
#అయితే_ముగింపు

    Pt_సమితి_Resource(gui.vim_పాఠ్యం_Area, Pt_ARG_FILL_COLOR, gui.వెనుక_pixel, 0);
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
    తిరిగి gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
    తిరిగి gui_get_rgb_color_cmn(r, g, b);
}

    శూన్య
gui_mch_సమితి_fg_color(guicolor_T color)
{
    Pg_సమితి_పాఠ్యం_Color(color);
}

    శూన్య
gui_mch_సమితి_bg_color(guicolor_T color)
{
    Pg_సమితి_Fill_Color(color);
}

    శూన్య
gui_mch_సమితి_sp_color(guicolor_T color)
{
}

    శూన్య
gui_mch_invert_rectangle(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం nr, పూర్ణాంకం nc)
{
    PhRect_t rect;

    rect.ul.x = FILL_X(నిలువు);
    rect.ul.y = FILL_Y(అడ్డువరుస);

    // FIXME: This has an off by one pixel problem
    rect.lr.x = rect.ul.x + nc * gui.char_వెడల్పు;
    rect.lr.y = rect.ul.y + nr * gui.char_ఎత్తు;
    ఒకవేళ (nc > 0)
	rect.lr.x -= 1;
    ఒకవేళ (nr > 0)
	rect.lr.y -= 1;

    DRAW_ప్రారంభం;
    Pg_సమితి_Draw_బహుళకం(Pg_Draw_బహుళకం_DSTINVERT);
    PgDrawRect(&rect, Pg_DRAW_FILL);
    Pg_సమితి_Draw_బహుళకం(Pg_Draw_బహుళకం_SRCCOPY);
    DRAW_చివర;
}

    శూన్య
gui_mch_clear_ఖండం(పూర్ణాంకం row1, పూర్ణాంకం col1, పూర్ణాంకం row2, పూర్ణాంకం col2)
{
    PhRect_t ఖండం = {
	{ FILL_X(col1), FILL_Y(row1) },
	{ FILL_X(col2 + 1) - 1, FILL_Y(row2 + 1) - 1}
    };

    DRAW_ప్రారంభం;
    gui_mch_సమితి_bg_color(gui.వెనుక_pixel);
    PgDrawRect(&ఖండం, Pg_DRAW_FILL);
    DRAW_చివర;
}

    శూన్య
gui_mch_clear_all(శూన్య)
{
    PhRect_t పాఠ్యం_rect = {
	{ gui.border_వెడల్పు, gui.border_వెడల్పు },
	{ Columns * gui.char_వెడల్పు + gui.border_వెడల్పు - 1 ,
	    అడ్డువరుసలు * gui.char_ఎత్తు + gui.border_వెడల్పు - 1 }
    };

    ఒకవేళ (is_ignore_draw == TRUE)
	తిరిగి;

    DRAW_ప్రారంభం;
    gui_mch_సమితి_bg_color(gui.వెనుక_pixel);
    PgDrawRect(&పాఠ్యం_rect, Pg_DRAW_FILL);
    DRAW_చివర;
}

    శూన్య
gui_mch_delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    PhRect_t    rect;
    Ph_బిందువు_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ఎడమ);
    rect.ul.y = FILL_Y(అడ్డువరుస + అంకె_పంక్తులు);

    rect.lr.x = FILL_X(gui.scroll_region_కుడి + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot + 1) - 1;

    Pt_Widget_స్థానభ్రంశం(gui.vim_పాఠ్యం_Area, &gui_ph_raw_స్థానభ్రంశం);
    Ph_Translate_బిందువు(&gui_ph_raw_స్థానభ్రంశం, PtWidgetPos(gui.vim_పాఠ్యం_Area, ఖాళీ));
    PhTranslateRect(&rect, &gui_ph_raw_స్థానభ్రంశం);

    delta.x = 0;
    delta.y = -అంకె_పంక్తులు * gui.char_ఎత్తు;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_కనుగొను_Disjoint(gui.vim_పాఠ్యం_Area)), &rect, &delta);

    gui_clear_ఖండం(
	gui.scroll_region_bot - అంకె_పంక్తులు + 1,
	gui.scroll_region_ఎడమ,
	gui.scroll_region_bot,
	gui.scroll_region_కుడి);
}

    శూన్య
gui_mch_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    PhRect_t    rect;
    Ph_బిందువు_t   delta;

    rect.ul.x = FILL_X(gui.scroll_region_ఎడమ);
    rect.ul.y = FILL_Y(అడ్డువరుస);

    rect.lr.x = FILL_X(gui.scroll_region_కుడి + 1) - 1;
    rect.lr.y = FILL_Y(gui.scroll_region_bot - అంకె_పంక్తులు + 1) - 1;

    Pt_Widget_స్థానభ్రంశం(gui.vim_పాఠ్యం_Area, &gui_ph_raw_స్థానభ్రంశం);
    Ph_Translate_బిందువు(&gui_ph_raw_స్థానభ్రంశం, PtWidgetPos(gui.vim_పాఠ్యం_Area, ఖాళీ));
    PhTranslateRect(&rect, &gui_ph_raw_స్థానభ్రంశం);

    delta.x = 0;
    delta.y = అంకె_పంక్తులు * gui.char_ఎత్తు;

    PgFlush();

    PhBlit(PtWidgetRid(Pt_కనుగొను_Disjoint(gui.vim_పాఠ్యం_Area)) , &rect, &delta);

    gui_clear_ఖండం(అడ్డువరుస, gui.scroll_region_ఎడమ,
	    అడ్డువరుస + అంకె_పంక్తులు - 1, gui.scroll_region_కుడి);
}

    శూన్య
gui_mch_draw_string(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, char_u *s, పూర్ణాంకం len, పూర్ణాంకం flags)
{
    స్థిర అక్షరం *utf8_బఫరు = ఖాళీ;
    స్థిర పూర్ణాంకం	utf8_len = 0;

    Ph_బిందువు_t	pos = { పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస) };
    PhRect_t	rect;

    ఒకవేళ (is_ignore_draw == TRUE)
	తిరిగి;

    DRAW_ప్రారంభం;

    ఒకవేళ (!(flags & DRAW_TRANSP))
    {
	PgDrawIRect(
		FILL_X(నిలువు), FILL_Y(అడ్డువరుస),
		FILL_X(నిలువు + len) - 1, FILL_Y(అడ్డువరుస + 1) - 1,
		Pg_DRAW_FILL);
    }

    ఒకవేళ (flags & DRAW_UNDERL)
	Pg_సమితి_Underline(gui.norm_pixel, Pg_TRANSPARENT, 0);

    ఒకవేళ (charset_translate != ఖాళీ && enc_utf8 == 0)
    {
	పూర్ణాంకం src_తీసుకున్న, dst_made;

	// Use a static buffer to avoid large amounts of de/allocations
	ఒకవేళ (utf8_len < len)
	{
	    అక్షరం *new_బఫరు = realloc(utf8_బఫరు, len * MB_LEN_MAX);

	    ఒకవేళ (new_బఫరు == ఖాళీ)
		తిరిగి;
	    utf8_బఫరు = new_బఫరు;
	    utf8_len = len;
	}

	Px_Translate_గమ్యం_UTF(
		charset_translate,
		s,
		len,
		&src_తీసుకున్న,
		utf8_బఫరు,
		utf8_len,
		&dst_made);
	s = utf8_బఫరు;
	len = dst_made;
    }

    Pg_Draw_పాఠ్యం(s, len, &pos, 0);

    ఒకవేళ (flags & DRAW_BOLD)
    {
	// FIXME: try and only calculate these values once...
	rect.ul.x = FILL_X(నిలువు) + 1;
	rect.ul.y = FILL_Y(అడ్డువరుస);
	rect.lr.x = FILL_X(నిలువు + len) - 1;
	rect.lr.y = FILL_Y(అడ్డువరుస + 1) - 1;
	// PgSetUserClip(NULL) causes the scrollbar to not redraw...
#ఒకవేళ 0
	pos.x++;

	Pg_సమితి_User_Clip(&rect);
	Pg_Draw_పాఠ్యం(s, len, &pos, 0);
	Pg_సమితి_User_Clip(ఖాళీ);
#లేకపోతే
	rect.lr.y -= (p_linespace + 1) / 2;
	// XXX: DrawTextArea doesn't work with phditto
	Pg_Draw_పాఠ్యం_Area(s, len, &rect, Pg_పాఠ్యం_BOTTOM);
#అయితే_ముగింపు
    }

    ఒకవేళ (flags & DRAW_UNDERL)
	Pg_సమితి_Underline(Pg_TRANSPARENT, Pg_TRANSPARENT, 0);

    DRAW_చివర;
}

////////////////////////////////////////////////////////////////////////////
// Cursor

    శూన్య
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    PhRect_t r;

    // FIXME: Double width characters

    r.ul.x = FILL_X(gui.నిలువు);
    r.ul.y = FILL_Y(gui.అడ్డువరుస);
    r.lr.x = r.ul.x + gui.char_వెడల్పు - 1;
    r.lr.y = r.ul.y + gui.char_ఎత్తు - 1;

    DRAW_ప్రారంభం;
    Pg_సమితి_Stroke_Color(color);
    PgDrawRect(&r, Pg_DRAW_STROKE);
    DRAW_చివర;
}

    శూన్య
gui_mch_draw_part_cursor(పూర్ణాంకం w, పూర్ణాంకం h, guicolor_T color)
{
    PhRect_t r;

    r.ul.x = FILL_X(gui.నిలువు);
    r.ul.y = FILL_Y(gui.అడ్డువరుస) + gui.char_ఎత్తు - h;
    r.lr.x = r.ul.x + w - 1;
    r.lr.y = r.ul.y + h - 1;

    DRAW_ప్రారంభం;
    gui_mch_సమితి_bg_color(color);
    PgDrawRect(&r, Pg_DRAW_FILL);
    DRAW_చివర;
}

    పూర్ణాంకం
gui_mch_is_blinking(శూన్య)
{
    తిరిగి blink_state != BLINK_NONE;
}

    పూర్ణాంకం
gui_mch_is_blink_off(శూన్య)
{
    తిరిగి blink_state == BLINK_OFF;
}

    శూన్య
gui_mch_సమితి_blinking(దీర్ఘ wait, దీర్ఘ on, దీర్ఘ off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    శూన్య
gui_mch_ప్రారంభం_blink(శూన్య)
{
    // Only turn on the timer on if none of the times are zero
    ఒకవేళ (blink_waittime && blink_ontime && blink_offtime && gui.లోపల_focus)
    {
	Pt_సమితి_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL,
		blink_waittime, 0);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    శూన్య
gui_mch_stop_blink(పూర్ణాంకం may_call_gui_update_cursor)
{
    Pt_సమితి_Resource(gui_ph_timer_cursor, Pt_ARG_TIMER_INITIAL, 0, 0);

    ఒకవేళ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);

    blink_state = BLINK_NONE;
}

////////////////////////////////////////////////////////////////////////////
// miscellaneous functions

    శూన్య
gui_mch_beep(శూన్య)
{
    PtBeep();
}

    శూన్య
gui_mch_flash(పూర్ణాంకం msec)
{
    Pg_సమితి_Fill_XORColor(Pg_BLACK, Pg_WHITE);
    Pg_సమితి_Draw_బహుళకం(Pg_DRAWMODE_XOR);
    gui_mch_clear_all();
    gui_mch_flush();

    ui_delay((దీర్ఘ) msec, TRUE);

    gui_mch_clear_all();
    Pg_సమితి_Draw_బహుళకం(Pg_DRAWMODE_OPAQUE);
    gui_mch_flush();
}

    శూన్య
gui_mch_flush(శూన్య)
{
    PgFlush();
}

    శూన్య
gui_mch_సమితి_పాఠ్యం_area_pos(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    PhArea_t area = {{x, y}, {w, h}};

    Pt_సమితి_Resource(gui.vim_పాఠ్యం_Area, Pt_ARG_AREA, &area, 0);
}

    పూర్ణాంకం
gui_mch_haskey(char_u *name)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; special_కీలకాలు[i].కీలకం_sym != 0; i++)
	ఒకవేళ (name[0] == special_కీలకాలు[i].vim_code0 &&
		 name[1] == special_కీలకాలు[i].vim_code1)
	    తిరిగి OK;
    తిరిగి FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Menu

#నిర్వచితమైతే FEAT_TOOLBAR
# include "toolbar.phi"

స్థిర PhImage_t *gui_ph_toolbar_images[] = {
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
    &tb_కనుగొను_phi,
    &tb_save_all_phi,
    &tb_save_session_phi,
    &tb_new_session_phi,
    &tb_ఎక్కించు_session_phi,
    &tb_macro_phi,
    &tb_replace_phi,
    &tb_close_phi,
    &tb_maximize_phi,
    &tb_minimize_phi,
    &tb_విభజించు_phi,
    &tb_shell_phi,
    &tb_కనుగొను_prev_phi,
    &tb_కనుగొను_తదుపరి_phi,
    &tb_కనుగొను_help_phi,
    &tb_make_phi,
    &tb_jump_phi,
    &tb_ctags_phi,
    &tb_vsplit_phi,
    &tb_maxwidth_phi,
    &tb_minwidth_phi
};

స్థిర PhImage_t *
gui_ph_toolbar_ఎక్కించు_icon(char_u *iconfile)
{
    స్థిర PhImage_t external_icon;
    PhImage_t *తాత్కాలిక_phi = ఖాళీ;

    తాత్కాలిక_phi = Px_ఎక్కించు_Image(iconfile, ఖాళీ);
    ఒకవేళ (తాత్కాలిక_phi != ఖాళీ)
    {
	// The label widget will free the image/palette/etc. for us when
	// it's destroyed
	తాత్కాలిక_phi->flags |= Ph_RELEASE_IMAGE_ALL;
	జ్ఞాపకం_నకలు(&external_icon, తాత్కాలిక_phi, పరిమాణం(external_icon));
	విడుదల(తాత్కాలిక_phi);

	తాత్కాలిక_phi = &external_icon;
    }
    తిరిగి తాత్కాలిక_phi;
}

/*
 * This returns either a builtin icon image, an external image or NULL
 * if it can't find either.  The caller can't and doesn't need to try and
 * free() the returned image, and it can't store the image pointer.
 * (When setting the Pt_ARG_LABEL_IMAGE resource, the contents of the
 * PhImage_t are copied, and the original PhImage_t aren't needed anymore).
 */
స్థిర PhImage_t *
gui_ph_toolbar_కనుగొను_icon(vimmenu_T *menu)
{
    char_u నిండిన_pathname[ MAXPATHL + 1 ];
    PhImage_t *icon = ఖాళీ;

    ఒకవేళ (menu->icon_builtin == FALSE)
    {
	ఒకవేళ (menu->iconfile != ఖాళీ)
	    // TODO: use gui_find_iconfile()
	    icon = gui_ph_toolbar_ఎక్కించు_icon(menu->iconfile);

	// TODO: Restrict loading to just .png? Search for any format?
	ఒకవేళ ((icon == ఖాళీ) &&
	    ((gui_కనుగొను_bitmap(menu->name, నిండిన_pathname, "gif") == OK) ||
	      (gui_కనుగొను_bitmap(menu->name, నిండిన_pathname, "png") == OK)))
	    icon = gui_ph_toolbar_ఎక్కించు_icon(నిండిన_pathname);

	ఒకవేళ (icon != ఖాళీ)
	    తిరిగి icon;
    }

    ఒకవేళ (menu->iconidx >= 0 &&
	    (menu->iconidx < ARRAY_నిడివి(gui_ph_toolbar_images)))
	తిరిగి gui_ph_toolbar_images[menu->iconidx];

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_MENU)
    శూన్య
gui_mch_enable_menu(పూర్ణాంకం flag)
{
    ఒకవేళ (flag != 0)
	PtRealizeWidget(gui.vimMenuBar);
    లేకపోతే
	PtUnrealizeWidget(gui.vimMenuBar);
}

    శూన్య
gui_mch_సమితి_menu_pos(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    // Nothing
}

/*
 * Change the position of a menu button in the parent
 */
    స్థిర శూన్య
gui_ph_position_menu(PtWidget_t *widget, పూర్ణాంకం ప్రాధాన్యత)
{
    PtWidget_t	*traverse;
    vimmenu_T	*menu;

    traverse = Pt_Widget_బిడ్డ_వెనుక(Pt_Widget_మాతృ(widget));

    // Iterate through the list of widgets in traverse, until
    // we find the position we want to insert our widget into
    // TODO: traverse from front to back, possible speedup?
    అయితే (traverse != ఖాళీ)
    {
	PtGetResource(traverse, Pt_ARG_సూచిక, &menu, 0);

	ఒకవేళ (menu != ఖాళీ &&
		ప్రాధాన్యత < menu->ప్రాధాన్యత &&
		widget != traverse)
	{
	    // Insert the widget before the current traverse widget
	    Pt_Widget_చొప్పించు(widget, traverse, 1);
	    తిరిగి;
	}

	traverse = Pt_Widget_Brother_లోపల_Front(traverse);
    }
}

/*
 * the index is ignored because it's not useful for our purposes
 */
    శూన్య
gui_mch_కూడు_menu(vimmenu_T *menu, పూర్ణాంకం సూచీ)
{
    vimmenu_T	*మాతృ = menu->మాతృ;
    char_u	*accel_కీలకం;
    char_u	mnemonic_str[MB_LEN_MAX];
    పూర్ణాంకం	    n;
    PtArg_t వాదనలు[5];

    menu->submenu_id = menu->id = ఖాళీ;

    ఒకవేళ (menu_is_menubar(menu->name))
    {

	accel_కీలకం = vim_strchr(menu->name, '&');
	ఒకవేళ (accel_కీలకం != ఖాళీ)
	{
	    mnemonic_str[0] = accel_కీలకం[1];
	    mnemonic_str[1] = NUL;
	}

	// Create the menu button
	n = 0;
	Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_పాఠ్యం_STRING, menu->dname, 0);
	Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ACCEL_పాఠ్యం, menu->actext, 0);
	ఒకవేళ (accel_కీలకం != ఖాళీ)
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ACCEL_కీలకం, mnemonic_str, 0);
	Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_సూచిక, menu, 0);

	ఒకవేళ (మాతృ != ఖాళీ)
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_BUTTON_TYPE, Pt_MENU_కుడి, 0);

	menu->id = PtCreateWidget(PtMenuButton,
		(మాతృ == ఖాళీ) ? gui.vimMenuBar : మాతృ->submenu_id,
		n, వాదనలు);

	Pt_కూడు_Callback(menu->id, Pt_CB_ARM, gui_ph_handle_pulldown_menu, menu);

	// Create the actual menu
	n = 0;
	ఒకవేళ (మాతృ != ఖాళీ)
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_MENU_FLAGS, Pt_TRUE, Pt_MENU_బిడ్డ);

	menu->submenu_id = PtCreateWidget(PtMenu, menu->id, n, వాదనలు);

	ఒకవేళ (మాతృ == ఖాళీ)
	{
	    Pt_కూడు_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		    gui_ph_handle_menu_unrealized, menu);

	    ఒకవేళ (menu->mnemonic != 0)
	    {
		Pt_కూడు_Hotkey_Handler(gui.vim_కిటికీ, సురక్షిత_tolower(menu->mnemonic),
			Pk_KM_Alt, 0, menu, gui_ph_handle_pulldown_menu);
	    }
	}

	gui_ph_position_menu(menu->id, menu->ప్రాధాన్యత);

	// Redraw menubar here instead of gui_mch_draw_menubar
	ఒకవేళ (gui.menu_is_చురుకైన)
	    PtRealizeWidget(menu->id);
    }
    లేకపోతే ఒకవేళ (menu_is_popup(menu->name))
    {
	menu->submenu_id = PtCreateWidget(PtMenu, gui.vim_కిటికీ, 0, ఖాళీ);
	Pt_కూడు_Callback(menu->submenu_id, Pt_CB_UNREALIZED,
		gui_ph_handle_menu_unrealized, menu);
    }
}

    శూన్య
gui_mch_కూడు_menu_అంశం(vimmenu_T *menu, పూర్ణాంకం సూచీ)
{
    vimmenu_T	*మాతృ = menu->మాతృ;
    char_u	*accel_కీలకం;
    char_u	mnemonic_str[MB_LEN_MAX];
    పూర్ణాంకం	    n;
    PtArg_t వాదనలు[13];

    n = 0;
    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_సూచిక, menu, 0);

# ifdef FEAT_TOOLBAR
    ఒకవేళ (menu_is_toolbar(మాతృ->name))
    {
	ఒకవేళ (menu_is_separator(menu->name))
	{
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_SEP_FLAGS,
		    Pt_SEP_VERTICAL, Pt_SEP_ORIENTATION);
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_SEP_TYPE, Pt_ETCHED_లోపల, 0);
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ANCHOR_FLAGS,
		    Pt_TRUE, Pt_ANCHOR_శిఖరం_BOTTOM);
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_వెడల్పు, 2, 0);
	    menu->id = PtCreateWidget(PtSeparator, gui.vimToolBar, n, వాదనలు);
	}
	లేకపోతే
	{
	    ఒకవేళ (అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *) p_toolbar, "text") != ఖాళీ)
	    {
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_BALLOON_POSITION,
			Pt_BALLOON_BOTTOM, 0);
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_పాఠ్యం_STRING, menu->dname, 0);
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_పాఠ్యం_FONT, "TextFont08", 0);
	    }
	    ఒకవేళ ((అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *) p_toolbar, "icons") != ఖాళీ) &&
		    (gui_ph_toolbar_images != ఖాళీ))
	    {
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_LABEL_IMAGE,
			gui_ph_toolbar_కనుగొను_icon(menu), 0);
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_LABEL_TYPE, Pt_పాఠ్యం_IMAGE, 0);
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_పాఠ్యం_IMAGE_SPACING, 0, 0);
	    }
	    ఒకవేళ (అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *) p_toolbar, "tooltips") != ఖాళీ)
	    {
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_LABEL_BALLOON,
			gui_ph_show_tooltip, 0);
		Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_LABEL_FLAGS,
			Pt_TRUE, Pt_SHOW_BALLOON);
	    }
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_MARGIN_ఎత్తు, 1, 0);
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_MARGIN_వెడల్పు, 1, 0);
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_FLAGS, Pt_FALSE,
		    Pt_HIGHLIGHTED | Pt_GETS_FOCUS);
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_FILL_COLOR, Pg_TRANSPARENT, 0);
	    menu->id = PtCreateWidget(PtButton, gui.vimToolBar, n, వాదనలు);

	    Pt_కూడు_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);
	}
	// Update toolbar if it's open
	ఒకవేళ (PtWidgetIsRealized(gui.vimToolBar))
	    PtRealizeWidget(menu->id);
    }
    లేకపోతే
# endif
	ఒకవేళ (menu_is_separator(menu->name))
    {
	menu->id = PtCreateWidget(PtSeparator, మాతృ->submenu_id, n, వాదనలు);
    }
    లేకపోతే
    {
	accel_కీలకం = vim_strchr(menu->name, '&');
	ఒకవేళ (accel_కీలకం != ఖాళీ)
	{
	    mnemonic_str[0] = accel_కీలకం[1];
	    mnemonic_str[1] = NUL;
	}

	Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_పాఠ్యం_STRING, menu->dname, 0);
	ఒకవేళ (accel_కీలకం != ఖాళీ)
	    Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ACCEL_కీలకం, mnemonic_str,
		    0);

	Pt_సమితి_Arg(&వాదనలు[ n++ ], Pt_ARG_ACCEL_పాఠ్యం, menu->actext, 0);

	menu->id = PtCreateWidget(PtMenuButton, మాతృ->submenu_id, n, వాదనలు);

	Pt_కూడు_Callback(menu->id, Pt_CB_ACTIVATE, gui_ph_handle_menu, menu);

# ifdef USE_PANEL_సమూహం
	ఒకవేళ (gui_ph_is_బఫరు_అంశం(menu, మాతృ) == TRUE)
	{
	    Pt_కూడు_Callback(menu->id, Pt_CB_DESTROYED,
		    gui_ph_handle_బఫరు_remove, menu);
	    gui_ph_pg_కూడు_బఫరు(menu->dname);
	}
# endif
    }

    gui_ph_position_menu(menu->id, menu->ప్రాధాన్యత);
}

    శూన్య
gui_mch_destroy_menu(vimmenu_T *menu)
{
    ఒకవేళ (menu->submenu_id != ఖాళీ)
	PtDestroyWidget(menu->submenu_id);
    ఒకవేళ (menu->id != ఖాళీ)
	PtDestroyWidget(menu->id);

    menu->submenu_id = ఖాళీ;
    menu->id = ఖాళీ;
}

    శూన్య
gui_mch_menu_grey(vimmenu_T *menu, పూర్ణాంకం grey)
{
    దీర్ఘ    flags, ముసుగు, fields;

    ఒకవేళ (menu->id == ఖాళీ)
	తిరిగి;

    flags = PtWidgetFlags(menu->id);
    ఒకవేళ (PtWidgetIsClass(menu->id, PtMenuButton) &&
	    PtWidgetIsClass(Pt_Widget_మాతృ(menu->id), PtMenu))
    {
	fields = Pt_FALSE;
	ముసుగు = Pt_SELECTABLE | Pt_HIGHLIGHTED;
    }
    లేకపోతే
    {
	fields = Pt_TRUE;
	ముసుగు = Pt_BLOCKED | Pt_GHOST;
    }

    ఒకవేళ (! grey)
	fields = ~fields;

    Pt_సమితి_Resource(menu->id, Pt_ARG_FLAGS, fields,
	    ముసుగు);
}

    శూన్య
gui_mch_menu_hidden(vimmenu_T *menu, పూర్ణాంకం hidden)
{
    // TODO: [un]realize the widget?
}

    శూన్య
gui_mch_draw_menubar(శూన్య)
{
    // The only time a redraw is needed is when a menu button
    // is added to the menubar, and that is detected and the bar
    // redrawn in gui_mch_add_menu_item
}

    శూన్య
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    Pt_సమితి_Resource(menu->submenu_id, Pt_ARG_POS, &abs_mouse, 0);
    PtRealizeWidget(menu->submenu_id);
}

    శూన్య
gui_mch_toggle_tearoffs(పూర్ణాంకం enable)
{
    // No tearoffs yet
}

#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR)
    శూన్య
gui_mch_show_toolbar(పూర్ణాంకం showit)
{
    ఒకవేళ (showit)
	PtRealizeWidget(gui.vimToolBar);
    లేకపోతే
	PtUnrealizeWidget(gui.vimToolBar);
}
#అయితే_ముగింపు

////////////////////////////////////////////////////////////////////////////
// Fonts

    స్థిర GuiFont
gui_ph_get_font(
	char_u	*font_name,
	int_u	font_flags,
	int_u	font_కొలత_2,
	// Check whether the resulting font has the font flags and size that
	// was asked for
	int_u	enforce
	)
{
    char_u	    *font_ట్యాగ్;
    FontQueryInfo   info;
    int_u	    style;

    font_ట్యాగ్ = alloc(MAX_FONT_ట్యాగ్);
    ఒకవేళ (font_ట్యాగ్ != ఖాళీ)
    {
	ఒకవేళ (PfGenerateFontName(font_name, font_flags, font_కొలత_2,
		    font_ట్యాగ్) != ఖాళీ)
	{
	    // Enforce some limits on the font used
	    style = PHFONT_INFO_FIXED;

	    ఒకవేళ (enforce & PF_STYLE_BOLD)
		style |= PHFONT_INFO_BOLD;
	    ఒకవేళ (enforce & PF_STYLE_ANTIALIAS)
		style |= PHFONT_INFO_ALIAS;
	    ఒకవేళ (enforce & PF_STYLE_ITALIC)
		style |= PHFONT_INFO_ITALIC;

	    PfQueryFontInfo(font_ట్యాగ్, &info);

	    ఒకవేళ (info.కొలత == 0)
		font_కొలత_2 = 0;

	    // Make sure font size matches, and that the font style
	    // at least has the bits we're checking for
	    ఒకవేళ (font_కొలత_2 == info.కొలత &&
		    style == (info.style & style))
		తిరిగి (GuiFont)font_ట్యాగ్;
	}
	vim_free(font_ట్యాగ్);
    }
    తిరిగి ఖాళీ;
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

    స్థిర పూర్ణాంకం
gui_ph_parse_font_name(
	char_u *vim_font,
	char_u **font_name,
	int_u *font_flags,
	int_u *font_కొలత_2)
{
    char_u  *mark;
    int_u   name_len, కొలత;

    mark = vim_strchr(vim_font, ':');
    ఒకవేళ (mark == ఖాళీ)
	name_len = STRLEN(vim_font);
    లేకపోతే
	name_len = (int_u) (mark - vim_font);

    *font_name = vim_strnsave(vim_font, name_len);
    ఒకవేళ (*font_name != ఖాళీ)
    {
	ఒకవేళ (mark != ఖాళీ)
	{
	    అయితే (*mark != NUL && *mark++ == ':')
	    {
		ఎంపిక (సురక్షిత_tolower(*mark++))
		{
		    స్థితి 'a': *font_flags |= PF_STYLE_ANTIALIAS; ఆపు;
		    స్థితి 'b': *font_flags |= PF_STYLE_BOLD; ఆపు;
		    స్థితి 'i': *font_flags |= PF_STYLE_ITALIC; ఆపు;

		    స్థితి 's':
			కొలత = getdigits(&mark);
			// Restrict the size to some vague limits
			ఒకవేళ (కొలత < 1 || కొలత > 100)
			    కొలత = 8;

			*font_కొలత_2 = కొలత;
			ఆపు;

		    అప్రమేయ:
			ఆపు;
		}
	    }
	}
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

    పూర్ణాంకం
gui_mch_init_font(char_u *vim_font_name, పూర్ణాంకం fontset)
{
    char_u  *font_ట్యాగ్;
    char_u  *font_name = ఖాళీ;
    int_u   font_flags = 0;
    int_u   font_కొలత_2  = 12;

    FontQueryInfo info;
    PhRect_t extent;

    ఒకవేళ (vim_font_name == ఖాళీ)
    {
	// Default font
	vim_font_name = "PC Terminal";
    }

    ఒకవేళ (STRCMP(vim_font_name, "*") == 0)
    {
	font_ట్యాగ్ = PtFontSelection(gui.vim_కిటికీ, ఖాళీ, ఖాళీ,
		"pcterm12", -1, PHFONT_FIXED, ఖాళీ);

	ఒకవేళ (font_ట్యాగ్ == ఖాళీ)
	    తిరిగి FAIL;

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ట్యాగ్;

	PfQueryFontInfo(font_ట్యాగ్, &info);
	font_name = vim_strsave(info.font);
    }
    లేకపోతే
    {
	ఒకవేళ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		    &font_కొలత_2) == FALSE)
	    తిరిగి FAIL;

	font_ట్యాగ్ = gui_ph_get_font(font_name, font_flags, font_కొలత_2, 0);
	ఒకవేళ (font_ట్యాగ్ == ఖాళీ)
	{
	    vim_free(font_name);
	    తిరిగి FAIL;
	}

	gui_mch_free_font(gui.norm_font);
	gui.norm_font = font_ట్యాగ్;
    }

    gui_mch_free_font(gui.bold_font);
    gui.bold_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_BOLD,
	    font_కొలత_2, PF_STYLE_BOLD);

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = gui_ph_get_font(font_name, font_flags | PF_STYLE_ITALIC,
	    font_కొలత_2, PF_STYLE_ITALIC);

    // This extent was brought to you by the letter 'g'
    Pf_Extent_పాఠ్యం(&extent, ఖాళీ, font_ట్యాగ్, "g", 1);

    gui.char_వెడల్పు = extent.lr.x - extent.ul.x + 1;
    gui.char_ఎత్తు = (- extent.ul.y) + extent.lr.y + 1;
    gui.char_ascent = - extent.ul.y;

    vim_free(font_name);
    తిరిగి OK;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    పూర్ణాంకం
gui_mch_adjust_charheight(శూన్య)
{
    FontQueryInfo info;

    PfQueryFontInfo(gui.norm_font, &info);

    gui.char_ఎత్తు = - info.ascender + info.descender + p_linespace;
    gui.char_ascent = - info.ascender + p_linespace / 2;

    తిరిగి OK;
}

    GuiFont
gui_mch_get_font(char_u *vim_font_name, పూర్ణాంకం report_error)
{
    char_u  *font_name;
    char_u  *font_ట్యాగ్;
    int_u   font_కొలత_2 = 12;
    int_u   font_flags = 0;

    ఒకవేళ (gui_ph_parse_font_name(vim_font_name, &font_name, &font_flags,
		&font_కొలత_2) != FALSE)
    {
	font_ట్యాగ్ = gui_ph_get_font(font_name, font_flags, font_కొలత_2, -1);
	vim_free(font_name);

	ఒకవేళ (font_ట్యాగ్ != ఖాళీ)
	    తిరిగి (GuiFont)font_ట్యాగ్;
    }

    ఒకవేళ (report_error)
	semsg(_(e_unknown_font_str), vim_font_name);

    తిరిగి FAIL;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి vim_strsave(name);
}
#అయితే_ముగింపు

    శూన్య
gui_mch_సమితి_font(GuiFont font)
{
    Pg_సమితి_Font(font);
}

    శూన్య
gui_mch_free_font(GuiFont font)
{
    vim_free(font);
}

