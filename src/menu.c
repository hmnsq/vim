/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Code for menus.  Used for the GUI and 'wildmenu'.
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_MENU)

#నిర్వచించు MENUDEPTH   10		// maximum depth of menus

#నిర్వచితమైతే FEAT_GUI_MSWIN
స్థిర పూర్ణాంకం కూడు_menu_మార్గం(char_u *, vimmenu_T *, పూర్ణాంకం *, char_u *, పూర్ణాంకం);
#లేకపోతే
స్థిర పూర్ణాంకం కూడు_menu_మార్గం(char_u *, vimmenu_T *, పూర్ణాంకం *, char_u *);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం menu_nable_recurse(vimmenu_T *menu, char_u *name, పూర్ణాంకం modes, పూర్ణాంకం enable);
స్థిర పూర్ణాంకం remove_menu(vimmenu_T **, char_u *, పూర్ణాంకం, పూర్ణాంకం silent);
స్థిర శూన్య free_menu(vimmenu_T **menup);
స్థిర శూన్య free_menu_string(vimmenu_T *, పూర్ణాంకం);
స్థిర పూర్ణాంకం show_menus(char_u *, పూర్ణాంకం);
స్థిర శూన్య show_menus_recursive(vimmenu_T *, పూర్ణాంకం, పూర్ణాంకం);
స్థిర char_u *menu_name_skip(char_u *name);
స్థిర పూర్ణాంకం menu_name_equal(char_u *name, vimmenu_T *menu);
స్థిర పూర్ణాంకం menu_namecmp(char_u *name, char_u *mname);
స్థిర పూర్ణాంకం get_menu_cmd_modes(char_u *, పూర్ణాంకం, పూర్ణాంకం *, పూర్ణాంకం *);
స్థిర char_u *popup_బహుళకం_name(char_u *name, పూర్ణాంకం idx);
స్థిర char_u *menu_పాఠ్యం(char_u *పాఠ్యం, పూర్ణాంకం *mnemonic, char_u **actext);

#ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
స్థిర శూన్య gui_create_tearoffs_recurse(vimmenu_T *menu, స్థిరరాశి char_u *pname, పూర్ణాంకం *pri_tab, పూర్ణాంకం pri_idx);
స్థిర శూన్య gui_కూడు_tearoff(char_u *tearpath, పూర్ణాంకం *pri_tab, పూర్ణాంకం pri_idx);
స్థిర శూన్య gui_destroy_tearoffs_recurse(vimmenu_T *menu);
స్థిర పూర్ణాంకం s_tearoffs = FALSE;
#అయితే_ముగింపు

స్థిర పూర్ణాంకం menu_is_hidden(char_u *name);
స్థిర పూర్ణాంకం menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
స్థిర పూర్ణాంకం menus_locked = 0;

#ఒకవేళ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
స్థిర char_u *menu_skip_part(char_u *p);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MULTI_LANG
స్థిర char_u *menutrans_lookup(char_u *name, పూర్ణాంకం len);
స్థిర శూన్య menu_unescape_name(char_u	*p);
#అయితే_ముగింపు

స్థిర char_u *menu_translate_tab_and_జరుపు(char_u *arg_ప్రారంభం);

