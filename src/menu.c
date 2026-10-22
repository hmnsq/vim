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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
நிலையான முழு add_menu_path(char_u *, vimmenu_T *, முழு *, char_u *, முழு);
#இல்லையெனில்
நிலையான முழு add_menu_path(char_u *, vimmenu_T *, முழு *, char_u *);
#endif
நிலையான முழு menu_nable_recurse(vimmenu_T *menu, char_u *name, முழு modes, முழு enable);
நிலையான முழு remove_menu(vimmenu_T **, char_u *, முழு, முழு silent);
நிலையான வெறுமை free_menu(vimmenu_T **menup);
நிலையான வெறுமை free_menu_string(vimmenu_T *, முழு);
நிலையான முழு show_menus(char_u *, முழு);
நிலையான வெறுமை show_menus_recursive(vimmenu_T *, முழு, முழு);
நிலையான char_u *menu_name_skip(char_u *name);
நிலையான முழு menu_name_equal(char_u *name, vimmenu_T *menu);
நிலையான முழு menu_namecmp(char_u *name, char_u *mname);
நிலையான முழு get_menu_cmd_modes(char_u *, முழு, முழு *, முழு *);
நிலையான char_u *popup_mode_name(char_u *name, முழு idx);
நிலையான char_u *menu_text(char_u *text, முழு *mnemonic, char_u **actext);

#எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
நிலையான வெறுமை gui_create_tearoffs_recurse(vimmenu_T *menu, மாறிலி char_u *pname, முழு *pri_tab, முழு pri_idx);
நிலையான வெறுமை gui_add_tearoff(char_u *tearpath, முழு *pri_tab, முழு pri_idx);
நிலையான வெறுமை gui_destroy_tearoffs_recurse(vimmenu_T *menu);
நிலையான முழு s_tearoffs = FALSE;
#endif

நிலையான முழு menu_is_hidden(char_u *name);
நிலையான முழு menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
நிலையான முழு menus_locked = 0;

#எனில் defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
நிலையான char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
நிலையான char_u *menutrans_lookup(char_u *name, முழு len);
நிலையான வெறுமை menu_unescape_name(char_u	*p);
#endif

நிலையான char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
நிலையான எழுத்து *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
நிலையான மாறிலி எழுத்து *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_மொத்தம் ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    நிலையான முழு
menu_is_winbar(char_u *name)
{
    திரும்பு (STRNCMP(name, "WinBar", 6) == 0);
}

    முழு
winbar_height(win_T *wp)
{
    எனில் (wp->w_winbar != வெற்று && wp->w_winbar->children != வெற்று)
	திரும்பு 1;
    திரும்பு 0;
}

    நிலையான vimmenu_T **
