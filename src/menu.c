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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
സ്ഥിതം പൂർണ്ണസംഖ്യ add_menu_path(char_u *, vimmenu_T *, പൂർണ്ണസംഖ്യ *, char_u *, പൂർണ്ണസംഖ്യ);
#അല്ലെങ്കിൽ
സ്ഥിതം പൂർണ്ണസംഖ്യ add_menu_path(char_u *, vimmenu_T *, പൂർണ്ണസംഖ്യ *, char_u *);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ menu_nable_recurse(vimmenu_T *menu, char_u *name, പൂർണ്ണസംഖ്യ modes, പൂർണ്ണസംഖ്യ enable);
സ്ഥിതം പൂർണ്ണസംഖ്യ remove_menu(vimmenu_T **, char_u *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ silent);
സ്ഥിതം ശൂന്യം free_menu(vimmenu_T **menup);
സ്ഥിതം ശൂന്യം free_menu_string(vimmenu_T *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ show_menus(char_u *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം show_menus_recursive(vimmenu_T *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
സ്ഥിതം char_u *menu_name_skip(char_u *name);
സ്ഥിതം പൂർണ്ണസംഖ്യ menu_name_equal(char_u *name, vimmenu_T *menu);
സ്ഥിതം പൂർണ്ണസംഖ്യ menu_namecmp(char_u *name, char_u *mname);
സ്ഥിതം പൂർണ്ണസംഖ്യ get_menu_cmd_modes(char_u *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ *, പൂർണ്ണസംഖ്യ *);
സ്ഥിതം char_u *popup_mode_name(char_u *name, പൂർണ്ണസംഖ്യ idx);
സ്ഥിതം char_u *menu_text(char_u *text, പൂർണ്ണസംഖ്യ *mnemonic, char_u **actext);

#എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
സ്ഥിതം ശൂന്യം gui_create_tearoffs_recurse(vimmenu_T *menu, സ്ഥിരം char_u *pname, പൂർണ്ണസംഖ്യ *pri_tab, പൂർണ്ണസംഖ്യ pri_idx);
സ്ഥിതം ശൂന്യം gui_add_tearoff(char_u *tearpath, പൂർണ്ണസംഖ്യ *pri_tab, പൂർണ്ണസംഖ്യ pri_idx);
സ്ഥിതം ശൂന്യം gui_destroy_tearoffs_recurse(vimmenu_T *menu);
സ്ഥിതം പൂർണ്ണസംഖ്യ s_tearoffs = FALSE;
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ menu_is_hidden(char_u *name);
സ്ഥിതം പൂർണ്ണസംഖ്യ menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
സ്ഥിതം പൂർണ്ണസംഖ്യ menus_locked = 0;

#എങ്കിൽ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
സ്ഥിതം char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
സ്ഥിതം char_u *menutrans_lookup(char_u *name, പൂർണ്ണസംഖ്യ len);
സ്ഥിതം ശൂന്യം menu_unescape_name(char_u	*p);
#endif

സ്ഥിതം char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
സ്ഥിതം അക്ഷരം *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
സ്ഥിതം സ്ഥിരം അക്ഷരം *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_എണ്ണം ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
menu_is_winbar(char_u *name)
{
    തിരികെ (STRNCMP(name, "WinBar", 6) == 0);
}

    പൂർണ്ണസംഖ്യ
winbar_height(win_T *wp)
{
    എങ്കിൽ (wp->w_winbar != ശൂന്യ && wp->w_winbar->children != ശൂന്യ)
	തിരികെ 1;
    തിരികെ 0;
}

    സ്ഥിതം vimmenu_T **
get_വേര്_menu(char_u *name)
{
    എങ്കിൽ (menu_is_winbar(name))
	തിരികെ &curwin->w_winbar;
    തിരികെ &വേര്_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_menus_locked(ശൂന്യം)
{
    എങ്കിൽ (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    ശൂന്യം
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    പൂർണ്ണസംഖ്യ		modes;
    char_u	*map_to;
    പൂർണ്ണസംഖ്യ		noremap;
    പൂർണ്ണസംഖ്യ		silent = FALSE;
    പൂർണ്ണസംഖ്യ		special = FALSE;
    പൂർണ്ണസംഖ്യ		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
#എങ്കിൽ defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    പൂർണ്ണസംഖ്യ		old_menu_height;
# എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    പൂർണ്ണസംഖ്യ		old_toolbar_height;
# endif
#endif
    പൂർണ്ണസംഖ്യ		pri_tab[MENUDEPTH + 1];
    പൂർണ്ണസംഖ്യ		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = ശൂന്യ;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**വേര്_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    തുടരുക;
	}
	എങ്കിൽ (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    തുടരുക;
	}
	എങ്കിൽ (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    തുടരുക;
	}
	നിർത്തുക;
    }


    // Locate an optional "icon=filename" argument.
    എങ്കിൽ (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	ആകുമ്പോൾ (*arg != NUL && *arg != ' ')
	{
	    എങ്കിൽ (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	എങ്കിൽ (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    വേണ്ടി (p = arg; *p; ++p)
	എങ്കിൽ (!VIM_ISDIGIT(*p) && *p != '.')
	    നിർത്തുക;
    എങ്കിൽ (VIM_ISWHITE(*p))
    {
	വേണ്ടി (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    എങ്കിൽ (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    എങ്കിൽ (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap->addr_എണ്ണം && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    അല്ലെങ്കിൽ
	i = 0;
    ആകുമ്പോൾ (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    എങ്കിൽ (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    എങ്കിൽ (*arg == NUL)
    {
	show_menus(arg, modes);
	തിരികെ;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    എങ്കിൽ (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	എങ്കിൽ (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    എങ്കിൽ (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		എങ്കിൽ (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((അക്ഷരം *)menu_path + 7);
		    എങ്കിൽ (menuarg.iconidx >= (പൂർണ്ണസംഖ്യ)TOOLBAR_NAME_എണ്ണം)
			menuarg.iconidx = -1;
		    അല്ലെങ്കിൽ
			menuarg.icon_builtin = TRUE;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)TOOLBAR_NAME_എണ്ണം; ++i)
		    എങ്കിൽ (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			നിർത്തുക;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    എങ്കിൽ (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	പോകുക theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    എങ്കിൽ (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	പോകുക theend;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	പോകുക theend;
    }
#എങ്കിൽ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    വേര്_menu_ptr = get_വേര്_menu(menu_path);
    എങ്കിൽ (വേര്_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    എങ്കിൽ (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	എങ്കിൽ (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	എങ്കിൽ (menu_is_popup(menu_path))
	{
	    വേണ്ടി (i = 0; i < MENU_INDEX_TIP; ++i)
		എങ്കിൽ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			menu_nable_recurse(*വേര്_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*വേര്_menu_ptr, menu_path, modes, enable);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (unmenu)
    {
	എങ്കിൽ (is_menus_locked())
	    പോകുക theend;

	/*
	 * Delete menu(s).
	 */
	എങ്കിൽ (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	എങ്കിൽ (menu_is_popup(menu_path))
	{
	    വേണ്ടി (i = 0; i < MENU_INDEX_TIP; ++i)
		എങ്കിൽ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			remove_menu(വേര്_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(വേര്_menu_ptr, menu_path, modes, FALSE);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (is_menus_locked())
	    പോകുക theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	എങ്കിൽ (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (modes & MENU_TIP_MODE)
	    map_buf = ശൂന്യ;	// Menu tips are plain text.
	അല്ലെങ്കിൽ
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ശൂന്യ);
	menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
	menuarg.iconfile = icon;
#endif
	menuarg.noremap[0] = noremap;
	menuarg.silent[0] = silent;
	add_menu_path(menu_path, &menuarg, pri_tab, map_to
#ifdef FEAT_GUI_MSWIN
		, TRUE
#endif
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	എങ്കിൽ (menu_is_popup(menu_path))
	{
	    വേണ്ടി (i = 0; i < MENU_INDEX_TIP; ++i)
		എങ്കിൽ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = ശൂന്യ;
			menuarg.iconidx = -1;
			menuarg.icon_builtin = FALSE;
#endif
			add_menu_path(p, &menuarg, pri_tab, map_to
#ifdef FEAT_GUI_MSWIN
				, TRUE
#endif
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

#എങ്കിൽ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    എങ്കിൽ (gui.in_use
	    && (gui.menu_height != old_menu_height
# എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    എങ്കിൽ (വേര്_menu_ptr == &curwin->w_winbar)
    {
	പൂർണ്ണസംഖ്യ h = winbar_height(curwin);

	എങ്കിൽ (h != curwin->w_winbar_height)
	{
	    എങ്കിൽ (h == 0)
		++curwin->w_height;
	    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_height > 0)
		--curwin->w_height;
	    curwin->w_winbar_height = h;
	}
	curwin->w_prev_height = curwin->w_height;
    }

theend:
    ;
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    പൂർണ്ണസംഖ്യ		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , പൂർണ്ണസംഖ്യ	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    പൂർണ്ണസംഖ്യ		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = ശൂന്യ;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		d;
#ifdef FEAT_GUI
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		new_idx;
#endif
    പൂർണ്ണസംഖ്യ		pri_idx = 0;
    പൂർണ്ണസംഖ്യ		old_modes = 0;
    പൂർണ്ണസംഖ്യ		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = ശൂന്യ;
#endif
    vimmenu_T	**വേര്_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    എങ്കിൽ (path_name == ശൂന്യ)
	തിരികെ FAIL;
    വേര്_menu_ptr = get_വേര്_menu(menu_path);
    menup = വേര്_menu_ptr;
    parent = ശൂന്യ;
    name = path_name;
    ആകുമ്പോൾ (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (പൂർണ്ണസംഖ്യ)STRLEN(name));
	എങ്കിൽ (map_to != ശൂന്യ)
	{
	    en_name = name;
	    name = map_to;
	}
	അല്ലെങ്കിൽ
	    en_name = ശൂന്യ;
#endif
	dname = menu_text(name, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (dname == ശൂന്യ)
	    പോകുക erret;
	എങ്കിൽ (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    പോകുക erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	ആകുമ്പോൾ (menu != ശൂന്യ)
	{
	    എങ്കിൽ (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		എങ്കിൽ (*next_name == NUL && menu->children != ശൂന്യ)
		{
		    എങ്കിൽ (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    പോകുക erret;
		}
		എങ്കിൽ (*next_name != NUL && menu->children == ശൂന്യ
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    എങ്കിൽ (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    പോകുക erret;
		}
		നിർത്തുക;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    എങ്കിൽ (parent != ശൂന്യ || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		എങ്കിൽ (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	എങ്കിൽ (menu == ശൂന്യ)
	{
	    എങ്കിൽ (*next_name == NUL && parent == ശൂന്യ)
	    {
		emsg(_(e_must_not_add_menu_ഇനങ്ങൾ_directly_to_menu_bar));
		പോകുക erret;
	    }

	    എങ്കിൽ (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		പോകുക erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    എങ്കിൽ (menu == ശൂന്യ)
		പോകുക erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    എങ്കിൽ (en_name != ശൂന്യ)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, ശൂന്യ, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
	    {
		menu->en_name = ശൂന്യ;
		menu->en_dname = ശൂന്യ;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = ശൂന്യ;
#endif
	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->next = *lower_pri;
	    *lower_pri = menu;

	    old_modes = 0;

#ifdef FEAT_TOOLBAR
	    menu->iconidx = menuarg->iconidx;
	    menu->icon_builtin = menuarg->icon_builtin;
	    എങ്കിൽ (*next_name == NUL && menuarg->iconfile != ശൂന്യ)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    എങ്കിൽ (parent != ശൂന്യ && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	അല്ലെങ്കിൽ
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    എങ്കിൽ (addtearoff)
#endif
	    {
		menu->modes |= modes;
		menu->enabled |= modes;
	    }
	}

#ifdef FEAT_GUI
	/*
	 * Add the menu item when it's used in one of the modes, but not when
	 * only a tooltip is defined.
	 */
	എങ്കിൽ ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    എങ്കിൽ (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		എങ്കിൽ (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		അല്ലെങ്കിൽ
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    എങ്കിൽ (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != ശൂന്യ
		    && menu_is_menubar(name)
#  ifdef VIMDLL
		    && (gui.in_use || gui.starting)
#  endif
		    )
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_path) + TEAR_LEN + 2);
		എങ്കിൽ (tearpath != ശൂന്യ)
		{
		    char_u  *s;
		    പൂർണ്ണസംഖ്യ	    len;

		    STRCPY(tearpath, menu_path);
		    len = (പൂർണ്ണസംഖ്യ)(next_name - path_name - 1);
		    വേണ്ടി (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			എങ്കിൽ ((*s == '\\' || *s == Ctrl_V) && s[1])
			{
			    ++len;
			    ++s;
			}
		    }
		    tearpath[len] = NUL;
		    gui_add_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
# endif
	}
#endif // FEAT_GUI

	menup = &menu->children;
	parent = menu;
	name = next_name;
	VIM_CLEAR(dname);
	എങ്കിൽ (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_തിരുകുക_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_തിരുകുക_MODE));
    എങ്കിൽ (sys_menu)
	modes &= ~old_modes;

    എങ്കിൽ (menu != ശൂന്യ && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == ശൂന്യ) ? ശൂന്യ : vim_strsave(call_data);

	// loop over all modes, may add more than one
	വേണ്ടി (i = 0; i < MENU_MODES; ++i)
	{
	    എങ്കിൽ (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		എങ്കിൽ (amenu && call_data != ശൂന്യ && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    തിരഞ്ഞെടുപ്പ് (1 << i)
		    {
			അവസ്ഥ MENU_VISUAL_MODE:
			അവസ്ഥ MENU_SELECT_MODE:
			അവസ്ഥ MENU_OP_PENDING_MODE:
			അവസ്ഥ MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    നിർത്തുക;
			അവസ്ഥ MENU_തിരുകുക_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    നിർത്തുക;
		    }
		}

		എങ്കിൽ (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    എങ്കിൽ (menu->strings[i] != ശൂന്യ)
		    {
			menu->strings[i][0] = c;
			എങ്കിൽ (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			അല്ലെങ്കിൽ
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			എങ്കിൽ (c == Ctrl_C)
			{
			    പൂർണ്ണസംഖ്യ	    len = (പൂർണ്ണസംഖ്യ)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		അല്ലെങ്കിൽ
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	എങ്കിൽ (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    തിരികെ OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    ആകുമ്പോൾ (parent != ശൂന്യ && parent->children == ശൂന്യ)
    {
	എങ്കിൽ (parent->parent == ശൂന്യ)
	    menup = വേര്_menu_ptr;
	അല്ലെങ്കിൽ
	    menup = &parent->parent->children;
	വേണ്ടി ( ; *menup != ശൂന്യ && *menup != parent; menup = &((*menup)->next))
	    ;
	എങ്കിൽ (*menup == ശൂന്യ) // safety check
	    നിർത്തുക;
	parent = parent->parent;
	free_menu(menup);
    }
    തിരികെ FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    പൂർണ്ണസംഖ്യ		modes,
    പൂർണ്ണസംഖ്യ		enable)
{
    char_u	*p;

    എങ്കിൽ (menu == ശൂന്യ)
	തിരികെ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ആകുമ്പോൾ (menu != ശൂന്യ)
    {
	എങ്കിൽ (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    എങ്കിൽ (*p != NUL)
	    {
		എങ്കിൽ (menu->children == ശൂന്യ)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    തിരികെ FAIL;
		}
		എങ്കിൽ (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
		എങ്കിൽ (enable)
		    menu->enabled |= modes;
		അല്ലെങ്കിൽ
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    എങ്കിൽ (*name != NUL && *name != '*')
		നിർത്തുക;
	}
	menu = menu->next;
    }
    എങ്കിൽ (*name != NUL && *name != '*' && menu == ശൂന്യ)
    {
	semsg(_(e_no_menu_str), name);
	തിരികെ FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    തിരികെ OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    പൂർണ്ണസംഖ്യ		modes,
    പൂർണ്ണസംഖ്യ		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    എങ്കിൽ (*menup == ശൂന്യ)
	തിരികെ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ആകുമ്പോൾ ((menu = *menup) != ശൂന്യ)
    {
	എങ്കിൽ (*name == NUL || menu_name_equal(name, menu))
	{
	    എങ്കിൽ (*p != NUL && menu->children == ശൂന്യ)
	    {
		എങ്കിൽ (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ ((menu->modes & modes) != 0x0)
	    {
#എങ്കിൽ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		എങ്കിൽ (*p == NUL && modes == MENU_ALL_MODES)
		{
		    എങ്കിൽ (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		എങ്കിൽ (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*name != NUL)
	    {
		എങ്കിൽ (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		തിരികെ FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    എങ്കിൽ (*name != NUL)
		നിർത്തുക;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    എങ്കിൽ (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    എങ്കിൽ ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    അല്ലെങ്കിൽ
		menup = &menu->next;
	}
	അല്ലെങ്കിൽ
	    menup = &menu->next;
    }
    എങ്കിൽ (*name != NUL)
    {
	എങ്കിൽ (menu == ശൂന്യ)
	{
	    എങ്കിൽ (!silent)
		semsg(_(e_no_menu_str), name);
	    തിരികെ FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#എങ്കിൽ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	എങ്കിൽ ((s_tearoffs) && (menu->children != ശൂന്യ)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	അല്ലെങ്കിൽ
#endif
	    child = menu->children;
	വേണ്ടി ( ; child != ശൂന്യ; child = child->next)
	    menu->modes |= child->modes;
	എങ്കിൽ (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    എങ്കിൽ (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	എങ്കിൽ ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#എങ്കിൽ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    എങ്കിൽ (s_tearoffs && menu->children != ശൂന്യ) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    തിരികെ OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    ശൂന്യം
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_ഇനങ്ങൾ);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    സ്ഥിതം ശൂന്യം
free_menu(vimmenu_T **menup)
{
    പൂർണ്ണസംഖ്യ		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    എങ്കിൽ (gui.in_use)
	gui_mch_destroy_menu(menu);
#endif

    // Don't change *menup until after calling gui_mch_destroy_menu(). The
    // MacOS code needs the original structure to properly delete the menu.
    *menup = menu->next;
    vim_free(menu->name);
    vim_free(menu->dname);
#ifdef FEAT_MULTI_LANG
    vim_free(menu->en_name);
    vim_free(menu->en_dname);
#endif
    vim_free(menu->actext);
#ifdef FEAT_TOOLBAR
    vim_free(menu->iconfile);
# ifdef FEAT_GUI_MOTIF
    vim_free(menu->xpm_fname);
# endif
#endif
    വേണ്ടി (i = 0; i < MENU_MODES; i++)
	free_menu_string(menu, i);
    vim_free(menu);

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif
}

/*
 * Free the menu->string with the given index.
 */
    സ്ഥിതം ശൂന്യം
free_menu_string(vimmenu_T *menu, പൂർണ്ണസംഖ്യ idx)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < MENU_MODES; i++)
	എങ്കിൽ (menu->strings[i] == menu->strings[idx])
	    എണ്ണം++;
    എങ്കിൽ (എണ്ണം == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = ശൂന്യ;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
show_menus(char_u *path_name, പൂർണ്ണസംഖ്യ modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = ശൂന്യ;

    name = path_name = vim_strsave(path_name);
    എങ്കിൽ (path_name == ശൂന്യ)
	തിരികെ FAIL;
    menu = *get_വേര്_menu(path_name);

    // First, find the (sub)menu with the given name
    ആകുമ്പോൾ (*name)
    {
	p = menu_name_skip(name);
	ആകുമ്പോൾ (menu != ശൂന്യ)
	{
	    എങ്കിൽ (menu_name_equal(name, menu))
	    {
		// Found menu
		എങ്കിൽ (*p != NUL && menu->children == ശൂന്യ)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ എങ്കിൽ ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    തിരികെ FAIL;
		}
		നിർത്തുക;
	    }
	    menu = menu->next;
	}
	എങ്കിൽ (menu == ശൂന്യ)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    തിരികെ FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    സന്ദേശം_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    തിരികെ OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    സ്ഥിതം ശൂന്യം
show_menus_recursive(vimmenu_T *menu, പൂർണ്ണസംഖ്യ modes, പൂർണ്ണസംഖ്യ depth)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		bit;

    എങ്കിൽ (menu != ശൂന്യ && (menu->modes & modes) == 0x0)
	തിരികെ;

    എങ്കിൽ (menu != ശൂന്യ)
    {
	സന്ദേശം_putchar('\n');
	എങ്കിൽ (got_int)		// "q" hit for "--more--"
	    തിരികെ;
	വേണ്ടി (i = 0; i < depth; i++)
	    സന്ദേശം_puts("  ");
	എങ്കിൽ (menu->priority)
	{
	    സന്ദേശം_outnum((long)menu->priority);
	    സന്ദേശം_puts(" ");
	}
				// Same highlighting as for directories!?
	സന്ദേശം_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    എങ്കിൽ (menu != ശൂന്യ && menu->children == ശൂന്യ)
    {
	വേണ്ടി (bit = 0; bit < MENU_MODES; bit++)
	    എങ്കിൽ ((menu->modes & modes & (1 << bit)) != 0)
	    {
		സന്ദേശം_putchar('\n');
		എങ്കിൽ (got_int)		// "q" hit for "--more--"
		    തിരികെ;
		വേണ്ടി (i = 0; i < depth + 2; i++)
		    സന്ദേശം_puts("  ");
		സന്ദേശം_puts(menu_mode_chars[bit]);
		എങ്കിൽ (menu->noremap[bit] == REMAP_NONE)
		    സന്ദേശം_putchar('*');
		അല്ലെങ്കിൽ എങ്കിൽ (menu->noremap[bit] == REMAP_SCRIPT)
		    സന്ദേശം_putchar('&');
		അല്ലെങ്കിൽ
		    സന്ദേശം_putchar(' ');
		എങ്കിൽ (menu->silent[bit])
		    സന്ദേശം_putchar('s');
		അല്ലെങ്കിൽ
		    സന്ദേശം_putchar(' ');
		എങ്കിൽ ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    സന്ദേശം_putchar('-');
		അല്ലെങ്കിൽ
		    സന്ദേശം_putchar(' ');
		സന്ദേശം_puts(" ");
		എങ്കിൽ (*menu->strings[bit] == NUL)
		    സന്ദേശം_puts_attr("<Nop>", HL_ATTR(HLF_8));
		അല്ലെങ്കിൽ
		    സന്ദേശം_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (menu == ശൂന്യ)
	{
	    menu = വേര്_menu;
	    depth--;
	}
	അല്ലെങ്കിൽ
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	വേണ്ടി (; menu != ശൂന്യ && !got_int; menu = menu->next)
	    എങ്കിൽ (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
സ്ഥിതം vimmenu_T	*expand_menu = ശൂന്യ;
സ്ഥിതം vimmenu_T	*expand_menu_alt = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ		expand_modes = 0x0;
സ്ഥിതം പൂർണ്ണസംഖ്യ		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = ശൂന്യ;
    char_u	*name;
    പൂർണ്ണസംഖ്യ		unmenu;
    vimmenu_T	*menu;
    പൂർണ്ണസംഖ്യ		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    വേണ്ടി (p = arg; *p; ++p)
	എങ്കിൽ (!VIM_ISDIGIT(*p) && *p != '.')
	    നിർത്തുക;

    എങ്കിൽ (!VIM_ISWHITE(*p))
    {
	എങ്കിൽ (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	അല്ലെങ്കിൽ
	    p = arg;
    }

    ആകുമ്പോൾ (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    വേണ്ടി (; *p && !VIM_ISWHITE(*p); ++p)
    {
	എങ്കിൽ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    എങ്കിൽ (expand_menus && VIM_ISWHITE(*p))
	തിരികെ ശൂന്യ;	// TODO: check for next command?
    എങ്കിൽ (*p == NUL)		// Complete the menu name
    {
	പൂർണ്ണസംഖ്യ try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, ശൂന്യ, &unmenu);
	എങ്കിൽ (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = വേര്_menu;
	എങ്കിൽ (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    എങ്കിൽ (path_name == ശൂന്യ)
		തിരികെ ശൂന്യ;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	ആകുമ്പോൾ (name != ശൂന്യ && *name)
	{
	    p = menu_name_skip(name);
	    ആകുമ്പോൾ (menu != ശൂന്യ)
	    {
		എങ്കിൽ (menu_name_equal(name, menu))
		{
		    // Found menu
		    എങ്കിൽ ((*p != NUL && menu->children == ശൂന്യ)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			തിരികെ ശൂന്യ;
		    }
		    നിർത്തുക;
		}
		menu = menu->next;
		എങ്കിൽ (menu == ശൂന്യ && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    എങ്കിൽ (menu == ശൂന്യ)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		തിരികെ ശൂന്യ;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	എങ്കിൽ (expand_menu == വേര്_menu)
	    expand_menu_alt = curwin->w_winbar;
	അല്ലെങ്കിൽ
	    expand_menu_alt = ശൂന്യ;
    }
    അല്ലെങ്കിൽ			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    തിരികെ ശൂന്യ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം vimmenu_T	*menu = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    സ്ഥിതം  പൂർണ്ണസംഖ്യ		should_advance = FALSE;
#endif

    എങ്കിൽ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    ആകുമ്പോൾ (menu != ശൂന്യ && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == ശൂന്യ))
    {
	menu = menu->next;
	എങ്കിൽ (menu == ശൂന്യ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    എങ്കിൽ (menu == ശൂന്യ)	    // at end of linked list
	തിരികെ ശൂന്യ;

    എങ്കിൽ (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	എങ്കിൽ (should_advance)
	    str = menu->en_dname;
	അല്ലെങ്കിൽ
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    എങ്കിൽ (menu->en_dname == ശൂന്യ)
		should_advance = TRUE;
	}
#endif
    അല്ലെങ്കിൽ
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    എങ്കിൽ (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	എങ്കിൽ (menu == ശൂന്യ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    തിരികെ str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം vimmenu_T	*menu = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    സ്ഥിതം char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    സ്ഥിതം  പൂർണ്ണസംഖ്യ		should_advance = FALSE;
#endif

    എങ്കിൽ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    ആകുമ്പോൾ (menu != ശൂന്യ
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	എങ്കിൽ (menu == ശൂന്യ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    എങ്കിൽ (menu == ശൂന്യ)	    // at end of linked list
	തിരികെ ശൂന്യ;

    എങ്കിൽ (menu->modes & expand_modes)
    {
	എങ്കിൽ (menu->children != ശൂന്യ)
	{
#ifdef FEAT_MULTI_LANG
	    എങ്കിൽ (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    അല്ലെങ്കിൽ
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		എങ്കിൽ (menu->en_dname == ശൂന്യ)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	അല്ലെങ്കിൽ
#ifdef FEAT_MULTI_LANG
	{
	    എങ്കിൽ (should_advance)
		str = menu->en_dname;
	    അല്ലെങ്കിൽ
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		എങ്കിൽ (menu->en_dname == ശൂന്യ)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    അല്ലെങ്കിൽ
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    എങ്കിൽ (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	എങ്കിൽ (menu == ശൂന്യ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    തിരികെ str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    സ്ഥിതം char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    വേണ്ടി (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	}
    }
    എങ്കിൽ (*p)
	*p++ = NUL;
    തിരികെ p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    എങ്കിൽ (menu->en_name != ശൂന്യ
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	തിരികെ TRUE;
#endif
    തിരികെ menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
menu_namecmp(char_u *name, char_u *mname)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	എങ്കിൽ (name[i] != mname[i])
	    നിർത്തുക;
    തിരികെ ((name[i] == NUL || name[i] == TAB)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_menu_cmd_modes(
    char_u  *cmd,
    പൂർണ്ണസംഖ്യ	    forceit,	    // Was there a "!" after the command?
    പൂർണ്ണസംഖ്യ	    *noremap,
    പൂർണ്ണസംഖ്യ	    *unmenu)
{
    പൂർണ്ണസംഖ്യ	    modes;

    തിരഞ്ഞെടുപ്പ് (*cmd++)
    {
	അവസ്ഥ 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    നിർത്തുക;
	അവസ്ഥ 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    നിർത്തുക;
	അവസ്ഥ 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    നിർത്തുക;
	അവസ്ഥ 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    നിർത്തുക;
	അവസ്ഥ 'i':			// imenu
	    modes = MENU_തിരുകുക_MODE;
	    നിർത്തുക;
	അവസ്ഥ 't':
	    എങ്കിൽ (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		നിർത്തുക;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    നിർത്തുക;
	അവസ്ഥ 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    നിർത്തുക;
	അവസ്ഥ 'a':			// amenu
	    modes = MENU_തിരുകുക_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    നിർത്തുക;
	അവസ്ഥ 'n':
	    എങ്കിൽ (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		നിർത്തുക;
	    }
	    // FALLTHROUGH
	സ്ഥിരസ്ഥിതി:
	    --cmd;
	    എങ്കിൽ (forceit)		// menu!!
		modes = MENU_തിരുകുക_MODE | MENU_CMDLINE_MODE;
	    അല്ലെങ്കിൽ			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    എങ്കിൽ (noremap != ശൂന്യ)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    എങ്കിൽ (unmenu != ശൂന്യ)
	*unmenu = (*cmd == 'u');
    തിരികെ modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    സ്ഥിതം char_u *
get_menu_mode_str(പൂർണ്ണസംഖ്യ modes)
{
    എങ്കിൽ ((modes & (MENU_തിരുകുക_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_തിരുകുക_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	തിരികെ (char_u *)"a";
    എങ്കിൽ ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	തിരികെ (char_u *)" ";
    എങ്കിൽ ((modes & (MENU_തിരുകുക_MODE | MENU_CMDLINE_MODE))
	    == (MENU_തിരുകുക_MODE | MENU_CMDLINE_MODE))
	തിരികെ (char_u *)"!";
    എങ്കിൽ ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	തിരികെ (char_u *)"v";
    എങ്കിൽ (modes & MENU_VISUAL_MODE)
	തിരികെ (char_u *)"x";
    എങ്കിൽ (modes & MENU_SELECT_MODE)
	തിരികെ (char_u *)"s";
    എങ്കിൽ (modes & MENU_OP_PENDING_MODE)
	തിരികെ (char_u *)"o";
    എങ്കിൽ (modes & MENU_തിരുകുക_MODE)
	തിരികെ (char_u *)"i";
    എങ്കിൽ (modes & MENU_TERMINAL_MODE)
	തിരികെ (char_u *)"tl";
    എങ്കിൽ (modes & MENU_CMDLINE_MODE)
	തിരികെ (char_u *)"c";
    എങ്കിൽ (modes & MENU_NORMAL_MODE)
	തിരികെ (char_u *)"n";
    എങ്കിൽ (modes & MENU_TIP_MODE)
	തിരികെ (char_u *)"t";

    തിരികെ (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    സ്ഥിതം char_u *
popup_mode_name(char_u *name, പൂർണ്ണസംഖ്യ idx)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len = (പൂർണ്ണസംഖ്യ)STRLEN(name);
    അക്ഷരം	*mode_chars = menu_mode_chars[idx];
    പൂർണ്ണസംഖ്യ		mode_chars_len = (പൂർണ്ണസംഖ്യ)strlen(mode_chars);
    പൂർണ്ണസംഖ്യ		i;

    p = vim_strnsave(name, len + mode_chars_len);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    വേണ്ടി (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    തിരികെ p;
}

#എങ്കിൽ defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    പൂർണ്ണസംഖ്യ
get_menu_index(vimmenu_T *menu, പൂർണ്ണസംഖ്യ state)
{
    പൂർണ്ണസംഖ്യ		idx;

    എങ്കിൽ ((state & MODE_തിരുകുക))
	idx = MENU_INDEX_തിരുകുക;
    അല്ലെങ്കിൽ എങ്കിൽ (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    അല്ലെങ്കിൽ എങ്കിൽ (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active)
    {
	എങ്കിൽ (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	അല്ലെങ്കിൽ
	    idx = MENU_INDEX_VISUAL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    അല്ലെങ്കിൽ എങ്കിൽ (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    അല്ലെങ്കിൽ എങ്കിൽ ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    അല്ലെങ്കിൽ
	idx = MENU_INDEX_INVALID;

    എങ്കിൽ (idx != MENU_INDEX_INVALID && menu->strings[idx] == ശൂന്യ)
	idx = MENU_INDEX_INVALID;
    തിരികെ idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    സ്ഥിതം char_u *
menu_text(char_u *str, പൂർണ്ണസംഖ്യ *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    എങ്കിൽ (p != ശൂന്യ)
    {
	എങ്കിൽ (actext != ശൂന്യ)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    അല്ലെങ്കിൽ
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    വേണ്ടി (p = text; p != ശൂന്യ; )
    {
	p = vim_strchr(p, '&');
	എങ്കിൽ (p != ശൂന്യ)
	{
	    എങ്കിൽ (p[1] == NUL)	    // trailing "&"
		നിർത്തുക;
	    എങ്കിൽ (mnemonic != ശൂന്യ && p[1] != '&')
#എങ്കിൽ !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#അല്ലെങ്കിൽ
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		അക്ഷരം c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    തിരികെ text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    പൂർണ്ണസംഖ്യ
menu_is_menubar(char_u *name)
{
    തിരികെ (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    പൂർണ്ണസംഖ്യ
menu_is_popup(char_u *name)
{
    തിരികെ (STRNCMP(name, "PopUp", 5) == 0);
}

#എങ്കിൽ defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    പൂർണ്ണസംഖ്യ
menu_is_child_of_popup(vimmenu_T *menu)
{
    ആകുമ്പോൾ (menu->parent != ശൂന്യ)
	menu = menu->parent;
    തിരികെ menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    പൂർണ്ണസംഖ്യ
menu_is_toolbar(char_u *name)
{
    തിരികെ (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    പൂർണ്ണസംഖ്യ
menu_is_separator(char_u *name)
{
    തിരികെ (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
menu_is_hidden(char_u *name)
{
    തിരികെ (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    തിരികെ (STRCMP(name, TEAR_STRING) == 0);
#അല്ലെങ്കിൽ
    തിരികെ FALSE;
#endif
}

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_menu_mode(ശൂന്യം)
{
# ifdef FEAT_TERMINAL
    എങ്കിൽ (term_use_loop())
	തിരികെ MENU_INDEX_TERMINAL;
# endif
    എങ്കിൽ (VIsual_active)
    {
	എങ്കിൽ (VIsual_select)
	    തിരികെ MENU_INDEX_SELECT;
	തിരികെ MENU_INDEX_VISUAL;
    }
    എങ്കിൽ (State & MODE_തിരുകുക)
	തിരികെ MENU_INDEX_തിരുകുക;
    എങ്കിൽ ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	തിരികെ MENU_INDEX_CMDLINE;
    എങ്കിൽ (finish_op)
	തിരികെ MENU_INDEX_OP_PENDING;
    എങ്കിൽ (State & MODE_NORMAL)
	തിരികെ MENU_INDEX_NORMAL;
    എങ്കിൽ (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	തിരികെ MENU_INDEX_തിരുകുക;
    തിരികെ MENU_INDEX_INVALID;
}

    പൂർണ്ണസംഖ്യ
get_menu_mode_flag(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ mode = get_menu_mode();

    എങ്കിൽ (mode == MENU_INDEX_INVALID)
	തിരികെ 0;
    തിരികെ 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    ശൂന്യം
show_popupmenu(ശൂന്യം)
{
    vimmenu_T	*menu;
    പൂർണ്ണസംഖ്യ		menu_mode;
    അക്ഷരം*	mode;
    പൂർണ്ണസംഖ്യ		mode_len;

    menu_mode = get_menu_mode();
    എങ്കിൽ (menu_mode == MENU_INDEX_INVALID)
	തിരികെ;
    mode = menu_mode_chars[menu_mode];
    mode_len = (പൂർണ്ണസംഖ്യ)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, ശൂന്യ, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	എങ്കിൽ (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    നിർത്തുക;

    // Only show a popup when it is defined and has entries
    എങ്കിൽ (menu == ശൂന്യ || menu->children == ശൂന്യ)
	തിരികെ;

# എങ്കിൽ defined(FEAT_GUI)
    എങ്കിൽ (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    അല്ലെങ്കിൽ
# endif
# എങ്കിൽ defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#എങ്കിൽ defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    പൂർണ്ണസംഖ്യ
check_menu_pointer(vimmenu_T *വേര്, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    വേണ്ടി (p = വേര്; p != ശൂന്യ; p = p->next)
	എങ്കിൽ (p == menu_to_check
		|| (p->children != ശൂന്യ
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    തിരികെ OK;
    തിരികെ FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    ശൂന്യം
gui_create_initial_menus(vimmenu_T *menu)
{
    പൂർണ്ണസംഖ്യ		idx = 0;

    ആകുമ്പോൾ (menu != ശൂന്യ)
    {
	// Don't add a menu when only a tip was defined.
	എങ്കിൽ (menu->modes & MENU_ALL_MODES)
	{
	    എങ്കിൽ (menu->children != ശൂന്യ)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    അല്ലെങ്കിൽ
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    സ്ഥിതം ശൂന്യം
gui_update_menus_recurse(vimmenu_T *menu, പൂർണ്ണസംഖ്യ mode)
{
    പൂർണ്ണസംഖ്യ		grey;

    ആകുമ്പോൾ (menu)
    {
	എങ്കിൽ ((menu->modes & menu->enabled & mode)
# എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	അല്ലെങ്കിൽ
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	എങ്കിൽ (vim_strchr(p_go, GO_GREY) != ശൂന്യ || menu->parent == ശൂന്യ
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	അല്ലെങ്കിൽ
	    gui_mch_menu_hidden(menu, grey);
	gui_update_menus_recurse(menu->children, mode);
	menu = menu->next;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    ശൂന്യം
gui_update_menus(പൂർണ്ണസംഖ്യ modes)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    prev_mode = -1;
    പൂർണ്ണസംഖ്യ		    mode = 0;

    എങ്കിൽ (modes != 0x0)
	mode = modes;
    അല്ലെങ്കിൽ
	mode = get_menu_mode_flag();

    എങ്കിൽ (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(വേര്_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    പൂർണ്ണസംഖ്യ
gui_is_menu_shortcut(പൂർണ്ണസംഖ്യ താക്കോൽ)
{
    vimmenu_T	*menu;

    എങ്കിൽ (താക്കോൽ < 256)
	താക്കോൽ = TOLOWER_LOC(താക്കോൽ);
    FOR_ALL_MENUS(menu)
	എങ്കിൽ (menu->mnemonic == താക്കോൽ
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == താക്കോൽ))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}
# endif
#endif // FEAT_GUI

#എങ്കിൽ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    ശൂന്യം
gui_mch_toggle_tearoffs(പൂർണ്ണസംഖ്യ enable)
{
    പൂർണ്ണസംഖ്യ		pri_tab[MENUDEPTH + 1];
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (enable)
    {
	വേണ്ടി (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(വേര്_menu, (char_u *)"", pri_tab, 0);
    }
    അല്ലെങ്കിൽ
	gui_destroy_tearoffs_recurse(വേര്_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    സ്ഥിതം ശൂന്യം
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    സ്ഥിരം char_u	*pname,
    പൂർണ്ണസംഖ്യ			*pri_tab,
    പൂർണ്ണസംഖ്യ			pri_idx)
{
    char_u	*newpname = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*s;
    char_u	*d;

    എങ്കിൽ (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    ആകുമ്പോൾ (menu != ശൂന്യ)
    {
	എങ്കിൽ (menu->children != ശൂന്യ && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(pname) + (പൂർണ്ണസംഖ്യ)STRLEN(menu->name);
	    വേണ്ടി (s = menu->name; *s; ++s)
		എങ്കിൽ (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    എങ്കിൽ (newpname != ശൂന്യ)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		വേണ്ടി (s = menu->name; *s; ++s)
		{
		    എങ്കിൽ (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		എങ്കിൽ (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_add_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			// remove TEAR_STRING
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->next;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    സ്ഥിതം ശൂന്യം
gui_add_tearoff(char_u *tearpath, പൂർണ്ണസംഖ്യ *pri_tab, പൂർണ്ണസംഖ്യ pri_idx)
{
    char_u	*tbuf;
    പൂർണ്ണസംഖ്യ		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned പൂർണ്ണസംഖ്യ)STRLEN(tearpath));
    എങ്കിൽ (tbuf == ശൂന്യ)
	തിരികെ;

    tbuf[0] = K_SPECIAL;
    tbuf[1] = K_SECOND(K_TEAROFF);
    tbuf[2] = K_THIRD(K_TEAROFF);
    STRCPY(tbuf + 3, tearpath);
    STRCAT(tbuf + 3, "\r");

    STRCAT(tearpath, ".");
    STRCAT(tearpath, TEAR_STRING);

    // Priority of tear-off is always 1
    t = pri_tab[pri_idx + 1];
    pri_tab[pri_idx + 1] = 1;

# ifdef FEAT_TOOLBAR
    menuarg.iconfile = ശൂന്യ;
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
# endif
    menuarg.noremap[0] = REMAP_NONE;
    menuarg.silent[0] = TRUE;

    menuarg.modes = MENU_ALL_MODES;
    add_menu_path(tearpath, &menuarg, pri_tab, tbuf, FALSE);

    menuarg.modes = MENU_TIP_MODE;
    add_menu_path(tearpath, &menuarg, pri_tab,
	    (char_u *)_("Tear off this menu"), FALSE);

    pri_tab[pri_idx + 1] = t;
    vim_free(tbuf);
}

/*
 * Recursively destroy tearoff items
 */
    സ്ഥിതം ശൂന്യം
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    ആകുമ്പോൾ (menu)
    {
	എങ്കിൽ (menu->children)
	{
	    // check if tearoff exists
	    എങ്കിൽ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    എങ്കിൽ (menu->children != ശൂന്യ) // if not the last one
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->next;
    }
}

#endif // FEAT_GUI_MSWIN && FEAT_TEAROFF

/*
 * Execute "menu".  Use by ":emenu" and the window toolbar.
 * "eap" is NULL for the window toolbar.
 * "mode_idx" specifies a MENU_INDEX_ value, use MENU_INDEX_INVALID to depend
 * on the current state.
 */
    ശൂന്യം
execute_menu(exarg_T *eap, vimmenu_T *menu, പൂർണ്ണസംഖ്യ mode_idx)
{
    പൂർണ്ണസംഖ്യ		idx = mode_idx;

    എങ്കിൽ (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	എങ്കിൽ (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_തിരുകുക;
	}
#ifdef FEAT_TERMINAL
	അല്ലെങ്കിൽ എങ്കിൽ (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eap != ശൂന്യ && eap->addr_എണ്ണം)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    എങ്കിൽ ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Set it up for line-wise visual mode
		VIsual_mode = 'V';
		curwin->w_cursor.lnum = eap->line1;
		curwin->w_cursor.col = 1;
		tpos.lnum = eap->line2;
		tpos.col = MAXCOL;
		tpos.coladd = 0;
	    }

	    // Activate visual mode
	    VIsual_active = TRUE;
	    VIsual_reselect = TRUE;
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;

	    check_cursor();

	    // Adjust the cursor to make sure it is in the correct pos
	    // for exclusive mode
	    എങ്കിൽ (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    എങ്കിൽ (idx == MENU_INDEX_INVALID || eap == ശൂന്യ)
	idx = MENU_INDEX_NORMAL;

    എങ്കിൽ (menu->strings[idx] != ശൂന്യ && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	എങ്കിൽ (eap == ശൂന്യ || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    എങ്കിൽ (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	അല്ലെങ്കിൽ
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap != ശൂന്യ)
    {
	char_u	*mode;

	തിരഞ്ഞെടുപ്പ് (idx)
	{
	    അവസ്ഥ MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		നിർത്തുക;
	    അവസ്ഥ MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		നിർത്തുക;
	    അവസ്ഥ MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		നിർത്തുക;
	    അവസ്ഥ MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		നിർത്തുക;
	    അവസ്ഥ MENU_INDEX_തിരുകുക:
		mode = (char_u *)"Insert";
		നിർത്തുക;
	    അവസ്ഥ MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		നിർത്തുക;
	    // case MENU_INDEX_TIP: cannot happen
	    സ്ഥിരസ്ഥിതി:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    സ്ഥിതം vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    എങ്കിൽ (saved_name == ശൂന്യ)
	തിരികെ ശൂന്യ;

    menu = *get_വേര്_menu(saved_name);
    name = saved_name;
    ആകുമ്പോൾ (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	ആകുമ്പോൾ (menu != ശൂന്യ)
	{
	    എങ്കിൽ (menu_name_equal(name, menu))
	    {
		എങ്കിൽ (*p == NUL && menu->children != ശൂന്യ)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = ശൂന്യ;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (*p != NUL && menu->children == ശൂന്യ)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ശൂന്യ;
		}
		നിർത്തുക;
	    }
	    menu = menu->next;
	}
	എങ്കിൽ (menu == ശൂന്യ || *p == NUL)
	    നിർത്തുക;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    എങ്കിൽ (menu == ശൂന്യ)
    {
	എങ്കിൽ (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	തിരികെ ശൂന്യ;
    }

    തിരികെ menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    ശൂന്യം
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    പൂർണ്ണസംഖ്യ		mode_idx = MENU_INDEX_INVALID;

    എങ്കിൽ (arg[0] && VIM_ISWHITE(arg[1]))
    {
	തിരഞ്ഞെടുപ്പ് (arg[0])
	{
	    അവസ്ഥ 'n': mode_idx = MENU_INDEX_NORMAL; നിർത്തുക;
	    അവസ്ഥ 'v': mode_idx = MENU_INDEX_VISUAL; നിർത്തുക;
	    അവസ്ഥ 's': mode_idx = MENU_INDEX_SELECT; നിർത്തുക;
	    അവസ്ഥ 'o': mode_idx = MENU_INDEX_OP_PENDING; നിർത്തുക;
	    അവസ്ഥ 't': mode_idx = MENU_INDEX_TERMINAL; നിർത്തുക;
	    അവസ്ഥ 'i': mode_idx = MENU_INDEX_തിരുകുക; നിർത്തുക;
	    അവസ്ഥ 'c': mode_idx = MENU_INDEX_CMDLINE; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: semsg(_(e_invalid_argument_str), arg);
		     തിരികെ;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    എങ്കിൽ (menu == ശൂന്യ)
	തിരികെ;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    ശൂന്യം
winbar_click(win_T *wp, പൂർണ്ണസംഖ്യ col)
{
    പൂർണ്ണസംഖ്യ		idx;

    എങ്കിൽ (wp->w_winbar_ഇനങ്ങൾ == ശൂന്യ)
	തിരികെ;
    വേണ്ടി (idx = 0; wp->w_winbar_ഇനങ്ങൾ[idx].wb_menu != ശൂന്യ; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_ഇനങ്ങൾ[idx];

	എങ്കിൽ (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = ശൂന്യ;
	    pos_T   save_visual = VIsual;
	    പൂർണ്ണസംഖ്യ	    save_visual_active = VIsual_active;
	    പൂർണ്ണസംഖ്യ	    save_visual_select = VIsual_select;
	    പൂർണ്ണസംഖ്യ	    save_visual_reselect = VIsual_reselect;
	    പൂർണ്ണസംഖ്യ	    save_visual_mode = VIsual_mode;

	    എങ്കിൽ (wp != curwin)
	    {
		// Clicking in the window toolbar of a not-current window.
		// Make that window the current one and save Visual mode.
		save_curwin = curwin;
		VIsual_active = FALSE;
		curwin = wp;
		curbuf = curwin->w_buffer;
		check_cursor();
	    }

	    // Note: the command might close the current window.
	    execute_menu(ശൂന്യ, item->wb_menu, -1);

	    എങ്കിൽ (save_curwin != ശൂന്യ && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    എങ്കിൽ (!win_valid(wp))
		നിർത്തുക;
	}
    }
}

#എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = ശൂന്യ;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_വേര്_menu(path_name);

    saved_name = vim_strsave(path_name);
    എങ്കിൽ (saved_name == ശൂന്യ)
	തിരികെ ശൂന്യ;

    name = saved_name;
    ആകുമ്പോൾ (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	ആകുമ്പോൾ (menu != ശൂന്യ)
	{
	    എങ്കിൽ (menu_name_equal(name, menu))
	    {
		എങ്കിൽ (menu->children == ശൂന്യ)
		{
		    // found a menu item instead of a sub-menu
		    എങ്കിൽ (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    അല്ലെങ്കിൽ
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ശൂന്യ;
		    പോകുക theend;
		}
		എങ്കിൽ (*p == NUL)	    // found a full match
		    പോകുക theend;
		നിർത്തുക;
	    }
	    menu = menu->next;
	}
	എങ്കിൽ (menu == ശൂന്യ)	// didn't find it
	    നിർത്തുക;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    എങ്കിൽ (menu == ശൂന്യ)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    തിരികെ menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

തരനിർവചനം ഘടന
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

സ്ഥിതം garray_T menutrans_ga = {0, 0, 0, 0, ശൂന്യ};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    ശൂന്യം
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    പൂർണ്ണസംഖ്യ			i;
    char_u		*from, *from_noamp, *to;

    എങ്കിൽ (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, വലുപ്പം(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    എങ്കിൽ (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	വേണ്ടി (i = 0; i < menutrans_ga.ga_len; ++i)
	{
	    vim_free(tp[i].from);
	    vim_free(tp[i].from_noamp);
	    vim_free(tp[i].to);
	}
	ga_clear(&menutrans_ga);
# ifdef FEAT_EVAL
	// Delete all "menutrans_" global variables.
	del_menutrans_vars();
# endif
    }
    അല്ലെങ്കിൽ
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	എങ്കിൽ (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		എങ്കിൽ (from != ശൂന്യ)
		{
		    from_noamp = menu_text(from, ശൂന്യ, ശൂന്യ);
		    to = vim_strnsave(to, arg - to);
		    എങ്കിൽ (from_noamp != ശൂന്യ && to != ശൂന്യ)
		    {
			menu_translate_tab_and_shift(from);
			menu_translate_tab_and_shift(to);
			menu_unescape_name(from);
			menu_unescape_name(to);
			tp[menutrans_ga.ga_len].from = from;
			tp[menutrans_ga.ga_len].from_noamp = from_noamp;
			tp[menutrans_ga.ga_len].to = to;
			++menutrans_ga.ga_len;
		    }
		    അല്ലെങ്കിൽ
		    {
			vim_free(from);
			vim_free(from_noamp);
			vim_free(to);
		    }
		}
	    }
	}
    }
#endif
}

#എങ്കിൽ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    സ്ഥിതം char_u *
menu_skip_part(char_u *p)
{
    ആകുമ്പോൾ (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	എങ്കിൽ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    തിരികെ p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    സ്ഥിതം char_u *
menutrans_lookup(char_u *name, പൂർണ്ണസംഖ്യ len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    പൂർണ്ണസംഖ്യ			i;
    char_u		*dname;

    വേണ്ടി (i = 0; i < menutrans_ga.ga_len; ++i)
	എങ്കിൽ (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    തിരികെ tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, ശൂന്യ, ശൂന്യ);
    name[len] = i;
    എങ്കിൽ (dname == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (i = 0; i < menutrans_ga.ga_len; ++i)
	എങ്കിൽ (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    തിരികെ tp[i].to;
	}
    vim_free(dname);

    തിരികെ ശൂന്യ;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    സ്ഥിതം ശൂന്യം
menu_unescape_name(char_u *name)
{
    char_u  *p;

    വേണ്ടി (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	എങ്കിൽ (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    സ്ഥിതം char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    ആകുമ്പോൾ (*arg && !VIM_ISWHITE(*arg))
    {
	എങ്കിൽ ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    എങ്കിൽ (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    തിരികെ arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, പൂർണ്ണസംഖ്യ modes, dict_T *dict)
{
    പൂർണ്ണസംഖ്യ		status;
    list_T	*l;

    എങ്കിൽ (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	എങ്കിൽ (l == ശൂന്യ)
	    തിരികെ FAIL;

	എങ്കിൽ (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    തിരികെ FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	വേണ്ടി (topmenu = menu; topmenu != ശൂന്യ; topmenu = topmenu->next)
	    എങ്കിൽ (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	തിരികെ OK;
    }

    എങ്കിൽ (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	തിരികെ OK;

    status = dict_add_string(dict, "name", menu->name);
    എങ്കിൽ (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    എങ്കിൽ (status == OK && menu->actext != ശൂന്യ)
	status = dict_add_string(dict, "accel", menu->actext);
    എങ്കിൽ (status == OK)
	status = dict_add_സംഖ്യ(dict, "priority", menu->priority);
    എങ്കിൽ (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    എങ്കിൽ (status == OK && menu->iconfile != ശൂന്യ)
	status = dict_add_string(dict, "icon", menu->iconfile);
    എങ്കിൽ (status == OK && menu->iconidx >= 0)
	status = dict_add_സംഖ്യ(dict, "iconidx", menu->iconidx);
#endif
    എങ്കിൽ (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	പൂർണ്ണസംഖ്യ	buflen;

	എങ്കിൽ (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	അല്ലെങ്കിൽ
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    എങ്കിൽ (status == OK && menu->children == ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ		bit;

	// Get the first mode in which the menu is available
	വേണ്ടി (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	എങ്കിൽ (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    എങ്കിൽ (menu->strings[bit] != ശൂന്യ)
	    {
		എങ്കിൽ (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		അല്ലെങ്കിൽ
		{
		    char_u *tofree = ശൂന്യ;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    എങ്കിൽ (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    എങ്കിൽ (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    എങ്കിൽ (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    എങ്കിൽ (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    എങ്കിൽ (status == OK && menu->children != ശൂന്യ)
    {
	vimmenu_T	*child;

	l = list_alloc();
	എങ്കിൽ (l == ശൂന്യ)
	    തിരികെ FAIL;

	എങ്കിൽ (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    തിരികെ FAIL;
	}
	child = menu->children;
	ആകുമ്പോൾ (child)
	{
	    എങ്കിൽ (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    തിരികെ status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    ശൂന്യം
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    പൂർണ്ണസംഖ്യ		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    retdict = rettv->vval.v_dict;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    menu_name = tv_get_string_chk(&argvars[0]);
    എങ്കിൽ (menu_name == ശൂന്യ)
	തിരികെ;

    // menu mode
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    അല്ലെങ്കിൽ
	which = (char_u *)"";	    // Default is modes for "menu"
    എങ്കിൽ (which == ശൂന്യ)
	തിരികെ;

    modes = get_menu_cmd_modes(which, *which == '!', ശൂന്യ, ശൂന്യ);

    // Locate the specified menu or menu item
    menu = *get_വേര്_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    എങ്കിൽ (saved_name == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	ആകുമ്പോൾ (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    ആകുമ്പോൾ (menu != ശൂന്യ)
	    {
		എങ്കിൽ (menu_name_equal(name, menu))
		    നിർത്തുക;
		menu = menu->next;
	    }
	    എങ്കിൽ (menu == ശൂന്യ || *p == NUL)
		നിർത്തുക;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    എങ്കിൽ (menu == ശൂന്യ)		// specified menu not found
	തിരികെ;

    എങ്കിൽ (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
