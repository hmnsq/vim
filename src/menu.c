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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
સ્થિર પૂર્ણાંક add_menu_path(char_u *, vimmenu_T *, પૂર્ણાંક *, char_u *, પૂર્ણાંક);
#નહીંતર
સ્થિર પૂર્ણાંક add_menu_path(char_u *, vimmenu_T *, પૂર્ણાંક *, char_u *);
#endif
સ્થિર પૂર્ણાંક menu_nable_recurse(vimmenu_T *menu, char_u *name, પૂર્ણાંક modes, પૂર્ણાંક enable);
સ્થિર પૂર્ણાંક remove_menu(vimmenu_T **, char_u *, પૂર્ણાંક, પૂર્ણાંક silent);
સ્થિર શૂન્ય free_menu(vimmenu_T **menup);
સ્થિર શૂન્ય free_menu_string(vimmenu_T *, પૂર્ણાંક);
સ્થિર પૂર્ણાંક show_menus(char_u *, પૂર્ણાંક);
સ્થિર શૂન્ય show_menus_recursive(vimmenu_T *, પૂર્ણાંક, પૂર્ણાંક);
સ્થિર char_u *menu_name_skip(char_u *name);
સ્થિર પૂર્ણાંક menu_name_equal(char_u *name, vimmenu_T *menu);
સ્થિર પૂર્ણાંક menu_namecmp(char_u *name, char_u *mname);
સ્થિર પૂર્ણાંક get_menu_cmd_modes(char_u *, પૂર્ણાંક, પૂર્ણાંક *, પૂર્ણાંક *);
સ્થિર char_u *popup_mode_name(char_u *name, પૂર્ણાંક idx);
સ્થિર char_u *menu_text(char_u *text, પૂર્ણાંક *mnemonic, char_u **actext);

#જો defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
સ્થિર શૂન્ય gui_create_tearoffs_recurse(vimmenu_T *menu, અચલ char_u *pname, પૂર્ણાંક *pri_tab, પૂર્ણાંક pri_idx);
સ્થિર શૂન્ય gui_add_tearoff(char_u *tearpath, પૂર્ણાંક *pri_tab, પૂર્ણાંક pri_idx);
સ્થિર શૂન્ય gui_destroy_tearoffs_recurse(vimmenu_T *menu);
સ્થિર પૂર્ણાંક s_tearoffs = FALSE;
#endif

સ્થિર પૂર્ણાંક menu_is_hidden(char_u *name);
સ્થિર પૂર્ણાંક menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
સ્થિર પૂર્ણાંક menus_locked = 0;

#જો defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
સ્થિર char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
સ્થિર char_u *menutrans_lookup(char_u *name, પૂર્ણાંક len);
સ્થિર શૂન્ય menu_unescape_name(char_u	*p);
#endif

સ્થિર char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
સ્થિર અક્ષર *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
સ્થિર અચલ અક્ષર *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_ગણતરી ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    સ્થિર પૂર્ણાંક
menu_is_winbar(char_u *name)
{
    પાછા (STRNCMP(name, "WinBar", 6) == 0);
}

    પૂર્ણાંક
winbar_height(win_T *wp)
{
    જો (wp->w_winbar != ખાલી && wp->w_winbar->children != ખાલી)
	પાછા 1;
    પાછા 0;
}

    સ્થિર vimmenu_T **
