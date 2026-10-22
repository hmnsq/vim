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

#𐤀𐤌 defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
ساكن صحيح add_menu_path(char_u *, vimmenu_T *, صحيح *, char_u *, صحيح);
#وإلا
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

#𐤀𐤌 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
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

#𐤀𐤌 defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
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
    أرجع (STRNCMP(name, "WinBar", 6) == 0);
}

    صحيح
winbar_height(win_T *wp)
{
    𐤀𐤌 (wp->w_winbar != عدم && wp->w_winbar->children != عدم)
	أرجع 1;
    أرجع 0;
}

    ساكن vimmenu_T **
get_root_menu(char_u *name)
{
    𐤀𐤌 (menu_is_winbar(name))
	أرجع &curwin->w_winbar;
    أرجع &root_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساكن صحيح
is_menus_locked(فراغ)
{
    𐤀𐤌 (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	أرجع TRUE;
    }
    أرجع FALSE;
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
#𐤀𐤌 defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    صحيح		old_menu_height;
# 𐤀𐤌 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
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

    لكل (;;)
    {
	𐤀𐤌 (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    استمر;
	}
	𐤀𐤌 (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    استمر;
	}
	𐤀𐤌 (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    استمر;
	}
	اكسر;
    }


    // Locate an optional "icon=filename" argument.
    𐤀𐤌 (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	طالما (*arg != NUL && *arg != ' ')
	{
	    𐤀𐤌 (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	𐤀𐤌 (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    لكل (p = arg; *p; ++p)
	𐤀𐤌 (!VIM_ISDIGIT(*p) && *p != '.')
	    اكسر;
    𐤀𐤌 (VIM_ISWHITE(*p))
    {
	لكل (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    𐤀𐤌 (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    𐤀𐤌 (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    وإلا 𐤀𐤌 (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    وإلا
	i = 0;
    طالما (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    𐤀𐤌 (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    وإلا 𐤀𐤌 (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    𐤀𐤌 (*arg == NUL)
    {
	show_menus(arg, modes);
	أرجع;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    𐤀𐤌 (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	𐤀𐤌 (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    𐤀𐤌 (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		𐤀𐤌 (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((حرف *)menu_path + 7);
		    𐤀𐤌 (menuarg.iconidx >= (صحيح)TOOLBAR_NAME_COUNT)
			menuarg.iconidx = -1;
		    وإلا
			menuarg.icon_builtin = TRUE;
		}
	    }
	    وإلا
	    {
		لكل (i = 0; i < (صحيح)TOOLBAR_NAME_COUNT; ++i)
		    𐤀𐤌 (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
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
    𐤀𐤌 (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	اذهب theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    𐤀𐤌 (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	اذهب theend;
    }
    وإلا 𐤀𐤌 (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	اذهب theend;
    }
#𐤀𐤌 defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# 𐤀𐤌 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    root_menu_ptr = get_root_menu(menu_path);
    𐤀𐤌 (root_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    𐤀𐤌 (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	𐤀𐤌 (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	𐤀𐤌 (menu_is_popup(menu_path))
	{
	    لكل (i = 0; i < MENU_INDEX_TIP; ++i)
		𐤀𐤌 (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    𐤀𐤌 (p != عدم)
		    {
			menu_nable_recurse(*root_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*root_menu_ptr, menu_path, modes, enable);
    }
    وإلا 𐤀𐤌 (unmenu)
    {
	𐤀𐤌 (is_menus_locked())
	    اذهب theend;

	/*
	 * Delete menu(s).
	 */
	𐤀𐤌 (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	𐤀𐤌 (menu_is_popup(menu_path))
	{
	    لكل (i = 0; i < MENU_INDEX_TIP; ++i)
		𐤀𐤌 (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    𐤀𐤌 (p != عدم)
		    {
			remove_menu(root_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(root_menu_ptr, menu_path, modes, FALSE);
    }
    وإلا
    {
	𐤀𐤌 (is_menus_locked())
	    اذهب theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	𐤀𐤌 (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = عدم;
	}
	وإلا 𐤀𐤌 (modes & MENU_TIP_MODE)
	    map_buf = عدم;	// Menu tips are plain text.
	وإلا
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
	𐤀𐤌 (menu_is_popup(menu_path))
	{
	    لكل (i = 0; i < MENU_INDEX_TIP; ++i)
		𐤀𐤌 (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    𐤀𐤌 (p != عدم)
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

#𐤀𐤌 defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    𐤀𐤌 (gui.in_use
	    && (gui.menu_height != old_menu_height
# 𐤀𐤌 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    𐤀𐤌 (root_menu_ptr == &curwin->w_winbar)
    {
	صحيح h = winbar_height(curwin);

	𐤀𐤌 (h != curwin->w_winbar_height)
	{
	    𐤀𐤌 (h == 0)
		++curwin->w_height;
	    وإلا 𐤀𐤌 (curwin->w_height > 0)
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
    𐤀𐤌 (path_name == عدم)
	أرجع FAIL;
    root_menu_ptr = get_root_menu(menu_path);
    menup = root_menu_ptr;
    parent = عدم;
    name = path_name;
    طالما (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (صحيح)STRLEN(name));
	𐤀𐤌 (map_to != عدم)
	{
	    en_name = name;
	    name = map_to;
	}
	وإلا
	    en_name = عدم;
#endif
	dname = menu_text(name, عدم, عدم);
	𐤀𐤌 (dname == عدم)
	    اذهب erret;
	𐤀𐤌 (*dname == NUL)
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
	طالما (menu != عدم)
	{
	    𐤀𐤌 (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		𐤀𐤌 (*next_name == NUL && menu->children != عدم)
		{
		    𐤀𐤌 (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    اذهب erret;
		}
		𐤀𐤌 (*next_name != NUL && menu->children == عدم
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    𐤀𐤌 (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    اذهب erret;
		}
		اكسر;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    𐤀𐤌 (parent != عدم || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		𐤀𐤌 (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	𐤀𐤌 (menu == عدم)
	{
	    𐤀𐤌 (*next_name == NUL && parent == عدم)
	    {
		emsg(_(e_must_not_add_menu_items_directly_to_menu_bar));
		اذهب erret;
	    }

	    𐤀𐤌 (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		اذهب erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    𐤀𐤌 (menu == عدم)
		اذهب erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    𐤀𐤌 (en_name != عدم)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, عدم, عدم);
	    }
	    وإلا
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
	    𐤀𐤌 (*next_name == NUL && menuarg->iconfile != عدم)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#𐤀𐤌 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    𐤀𐤌 (parent != عدم && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	وإلا
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    𐤀𐤌 (addtearoff)
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
	𐤀𐤌 ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    𐤀𐤌 (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		𐤀𐤌 (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		وإلا
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# 𐤀𐤌 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    𐤀𐤌 (	addtearoff
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
		𐤀𐤌 (tearpath != عدم)
		{
		    char_u  *s;
		    صحيح	    len;

		    STRCPY(tearpath, menu_path);
		    len = (صحيح)(next_name - path_name - 1);
		    لكل (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			𐤀𐤌 ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	𐤀𐤌 (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    𐤀𐤌 (sys_menu)
	modes &= ~old_modes;

    𐤀𐤌 (menu != عدم && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == عدم) ? عدم : vim_strsave(call_data);

	// loop over all modes, may add more than one
	لكل (i = 0; i < MENU_MODES; ++i)
	{
	    𐤀𐤌 (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		𐤀𐤌 (amenu && call_data != عدم && *call_data != NUL
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

		𐤀𐤌 (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    𐤀𐤌 (menu->strings[i] != عدم)
		    {
			menu->strings[i][0] = c;
			𐤀𐤌 (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			وإلا
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			𐤀𐤌 (c == Ctrl_C)
			{
			    صحيح	    len = (صحيح)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		وإلا
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#𐤀𐤌 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	𐤀𐤌 (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    أرجع OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    طالما (parent != عدم && parent->children == عدم)
    {
	𐤀𐤌 (parent->parent == عدم)
	    menup = root_menu_ptr;
	وإلا
	    menup = &parent->parent->children;
	لكل ( ; *menup != عدم && *menup != parent; menup = &((*menup)->next))
	    ;
	𐤀𐤌 (*menup == عدم) // safety check
	    اكسر;
	parent = parent->parent;
	free_menu(menup);
    }
    أرجع FAIL;
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

    𐤀𐤌 (menu == عدم)
	أرجع OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    طالما (menu != عدم)
    {
	𐤀𐤌 (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    𐤀𐤌 (*p != NUL)
	    {
		𐤀𐤌 (menu->children == عدم)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    أرجع FAIL;
		}
		𐤀𐤌 (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا
		𐤀𐤌 (enable)
		    menu->enabled |= modes;
		وإلا
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    𐤀𐤌 (*name != NUL && *name != '*')
		اكسر;
	}
	menu = menu->next;
    }
    𐤀𐤌 (*name != NUL && *name != '*' && menu == عدم)
    {
	semsg(_(e_no_menu_str), name);
	أرجع FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    أرجع OK;
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

    𐤀𐤌 (*menup == عدم)
	أرجع OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    طالما ((menu = *menup) != عدم)
    {
	𐤀𐤌 (*name == NUL || menu_name_equal(name, menu))
	{
	    𐤀𐤌 (*p != NUL && menu->children == عدم)
	    {
		𐤀𐤌 (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		أرجع FAIL;
	    }
	    𐤀𐤌 ((menu->modes & modes) != 0x0)
	    {
#𐤀𐤌 defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		𐤀𐤌 (*p == NUL && modes == MENU_ALL_MODES)
		{
		    𐤀𐤌 (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		𐤀𐤌 (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا 𐤀𐤌 (*name != NUL)
	    {
		𐤀𐤌 (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		أرجع FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    𐤀𐤌 (*name != NUL)
		اكسر;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    𐤀𐤌 (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    𐤀𐤌 ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    وإلا
		menup = &menu->next;
	}
	وإلا
	    menup = &menu->next;
    }
    𐤀𐤌 (*name != NUL)
    {
	𐤀𐤌 (menu == عدم)
	{
	    𐤀𐤌 (!silent)
		semsg(_(e_no_menu_str), name);
	    أرجع FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#𐤀𐤌 defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	𐤀𐤌 ((s_tearoffs) && (menu->children != عدم)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	وإلا
#endif
	    child = menu->children;
	لكل ( ; child != عدم; child = child->next)
	    menu->modes |= child->modes;
	𐤀𐤌 (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#𐤀𐤌 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    𐤀𐤌 (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	𐤀𐤌 ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#𐤀𐤌 defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    𐤀𐤌 (s_tearoffs && menu->children != عدم) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    أرجع OK;
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
    𐤀𐤌 (gui.in_use)
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
    لكل (i = 0; i < MENU_MODES; i++)
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

    لكل (i = 0; i < MENU_MODES; i++)
	𐤀𐤌 (menu->strings[i] == menu->strings[idx])
	    count++;
    𐤀𐤌 (count == 1)
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
    𐤀𐤌 (path_name == عدم)
	أرجع FAIL;
    menu = *get_root_menu(path_name);

    // First, find the (sub)menu with the given name
    طالما (*name)
    {
	p = menu_name_skip(name);
	طالما (menu != عدم)
	{
	    𐤀𐤌 (menu_name_equal(name, menu))
	    {
		// Found menu
		𐤀𐤌 (*p != NUL && menu->children == عدم)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    أرجع FAIL;
		}
		وإلا 𐤀𐤌 ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    أرجع FAIL;
		}
		اكسر;
	    }
	    menu = menu->next;
	}
	𐤀𐤌 (menu == عدم)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    أرجع FAIL;
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
    أرجع OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ساكن فراغ
show_menus_recursive(vimmenu_T *menu, صحيح modes, صحيح depth)
{
    صحيح		i;
    صحيح		bit;

    𐤀𐤌 (menu != عدم && (menu->modes & modes) == 0x0)
	أرجع;

    𐤀𐤌 (menu != عدم)
    {
	msg_putchar('\n');
	𐤀𐤌 (got_int)		// "q" hit for "--more--"
	    أرجع;
	لكل (i = 0; i < depth; i++)
	    msg_puts("  ");
	𐤀𐤌 (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    msg_puts(" ");
	}
				// Same highlighting as for directories!?
	msg_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    𐤀𐤌 (menu != عدم && menu->children == عدم)
    {
	لكل (bit = 0; bit < MENU_MODES; bit++)
	    𐤀𐤌 ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		𐤀𐤌 (got_int)		// "q" hit for "--more--"
		    أرجع;
		لكل (i = 0; i < depth + 2; i++)
		    msg_puts("  ");
		msg_puts(menu_mode_chars[bit]);
		𐤀𐤌 (menu->noremap[bit] == REMAP_NONE)
		    msg_putchar('*');
		وإلا 𐤀𐤌 (menu->noremap[bit] == REMAP_SCRIPT)
		    msg_putchar('&');
		وإلا
		    msg_putchar(' ');
		𐤀𐤌 (menu->silent[bit])
		    msg_putchar('s');
		وإلا
		    msg_putchar(' ');
		𐤀𐤌 ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    msg_putchar('-');
		وإلا
		    msg_putchar(' ');
		msg_puts(" ");
		𐤀𐤌 (*menu->strings[bit] == NUL)
		    msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
		وإلا
		    msg_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    وإلا
    {
	𐤀𐤌 (menu == عدم)
	{
	    menu = root_menu;
	    depth--;
	}
	وإلا
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	لكل (; menu != عدم && !got_int; menu = menu->next)
	    𐤀𐤌 (!menu_is_hidden(menu->dname))
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
    لكل (p = arg; *p; ++p)
	𐤀𐤌 (!VIM_ISDIGIT(*p) && *p != '.')
	    اكسر;

    𐤀𐤌 (!VIM_ISWHITE(*p))
    {
	𐤀𐤌 (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	وإلا 𐤀𐤌 (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	وإلا
	    p = arg;
    }

    طالما (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    لكل (; *p && !VIM_ISWHITE(*p); ++p)
    {
	𐤀𐤌 ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	وإلا 𐤀𐤌 (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    𐤀𐤌 (expand_menus && VIM_ISWHITE(*p))
	أرجع عدم;	// TODO: check for next command?
    𐤀𐤌 (*p == NUL)		// Complete the menu name
    {
	صحيح try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, عدم, &unmenu);
	𐤀𐤌 (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	𐤀𐤌 (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    𐤀𐤌 (path_name == عدم)
		أرجع عدم;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	طالما (name != عدم && *name)
	{
	    p = menu_name_skip(name);
	    طالما (menu != عدم)
	    {
		𐤀𐤌 (menu_name_equal(name, menu))
		{
		    // Found menu
		    𐤀𐤌 ((*p != NUL && menu->children == عدم)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			أرجع عدم;
		    }
		    اكسر;
		}
		menu = menu->next;
		𐤀𐤌 (menu == عدم && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    𐤀𐤌 (menu == عدم)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		أرجع عدم;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	𐤀𐤌 (expand_menu == root_menu)
	    expand_menu_alt = curwin->w_winbar;
	وإلا
	    expand_menu_alt = عدم;
    }
    وإلا			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    أرجع عدم;
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

    𐤀𐤌 (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    طالما (menu != عدم && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == عدم))
    {
	menu = menu->next;
	𐤀𐤌 (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    𐤀𐤌 (menu == عدم)	    // at end of linked list
	أرجع عدم;

    𐤀𐤌 (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	𐤀𐤌 (should_advance)
	    str = menu->en_dname;
	وإلا
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    𐤀𐤌 (menu->en_dname == عدم)
		should_advance = TRUE;
	}
#endif
    وإلا
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    𐤀𐤌 (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	𐤀𐤌 (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    أرجع str;
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

    𐤀𐤌 (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    طالما (menu != عدم
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	𐤀𐤌 (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    𐤀𐤌 (menu == عدم)	    // at end of linked list
	أرجع عدم;

    𐤀𐤌 (menu->modes & expand_modes)
    {
	𐤀𐤌 (menu->children != عدم)
	{
#ifdef FEAT_MULTI_LANG
	    𐤀𐤌 (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    وإلا
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		𐤀𐤌 (menu->en_dname == عدم)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	وإلا
#ifdef FEAT_MULTI_LANG
	{
	    𐤀𐤌 (should_advance)
		str = menu->en_dname;
	    وإلا
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		𐤀𐤌 (menu->en_dname == عدم)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    وإلا
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    𐤀𐤌 (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	𐤀𐤌 (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    أرجع str;
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

    لكل (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	𐤀𐤌 (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    𐤀𐤌 (*p == NUL)
		اكسر;
	}
    }
    𐤀𐤌 (*p)
	*p++ = NUL;
    أرجع p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ساكن صحيح
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    𐤀𐤌 (menu->en_name != عدم
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	أرجع TRUE;
#endif
    أرجع menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ساكن صحيح
menu_namecmp(char_u *name, char_u *mname)
{
    صحيح		i;

    لكل (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	𐤀𐤌 (name[i] != mname[i])
	    اكسر;
    أرجع ((name[i] == NUL || name[i] == TAB)
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
	    𐤀𐤌 (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
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
	    𐤀𐤌 (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		اكسر;
	    }
	    // FALLTHROUGH
	افتراضي:
	    --cmd;
	    𐤀𐤌 (forceit)		// menu!!
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    وإلا			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    𐤀𐤌 (noremap != عدم)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    𐤀𐤌 (unmenu != عدم)
	*unmenu = (*cmd == 'u');
    أرجع modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ساكن char_u *
get_menu_mode_str(صحيح modes)
{
    𐤀𐤌 ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	أرجع (char_u *)"a";
    𐤀𐤌 ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	أرجع (char_u *)" ";
    𐤀𐤌 ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	أرجع (char_u *)"!";
    𐤀𐤌 ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	أرجع (char_u *)"v";
    𐤀𐤌 (modes & MENU_VISUAL_MODE)
	أرجع (char_u *)"x";
    𐤀𐤌 (modes & MENU_SELECT_MODE)
	أرجع (char_u *)"s";
    𐤀𐤌 (modes & MENU_OP_PENDING_MODE)
	أرجع (char_u *)"o";
    𐤀𐤌 (modes & MENU_INSERT_MODE)
	أرجع (char_u *)"i";
    𐤀𐤌 (modes & MENU_TERMINAL_MODE)
	أرجع (char_u *)"tl";
    𐤀𐤌 (modes & MENU_CMDLINE_MODE)
	أرجع (char_u *)"c";
    𐤀𐤌 (modes & MENU_NORMAL_MODE)
	أرجع (char_u *)"n";
    𐤀𐤌 (modes & MENU_TIP_MODE)
	أرجع (char_u *)"t";

    أرجع (char_u *)"";
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
    𐤀𐤌 (p == عدم)
	أرجع عدم;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    لكل (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    أرجع p;
}

#𐤀𐤌 defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    صحيح
get_menu_index(vimmenu_T *menu, صحيح state)
{
    صحيح		idx;

    𐤀𐤌 ((state & MODE_INSERT))
	idx = MENU_INDEX_INSERT;
    وإلا 𐤀𐤌 (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    وإلا 𐤀𐤌 (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    وإلا 𐤀𐤌 (VIsual_active)
    {
	𐤀𐤌 (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	وإلا
	    idx = MENU_INDEX_VISUAL;
    }
    وإلا 𐤀𐤌 (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    وإلا 𐤀𐤌 (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    وإلا 𐤀𐤌 ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    وإلا
	idx = MENU_INDEX_INVALID;

    𐤀𐤌 (idx != MENU_INDEX_INVALID && menu->strings[idx] == عدم)
	idx = MENU_INDEX_INVALID;
    أرجع idx;
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
    𐤀𐤌 (p != عدم)
    {
	𐤀𐤌 (actext != عدم)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    وإلا
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    لكل (p = text; p != عدم; )
    {
	p = vim_strchr(p, '&');
	𐤀𐤌 (p != عدم)
	{
	    𐤀𐤌 (p[1] == NUL)	    // trailing "&"
		اكسر;
	    𐤀𐤌 (mnemonic != عدم && p[1] != '&')
#𐤀𐤌 !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#وإلا
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
    أرجع text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    صحيح
menu_is_menubar(char_u *name)
{
    أرجع (!menu_is_popup(name)
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
    أرجع (STRNCMP(name, "PopUp", 5) == 0);
}

#𐤀𐤌 defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    صحيح
menu_is_child_of_popup(vimmenu_T *menu)
{
    طالما (menu->parent != عدم)
	menu = menu->parent;
    أرجع menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    صحيح
menu_is_toolbar(char_u *name)
{
    أرجع (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    صحيح
menu_is_separator(char_u *name)
{
    أرجع (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ساكن صحيح
menu_is_hidden(char_u *name)
{
    أرجع (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ساكن صحيح
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    أرجع (STRCMP(name, TEAR_STRING) == 0);
#وإلا
    أرجع FALSE;
#endif
}

#𐤀𐤌 defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    ساكن صحيح
get_menu_mode(فراغ)
{
# ifdef FEAT_TERMINAL
    𐤀𐤌 (term_use_loop())
	أرجع MENU_INDEX_TERMINAL;
# endif
    𐤀𐤌 (VIsual_active)
    {
	𐤀𐤌 (VIsual_select)
	    أرجع MENU_INDEX_SELECT;
	أرجع MENU_INDEX_VISUAL;
    }
    𐤀𐤌 (State & MODE_INSERT)
	أرجع MENU_INDEX_INSERT;
    𐤀𐤌 ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	أرجع MENU_INDEX_CMDLINE;
    𐤀𐤌 (finish_op)
	أرجع MENU_INDEX_OP_PENDING;
    𐤀𐤌 (State & MODE_NORMAL)
	أرجع MENU_INDEX_NORMAL;
    𐤀𐤌 (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	أرجع MENU_INDEX_INSERT;
    أرجع MENU_INDEX_INVALID;
}

    صحيح
get_menu_mode_flag(فراغ)
{
    صحيح mode = get_menu_mode();

    𐤀𐤌 (mode == MENU_INDEX_INVALID)
	أرجع 0;
    أرجع 1 << mode;
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
    𐤀𐤌 (menu_mode == MENU_INDEX_INVALID)
	أرجع;
    mode = menu_mode_chars[menu_mode];
    mode_len = (صحيح)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, عدم, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	𐤀𐤌 (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    اكسر;

    // Only show a popup when it is defined and has entries
    𐤀𐤌 (menu == عدم || menu->children == عدم)
	أرجع;

# 𐤀𐤌 defined(FEAT_GUI)
    𐤀𐤌 (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# 𐤀𐤌 defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    وإلا
# endif
# 𐤀𐤌 defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#𐤀𐤌 defined(FEAT_GUI)

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

    لكل (p = root; p != عدم; p = p->next)
	𐤀𐤌 (p == menu_to_check
		|| (p->children != عدم
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    أرجع OK;
    أرجع FAIL;
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

    طالما (menu != عدم)
    {
	// Don't add a menu when only a tip was defined.
	𐤀𐤌 (menu->modes & MENU_ALL_MODES)
	{
	    𐤀𐤌 (menu->children != عدم)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    وإلا
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

    طالما (menu)
    {
	𐤀𐤌 ((menu->modes & menu->enabled & mode)
# 𐤀𐤌 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	وإلا
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	𐤀𐤌 (vim_strchr(p_go, GO_GREY) != عدم || menu->parent == عدم
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	وإلا
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

    𐤀𐤌 (modes != 0x0)
	mode = modes;
    وإلا
	mode = get_menu_mode_flag();

    𐤀𐤌 (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# 𐤀𐤌 defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    صحيح
gui_is_menu_shortcut(صحيح key)
{
    vimmenu_T	*menu;

    𐤀𐤌 (key < 256)
	key = TOLOWER_LOC(key);
    FOR_ALL_MENUS(menu)
	𐤀𐤌 (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == key))
	    أرجع TRUE;
    أرجع FALSE;
}
# endif
#endif // FEAT_GUI

#𐤀𐤌 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    صحيح		pri_tab[MENUDEPTH + 1];
    صحيح		i;

    𐤀𐤌 (enable)
    {
	لكل (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    وإلا
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

    𐤀𐤌 (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    طالما (menu != عدم)
    {
	𐤀𐤌 (menu->children != عدم && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (صحيح)STRLEN(pname) + (صحيح)STRLEN(menu->name);
	    لكل (s = menu->name; *s; ++s)
		𐤀𐤌 (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    𐤀𐤌 (newpname != عدم)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		لكل (s = menu->name; *s; ++s)
		{
		    𐤀𐤌 (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		𐤀𐤌 (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    𐤀𐤌 (tbuf == عدم)
	أرجع;

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
    طالما (menu)
    {
	𐤀𐤌 (menu->children)
	{
	    // check if tearoff exists
	    𐤀𐤌 (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    𐤀𐤌 (menu->children != عدم) // if not the last one
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

    𐤀𐤌 (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	𐤀𐤌 (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_INSERT;
	}
#ifdef FEAT_TERMINAL
	وإلا 𐤀𐤌 (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	وإلا 𐤀𐤌 (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	وإلا 𐤀𐤌 (eap != عدم && eap->addr_count)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    𐤀𐤌 ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    وإلا
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
	    𐤀𐤌 (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    𐤀𐤌 (idx == MENU_INDEX_INVALID || eap == عدم)
	idx = MENU_INDEX_NORMAL;

    𐤀𐤌 (menu->strings[idx] != عدم && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	𐤀𐤌 (eap == عدم || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    𐤀𐤌 (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	وإلا
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    وإلا 𐤀𐤌 (eap != عدم)
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
    𐤀𐤌 (saved_name == عدم)
	أرجع عدم;

    menu = *get_root_menu(saved_name);
    name = saved_name;
    طالما (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	طالما (menu != عدم)
	{
	    𐤀𐤌 (menu_name_equal(name, menu))
	    {
		𐤀𐤌 (*p == NUL && menu->children != عدم)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = عدم;
		}
		وإلا 𐤀𐤌 (*p != NUL && menu->children == عدم)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = عدم;
		}
		اكسر;
	    }
	    menu = menu->next;
	}
	𐤀𐤌 (menu == عدم || *p == NUL)
	    اكسر;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    𐤀𐤌 (menu == عدم)
    {
	𐤀𐤌 (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	أرجع عدم;
    }

    أرجع menu;
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

    𐤀𐤌 (arg[0] && VIM_ISWHITE(arg[1]))
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
		     أرجع;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    𐤀𐤌 (menu == عدم)
	أرجع;

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

    𐤀𐤌 (wp->w_winbar_items == عدم)
	أرجع;
    لكل (idx = 0; wp->w_winbar_items[idx].wb_menu != عدم; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_items[idx];

	𐤀𐤌 (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = عدم;
	    pos_T   save_visual = VIsual;
	    صحيح	    save_visual_active = VIsual_active;
	    صحيح	    save_visual_select = VIsual_select;
	    صحيح	    save_visual_reselect = VIsual_reselect;
	    صحيح	    save_visual_mode = VIsual_mode;

	    𐤀𐤌 (wp != curwin)
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

	    𐤀𐤌 (save_curwin != عدم && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    𐤀𐤌 (!win_valid(wp))
		اكسر;
	}
    }
}

#𐤀𐤌 defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
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
    𐤀𐤌 (saved_name == عدم)
	أرجع عدم;

    name = saved_name;
    طالما (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	طالما (menu != عدم)
	{
	    𐤀𐤌 (menu_name_equal(name, menu))
	    {
		𐤀𐤌 (menu->children == عدم)
		{
		    // found a menu item instead of a sub-menu
		    𐤀𐤌 (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    وإلا
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = عدم;
		    اذهب theend;
		}
		𐤀𐤌 (*p == NUL)	    // found a full match
		    اذهب theend;
		اكسر;
	    }
	    menu = menu->next;
	}
	𐤀𐤌 (menu == عدم)	// didn't find it
	    اكسر;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    𐤀𐤌 (menu == عدم)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    أرجع menu;
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

    𐤀𐤌 (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, حجم(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    𐤀𐤌 (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	لكل (i = 0; i < menutrans_ga.ga_len; ++i)
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
    وإلا
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	𐤀𐤌 (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	وإلا
	{
	    𐤀𐤌 (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		𐤀𐤌 (from != عدم)
		{
		    from_noamp = menu_text(from, عدم, عدم);
		    to = vim_strnsave(to, arg - to);
		    𐤀𐤌 (from_noamp != عدم && to != عدم)
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
		    وإلا
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

#𐤀𐤌 defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ساكن char_u *
menu_skip_part(char_u *p)
{
    طالما (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	𐤀𐤌 ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    أرجع p;
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

    لكل (i = 0; i < menutrans_ga.ga_len; ++i)
	𐤀𐤌 (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    أرجع tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, عدم, عدم);
    name[len] = i;
    𐤀𐤌 (dname == عدم)
	أرجع عدم;

    لكل (i = 0; i < menutrans_ga.ga_len; ++i)
	𐤀𐤌 (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    أرجع tp[i].to;
	}
    vim_free(dname);

    أرجع عدم;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ساكن فراغ
menu_unescape_name(char_u *name)
{
    char_u  *p;

    لكل (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	𐤀𐤌 (*p == '\\')
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

    طالما (*arg && !VIM_ISWHITE(*arg))
    {
	𐤀𐤌 ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	وإلا 𐤀𐤌 (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    𐤀𐤌 (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    أرجع arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ساكن صحيح
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, صحيح modes, dict_T *dict)
{
    صحيح		status;
    list_T	*l;

    𐤀𐤌 (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	𐤀𐤌 (l == عدم)
	    أرجع FAIL;

	𐤀𐤌 (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    أرجع FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	لكل (topmenu = menu; topmenu != عدم; topmenu = topmenu->next)
	    𐤀𐤌 (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	أرجع OK;
    }

    𐤀𐤌 (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	أرجع OK;

    status = dict_add_string(dict, "name", menu->name);
    𐤀𐤌 (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    𐤀𐤌 (status == OK && menu->actext != عدم)
	status = dict_add_string(dict, "accel", menu->actext);
    𐤀𐤌 (status == OK)
	status = dict_add_number(dict, "priority", menu->priority);
    𐤀𐤌 (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    𐤀𐤌 (status == OK && menu->iconfile != عدم)
	status = dict_add_string(dict, "icon", menu->iconfile);
    𐤀𐤌 (status == OK && menu->iconidx >= 0)
	status = dict_add_number(dict, "iconidx", menu->iconidx);
#endif
    𐤀𐤌 (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	صحيح	buflen;

	𐤀𐤌 (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	وإلا
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    𐤀𐤌 (status == OK && menu->children == عدم)
    {
	صحيح		bit;

	// Get the first mode in which the menu is available
	لكل (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	𐤀𐤌 (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    𐤀𐤌 (menu->strings[bit] != عدم)
	    {
		𐤀𐤌 (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		وإلا
		{
		    char_u *tofree = عدم;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    𐤀𐤌 (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    𐤀𐤌 (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    𐤀𐤌 (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    𐤀𐤌 (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    𐤀𐤌 (status == OK && menu->children != عدم)
    {
	vimmenu_T	*child;

	l = list_alloc();
	𐤀𐤌 (l == عدم)
	    أرجع FAIL;

	𐤀𐤌 (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    أرجع FAIL;
	}
	child = menu->children;
	طالما (child)
	{
	    𐤀𐤌 (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    أرجع status;
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

    𐤀𐤌 (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    retdict = rettv->vval.v_dict;

    𐤀𐤌 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    menu_name = tv_get_string_chk(&argvars[0]);
    𐤀𐤌 (menu_name == عدم)
	أرجع;

    // menu mode
    𐤀𐤌 (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    وإلا
	which = (char_u *)"";	    // Default is modes for "menu"
    𐤀𐤌 (which == عدم)
	أرجع;

    modes = get_menu_cmd_modes(which, *which == '!', عدم, عدم);

    // Locate the specified menu or menu item
    menu = *get_root_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    𐤀𐤌 (saved_name == عدم)
	أرجع;
    𐤀𐤌 (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	طالما (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    طالما (menu != عدم)
	    {
		𐤀𐤌 (menu_name_equal(name, menu))
		    اكسر;
		menu = menu->next;
	    }
	    𐤀𐤌 (menu == عدم || *p == NUL)
		اكسر;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    𐤀𐤌 (menu == عدم)		// specified menu not found
	أرجع;

    𐤀𐤌 (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
