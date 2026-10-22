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

#ضمن "vim.h"

#אם defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
ساكن صحيح add_menu_path(char_u *, vimmenu_T *, صحيح *, char_u *, صحيح);
#אחרת
ساكن صحيح add_menu_path(char_u *, vimmenu_T *, صحيح *, char_u *);
#endif
ساكن صحيح menu_nable_recurse(vimmenu_T *menu, char_u *name, صحيح modes, صحيح enable);
ساكن صحيح remove_menu(vimmenu_T **, char_u *, صحيح, صحيح silent);
ساكن فراغ free_menu(vimmenu_T **menup);
ساكن فراغ free_menu_string(vimmenu_T *, صحيح);
ساكن صحيح show_menus(char_u *, صحيح);
ساكن فراغ show_menus_recursive(vimmenu_T *, صحيح, صحيح);
ساكن char_u *menu_name_skip(char_u *name);
ساكن صحيح menu_name_equal(char_u *name, vimmenu_T *menu);
ساكن صحيح menu_namecmp(char_u *name, char_u *mname);
ساكن صحيح get_menu_cmd_modes(char_u *, صحيح, صحيح *, صحيح *);
ساكن char_u *popup_mode_name(char_u *name, صحيح idx);
ساكن char_u *menu_text(char_u *text, صحيح *mnemonic, char_u **actext);

#אם defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
ساكن فراغ gui_create_tearoffs_recurse(vimmenu_T *menu, ثابت char_u *pname, صحيح *pri_tab, صحيح pri_idx);
ساكن فراغ gui_add_tearoff(char_u *tearpath, صحيح *pri_tab, صحيح pri_idx);
ساكن فراغ gui_destroy_tearoffs_recurse(vimmenu_T *menu);
ساكن صحيح s_tearoffs = FALSE;
#endif

ساكن صحيح menu_is_hidden(char_u *name);
ساكن صحيح menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
ساكن صحيح menus_locked = 0;

#אם defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
ساكن char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
ساكن char_u *menutrans_lookup(char_u *name, صحيح len);
ساكن فراغ menu_unescape_name(char_u	*p);
#endif

ساكن char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
ساكن حرف *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
ساكن ثابت حرف *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_COUNT ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    ساكن صحيح
menu_is_winbar(char_u *name)
{
    החזר (STRNCMP(name, "WinBar", 6) == 0);
}

    صحيح
winbar_height(win_T *wp)
{
    אם (wp->w_winbar != عدم && wp->w_winbar->children != عدم)
	החזר 1;
    החזר 0;
}

    ساكن vimmenu_T **