get_வேர்_menu(char_u *name)
{
    எனில் (menu_is_winbar(name))
	திரும்பு &curwin->w_winbar;
    திரும்பு &வேர்_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    நிலையான முழு
is_menus_locked(வெறுமை)
{
    எனில் (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    வெறுமை
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    முழு		modes;
    char_u	*map_to;
    முழு		noremap;
    முழு		silent = FALSE;
    முழு		special = FALSE;
    முழு		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    முழு		i;
#எனில் defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    முழு		old_menu_height;
# எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    முழு		old_toolbar_height;
# endif
#endif
    முழு		pri_tab[MENUDEPTH + 1];
    முழு		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = வெற்று;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**வேர்_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    க்கு (;;)
    {
	எனில் (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    தொடர்;
	}
	எனில் (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    தொடர்;
	}
	எனில் (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    தொடர்;
	}
	நிறுத்து;
    }


    // Locate an optional "icon=filename" argument.
    எனில் (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	போது (*arg != NUL && *arg != ' ')
	{
	    எனில் (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	எனில் (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    க்கு (p = arg; *p; ++p)
	எனில் (!VIM_ISDIGIT(*p) && *p != '.')
	    நிறுத்து;
    எனில் (VIM_ISWHITE(*p))
    {
	க்கு (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    எனில் (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    எனில் (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    இல்லையெனில் எனில் (eap->addr_மொத்தம் && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    இல்லையெனில்
	i = 0;
    போது (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    எனில் (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    இல்லையெனில் எனில் (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    எனில் (*arg == NUL)
    {
	show_menus(arg, modes);
	திரும்பு;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    எனில் (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	எனில் (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    எனில் (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		எனில் (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((எழுத்து *)menu_path + 7);
		    எனில் (menuarg.iconidx >= (முழு)TOOLBAR_NAME_மொத்தம்)
			menuarg.iconidx = -1;
		    இல்லையெனில்
			menuarg.icon_builtin = TRUE;
		}
	    }
	    இல்லையெனில்
	    {
		க்கு (i = 0; i < (முழு)TOOLBAR_NAME_மொத்தம்; ++i)
		    எனில் (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			நிறுத்து;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    எனில் (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	செல் theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    எனில் (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	செல் theend;
    }
    இல்லையெனில் எனில் (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	செல் theend;
    }
#எனில் defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    வேர்_menu_ptr = get_வேர்_menu(menu_path);
    எனில் (வேர்_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    எனில் (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	எனில் (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	எனில் (menu_is_popup(menu_path))
	{
	    க்கு (i = 0; i < MENU_INDEX_TIP; ++i)
		எனில் (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    எனில் (p != வெற்று)
		    {
			menu_nable_recurse(*வேர்_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*வேர்_menu_ptr, menu_path, modes, enable);
    }
    இல்லையெனில் எனில் (unmenu)
    {
	எனில் (is_menus_locked())
	    செல் theend;

	/*
	 * Delete menu(s).
	 */
	எனில் (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	எனில் (menu_is_popup(menu_path))
	{
	    க்கு (i = 0; i < MENU_INDEX_TIP; ++i)
		எனில் (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    எனில் (p != வெற்று)
		    {
			remove_menu(வேர்_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(வேர்_menu_ptr, menu_path, modes, FALSE);
    }
    இல்லையெனில்
    {
	எனில் (is_menus_locked())
	    செல் theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	எனில் (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = வெற்று;
	}
	இல்லையெனில் எனில் (modes & MENU_TIP_MODE)
	    map_buf = வெற்று;	// Menu tips are plain text.
	இல்லையெனில்
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), வெற்று);
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
	எனில் (menu_is_popup(menu_path))
	{
	    க்கு (i = 0; i < MENU_INDEX_TIP; ++i)
		எனில் (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    எனில் (p != வெற்று)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = வெற்று;
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

#எனில் defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    எனில் (gui.in_use
	    && (gui.menu_height != old_menu_height
# எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    எனில் (வேர்_menu_ptr == &curwin->w_winbar)
    {
	முழு h = winbar_height(curwin);

	எனில் (h != curwin->w_winbar_height)
	{
	    எனில் (h == 0)
		++curwin->w_height;
	    இல்லையெனில் எனில் (curwin->w_height > 0)
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
    நிலையான முழு
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    முழு		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , முழு	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    முழு		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = வெற்று;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    முழு		i;
    முழு		c;
    முழு		d;
#ifdef FEAT_GUI
    முழு		idx;
    முழு		new_idx;
#endif
    முழு		pri_idx = 0;
    முழு		old_modes = 0;
    முழு		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = வெற்று;
#endif
    vimmenu_T	**வேர்_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    எனில் (path_name == வெற்று)
	திரும்பு FAIL;
    வேர்_menu_ptr = get_வேர்_menu(menu_path);
    menup = வேர்_menu_ptr;
    parent = வெற்று;
    name = path_name;
    போது (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (முழு)STRLEN(name));
	எனில் (map_to != வெற்று)
	{
	    en_name = name;
	    name = map_to;
	}
	இல்லையெனில்
	    en_name = வெற்று;
#endif
	dname = menu_text(name, வெற்று, வெற்று);
	எனில் (dname == வெற்று)
	    செல் erret;
	எனில் (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    செல் erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	போது (menu != வெற்று)
	{
	    எனில் (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		எனில் (*next_name == NUL && menu->children != வெற்று)
		{
		    எனில் (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    செல் erret;
		}
		எனில் (*next_name != NUL && menu->children == வெற்று
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    எனில் (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    செல் erret;
		}
		நிறுத்து;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    எனில் (parent != வெற்று || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		எனில் (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	எனில் (menu == வெற்று)
	{
	    எனில் (*next_name == NUL && parent == வெற்று)
	    {
		emsg(_(e_must_not_add_menu_பொருட்கள்_directly_to_menu_bar));
		செல் erret;
	    }

	    எனில் (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		செல் erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    எனில் (menu == வெற்று)
		செல் erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    எனில் (en_name != வெற்று)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, வெற்று, வெற்று);
	    }
	    இல்லையெனில்
	    {
		menu->en_name = வெற்று;
		menu->en_dname = வெற்று;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = வெற்று;
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
	    எனில் (*next_name == NUL && menuarg->iconfile != வெற்று)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    எனில் (parent != வெற்று && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	இல்லையெனில்
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    எனில் (addtearoff)
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
	எனில் ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    எனில் (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		எனில் (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		இல்லையெனில்
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    எனில் (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != வெற்று
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
		எனில் (tearpath != வெற்று)
		{
		    char_u  *s;
		    முழு	    len;

		    STRCPY(tearpath, menu_path);
		    len = (முழு)(next_name - path_name - 1);
		    க்கு (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			எனில் ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	எனில் (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_செருகு_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_செருகு_MODE));
    எனில் (sys_menu)
	modes &= ~old_modes;

    எனில் (menu != வெற்று && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == வெற்று) ? வெற்று : vim_strsave(call_data);

	// loop over all modes, may add more than one
	க்கு (i = 0; i < MENU_MODES; ++i)
	{
	    எனில் (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		எனில் (amenu && call_data != வெற்று && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    தேர்வு (1 << i)
		    {
			நிலை MENU_VISUAL_MODE:
			நிலை MENU_SELECT_MODE:
			நிலை MENU_OP_PENDING_MODE:
			நிலை MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    நிறுத்து;
			நிலை MENU_செருகு_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    நிறுத்து;
		    }
		}

		எனில் (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    எனில் (menu->strings[i] != வெற்று)
		    {
			menu->strings[i][0] = c;
			எனில் (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			இல்லையெனில்
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			எனில் (c == Ctrl_C)
			{
			    முழு	    len = (முழு)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		இல்லையெனில்
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	எனில் (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    திரும்பு OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    போது (parent != வெற்று && parent->children == வெற்று)
    {
	எனில் (parent->parent == வெற்று)
	    menup = வேர்_menu_ptr;
	இல்லையெனில்
	    menup = &parent->parent->children;
	க்கு ( ; *menup != வெற்று && *menup != parent; menup = &((*menup)->next))
	    ;
	எனில் (*menup == வெற்று) // safety check
	    நிறுத்து;
	parent = parent->parent;
	free_menu(menup);
    }
    திரும்பு FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    நிலையான முழு
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    முழு		modes,
    முழு		enable)
{
    char_u	*p;

    எனில் (menu == வெற்று)
	திரும்பு OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    போது (menu != வெற்று)
    {
	எனில் (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    எனில் (*p != NUL)
	    {
		எனில் (menu->children == வெற்று)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    திரும்பு FAIL;
		}
		எனில் (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
		எனில் (enable)
		    menu->enabled |= modes;
		இல்லையெனில்
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    எனில் (*name != NUL && *name != '*')
		நிறுத்து;
	}
	menu = menu->next;
    }
    எனில் (*name != NUL && *name != '*' && menu == வெற்று)
    {
	semsg(_(e_no_menu_str), name);
	திரும்பு FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    திரும்பு OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    நிலையான முழு
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    முழு		modes,
    முழு		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    எனில் (*menup == வெற்று)
	திரும்பு OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    போது ((menu = *menup) != வெற்று)
    {
	எனில் (*name == NUL || menu_name_equal(name, menu))
	{
	    எனில் (*p != NUL && menu->children == வெற்று)
	    {
		எனில் (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		திரும்பு FAIL;
	    }
	    எனில் ((menu->modes & modes) != 0x0)
	    {
#எனில் defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		எனில் (*p == NUL && modes == MENU_ALL_MODES)
		{
		    எனில் (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		எனில் (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (*name != NUL)
	    {
		எனில் (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		திரும்பு FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    எனில் (*name != NUL)
		நிறுத்து;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    எனில் (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    எனில் ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    இல்லையெனில்
		menup = &menu->next;
	}
	இல்லையெனில்
	    menup = &menu->next;
    }
    எனில் (*name != NUL)
    {
	எனில் (menu == வெற்று)
	{
	    எனில் (!silent)
		semsg(_(e_no_menu_str), name);
	    திரும்பு FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#எனில் defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	எனில் ((s_tearoffs) && (menu->children != வெற்று)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	இல்லையெனில்
#endif
	    child = menu->children;
	க்கு ( ; child != வெற்று; child = child->next)
	    menu->modes |= child->modes;
	எனில் (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    எனில் (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	எனில் ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#எனில் defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    எனில் (s_tearoffs && menu->children != வெற்று) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    திரும்பு OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    வெறுமை
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_பொருட்கள்);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    நிலையான வெறுமை
free_menu(vimmenu_T **menup)
{
    முழு		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    எனில் (gui.in_use)
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
    க்கு (i = 0; i < MENU_MODES; i++)
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
    நிலையான வெறுமை
free_menu_string(vimmenu_T *menu, முழு idx)
{
    முழு		மொத்தம் = 0;
    முழு		i;

    க்கு (i = 0; i < MENU_MODES; i++)
	எனில் (menu->strings[i] == menu->strings[idx])
	    மொத்தம்++;
    எனில் (மொத்தம் == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = வெற்று;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    நிலையான முழு
show_menus(char_u *path_name, முழு modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = வெற்று;

    name = path_name = vim_strsave(path_name);
    எனில் (path_name == வெற்று)
	திரும்பு FAIL;
    menu = *get_வேர்_menu(path_name);

    // First, find the (sub)menu with the given name
    போது (*name)
    {
	p = menu_name_skip(name);
	போது (menu != வெற்று)
	{
	    எனில் (menu_name_equal(name, menu))
	    {
		// Found menu
		எனில் (*p != NUL && menu->children == வெற்று)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    திரும்பு FAIL;
		}
		இல்லையெனில் எனில் ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    திரும்பு FAIL;
		}
		நிறுத்து;
	    }
	    menu = menu->next;
	}
	எனில் (menu == வெற்று)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    திரும்பு FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    செய்தி_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    திரும்பு OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    நிலையான வெறுமை
show_menus_recursive(vimmenu_T *menu, முழு modes, முழு depth)
{
    முழு		i;
    முழு		bit;

    எனில் (menu != வெற்று && (menu->modes & modes) == 0x0)
	திரும்பு;

    எனில் (menu != வெற்று)
    {
	செய்தி_putchar('\n');
	எனில் (got_int)		// "q" hit for "--more--"
	    திரும்பு;
	க்கு (i = 0; i < depth; i++)
	    செய்தி_puts("  ");
	எனில் (menu->priority)
	{
	    செய்தி_outnum((long)menu->priority);
	    செய்தி_puts(" ");
	}
				// Same highlighting as for directories!?
	செய்தி_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    எனில் (menu != வெற்று && menu->children == வெற்று)
    {
	க்கு (bit = 0; bit < MENU_MODES; bit++)
	    எனில் ((menu->modes & modes & (1 << bit)) != 0)
	    {
		செய்தி_putchar('\n');
		எனில் (got_int)		// "q" hit for "--more--"
		    திரும்பு;
		க்கு (i = 0; i < depth + 2; i++)
		    செய்தி_puts("  ");
		செய்தி_puts(menu_mode_chars[bit]);
		எனில் (menu->noremap[bit] == REMAP_NONE)
		    செய்தி_putchar('*');
		இல்லையெனில் எனில் (menu->noremap[bit] == REMAP_SCRIPT)
		    செய்தி_putchar('&');
		இல்லையெனில்
		    செய்தி_putchar(' ');
		எனில் (menu->silent[bit])
		    செய்தி_putchar('s');
		இல்லையெனில்
		    செய்தி_putchar(' ');
		எனில் ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    செய்தி_putchar('-');
		இல்லையெனில்
		    செய்தி_putchar(' ');
		செய்தி_puts(" ");
		எனில் (*menu->strings[bit] == NUL)
		    செய்தி_puts_attr("<Nop>", HL_ATTR(HLF_8));
		இல்லையெனில்
		    செய்தி_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    இல்லையெனில்
    {
	எனில் (menu == வெற்று)
	{
	    menu = வேர்_menu;
	    depth--;
	}
	இல்லையெனில்
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	க்கு (; menu != வெற்று && !got_int; menu = menu->next)
	    எனில் (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
நிலையான vimmenu_T	*expand_menu = வெற்று;
நிலையான vimmenu_T	*expand_menu_alt = வெற்று;
நிலையான முழு		expand_modes = 0x0;
நிலையான முழு		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    முழு		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = வெற்று;
    char_u	*name;
    முழு		unmenu;
    vimmenu_T	*menu;
    முழு		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    க்கு (p = arg; *p; ++p)
	எனில் (!VIM_ISDIGIT(*p) && *p != '.')
	    நிறுத்து;

    எனில் (!VIM_ISWHITE(*p))
    {
	எனில் (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	இல்லையெனில் எனில் (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	இல்லையெனில்
	    p = arg;
    }

    போது (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    க்கு (; *p && !VIM_ISWHITE(*p); ++p)
    {
	எனில் ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	இல்லையெனில் எனில் (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    எனில் (expand_menus && VIM_ISWHITE(*p))
	திரும்பு வெற்று;	// TODO: check for next command?
    எனில் (*p == NUL)		// Complete the menu name
    {
	முழு try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, வெற்று, &unmenu);
	எனில் (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = வேர்_menu;
	எனில் (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    எனில் (path_name == வெற்று)
		திரும்பு வெற்று;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	போது (name != வெற்று && *name)
	{
	    p = menu_name_skip(name);
	    போது (menu != வெற்று)
	    {
		எனில் (menu_name_equal(name, menu))
		{
		    // Found menu
		    எனில் ((*p != NUL && menu->children == வெற்று)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			திரும்பு வெற்று;
		    }
		    நிறுத்து;
		}
		menu = menu->next;
		எனில் (menu == வெற்று && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    எனில் (menu == வெற்று)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		திரும்பு வெற்று;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	எனில் (expand_menu == வேர்_menu)
	    expand_menu_alt = curwin->w_winbar;
	இல்லையெனில்
	    expand_menu_alt = வெற்று;
    }
    இல்லையெனில்			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    திரும்பு வெற்று;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, முழு idx)
{
    நிலையான vimmenu_T	*menu = வெற்று;
    நிலையான முழு		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    நிலையான  முழு		should_advance = FALSE;
#endif

    எனில் (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    போது (menu != வெற்று && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == வெற்று))
    {
	menu = menu->next;
	எனில் (menu == வெற்று && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    எனில் (menu == வெற்று)	    // at end of linked list
	திரும்பு வெற்று;

    எனில் (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	எனில் (should_advance)
	    str = menu->en_dname;
	இல்லையெனில்
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    எனில் (menu->en_dname == வெற்று)
		should_advance = TRUE;
	}
#endif
    இல்லையெனில்
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    எனில் (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	எனில் (menu == வெற்று && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    திரும்பு str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, முழு idx)
{
    நிலையான vimmenu_T	*menu = வெற்று;
    நிலையான முழு		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    நிலையான char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    நிலையான  முழு		should_advance = FALSE;
#endif

    எனில் (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    போது (menu != வெற்று
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	எனில் (menu == வெற்று && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    எனில் (menu == வெற்று)	    // at end of linked list
	திரும்பு வெற்று;

    எனில் (menu->modes & expand_modes)
    {
	எனில் (menu->children != வெற்று)
	{
#ifdef FEAT_MULTI_LANG
	    எனில் (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    இல்லையெனில்
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		எனில் (menu->en_dname == வெற்று)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	இல்லையெனில்
#ifdef FEAT_MULTI_LANG
	{
	    எனில் (should_advance)
		str = menu->en_dname;
	    இல்லையெனில்
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		எனில் (menu->en_dname == வெற்று)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    இல்லையெனில்
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    எனில் (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	எனில் (menu == வெற்று && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    திரும்பு str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    நிலையான char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    க்கு (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	எனில் (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    எனில் (*p == NUL)
		நிறுத்து;
	}
    }
    எனில் (*p)
	*p++ = NUL;
    திரும்பு p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    நிலையான முழு
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    எனில் (menu->en_name != வெற்று
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	திரும்பு TRUE;
#endif
    திரும்பு menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    நிலையான முழு
menu_namecmp(char_u *name, char_u *mname)
{
    முழு		i;

    க்கு (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	எனில் (name[i] != mname[i])
	    நிறுத்து;
    திரும்பு ((name[i] == NUL || name[i] == TAB)
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
    நிலையான முழு
get_menu_cmd_modes(
    char_u  *cmd,
    முழு	    forceit,	    // Was there a "!" after the command?
    முழு	    *noremap,
    முழு	    *unmenu)
{
    முழு	    modes;

    தேர்வு (*cmd++)
    {
	நிலை 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    நிறுத்து;
	நிலை 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    நிறுத்து;
	நிலை 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    நிறுத்து;
	நிலை 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    நிறுத்து;
	நிலை 'i':			// imenu
	    modes = MENU_செருகு_MODE;
	    நிறுத்து;
	நிலை 't':
	    எனில் (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		நிறுத்து;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    நிறுத்து;
	நிலை 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    நிறுத்து;
	நிலை 'a':			// amenu
	    modes = MENU_செருகு_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    நிறுத்து;
	நிலை 'n':
	    எனில் (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		நிறுத்து;
	    }
	    // FALLTHROUGH
	இயல்பு:
	    --cmd;
	    எனில் (forceit)		// menu!!
		modes = MENU_செருகு_MODE | MENU_CMDLINE_MODE;
	    இல்லையெனில்			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    எனில் (noremap != வெற்று)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    எனில் (unmenu != வெற்று)
	*unmenu = (*cmd == 'u');
    திரும்பு modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    நிலையான char_u *
get_menu_mode_str(முழு modes)
{
    எனில் ((modes & (MENU_செருகு_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_செருகு_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	திரும்பு (char_u *)"a";
    எனில் ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	திரும்பு (char_u *)" ";
    எனில் ((modes & (MENU_செருகு_MODE | MENU_CMDLINE_MODE))
	    == (MENU_செருகு_MODE | MENU_CMDLINE_MODE))
	திரும்பு (char_u *)"!";
    எனில் ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	திரும்பு (char_u *)"v";
    எனில் (modes & MENU_VISUAL_MODE)
	திரும்பு (char_u *)"x";
    எனில் (modes & MENU_SELECT_MODE)
	திரும்பு (char_u *)"s";
    எனில் (modes & MENU_OP_PENDING_MODE)
	திரும்பு (char_u *)"o";
    எனில் (modes & MENU_செருகு_MODE)
	திரும்பு (char_u *)"i";
    எனில் (modes & MENU_TERMINAL_MODE)
	திரும்பு (char_u *)"tl";
    எனில் (modes & MENU_CMDLINE_MODE)
	திரும்பு (char_u *)"c";
    எனில் (modes & MENU_NORMAL_MODE)
	திரும்பு (char_u *)"n";
    எனில் (modes & MENU_TIP_MODE)
	திரும்பு (char_u *)"t";

    திரும்பு (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    நிலையான char_u *
popup_mode_name(char_u *name, முழு idx)
{
    char_u	*p;
    முழு		len = (முழு)STRLEN(name);
    எழுத்து	*mode_chars = menu_mode_chars[idx];
    முழு		mode_chars_len = (முழு)strlen(mode_chars);
    முழு		i;

    p = vim_strnsave(name, len + mode_chars_len);
    எனில் (p == வெற்று)
	திரும்பு வெற்று;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    க்கு (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    திரும்பு p;
}

#எனில் defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    முழு
get_menu_index(vimmenu_T *menu, முழு state)
{
    முழு		idx;

    எனில் ((state & MODE_செருகு))
	idx = MENU_INDEX_செருகு;
    இல்லையெனில் எனில் (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    இல்லையெனில் எனில் (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    இல்லையெனில் எனில் (VIsual_active)
    {
	எனில் (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	இல்லையெனில்
	    idx = MENU_INDEX_VISUAL;
    }
    இல்லையெனில் எனில் (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    இல்லையெனில் எனில் (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    இல்லையெனில் எனில் ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    இல்லையெனில்
	idx = MENU_INDEX_INVALID;

    எனில் (idx != MENU_INDEX_INVALID && menu->strings[idx] == வெற்று)
	idx = MENU_INDEX_INVALID;
    திரும்பு idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    நிலையான char_u *
menu_text(char_u *str, முழு *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    எனில் (p != வெற்று)
    {
	எனில் (actext != வெற்று)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    இல்லையெனில்
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    க்கு (p = text; p != வெற்று; )
    {
	p = vim_strchr(p, '&');
	எனில் (p != வெற்று)
	{
	    எனில் (p[1] == NUL)	    // trailing "&"
		நிறுத்து;
	    எனில் (mnemonic != வெற்று && p[1] != '&')
#எனில் !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#இல்லையெனில்
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		எழுத்து c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    திரும்பு text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    முழு
menu_is_menubar(char_u *name)
{
    திரும்பு (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    முழு
menu_is_popup(char_u *name)
{
    திரும்பு (STRNCMP(name, "PopUp", 5) == 0);
}

#எனில் defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    முழு
menu_is_child_of_popup(vimmenu_T *menu)
{
    போது (menu->parent != வெற்று)
	menu = menu->parent;
    திரும்பு menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    முழு
menu_is_toolbar(char_u *name)
{
    திரும்பு (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    முழு
menu_is_separator(char_u *name)
{
    திரும்பு (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    நிலையான முழு
menu_is_hidden(char_u *name)
{
    திரும்பு (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    நிலையான முழு
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    திரும்பு (STRCMP(name, TEAR_STRING) == 0);
#இல்லையெனில்
    திரும்பு FALSE;
#endif
}

#எனில் defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    நிலையான முழு
get_menu_mode(வெறுமை)
{
# ifdef FEAT_TERMINAL
    எனில் (term_use_loop())
	திரும்பு MENU_INDEX_TERMINAL;
# endif
    எனில் (VIsual_active)
    {
	எனில் (VIsual_select)
	    திரும்பு MENU_INDEX_SELECT;
	திரும்பு MENU_INDEX_VISUAL;
    }
    எனில் (State & MODE_செருகு)
	திரும்பு MENU_INDEX_செருகு;
    எனில் ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	திரும்பு MENU_INDEX_CMDLINE;
    எனில் (finish_op)
	திரும்பு MENU_INDEX_OP_PENDING;
    எனில் (State & MODE_NORMAL)
	திரும்பு MENU_INDEX_NORMAL;
    எனில் (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	திரும்பு MENU_INDEX_செருகு;
    திரும்பு MENU_INDEX_INVALID;
}

    முழு
get_menu_mode_flag(வெறுமை)
{
    முழு mode = get_menu_mode();

    எனில் (mode == MENU_INDEX_INVALID)
	திரும்பு 0;
    திரும்பு 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    வெறுமை
show_popupmenu(வெறுமை)
{
    vimmenu_T	*menu;
    முழு		menu_mode;
    எழுத்து*	mode;
    முழு		mode_len;

    menu_mode = get_menu_mode();
    எனில் (menu_mode == MENU_INDEX_INVALID)
	திரும்பு;
    mode = menu_mode_chars[menu_mode];
    mode_len = (முழு)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, வெற்று, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	எனில் (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    நிறுத்து;

    // Only show a popup when it is defined and has entries
    எனில் (menu == வெற்று || menu->children == வெற்று)
	திரும்பு;

# எனில் defined(FEAT_GUI)
    எனில் (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# எனில் defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    இல்லையெனில்
# endif
# எனில் defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#எனில் defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    முழு
check_menu_pointer(vimmenu_T *வேர், vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    க்கு (p = வேர்; p != வெற்று; p = p->next)
	எனில் (p == menu_to_check
		|| (p->children != வெற்று
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    திரும்பு OK;
    திரும்பு FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    வெறுமை
gui_create_initial_menus(vimmenu_T *menu)
{
    முழு		idx = 0;

    போது (menu != வெற்று)
    {
	// Don't add a menu when only a tip was defined.
	எனில் (menu->modes & MENU_ALL_MODES)
	{
	    எனில் (menu->children != வெற்று)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    இல்லையெனில்
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    நிலையான வெறுமை
gui_update_menus_recurse(vimmenu_T *menu, முழு mode)
{
    முழு		grey;

    போது (menu)
    {
	எனில் ((menu->modes & menu->enabled & mode)
# எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	இல்லையெனில்
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	எனில் (vim_strchr(p_go, GO_GREY) != வெற்று || menu->parent == வெற்று
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	இல்லையெனில்
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
    வெறுமை
gui_update_menus(முழு modes)
{
    நிலையான முழு	    prev_mode = -1;
    முழு		    mode = 0;

    எனில் (modes != 0x0)
	mode = modes;
    இல்லையெனில்
	mode = get_menu_mode_flag();

    எனில் (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(வேர்_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    முழு
gui_is_menu_shortcut(முழு சாவி)
{
    vimmenu_T	*menu;

    எனில் (சாவி < 256)
	சாவி = TOLOWER_LOC(சாவி);
    FOR_ALL_MENUS(menu)
	எனில் (menu->mnemonic == சாவி
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == சாவி))
	    திரும்பு TRUE;
    திரும்பு FALSE;
}
# endif
#endif // FEAT_GUI

#எனில் defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    வெறுமை
gui_mch_toggle_tearoffs(முழு enable)
{
    முழு		pri_tab[MENUDEPTH + 1];
    முழு		i;

    எனில் (enable)
    {
	க்கு (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(வேர்_menu, (char_u *)"", pri_tab, 0);
    }
    இல்லையெனில்
	gui_destroy_tearoffs_recurse(வேர்_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    நிலையான வெறுமை
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    மாறிலி char_u	*pname,
    முழு			*pri_tab,
    முழு			pri_idx)
{
    char_u	*newpname = வெற்று;
    முழு		len;
    char_u	*s;
    char_u	*d;

    எனில் (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    போது (menu != வெற்று)
    {
	எனில் (menu->children != வெற்று && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (முழு)STRLEN(pname) + (முழு)STRLEN(menu->name);
	    க்கு (s = menu->name; *s; ++s)
		எனில் (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    எனில் (newpname != வெற்று)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		க்கு (s = menu->name; *s; ++s)
		{
		    எனில் (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		எனில் (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    நிலையான வெறுமை
gui_add_tearoff(char_u *tearpath, முழு *pri_tab, முழு pri_idx)
{
    char_u	*tbuf;
    முழு		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned முழு)STRLEN(tearpath));
    எனில் (tbuf == வெற்று)
	திரும்பு;

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
    menuarg.iconfile = வெற்று;
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
    நிலையான வெறுமை
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    போது (menu)
    {
	எனில் (menu->children)
	{
	    // check if tearoff exists
	    எனில் (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    எனில் (menu->children != வெற்று) // if not the last one
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
    வெறுமை
execute_menu(exarg_T *eap, vimmenu_T *menu, முழு mode_idx)
{
    முழு		idx = mode_idx;

    எனில் (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	எனில் (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_செருகு;
	}
#ifdef FEAT_TERMINAL
	இல்லையெனில் எனில் (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	இல்லையெனில் எனில் (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	இல்லையெனில் எனில் (eap != வெற்று && eap->addr_மொத்தம்)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    எனில் ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    இல்லையெனில்
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
	    எனில் (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    எனில் (idx == MENU_INDEX_INVALID || eap == வெற்று)
	idx = MENU_INDEX_NORMAL;

    எனில் (menu->strings[idx] != வெற்று && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	எனில் (eap == வெற்று || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    எனில் (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	இல்லையெனில்
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    இல்லையெனில் எனில் (eap != வெற்று)
    {
	char_u	*mode;

	தேர்வு (idx)
	{
	    நிலை MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		நிறுத்து;
	    நிலை MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		நிறுத்து;
	    நிலை MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		நிறுத்து;
	    நிலை MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		நிறுத்து;
	    நிலை MENU_INDEX_செருகு:
		mode = (char_u *)"Insert";
		நிறுத்து;
	    நிலை MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		நிறுத்து;
	    // case MENU_INDEX_TIP: cannot happen
	    இயல்பு:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    நிலையான vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    முழு		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    எனில் (saved_name == வெற்று)
	திரும்பு வெற்று;

    menu = *get_வேர்_menu(saved_name);
    name = saved_name;
    போது (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	போது (menu != வெற்று)
	{
	    எனில் (menu_name_equal(name, menu))
	    {
		எனில் (*p == NUL && menu->children != வெற்று)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = வெற்று;
		}
		இல்லையெனில் எனில் (*p != NUL && menu->children == வெற்று)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = வெற்று;
		}
		நிறுத்து;
	    }
	    menu = menu->next;
	}
	எனில் (menu == வெற்று || *p == NUL)
	    நிறுத்து;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    எனில் (menu == வெற்று)
    {
	எனில் (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	திரும்பு வெற்று;
    }

    திரும்பு menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    வெறுமை
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    முழு		mode_idx = MENU_INDEX_INVALID;

    எனில் (arg[0] && VIM_ISWHITE(arg[1]))
    {
	தேர்வு (arg[0])
	{
	    நிலை 'n': mode_idx = MENU_INDEX_NORMAL; நிறுத்து;
	    நிலை 'v': mode_idx = MENU_INDEX_VISUAL; நிறுத்து;
	    நிலை 's': mode_idx = MENU_INDEX_SELECT; நிறுத்து;
	    நிலை 'o': mode_idx = MENU_INDEX_OP_PENDING; நிறுத்து;
	    நிலை 't': mode_idx = MENU_INDEX_TERMINAL; நிறுத்து;
	    நிலை 'i': mode_idx = MENU_INDEX_செருகு; நிறுத்து;
	    நிலை 'c': mode_idx = MENU_INDEX_CMDLINE; நிறுத்து;
	    இயல்பு: semsg(_(e_invalid_argument_str), arg);
		     திரும்பு;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    எனில் (menu == வெற்று)
	திரும்பு;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    வெறுமை
winbar_click(win_T *wp, முழு col)
{
    முழு		idx;

    எனில் (wp->w_winbar_பொருட்கள் == வெற்று)
	திரும்பு;
    க்கு (idx = 0; wp->w_winbar_பொருட்கள்[idx].wb_menu != வெற்று; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_பொருட்கள்[idx];

	எனில் (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = வெற்று;
	    pos_T   save_visual = VIsual;
	    முழு	    save_visual_active = VIsual_active;
	    முழு	    save_visual_select = VIsual_select;
	    முழு	    save_visual_reselect = VIsual_reselect;
	    முழு	    save_visual_mode = VIsual_mode;

	    எனில் (wp != curwin)
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
	    execute_menu(வெற்று, item->wb_menu, -1);

	    எனில் (save_curwin != வெற்று && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    எனில் (!win_valid(wp))
		நிறுத்து;
	}
    }
}

#எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = வெற்று;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_வேர்_menu(path_name);

    saved_name = vim_strsave(path_name);
    எனில் (saved_name == வெற்று)
	திரும்பு வெற்று;

    name = saved_name;
    போது (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	போது (menu != வெற்று)
	{
	    எனில் (menu_name_equal(name, menu))
	    {
		எனில் (menu->children == வெற்று)
		{
		    // found a menu item instead of a sub-menu
		    எனில் (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    இல்லையெனில்
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = வெற்று;
		    செல் theend;
		}
		எனில் (*p == NUL)	    // found a full match
		    செல் theend;
		நிறுத்து;
	    }
	    menu = menu->next;
	}
	எனில் (menu == வெற்று)	// didn't find it
	    நிறுத்து;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    எனில் (menu == வெற்று)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    திரும்பு menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

வகைவரை கட்டமைப்பு
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

நிலையான garray_T menutrans_ga = {0, 0, 0, 0, வெற்று};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    வெறுமை
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    முழு			i;
    char_u		*from, *from_noamp, *to;

    எனில் (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, அளவு(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    எனில் (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	க்கு (i = 0; i < menutrans_ga.ga_len; ++i)
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
    இல்லையெனில்
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	எனில் (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	இல்லையெனில்
	{
	    எனில் (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		எனில் (from != வெற்று)
		{
		    from_noamp = menu_text(from, வெற்று, வெற்று);
		    to = vim_strnsave(to, arg - to);
		    எனில் (from_noamp != வெற்று && to != வெற்று)
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
		    இல்லையெனில்
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

#எனில் defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    நிலையான char_u *
menu_skip_part(char_u *p)
{
    போது (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	எனில் ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    திரும்பு p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    நிலையான char_u *
menutrans_lookup(char_u *name, முழு len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    முழு			i;
    char_u		*dname;

    க்கு (i = 0; i < menutrans_ga.ga_len; ++i)
	எனில் (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    திரும்பு tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, வெற்று, வெற்று);
    name[len] = i;
    எனில் (dname == வெற்று)
	திரும்பு வெற்று;

    க்கு (i = 0; i < menutrans_ga.ga_len; ++i)
	எனில் (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    திரும்பு tp[i].to;
	}
    vim_free(dname);

    திரும்பு வெற்று;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    நிலையான வெறுமை
menu_unescape_name(char_u *name)
{
    char_u  *p;

    க்கு (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	எனில் (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    நிலையான char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    போது (*arg && !VIM_ISWHITE(*arg))
    {
	எனில் ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	இல்லையெனில் எனில் (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    எனில் (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    திரும்பு arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    நிலையான முழு
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, முழு modes, dict_T *dict)
{
    முழு		status;
    list_T	*l;

    எனில் (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	எனில் (l == வெற்று)
	    திரும்பு FAIL;

	எனில் (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    திரும்பு FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	க்கு (topmenu = menu; topmenu != வெற்று; topmenu = topmenu->next)
	    எனில் (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	திரும்பு OK;
    }

    எனில் (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	திரும்பு OK;

    status = dict_add_string(dict, "name", menu->name);
    எனில் (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    எனில் (status == OK && menu->actext != வெற்று)
	status = dict_add_string(dict, "accel", menu->actext);
    எனில் (status == OK)
	status = dict_add_எண்(dict, "priority", menu->priority);
    எனில் (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    எனில் (status == OK && menu->iconfile != வெற்று)
	status = dict_add_string(dict, "icon", menu->iconfile);
    எனில் (status == OK && menu->iconidx >= 0)
	status = dict_add_எண்(dict, "iconidx", menu->iconidx);
#endif
    எனில் (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	முழு	buflen;

	எனில் (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	இல்லையெனில்
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    எனில் (status == OK && menu->children == வெற்று)
    {
	முழு		bit;

	// Get the first mode in which the menu is available
	க்கு (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	எனில் (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    எனில் (menu->strings[bit] != வெற்று)
	    {
		எனில் (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		இல்லையெனில்
		{
		    char_u *tofree = வெற்று;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    எனில் (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    எனில் (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    எனில் (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    எனில் (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    எனில் (status == OK && menu->children != வெற்று)
    {
	vimmenu_T	*child;

	l = list_alloc();
	எனில் (l == வெற்று)
	    திரும்பு FAIL;

	எனில் (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    திரும்பு FAIL;
	}
	child = menu->children;
	போது (child)
	{
	    எனில் (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    திரும்பு status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    வெறுமை
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    முழு		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    retdict = rettv->vval.v_dict;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    menu_name = tv_get_string_chk(&argvars[0]);
    எனில் (menu_name == வெற்று)
	திரும்பு;

    // menu mode
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    இல்லையெனில்
	which = (char_u *)"";	    // Default is modes for "menu"
    எனில் (which == வெற்று)
	திரும்பு;

    modes = get_menu_cmd_modes(which, *which == '!', வெற்று, வெற்று);

    // Locate the specified menu or menu item
    menu = *get_வேர்_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    எனில் (saved_name == வெற்று)
	திரும்பு;
    எனில் (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	போது (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    போது (menu != வெற்று)
	    {
		எனில் (menu_name_equal(name, menu))
		    நிறுத்து;
		menu = menu->next;
	    }
	    எனில் (menu == வெற்று || *p == NUL)
		நிறுத்து;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    எனில் (menu == வெற்று)		// specified menu not found
	திரும்பு;

    எனில் (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