// The character for each menu mode
స్థిర అక్షరం *menu_బహుళకం_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#నిర్వచితమైతే FEAT_TOOLBAR
స్థిర స్థిరరాశి అక్షరం *toolbar_పేర్లు[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_లెక్క ARRAY_నిడివి(toolbar_పేర్లు)
#అయితే_ముగింపు

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    స్థిర పూర్ణాంకం
menu_is_winbar(char_u *name)
{
    తిరిగి (STRNCMP(name, "WinBar", 6) == 0);
}

    పూర్ణాంకం
winbar_ఎత్తు(win_T *wp)
{
    ఒకవేళ (wp->w_winbar != ఖాళీ && wp->w_winbar->children != ఖాళీ)
	తిరిగి 1;
    తిరిగి 0;
}

    స్థిర vimmenu_T **
get_మూలం_menu(char_u *name)
{
    ఒకవేళ (menu_is_winbar(name))
	తిరిగి &curwin->w_winbar;
    తిరిగి &మూలం_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    స్థిర పూర్ణాంకం
is_menus_locked(శూన్య)
{
    ఒకవేళ (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    శూన్య
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_మార్గం;
    పూర్ణాంకం		modes;
    char_u	*map_గమ్యం;
    పూర్ణాంకం		noremap;
    పూర్ణాంకం		silent = FALSE;
    పూర్ణాంకం		special = FALSE;
    పూర్ణాంకం		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    పూర్ణాంకం		i;
#ఒకవేళ defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    పూర్ణాంకం		old_menu_ఎత్తు;
# ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    పూర్ణాంకం		old_toolbar_ఎత్తు;
# endif
#అయితే_ముగింపు
    పూర్ణాంకం		pri_tab[MENUDEPTH + 1];
    పూర్ణాంకం		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#నిర్వచితమైతే FEAT_TOOLBAR
    char_u	*icon = ఖాళీ;
#అయితే_ముగింపు
    vimmenu_T	menuarg;
    vimmenu_T	**మూలం_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    కొరకు (;;)
    {
	ఒకవేళ (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    కొనసాగు;
	}
	ఒకవేళ (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    కొనసాగు;
	}
	ఒకవేళ (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    కొనసాగు;
	}
	ఆపు;
    }


    // Locate an optional "icon=filename" argument.
    ఒకవేళ (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#నిర్వచితమైతే FEAT_TOOLBAR
	icon = arg;
#అయితే_ముగింపు
	అయితే (*arg != NUL && *arg != ' ')
	{
	    ఒకవేళ (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	ఒకవేళ (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    కొరకు (p = arg; *p; ++p)
	ఒకవేళ (!VIM_ISDIGIT(*p) && *p != '.')
	    ఆపు;
    ఒకవేళ (VIM_ISWHITE(*p))
    {
	కొరకు (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    ఒకవేళ (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    ఒకవేళ (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    లేకపోతే ఒకవేళ (eap->addr_లెక్క && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    లేకపోతే
	i = 0;
    అయితే (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    ఒకవేళ (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    లేకపోతే ఒకవేళ (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    ఒకవేళ (*arg == NUL)
    {
	show_menus(arg, modes);
	తిరిగి;
    }

#నిర్వచితమైతే FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    ఒకవేళ (menu_is_toolbar(arg))
    {
	menu_మార్గం = menu_skip_part(arg);
	ఒకవేళ (*menu_మార్గం == '.')
	{
	    p = menu_skip_part(++menu_మార్గం);
	    ఒకవేళ (STRNCMP(menu_మార్గం, "BuiltIn", 7) == 0)
	    {
		ఒకవేళ (skipdigits(menu_మార్గం + 7) == p)
		{
		    menuarg.iconidx = atoi((అక్షరం *)menu_మార్గం + 7);
		    ఒకవేళ (menuarg.iconidx >= (పూర్ణాంకం)TOOLBAR_NAME_లెక్క)
			menuarg.iconidx = -1;
		    లేకపోతే
			menuarg.icon_builtin = TRUE;
		}
	    }
	    లేకపోతే
	    {
		కొరకు (i = 0; i < (పూర్ణాంకం)TOOLBAR_NAME_లెక్క; ++i)
		    ఒకవేళ (STRNCMP(toolbar_పేర్లు[i], menu_మార్గం, p - menu_మార్గం)
									 == 0)
		    {
			menuarg.iconidx = i;
			ఆపు;
		    }
	    }
	}
    }
#అయితే_ముగింపు

    menu_మార్గం = arg;
    ఒకవేళ (*menu_మార్గం == '.')
    {
	semsg(_(e_invalid_వాదం_str), menu_మార్గం);
	వెళ్ళు theend;
    }

    map_గమ్యం = menu_translate_tab_and_జరుపు(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    ఒకవేళ (*map_గమ్యం == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_మార్గం, modes);
	వెళ్ళు theend;
    }
    లేకపోతే ఒకవేళ (*map_గమ్యం != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_గమ్యం);
	వెళ్ళు theend;
    }
#ఒకవేళ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_ఎత్తు = gui.menu_ఎత్తు;
# ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_ఎత్తు = gui.toolbar_ఎత్తు;
# endif
#అయితే_ముగింపు

    మూలం_menu_ptr = get_మూలం_menu(menu_మార్గం);
    ఒకవేళ (మూలం_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    ఒకవేళ (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	ఒకవేళ (STRCMP(menu_మార్గం, "*") == 0)	// meaning: do all menus
	    menu_మార్గం = (char_u *)"";

	ఒకవేళ (menu_is_popup(menu_మార్గం))
	{
	    కొరకు (i = 0; i < MENU_సూచీ_TIP; ++i)
		ఒకవేళ (modes & (1 << i))
		{
		    p = popup_బహుళకం_name(menu_మార్గం, i);
		    ఒకవేళ (p != ఖాళీ)
		    {
			menu_nable_recurse(*మూలం_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*మూలం_menu_ptr, menu_మార్గం, modes, enable);
    }
    లేకపోతే ఒకవేళ (unmenu)
    {
	ఒకవేళ (is_menus_locked())
	    వెళ్ళు theend;

	/*
	 * Delete menu(s).
	 */
	ఒకవేళ (STRCMP(menu_మార్గం, "*") == 0)	// meaning: remove all menus
	    menu_మార్గం = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	ఒకవేళ (menu_is_popup(menu_మార్గం))
	{
	    కొరకు (i = 0; i < MENU_సూచీ_TIP; ++i)
		ఒకవేళ (modes & (1 << i))
		{
		    p = popup_బహుళకం_name(menu_మార్గం, i);
		    ఒకవేళ (p != ఖాళీ)
		    {
			remove_menu(మూలం_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(మూలం_menu_ptr, menu_మార్గం, modes, FALSE);
    }
    లేకపోతే
    {
	ఒకవేళ (is_menus_locked())
	    వెళ్ళు theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	ఒకవేళ (STRICMP(map_గమ్యం, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_గమ్యం = (char_u *)"";
	    map_buf = ఖాళీ;
	}
	లేకపోతే ఒకవేళ (modes & MENU_TIP_బహుళకం)
	    map_buf = ఖాళీ;	// Menu tips are plain text.
	లేకపోతే
	    map_గమ్యం = replace_termcodes(map_గమ్యం, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ఖాళీ);
	menuarg.modes = modes;
#నిర్వచితమైతే FEAT_TOOLBAR
	menuarg.iconfile = icon;
#అయితే_ముగింపు
	menuarg.noremap[0] = noremap;
	menuarg.silent[0] = silent;
	కూడు_menu_మార్గం(menu_మార్గం, &menuarg, pri_tab, map_గమ్యం
#నిర్వచితమైతే FEAT_GUI_MSWIN
		, TRUE
#అయితే_ముగింపు
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	ఒకవేళ (menu_is_popup(menu_మార్గం))
	{
	    కొరకు (i = 0; i < MENU_సూచీ_TIP; ++i)
		ఒకవేళ (modes & (1 << i))
		{
		    p = popup_బహుళకం_name(menu_మార్గం, i);
		    ఒకవేళ (p != ఖాళీ)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#నిర్వచితమైతే FEAT_TOOLBAR
			menuarg.iconfile = ఖాళీ;
			menuarg.iconidx = -1;
			menuarg.icon_builtin = FALSE;
#అయితే_ముగింపు
			కూడు_menu_మార్గం(p, &menuarg, pri_tab, map_గమ్యం
#నిర్వచితమైతే FEAT_GUI_MSWIN
				, TRUE
#అయితే_ముగింపు
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

#ఒకవేళ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    ఒకవేళ (gui.లోపల_use
	    && (gui.menu_ఎత్తు != old_menu_ఎత్తు
# ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_ఎత్తు != old_toolbar_ఎత్తు
# endif
	    ))
	gui_సమితి_shellsize(FALSE, FALSE, RESIZE_VERT);
#అయితే_ముగింపు
    ఒకవేళ (మూలం_menu_ptr == &curwin->w_winbar)
    {
	పూర్ణాంకం h = winbar_ఎత్తు(curwin);

	ఒకవేళ (h != curwin->w_winbar_ఎత్తు)
	{
	    ఒకవేళ (h == 0)
		++curwin->w_ఎత్తు;
	    లేకపోతే ఒకవేళ (curwin->w_ఎత్తు > 0)
		--curwin->w_ఎత్తు;
	    curwin->w_winbar_ఎత్తు = h;
	}
	curwin->w_prev_ఎత్తు = curwin->w_ఎత్తు;
    }

theend:
    ;
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    స్థిర పూర్ణాంకం
కూడు_menu_మార్గం(
    char_u	*menu_మార్గం,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    పూర్ణాంకం		*pri_tab,
    char_u	*call_దత్తాంశం
#నిర్వచితమైతే FEAT_GUI_MSWIN
    , పూర్ణాంకం	addtearoff	// may add tearoff item
#అయితే_ముగింపు
    )
{
    char_u	*మార్గం_name;
    పూర్ణాంకం		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = ఖాళీ;
    vimmenu_T	*మాతృ;
    vimmenu_T	**క్రింది_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*తదుపరి_name;
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;
    పూర్ణాంకం		d;
#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం		idx;
    పూర్ణాంకం		new_idx;
#అయితే_ముగింపు
    పూర్ణాంకం		pri_idx = 0;
    పూర్ణాంకం		old_modes = 0;
    పూర్ణాంకం		amenu;
#నిర్వచితమైతే FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_గమ్యం = ఖాళీ;
#అయితే_ముగింపు
    vimmenu_T	**మూలం_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    మార్గం_name = vim_strsave(menu_మార్గం);
    ఒకవేళ (మార్గం_name == ఖాళీ)
	తిరిగి FAIL;
    మూలం_menu_ptr = get_మూలం_menu(menu_మార్గం);
    menup = మూలం_menu_ptr;
    మాతృ = ఖాళీ;
    name = మార్గం_name;
    అయితే (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	తదుపరి_name = menu_name_skip(name);
#నిర్వచితమైతే	FEAT_MULTI_LANG
	map_గమ్యం = menutrans_lookup(name, (పూర్ణాంకం)STRLEN(name));
	ఒకవేళ (map_గమ్యం != ఖాళీ)
	{
	    en_name = name;
	    name = map_గమ్యం;
	}
	లేకపోతే
	    en_name = ఖాళీ;
#అయితే_ముగింపు
	dname = menu_పాఠ్యం(name, ఖాళీ, ఖాళీ);
	ఒకవేళ (dname == ఖాళీ)
	    వెళ్ళు erret;
	ఒకవేళ (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_ఖాళీలు_menu_name));
	    వెళ్ళు erret;
	}

	// See if it's already there
	క్రింది_pri = menup;
#నిర్వచితమైతే FEAT_GUI
	idx = 0;
	new_idx = 0;
#అయితే_ముగింపు
	menu = *menup;
	అయితే (menu != ఖాళీ)
	{
	    ఒకవేళ (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		ఒకవేళ (*తదుపరి_name == NUL && menu->children != ఖాళీ)
		{
		    ఒకవేళ (!sys_menu)
			emsg(_(e_menu_మార్గం_must_not_lead_గమ్యం_sub_menu));
		    వెళ్ళు erret;
		}
		ఒకవేళ (*తదుపరి_name != NUL && menu->children == ఖాళీ
#నిర్వచితమైతే FEAT_GUI_MSWIN
			&& addtearoff
#అయితే_ముగింపు
			)
		{
		    ఒకవేళ (!sys_menu)
			emsg(_(e_part_యొక్క_menu_అంశం_మార్గం_is_not_sub_menu));
		    వెళ్ళు erret;
		}
		ఆపు;
	    }
	    menup = &menu->తదుపరి;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    ఒకవేళ (మాతృ != ఖాళీ || menu_is_menubar(menu->name))
	    {
#నిర్వచితమైతే FEAT_GUI
		++idx;
#అయితే_ముగింపు
		ఒకవేళ (menu->ప్రాధాన్యత <= pri_tab[pri_idx])
		{
		    క్రింది_pri = menup;
#నిర్వచితమైతే FEAT_GUI
		    new_idx = idx;
#అయితే_ముగింపు
		}
	    }
	    menu = menu->తదుపరి;
	}

	ఒకవేళ (menu == ఖాళీ)
	{
	    ఒకవేళ (*తదుపరి_name == NUL && మాతృ == ఖాళీ)
	    {
		emsg(_(e_must_not_కూడు_menu_వస్తువులు_directly_గమ్యం_menu_bar));
		వెళ్ళు erret;
	    }

	    ఒకవేళ (menu_is_separator(dname) && *తదుపరి_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_యొక్క_menu_మార్గం));
		వెళ్ళు erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    ఒకవేళ (menu == ఖాళీ)
		వెళ్ళు erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_పాఠ్యం(name, &menu->mnemonic, &menu->actext);
#నిర్వచితమైతే	FEAT_MULTI_LANG
	    ఒకవేళ (en_name != ఖాళీ)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_పాఠ్యం(en_name, ఖాళీ, ఖాళీ);
	    }
	    లేకపోతే
	    {
		menu->en_name = ఖాళీ;
		menu->en_dname = ఖాళీ;
	    }
#అయితే_ముగింపు
	    menu->ప్రాధాన్యత = pri_tab[pri_idx];
	    menu->మాతృ = మాతృ;
#నిర్వచితమైతే FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BEVAL_TIP
	    menu->tip = ఖాళీ;
#అయితే_ముగింపు
	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->తదుపరి = *క్రింది_pri;
	    *క్రింది_pri = menu;

	    old_modes = 0;

#నిర్వచితమైతే FEAT_TOOLBAR
	    menu->iconidx = menuarg->iconidx;
	    menu->icon_builtin = menuarg->icon_builtin;
	    ఒకవేళ (*తదుపరి_name == NUL && menuarg->iconfile != ఖాళీ)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    ఒకవేళ (మాతృ != ఖాళీ && menu_is_tearoff(మాతృ->children->dname))
		మాతృ->children->modes |= modes;
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#నిర్వచితమైతే FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    ఒకవేళ (addtearoff)
#అయితే_ముగింపు
	    {
		menu->modes |= modes;
		menu->enabled |= modes;
	    }
	}

#నిర్వచితమైతే FEAT_GUI
	/*
	 * Add the menu item when it's used in one of the modes, but not when
	 * only a tooltip is defined.
	 */
	ఒకవేళ ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    ఒకవేళ (gui.లోపల_use)  // Otherwise it will be added when GUI starts
	    {
		ఒకవేళ (*తదుపరి_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_కూడు_menu_అంశం(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		లేకపోతే
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_కూడు_menu(menu, new_idx);
		}
	    }

# ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    ఒకవేళ (	addtearoff
		    && *తదుపరి_name
		    && vim_strchr(p_go, GO_TEAROFF) != ఖాళీ
		    && menu_is_menubar(name)
#  ifdef VIMDLL
		    && (gui.లోపల_use || gui.starting)
#  endif
		    )
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_మార్గం) + TEAR_LEN + 2);
		ఒకవేళ (tearpath != ఖాళీ)
		{
		    char_u  *s;
		    పూర్ణాంకం	    len;

		    STRCPY(tearpath, menu_మార్గం);
		    len = (పూర్ణాంకం)(తదుపరి_name - మార్గం_name - 1);
		    కొరకు (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			ఒకవేళ ((*s == '\\' || *s == Ctrl_V) && s[1])
			{
			    ++len;
			    ++s;
			}
		    }
		    tearpath[len] = NUL;
		    gui_కూడు_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
# endif
	}
#అయితే_ముగింపు // FEAT_GUI

	menup = &menu->children;
	మాతృ = menu;
	name = తదుపరి_name;
	VIM_CLEAR(dname);
	ఒకవేళ (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(మార్గం_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_సాధారణ_బహుళకం | MENU_చొప్పించు_బహుళకం)) ==
				       (MENU_సాధారణ_బహుళకం | MENU_చొప్పించు_బహుళకం));
    ఒకవేళ (sys_menu)
	modes &= ~old_modes;

    ఒకవేళ (menu != ఖాళీ && modes)
    {
#నిర్వచితమైతే FEAT_GUI
	menu->cb = gui_menu_cb;
#అయితే_ముగింపు
	p = (call_దత్తాంశం == ఖాళీ) ? ఖాళీ : vim_strsave(call_దత్తాంశం);

	// loop over all modes, may add more than one
	కొరకు (i = 0; i < MENU_MODES; ++i)
	{
	    ఒకవేళ (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		ఒకవేళ (amenu && call_దత్తాంశం != ఖాళీ && *call_దత్తాంశం != NUL
#నిర్వచితమైతే FEAT_GUI_MSWIN
		       && addtearoff
#అయితే_ముగింపు
				       )
		{
		    ఎంపిక (1 << i)
		    {
			స్థితి MENU_VISUAL_బహుళకం:
			స్థితి MENU_SELECT_బహుళకం:
			స్థితి MENU_OP_PENDING_బహుళకం:
			స్థితి MENU_CMDLINE_బహుళకం:
			    c = Ctrl_C;
			    ఆపు;
			స్థితి MENU_చొప్పించు_బహుళకం:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    ఆపు;
		    }
		}

		ఒకవేళ (c != 0)
		{
		    menu->అక్షరశ్రేణులు[i] = alloc(STRLEN(call_దత్తాంశం) + 5);
		    ఒకవేళ (menu->అక్షరశ్రేణులు[i] != ఖాళీ)
		    {
			menu->అక్షరశ్రేణులు[i][0] = c;
			ఒకవేళ (d == 0)
			    STRCPY(menu->అక్షరశ్రేణులు[i] + 1, call_దత్తాంశం);
			లేకపోతే
			{
			    menu->అక్షరశ్రేణులు[i][1] = d;
			    STRCPY(menu->అక్షరశ్రేణులు[i] + 2, call_దత్తాంశం);
			}
			ఒకవేళ (c == Ctrl_C)
			{
			    పూర్ణాంకం	    len = (పూర్ణాంకం)STRLEN(menu->అక్షరశ్రేణులు[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->అక్షరశ్రేణులు[i][len] = Ctrl_BSL;
			    menu->అక్షరశ్రేణులు[i][len + 1] = Ctrl_G;
			    menu->అక్షరశ్రేణులు[i][len + 2] = NUL;
			}
		    }
		}
		లేకపోతే
		    menu->అక్షరశ్రేణులు[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	ఒకవేళ (modes & MENU_TIP_బహుళకం)
	    gui_mch_menu_సమితి_tip(menu);
#అయితే_ముగింపు
    }
    తిరిగి OK;

erret:
    vim_free(మార్గం_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    అయితే (మాతృ != ఖాళీ && మాతృ->children == ఖాళీ)
    {
	ఒకవేళ (మాతృ->మాతృ == ఖాళీ)
	    menup = మూలం_menu_ptr;
	లేకపోతే
	    menup = &మాతృ->మాతృ->children;
	కొరకు ( ; *menup != ఖాళీ && *menup != మాతృ; menup = &((*menup)->తదుపరి))
	    ;
	ఒకవేళ (*menup == ఖాళీ) // safety check
	    ఆపు;
	మాతృ = మాతృ->మాతృ;
	free_menu(menup);
    }
    తిరిగి FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    స్థిర పూర్ణాంకం
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    పూర్ణాంకం		modes,
    పూర్ణాంకం		enable)
{
    char_u	*p;

    ఒకవేళ (menu == ఖాళీ)
	తిరిగి OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    అయితే (menu != ఖాళీ)
    {
	ఒకవేళ (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    ఒకవేళ (*p != NUL)
	    {
		ఒకవేళ (menu->children == ఖాళీ)
		{
		    emsg(_(e_part_యొక్క_menu_అంశం_మార్గం_is_not_sub_menu));
		    తిరిగి FAIL;
		}
		ఒకవేళ (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
		ఒకవేళ (enable)
		    menu->enabled |= modes;
		లేకపోతే
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    ఒకవేళ (*name != NUL && *name != '*')
		ఆపు;
	}
	menu = menu->తదుపరి;
    }
    ఒకవేళ (*name != NUL && *name != '*' && menu == ఖాళీ)
    {
	semsg(_(e_no_menu_str), name);
	తిరిగి FAIL;
    }

#నిర్వచితమైతే FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    స్థిర పూర్ణాంకం
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    పూర్ణాంకం		modes,
    పూర్ణాంకం		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*బిడ్డ;
    char_u	*p;

    ఒకవేళ (*menup == ఖాళీ)
	తిరిగి OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    అయితే ((menu = *menup) != ఖాళీ)
    {
	ఒకవేళ (*name == NUL || menu_name_equal(name, menu))
	{
	    ఒకవేళ (*p != NUL && menu->children == ఖాళీ)
	    {
		ఒకవేళ (!silent)
		    emsg(_(e_part_యొక్క_menu_అంశం_మార్గం_is_not_sub_menu));
		తిరిగి FAIL;
	    }
	    ఒకవేళ ((menu->modes & modes) != 0x0)
	    {
#ఒకవేళ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		ఒకవేళ (*p == NUL && modes == MENU_ALL_MODES)
		{
		    ఒకవేళ (Is_కిటికీ(menu->tearoff_handle))
			Destroy_కిటికీ(menu->tearoff_handle);
		}
#అయితే_ముగింపు
		ఒకవేళ (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (*name != NUL)
	    {
		ఒకవేళ (!silent)
		    emsg(_(e_menu_only_exists_లోపల_another_బహుళకం));
		తిరిగి FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    ఒకవేళ (*name != NUL)
		ఆపు;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    ఒకవేళ (modes & MENU_TIP_బహుళకం)
		free_menu_string(menu, MENU_సూచీ_TIP);
	    ఒకవేళ ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    లేకపోతే
		menup = &menu->తదుపరి;
	}
	లేకపోతే
	    menup = &menu->తదుపరి;
    }
    ఒకవేళ (*name != NUL)
    {
	ఒకవేళ (menu == ఖాళీ)
	{
	    ఒకవేళ (!silent)
		semsg(_(e_no_menu_str), name);
	    తిరిగి FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#ఒకవేళ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	ఒకవేళ ((s_tearoffs) && (menu->children != ఖాళీ)) // there's a tear bar.
	    బిడ్డ = menu->children->తదుపరి; // don't count tearoff bar
	లేకపోతే
#అయితే_ముగింపు
	    బిడ్డ = menu->children;
	కొరకు ( ; బిడ్డ != ఖాళీ; బిడ్డ = బిడ్డ->తదుపరి)
	    menu->modes |= బిడ్డ->modes;
	ఒకవేళ (modes & MENU_TIP_బహుళకం)
	{
	    free_menu_string(menu, MENU_సూచీ_TIP);
#ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    ఒకవేళ (gui.లోపల_use)
		gui_mch_menu_సమితి_tip(menu);
#అయితే_ముగింపు
	}
	ఒకవేళ ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#ఒకవేళ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    ఒకవేళ (s_tearoffs && menu->children != ఖాళీ) // there's a tear bar.
		free_menu(&menu->children);
#అయితే_ముగింపు
	    *menup = menu;
	    free_menu(menup);
	}
    }

    తిరిగి OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    శూన్య
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_వస్తువులు);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    స్థిర శూన్య
free_menu(vimmenu_T **menup)
{
    పూర్ణాంకం		i;
    vimmenu_T	*menu;

    menu = *menup;

#నిర్వచితమైతే FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    ఒకవేళ (gui.లోపల_use)
	gui_mch_destroy_menu(menu);
#అయితే_ముగింపు

    // Don't change *menup until after calling gui_mch_destroy_menu(). The
    // MacOS code needs the original structure to properly delete the menu.
    *menup = menu->తదుపరి;
    vim_free(menu->name);
    vim_free(menu->dname);
#నిర్వచితమైతే FEAT_MULTI_LANG
    vim_free(menu->en_name);
    vim_free(menu->en_dname);
#అయితే_ముగింపు
    vim_free(menu->actext);
#నిర్వచితమైతే FEAT_TOOLBAR
    vim_free(menu->iconfile);
# ifdef FEAT_GUI_MOTIF
    vim_free(menu->xpm_fname);
# endif
#అయితే_ముగింపు
    కొరకు (i = 0; i < MENU_MODES; i++)
	free_menu_string(menu, i);
    vim_free(menu);

#నిర్వచితమైతే FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#అయితే_ముగింపు
}

/*
 * Free the menu->string with the given index.
 */
    స్థిర శూన్య
free_menu_string(vimmenu_T *menu, పూర్ణాంకం idx)
{
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < MENU_MODES; i++)
	ఒకవేళ (menu->అక్షరశ్రేణులు[i] == menu->అక్షరశ్రేణులు[idx])
	    లెక్క++;
    ఒకవేళ (లెక్క == 1)
	vim_free(menu->అక్షరశ్రేణులు[idx]);
    menu->అక్షరశ్రేణులు[idx] = ఖాళీ;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    స్థిర పూర్ణాంకం
show_menus(char_u *మార్గం_name, పూర్ణాంకం modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*మాతృ = ఖాళీ;

    name = మార్గం_name = vim_strsave(మార్గం_name);
    ఒకవేళ (మార్గం_name == ఖాళీ)
	తిరిగి FAIL;
    menu = *get_మూలం_menu(మార్గం_name);

    // First, find the (sub)menu with the given name
    అయితే (*name)
    {
	p = menu_name_skip(name);
	అయితే (menu != ఖాళీ)
	{
	    ఒకవేళ (menu_name_equal(name, menu))
	    {
		// Found menu
		ఒకవేళ (*p != NUL && menu->children == ఖాళీ)
		{
		    emsg(_(e_part_యొక్క_menu_అంశం_మార్గం_is_not_sub_menu));
		    vim_free(మార్గం_name);
		    తిరిగి FAIL;
		}
		లేకపోతే ఒకవేళ ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_లోపల_another_బహుళకం));
		    vim_free(మార్గం_name);
		    తిరిగి FAIL;
		}
		ఆపు;
	    }
	    menu = menu->తదుపరి;
	}
	ఒకవేళ (menu == ఖాళీ)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(మార్గం_name);
	    తిరిగి FAIL;
	}
	name = p;
	మాతృ = menu;
	menu = menu->children;
    }
    vim_free(మార్గం_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    సందేశం_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(మాతృ, modes, 0);

    --menus_locked;
    తిరిగి OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    స్థిర శూన్య
show_menus_recursive(vimmenu_T *menu, పూర్ణాంకం modes, పూర్ణాంకం లోతు)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		బిట్టు;

    ఒకవేళ (menu != ఖాళీ && (menu->modes & modes) == 0x0)
	తిరిగి;

    ఒకవేళ (menu != ఖాళీ)
    {
	సందేశం_putchar('\n');
	ఒకవేళ (got_int)		// "q" hit for "--more--"
	    తిరిగి;
	కొరకు (i = 0; i < లోతు; i++)
	    సందేశం_puts("  ");
	ఒకవేళ (menu->ప్రాధాన్యత)
	{
	    సందేశం_outnum((దీర్ఘ)menu->ప్రాధాన్యత);
	    సందేశం_puts(" ");
	}
				// Same highlighting as for directories!?
	సందేశం_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    ఒకవేళ (menu != ఖాళీ && menu->children == ఖాళీ)
    {
	కొరకు (బిట్టు = 0; బిట్టు < MENU_MODES; బిట్టు++)
	    ఒకవేళ ((menu->modes & modes & (1 << బిట్టు)) != 0)
	    {
		సందేశం_putchar('\n');
		ఒకవేళ (got_int)		// "q" hit for "--more--"
		    తిరిగి;
		కొరకు (i = 0; i < లోతు + 2; i++)
		    సందేశం_puts("  ");
		సందేశం_puts(menu_బహుళకం_chars[బిట్టు]);
		ఒకవేళ (menu->noremap[బిట్టు] == REMAP_NONE)
		    సందేశం_putchar('*');
		లేకపోతే ఒకవేళ (menu->noremap[బిట్టు] == REMAP_SCRIPT)
		    సందేశం_putchar('&');
		లేకపోతే
		    సందేశం_putchar(' ');
		ఒకవేళ (menu->silent[బిట్టు])
		    సందేశం_putchar('s');
		లేకపోతే
		    సందేశం_putchar(' ');
		ఒకవేళ ((menu->modes & menu->enabled & (1 << బిట్టు)) == 0)
		    సందేశం_putchar('-');
		లేకపోతే
		    సందేశం_putchar(' ');
		సందేశం_puts(" ");
		ఒకవేళ (*menu->అక్షరశ్రేణులు[బిట్టు] == NUL)
		    సందేశం_puts_attr("<Nop>", HL_ATTR(HLF_8));
		లేకపోతే
		    సందేశం_outtrans_special(menu->అక్షరశ్రేణులు[బిట్టు], FALSE, 0);
	    }
    }
    లేకపోతే
    {
	ఒకవేళ (menu == ఖాళీ)
	{
	    menu = మూలం_menu;
	    లోతు--;
	}
	లేకపోతే
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	కొరకు (; menu != ఖాళీ && !got_int; menu = menu->తదుపరి)
	    ఒకవేళ (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, లోతు + 1);
    }
}

/*
 * Used when expanding menu names.
 */
స్థిర vimmenu_T	*expand_menu = ఖాళీ;
స్థిర vimmenu_T	*expand_menu_alt = ఖాళీ;
స్థిర పూర్ణాంకం		expand_modes = 0x0;
స్థిర పూర్ణాంకం		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
సమితి_context_లోపల_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    పూర్ణాంకం		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*మార్గం_name = ఖాళీ;
    char_u	*name;
    పూర్ణాంకం		unmenu;
    vimmenu_T	*menu;
    పూర్ణాంకం		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    కొరకు (p = arg; *p; ++p)
	ఒకవేళ (!VIM_ISDIGIT(*p) && *p != '.')
	    ఆపు;

    ఒకవేళ (!VIM_ISWHITE(*p))
    {
	ఒకవేళ (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	లేకపోతే ఒకవేళ (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	లేకపోతే
	    p = arg;
    }

    అయితే (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    కొరకు (; *p && !VIM_ISWHITE(*p); ++p)
    {
	ఒకవేళ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	లేకపోతే ఒకవేళ (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    ఒకవేళ (expand_menus && VIM_ISWHITE(*p))
	తిరిగి ఖాళీ;	// TODO: check for next command?
    ఒకవేళ (*p == NUL)		// Complete the menu name
    {
	పూర్ణాంకం try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, ఖాళీ, &unmenu);
	ఒకవేళ (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = మూలం_menu;
	ఒకవేళ (after_dot != arg)
	{
	    మార్గం_name = alloc(after_dot - arg);
	    ఒకవేళ (మార్గం_name == ఖాళీ)
		తిరిగి ఖాళీ;
	    vim_strncpy(మార్గం_name, arg, after_dot - arg - 1);
	}
	name = మార్గం_name;
	అయితే (name != ఖాళీ && *name)
	{
	    p = menu_name_skip(name);
	    అయితే (menu != ఖాళీ)
	    {
		ఒకవేళ (menu_name_equal(name, menu))
		{
		    // Found menu
		    ఒకవేళ ((*p != NUL && menu->children == ఖాళీ)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(మార్గం_name);
			తిరిగి ఖాళీ;
		    }
		    ఆపు;
		}
		menu = menu->తదుపరి;
		ఒకవేళ (menu == ఖాళీ && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    ఒకవేళ (menu == ఖాళీ)
	    {
		// No menu found with the name we were looking for
		vim_free(మార్గం_name);
		తిరిగి ఖాళీ;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(మార్గం_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_నమూనా = after_dot;
	expand_menu = menu;
	ఒకవేళ (expand_menu == మూలం_menu)
	    expand_menu_alt = curwin->w_winbar;
	లేకపోతే
	    expand_menu_alt = ఖాళీ;
    }
    లేకపోతే			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    తిరిగి ఖాళీ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    స్థిర vimmenu_T	*menu = ఖాళీ;
    స్థిర పూర్ణాంకం		did_alt_menu = FALSE;
    char_u		*str;
#నిర్వచితమైతే FEAT_MULTI_LANG
    స్థిర  పూర్ణాంకం		should_advance = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#నిర్వచితమైతే FEAT_MULTI_LANG
	should_advance = FALSE;
#అయితే_ముగింపు
    }

    // Skip PopUp[nvoci].
    అయితే (menu != ఖాళీ && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == ఖాళీ))
    {
	menu = menu->తదుపరి;
	ఒకవేళ (menu == ఖాళీ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ఒకవేళ (menu == ఖాళీ)	    // at end of linked list
	తిరిగి ఖాళీ;

    ఒకవేళ (menu->modes & expand_modes)
#నిర్వచితమైతే FEAT_MULTI_LANG
	ఒకవేళ (should_advance)
	    str = menu->en_dname;
	లేకపోతే
	{
#అయితే_ముగింపు
	    str = menu->dname;
#నిర్వచితమైతే FEAT_MULTI_LANG
	    ఒకవేళ (menu->en_dname == ఖాళీ)
		should_advance = TRUE;
	}
#అయితే_ముగింపు
    లేకపోతే
	str = (char_u *)"";

#నిర్వచితమైతే FEAT_MULTI_LANG
    ఒకవేళ (should_advance)
#అయితే_ముగింపు
    {
	// Advance to next menu entry.
	menu = menu->తదుపరి;
	ఒకవేళ (menu == ఖాళీ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#నిర్వచితమైతే FEAT_MULTI_LANG
    should_advance = !should_advance;
#అయితే_ముగింపు

    తిరిగి str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_పేర్లు(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    స్థిర vimmenu_T	*menu = ఖాళీ;
    స్థిర పూర్ణాంకం		did_alt_menu = FALSE;
#నిర్వచించు TBUFFER_LEN 256
    స్థిర char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#నిర్వచితమైతే FEAT_MULTI_LANG
    స్థిర  పూర్ణాంకం		should_advance = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#నిర్వచితమైతే FEAT_MULTI_LANG
	should_advance = FALSE;
#అయితే_ముగింపు
    }

    // Skip Browse-style entries, popup menus and separators.
    అయితే (menu != ఖాళీ
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#నిర్వచితం_కాకపోతే FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#అయితే_ముగింపు
	       ))
    {
	menu = menu->తదుపరి;
	ఒకవేళ (menu == ఖాళీ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ఒకవేళ (menu == ఖాళీ)	    // at end of linked list
	తిరిగి ఖాళీ;

    ఒకవేళ (menu->modes & expand_modes)
    {
	ఒకవేళ (menu->children != ఖాళీ)
	{
#నిర్వచితమైతే FEAT_MULTI_LANG
	    ఒకవేళ (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    లేకపోతే
	    {
#అయితే_ముగింపు
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#నిర్వచితమైతే FEAT_MULTI_LANG
		ఒకవేళ (menu->en_dname == ఖాళీ)
		    should_advance = TRUE;
	    }
#అయితే_ముగింపు
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	లేకపోతే
#నిర్వచితమైతే FEAT_MULTI_LANG
	{
	    ఒకవేళ (should_advance)
		str = menu->en_dname;
	    లేకపోతే
	    {
#అయితే_ముగింపు
		str = menu->dname;
#నిర్వచితమైతే FEAT_MULTI_LANG
		ఒకవేళ (menu->en_dname == ఖాళీ)
		    should_advance = TRUE;
	    }
	}
#అయితే_ముగింపు
    }
    లేకపోతే
	str = (char_u *)"";

#నిర్వచితమైతే FEAT_MULTI_LANG
    ఒకవేళ (should_advance)
#అయితే_ముగింపు
    {
	// Advance to next menu entry.
	menu = menu->తదుపరి;
	ఒకవేళ (menu == ఖాళీ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#నిర్వచితమైతే FEAT_MULTI_LANG
    should_advance = !should_advance;
#అయితే_ముగింపు

    తిరిగి str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    స్థిర char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    కొరకు (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    ఒకవేళ (*p == NUL)
		ఆపు;
	}
    }
    ఒకవేళ (*p)
	*p++ = NUL;
    తిరిగి p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    స్థిర పూర్ణాంకం
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#నిర్వచితమైతే FEAT_MULTI_LANG
    ఒకవేళ (menu->en_name != ఖాళీ
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	తిరిగి TRUE;
#అయితే_ముగింపు
    తిరిగి menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    స్థిర పూర్ణాంకం
menu_namecmp(char_u *name, char_u *mname)
{
    పూర్ణాంకం		i;

    కొరకు (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	ఒకవేళ (name[i] != mname[i])
	    ఆపు;
    తిరిగి ((name[i] == NUL || name[i] == TAB)
	    && (mname[i] == NUL || mname[i] == TAB));
}

/*
 * Return the modes specified by the given menu command (eg :menu! returns
 * MENU_CMDLINE_MODE | MENU_INSERT_MODE).
 * If "noremap" is not NULL, then the flag it points to is set according to
 * whether the command is a "nore" command.
 * If "unmenu" is not NULL, then the flag it points to is set according to
 * whether the command is an "unmenu" command.
 */
    స్థిర పూర్ణాంకం
get_menu_cmd_modes(
    char_u  *cmd,
    పూర్ణాంకం	    forceit,	    // Was there a "!" after the command?
    పూర్ణాంకం	    *noremap,
    పూర్ణాంకం	    *unmenu)
{
    పూర్ణాంకం	    modes;

    ఎంపిక (*cmd++)
    {
	స్థితి 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం;
	    ఆపు;
	స్థితి 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_బహుళకం;
	    ఆపు;
	స్థితి 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_బహుళకం;
	    ఆపు;
	స్థితి 'o':			// omenu
	    modes = MENU_OP_PENDING_బహుళకం;
	    ఆపు;
	స్థితి 'i':			// imenu
	    modes = MENU_చొప్పించు_బహుళకం;
	    ఆపు;
	స్థితి 't':
	    ఒకవేళ (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_బహుళకం;
		++cmd;
		ఆపు;
	    }
	    modes = MENU_TIP_బహుళకం;	// tmenu
	    ఆపు;
	స్థితి 'c':			// cmenu
	    modes = MENU_CMDLINE_బహుళకం;
	    ఆపు;
	స్థితి 'a':			// amenu
	    modes = MENU_చొప్పించు_బహుళకం | MENU_CMDLINE_బహుళకం | MENU_సాధారణ_బహుళకం
				    | MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం
				    | MENU_OP_PENDING_బహుళకం;
	    ఆపు;
	స్థితి 'n':
	    ఒకవేళ (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_సాధారణ_బహుళకం;
		ఆపు;
	    }
	    // FALLTHROUGH
	అప్రమేయ:
	    --cmd;
	    ఒకవేళ (forceit)		// menu!!
		modes = MENU_చొప్పించు_బహుళకం | MENU_CMDLINE_బహుళకం;
	    లేకపోతే			// menu
		modes = MENU_సాధారణ_బహుళకం | MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం
						       | MENU_OP_PENDING_బహుళకం;
    }

    ఒకవేళ (noremap != ఖాళీ)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    ఒకవేళ (unmenu != ఖాళీ)
	*unmenu = (*cmd == 'u');
    తిరిగి modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    స్థిర char_u *
get_menu_బహుళకం_str(పూర్ణాంకం modes)
{
    ఒకవేళ ((modes & (MENU_చొప్పించు_బహుళకం | MENU_CMDLINE_బహుళకం | MENU_సాధారణ_బహుళకం |
		  MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం | MENU_OP_PENDING_బహుళకం))
	    == (MENU_చొప్పించు_బహుళకం | MENU_CMDLINE_బహుళకం | MENU_సాధారణ_బహుళకం |
		MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం | MENU_OP_PENDING_బహుళకం))
	తిరిగి (char_u *)"a";
    ఒకవేళ ((modes & (MENU_సాధారణ_బహుళకం | MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం |
		  MENU_OP_PENDING_బహుళకం))
	    == (MENU_సాధారణ_బహుళకం | MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం |
		MENU_OP_PENDING_బహుళకం))
	తిరిగి (char_u *)" ";
    ఒకవేళ ((modes & (MENU_చొప్పించు_బహుళకం | MENU_CMDLINE_బహుళకం))
	    == (MENU_చొప్పించు_బహుళకం | MENU_CMDLINE_బహుళకం))
	తిరిగి (char_u *)"!";
    ఒకవేళ ((modes & (MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం))
	    == (MENU_VISUAL_బహుళకం | MENU_SELECT_బహుళకం))
	తిరిగి (char_u *)"v";
    ఒకవేళ (modes & MENU_VISUAL_బహుళకం)
	తిరిగి (char_u *)"x";
    ఒకవేళ (modes & MENU_SELECT_బహుళకం)
	తిరిగి (char_u *)"s";
    ఒకవేళ (modes & MENU_OP_PENDING_బహుళకం)
	తిరిగి (char_u *)"o";
    ఒకవేళ (modes & MENU_చొప్పించు_బహుళకం)
	తిరిగి (char_u *)"i";
    ఒకవేళ (modes & MENU_TERMINAL_బహుళకం)
	తిరిగి (char_u *)"tl";
    ఒకవేళ (modes & MENU_CMDLINE_బహుళకం)
	తిరిగి (char_u *)"c";
    ఒకవేళ (modes & MENU_సాధారణ_బహుళకం)
	తిరిగి (char_u *)"n";
    ఒకవేళ (modes & MENU_TIP_బహుళకం)
	తిరిగి (char_u *)"t";

    తిరిగి (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    స్థిర char_u *
popup_బహుళకం_name(char_u *name, పూర్ణాంకం idx)
{
    char_u	*p;
    పూర్ణాంకం		len = (పూర్ణాంకం)STRLEN(name);
    అక్షరం	*బహుళకం_chars = menu_బహుళకం_chars[idx];
    పూర్ణాంకం		బహుళకం_chars_len = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(బహుళకం_chars);
    పూర్ణాంకం		i;

    p = vim_strnsave(name, len + బహుళకం_chars_len);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;

    mch_memmove(p + 5 + బహుళకం_chars_len, p + 5, (size_t)(len - 4));
    కొరకు (i = 0; i < బహుళకం_chars_len; ++i)
	p[5 + i] = menu_బహుళకం_chars[idx][i];
    తిరిగి p;
}

#ఒకవేళ defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    పూర్ణాంకం
get_menu_సూచీ(vimmenu_T *menu, పూర్ణాంకం state)
{
    పూర్ణాంకం		idx;

    ఒకవేళ ((state & బహుళకం_చొప్పించు))
	idx = MENU_సూచీ_చొప్పించు;
    లేకపోతే ఒకవేళ (state & బహుళకం_CMDLINE)
	idx = MENU_సూచీ_CMDLINE;
# ifdef FEAT_TERMINAL
    లేకపోతే ఒకవేళ (పదం_use_loop())
	idx = MENU_సూచీ_TERMINAL;
# endif
    లేకపోతే ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (VIsual_select)
	    idx = MENU_సూచీ_SELECT;
	లేకపోతే
	    idx = MENU_సూచీ_VISUAL;
    }
    లేకపోతే ఒకవేళ (state == బహుళకం_HITRETURN || state == బహుళకం_ASKMORE)
	idx = MENU_సూచీ_CMDLINE;
    లేకపోతే ఒకవేళ (ముగింపు_op)
	idx = MENU_సూచీ_OP_PENDING;
    లేకపోతే ఒకవేళ ((state & బహుళకం_సాధారణ))
	idx = MENU_సూచీ_సాధారణ;
    లేకపోతే
	idx = MENU_సూచీ_INVALID;

    ఒకవేళ (idx != MENU_సూచీ_INVALID && menu->అక్షరశ్రేణులు[idx] == ఖాళీ)
	idx = MENU_సూచీ_INVALID;
    తిరిగి idx;
}
#అయితే_ముగింపు

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    స్థిర char_u *
menu_పాఠ్యం(char_u *str, పూర్ణాంకం *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*పాఠ్యం;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    ఒకవేళ (p != ఖాళీ)
    {
	ఒకవేళ (actext != ఖాళీ)
	    *actext = vim_strsave(p + 1);
	పాఠ్యం = vim_strnsave(str, p - str);
    }
    లేకపోతే
	పాఠ్యం = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    కొరకు (p = పాఠ్యం; p != ఖాళీ; )
    {
	p = vim_strchr(p, '&');
	ఒకవేళ (p != ఖాళీ)
	{
	    ఒకవేళ (p[1] == NUL)	    // trailing "&"
		ఆపు;
	    ఒకవేళ (mnemonic != ఖాళీ && p[1] != '&')
#ఒకవేళ !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#లేకపోతే
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		అక్షరం c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#అయితే_ముగింపు
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    తిరిగి పాఠ్యం;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    పూర్ణాంకం
menu_is_menubar(char_u *name)
{
    తిరిగి (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    పూర్ణాంకం
menu_is_popup(char_u *name)
{
    తిరిగి (STRNCMP(name, "PopUp", 5) == 0);
}

#ఒకవేళ defined(FEAT_GUI_MOTIF) && (Xm_సంస్కరణ <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    పూర్ణాంకం
menu_is_బిడ్డ_యొక్క_popup(vimmenu_T *menu)
{
    అయితే (menu->మాతృ != ఖాళీ)
	menu = menu->మాతృ;
    తిరిగి menu_is_popup(menu->name);
}
#అయితే_ముగింపు

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    పూర్ణాంకం
menu_is_toolbar(char_u *name)
{
    తిరిగి (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    పూర్ణాంకం
menu_is_separator(char_u *name)
{
    తిరిగి (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    స్థిర పూర్ణాంకం
menu_is_hidden(char_u *name)
{
    తిరిగి (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    స్థిర పూర్ణాంకం
menu_is_tearoff(char_u *name వాడని)
{
#నిర్వచితమైతే FEAT_GUI
    తిరిగి (STRCMP(name, TEAR_STRING) == 0);
#లేకపోతే
    తిరిగి FALSE;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_పదం_POPUP_MENU)

    స్థిర పూర్ణాంకం
get_menu_బహుళకం(శూన్య)
{
# ifdef FEAT_TERMINAL
    ఒకవేళ (పదం_use_loop())
	తిరిగి MENU_సూచీ_TERMINAL;
# endif
    ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (VIsual_select)
	    తిరిగి MENU_సూచీ_SELECT;
	తిరిగి MENU_సూచీ_VISUAL;
    }
    ఒకవేళ (State & బహుళకం_చొప్పించు)
	తిరిగి MENU_సూచీ_చొప్పించు;
    ఒకవేళ ((State & బహుళకం_CMDLINE) || State == బహుళకం_ASKMORE
						    || State == బహుళకం_HITRETURN)
	తిరిగి MENU_సూచీ_CMDLINE;
    ఒకవేళ (ముగింపు_op)
	తిరిగి MENU_సూచీ_OP_PENDING;
    ఒకవేళ (State & బహుళకం_సాధారణ)
	తిరిగి MENU_సూచీ_సాధారణ;
    ఒకవేళ (State & బహుళకం_LANGMAP)	// must be a "r" command, like Insert mode
	తిరిగి MENU_సూచీ_చొప్పించు;
    తిరిగి MENU_సూచీ_INVALID;
}

    పూర్ణాంకం
get_menu_బహుళకం_flag(శూన్య)
{
    పూర్ణాంకం బహుళకం = get_menu_బహుళకం();

    ఒకవేళ (బహుళకం == MENU_సూచీ_INVALID)
	తిరిగి 0;
    తిరిగి 1 << బహుళకం;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    శూన్య
show_popupmenu(శూన్య)
{
    vimmenu_T	*menu;
    పూర్ణాంకం		menu_బహుళకం;
    అక్షరం*	బహుళకం;
    పూర్ణాంకం		బహుళకం_len;

    menu_బహుళకం = get_menu_బహుళకం();
    ఒకవేళ (menu_బహుళకం == MENU_సూచీ_INVALID)
	తిరిగి;
    బహుళకం = menu_బహుళకం_chars[menu_బహుళకం];
    బహుళకం_len = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(బహుళకం);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)బహుళకం, ఖాళీ, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	ఒకవేళ (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, బహుళకం, బహుళకం_len) == 0)
	    ఆపు;

    // Only show a popup when it is defined and has entries
    ఒకవేళ (menu == ఖాళీ || menu->children == ఖాళీ)
	తిరిగి;

# ఒకవేళ defined(FEAT_GUI)
    ఒకవేళ (gui.లోపల_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# ఒకవేళ defined(FEAT_GUI) && defined(FEAT_పదం_POPUP_MENU)
    లేకపోతే
# endif
# ఒకవేళ defined(FEAT_పదం_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    పూర్ణాంకం
check_menu_సూచిక(vimmenu_T *మూలం, vimmenu_T *menu_గమ్యం_check)
{
    vimmenu_T	*p;

    కొరకు (p = మూలం; p != ఖాళీ; p = p->తదుపరి)
	ఒకవేళ (p == menu_గమ్యం_check
		|| (p->children != ఖాళీ
		    && check_menu_సూచిక(p->children, menu_గమ్యం_check) == OK))
	    తిరిగి OK;
    తిరిగి FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    శూన్య
gui_create_initial_menus(vimmenu_T *menu)
{
    పూర్ణాంకం		idx = 0;

    అయితే (menu != ఖాళీ)
    {
	// Don't add a menu when only a tip was defined.
	ఒకవేళ (menu->modes & MENU_ALL_MODES)
	{
	    ఒకవేళ (menu->children != ఖాళీ)
	    {
		gui_mch_కూడు_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    లేకపోతే
		gui_mch_కూడు_menu_అంశం(menu, idx);
	}
	menu = menu->తదుపరి;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    స్థిర శూన్య
gui_update_menus_recurse(vimmenu_T *menu, పూర్ణాంకం బహుళకం)
{
    పూర్ణాంకం		grey;

    అయితే (menu)
    {
	ఒకవేళ ((menu->modes & menu->enabled & బహుళకం)
# ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	లేకపోతే
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	ఒకవేళ (vim_strchr(p_go, GO_GREY) != ఖాళీ || menu->మాతృ == ఖాళీ
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->మాతృ->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	లేకపోతే
	    gui_mch_menu_hidden(menu, grey);
	gui_update_menus_recurse(menu->children, బహుళకం);
	menu = menu->తదుపరి;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    శూన్య
gui_update_menus(పూర్ణాంకం modes)
{
    స్థిర పూర్ణాంకం	    prev_బహుళకం = -1;
    పూర్ణాంకం		    బహుళకం = 0;

    ఒకవేళ (modes != 0x0)
	బహుళకం = modes;
    లేకపోతే
	బహుళకం = get_menu_బహుళకం_flag();

    ఒకవేళ (force_menu_update || బహుళకం != prev_బహుళకం)
    {
	gui_update_menus_recurse(మూలం_menu, బహుళకం);
	gui_mch_draw_menubar();
	prev_బహుళకం = బహుళకం;
	force_menu_update = FALSE;
    }
}

# ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    పూర్ణాంకం
gui_is_menu_shortcut(పూర్ణాంకం కీలకం)
{
    vimmenu_T	*menu;

    ఒకవేళ (కీలకం < 256)
	కీలకం = TOLOWER_LOC(కీలకం);
    FOR_ALL_MENUS(menu)
	ఒకవేళ (menu->mnemonic == కీలకం
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == కీలకం))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}
# endif
#అయితే_ముగింపు // FEAT_GUI

#ఒకవేళ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    శూన్య
gui_mch_toggle_tearoffs(పూర్ణాంకం enable)
{
    పూర్ణాంకం		pri_tab[MENUDEPTH + 1];
    పూర్ణాంకం		i;

    ఒకవేళ (enable)
    {
	కొరకు (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(మూలం_menu, (char_u *)"", pri_tab, 0);
    }
    లేకపోతే
	gui_destroy_tearoffs_recurse(మూలం_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    స్థిర శూన్య
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    స్థిరరాశి char_u	*pname,
    పూర్ణాంకం			*pri_tab,
    పూర్ణాంకం			pri_idx)
{
    char_u	*newpname = ఖాళీ;
    పూర్ణాంకం		len;
    char_u	*s;
    char_u	*d;

    ఒకవేళ (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    అయితే (menu != ఖాళీ)
    {
	ఒకవేళ (menu->children != ఖాళీ && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (పూర్ణాంకం)STRLEN(pname) + (పూర్ణాంకం)STRLEN(menu->name);
	    కొరకు (s = menu->name; *s; ++s)
		ఒకవేళ (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    ఒకవేళ (newpname != ఖాళీ)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		కొరకు (s = menu->name; *s; ++s)
		{
		    ఒకవేళ (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		ఒకవేళ (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_కూడు_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			// remove TEAR_STRING
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->తదుపరి;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    స్థిర శూన్య
gui_కూడు_tearoff(char_u *tearpath, పూర్ణాంకం *pri_tab, పూర్ణాంకం pri_idx)
{
    char_u	*tbuf;
    పూర్ణాంకం		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (గుర్తులేని పూర్ణాంకం)STRLEN(tearpath));
    ఒకవేళ (tbuf == ఖాళీ)
	తిరిగి;

    tbuf[0] = K_SPECIAL;
    tbuf[1] = K_రెండవ(K_TEAROFF);
    tbuf[2] = K_మూడవ(K_TEAROFF);
    STRCPY(tbuf + 3, tearpath);
    STRCAT(tbuf + 3, "\r");

    STRCAT(tearpath, ".");
    STRCAT(tearpath, TEAR_STRING);

    // Priority of tear-off is always 1
    t = pri_tab[pri_idx + 1];
    pri_tab[pri_idx + 1] = 1;

# ifdef FEAT_TOOLBAR
    menuarg.iconfile = ఖాళీ;
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
# endif
    menuarg.noremap[0] = REMAP_NONE;
    menuarg.silent[0] = TRUE;

    menuarg.modes = MENU_ALL_MODES;
    కూడు_menu_మార్గం(tearpath, &menuarg, pri_tab, tbuf, FALSE);

    menuarg.modes = MENU_TIP_బహుళకం;
    కూడు_menu_మార్గం(tearpath, &menuarg, pri_tab,
	    (char_u *)_("Tear off this menu"), FALSE);

    pri_tab[pri_idx + 1] = t;
    vim_free(tbuf);
}

/*
 * Recursively destroy tearoff items
 */
    స్థిర శూన్య
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    అయితే (menu)
    {
	ఒకవేళ (menu->children)
	{
	    // check if tearoff exists
	    ఒకవేళ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    ఒకవేళ (menu->children != ఖాళీ) // if not the last one
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->తదుపరి;
    }
}

#అయితే_ముగింపు // FEAT_GUI_MSWIN && FEAT_TEAROFF

/*
 * Execute "menu".  Use by ":emenu" and the window toolbar.
 * "eap" is NULL for the window toolbar.
 * "mode_idx" specifies a MENU_INDEX_ value, use MENU_INDEX_INVALID to depend
 * on the current state.
 */
    శూన్య
execute_menu(exarg_T *eap, vimmenu_T *menu, పూర్ణాంకం బహుళకం_idx)
{
    పూర్ణాంకం		idx = బహుళకం_idx;

    ఒకవేళ (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	ఒకవేళ (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_సూచీ_చొప్పించు;
	}
#నిర్వచితమైతే FEAT_TERMINAL
	లేకపోతే ఒకవేళ (పదం_use_loop())
	{
	    idx = MENU_సూచీ_TERMINAL;
	}
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (VIsual_చురుకైన)
	{
	    idx = MENU_సూచీ_VISUAL;
	}
	లేకపోతే ఒకవేళ (eap != ఖాళీ && eap->addr_లెక్క)
	{
	    pos_T	tpos;

	    idx = MENU_సూచీ_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    ఒకవేళ ((curbuf->b_visual.vi_ప్రారంభం.lnum == eap->line1)
		    && (curbuf->b_visual.vi_చివర.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_బహుళకం = curbuf->b_visual.vi_బహుళకం;
		tpos = curbuf->b_visual.vi_చివర;
		curwin->w_cursor = curbuf->b_visual.vi_ప్రారంభం;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    లేకపోతే
	    {
		// Set it up for line-wise visual mode
		VIsual_బహుళకం = 'V';
		curwin->w_cursor.lnum = eap->line1;
		curwin->w_cursor.నిలువు = 1;
		tpos.lnum = eap->line2;
		tpos.నిలువు = MAXCOL;
		tpos.coladd = 0;
	    }

	    // Activate visual mode
	    VIsual_చురుకైన = TRUE;
	    VIsual_reselect = TRUE;
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;

	    check_cursor();

	    // Adjust the cursor to make sure it is in the correct pos
	    // for exclusive mode
	    ఒకవేళ (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.నిలువు;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    ఒకవేళ (idx == MENU_సూచీ_INVALID || eap == ఖాళీ)
	idx = MENU_సూచీ_సాధారణ;

    ఒకవేళ (menu->అక్షరశ్రేణులు[idx] != ఖాళీ && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	ఒకవేళ (eap == ఖాళీ || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_సాధారణ_busy;
	    ఒకవేళ (save_current_state(&save_state))
		exec_సాధారణ_cmd(menu->అక్షరశ్రేణులు[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_సాధారణ_busy;
	}
	లేకపోతే
	    ins_typebuf(menu->అక్షరశ్రేణులు[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    లేకపోతే ఒకవేళ (eap != ఖాళీ)
    {
	char_u	*బహుళకం;

	ఎంపిక (idx)
	{
	    స్థితి MENU_సూచీ_VISUAL:
		బహుళకం = (char_u *)"Visual";
		ఆపు;
	    స్థితి MENU_సూచీ_SELECT:
		బహుళకం = (char_u *)"Select";
		ఆపు;
	    స్థితి MENU_సూచీ_OP_PENDING:
		బహుళకం = (char_u *)"Op-pending";
		ఆపు;
	    స్థితి MENU_సూచీ_TERMINAL:
		బహుళకం = (char_u *)"Terminal";
		ఆపు;
	    స్థితి MENU_సూచీ_చొప్పించు:
		బహుళకం = (char_u *)"Insert";
		ఆపు;
	    స్థితి MENU_సూచీ_CMDLINE:
		బహుళకం = (char_u *)"Cmdline";
		ఆపు;
	    // case MENU_INDEX_TIP: cannot happen
	    అప్రమేయ:
		బహుళకం = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_బహుళకం), బహుళకం);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    స్థిర vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    పూర్ణాంకం		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    ఒకవేళ (saved_name == ఖాళీ)
	తిరిగి ఖాళీ;

    menu = *get_మూలం_menu(saved_name);
    name = saved_name;
    అయితే (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	అయితే (menu != ఖాళీ)
	{
	    ఒకవేళ (menu_name_equal(name, menu))
	    {
		ఒకవేళ (*p == NUL && menu->children != ఖాళీ)
		{
		    emsg(_(e_menu_మార్గం_must_lead_గమ్యం_menu_అంశం));
		    gave_emsg = TRUE;
		    menu = ఖాళీ;
		}
		లేకపోతే ఒకవేళ (*p != NUL && menu->children == ఖాళీ)
		{
		    emsg(_(e_part_యొక్క_menu_అంశం_మార్గం_is_not_sub_menu));
		    menu = ఖాళీ;
		}
		ఆపు;
	    }
	    menu = menu->తదుపరి;
	}
	ఒకవేళ (menu == ఖాళీ || *p == NUL)
	    ఆపు;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    ఒకవేళ (menu == ఖాళీ)
    {
	ఒకవేళ (!gave_emsg)
	    semsg(_(e_menu_not_దొరికింది_str), name_arg);
	తిరిగి ఖాళీ;
    }

    తిరిగి menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    శూన్య
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    పూర్ణాంకం		బహుళకం_idx = MENU_సూచీ_INVALID;

    ఒకవేళ (arg[0] && VIM_ISWHITE(arg[1]))
    {
	ఎంపిక (arg[0])
	{
	    స్థితి 'n': బహుళకం_idx = MENU_సూచీ_సాధారణ; ఆపు;
	    స్థితి 'v': బహుళకం_idx = MENU_సూచీ_VISUAL; ఆపు;
	    స్థితి 's': బహుళకం_idx = MENU_సూచీ_SELECT; ఆపు;
	    స్థితి 'o': బహుళకం_idx = MENU_సూచీ_OP_PENDING; ఆపు;
	    స్థితి 't': బహుళకం_idx = MENU_సూచీ_TERMINAL; ఆపు;
	    స్థితి 'i': బహుళకం_idx = MENU_సూచీ_చొప్పించు; ఆపు;
	    స్థితి 'c': బహుళకం_idx = MENU_సూచీ_CMDLINE; ఆపు;
	    అప్రమేయ: semsg(_(e_invalid_వాదం_str), arg);
		     తిరిగి;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    ఒకవేళ (menu == ఖాళీ)
	తిరిగి;

    // Found the menu, so execute.
    execute_menu(eap, menu, బహుళకం_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    శూన్య
winbar_click(win_T *wp, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం		idx;

    ఒకవేళ (wp->w_winbar_వస్తువులు == ఖాళీ)
	తిరిగి;
    కొరకు (idx = 0; wp->w_winbar_వస్తువులు[idx].wb_menu != ఖాళీ; ++idx)
    {
	winbar_అంశం_T *అంశం = &wp->w_winbar_వస్తువులు[idx];

	ఒకవేళ (నిలువు >= అంశం->wb_startcol && నిలువు <= అంశం->wb_endcol)
	{
	    win_T   *save_curwin = ఖాళీ;
	    pos_T   save_visual = VIsual;
	    పూర్ణాంకం	    save_visual_చురుకైన = VIsual_చురుకైన;
	    పూర్ణాంకం	    save_visual_select = VIsual_select;
	    పూర్ణాంకం	    save_visual_reselect = VIsual_reselect;
	    పూర్ణాంకం	    save_visual_బహుళకం = VIsual_బహుళకం;

	    ఒకవేళ (wp != curwin)
	    {
		// Clicking in the window toolbar of a not-current window.
		// Make that window the current one and save Visual mode.
		save_curwin = curwin;
		VIsual_చురుకైన = FALSE;
		curwin = wp;
		curbuf = curwin->w_బఫరు;
		check_cursor();
	    }

	    // Note: the command might close the current window.
	    execute_menu(ఖాళీ, అంశం->wb_menu, -1);

	    ఒకవేళ (save_curwin != ఖాళీ && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_బఫరు;
		VIsual = save_visual;
		VIsual_చురుకైన = save_visual_చురుకైన;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_బహుళకం = save_visual_బహుళకం;
	    }
	    ఒకవేళ (!win_valid(wp))
		ఆపు;
	}
    }
}

#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_పదం_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_కనుగొను_menu(char_u *మార్గం_name)
{
    vimmenu_T	*menu = ఖాళీ;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_మూలం_menu(మార్గం_name);

    saved_name = vim_strsave(మార్గం_name);
    ఒకవేళ (saved_name == ఖాళీ)
	తిరిగి ఖాళీ;

    name = saved_name;
    అయితే (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	అయితే (menu != ఖాళీ)
	{
	    ఒకవేళ (menu_name_equal(name, menu))
	    {
		ఒకవేళ (menu->children == ఖాళీ)
		{
		    // found a menu item instead of a sub-menu
		    ఒకవేళ (*p == NUL)
			emsg(_(e_menu_మార్గం_must_lead_గమ్యం_sub_menu));
		    లేకపోతే
			emsg(_(e_part_యొక్క_menu_అంశం_మార్గం_is_not_sub_menu));
		    menu = ఖాళీ;
		    వెళ్ళు theend;
		}
		ఒకవేళ (*p == NUL)	    // found a full match
		    వెళ్ళు theend;
		ఆపు;
	    }
	    menu = menu->తదుపరి;
	}
	ఒకవేళ (menu == ఖాళీ)	// didn't find it
	    ఆపు;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    ఒకవేళ (menu == ఖాళీ)
	emsg(_(e_menu_not_దొరికింది_check_menu_పేర్లు));
theend:
    vim_free(saved_name);
    తిరిగి menu;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

రకనిర్వచనం నిర్మాణం
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*గమ్యం;		// translated name
} menutrans_T;

స్థిర garray_T menutrans_ga = {0, 0, 0, 0, ఖాళీ};
#అయితే_ముగింపు

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    శూన్య
ex_menutranslate(exarg_T *eap వాడని)
{
#నిర్వచితమైతే FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    పూర్ణాంకం			i;
    char_u		*from, *from_noamp, *గమ్యం;

    ఒకవేళ (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, పరిమాణం(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    ఒకవేళ (STRNCMP(arg, "clear", 5) == 0 && చివర్లు_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_దత్తాంశం;
	కొరకు (i = 0; i < menutrans_ga.ga_len; ++i)
	{
	    vim_free(tp[i].from);
	    vim_free(tp[i].from_noamp);
	    vim_free(tp[i].గమ్యం);
	}
	ga_clear(&menutrans_ga);
# ifdef FEAT_EVAL
	// Delete all "menutrans_" global variables.
	del_menutrans_vars();
# endif
    }
    లేకపోతే
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	గమ్యం = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(గమ్యం);
	ఒకవేళ (arg == గమ్యం || చివర్లు_excmd2(eap->arg, from)
		      || చివర్లు_excmd2(eap->arg, గమ్యం)
		      || !చివర్లు_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_వాదం));
	లేకపోతే
	{
	    ఒకవేళ (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_దత్తాంశం;
		from = vim_strsave(from);
		ఒకవేళ (from != ఖాళీ)
		{
		    from_noamp = menu_పాఠ్యం(from, ఖాళీ, ఖాళీ);
		    గమ్యం = vim_strnsave(గమ్యం, arg - గమ్యం);
		    ఒకవేళ (from_noamp != ఖాళీ && గమ్యం != ఖాళీ)
		    {
			menu_translate_tab_and_జరుపు(from);
			menu_translate_tab_and_జరుపు(గమ్యం);
			menu_unescape_name(from);
			menu_unescape_name(గమ్యం);
			tp[menutrans_ga.ga_len].from = from;
			tp[menutrans_ga.ga_len].from_noamp = from_noamp;
			tp[menutrans_ga.ga_len].గమ్యం = గమ్యం;
			++menutrans_ga.ga_len;
		    }
		    లేకపోతే
		    {
			vim_free(from);
			vim_free(from_noamp);
			vim_free(గమ్యం);
		    }
		}
	    }
	}
    }
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    స్థిర char_u *
menu_skip_part(char_u *p)
{
    అయితే (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	ఒకవేళ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    తిరిగి p;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    స్థిర char_u *
menutrans_lookup(char_u *name, పూర్ణాంకం len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_దత్తాంశం;
    పూర్ణాంకం			i;
    char_u		*dname;

    కొరకు (i = 0; i < menutrans_ga.ga_len; ++i)
	ఒకవేళ (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    తిరిగి tp[i].గమ్యం;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_పాఠ్యం(name, ఖాళీ, ఖాళీ);
    name[len] = i;
    ఒకవేళ (dname == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (i = 0; i < menutrans_ga.ga_len; ++i)
	ఒకవేళ (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    తిరిగి tp[i].గమ్యం;
	}
    vim_free(dname);

    తిరిగి ఖాళీ;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    స్థిర శూన్య
menu_unescape_name(char_u *name)
{
    char_u  *p;

    కొరకు (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	ఒకవేళ (*p == '\\')
	    STRMOVE(p, p + 1);
}
#అయితే_ముగింపు // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    స్థిర char_u *
menu_translate_tab_and_జరుపు(char_u *arg_ప్రారంభం)
{
    char_u	*arg = arg_ప్రారంభం;

    అయితే (*arg && !VIM_ISWHITE(*arg))
    {
	ఒకవేళ ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	లేకపోతే ఒకవేళ (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    ఒకవేళ (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    తిరిగి arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    స్థిర పూర్ణాంకం
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, పూర్ణాంకం modes, dict_T *dict)
{
    పూర్ణాంకం		స్థితి_2;
    list_T	*l;

    ఒకవేళ (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	ఒకవేళ (l == ఖాళీ)
	    తిరిగి FAIL;

	ఒకవేళ (dict_కూడు_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    తిరిగి FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	కొరకు (topmenu = menu; topmenu != ఖాళీ; topmenu = topmenu->తదుపరి)
	    ఒకవేళ (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	తిరిగి OK;
    }

    ఒకవేళ (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	తిరిగి OK;

    స్థితి_2 = dict_కూడు_string(dict, "name", menu->name);
    ఒకవేళ (స్థితి_2 == OK)
	స్థితి_2 = dict_కూడు_string(dict, "display", menu->dname);
    ఒకవేళ (స్థితి_2 == OK && menu->actext != ఖాళీ)
	స్థితి_2 = dict_కూడు_string(dict, "accel", menu->actext);
    ఒకవేళ (స్థితి_2 == OK)
	స్థితి_2 = dict_కూడు_సంఖ్య(dict, "priority", menu->ప్రాధాన్యత);
    ఒకవేళ (స్థితి_2 == OK)
	స్థితి_2 = dict_కూడు_string(dict, "modes",
					get_menu_బహుళకం_str(menu->modes));
#నిర్వచితమైతే FEAT_TOOLBAR
    ఒకవేళ (స్థితి_2 == OK && menu->iconfile != ఖాళీ)
	స్థితి_2 = dict_కూడు_string(dict, "icon", menu->iconfile);
    ఒకవేళ (స్థితి_2 == OK && menu->iconidx >= 0)
	స్థితి_2 = dict_కూడు_సంఖ్య(dict, "iconidx", menu->iconidx);
#అయితే_ముగింపు
    ఒకవేళ (స్థితి_2 == OK)
    {
	char_u	buf[NUMBUFLEN];
	పూర్ణాంకం	buflen;

	ఒకవేళ (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	లేకపోతే
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	స్థితి_2 = dict_కూడు_string_len(dict, "shortcut", buf, buflen);
    }
    ఒకవేళ (స్థితి_2 == OK && menu->children == ఖాళీ)
    {
	పూర్ణాంకం		బిట్టు;

	// Get the first mode in which the menu is available
	కొరకు (బిట్టు = 0; బిట్టు < MENU_MODES && !((1 << బిట్టు) & modes); బిట్టు++)
	    ;
	ఒకవేళ (బిట్టు < MENU_MODES) // just in case, avoid Coverity warning
	{
	    ఒకవేళ (menu->అక్షరశ్రేణులు[బిట్టు] != ఖాళీ)
	    {
		ఒకవేళ (*menu->అక్షరశ్రేణులు[బిట్టు] == NUL)
		    స్థితి_2 = dict_కూడు_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		లేకపోతే
		{
		    char_u *tofree = ఖాళీ;

		    స్థితి_2 = dict_కూడు_string(dict, "rhs",
			tofree = str2special_save(menu->అక్షరశ్రేణులు[బిట్టు], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    ఒకవేళ (స్థితి_2 == OK)
		స్థితి_2 = dict_కూడు_bool(dict, "noremenu",
					     menu->noremap[బిట్టు] == REMAP_NONE);
	    ఒకవేళ (స్థితి_2 == OK)
		స్థితి_2 = dict_కూడు_bool(dict, "script",
					   menu->noremap[బిట్టు] == REMAP_SCRIPT);
	    ఒకవేళ (స్థితి_2 == OK)
		స్థితి_2 = dict_కూడు_bool(dict, "silent", menu->silent[బిట్టు]);
	    ఒకవేళ (స్థితి_2 == OK)
		స్థితి_2 = dict_కూడు_bool(dict, "enabled",
					  ((menu->enabled & (1 << బిట్టు)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    ఒకవేళ (స్థితి_2 == OK && menu->children != ఖాళీ)
    {
	vimmenu_T	*బిడ్డ;

	l = list_alloc();
	ఒకవేళ (l == ఖాళీ)
	    తిరిగి FAIL;

	ఒకవేళ (dict_కూడు_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    తిరిగి FAIL;
	}
	బిడ్డ = menu->children;
	అయితే (బిడ్డ)
	{
	    ఒకవేళ (!menu_is_tearoff(బిడ్డ->dname))		// skip tearoff menu
		list_append_string(l, బిడ్డ->dname, -1);
	    బిడ్డ = బిడ్డ->తదుపరి;
	}
    }

    తిరిగి స్థితి_2;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    శూన్య
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    పూర్ణాంకం		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    retdict = rettv->vval.v_dict;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    menu_name = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (menu_name == ఖాళీ)
	తిరిగి;

    // menu mode
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    లేకపోతే
	which = (char_u *)"";	    // Default is modes for "menu"
    ఒకవేళ (which == ఖాళీ)
	తిరిగి;

    modes = get_menu_cmd_modes(which, *which == '!', ఖాళీ, ఖాళీ);

    // Locate the specified menu or menu item
    menu = *get_మూలం_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    ఒకవేళ (saved_name == ఖాళీ)
	తిరిగి;
    ఒకవేళ (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	అయితే (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    అయితే (menu != ఖాళీ)
	    {
		ఒకవేళ (menu_name_equal(name, menu))
		    ఆపు;
		menu = menu->తదుపరి;
	    }
	    ఒకవేళ (menu == ఖాళీ || *p == NUL)
		ఆపు;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    ఒకవేళ (menu == ఖాళీ)		// specified menu not found
	తిరిగి;

    ఒకవేళ (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#అయితే_ముగింపు // FEAT_MENU
