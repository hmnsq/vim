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

#شامل "vim.h"

#اگر defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
ایستا صحیح add_menu_path(char_u *, vimmenu_T *, صحیح *, char_u *, صحیح);
#وگرنه
ایستا صحیح add_menu_path(char_u *, vimmenu_T *, صحیح *, char_u *);
#endif
ایستا صحیح menu_nable_recurse(vimmenu_T *menu, char_u *name, صحیح modes, صحیح enable);
ایستا صحیح remove_menu(vimmenu_T **, char_u *, صحیح, صحیح silent);
ایستا تهی free_menu(vimmenu_T **menup);
ایستا تهی free_menu_string(vimmenu_T *, صحیح);
ایستا صحیح show_menus(char_u *, صحیح);
ایستا تهی show_menus_recursive(vimmenu_T *, صحیح, صحیح);
ایستا char_u *menu_name_skip(char_u *name);
ایستا صحیح menu_name_equal(char_u *name, vimmenu_T *menu);
ایستا صحیح menu_namecmp(char_u *name, char_u *mname);
ایستا صحیح get_menu_cmd_modes(char_u *, صحیح, صحیح *, صحیح *);
ایستا char_u *popup_mode_name(char_u *name, صحیح idx);
ایستا char_u *menu_text(char_u *text, صحیح *mnemonic, char_u **actext);

#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
ایستا تهی gui_create_tearoffs_recurse(vimmenu_T *menu, ثابت char_u *pname, صحیح *pri_tab, صحیح pri_idx);
ایستا تهی gui_add_tearoff(char_u *tearpath, صحیح *pri_tab, صحیح pri_idx);
ایستا تهی gui_destroy_tearoffs_recurse(vimmenu_T *menu);
ایستا صحیح s_tearoffs = FALSE;
#endif

ایستا صحیح menu_is_hidden(char_u *name);
ایستا صحیح menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
ایستا صحیح menus_locked = 0;

#اگر defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
ایستا char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
ایستا char_u *menutrans_lookup(char_u *name, صحیح len);
ایستا تهی menu_unescape_name(char_u	*p);
#endif

ایستا char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
ایستا نویسه *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
ایستا ثابت نویسه *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_شمار ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    ایستا صحیح
menu_is_winbar(char_u *name)
{
    بازگشت (STRNCMP(name, "WinBar", 6) == 0);
}

    صحیح
winbar_height(win_T *wp)
{
    اگر (wp->w_winbar != پوچ && wp->w_winbar->children != پوچ)
	بازگشت 1;
    بازگشت 0;
}

    ایستا vimmenu_T **