get_root_menu(char_u *name)
{
    אם (menu_is_winbar(name))
	החזר &curwin->w_winbar;
    החזר &root_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساكن صحيح
is_menus_locked(فراغ)
{
    אם (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    فراغ
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    صحيح		modes;
    char_u	*map_to;
    صحيح		noremap;
    صحيح		silent = FALSE;
    صحيح		special = FALSE;
    صحيح		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    صحيح		i;
#אם defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    صحيح		old_menu_height;
# אם defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    صحيح		old_toolbar_height;
# endif
#endif
    صحيح		pri_tab[MENUDEPTH + 1];
    صحيح		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = عدم;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**root_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    עבור (;;)
    {
	אם (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    استمر;
	}
	אם (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    استمر;
	}
	אם (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    استمر;
	}
	اكسر;
    }


    // Locate an optional "icon=filename" argument.
    אם (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	כלעוד (*arg != NUL && *arg != ' ')
	{
	    אם (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	אם (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    עבור (p = arg; *p; ++p)
	אם (!VIM_ISDIGIT(*p) && *p != '.')
	    اكسر;
    אם (VIM_ISWHITE(*p))
    {
	עבור (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    אם (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    אם (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    אחרת אם (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    אחרת
	i = 0;
    כלעוד (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    אם (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    אחרת אם (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    אם (*arg == NUL)
    {
	show_menus(arg, modes);
	החזר;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    אם (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	אם (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    אם (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		אם (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((حرف *)menu_path + 7);
		    אם (menuarg.iconidx >= (صحيح)TOOLBAR_NAME_COUNT)
			menuarg.iconidx = -1;
		    אחרת
			menuarg.icon_builtin = TRUE;
		}
	    }
	    אחרת
	    {
		עבור (i = 0; i < (صحيح)TOOLBAR_NAME_COUNT; ++i)
		    אם (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			اكسر;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    אם (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	اذهب theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    אם (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	اذهب theend;
    }
    אחרת אם (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	اذهب theend;
    }
#אם defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# אם defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    root_menu_ptr = get_root_menu(menu_path);
    אם (root_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    אם (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	אם (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	אם (menu_is_popup(menu_path))
	{
	    עבור (i = 0; i < MENU_INDEX_TIP; ++i)
		אם (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    אם (p != عدم)
		    {
			menu_nable_recurse(*root_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*root_menu_ptr, menu_path, modes, enable);
    }
    אחרת אם (unmenu)
    {
	אם (is_menus_locked())
	    اذهب theend;

	/*
	 * Delete menu(s).
	 */
	אם (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	אם (menu_is_popup(menu_path))
	{
	    עבור (i = 0; i < MENU_INDEX_TIP; ++i)
		אם (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    אם (p != عدم)
		    {
			remove_menu(root_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(root_menu_ptr, menu_path, modes, FALSE);
    }
    אחרת
    {
	אם (is_menus_locked())
	    اذهب theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	אם (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = عدم;
	}
	אחרת אם (modes & MENU_TIP_MODE)
	    map_buf = عدم;	// Menu tips are plain text.
	אחרת
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), عدم);
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
	אם (menu_is_popup(menu_path))
	{
	    עבור (i = 0; i < MENU_INDEX_TIP; ++i)
		אם (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    אם (p != عدم)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = عدم;
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

#אם defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    אם (gui.in_use
	    && (gui.menu_height != old_menu_height
# אם defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    אם (root_menu_ptr == &curwin->w_winbar)
    {
	صحيح h = winbar_height(curwin);

	אם (h != curwin->w_winbar_height)
	{
	    אם (h == 0)
		++curwin->w_height;
	    אחרת אם (curwin->w_height > 0)
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
    ساكن صحيح
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    صحيح		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , صحيح	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    صحيح		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = عدم;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    صحيح		i;
    صحيح		c;
    صحيح		d;
#ifdef FEAT_GUI
    صحيح		idx;
    صحيح		new_idx;
#endif
    صحيح		pri_idx = 0;
    صحيح		old_modes = 0;
    صحيح		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = عدم;
#endif
    vimmenu_T	**root_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    אם (path_name == عدم)
	החזר FAIL;
    root_menu_ptr = get_root_menu(menu_path);
    menup = root_menu_ptr;
    parent = عدم;
    name = path_name;
    כלעוד (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (صحيح)STRLEN(name));
	אם (map_to != عدم)
	{
	    en_name = name;
	    name = map_to;
	}
	אחרת
	    en_name = عدم;
#endif
	dname = menu_text(name, عدم, عدم);
	אם (dname == عدم)
	    اذهب erret;
	אם (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    اذهب erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	כלעוד (menu != عدم)
	{
	    אם (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		אם (*next_name == NUL && menu->children != عدم)
		{
		    אם (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    اذهب erret;
		}
		אם (*next_name != NUL && menu->children == عدم
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    אם (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    اذهب erret;
		}
		اكسر;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    אם (parent != عدم || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		אם (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	אם (menu == عدم)
	{
	    אם (*next_name == NUL && parent == عدم)
	    {
		emsg(_(e_must_not_add_menu_items_directly_to_menu_bar));
		اذهب erret;
	    }

	    אם (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		اذهب erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    אם (menu == عدم)
		اذهب erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    אם (en_name != عدم)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, عدم, عدم);
	    }
	    אחרת
	    {
		menu->en_name = عدم;
		menu->en_dname = عدم;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = عدم;
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
	    אם (*next_name == NUL && menuarg->iconfile != عدم)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#אם defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    אם (parent != عدم && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	אחרת
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    אם (addtearoff)
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
	אם ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    אם (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		אם (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		אחרת
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# אם defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    אם (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != عدم
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
		אם (tearpath != عدم)
		{
		    char_u  *s;
		    صحيح	    len;

		    STRCPY(tearpath, menu_path);
		    len = (صحيح)(next_name - path_name - 1);
		    עבור (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			אם ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	אם (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    אם (sys_menu)
	modes &= ~old_modes;

    אם (menu != عدم && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == عدم) ? عدم : vim_strsave(call_data);

	// loop over all modes, may add more than one
	עבור (i = 0; i < MENU_MODES; ++i)
	{
	    אם (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		אם (amenu && call_data != عدم && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    بدل (1 << i)
		    {
			حالة MENU_VISUAL_MODE:
			حالة MENU_SELECT_MODE:
			حالة MENU_OP_PENDING_MODE:
			حالة MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    اكسر;
			حالة MENU_INSERT_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    اكسر;
		    }
		}

		אם (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    אם (menu->strings[i] != عدم)
		    {
			menu->strings[i][0] = c;
			אם (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			אחרת
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			אם (c == Ctrl_C)
			{
			    صحيح	    len = (صحيح)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		אחרת
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#אם defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	אם (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    החזר OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    כלעוד (parent != عدم && parent->children == عدم)
    {
	אם (parent->parent == عدم)
	    menup = root_menu_ptr;
	אחרת
	    menup = &parent->parent->children;
	עבור ( ; *menup != عدم && *menup != parent; menup = &((*menup)->next))
	    ;
	אם (*menup == عدم) // safety check
	    اكسر;
	parent = parent->parent;
	free_menu(menup);
    }
    החזר FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    ساكن صحيح
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    صحيح		modes,
    صحيح		enable)
{
    char_u	*p;

    אם (menu == عدم)
	החזר OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    כלעוד (menu != عدم)
    {
	אם (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    אם (*p != NUL)
	    {
		אם (menu->children == عدم)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    החזר FAIL;
		}
		אם (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    החזר FAIL;
	    }
	    אחרת
		אם (enable)
		    menu->enabled |= modes;
		אחרת
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    אם (*name != NUL && *name != '*')
		اكسر;
	}
	menu = menu->next;
    }
    אם (*name != NUL && *name != '*' && menu == عدم)
    {
	semsg(_(e_no_menu_str), name);
	החזר FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    החזר OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    ساكن صحيح
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    صحيح		modes,
    صحيح		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    אם (*menup == عدم)
	החזר OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    כלעוד ((menu = *menup) != عدم)
    {
	אם (*name == NUL || menu_name_equal(name, menu))
	{
	    אם (*p != NUL && menu->children == عدم)
	    {
		אם (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		החזר FAIL;
	    }
	    אם ((menu->modes & modes) != 0x0)
	    {
#אם defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		אם (*p == NUL && modes == MENU_ALL_MODES)
		{
		    אם (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		אם (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    החזר FAIL;
	    }
	    אחרת אם (*name != NUL)
	    {
		אם (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		החזר FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    אם (*name != NUL)
		اكسر;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    אם (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    אם ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    אחרת
		menup = &menu->next;
	}
	אחרת
	    menup = &menu->next;
    }
    אם (*name != NUL)
    {
	אם (menu == عدم)
	{
	    אם (!silent)
		semsg(_(e_no_menu_str), name);
	    החזר FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#אם defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	אם ((s_tearoffs) && (menu->children != عدم)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	אחרת
#endif
	    child = menu->children;
	עבור ( ; child != عدم; child = child->next)
	    menu->modes |= child->modes;
	אם (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#אם defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    אם (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	אם ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#אם defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    אם (s_tearoffs && menu->children != عدم) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    החזר OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    فراغ
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_items);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    ساكن فراغ
free_menu(vimmenu_T **menup)
{
    صحيح		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    אם (gui.in_use)
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
    עבור (i = 0; i < MENU_MODES; i++)
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
    ساكن فراغ
free_menu_string(vimmenu_T *menu, صحيح idx)
{
    صحيح		count = 0;
    صحيح		i;

    עבור (i = 0; i < MENU_MODES; i++)
	אם (menu->strings[i] == menu->strings[idx])
	    count++;
    אם (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = عدم;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    ساكن صحيح
show_menus(char_u *path_name, صحيح modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = عدم;

    name = path_name = vim_strsave(path_name);
    אם (path_name == عدم)
	החזר FAIL;
    menu = *get_root_menu(path_name);

    // First, find the (sub)menu with the given name
    כלעוד (*name)
    {
	p = menu_name_skip(name);
	כלעוד (menu != عدم)
	{
	    אם (menu_name_equal(name, menu))
	    {
		// Found menu
		אם (*p != NUL && menu->children == عدم)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    החזר FAIL;
		}
		אחרת אם ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    החזר FAIL;
		}
		اكسر;
	    }
	    menu = menu->next;
	}
	אם (menu == عدم)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    החזר FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    msg_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    החזר OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ساكن فراغ
show_menus_recursive(vimmenu_T *menu, صحيح modes, صحيح depth)
{
    صحيح		i;
    صحيح		bit;

    אם (menu != عدم && (menu->modes & modes) == 0x0)
	החזר;

    אם (menu != عدم)
    {
	msg_putchar('\n');
	אם (got_int)		// "q" hit for "--more--"
	    החזר;
	עבור (i = 0; i < depth; i++)
	    msg_puts("  ");
	אם (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    msg_puts(" ");
	}
				// Same highlighting as for directories!?
	msg_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    אם (menu != عدم && menu->children == عدم)
    {
	עבור (bit = 0; bit < MENU_MODES; bit++)
	    אם ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		אם (got_int)		// "q" hit for "--more--"
		    החזר;
		עבור (i = 0; i < depth + 2; i++)
		    msg_puts("  ");
		msg_puts(menu_mode_chars[bit]);
		אם (menu->noremap[bit] == REMAP_NONE)
		    msg_putchar('*');
		אחרת אם (menu->noremap[bit] == REMAP_SCRIPT)
		    msg_putchar('&');
		אחרת
		    msg_putchar(' ');
		אם (menu->silent[bit])
		    msg_putchar('s');
		אחרת
		    msg_putchar(' ');
		אם ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    msg_putchar('-');
		אחרת
		    msg_putchar(' ');
		msg_puts(" ");
		אם (*menu->strings[bit] == NUL)
		    msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
		אחרת
		    msg_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    אחרת
    {
	אם (menu == عدم)
	{
	    menu = root_menu;
	    depth--;
	}
	אחרת
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	עבור (; menu != عدم && !got_int; menu = menu->next)
	    אם (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
ساكن vimmenu_T	*expand_menu = عدم;
ساكن vimmenu_T	*expand_menu_alt = عدم;
ساكن صحيح		expand_modes = 0x0;
ساكن صحيح		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    صحيح		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = عدم;
    char_u	*name;
    صحيح		unmenu;
    vimmenu_T	*menu;
    صحيح		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    עבור (p = arg; *p; ++p)
	אם (!VIM_ISDIGIT(*p) && *p != '.')
	    اكسر;

    אם (!VIM_ISWHITE(*p))
    {
	אם (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	אחרת אם (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	אחרת
	    p = arg;
    }

    כלעוד (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    עבור (; *p && !VIM_ISWHITE(*p); ++p)
    {
	אם ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	אחרת אם (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    אם (expand_menus && VIM_ISWHITE(*p))
	החזר عدم;	// TODO: check for next command?
    אם (*p == NUL)		// Complete the menu name
    {
	صحيح try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, عدم, &unmenu);
	אם (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	אם (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    אם (path_name == عدم)
		החזר عدم;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	כלעוד (name != عدم && *name)
	{
	    p = menu_name_skip(name);
	    כלעוד (menu != عدم)
	    {
		אם (menu_name_equal(name, menu))
		{
		    // Found menu
		    אם ((*p != NUL && menu->children == عدم)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			החזר عدم;
		    }
		    اكسر;
		}
		menu = menu->next;
		אם (menu == عدم && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    אם (menu == عدم)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		החזר عدم;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	אם (expand_menu == root_menu)
	    expand_menu_alt = curwin->w_winbar;
	אחרת
	    expand_menu_alt = عدم;
    }
    אחרת			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    החזר عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, صحيح idx)
{
    ساكن vimmenu_T	*menu = عدم;
    ساكن صحيح		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    ساكن  صحيح		should_advance = FALSE;
#endif

    אם (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    כלעוד (menu != عدم && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == عدم))
    {
	menu = menu->next;
	אם (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    אם (menu == عدم)	    // at end of linked list
	החזר عدم;

    אם (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	אם (should_advance)
	    str = menu->en_dname;
	אחרת
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    אם (menu->en_dname == عدم)
		should_advance = TRUE;
	}
#endif
    אחרת
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    אם (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	אם (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    החזר str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, صحيح idx)
{
    ساكن vimmenu_T	*menu = عدم;
    ساكن صحيح		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    ساكن char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    ساكن  صحيح		should_advance = FALSE;
#endif

    אם (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    כלעוד (menu != عدم
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	אם (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    אם (menu == عدم)	    // at end of linked list
	החזר عدم;

    אם (menu->modes & expand_modes)
    {
	אם (menu->children != عدم)
	{
#ifdef FEAT_MULTI_LANG
	    אם (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    אחרת
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		אם (menu->en_dname == عدم)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	אחרת
#ifdef FEAT_MULTI_LANG
	{
	    אם (should_advance)
		str = menu->en_dname;
	    אחרת
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		אם (menu->en_dname == عدم)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    אחרת
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    אם (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	אם (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    החזר str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    ساكن char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    עבור (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	אם (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    אם (*p == NUL)
		اكسر;
	}
    }
    אם (*p)
	*p++ = NUL;
    החזר p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ساكن صحيح
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    אם (menu->en_name != عدم
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	החזר TRUE;
#endif
    החזר menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ساكن صحيح
menu_namecmp(char_u *name, char_u *mname)
{
    صحيح		i;

    עבור (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	אם (name[i] != mname[i])
	    اكسر;
    החזר ((name[i] == NUL || name[i] == TAB)
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
    ساكن صحيح
get_menu_cmd_modes(
    char_u  *cmd,
    صحيح	    forceit,	    // Was there a "!" after the command?
    صحيح	    *noremap,
    صحيح	    *unmenu)
{
    صحيح	    modes;

    بدل (*cmd++)
    {
	حالة 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    اكسر;
	حالة 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    اكسر;
	حالة 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    اكسر;
	حالة 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    اكسر;
	حالة 'i':			// imenu
	    modes = MENU_INSERT_MODE;
	    اكسر;
	حالة 't':
	    אם (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		اكسر;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    اكسر;
	حالة 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    اكسر;
	حالة 'a':			// amenu
	    modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    اكسر;
	حالة 'n':
	    אם (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		اكسر;
	    }
	    // FALLTHROUGH
	افتراضي:
	    --cmd;
	    אם (forceit)		// menu!!
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    אחרת			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    אם (noremap != عدم)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    אם (unmenu != عدم)
	*unmenu = (*cmd == 'u');
    החזר modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ساكن char_u *
get_menu_mode_str(صحيح modes)
{
    אם ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	החזר (char_u *)"a";
    אם ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	החזר (char_u *)" ";
    אם ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	החזר (char_u *)"!";
    אם ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	החזר (char_u *)"v";
    אם (modes & MENU_VISUAL_MODE)
	החזר (char_u *)"x";
    אם (modes & MENU_SELECT_MODE)
	החזר (char_u *)"s";
    אם (modes & MENU_OP_PENDING_MODE)
	החזר (char_u *)"o";
    אם (modes & MENU_INSERT_MODE)
	החזר (char_u *)"i";
    אם (modes & MENU_TERMINAL_MODE)
	החזר (char_u *)"tl";
    אם (modes & MENU_CMDLINE_MODE)
	החזר (char_u *)"c";
    אם (modes & MENU_NORMAL_MODE)
	החזר (char_u *)"n";
    אם (modes & MENU_TIP_MODE)
	החזר (char_u *)"t";

    החזר (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    ساكن char_u *
popup_mode_name(char_u *name, صحيح idx)
{
    char_u	*p;
    صحيح		len = (صحيح)STRLEN(name);
    حرف	*mode_chars = menu_mode_chars[idx];
    صحيح		mode_chars_len = (صحيح)strlen(mode_chars);
    صحيح		i;

    p = vim_strnsave(name, len + mode_chars_len);
    אם (p == عدم)
	החזר عدم;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    עבור (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    החזר p;
}

#אם defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    صحيح
get_menu_index(vimmenu_T *menu, صحيح state)
{
    صحيح		idx;

    אם ((state & MODE_INSERT))
	idx = MENU_INDEX_INSERT;
    אחרת אם (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    אחרת אם (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    אחרת אם (VIsual_active)
    {
	אם (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	אחרת
	    idx = MENU_INDEX_VISUAL;
    }
    אחרת אם (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    אחרת אם (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    אחרת אם ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    אחרת
	idx = MENU_INDEX_INVALID;

    אם (idx != MENU_INDEX_INVALID && menu->strings[idx] == عدم)
	idx = MENU_INDEX_INVALID;
    החזר idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    ساكن char_u *
menu_text(char_u *str, صحيح *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    אם (p != عدم)
    {
	אם (actext != عدم)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    אחרת
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    עבור (p = text; p != عدم; )
    {
	p = vim_strchr(p, '&');
	אם (p != عدم)
	{
	    אם (p[1] == NUL)	    // trailing "&"
		اكسر;
	    אם (mnemonic != عدم && p[1] != '&')
#אם !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#אחרת
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		حرف c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    החזר text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    صحيح
menu_is_menubar(char_u *name)
{
    החזר (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    صحيح
menu_is_popup(char_u *name)
{
    החזר (STRNCMP(name, "PopUp", 5) == 0);
}

#אם defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    صحيح
menu_is_child_of_popup(vimmenu_T *menu)
{
    כלעוד (menu->parent != عدم)
	menu = menu->parent;
    החזר menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    صحيح
menu_is_toolbar(char_u *name)
{
    החזר (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    صحيح
menu_is_separator(char_u *name)
{
    החזר (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ساكن صحيح
menu_is_hidden(char_u *name)
{
    החזר (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ساكن صحيح
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    החזר (STRCMP(name, TEAR_STRING) == 0);
#אחרת
    החזר FALSE;
#endif
}

#אם defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    ساكن صحيح
get_menu_mode(فراغ)
{
# ifdef FEAT_TERMINAL
    אם (term_use_loop())
	החזר MENU_INDEX_TERMINAL;
# endif
    אם (VIsual_active)
    {
	אם (VIsual_select)
	    החזר MENU_INDEX_SELECT;
	החזר MENU_INDEX_VISUAL;
    }
    אם (State & MODE_INSERT)
	החזר MENU_INDEX_INSERT;
    אם ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	החזר MENU_INDEX_CMDLINE;
    אם (finish_op)
	החזר MENU_INDEX_OP_PENDING;
    אם (State & MODE_NORMAL)
	החזר MENU_INDEX_NORMAL;
    אם (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	החזר MENU_INDEX_INSERT;
    החזר MENU_INDEX_INVALID;
}

    صحيح
get_menu_mode_flag(فراغ)
{
    صحيح mode = get_menu_mode();

    אם (mode == MENU_INDEX_INVALID)
	החזר 0;
    החזר 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    فراغ
show_popupmenu(فراغ)
{
    vimmenu_T	*menu;
    صحيح		menu_mode;
    حرف*	mode;
    صحيح		mode_len;

    menu_mode = get_menu_mode();
    אם (menu_mode == MENU_INDEX_INVALID)
	החזר;
    mode = menu_mode_chars[menu_mode];
    mode_len = (صحيح)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, عدم, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	אם (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    اكسر;

    // Only show a popup when it is defined and has entries
    אם (menu == عدم || menu->children == عدم)
	החזר;

# אם defined(FEAT_GUI)
    אם (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# אם defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    אחרת
# endif
# אם defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#אם defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    صحيح
check_menu_pointer(vimmenu_T *root, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    עבור (p = root; p != عدم; p = p->next)
	אם (p == menu_to_check
		|| (p->children != عدم
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    החזר OK;
    החזר FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    فراغ
gui_create_initial_menus(vimmenu_T *menu)
{
    صحيح		idx = 0;

    כלעוד (menu != عدم)
    {
	// Don't add a menu when only a tip was defined.
	אם (menu->modes & MENU_ALL_MODES)
	{
	    אם (menu->children != عدم)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    אחרת
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    ساكن فراغ
gui_update_menus_recurse(vimmenu_T *menu, صحيح mode)
{
    صحيح		grey;

    כלעוד (menu)
    {
	אם ((menu->modes & menu->enabled & mode)
# אם defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	אחרת
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	אם (vim_strchr(p_go, GO_GREY) != عدم || menu->parent == عدم
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	אחרת
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
    فراغ
gui_update_menus(صحيح modes)
{
    ساكن صحيح	    prev_mode = -1;
    صحيح		    mode = 0;

    אם (modes != 0x0)
	mode = modes;
    אחרת
	mode = get_menu_mode_flag();

    אם (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# אם defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    صحيح
gui_is_menu_shortcut(صحيح key)
{
    vimmenu_T	*menu;

    אם (key < 256)
	key = TOLOWER_LOC(key);
    FOR_ALL_MENUS(menu)
	אם (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == key))
	    החזר TRUE;
    החזר FALSE;
}
# endif
#endif // FEAT_GUI

#אם defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    صحيح		pri_tab[MENUDEPTH + 1];
    صحيح		i;

    אם (enable)
    {
	עבור (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    אחרת
	gui_destroy_tearoffs_recurse(root_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    ساكن فراغ
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ثابت char_u	*pname,
    صحيح			*pri_tab,
    صحيح			pri_idx)
{
    char_u	*newpname = عدم;
    صحيح		len;
    char_u	*s;
    char_u	*d;

    אם (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    כלעוד (menu != عدم)
    {
	אם (menu->children != عدم && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (صحيح)STRLEN(pname) + (صحيح)STRLEN(menu->name);
	    עבור (s = menu->name; *s; ++s)
		אם (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    אם (newpname != عدم)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		עבור (s = menu->name; *s; ++s)
		{
		    אם (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		אם (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    ساكن فراغ
gui_add_tearoff(char_u *tearpath, صحيح *pri_tab, صحيح pri_idx)
{
    char_u	*tbuf;
    صحيح		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned صحيح)STRLEN(tearpath));
    אם (tbuf == عدم)
	החזר;

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
    menuarg.iconfile = عدم;
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
    ساكن فراغ
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    כלעוד (menu)
    {
	אם (menu->children)
	{
	    // check if tearoff exists
	    אם (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    אם (menu->children != عدم) // if not the last one
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
    فراغ
execute_menu(exarg_T *eap, vimmenu_T *menu, صحيح mode_idx)
{
    صحيح		idx = mode_idx;

    אם (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	אם (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_INSERT;
	}
#ifdef FEAT_TERMINAL
	אחרת אם (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	אחרת אם (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	אחרת אם (eap != عدم && eap->addr_count)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    אם ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    אחרת
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
	    אם (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    אם (idx == MENU_INDEX_INVALID || eap == عدم)
	idx = MENU_INDEX_NORMAL;

    אם (menu->strings[idx] != عدم && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	אם (eap == عدم || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    אם (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	אחרת
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    אחרת אם (eap != عدم)
    {
	char_u	*mode;

	بدل (idx)
	{
	    حالة MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		اكسر;
	    حالة MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		اكسر;
	    حالة MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		اكسر;
	    حالة MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		اكسر;
	    حالة MENU_INDEX_INSERT:
		mode = (char_u *)"Insert";
		اكسر;
	    حالة MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		اكسر;
	    // case MENU_INDEX_TIP: cannot happen
	    افتراضي:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    ساكن vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    صحيح		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    אם (saved_name == عدم)
	החזר عدم;

    menu = *get_root_menu(saved_name);
    name = saved_name;
    כלעוד (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	כלעוד (menu != عدم)
	{
	    אם (menu_name_equal(name, menu))
	    {
		אם (*p == NUL && menu->children != عدم)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = عدم;
		}
		אחרת אם (*p != NUL && menu->children == عدم)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = عدم;
		}
		اكسر;
	    }
	    menu = menu->next;
	}
	אם (menu == عدم || *p == NUL)
	    اكسر;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    אם (menu == عدم)
    {
	אם (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	החזר عدم;
    }

    החזר menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    فراغ
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    صحيح		mode_idx = MENU_INDEX_INVALID;

    אם (arg[0] && VIM_ISWHITE(arg[1]))
    {
	بدل (arg[0])
	{
	    حالة 'n': mode_idx = MENU_INDEX_NORMAL; اكسر;
	    حالة 'v': mode_idx = MENU_INDEX_VISUAL; اكسر;
	    حالة 's': mode_idx = MENU_INDEX_SELECT; اكسر;
	    حالة 'o': mode_idx = MENU_INDEX_OP_PENDING; اكسر;
	    حالة 't': mode_idx = MENU_INDEX_TERMINAL; اكسر;
	    حالة 'i': mode_idx = MENU_INDEX_INSERT; اكسر;
	    حالة 'c': mode_idx = MENU_INDEX_CMDLINE; اكسر;
	    افتراضي: semsg(_(e_invalid_argument_str), arg);
		     החזר;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    אם (menu == عدم)
	החזר;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    فراغ
winbar_click(win_T *wp, صحيح col)
{
    صحيح		idx;

    אם (wp->w_winbar_items == عدم)
	החזר;
    עבור (idx = 0; wp->w_winbar_items[idx].wb_menu != عدم; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_items[idx];

	אם (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = عدم;
	    pos_T   save_visual = VIsual;
	    صحيح	    save_visual_active = VIsual_active;
	    صحيح	    save_visual_select = VIsual_select;
	    صحيح	    save_visual_reselect = VIsual_reselect;
	    صحيح	    save_visual_mode = VIsual_mode;

	    אם (wp != curwin)
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
	    execute_menu(عدم, item->wb_menu, -1);

	    אם (save_curwin != عدم && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    אם (!win_valid(wp))
		اكسر;
	}
    }
}

#אם defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = عدم;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_root_menu(path_name);

    saved_name = vim_strsave(path_name);
    אם (saved_name == عدم)
	החזר عدم;

    name = saved_name;
    כלעוד (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	כלעוד (menu != عدم)
	{
	    אם (menu_name_equal(name, menu))
	    {
		אם (menu->children == عدم)
		{
		    // found a menu item instead of a sub-menu
		    אם (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    אחרת
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = عدم;
		    اذهب theend;
		}
		אם (*p == NUL)	    // found a full match
		    اذهب theend;
		اكسر;
	    }
	    menu = menu->next;
	}
	אם (menu == عدم)	// didn't find it
	    اكسر;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    אם (menu == عدم)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    החזר menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

تعريفنوع بنية
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

ساكن garray_T menutrans_ga = {0, 0, 0, 0, عدم};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    فراغ
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    صحيح			i;
    char_u		*from, *from_noamp, *to;

    אם (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, حجم(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    אם (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	עבור (i = 0; i < menutrans_ga.ga_len; ++i)
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
    אחרת
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	אם (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	אחרת
	{
	    אם (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		אם (from != عدم)
		{
		    from_noamp = menu_text(from, عدم, عدم);
		    to = vim_strnsave(to, arg - to);
		    אם (from_noamp != عدم && to != عدم)
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
		    אחרת
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

#אם defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ساكن char_u *
menu_skip_part(char_u *p)
{
    כלעוד (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	אם ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    החזר p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    ساكن char_u *
menutrans_lookup(char_u *name, صحيح len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    صحيح			i;
    char_u		*dname;

    עבור (i = 0; i < menutrans_ga.ga_len; ++i)
	אם (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    החזר tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, عدم, عدم);
    name[len] = i;
    אם (dname == عدم)
	החזר عدم;

    עבור (i = 0; i < menutrans_ga.ga_len; ++i)
	אם (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    החזר tp[i].to;
	}
    vim_free(dname);

    החזר عدم;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ساكن فراغ
menu_unescape_name(char_u *name)
{
    char_u  *p;

    עבור (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	אם (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    ساكن char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    כלעוד (*arg && !VIM_ISWHITE(*arg))
    {
	אם ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	אחרת אם (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    אם (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    החזר arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ساكن صحيح
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, صحيح modes, dict_T *dict)
{
    صحيح		status;
    list_T	*l;

    אם (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	אם (l == عدم)
	    החזר FAIL;

	אם (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    החזר FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	עבור (topmenu = menu; topmenu != عدم; topmenu = topmenu->next)
	    אם (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	החזר OK;
    }

    אם (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	החזר OK;

    status = dict_add_string(dict, "name", menu->name);
    אם (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    אם (status == OK && menu->actext != عدم)
	status = dict_add_string(dict, "accel", menu->actext);
    אם (status == OK)
	status = dict_add_number(dict, "priority", menu->priority);
    אם (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    אם (status == OK && menu->iconfile != عدم)
	status = dict_add_string(dict, "icon", menu->iconfile);
    אם (status == OK && menu->iconidx >= 0)
	status = dict_add_number(dict, "iconidx", menu->iconidx);
#endif
    אם (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	صحيح	buflen;

	אם (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	אחרת
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    אם (status == OK && menu->children == عدم)
    {
	صحيح		bit;

	// Get the first mode in which the menu is available
	עבור (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	אם (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    אם (menu->strings[bit] != عدم)
	    {
		אם (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		אחרת
		{
		    char_u *tofree = عدم;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    אם (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    אם (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    אם (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    אם (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    אם (status == OK && menu->children != عدم)
    {
	vimmenu_T	*child;

	l = list_alloc();
	אם (l == عدم)
	    החזר FAIL;

	אם (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    החזר FAIL;
	}
	child = menu->children;
	כלעוד (child)
	{
	    אם (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    החזר status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    فراغ
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    صحيح		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    אם (rettv_dict_alloc(rettv) == FAIL)
	החזר;
    retdict = rettv->vval.v_dict;

    אם (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	החזר;

    menu_name = tv_get_string_chk(&argvars[0]);
    אם (menu_name == عدم)
	החזר;

    // menu mode
    אם (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    אחרת
	which = (char_u *)"";	    // Default is modes for "menu"
    אם (which == عدم)
	החזר;

    modes = get_menu_cmd_modes(which, *which == '!', عدم, عدم);

    // Locate the specified menu or menu item
    menu = *get_root_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    אם (saved_name == عدم)
	החזר;
    אם (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	כלעוד (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    כלעוד (menu != عدم)
	    {
		אם (menu_name_equal(name, menu))
		    اكسر;
		menu = menu->next;
	    }
	    אם (menu == عدم || *p == NUL)
		اكسر;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    אם (menu == عدم)		// specified menu not found
	החזר;

    אם (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