get_મૂળ_menu(char_u *name)
{
    જો (menu_is_winbar(name))
	પાછા &curwin->w_winbar;
    પાછા &મૂળ_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    સ્થિર પૂર્ણાંક
is_menus_locked(શૂન્ય)
{
    જો (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    શૂન્ય
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    પૂર્ણાંક		modes;
    char_u	*map_to;
    પૂર્ણાંક		noremap;
    પૂર્ણાંક		silent = FALSE;
    પૂર્ણાંક		special = FALSE;
    પૂર્ણાંક		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    પૂર્ણાંક		i;
#જો defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    પૂર્ણાંક		old_menu_height;
# જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    પૂર્ણાંક		old_toolbar_height;
# endif
#endif
    પૂર્ણાંક		pri_tab[MENUDEPTH + 1];
    પૂર્ણાંક		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = ખાલી;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**મૂળ_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    માટે (;;)
    {
	જો (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    ચાલુ;
	}
	જો (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    ચાલુ;
	}
	જો (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    ચાલુ;
	}
	તોડો;
    }


    // Locate an optional "icon=filename" argument.
    જો (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	જ્યારે (*arg != NUL && *arg != ' ')
	{
	    જો (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	જો (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    માટે (p = arg; *p; ++p)
	જો (!VIM_ISDIGIT(*p) && *p != '.')
	    તોડો;
    જો (VIM_ISWHITE(*p))
    {
	માટે (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    જો (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    જો (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    નહીંતર જો (eap->addr_ગણતરી && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    નહીંતર
	i = 0;
    જ્યારે (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    જો (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    નહીંતર જો (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    જો (*arg == NUL)
    {
	show_menus(arg, modes);
	પાછા;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    જો (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	જો (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    જો (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		જો (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((અક્ષર *)menu_path + 7);
		    જો (menuarg.iconidx >= (પૂર્ણાંક)TOOLBAR_NAME_ગણતરી)
			menuarg.iconidx = -1;
		    નહીંતર
			menuarg.icon_builtin = TRUE;
		}
	    }
	    નહીંતર
	    {
		માટે (i = 0; i < (પૂર્ણાંક)TOOLBAR_NAME_ગણતરી; ++i)
		    જો (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			તોડો;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    જો (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	જાઓ theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    જો (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	જાઓ theend;
    }
    નહીંતર જો (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	જાઓ theend;
    }
#જો defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    મૂળ_menu_ptr = get_મૂળ_menu(menu_path);
    જો (મૂળ_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    જો (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	જો (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	જો (menu_is_popup(menu_path))
	{
	    માટે (i = 0; i < MENU_INDEX_TIP; ++i)
		જો (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    જો (p != ખાલી)
		    {
			menu_nable_recurse(*મૂળ_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*મૂળ_menu_ptr, menu_path, modes, enable);
    }
    નહીંતર જો (unmenu)
    {
	જો (is_menus_locked())
	    જાઓ theend;

	/*
	 * Delete menu(s).
	 */
	જો (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	જો (menu_is_popup(menu_path))
	{
	    માટે (i = 0; i < MENU_INDEX_TIP; ++i)
		જો (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    જો (p != ખાલી)
		    {
			remove_menu(મૂળ_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(મૂળ_menu_ptr, menu_path, modes, FALSE);
    }
    નહીંતર
    {
	જો (is_menus_locked())
	    જાઓ theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	જો (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = ખાલી;
	}
	નહીંતર જો (modes & MENU_TIP_MODE)
	    map_buf = ખાલી;	// Menu tips are plain text.
	નહીંતર
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ખાલી);
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
	જો (menu_is_popup(menu_path))
	{
	    માટે (i = 0; i < MENU_INDEX_TIP; ++i)
		જો (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    જો (p != ખાલી)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = ખાલી;
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

#જો defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    જો (gui.in_use
	    && (gui.menu_height != old_menu_height
# જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    જો (મૂળ_menu_ptr == &curwin->w_winbar)
    {
	પૂર્ણાંક h = winbar_height(curwin);

	જો (h != curwin->w_winbar_height)
	{
	    જો (h == 0)
		++curwin->w_height;
	    નહીંતર જો (curwin->w_height > 0)
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
    સ્થિર પૂર્ણાંક
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    પૂર્ણાંક		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , પૂર્ણાંક	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    પૂર્ણાંક		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = ખાલી;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;
    પૂર્ણાંક		d;
#ifdef FEAT_GUI
    પૂર્ણાંક		idx;
    પૂર્ણાંક		new_idx;
#endif
    પૂર્ણાંક		pri_idx = 0;
    પૂર્ણાંક		old_modes = 0;
    પૂર્ણાંક		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = ખાલી;
#endif
    vimmenu_T	**મૂળ_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    જો (path_name == ખાલી)
	પાછા FAIL;
    મૂળ_menu_ptr = get_મૂળ_menu(menu_path);
    menup = મૂળ_menu_ptr;
    parent = ખાલી;
    name = path_name;
    જ્યારે (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (પૂર્ણાંક)STRLEN(name));
	જો (map_to != ખાલી)
	{
	    en_name = name;
	    name = map_to;
	}
	નહીંતર
	    en_name = ખાલી;
#endif
	dname = menu_text(name, ખાલી, ખાલી);
	જો (dname == ખાલી)
	    જાઓ erret;
	જો (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    જાઓ erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	જ્યારે (menu != ખાલી)
	{
	    જો (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		જો (*next_name == NUL && menu->children != ખાલી)
		{
		    જો (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    જાઓ erret;
		}
		જો (*next_name != NUL && menu->children == ખાલી
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    જો (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    જાઓ erret;
		}
		તોડો;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    જો (parent != ખાલી || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		જો (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	જો (menu == ખાલી)
	{
	    જો (*next_name == NUL && parent == ખાલી)
	    {
		emsg(_(e_must_not_add_menu_વસ્તુઓ_directly_to_menu_bar));
		જાઓ erret;
	    }

	    જો (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		જાઓ erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    જો (menu == ખાલી)
		જાઓ erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    જો (en_name != ખાલી)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, ખાલી, ખાલી);
	    }
	    નહીંતર
	    {
		menu->en_name = ખાલી;
		menu->en_dname = ખાલી;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = ખાલી;
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
	    જો (*next_name == NUL && menuarg->iconfile != ખાલી)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#જો defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    જો (parent != ખાલી && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	નહીંતર
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    જો (addtearoff)
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
	જો ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    જો (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		જો (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		નહીંતર
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# જો defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    જો (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != ખાલી
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
		જો (tearpath != ખાલી)
		{
		    char_u  *s;
		    પૂર્ણાંક	    len;

		    STRCPY(tearpath, menu_path);
		    len = (પૂર્ણાંક)(next_name - path_name - 1);
		    માટે (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			જો ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	જો (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_ઉમેરો_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_ઉમેરો_MODE));
    જો (sys_menu)
	modes &= ~old_modes;

    જો (menu != ખાલી && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == ખાલી) ? ખાલી : vim_strsave(call_data);

	// loop over all modes, may add more than one
	માટે (i = 0; i < MENU_MODES; ++i)
	{
	    જો (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		જો (amenu && call_data != ખાલી && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    પસંદગી (1 << i)
		    {
			સ્થિતિ MENU_VISUAL_MODE:
			સ્થિતિ MENU_SELECT_MODE:
			સ્થિતિ MENU_OP_PENDING_MODE:
			સ્થિતિ MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    તોડો;
			સ્થિતિ MENU_ઉમેરો_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    તોડો;
		    }
		}

		જો (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    જો (menu->strings[i] != ખાલી)
		    {
			menu->strings[i][0] = c;
			જો (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			નહીંતર
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			જો (c == Ctrl_C)
			{
			    પૂર્ણાંક	    len = (પૂર્ણાંક)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		નહીંતર
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	જો (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    પાછા OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    જ્યારે (parent != ખાલી && parent->children == ખાલી)
    {
	જો (parent->parent == ખાલી)
	    menup = મૂળ_menu_ptr;
	નહીંતર
	    menup = &parent->parent->children;
	માટે ( ; *menup != ખાલી && *menup != parent; menup = &((*menup)->next))
	    ;
	જો (*menup == ખાલી) // safety check
	    તોડો;
	parent = parent->parent;
	free_menu(menup);
    }
    પાછા FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    સ્થિર પૂર્ણાંક
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    પૂર્ણાંક		modes,
    પૂર્ણાંક		enable)
{
    char_u	*p;

    જો (menu == ખાલી)
	પાછા OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    જ્યારે (menu != ખાલી)
    {
	જો (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    જો (*p != NUL)
	    {
		જો (menu->children == ખાલી)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    પાછા FAIL;
		}
		જો (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર
		જો (enable)
		    menu->enabled |= modes;
		નહીંતર
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    જો (*name != NUL && *name != '*')
		તોડો;
	}
	menu = menu->next;
    }
    જો (*name != NUL && *name != '*' && menu == ખાલી)
    {
	semsg(_(e_no_menu_str), name);
	પાછા FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    પાછા OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    સ્થિર પૂર્ણાંક
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    પૂર્ણાંક		modes,
    પૂર્ણાંક		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    જો (*menup == ખાલી)
	પાછા OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    જ્યારે ((menu = *menup) != ખાલી)
    {
	જો (*name == NUL || menu_name_equal(name, menu))
	{
	    જો (*p != NUL && menu->children == ખાલી)
	    {
		જો (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		પાછા FAIL;
	    }
	    જો ((menu->modes & modes) != 0x0)
	    {
#જો defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		જો (*p == NUL && modes == MENU_ALL_MODES)
		{
		    જો (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		જો (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (*name != NUL)
	    {
		જો (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		પાછા FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    જો (*name != NUL)
		તોડો;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    જો (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    જો ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    નહીંતર
		menup = &menu->next;
	}
	નહીંતર
	    menup = &menu->next;
    }
    જો (*name != NUL)
    {
	જો (menu == ખાલી)
	{
	    જો (!silent)
		semsg(_(e_no_menu_str), name);
	    પાછા FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#જો defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	જો ((s_tearoffs) && (menu->children != ખાલી)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	નહીંતર
#endif
	    child = menu->children;
	માટે ( ; child != ખાલી; child = child->next)
	    menu->modes |= child->modes;
	જો (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    જો (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	જો ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#જો defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    જો (s_tearoffs && menu->children != ખાલી) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    પાછા OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    શૂન્ય
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_વસ્તુઓ);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    સ્થિર શૂન્ય
free_menu(vimmenu_T **menup)
{
    પૂર્ણાંક		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    જો (gui.in_use)
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
    માટે (i = 0; i < MENU_MODES; i++)
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
    સ્થિર શૂન્ય
free_menu_string(vimmenu_T *menu, પૂર્ણાંક idx)
{
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		i;

    માટે (i = 0; i < MENU_MODES; i++)
	જો (menu->strings[i] == menu->strings[idx])
	    ગણતરી++;
    જો (ગણતરી == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = ખાલી;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    સ્થિર પૂર્ણાંક
show_menus(char_u *path_name, પૂર્ણાંક modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = ખાલી;

    name = path_name = vim_strsave(path_name);
    જો (path_name == ખાલી)
	પાછા FAIL;
    menu = *get_મૂળ_menu(path_name);

    // First, find the (sub)menu with the given name
    જ્યારે (*name)
    {
	p = menu_name_skip(name);
	જ્યારે (menu != ખાલી)
	{
	    જો (menu_name_equal(name, menu))
	    {
		// Found menu
		જો (*p != NUL && menu->children == ખાલી)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    પાછા FAIL;
		}
		નહીંતર જો ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    પાછા FAIL;
		}
		તોડો;
	    }
	    menu = menu->next;
	}
	જો (menu == ખાલી)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    પાછા FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    સંદેશ_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    પાછા OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    સ્થિર શૂન્ય
show_menus_recursive(vimmenu_T *menu, પૂર્ણાંક modes, પૂર્ણાંક depth)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		bit;

    જો (menu != ખાલી && (menu->modes & modes) == 0x0)
	પાછા;

    જો (menu != ખાલી)
    {
	સંદેશ_putchar('\n');
	જો (got_int)		// "q" hit for "--more--"
	    પાછા;
	માટે (i = 0; i < depth; i++)
	    સંદેશ_puts("  ");
	જો (menu->priority)
	{
	    સંદેશ_outnum((long)menu->priority);
	    સંદેશ_puts(" ");
	}
				// Same highlighting as for directories!?
	સંદેશ_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    જો (menu != ખાલી && menu->children == ખાલી)
    {
	માટે (bit = 0; bit < MENU_MODES; bit++)
	    જો ((menu->modes & modes & (1 << bit)) != 0)
	    {
		સંદેશ_putchar('\n');
		જો (got_int)		// "q" hit for "--more--"
		    પાછા;
		માટે (i = 0; i < depth + 2; i++)
		    સંદેશ_puts("  ");
		સંદેશ_puts(menu_mode_chars[bit]);
		જો (menu->noremap[bit] == REMAP_NONE)
		    સંદેશ_putchar('*');
		નહીંતર જો (menu->noremap[bit] == REMAP_SCRIPT)
		    સંદેશ_putchar('&');
		નહીંતર
		    સંદેશ_putchar(' ');
		જો (menu->silent[bit])
		    સંદેશ_putchar('s');
		નહીંતર
		    સંદેશ_putchar(' ');
		જો ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    સંદેશ_putchar('-');
		નહીંતર
		    સંદેશ_putchar(' ');
		સંદેશ_puts(" ");
		જો (*menu->strings[bit] == NUL)
		    સંદેશ_puts_attr("<Nop>", HL_ATTR(HLF_8));
		નહીંતર
		    સંદેશ_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    નહીંતર
    {
	જો (menu == ખાલી)
	{
	    menu = મૂળ_menu;
	    depth--;
	}
	નહીંતર
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	માટે (; menu != ખાલી && !got_int; menu = menu->next)
	    જો (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
સ્થિર vimmenu_T	*expand_menu = ખાલી;
સ્થિર vimmenu_T	*expand_menu_alt = ખાલી;
સ્થિર પૂર્ણાંક		expand_modes = 0x0;
સ્થિર પૂર્ણાંક		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    પૂર્ણાંક		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = ખાલી;
    char_u	*name;
    પૂર્ણાંક		unmenu;
    vimmenu_T	*menu;
    પૂર્ણાંક		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    માટે (p = arg; *p; ++p)
	જો (!VIM_ISDIGIT(*p) && *p != '.')
	    તોડો;

    જો (!VIM_ISWHITE(*p))
    {
	જો (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	નહીંતર જો (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	નહીંતર
	    p = arg;
    }

    જ્યારે (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    માટે (; *p && !VIM_ISWHITE(*p); ++p)
    {
	જો ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	નહીંતર જો (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    જો (expand_menus && VIM_ISWHITE(*p))
	પાછા ખાલી;	// TODO: check for next command?
    જો (*p == NUL)		// Complete the menu name
    {
	પૂર્ણાંક try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, ખાલી, &unmenu);
	જો (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = મૂળ_menu;
	જો (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    જો (path_name == ખાલી)
		પાછા ખાલી;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	જ્યારે (name != ખાલી && *name)
	{
	    p = menu_name_skip(name);
	    જ્યારે (menu != ખાલી)
	    {
		જો (menu_name_equal(name, menu))
		{
		    // Found menu
		    જો ((*p != NUL && menu->children == ખાલી)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			પાછા ખાલી;
		    }
		    તોડો;
		}
		menu = menu->next;
		જો (menu == ખાલી && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    જો (menu == ખાલી)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		પાછા ખાલી;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	જો (expand_menu == મૂળ_menu)
	    expand_menu_alt = curwin->w_winbar;
	નહીંતર
	    expand_menu_alt = ખાલી;
    }
    નહીંતર			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    પાછા ખાલી;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    સ્થિર vimmenu_T	*menu = ખાલી;
    સ્થિર પૂર્ણાંક		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    સ્થિર  પૂર્ણાંક		should_advance = FALSE;
#endif

    જો (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    જ્યારે (menu != ખાલી && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == ખાલી))
    {
	menu = menu->next;
	જો (menu == ખાલી && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    જો (menu == ખાલી)	    // at end of linked list
	પાછા ખાલી;

    જો (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	જો (should_advance)
	    str = menu->en_dname;
	નહીંતર
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    જો (menu->en_dname == ખાલી)
		should_advance = TRUE;
	}
#endif
    નહીંતર
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    જો (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	જો (menu == ખાલી && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    પાછા str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    સ્થિર vimmenu_T	*menu = ખાલી;
    સ્થિર પૂર્ણાંક		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    સ્થિર char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    સ્થિર  પૂર્ણાંક		should_advance = FALSE;
#endif

    જો (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    જ્યારે (menu != ખાલી
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	જો (menu == ખાલી && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    જો (menu == ખાલી)	    // at end of linked list
	પાછા ખાલી;

    જો (menu->modes & expand_modes)
    {
	જો (menu->children != ખાલી)
	{
#ifdef FEAT_MULTI_LANG
	    જો (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    નહીંતર
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		જો (menu->en_dname == ખાલી)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	નહીંતર
#ifdef FEAT_MULTI_LANG
	{
	    જો (should_advance)
		str = menu->en_dname;
	    નહીંતર
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		જો (menu->en_dname == ખાલી)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    નહીંતર
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    જો (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	જો (menu == ખાલી && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    પાછા str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    સ્થિર char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    માટે (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	જો (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    જો (*p == NUL)
		તોડો;
	}
    }
    જો (*p)
	*p++ = NUL;
    પાછા p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    સ્થિર પૂર્ણાંક
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    જો (menu->en_name != ખાલી
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	પાછા TRUE;
#endif
    પાછા menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    સ્થિર પૂર્ણાંક
menu_namecmp(char_u *name, char_u *mname)
{
    પૂર્ણાંક		i;

    માટે (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	જો (name[i] != mname[i])
	    તોડો;
    પાછા ((name[i] == NUL || name[i] == TAB)
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
    સ્થિર પૂર્ણાંક
get_menu_cmd_modes(
    char_u  *cmd,
    પૂર્ણાંક	    forceit,	    // Was there a "!" after the command?
    પૂર્ણાંક	    *noremap,
    પૂર્ણાંક	    *unmenu)
{
    પૂર્ણાંક	    modes;

    પસંદગી (*cmd++)
    {
	સ્થિતિ 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    તોડો;
	સ્થિતિ 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    તોડો;
	સ્થિતિ 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    તોડો;
	સ્થિતિ 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    તોડો;
	સ્થિતિ 'i':			// imenu
	    modes = MENU_ઉમેરો_MODE;
	    તોડો;
	સ્થિતિ 't':
	    જો (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		તોડો;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    તોડો;
	સ્થિતિ 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    તોડો;
	સ્થિતિ 'a':			// amenu
	    modes = MENU_ઉમેરો_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    તોડો;
	સ્થિતિ 'n':
	    જો (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		તોડો;
	    }
	    // FALLTHROUGH
	મૂળભૂત:
	    --cmd;
	    જો (forceit)		// menu!!
		modes = MENU_ઉમેરો_MODE | MENU_CMDLINE_MODE;
	    નહીંતર			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    જો (noremap != ખાલી)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    જો (unmenu != ખાલી)
	*unmenu = (*cmd == 'u');
    પાછા modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    સ્થિર char_u *
get_menu_mode_str(પૂર્ણાંક modes)
{
    જો ((modes & (MENU_ઉમેરો_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_ઉમેરો_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	પાછા (char_u *)"a";
    જો ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	પાછા (char_u *)" ";
    જો ((modes & (MENU_ઉમેરો_MODE | MENU_CMDLINE_MODE))
	    == (MENU_ઉમેરો_MODE | MENU_CMDLINE_MODE))
	પાછા (char_u *)"!";
    જો ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	પાછા (char_u *)"v";
    જો (modes & MENU_VISUAL_MODE)
	પાછા (char_u *)"x";
    જો (modes & MENU_SELECT_MODE)
	પાછા (char_u *)"s";
    જો (modes & MENU_OP_PENDING_MODE)
	પાછા (char_u *)"o";
    જો (modes & MENU_ઉમેરો_MODE)
	પાછા (char_u *)"i";
    જો (modes & MENU_TERMINAL_MODE)
	પાછા (char_u *)"tl";
    જો (modes & MENU_CMDLINE_MODE)
	પાછા (char_u *)"c";
    જો (modes & MENU_NORMAL_MODE)
	પાછા (char_u *)"n";
    જો (modes & MENU_TIP_MODE)
	પાછા (char_u *)"t";

    પાછા (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    સ્થિર char_u *
popup_mode_name(char_u *name, પૂર્ણાંક idx)
{
    char_u	*p;
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(name);
    અક્ષર	*mode_chars = menu_mode_chars[idx];
    પૂર્ણાંક		mode_chars_len = (પૂર્ણાંક)strlen(mode_chars);
    પૂર્ણાંક		i;

    p = vim_strnsave(name, len + mode_chars_len);
    જો (p == ખાલી)
	પાછા ખાલી;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    માટે (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    પાછા p;
}

#જો defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    પૂર્ણાંક
get_menu_index(vimmenu_T *menu, પૂર્ણાંક state)
{
    પૂર્ણાંક		idx;

    જો ((state & MODE_ઉમેરો))
	idx = MENU_INDEX_ઉમેરો;
    નહીંતર જો (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    નહીંતર જો (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    નહીંતર જો (VIsual_active)
    {
	જો (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	નહીંતર
	    idx = MENU_INDEX_VISUAL;
    }
    નહીંતર જો (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    નહીંતર જો (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    નહીંતર જો ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    નહીંતર
	idx = MENU_INDEX_INVALID;

    જો (idx != MENU_INDEX_INVALID && menu->strings[idx] == ખાલી)
	idx = MENU_INDEX_INVALID;
    પાછા idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    સ્થિર char_u *
menu_text(char_u *str, પૂર્ણાંક *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    જો (p != ખાલી)
    {
	જો (actext != ખાલી)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    નહીંતર
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    માટે (p = text; p != ખાલી; )
    {
	p = vim_strchr(p, '&');
	જો (p != ખાલી)
	{
	    જો (p[1] == NUL)	    // trailing "&"
		તોડો;
	    જો (mnemonic != ખાલી && p[1] != '&')
#જો !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#નહીંતર
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		અક્ષર c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    પાછા text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    પૂર્ણાંક
menu_is_menubar(char_u *name)
{
    પાછા (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    પૂર્ણાંક
menu_is_popup(char_u *name)
{
    પાછા (STRNCMP(name, "PopUp", 5) == 0);
}

#જો defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    પૂર્ણાંક
menu_is_child_of_popup(vimmenu_T *menu)
{
    જ્યારે (menu->parent != ખાલી)
	menu = menu->parent;
    પાછા menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    પૂર્ણાંક
menu_is_toolbar(char_u *name)
{
    પાછા (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    પૂર્ણાંક
menu_is_separator(char_u *name)
{
    પાછા (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    સ્થિર પૂર્ણાંક
menu_is_hidden(char_u *name)
{
    પાછા (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    સ્થિર પૂર્ણાંક
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    પાછા (STRCMP(name, TEAR_STRING) == 0);
#નહીંતર
    પાછા FALSE;
#endif
}

#જો defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    સ્થિર પૂર્ણાંક
get_menu_mode(શૂન્ય)
{
# ifdef FEAT_TERMINAL
    જો (term_use_loop())
	પાછા MENU_INDEX_TERMINAL;
# endif
    જો (VIsual_active)
    {
	જો (VIsual_select)
	    પાછા MENU_INDEX_SELECT;
	પાછા MENU_INDEX_VISUAL;
    }
    જો (State & MODE_ઉમેરો)
	પાછા MENU_INDEX_ઉમેરો;
    જો ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	પાછા MENU_INDEX_CMDLINE;
    જો (finish_op)
	પાછા MENU_INDEX_OP_PENDING;
    જો (State & MODE_NORMAL)
	પાછા MENU_INDEX_NORMAL;
    જો (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	પાછા MENU_INDEX_ઉમેરો;
    પાછા MENU_INDEX_INVALID;
}

    પૂર્ણાંક
get_menu_mode_flag(શૂન્ય)
{
    પૂર્ણાંક mode = get_menu_mode();

    જો (mode == MENU_INDEX_INVALID)
	પાછા 0;
    પાછા 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    શૂન્ય
show_popupmenu(શૂન્ય)
{
    vimmenu_T	*menu;
    પૂર્ણાંક		menu_mode;
    અક્ષર*	mode;
    પૂર્ણાંક		mode_len;

    menu_mode = get_menu_mode();
    જો (menu_mode == MENU_INDEX_INVALID)
	પાછા;
    mode = menu_mode_chars[menu_mode];
    mode_len = (પૂર્ણાંક)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, ખાલી, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	જો (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    તોડો;

    // Only show a popup when it is defined and has entries
    જો (menu == ખાલી || menu->children == ખાલી)
	પાછા;

# જો defined(FEAT_GUI)
    જો (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# જો defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    નહીંતર
# endif
# જો defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#જો defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    પૂર્ણાંક
check_menu_pointer(vimmenu_T *મૂળ, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    માટે (p = મૂળ; p != ખાલી; p = p->next)
	જો (p == menu_to_check
		|| (p->children != ખાલી
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    પાછા OK;
    પાછા FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    શૂન્ય
gui_create_initial_menus(vimmenu_T *menu)
{
    પૂર્ણાંક		idx = 0;

    જ્યારે (menu != ખાલી)
    {
	// Don't add a menu when only a tip was defined.
	જો (menu->modes & MENU_ALL_MODES)
	{
	    જો (menu->children != ખાલી)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    નહીંતર
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    સ્થિર શૂન્ય
gui_update_menus_recurse(vimmenu_T *menu, પૂર્ણાંક mode)
{
    પૂર્ણાંક		grey;

    જ્યારે (menu)
    {
	જો ((menu->modes & menu->enabled & mode)
# જો defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	નહીંતર
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	જો (vim_strchr(p_go, GO_GREY) != ખાલી || menu->parent == ખાલી
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	નહીંતર
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
    શૂન્ય
gui_update_menus(પૂર્ણાંક modes)
{
    સ્થિર પૂર્ણાંક	    prev_mode = -1;
    પૂર્ણાંક		    mode = 0;

    જો (modes != 0x0)
	mode = modes;
    નહીંતર
	mode = get_menu_mode_flag();

    જો (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(મૂળ_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    પૂર્ણાંક
gui_is_menu_shortcut(પૂર્ણાંક ચાવી)
{
    vimmenu_T	*menu;

    જો (ચાવી < 256)
	ચાવી = TOLOWER_LOC(ચાવી);
    FOR_ALL_MENUS(menu)
	જો (menu->mnemonic == ચાવી
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == ચાવી))
	    પાછા TRUE;
    પાછા FALSE;
}
# endif
#endif // FEAT_GUI

#જો defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    શૂન્ય
gui_mch_toggle_tearoffs(પૂર્ણાંક enable)
{
    પૂર્ણાંક		pri_tab[MENUDEPTH + 1];
    પૂર્ણાંક		i;

    જો (enable)
    {
	માટે (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(મૂળ_menu, (char_u *)"", pri_tab, 0);
    }
    નહીંતર
	gui_destroy_tearoffs_recurse(મૂળ_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    સ્થિર શૂન્ય
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    અચલ char_u	*pname,
    પૂર્ણાંક			*pri_tab,
    પૂર્ણાંક			pri_idx)
{
    char_u	*newpname = ખાલી;
    પૂર્ણાંક		len;
    char_u	*s;
    char_u	*d;

    જો (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    જ્યારે (menu != ખાલી)
    {
	જો (menu->children != ખાલી && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (પૂર્ણાંક)STRLEN(pname) + (પૂર્ણાંક)STRLEN(menu->name);
	    માટે (s = menu->name; *s; ++s)
		જો (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    જો (newpname != ખાલી)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		માટે (s = menu->name; *s; ++s)
		{
		    જો (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		જો (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    સ્થિર શૂન્ય
gui_add_tearoff(char_u *tearpath, પૂર્ણાંક *pri_tab, પૂર્ણાંક pri_idx)
{
    char_u	*tbuf;
    પૂર્ણાંક		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned પૂર્ણાંક)STRLEN(tearpath));
    જો (tbuf == ખાલી)
	પાછા;

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
    menuarg.iconfile = ખાલી;
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
    સ્થિર શૂન્ય
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    જ્યારે (menu)
    {
	જો (menu->children)
	{
	    // check if tearoff exists
	    જો (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    જો (menu->children != ખાલી) // if not the last one
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
    શૂન્ય
execute_menu(exarg_T *eap, vimmenu_T *menu, પૂર્ણાંક mode_idx)
{
    પૂર્ણાંક		idx = mode_idx;

    જો (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	જો (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_ઉમેરો;
	}
#ifdef FEAT_TERMINAL
	નહીંતર જો (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	નહીંતર જો (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	નહીંતર જો (eap != ખાલી && eap->addr_ગણતરી)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    જો ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    નહીંતર
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
	    જો (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    જો (idx == MENU_INDEX_INVALID || eap == ખાલી)
	idx = MENU_INDEX_NORMAL;

    જો (menu->strings[idx] != ખાલી && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	જો (eap == ખાલી || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    જો (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	નહીંતર
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    નહીંતર જો (eap != ખાલી)
    {
	char_u	*mode;

	પસંદગી (idx)
	{
	    સ્થિતિ MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		તોડો;
	    સ્થિતિ MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		તોડો;
	    સ્થિતિ MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		તોડો;
	    સ્થિતિ MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		તોડો;
	    સ્થિતિ MENU_INDEX_ઉમેરો:
		mode = (char_u *)"Insert";
		તોડો;
	    સ્થિતિ MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		તોડો;
	    // case MENU_INDEX_TIP: cannot happen
	    મૂળભૂત:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    સ્થિર vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    પૂર્ણાંક		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    જો (saved_name == ખાલી)
	પાછા ખાલી;

    menu = *get_મૂળ_menu(saved_name);
    name = saved_name;
    જ્યારે (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	જ્યારે (menu != ખાલી)
	{
	    જો (menu_name_equal(name, menu))
	    {
		જો (*p == NUL && menu->children != ખાલી)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = ખાલી;
		}
		નહીંતર જો (*p != NUL && menu->children == ખાલી)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ખાલી;
		}
		તોડો;
	    }
	    menu = menu->next;
	}
	જો (menu == ખાલી || *p == NUL)
	    તોડો;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    જો (menu == ખાલી)
    {
	જો (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	પાછા ખાલી;
    }

    પાછા menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    શૂન્ય
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    પૂર્ણાંક		mode_idx = MENU_INDEX_INVALID;

    જો (arg[0] && VIM_ISWHITE(arg[1]))
    {
	પસંદગી (arg[0])
	{
	    સ્થિતિ 'n': mode_idx = MENU_INDEX_NORMAL; તોડો;
	    સ્થિતિ 'v': mode_idx = MENU_INDEX_VISUAL; તોડો;
	    સ્થિતિ 's': mode_idx = MENU_INDEX_SELECT; તોડો;
	    સ્થિતિ 'o': mode_idx = MENU_INDEX_OP_PENDING; તોડો;
	    સ્થિતિ 't': mode_idx = MENU_INDEX_TERMINAL; તોડો;
	    સ્થિતિ 'i': mode_idx = MENU_INDEX_ઉમેરો; તોડો;
	    સ્થિતિ 'c': mode_idx = MENU_INDEX_CMDLINE; તોડો;
	    મૂળભૂત: semsg(_(e_invalid_argument_str), arg);
		     પાછા;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    જો (menu == ખાલી)
	પાછા;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    શૂન્ય
winbar_click(win_T *wp, પૂર્ણાંક col)
{
    પૂર્ણાંક		idx;

    જો (wp->w_winbar_વસ્તુઓ == ખાલી)
	પાછા;
    માટે (idx = 0; wp->w_winbar_વસ્તુઓ[idx].wb_menu != ખાલી; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_વસ્તુઓ[idx];

	જો (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = ખાલી;
	    pos_T   save_visual = VIsual;
	    પૂર્ણાંક	    save_visual_active = VIsual_active;
	    પૂર્ણાંક	    save_visual_select = VIsual_select;
	    પૂર્ણાંક	    save_visual_reselect = VIsual_reselect;
	    પૂર્ણાંક	    save_visual_mode = VIsual_mode;

	    જો (wp != curwin)
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
	    execute_menu(ખાલી, item->wb_menu, -1);

	    જો (save_curwin != ખાલી && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    જો (!win_valid(wp))
		તોડો;
	}
    }
}

#જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = ખાલી;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_મૂળ_menu(path_name);

    saved_name = vim_strsave(path_name);
    જો (saved_name == ખાલી)
	પાછા ખાલી;

    name = saved_name;
    જ્યારે (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	જ્યારે (menu != ખાલી)
	{
	    જો (menu_name_equal(name, menu))
	    {
		જો (menu->children == ખાલી)
		{
		    // found a menu item instead of a sub-menu
		    જો (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    નહીંતર
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ખાલી;
		    જાઓ theend;
		}
		જો (*p == NUL)	    // found a full match
		    જાઓ theend;
		તોડો;
	    }
	    menu = menu->next;
	}
	જો (menu == ખાલી)	// didn't find it
	    તોડો;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    જો (menu == ખાલી)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    પાછા menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

પ્રકારવ્યાખ્યા સંરચના
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

સ્થિર garray_T menutrans_ga = {0, 0, 0, 0, ખાલી};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    શૂન્ય
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    પૂર્ણાંક			i;
    char_u		*from, *from_noamp, *to;

    જો (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, કદ(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    જો (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	માટે (i = 0; i < menutrans_ga.ga_len; ++i)
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
    નહીંતર
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	જો (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	નહીંતર
	{
	    જો (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		જો (from != ખાલી)
		{
		    from_noamp = menu_text(from, ખાલી, ખાલી);
		    to = vim_strnsave(to, arg - to);
		    જો (from_noamp != ખાલી && to != ખાલી)
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
		    નહીંતર
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

#જો defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    સ્થિર char_u *
menu_skip_part(char_u *p)
{
    જ્યારે (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	જો ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    પાછા p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    સ્થિર char_u *
menutrans_lookup(char_u *name, પૂર્ણાંક len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    પૂર્ણાંક			i;
    char_u		*dname;

    માટે (i = 0; i < menutrans_ga.ga_len; ++i)
	જો (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    પાછા tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, ખાલી, ખાલી);
    name[len] = i;
    જો (dname == ખાલી)
	પાછા ખાલી;

    માટે (i = 0; i < menutrans_ga.ga_len; ++i)
	જો (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    પાછા tp[i].to;
	}
    vim_free(dname);

    પાછા ખાલી;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    સ્થિર શૂન્ય
menu_unescape_name(char_u *name)
{
    char_u  *p;

    માટે (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	જો (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    સ્થિર char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    જ્યારે (*arg && !VIM_ISWHITE(*arg))
    {
	જો ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	નહીંતર જો (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    જો (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    પાછા arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    સ્થિર પૂર્ણાંક
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, પૂર્ણાંક modes, dict_T *dict)
{
    પૂર્ણાંક		status;
    list_T	*l;

    જો (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	જો (l == ખાલી)
	    પાછા FAIL;

	જો (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    પાછા FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	માટે (topmenu = menu; topmenu != ખાલી; topmenu = topmenu->next)
	    જો (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	પાછા OK;
    }

    જો (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	પાછા OK;

    status = dict_add_string(dict, "name", menu->name);
    જો (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    જો (status == OK && menu->actext != ખાલી)
	status = dict_add_string(dict, "accel", menu->actext);
    જો (status == OK)
	status = dict_add_સંખ્યા(dict, "priority", menu->priority);
    જો (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    જો (status == OK && menu->iconfile != ખાલી)
	status = dict_add_string(dict, "icon", menu->iconfile);
    જો (status == OK && menu->iconidx >= 0)
	status = dict_add_સંખ્યા(dict, "iconidx", menu->iconidx);
#endif
    જો (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	પૂર્ણાંક	buflen;

	જો (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	નહીંતર
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    જો (status == OK && menu->children == ખાલી)
    {
	પૂર્ણાંક		bit;

	// Get the first mode in which the menu is available
	માટે (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	જો (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    જો (menu->strings[bit] != ખાલી)
	    {
		જો (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		નહીંતર
		{
		    char_u *tofree = ખાલી;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    જો (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    જો (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    જો (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    જો (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    જો (status == OK && menu->children != ખાલી)
    {
	vimmenu_T	*child;

	l = list_alloc();
	જો (l == ખાલી)
	    પાછા FAIL;

	જો (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    પાછા FAIL;
	}
	child = menu->children;
	જ્યારે (child)
	{
	    જો (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    પાછા status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    શૂન્ય
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    પૂર્ણાંક		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    retdict = rettv->vval.v_dict;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    menu_name = tv_get_string_chk(&argvars[0]);
    જો (menu_name == ખાલી)
	પાછા;

    // menu mode
    જો (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    નહીંતર
	which = (char_u *)"";	    // Default is modes for "menu"
    જો (which == ખાલી)
	પાછા;

    modes = get_menu_cmd_modes(which, *which == '!', ખાલી, ખાલી);

    // Locate the specified menu or menu item
    menu = *get_મૂળ_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    જો (saved_name == ખાલી)
	પાછા;
    જો (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	જ્યારે (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    જ્યારે (menu != ખાલી)
	    {
		જો (menu_name_equal(name, menu))
		    તોડો;
		menu = menu->next;
	    }
	    જો (menu == ખાલી || *p == NUL)
		તોડો;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    જો (menu == ખાલી)		// specified menu not found
	પાછા;

    જો (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
