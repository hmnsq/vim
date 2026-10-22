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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
স্থির পূর্ণাঙ্ক add_menu_path(char_u *, vimmenu_T *, পূর্ণাঙ্ক *, char_u *, পূর্ণাঙ্ক);
#অন্যথা
স্থির পূর্ণাঙ্ক add_menu_path(char_u *, vimmenu_T *, পূর্ণাঙ্ক *, char_u *);
#endif
স্থির পূর্ণাঙ্ক menu_nable_recurse(vimmenu_T *menu, char_u *name, পূর্ণাঙ্ক modes, পূর্ণাঙ্ক enable);
স্থির পূর্ণাঙ্ক remove_menu(vimmenu_T **, char_u *, পূর্ণাঙ্ক, পূর্ণাঙ্ক silent);
স্থির শূন্য free_menu(vimmenu_T **menup);
স্থির শূন্য free_menu_string(vimmenu_T *, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক show_menus(char_u *, পূর্ণাঙ্ক);
স্থির শূন্য show_menus_recursive(vimmenu_T *, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
স্থির char_u *menu_name_skip(char_u *name);
স্থির পূর্ণাঙ্ক menu_name_equal(char_u *name, vimmenu_T *menu);
স্থির পূর্ণাঙ্ক menu_namecmp(char_u *name, char_u *mname);
স্থির পূর্ণাঙ্ক get_menu_cmd_modes(char_u *, পূর্ণাঙ্ক, পূর্ণাঙ্ক *, পূর্ণাঙ্ক *);
স্থির char_u *popup_mode_name(char_u *name, পূর্ণাঙ্ক idx);
স্থির char_u *menu_text(char_u *text, পূর্ণাঙ্ক *mnemonic, char_u **actext);

#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
স্থির শূন্য gui_create_tearoffs_recurse(vimmenu_T *menu, ধ্রুৱ char_u *pname, পূর্ণাঙ্ক *pri_tab, পূর্ণাঙ্ক pri_idx);
স্থির শূন্য gui_add_tearoff(char_u *tearpath, পূর্ণাঙ্ক *pri_tab, পূর্ণাঙ্ক pri_idx);
স্থির শূন্য gui_destroy_tearoffs_recurse(vimmenu_T *menu);
স্থির পূর্ণাঙ্ক s_tearoffs = FALSE;
#endif

স্থির পূর্ণাঙ্ক menu_is_hidden(char_u *name);
স্থির পূর্ণাঙ্ক menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
স্থির পূর্ণাঙ্ক menus_locked = 0;

#যদি defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
স্থির char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
স্থির char_u *menutrans_lookup(char_u *name, পূর্ণাঙ্ক len);
স্থির শূন্য menu_unescape_name(char_u	*p);
#endif

স্থির char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
স্থির ৱর্ণ *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
স্থির ধ্রুৱ ৱর্ণ *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_গুনতি ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    স্থির পূর্ণাঙ্ক
menu_is_winbar(char_u *name)
{
    ঘূৰাই (STRNCMP(name, "WinBar", 6) == 0);
}

    পূর্ণাঙ্ক
winbar_height(win_T *wp)
{
    যদি (wp->w_winbar != রিক্ত && wp->w_winbar->children != রিক্ত)
	ঘূৰাই 1;
    ঘূৰাই 0;
}

    স্থির vimmenu_T **
get_মূল_menu(char_u *name)
{
    যদি (menu_is_winbar(name))
	ঘূৰাই &curwin->w_winbar;
    ঘূৰাই &মূল_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    স্থির পূর্ণাঙ্ক
is_menus_locked(শূন্য)
{
    যদি (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    শূন্য
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    পূর্ণাঙ্ক		modes;
    char_u	*map_to;
    পূর্ণাঙ্ক		noremap;
    পূর্ণাঙ্ক		silent = FALSE;
    পূর্ণাঙ্ক		special = FALSE;
    পূর্ণাঙ্ক		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    পূর্ণাঙ্ক		i;
#যদি defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    পূর্ণাঙ্ক		old_menu_height;
# যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    পূর্ণাঙ্ক		old_toolbar_height;
# endif
#endif
    পূর্ণাঙ্ক		pri_tab[MENUDEPTH + 1];
    পূর্ণাঙ্ক		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = রিক্ত;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**মূল_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    বাবে (;;)
    {
	যদি (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    অনুৱর্ত;
	}
	যদি (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    অনুৱর্ত;
	}
	যদি (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    অনুৱর্ত;
	}
	ভঙ্গ;
    }


    // Locate an optional "icon=filename" argument.
    যদি (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	যেতিয়ালৈ (*arg != NUL && *arg != ' ')
	{
	    যদি (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	যদি (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    বাবে (p = arg; *p; ++p)
	যদি (!VIM_ISDIGIT(*p) && *p != '.')
	    ভঙ্গ;
    যদি (VIM_ISWHITE(*p))
    {
	বাবে (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    যদি (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    যদি (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    অন্যথা যদি (eap->addr_গুনতি && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    অন্যথা
	i = 0;
    যেতিয়ালৈ (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    যদি (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    অন্যথা যদি (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    যদি (*arg == NUL)
    {
	show_menus(arg, modes);
	ঘূৰাই;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    যদি (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	যদি (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    যদি (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		যদি (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((ৱর্ণ *)menu_path + 7);
		    যদি (menuarg.iconidx >= (পূর্ণাঙ্ক)TOOLBAR_NAME_গুনতি)
			menuarg.iconidx = -1;
		    অন্যথা
			menuarg.icon_builtin = TRUE;
		}
	    }
	    অন্যথা
	    {
		বাবে (i = 0; i < (পূর্ণাঙ্ক)TOOLBAR_NAME_গুনতি; ++i)
		    যদি (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			ভঙ্গ;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    যদি (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	গচ্ছ theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    যদি (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	গচ্ছ theend;
    }
    অন্যথা যদি (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	গচ্ছ theend;
    }
#যদি defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    মূল_menu_ptr = get_মূল_menu(menu_path);
    যদি (মূল_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    যদি (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	যদি (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	যদি (menu_is_popup(menu_path))
	{
	    বাবে (i = 0; i < MENU_INDEX_TIP; ++i)
		যদি (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    যদি (p != রিক্ত)
		    {
			menu_nable_recurse(*মূল_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*মূল_menu_ptr, menu_path, modes, enable);
    }
    অন্যথা যদি (unmenu)
    {
	যদি (is_menus_locked())
	    গচ্ছ theend;

	/*
	 * Delete menu(s).
	 */
	যদি (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	যদি (menu_is_popup(menu_path))
	{
	    বাবে (i = 0; i < MENU_INDEX_TIP; ++i)
		যদি (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    যদি (p != রিক্ত)
		    {
			remove_menu(মূল_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(মূল_menu_ptr, menu_path, modes, FALSE);
    }
    অন্যথা
    {
	যদি (is_menus_locked())
	    গচ্ছ theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	যদি (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = রিক্ত;
	}
	অন্যথা যদি (modes & MENU_TIP_MODE)
	    map_buf = রিক্ত;	// Menu tips are plain text.
	অন্যথা
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), রিক্ত);
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
	যদি (menu_is_popup(menu_path))
	{
	    বাবে (i = 0; i < MENU_INDEX_TIP; ++i)
		যদি (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    যদি (p != রিক্ত)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = রিক্ত;
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

#যদি defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    যদি (gui.in_use
	    && (gui.menu_height != old_menu_height
# যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    যদি (মূল_menu_ptr == &curwin->w_winbar)
    {
	পূর্ণাঙ্ক h = winbar_height(curwin);

	যদি (h != curwin->w_winbar_height)
	{
	    যদি (h == 0)
		++curwin->w_height;
	    অন্যথা যদি (curwin->w_height > 0)
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
    স্থির পূর্ণাঙ্ক
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    পূর্ণাঙ্ক		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , পূর্ণাঙ্ক	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    পূর্ণাঙ্ক		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = রিক্ত;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		d;
#ifdef FEAT_GUI
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		new_idx;
#endif
    পূর্ণাঙ্ক		pri_idx = 0;
    পূর্ণাঙ্ক		old_modes = 0;
    পূর্ণাঙ্ক		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = রিক্ত;
#endif
    vimmenu_T	**মূল_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    যদি (path_name == রিক্ত)
	ঘূৰাই FAIL;
    মূল_menu_ptr = get_মূল_menu(menu_path);
    menup = মূল_menu_ptr;
    parent = রিক্ত;
    name = path_name;
    যেতিয়ালৈ (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (পূর্ণাঙ্ক)STRLEN(name));
	যদি (map_to != রিক্ত)
	{
	    en_name = name;
	    name = map_to;
	}
	অন্যথা
	    en_name = রিক্ত;
#endif
	dname = menu_text(name, রিক্ত, রিক্ত);
	যদি (dname == রিক্ত)
	    গচ্ছ erret;
	যদি (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    গচ্ছ erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	যেতিয়ালৈ (menu != রিক্ত)
	{
	    যদি (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		যদি (*next_name == NUL && menu->children != রিক্ত)
		{
		    যদি (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    গচ্ছ erret;
		}
		যদি (*next_name != NUL && menu->children == রিক্ত
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    যদি (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    গচ্ছ erret;
		}
		ভঙ্গ;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    যদি (parent != রিক্ত || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		যদি (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	যদি (menu == রিক্ত)
	{
	    যদি (*next_name == NUL && parent == রিক্ত)
	    {
		emsg(_(e_must_not_add_menu_বস্তুসমূহ_directly_to_menu_bar));
		গচ্ছ erret;
	    }

	    যদি (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		গচ্ছ erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    যদি (menu == রিক্ত)
		গচ্ছ erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    যদি (en_name != রিক্ত)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, রিক্ত, রিক্ত);
	    }
	    অন্যথা
	    {
		menu->en_name = রিক্ত;
		menu->en_dname = রিক্ত;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = রিক্ত;
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
	    যদি (*next_name == NUL && menuarg->iconfile != রিক্ত)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    যদি (parent != রিক্ত && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	অন্যথা
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    যদি (addtearoff)
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
	যদি ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    যদি (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		যদি (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		অন্যথা
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    যদি (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != রিক্ত
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
		যদি (tearpath != রিক্ত)
		{
		    char_u  *s;
		    পূর্ণাঙ্ক	    len;

		    STRCPY(tearpath, menu_path);
		    len = (পূর্ণাঙ্ক)(next_name - path_name - 1);
		    বাবে (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			যদি ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	যদি (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_ঢোকাও_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_ঢোকাও_MODE));
    যদি (sys_menu)
	modes &= ~old_modes;

    যদি (menu != রিক্ত && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == রিক্ত) ? রিক্ত : vim_strsave(call_data);

	// loop over all modes, may add more than one
	বাবে (i = 0; i < MENU_MODES; ++i)
	{
	    যদি (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		যদি (amenu && call_data != রিক্ত && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    ৱিকল্প (1 << i)
		    {
			পক্ষ MENU_VISUAL_MODE:
			পক্ষ MENU_SELECT_MODE:
			পক্ষ MENU_OP_PENDING_MODE:
			পক্ষ MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    ভঙ্গ;
			পক্ষ MENU_ঢোকাও_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    ভঙ্গ;
		    }
		}

		যদি (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    যদি (menu->strings[i] != রিক্ত)
		    {
			menu->strings[i][0] = c;
			যদি (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			অন্যথা
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			যদি (c == Ctrl_C)
			{
			    পূর্ণাঙ্ক	    len = (পূর্ণাঙ্ক)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		অন্যথা
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	যদি (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    ঘূৰাই OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    যেতিয়ালৈ (parent != রিক্ত && parent->children == রিক্ত)
    {
	যদি (parent->parent == রিক্ত)
	    menup = মূল_menu_ptr;
	অন্যথা
	    menup = &parent->parent->children;
	বাবে ( ; *menup != রিক্ত && *menup != parent; menup = &((*menup)->next))
	    ;
	যদি (*menup == রিক্ত) // safety check
	    ভঙ্গ;
	parent = parent->parent;
	free_menu(menup);
    }
    ঘূৰাই FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    স্থির পূর্ণাঙ্ক
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    পূর্ণাঙ্ক		modes,
    পূর্ণাঙ্ক		enable)
{
    char_u	*p;

    যদি (menu == রিক্ত)
	ঘূৰাই OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    যেতিয়ালৈ (menu != রিক্ত)
    {
	যদি (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    যদি (*p != NUL)
	    {
		যদি (menu->children == রিক্ত)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    ঘূৰাই FAIL;
		}
		যদি (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    ঘূৰাই FAIL;
	    }
	    অন্যথা
		যদি (enable)
		    menu->enabled |= modes;
		অন্যথা
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    যদি (*name != NUL && *name != '*')
		ভঙ্গ;
	}
	menu = menu->next;
    }
    যদি (*name != NUL && *name != '*' && menu == রিক্ত)
    {
	semsg(_(e_no_menu_str), name);
	ঘূৰাই FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    ঘূৰাই OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    স্থির পূর্ণাঙ্ক
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    পূর্ণাঙ্ক		modes,
    পূর্ণাঙ্ক		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    যদি (*menup == রিক্ত)
	ঘূৰাই OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    যেতিয়ালৈ ((menu = *menup) != রিক্ত)
    {
	যদি (*name == NUL || menu_name_equal(name, menu))
	{
	    যদি (*p != NUL && menu->children == রিক্ত)
	    {
		যদি (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		ঘূৰাই FAIL;
	    }
	    যদি ((menu->modes & modes) != 0x0)
	    {
#যদি defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		যদি (*p == NUL && modes == MENU_ALL_MODES)
		{
		    যদি (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		যদি (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    ঘূৰাই FAIL;
	    }
	    অন্যথা যদি (*name != NUL)
	    {
		যদি (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		ঘূৰাই FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    যদি (*name != NUL)
		ভঙ্গ;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    যদি (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    যদি ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    অন্যথা
		menup = &menu->next;
	}
	অন্যথা
	    menup = &menu->next;
    }
    যদি (*name != NUL)
    {
	যদি (menu == রিক্ত)
	{
	    যদি (!silent)
		semsg(_(e_no_menu_str), name);
	    ঘূৰাই FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#যদি defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	যদি ((s_tearoffs) && (menu->children != রিক্ত)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	অন্যথা
#endif
	    child = menu->children;
	বাবে ( ; child != রিক্ত; child = child->next)
	    menu->modes |= child->modes;
	যদি (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    যদি (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	যদি ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#যদি defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    যদি (s_tearoffs && menu->children != রিক্ত) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    ঘূৰাই OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    শূন্য
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_বস্তুসমূহ);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    স্থির শূন্য
free_menu(vimmenu_T **menup)
{
    পূর্ণাঙ্ক		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    যদি (gui.in_use)
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
    বাবে (i = 0; i < MENU_MODES; i++)
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
    স্থির শূন্য
free_menu_string(vimmenu_T *menu, পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক		গুনতি = 0;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < MENU_MODES; i++)
	যদি (menu->strings[i] == menu->strings[idx])
	    গুনতি++;
    যদি (গুনতি == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = রিক্ত;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    স্থির পূর্ণাঙ্ক
show_menus(char_u *path_name, পূর্ণাঙ্ক modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = রিক্ত;

    name = path_name = vim_strsave(path_name);
    যদি (path_name == রিক্ত)
	ঘূৰাই FAIL;
    menu = *get_মূল_menu(path_name);

    // First, find the (sub)menu with the given name
    যেতিয়ালৈ (*name)
    {
	p = menu_name_skip(name);
	যেতিয়ালৈ (menu != রিক্ত)
	{
	    যদি (menu_name_equal(name, menu))
	    {
		// Found menu
		যদি (*p != NUL && menu->children == রিক্ত)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    ঘূৰাই FAIL;
		}
		অন্যথা যদি ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    ঘূৰাই FAIL;
		}
		ভঙ্গ;
	    }
	    menu = menu->next;
	}
	যদি (menu == রিক্ত)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    ঘূৰাই FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    বাৰ্তা_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    ঘূৰাই OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    স্থির শূন্য
show_menus_recursive(vimmenu_T *menu, পূর্ণাঙ্ক modes, পূর্ণাঙ্ক depth)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		bit;

    যদি (menu != রিক্ত && (menu->modes & modes) == 0x0)
	ঘূৰাই;

    যদি (menu != রিক্ত)
    {
	বাৰ্তা_putchar('\n');
	যদি (got_int)		// "q" hit for "--more--"
	    ঘূৰাই;
	বাবে (i = 0; i < depth; i++)
	    বাৰ্তা_puts("  ");
	যদি (menu->priority)
	{
	    বাৰ্তা_outnum((long)menu->priority);
	    বাৰ্তা_puts(" ");
	}
				// Same highlighting as for directories!?
	বাৰ্তা_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    যদি (menu != রিক্ত && menu->children == রিক্ত)
    {
	বাবে (bit = 0; bit < MENU_MODES; bit++)
	    যদি ((menu->modes & modes & (1 << bit)) != 0)
	    {
		বাৰ্তা_putchar('\n');
		যদি (got_int)		// "q" hit for "--more--"
		    ঘূৰাই;
		বাবে (i = 0; i < depth + 2; i++)
		    বাৰ্তা_puts("  ");
		বাৰ্তা_puts(menu_mode_chars[bit]);
		যদি (menu->noremap[bit] == REMAP_NONE)
		    বাৰ্তা_putchar('*');
		অন্যথা যদি (menu->noremap[bit] == REMAP_SCRIPT)
		    বাৰ্তা_putchar('&');
		অন্যথা
		    বাৰ্তা_putchar(' ');
		যদি (menu->silent[bit])
		    বাৰ্তা_putchar('s');
		অন্যথা
		    বাৰ্তা_putchar(' ');
		যদি ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    বাৰ্তা_putchar('-');
		অন্যথা
		    বাৰ্তা_putchar(' ');
		বাৰ্তা_puts(" ");
		যদি (*menu->strings[bit] == NUL)
		    বাৰ্তা_puts_attr("<Nop>", HL_ATTR(HLF_8));
		অন্যথা
		    বাৰ্তা_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    অন্যথা
    {
	যদি (menu == রিক্ত)
	{
	    menu = মূল_menu;
	    depth--;
	}
	অন্যথা
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	বাবে (; menu != রিক্ত && !got_int; menu = menu->next)
	    যদি (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
স্থির vimmenu_T	*expand_menu = রিক্ত;
স্থির vimmenu_T	*expand_menu_alt = রিক্ত;
স্থির পূর্ণাঙ্ক		expand_modes = 0x0;
স্থির পূর্ণাঙ্ক		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    পূর্ণাঙ্ক		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = রিক্ত;
    char_u	*name;
    পূর্ণাঙ্ক		unmenu;
    vimmenu_T	*menu;
    পূর্ণাঙ্ক		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    বাবে (p = arg; *p; ++p)
	যদি (!VIM_ISDIGIT(*p) && *p != '.')
	    ভঙ্গ;

    যদি (!VIM_ISWHITE(*p))
    {
	যদি (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	অন্যথা যদি (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	অন্যথা
	    p = arg;
    }

    যেতিয়ালৈ (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    বাবে (; *p && !VIM_ISWHITE(*p); ++p)
    {
	যদি ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	অন্যথা যদি (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    যদি (expand_menus && VIM_ISWHITE(*p))
	ঘূৰাই রিক্ত;	// TODO: check for next command?
    যদি (*p == NUL)		// Complete the menu name
    {
	পূর্ণাঙ্ক try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, রিক্ত, &unmenu);
	যদি (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = মূল_menu;
	যদি (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    যদি (path_name == রিক্ত)
		ঘূৰাই রিক্ত;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	যেতিয়ালৈ (name != রিক্ত && *name)
	{
	    p = menu_name_skip(name);
	    যেতিয়ালৈ (menu != রিক্ত)
	    {
		যদি (menu_name_equal(name, menu))
		{
		    // Found menu
		    যদি ((*p != NUL && menu->children == রিক্ত)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			ঘূৰাই রিক্ত;
		    }
		    ভঙ্গ;
		}
		menu = menu->next;
		যদি (menu == রিক্ত && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    যদি (menu == রিক্ত)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		ঘূৰাই রিক্ত;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	যদি (expand_menu == মূল_menu)
	    expand_menu_alt = curwin->w_winbar;
	অন্যথা
	    expand_menu_alt = রিক্ত;
    }
    অন্যথা			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    ঘূৰাই রিক্ত;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    স্থির vimmenu_T	*menu = রিক্ত;
    স্থির পূর্ণাঙ্ক		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    স্থির  পূর্ণাঙ্ক		should_advance = FALSE;
#endif

    যদি (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    যেতিয়ালৈ (menu != রিক্ত && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == রিক্ত))
    {
	menu = menu->next;
	যদি (menu == রিক্ত && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    যদি (menu == রিক্ত)	    // at end of linked list
	ঘূৰাই রিক্ত;

    যদি (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	যদি (should_advance)
	    str = menu->en_dname;
	অন্যথা
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    যদি (menu->en_dname == রিক্ত)
		should_advance = TRUE;
	}
#endif
    অন্যথা
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    যদি (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	যদি (menu == রিক্ত && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ঘূৰাই str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    স্থির vimmenu_T	*menu = রিক্ত;
    স্থির পূর্ণাঙ্ক		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    স্থির char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    স্থির  পূর্ণাঙ্ক		should_advance = FALSE;
#endif

    যদি (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    যেতিয়ালৈ (menu != রিক্ত
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	যদি (menu == রিক্ত && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    যদি (menu == রিক্ত)	    // at end of linked list
	ঘূৰাই রিক্ত;

    যদি (menu->modes & expand_modes)
    {
	যদি (menu->children != রিক্ত)
	{
#ifdef FEAT_MULTI_LANG
	    যদি (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    অন্যথা
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		যদি (menu->en_dname == রিক্ত)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	অন্যথা
#ifdef FEAT_MULTI_LANG
	{
	    যদি (should_advance)
		str = menu->en_dname;
	    অন্যথা
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		যদি (menu->en_dname == রিক্ত)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    অন্যথা
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    যদি (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	যদি (menu == রিক্ত && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ঘূৰাই str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    স্থির char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    বাবে (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    যদি (*p == NUL)
		ভঙ্গ;
	}
    }
    যদি (*p)
	*p++ = NUL;
    ঘূৰাই p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    স্থির পূর্ণাঙ্ক
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    যদি (menu->en_name != রিক্ত
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	ঘূৰাই TRUE;
#endif
    ঘূৰাই menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    স্থির পূর্ণাঙ্ক
menu_namecmp(char_u *name, char_u *mname)
{
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	যদি (name[i] != mname[i])
	    ভঙ্গ;
    ঘূৰাই ((name[i] == NUL || name[i] == TAB)
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
    স্থির পূর্ণাঙ্ক
get_menu_cmd_modes(
    char_u  *cmd,
    পূর্ণাঙ্ক	    forceit,	    // Was there a "!" after the command?
    পূর্ণাঙ্ক	    *noremap,
    পূর্ণাঙ্ক	    *unmenu)
{
    পূর্ণাঙ্ক	    modes;

    ৱিকল্প (*cmd++)
    {
	পক্ষ 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    ভঙ্গ;
	পক্ষ 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    ভঙ্গ;
	পক্ষ 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    ভঙ্গ;
	পক্ষ 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    ভঙ্গ;
	পক্ষ 'i':			// imenu
	    modes = MENU_ঢোকাও_MODE;
	    ভঙ্গ;
	পক্ষ 't':
	    যদি (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		ভঙ্গ;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    ভঙ্গ;
	পক্ষ 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    ভঙ্গ;
	পক্ষ 'a':			// amenu
	    modes = MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    ভঙ্গ;
	পক্ষ 'n':
	    যদি (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		ভঙ্গ;
	    }
	    // FALLTHROUGH
	স্ৱভাৱ:
	    --cmd;
	    যদি (forceit)		// menu!!
		modes = MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE;
	    অন্যথা			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    যদি (noremap != রিক্ত)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    যদি (unmenu != রিক্ত)
	*unmenu = (*cmd == 'u');
    ঘূৰাই modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    স্থির char_u *
get_menu_mode_str(পূর্ণাঙ্ক modes)
{
    যদি ((modes & (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	ঘূৰাই (char_u *)"a";
    যদি ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	ঘূৰাই (char_u *)" ";
    যদি ((modes & (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE))
	    == (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE))
	ঘূৰাই (char_u *)"!";
    যদি ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	ঘূৰাই (char_u *)"v";
    যদি (modes & MENU_VISUAL_MODE)
	ঘূৰাই (char_u *)"x";
    যদি (modes & MENU_SELECT_MODE)
	ঘূৰাই (char_u *)"s";
    যদি (modes & MENU_OP_PENDING_MODE)
	ঘূৰাই (char_u *)"o";
    যদি (modes & MENU_ঢোকাও_MODE)
	ঘূৰাই (char_u *)"i";
    যদি (modes & MENU_TERMINAL_MODE)
	ঘূৰাই (char_u *)"tl";
    যদি (modes & MENU_CMDLINE_MODE)
	ঘূৰাই (char_u *)"c";
    যদি (modes & MENU_NORMAL_MODE)
	ঘূৰাই (char_u *)"n";
    যদি (modes & MENU_TIP_MODE)
	ঘূৰাই (char_u *)"t";

    ঘূৰাই (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    স্থির char_u *
popup_mode_name(char_u *name, পূর্ণাঙ্ক idx)
{
    char_u	*p;
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)STRLEN(name);
    ৱর্ণ	*mode_chars = menu_mode_chars[idx];
    পূর্ণাঙ্ক		mode_chars_len = (পূর্ণাঙ্ক)strlen(mode_chars);
    পূর্ণাঙ্ক		i;

    p = vim_strnsave(name, len + mode_chars_len);
    যদি (p == রিক্ত)
	ঘূৰাই রিক্ত;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    বাবে (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    ঘূৰাই p;
}

#যদি defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    পূর্ণাঙ্ক
get_menu_index(vimmenu_T *menu, পূর্ণাঙ্ক state)
{
    পূর্ণাঙ্ক		idx;

    যদি ((state & MODE_ঢোকাও))
	idx = MENU_INDEX_ঢোকাও;
    অন্যথা যদি (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    অন্যথা যদি (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    অন্যথা যদি (VIsual_active)
    {
	যদি (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	অন্যথা
	    idx = MENU_INDEX_VISUAL;
    }
    অন্যথা যদি (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    অন্যথা যদি (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    অন্যথা যদি ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    অন্যথা
	idx = MENU_INDEX_INVALID;

    যদি (idx != MENU_INDEX_INVALID && menu->strings[idx] == রিক্ত)
	idx = MENU_INDEX_INVALID;
    ঘূৰাই idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    স্থির char_u *
menu_text(char_u *str, পূর্ণাঙ্ক *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    যদি (p != রিক্ত)
    {
	যদি (actext != রিক্ত)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    অন্যথা
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    বাবে (p = text; p != রিক্ত; )
    {
	p = vim_strchr(p, '&');
	যদি (p != রিক্ত)
	{
	    যদি (p[1] == NUL)	    // trailing "&"
		ভঙ্গ;
	    যদি (mnemonic != রিক্ত && p[1] != '&')
#যদি !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#অন্যথা
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		ৱর্ণ c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    ঘূৰাই text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    পূর্ণাঙ্ক
menu_is_menubar(char_u *name)
{
    ঘূৰাই (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    পূর্ণাঙ্ক
menu_is_popup(char_u *name)
{
    ঘূৰাই (STRNCMP(name, "PopUp", 5) == 0);
}

#যদি defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    পূর্ণাঙ্ক
menu_is_child_of_popup(vimmenu_T *menu)
{
    যেতিয়ালৈ (menu->parent != রিক্ত)
	menu = menu->parent;
    ঘূৰাই menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    পূর্ণাঙ্ক
menu_is_toolbar(char_u *name)
{
    ঘূৰাই (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    পূর্ণাঙ্ক
menu_is_separator(char_u *name)
{
    ঘূৰাই (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    স্থির পূর্ণাঙ্ক
menu_is_hidden(char_u *name)
{
    ঘূৰাই (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    স্থির পূর্ণাঙ্ক
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    ঘূৰাই (STRCMP(name, TEAR_STRING) == 0);
#অন্যথা
    ঘূৰাই FALSE;
#endif
}

#যদি defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    স্থির পূর্ণাঙ্ক
get_menu_mode(শূন্য)
{
# ifdef FEAT_TERMINAL
    যদি (term_use_loop())
	ঘূৰাই MENU_INDEX_TERMINAL;
# endif
    যদি (VIsual_active)
    {
	যদি (VIsual_select)
	    ঘূৰাই MENU_INDEX_SELECT;
	ঘূৰাই MENU_INDEX_VISUAL;
    }
    যদি (State & MODE_ঢোকাও)
	ঘূৰাই MENU_INDEX_ঢোকাও;
    যদি ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	ঘূৰাই MENU_INDEX_CMDLINE;
    যদি (finish_op)
	ঘূৰাই MENU_INDEX_OP_PENDING;
    যদি (State & MODE_NORMAL)
	ঘূৰাই MENU_INDEX_NORMAL;
    যদি (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	ঘূৰাই MENU_INDEX_ঢোকাও;
    ঘূৰাই MENU_INDEX_INVALID;
}

    পূর্ণাঙ্ক
get_menu_mode_flag(শূন্য)
{
    পূর্ণাঙ্ক mode = get_menu_mode();

    যদি (mode == MENU_INDEX_INVALID)
	ঘূৰাই 0;
    ঘূৰাই 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    শূন্য
show_popupmenu(শূন্য)
{
    vimmenu_T	*menu;
    পূর্ণাঙ্ক		menu_mode;
    ৱর্ণ*	mode;
    পূর্ণাঙ্ক		mode_len;

    menu_mode = get_menu_mode();
    যদি (menu_mode == MENU_INDEX_INVALID)
	ঘূৰাই;
    mode = menu_mode_chars[menu_mode];
    mode_len = (পূর্ণাঙ্ক)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, রিক্ত, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	যদি (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    ভঙ্গ;

    // Only show a popup when it is defined and has entries
    যদি (menu == রিক্ত || menu->children == রিক্ত)
	ঘূৰাই;

# যদি defined(FEAT_GUI)
    যদি (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# যদি defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    অন্যথা
# endif
# যদি defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#যদি defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    পূর্ণাঙ্ক
check_menu_pointer(vimmenu_T *মূল, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    বাবে (p = মূল; p != রিক্ত; p = p->next)
	যদি (p == menu_to_check
		|| (p->children != রিক্ত
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    ঘূৰাই OK;
    ঘূৰাই FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    শূন্য
gui_create_initial_menus(vimmenu_T *menu)
{
    পূর্ণাঙ্ক		idx = 0;

    যেতিয়ালৈ (menu != রিক্ত)
    {
	// Don't add a menu when only a tip was defined.
	যদি (menu->modes & MENU_ALL_MODES)
	{
	    যদি (menu->children != রিক্ত)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    অন্যথা
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    স্থির শূন্য
gui_update_menus_recurse(vimmenu_T *menu, পূর্ণাঙ্ক mode)
{
    পূর্ণাঙ্ক		grey;

    যেতিয়ালৈ (menu)
    {
	যদি ((menu->modes & menu->enabled & mode)
# যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	অন্যথা
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	যদি (vim_strchr(p_go, GO_GREY) != রিক্ত || menu->parent == রিক্ত
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	অন্যথা
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
    শূন্য
gui_update_menus(পূর্ণাঙ্ক modes)
{
    স্থির পূর্ণাঙ্ক	    prev_mode = -1;
    পূর্ণাঙ্ক		    mode = 0;

    যদি (modes != 0x0)
	mode = modes;
    অন্যথা
	mode = get_menu_mode_flag();

    যদি (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(মূল_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    পূর্ণাঙ্ক
gui_is_menu_shortcut(পূর্ণাঙ্ক চাবি)
{
    vimmenu_T	*menu;

    যদি (চাবি < 256)
	চাবি = TOLOWER_LOC(চাবি);
    FOR_ALL_MENUS(menu)
	যদি (menu->mnemonic == চাবি
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == চাবি))
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}
# endif
#endif // FEAT_GUI

#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    শূন্য
gui_mch_toggle_tearoffs(পূর্ণাঙ্ক enable)
{
    পূর্ণাঙ্ক		pri_tab[MENUDEPTH + 1];
    পূর্ণাঙ্ক		i;

    যদি (enable)
    {
	বাবে (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(মূল_menu, (char_u *)"", pri_tab, 0);
    }
    অন্যথা
	gui_destroy_tearoffs_recurse(মূল_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    স্থির শূন্য
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ধ্রুৱ char_u	*pname,
    পূর্ণাঙ্ক			*pri_tab,
    পূর্ণাঙ্ক			pri_idx)
{
    char_u	*newpname = রিক্ত;
    পূর্ণাঙ্ক		len;
    char_u	*s;
    char_u	*d;

    যদি (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    যেতিয়ালৈ (menu != রিক্ত)
    {
	যদি (menu->children != রিক্ত && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (পূর্ণাঙ্ক)STRLEN(pname) + (পূর্ণাঙ্ক)STRLEN(menu->name);
	    বাবে (s = menu->name; *s; ++s)
		যদি (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    যদি (newpname != রিক্ত)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		বাবে (s = menu->name; *s; ++s)
		{
		    যদি (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		যদি (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    স্থির শূন্য
gui_add_tearoff(char_u *tearpath, পূর্ণাঙ্ক *pri_tab, পূর্ণাঙ্ক pri_idx)
{
    char_u	*tbuf;
    পূর্ণাঙ্ক		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned পূর্ণাঙ্ক)STRLEN(tearpath));
    যদি (tbuf == রিক্ত)
	ঘূৰাই;

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
    menuarg.iconfile = রিক্ত;
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
    স্থির শূন্য
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    যেতিয়ালৈ (menu)
    {
	যদি (menu->children)
	{
	    // check if tearoff exists
	    যদি (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    যদি (menu->children != রিক্ত) // if not the last one
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
    শূন্য
execute_menu(exarg_T *eap, vimmenu_T *menu, পূর্ণাঙ্ক mode_idx)
{
    পূর্ণাঙ্ক		idx = mode_idx;

    যদি (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	যদি (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_ঢোকাও;
	}
#ifdef FEAT_TERMINAL
	অন্যথা যদি (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	অন্যথা যদি (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	অন্যথা যদি (eap != রিক্ত && eap->addr_গুনতি)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    যদি ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    অন্যথা
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
	    যদি (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    যদি (idx == MENU_INDEX_INVALID || eap == রিক্ত)
	idx = MENU_INDEX_NORMAL;

    যদি (menu->strings[idx] != রিক্ত && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	যদি (eap == রিক্ত || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    যদি (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	অন্যথা
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    অন্যথা যদি (eap != রিক্ত)
    {
	char_u	*mode;

	ৱিকল্প (idx)
	{
	    পক্ষ MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		ভঙ্গ;
	    পক্ষ MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		ভঙ্গ;
	    পক্ষ MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		ভঙ্গ;
	    পক্ষ MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		ভঙ্গ;
	    পক্ষ MENU_INDEX_ঢোকাও:
		mode = (char_u *)"Insert";
		ভঙ্গ;
	    পক্ষ MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		ভঙ্গ;
	    // case MENU_INDEX_TIP: cannot happen
	    স্ৱভাৱ:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    স্থির vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    পূর্ণাঙ্ক		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    যদি (saved_name == রিক্ত)
	ঘূৰাই রিক্ত;

    menu = *get_মূল_menu(saved_name);
    name = saved_name;
    যেতিয়ালৈ (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	যেতিয়ালৈ (menu != রিক্ত)
	{
	    যদি (menu_name_equal(name, menu))
	    {
		যদি (*p == NUL && menu->children != রিক্ত)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = রিক্ত;
		}
		অন্যথা যদি (*p != NUL && menu->children == রিক্ত)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = রিক্ত;
		}
		ভঙ্গ;
	    }
	    menu = menu->next;
	}
	যদি (menu == রিক্ত || *p == NUL)
	    ভঙ্গ;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    যদি (menu == রিক্ত)
    {
	যদি (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    শূন্য
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		mode_idx = MENU_INDEX_INVALID;

    যদি (arg[0] && VIM_ISWHITE(arg[1]))
    {
	ৱিকল্প (arg[0])
	{
	    পক্ষ 'n': mode_idx = MENU_INDEX_NORMAL; ভঙ্গ;
	    পক্ষ 'v': mode_idx = MENU_INDEX_VISUAL; ভঙ্গ;
	    পক্ষ 's': mode_idx = MENU_INDEX_SELECT; ভঙ্গ;
	    পক্ষ 'o': mode_idx = MENU_INDEX_OP_PENDING; ভঙ্গ;
	    পক্ষ 't': mode_idx = MENU_INDEX_TERMINAL; ভঙ্গ;
	    পক্ষ 'i': mode_idx = MENU_INDEX_ঢোকাও; ভঙ্গ;
	    পক্ষ 'c': mode_idx = MENU_INDEX_CMDLINE; ভঙ্গ;
	    স্ৱভাৱ: semsg(_(e_invalid_argument_str), arg);
		     ঘূৰাই;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    যদি (menu == রিক্ত)
	ঘূৰাই;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    শূন্য
winbar_click(win_T *wp, পূর্ণাঙ্ক col)
{
    পূর্ণাঙ্ক		idx;

    যদি (wp->w_winbar_বস্তুসমূহ == রিক্ত)
	ঘূৰাই;
    বাবে (idx = 0; wp->w_winbar_বস্তুসমূহ[idx].wb_menu != রিক্ত; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_বস্তুসমূহ[idx];

	যদি (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = রিক্ত;
	    pos_T   save_visual = VIsual;
	    পূর্ণাঙ্ক	    save_visual_active = VIsual_active;
	    পূর্ণাঙ্ক	    save_visual_select = VIsual_select;
	    পূর্ণাঙ্ক	    save_visual_reselect = VIsual_reselect;
	    পূর্ণাঙ্ক	    save_visual_mode = VIsual_mode;

	    যদি (wp != curwin)
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
	    execute_menu(রিক্ত, item->wb_menu, -1);

	    যদি (save_curwin != রিক্ত && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    যদি (!win_valid(wp))
		ভঙ্গ;
	}
    }
}

#যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = রিক্ত;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_মূল_menu(path_name);

    saved_name = vim_strsave(path_name);
    যদি (saved_name == রিক্ত)
	ঘূৰাই রিক্ত;

    name = saved_name;
    যেতিয়ালৈ (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	যেতিয়ালৈ (menu != রিক্ত)
	{
	    যদি (menu_name_equal(name, menu))
	    {
		যদি (menu->children == রিক্ত)
		{
		    // found a menu item instead of a sub-menu
		    যদি (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    অন্যথা
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = রিক্ত;
		    গচ্ছ theend;
		}
		যদি (*p == NUL)	    // found a full match
		    গচ্ছ theend;
		ভঙ্গ;
	    }
	    menu = menu->next;
	}
	যদি (menu == রিক্ত)	// didn't find it
	    ভঙ্গ;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    যদি (menu == রিক্ত)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    ঘূৰাই menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

প্রকারনির্দেশ সংরচনা
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

স্থির garray_T menutrans_ga = {0, 0, 0, 0, রিক্ত};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    শূন্য
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    পূর্ণাঙ্ক			i;
    char_u		*from, *from_noamp, *to;

    যদি (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, পরিমাণ(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    যদি (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	বাবে (i = 0; i < menutrans_ga.ga_len; ++i)
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
    অন্যথা
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	যদি (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	অন্যথা
	{
	    যদি (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		যদি (from != রিক্ত)
		{
		    from_noamp = menu_text(from, রিক্ত, রিক্ত);
		    to = vim_strnsave(to, arg - to);
		    যদি (from_noamp != রিক্ত && to != রিক্ত)
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
		    অন্যথা
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

#যদি defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    স্থির char_u *
menu_skip_part(char_u *p)
{
    যেতিয়ালৈ (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	যদি ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    ঘূৰাই p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    স্থির char_u *
menutrans_lookup(char_u *name, পূর্ণাঙ্ক len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    পূর্ণাঙ্ক			i;
    char_u		*dname;

    বাবে (i = 0; i < menutrans_ga.ga_len; ++i)
	যদি (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    ঘূৰাই tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, রিক্ত, রিক্ত);
    name[len] = i;
    যদি (dname == রিক্ত)
	ঘূৰাই রিক্ত;

    বাবে (i = 0; i < menutrans_ga.ga_len; ++i)
	যদি (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    ঘূৰাই tp[i].to;
	}
    vim_free(dname);

    ঘূৰাই রিক্ত;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    স্থির শূন্য
menu_unescape_name(char_u *name)
{
    char_u  *p;

    বাবে (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	যদি (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    স্থির char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    যেতিয়ালৈ (*arg && !VIM_ISWHITE(*arg))
    {
	যদি ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	অন্যথা যদি (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    যদি (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    ঘূৰাই arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    স্থির পূর্ণাঙ্ক
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, পূর্ণাঙ্ক modes, dict_T *dict)
{
    পূর্ণাঙ্ক		status;
    list_T	*l;

    যদি (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	যদি (l == রিক্ত)
	    ঘূৰাই FAIL;

	যদি (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ঘূৰাই FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	বাবে (topmenu = menu; topmenu != রিক্ত; topmenu = topmenu->next)
	    যদি (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	ঘূৰাই OK;
    }

    যদি (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	ঘূৰাই OK;

    status = dict_add_string(dict, "name", menu->name);
    যদি (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    যদি (status == OK && menu->actext != রিক্ত)
	status = dict_add_string(dict, "accel", menu->actext);
    যদি (status == OK)
	status = dict_add_সংখ্যা(dict, "priority", menu->priority);
    যদি (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    যদি (status == OK && menu->iconfile != রিক্ত)
	status = dict_add_string(dict, "icon", menu->iconfile);
    যদি (status == OK && menu->iconidx >= 0)
	status = dict_add_সংখ্যা(dict, "iconidx", menu->iconidx);
#endif
    যদি (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	পূর্ণাঙ্ক	buflen;

	যদি (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	অন্যথা
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    যদি (status == OK && menu->children == রিক্ত)
    {
	পূর্ণাঙ্ক		bit;

	// Get the first mode in which the menu is available
	বাবে (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	যদি (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    যদি (menu->strings[bit] != রিক্ত)
	    {
		যদি (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		অন্যথা
		{
		    char_u *tofree = রিক্ত;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    যদি (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    যদি (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    যদি (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    যদি (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    যদি (status == OK && menu->children != রিক্ত)
    {
	vimmenu_T	*child;

	l = list_alloc();
	যদি (l == রিক্ত)
	    ঘূৰাই FAIL;

	যদি (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ঘূৰাই FAIL;
	}
	child = menu->children;
	যেতিয়ালৈ (child)
	{
	    যদি (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    ঘূৰাই status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    শূন্য
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    পূর্ণাঙ্ক		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ঘূৰাই;
    retdict = rettv->vval.v_dict;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    menu_name = tv_get_string_chk(&argvars[0]);
    যদি (menu_name == রিক্ত)
	ঘূৰাই;

    // menu mode
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    অন্যথা
	which = (char_u *)"";	    // Default is modes for "menu"
    যদি (which == রিক্ত)
	ঘূৰাই;

    modes = get_menu_cmd_modes(which, *which == '!', রিক্ত, রিক্ত);

    // Locate the specified menu or menu item
    menu = *get_মূল_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    যদি (saved_name == রিক্ত)
	ঘূৰাই;
    যদি (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	যেতিয়ালৈ (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    যেতিয়ালৈ (menu != রিক্ত)
	    {
		যদি (menu_name_equal(name, menu))
		    ভঙ্গ;
		menu = menu->next;
	    }
	    যদি (menu == রিক্ত || *p == NUL)
		ভঙ্গ;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    যদি (menu == রিক্ত)		// specified menu not found
	ঘূৰাই;

    যদি (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