get_ریشه_menu(char_u *name)
{
    اگر (menu_is_winbar(name))
	بازگشت &curwin->w_winbar;
    بازگشت &ریشه_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ایستا صحیح
is_menus_locked(تهی)
{
    اگر (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    تهی
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    صحیح		modes;
    char_u	*map_to;
    صحیح		noremap;
    صحیح		silent = FALSE;
    صحیح		special = FALSE;
    صحیح		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    صحیح		i;
#اگر defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    صحیح		old_menu_height;
# اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    صحیح		old_toolbar_height;
# endif
#endif
    صحیح		pri_tab[MENUDEPTH + 1];
    صحیح		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = پوچ;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**ریشه_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    برای (;;)
    {
	اگر (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    ادامه;
	}
	اگر (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    ادامه;
	}
	اگر (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    ادامه;
	}
	بشکن;
    }


    // Locate an optional "icon=filename" argument.
    اگر (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	وقتی (*arg != NUL && *arg != ' ')
	{
	    اگر (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	اگر (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    برای (p = arg; *p; ++p)
	اگر (!VIM_ISDIGIT(*p) && *p != '.')
	    بشکن;
    اگر (VIM_ISWHITE(*p))
    {
	برای (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    اگر (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    اگر (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    وگرنه اگر (eap->addr_شمار && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    وگرنه
	i = 0;
    وقتی (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    اگر (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    وگرنه اگر (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    اگر (*arg == NUL)
    {
	show_menus(arg, modes);
	بازگشت;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    اگر (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	اگر (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    اگر (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		اگر (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((نویسه *)menu_path + 7);
		    اگر (menuarg.iconidx >= (صحیح)TOOLBAR_NAME_شمار)
			menuarg.iconidx = -1;
		    وگرنه
			menuarg.icon_builtin = TRUE;
		}
	    }
	    وگرنه
	    {
		برای (i = 0; i < (صحیح)TOOLBAR_NAME_شمار; ++i)
		    اگر (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			بشکن;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    اگر (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	برو theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    اگر (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	برو theend;
    }
    وگرنه اگر (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	برو theend;
    }
#اگر defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    ریشه_menu_ptr = get_ریشه_menu(menu_path);
    اگر (ریشه_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    اگر (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	اگر (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	اگر (menu_is_popup(menu_path))
	{
	    برای (i = 0; i < MENU_INDEX_TIP; ++i)
		اگر (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    اگر (p != پوچ)
		    {
			menu_nable_recurse(*ریشه_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*ریشه_menu_ptr, menu_path, modes, enable);
    }
    وگرنه اگر (unmenu)
    {
	اگر (is_menus_locked())
	    برو theend;

	/*
	 * Delete menu(s).
	 */
	اگر (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	اگر (menu_is_popup(menu_path))
	{
	    برای (i = 0; i < MENU_INDEX_TIP; ++i)
		اگر (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    اگر (p != پوچ)
		    {
			remove_menu(ریشه_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(ریشه_menu_ptr, menu_path, modes, FALSE);
    }
    وگرنه
    {
	اگر (is_menus_locked())
	    برو theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	اگر (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = پوچ;
	}
	وگرنه اگر (modes & MENU_TIP_MODE)
	    map_buf = پوچ;	// Menu tips are plain text.
	وگرنه
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), پوچ);
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
	اگر (menu_is_popup(menu_path))
	{
	    برای (i = 0; i < MENU_INDEX_TIP; ++i)
		اگر (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    اگر (p != پوچ)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = پوچ;
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

#اگر defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    اگر (gui.in_use
	    && (gui.menu_height != old_menu_height
# اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    اگر (ریشه_menu_ptr == &curwin->w_winbar)
    {
	صحیح h = winbar_height(curwin);

	اگر (h != curwin->w_winbar_height)
	{
	    اگر (h == 0)
		++curwin->w_height;
	    وگرنه اگر (curwin->w_height > 0)
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
    ایستا صحیح
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    صحیح		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , صحیح	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    صحیح		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = پوچ;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    صحیح		i;
    صحیح		c;
    صحیح		d;
#ifdef FEAT_GUI
    صحیح		idx;
    صحیح		new_idx;
#endif
    صحیح		pri_idx = 0;
    صحیح		old_modes = 0;
    صحیح		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = پوچ;
#endif
    vimmenu_T	**ریشه_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    اگر (path_name == پوچ)
	بازگشت FAIL;
    ریشه_menu_ptr = get_ریشه_menu(menu_path);
    menup = ریشه_menu_ptr;
    parent = پوچ;
    name = path_name;
    وقتی (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (صحیح)STRLEN(name));
	اگر (map_to != پوچ)
	{
	    en_name = name;
	    name = map_to;
	}
	وگرنه
	    en_name = پوچ;
#endif
	dname = menu_text(name, پوچ, پوچ);
	اگر (dname == پوچ)
	    برو erret;
	اگر (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    برو erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	وقتی (menu != پوچ)
	{
	    اگر (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		اگر (*next_name == NUL && menu->children != پوچ)
		{
		    اگر (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    برو erret;
		}
		اگر (*next_name != NUL && menu->children == پوچ
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    اگر (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    برو erret;
		}
		بشکن;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    اگر (parent != پوچ || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		اگر (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	اگر (menu == پوچ)
	{
	    اگر (*next_name == NUL && parent == پوچ)
	    {
		emsg(_(e_must_not_add_menu_اقلام_directly_to_menu_bar));
		برو erret;
	    }

	    اگر (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		برو erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    اگر (menu == پوچ)
		برو erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    اگر (en_name != پوچ)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, پوچ, پوچ);
	    }
	    وگرنه
	    {
		menu->en_name = پوچ;
		menu->en_dname = پوچ;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = پوچ;
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
	    اگر (*next_name == NUL && menuarg->iconfile != پوچ)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    اگر (parent != پوچ && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	وگرنه
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    اگر (addtearoff)
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
	اگر ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    اگر (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		اگر (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		وگرنه
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    اگر (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != پوچ
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
		اگر (tearpath != پوچ)
		{
		    char_u  *s;
		    صحیح	    len;

		    STRCPY(tearpath, menu_path);
		    len = (صحیح)(next_name - path_name - 1);
		    برای (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			اگر ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	اگر (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_درج_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_درج_MODE));
    اگر (sys_menu)
	modes &= ~old_modes;

    اگر (menu != پوچ && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == پوچ) ? پوچ : vim_strsave(call_data);

	// loop over all modes, may add more than one
	برای (i = 0; i < MENU_MODES; ++i)
	{
	    اگر (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		اگر (amenu && call_data != پوچ && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    انتخاب (1 << i)
		    {
			حالت MENU_VISUAL_MODE:
			حالت MENU_SELECT_MODE:
			حالت MENU_OP_PENDING_MODE:
			حالت MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    بشکن;
			حالت MENU_درج_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    بشکن;
		    }
		}

		اگر (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    اگر (menu->strings[i] != پوچ)
		    {
			menu->strings[i][0] = c;
			اگر (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			وگرنه
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			اگر (c == Ctrl_C)
			{
			    صحیح	    len = (صحیح)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		وگرنه
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	اگر (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    بازگشت OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    وقتی (parent != پوچ && parent->children == پوچ)
    {
	اگر (parent->parent == پوچ)
	    menup = ریشه_menu_ptr;
	وگرنه
	    menup = &parent->parent->children;
	برای ( ; *menup != پوچ && *menup != parent; menup = &((*menup)->next))
	    ;
	اگر (*menup == پوچ) // safety check
	    بشکن;
	parent = parent->parent;
	free_menu(menup);
    }
    بازگشت FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    ایستا صحیح
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    صحیح		modes,
    صحیح		enable)
{
    char_u	*p;

    اگر (menu == پوچ)
	بازگشت OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    وقتی (menu != پوچ)
    {
	اگر (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    اگر (*p != NUL)
	    {
		اگر (menu->children == پوچ)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    بازگشت FAIL;
		}
		اگر (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه
		اگر (enable)
		    menu->enabled |= modes;
		وگرنه
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    اگر (*name != NUL && *name != '*')
		بشکن;
	}
	menu = menu->next;
    }
    اگر (*name != NUL && *name != '*' && menu == پوچ)
    {
	semsg(_(e_no_menu_str), name);
	بازگشت FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    بازگشت OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    ایستا صحیح
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    صحیح		modes,
    صحیح		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    اگر (*menup == پوچ)
	بازگشت OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    وقتی ((menu = *menup) != پوچ)
    {
	اگر (*name == NUL || menu_name_equal(name, menu))
	{
	    اگر (*p != NUL && menu->children == پوچ)
	    {
		اگر (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		بازگشت FAIL;
	    }
	    اگر ((menu->modes & modes) != 0x0)
	    {
#اگر defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		اگر (*p == NUL && modes == MENU_ALL_MODES)
		{
		    اگر (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		اگر (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (*name != NUL)
	    {
		اگر (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		بازگشت FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    اگر (*name != NUL)
		بشکن;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    اگر (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    اگر ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    وگرنه
		menup = &menu->next;
	}
	وگرنه
	    menup = &menu->next;
    }
    اگر (*name != NUL)
    {
	اگر (menu == پوچ)
	{
	    اگر (!silent)
		semsg(_(e_no_menu_str), name);
	    بازگشت FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#اگر defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	اگر ((s_tearoffs) && (menu->children != پوچ)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	وگرنه
#endif
	    child = menu->children;
	برای ( ; child != پوچ; child = child->next)
	    menu->modes |= child->modes;
	اگر (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    اگر (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	اگر ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#اگر defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    اگر (s_tearoffs && menu->children != پوچ) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    بازگشت OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    تهی
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_اقلام);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    ایستا تهی
free_menu(vimmenu_T **menup)
{
    صحیح		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    اگر (gui.in_use)
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
    برای (i = 0; i < MENU_MODES; i++)
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
    ایستا تهی
free_menu_string(vimmenu_T *menu, صحیح idx)
{
    صحیح		شمار = 0;
    صحیح		i;

    برای (i = 0; i < MENU_MODES; i++)
	اگر (menu->strings[i] == menu->strings[idx])
	    شمار++;
    اگر (شمار == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = پوچ;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    ایستا صحیح
show_menus(char_u *path_name, صحیح modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = پوچ;

    name = path_name = vim_strsave(path_name);
    اگر (path_name == پوچ)
	بازگشت FAIL;
    menu = *get_ریشه_menu(path_name);

    // First, find the (sub)menu with the given name
    وقتی (*name)
    {
	p = menu_name_skip(name);
	وقتی (menu != پوچ)
	{
	    اگر (menu_name_equal(name, menu))
	    {
		// Found menu
		اگر (*p != NUL && menu->children == پوچ)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    بازگشت FAIL;
		}
		وگرنه اگر ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    بازگشت FAIL;
		}
		بشکن;
	    }
	    menu = menu->next;
	}
	اگر (menu == پوچ)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    بازگشت FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    پیام_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    بازگشت OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ایستا تهی
show_menus_recursive(vimmenu_T *menu, صحیح modes, صحیح depth)
{
    صحیح		i;
    صحیح		bit;

    اگر (menu != پوچ && (menu->modes & modes) == 0x0)
	بازگشت;

    اگر (menu != پوچ)
    {
	پیام_putchar('\n');
	اگر (got_int)		// "q" hit for "--more--"
	    بازگشت;
	برای (i = 0; i < depth; i++)
	    پیام_puts("  ");
	اگر (menu->priority)
	{
	    پیام_outnum((long)menu->priority);
	    پیام_puts(" ");
	}
				// Same highlighting as for directories!?
	پیام_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    اگر (menu != پوچ && menu->children == پوچ)
    {
	برای (bit = 0; bit < MENU_MODES; bit++)
	    اگر ((menu->modes & modes & (1 << bit)) != 0)
	    {
		پیام_putchar('\n');
		اگر (got_int)		// "q" hit for "--more--"
		    بازگشت;
		برای (i = 0; i < depth + 2; i++)
		    پیام_puts("  ");
		پیام_puts(menu_mode_chars[bit]);
		اگر (menu->noremap[bit] == REMAP_NONE)
		    پیام_putchar('*');
		وگرنه اگر (menu->noremap[bit] == REMAP_SCRIPT)
		    پیام_putchar('&');
		وگرنه
		    پیام_putchar(' ');
		اگر (menu->silent[bit])
		    پیام_putchar('s');
		وگرنه
		    پیام_putchar(' ');
		اگر ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    پیام_putchar('-');
		وگرنه
		    پیام_putchar(' ');
		پیام_puts(" ");
		اگر (*menu->strings[bit] == NUL)
		    پیام_puts_attr("<Nop>", HL_ATTR(HLF_8));
		وگرنه
		    پیام_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    وگرنه
    {
	اگر (menu == پوچ)
	{
	    menu = ریشه_menu;
	    depth--;
	}
	وگرنه
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	برای (; menu != پوچ && !got_int; menu = menu->next)
	    اگر (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
ایستا vimmenu_T	*expand_menu = پوچ;
ایستا vimmenu_T	*expand_menu_alt = پوچ;
ایستا صحیح		expand_modes = 0x0;
ایستا صحیح		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    صحیح		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = پوچ;
    char_u	*name;
    صحیح		unmenu;
    vimmenu_T	*menu;
    صحیح		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    برای (p = arg; *p; ++p)
	اگر (!VIM_ISDIGIT(*p) && *p != '.')
	    بشکن;

    اگر (!VIM_ISWHITE(*p))
    {
	اگر (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	وگرنه اگر (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	وگرنه
	    p = arg;
    }

    وقتی (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    برای (; *p && !VIM_ISWHITE(*p); ++p)
    {
	اگر ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	وگرنه اگر (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    اگر (expand_menus && VIM_ISWHITE(*p))
	بازگشت پوچ;	// TODO: check for next command?
    اگر (*p == NUL)		// Complete the menu name
    {
	صحیح try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, پوچ, &unmenu);
	اگر (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = ریشه_menu;
	اگر (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    اگر (path_name == پوچ)
		بازگشت پوچ;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	وقتی (name != پوچ && *name)
	{
	    p = menu_name_skip(name);
	    وقتی (menu != پوچ)
	    {
		اگر (menu_name_equal(name, menu))
		{
		    // Found menu
		    اگر ((*p != NUL && menu->children == پوچ)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			بازگشت پوچ;
		    }
		    بشکن;
		}
		menu = menu->next;
		اگر (menu == پوچ && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    اگر (menu == پوچ)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		بازگشت پوچ;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	اگر (expand_menu == ریشه_menu)
	    expand_menu_alt = curwin->w_winbar;
	وگرنه
	    expand_menu_alt = پوچ;
    }
    وگرنه			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    بازگشت پوچ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, صحیح idx)
{
    ایستا vimmenu_T	*menu = پوچ;
    ایستا صحیح		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    ایستا  صحیح		should_advance = FALSE;
#endif

    اگر (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    وقتی (menu != پوچ && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == پوچ))
    {
	menu = menu->next;
	اگر (menu == پوچ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    اگر (menu == پوچ)	    // at end of linked list
	بازگشت پوچ;

    اگر (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	اگر (should_advance)
	    str = menu->en_dname;
	وگرنه
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    اگر (menu->en_dname == پوچ)
		should_advance = TRUE;
	}
#endif
    وگرنه
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    اگر (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	اگر (menu == پوچ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    بازگشت str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, صحیح idx)
{
    ایستا vimmenu_T	*menu = پوچ;
    ایستا صحیح		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    ایستا char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    ایستا  صحیح		should_advance = FALSE;
#endif

    اگر (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    وقتی (menu != پوچ
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	اگر (menu == پوچ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    اگر (menu == پوچ)	    // at end of linked list
	بازگشت پوچ;

    اگر (menu->modes & expand_modes)
    {
	اگر (menu->children != پوچ)
	{
#ifdef FEAT_MULTI_LANG
	    اگر (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    وگرنه
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		اگر (menu->en_dname == پوچ)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	وگرنه
#ifdef FEAT_MULTI_LANG
	{
	    اگر (should_advance)
		str = menu->en_dname;
	    وگرنه
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		اگر (menu->en_dname == پوچ)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    وگرنه
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    اگر (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	اگر (menu == پوچ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    بازگشت str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    ایستا char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    برای (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    اگر (*p == NUL)
		بشکن;
	}
    }
    اگر (*p)
	*p++ = NUL;
    بازگشت p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ایستا صحیح
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    اگر (menu->en_name != پوچ
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	بازگشت TRUE;
#endif
    بازگشت menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ایستا صحیح
menu_namecmp(char_u *name, char_u *mname)
{
    صحیح		i;

    برای (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	اگر (name[i] != mname[i])
	    بشکن;
    بازگشت ((name[i] == NUL || name[i] == TAB)
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
    ایستا صحیح
get_menu_cmd_modes(
    char_u  *cmd,
    صحیح	    forceit,	    // Was there a "!" after the command?
    صحیح	    *noremap,
    صحیح	    *unmenu)
{
    صحیح	    modes;

    انتخاب (*cmd++)
    {
	حالت 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    بشکن;
	حالت 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    بشکن;
	حالت 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    بشکن;
	حالت 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    بشکن;
	حالت 'i':			// imenu
	    modes = MENU_درج_MODE;
	    بشکن;
	حالت 't':
	    اگر (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		بشکن;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    بشکن;
	حالت 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    بشکن;
	حالت 'a':			// amenu
	    modes = MENU_درج_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    بشکن;
	حالت 'n':
	    اگر (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		بشکن;
	    }
	    // FALLTHROUGH
	پیشفرض:
	    --cmd;
	    اگر (forceit)		// menu!!
		modes = MENU_درج_MODE | MENU_CMDLINE_MODE;
	    وگرنه			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    اگر (noremap != پوچ)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    اگر (unmenu != پوچ)
	*unmenu = (*cmd == 'u');
    بازگشت modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ایستا char_u *
get_menu_mode_str(صحیح modes)
{
    اگر ((modes & (MENU_درج_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_درج_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	بازگشت (char_u *)"a";
    اگر ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	بازگشت (char_u *)" ";
    اگر ((modes & (MENU_درج_MODE | MENU_CMDLINE_MODE))
	    == (MENU_درج_MODE | MENU_CMDLINE_MODE))
	بازگشت (char_u *)"!";
    اگر ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	بازگشت (char_u *)"v";
    اگر (modes & MENU_VISUAL_MODE)
	بازگشت (char_u *)"x";
    اگر (modes & MENU_SELECT_MODE)
	بازگشت (char_u *)"s";
    اگر (modes & MENU_OP_PENDING_MODE)
	بازگشت (char_u *)"o";
    اگر (modes & MENU_درج_MODE)
	بازگشت (char_u *)"i";
    اگر (modes & MENU_TERMINAL_MODE)
	بازگشت (char_u *)"tl";
    اگر (modes & MENU_CMDLINE_MODE)
	بازگشت (char_u *)"c";
    اگر (modes & MENU_NORMAL_MODE)
	بازگشت (char_u *)"n";
    اگر (modes & MENU_TIP_MODE)
	بازگشت (char_u *)"t";

    بازگشت (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    ایستا char_u *
popup_mode_name(char_u *name, صحیح idx)
{
    char_u	*p;
    صحیح		len = (صحیح)STRLEN(name);
    نویسه	*mode_chars = menu_mode_chars[idx];
    صحیح		mode_chars_len = (صحیح)strlen(mode_chars);
    صحیح		i;

    p = vim_strnsave(name, len + mode_chars_len);
    اگر (p == پوچ)
	بازگشت پوچ;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    برای (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    بازگشت p;
}

#اگر defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    صحیح
get_menu_index(vimmenu_T *menu, صحیح state)
{
    صحیح		idx;

    اگر ((state & MODE_درج))
	idx = MENU_INDEX_درج;
    وگرنه اگر (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    وگرنه اگر (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    وگرنه اگر (VIsual_active)
    {
	اگر (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	وگرنه
	    idx = MENU_INDEX_VISUAL;
    }
    وگرنه اگر (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    وگرنه اگر (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    وگرنه اگر ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    وگرنه
	idx = MENU_INDEX_INVALID;

    اگر (idx != MENU_INDEX_INVALID && menu->strings[idx] == پوچ)
	idx = MENU_INDEX_INVALID;
    بازگشت idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    ایستا char_u *
menu_text(char_u *str, صحیح *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    اگر (p != پوچ)
    {
	اگر (actext != پوچ)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    وگرنه
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    برای (p = text; p != پوچ; )
    {
	p = vim_strchr(p, '&');
	اگر (p != پوچ)
	{
	    اگر (p[1] == NUL)	    // trailing "&"
		بشکن;
	    اگر (mnemonic != پوچ && p[1] != '&')
#اگر !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#وگرنه
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		نویسه c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    بازگشت text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    صحیح
menu_is_menubar(char_u *name)
{
    بازگشت (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    صحیح
menu_is_popup(char_u *name)
{
    بازگشت (STRNCMP(name, "PopUp", 5) == 0);
}

#اگر defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    صحیح
menu_is_child_of_popup(vimmenu_T *menu)
{
    وقتی (menu->parent != پوچ)
	menu = menu->parent;
    بازگشت menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    صحیح
menu_is_toolbar(char_u *name)
{
    بازگشت (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    صحیح
menu_is_separator(char_u *name)
{
    بازگشت (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ایستا صحیح
menu_is_hidden(char_u *name)
{
    بازگشت (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ایستا صحیح
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    بازگشت (STRCMP(name, TEAR_STRING) == 0);
#وگرنه
    بازگشت FALSE;
#endif
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    ایستا صحیح
get_menu_mode(تهی)
{
# ifdef FEAT_TERMINAL
    اگر (term_use_loop())
	بازگشت MENU_INDEX_TERMINAL;
# endif
    اگر (VIsual_active)
    {
	اگر (VIsual_select)
	    بازگشت MENU_INDEX_SELECT;
	بازگشت MENU_INDEX_VISUAL;
    }
    اگر (State & MODE_درج)
	بازگشت MENU_INDEX_درج;
    اگر ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	بازگشت MENU_INDEX_CMDLINE;
    اگر (finish_op)
	بازگشت MENU_INDEX_OP_PENDING;
    اگر (State & MODE_NORMAL)
	بازگشت MENU_INDEX_NORMAL;
    اگر (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	بازگشت MENU_INDEX_درج;
    بازگشت MENU_INDEX_INVALID;
}

    صحیح
get_menu_mode_flag(تهی)
{
    صحیح mode = get_menu_mode();

    اگر (mode == MENU_INDEX_INVALID)
	بازگشت 0;
    بازگشت 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    تهی
show_popupmenu(تهی)
{
    vimmenu_T	*menu;
    صحیح		menu_mode;
    نویسه*	mode;
    صحیح		mode_len;

    menu_mode = get_menu_mode();
    اگر (menu_mode == MENU_INDEX_INVALID)
	بازگشت;
    mode = menu_mode_chars[menu_mode];
    mode_len = (صحیح)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, پوچ, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	اگر (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    بشکن;

    // Only show a popup when it is defined and has entries
    اگر (menu == پوچ || menu->children == پوچ)
	بازگشت;

# اگر defined(FEAT_GUI)
    اگر (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# اگر defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    وگرنه
# endif
# اگر defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#اگر defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    صحیح
check_menu_pointer(vimmenu_T *ریشه, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    برای (p = ریشه; p != پوچ; p = p->next)
	اگر (p == menu_to_check
		|| (p->children != پوچ
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    بازگشت OK;
    بازگشت FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    تهی
gui_create_initial_menus(vimmenu_T *menu)
{
    صحیح		idx = 0;

    وقتی (menu != پوچ)
    {
	// Don't add a menu when only a tip was defined.
	اگر (menu->modes & MENU_ALL_MODES)
	{
	    اگر (menu->children != پوچ)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    وگرنه
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    ایستا تهی
gui_update_menus_recurse(vimmenu_T *menu, صحیح mode)
{
    صحیح		grey;

    وقتی (menu)
    {
	اگر ((menu->modes & menu->enabled & mode)
# اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	وگرنه
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	اگر (vim_strchr(p_go, GO_GREY) != پوچ || menu->parent == پوچ
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	وگرنه
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
    تهی
gui_update_menus(صحیح modes)
{
    ایستا صحیح	    prev_mode = -1;
    صحیح		    mode = 0;

    اگر (modes != 0x0)
	mode = modes;
    وگرنه
	mode = get_menu_mode_flag();

    اگر (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(ریشه_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    صحیح
gui_is_menu_shortcut(صحیح کلید)
{
    vimmenu_T	*menu;

    اگر (کلید < 256)
	کلید = TOLOWER_LOC(کلید);
    FOR_ALL_MENUS(menu)
	اگر (menu->mnemonic == کلید
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == کلید))
	    بازگشت TRUE;
    بازگشت FALSE;
}
# endif
#endif // FEAT_GUI

#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    تهی
gui_mch_toggle_tearoffs(صحیح enable)
{
    صحیح		pri_tab[MENUDEPTH + 1];
    صحیح		i;

    اگر (enable)
    {
	برای (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(ریشه_menu, (char_u *)"", pri_tab, 0);
    }
    وگرنه
	gui_destroy_tearoffs_recurse(ریشه_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    ایستا تهی
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ثابت char_u	*pname,
    صحیح			*pri_tab,
    صحیح			pri_idx)
{
    char_u	*newpname = پوچ;
    صحیح		len;
    char_u	*s;
    char_u	*d;

    اگر (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    وقتی (menu != پوچ)
    {
	اگر (menu->children != پوچ && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (صحیح)STRLEN(pname) + (صحیح)STRLEN(menu->name);
	    برای (s = menu->name; *s; ++s)
		اگر (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    اگر (newpname != پوچ)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		برای (s = menu->name; *s; ++s)
		{
		    اگر (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		اگر (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    ایستا تهی
gui_add_tearoff(char_u *tearpath, صحیح *pri_tab, صحیح pri_idx)
{
    char_u	*tbuf;
    صحیح		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned صحیح)STRLEN(tearpath));
    اگر (tbuf == پوچ)
	بازگشت;

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
    menuarg.iconfile = پوچ;
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
    ایستا تهی
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    وقتی (menu)
    {
	اگر (menu->children)
	{
	    // check if tearoff exists
	    اگر (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    اگر (menu->children != پوچ) // if not the last one
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
    تهی
execute_menu(exarg_T *eap, vimmenu_T *menu, صحیح mode_idx)
{
    صحیح		idx = mode_idx;

    اگر (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	اگر (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_درج;
	}
#ifdef FEAT_TERMINAL
	وگرنه اگر (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	وگرنه اگر (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	وگرنه اگر (eap != پوچ && eap->addr_شمار)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    اگر ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    وگرنه
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
	    اگر (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    اگر (idx == MENU_INDEX_INVALID || eap == پوچ)
	idx = MENU_INDEX_NORMAL;

    اگر (menu->strings[idx] != پوچ && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	اگر (eap == پوچ || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    اگر (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	وگرنه
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    وگرنه اگر (eap != پوچ)
    {
	char_u	*mode;

	انتخاب (idx)
	{
	    حالت MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		بشکن;
	    حالت MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		بشکن;
	    حالت MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		بشکن;
	    حالت MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		بشکن;
	    حالت MENU_INDEX_درج:
		mode = (char_u *)"Insert";
		بشکن;
	    حالت MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		بشکن;
	    // case MENU_INDEX_TIP: cannot happen
	    پیشفرض:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    ایستا vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    صحیح		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    اگر (saved_name == پوچ)
	بازگشت پوچ;

    menu = *get_ریشه_menu(saved_name);
    name = saved_name;
    وقتی (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	وقتی (menu != پوچ)
	{
	    اگر (menu_name_equal(name, menu))
	    {
		اگر (*p == NUL && menu->children != پوچ)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = پوچ;
		}
		وگرنه اگر (*p != NUL && menu->children == پوچ)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = پوچ;
		}
		بشکن;
	    }
	    menu = menu->next;
	}
	اگر (menu == پوچ || *p == NUL)
	    بشکن;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    اگر (menu == پوچ)
    {
	اگر (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	بازگشت پوچ;
    }

    بازگشت menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    تهی
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    صحیح		mode_idx = MENU_INDEX_INVALID;

    اگر (arg[0] && VIM_ISWHITE(arg[1]))
    {
	انتخاب (arg[0])
	{
	    حالت 'n': mode_idx = MENU_INDEX_NORMAL; بشکن;
	    حالت 'v': mode_idx = MENU_INDEX_VISUAL; بشکن;
	    حالت 's': mode_idx = MENU_INDEX_SELECT; بشکن;
	    حالت 'o': mode_idx = MENU_INDEX_OP_PENDING; بشکن;
	    حالت 't': mode_idx = MENU_INDEX_TERMINAL; بشکن;
	    حالت 'i': mode_idx = MENU_INDEX_درج; بشکن;
	    حالت 'c': mode_idx = MENU_INDEX_CMDLINE; بشکن;
	    پیشفرض: semsg(_(e_invalid_argument_str), arg);
		     بازگشت;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    اگر (menu == پوچ)
	بازگشت;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    تهی
winbar_click(win_T *wp, صحیح col)
{
    صحیح		idx;

    اگر (wp->w_winbar_اقلام == پوچ)
	بازگشت;
    برای (idx = 0; wp->w_winbar_اقلام[idx].wb_menu != پوچ; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_اقلام[idx];

	اگر (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = پوچ;
	    pos_T   save_visual = VIsual;
	    صحیح	    save_visual_active = VIsual_active;
	    صحیح	    save_visual_select = VIsual_select;
	    صحیح	    save_visual_reselect = VIsual_reselect;
	    صحیح	    save_visual_mode = VIsual_mode;

	    اگر (wp != curwin)
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
	    execute_menu(پوچ, item->wb_menu, -1);

	    اگر (save_curwin != پوچ && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    اگر (!win_valid(wp))
		بشکن;
	}
    }
}

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = پوچ;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_ریشه_menu(path_name);

    saved_name = vim_strsave(path_name);
    اگر (saved_name == پوچ)
	بازگشت پوچ;

    name = saved_name;
    وقتی (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	وقتی (menu != پوچ)
	{
	    اگر (menu_name_equal(name, menu))
	    {
		اگر (menu->children == پوچ)
		{
		    // found a menu item instead of a sub-menu
		    اگر (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    وگرنه
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = پوچ;
		    برو theend;
		}
		اگر (*p == NUL)	    // found a full match
		    برو theend;
		بشکن;
	    }
	    menu = menu->next;
	}
	اگر (menu == پوچ)	// didn't find it
	    بشکن;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    اگر (menu == پوچ)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    بازگشت menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

تعریفنوع ساختار
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

ایستا garray_T menutrans_ga = {0, 0, 0, 0, پوچ};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    تهی
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    صحیح			i;
    char_u		*from, *from_noamp, *to;

    اگر (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, اندازه(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    اگر (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	برای (i = 0; i < menutrans_ga.ga_len; ++i)
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
    وگرنه
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	اگر (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	وگرنه
	{
	    اگر (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		اگر (from != پوچ)
		{
		    from_noamp = menu_text(from, پوچ, پوچ);
		    to = vim_strnsave(to, arg - to);
		    اگر (from_noamp != پوچ && to != پوچ)
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
		    وگرنه
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

#اگر defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ایستا char_u *
menu_skip_part(char_u *p)
{
    وقتی (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	اگر ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    بازگشت p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    ایستا char_u *
menutrans_lookup(char_u *name, صحیح len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    صحیح			i;
    char_u		*dname;

    برای (i = 0; i < menutrans_ga.ga_len; ++i)
	اگر (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    بازگشت tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, پوچ, پوچ);
    name[len] = i;
    اگر (dname == پوچ)
	بازگشت پوچ;

    برای (i = 0; i < menutrans_ga.ga_len; ++i)
	اگر (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    بازگشت tp[i].to;
	}
    vim_free(dname);

    بازگشت پوچ;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ایستا تهی
menu_unescape_name(char_u *name)
{
    char_u  *p;

    برای (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	اگر (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    ایستا char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    وقتی (*arg && !VIM_ISWHITE(*arg))
    {
	اگر ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	وگرنه اگر (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    اگر (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    بازگشت arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ایستا صحیح
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, صحیح modes, dict_T *dict)
{
    صحیح		status;
    list_T	*l;

    اگر (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	اگر (l == پوچ)
	    بازگشت FAIL;

	اگر (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    بازگشت FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	برای (topmenu = menu; topmenu != پوچ; topmenu = topmenu->next)
	    اگر (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	بازگشت OK;
    }

    اگر (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	بازگشت OK;

    status = dict_add_string(dict, "name", menu->name);
    اگر (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    اگر (status == OK && menu->actext != پوچ)
	status = dict_add_string(dict, "accel", menu->actext);
    اگر (status == OK)
	status = dict_add_عدد(dict, "priority", menu->priority);
    اگر (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    اگر (status == OK && menu->iconfile != پوچ)
	status = dict_add_string(dict, "icon", menu->iconfile);
    اگر (status == OK && menu->iconidx >= 0)
	status = dict_add_عدد(dict, "iconidx", menu->iconidx);
#endif
    اگر (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	صحیح	buflen;

	اگر (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	وگرنه
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    اگر (status == OK && menu->children == پوچ)
    {
	صحیح		bit;

	// Get the first mode in which the menu is available
	برای (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	اگر (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    اگر (menu->strings[bit] != پوچ)
	    {
		اگر (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		وگرنه
		{
		    char_u *tofree = پوچ;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    اگر (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    اگر (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    اگر (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    اگر (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    اگر (status == OK && menu->children != پوچ)
    {
	vimmenu_T	*child;

	l = list_alloc();
	اگر (l == پوچ)
	    بازگشت FAIL;

	اگر (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    بازگشت FAIL;
	}
	child = menu->children;
	وقتی (child)
	{
	    اگر (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    بازگشت status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    تهی
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    صحیح		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    retdict = rettv->vval.v_dict;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    menu_name = tv_get_string_chk(&argvars[0]);
    اگر (menu_name == پوچ)
	بازگشت;

    // menu mode
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    وگرنه
	which = (char_u *)"";	    // Default is modes for "menu"
    اگر (which == پوچ)
	بازگشت;

    modes = get_menu_cmd_modes(which, *which == '!', پوچ, پوچ);

    // Locate the specified menu or menu item
    menu = *get_ریشه_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    اگر (saved_name == پوچ)
	بازگشت;
    اگر (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	وقتی (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    وقتی (menu != پوچ)
	    {
		اگر (menu_name_equal(name, menu))
		    بشکن;
		menu = menu->next;
	    }
	    اگر (menu == پوچ || *p == NUL)
		بشکن;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    اگر (menu == پوچ)		// specified menu not found
	بازگشت;

    اگر (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
