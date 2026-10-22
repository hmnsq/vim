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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
স্থির পূর্ণসংখ্যা add_menu_path(char_u *, vimmenu_T *, পূর্ণসংখ্যা *, char_u *, পূর্ণসংখ্যা);
#অন্যথা
স্থির পূর্ণসংখ্যা add_menu_path(char_u *, vimmenu_T *, পূর্ণসংখ্যা *, char_u *);
#endif
স্থির পূর্ণসংখ্যা menu_nable_recurse(vimmenu_T *menu, char_u *name, পূর্ণসংখ্যা modes, পূর্ণসংখ্যা enable);
স্থির পূর্ণসংখ্যা remove_menu(vimmenu_T **, char_u *, পূর্ণসংখ্যা, পূর্ণসংখ্যা silent);
স্থির শূন্য free_menu(vimmenu_T **menup);
স্থির শূন্য free_menu_string(vimmenu_T *, পূর্ণসংখ্যা);
স্থির পূর্ণসংখ্যা show_menus(char_u *, পূর্ণসংখ্যা);
স্থির শূন্য show_menus_recursive(vimmenu_T *, পূর্ণসংখ্যা, পূর্ণসংখ্যা);
স্থির char_u *menu_name_skip(char_u *name);
স্থির পূর্ণসংখ্যা menu_name_equal(char_u *name, vimmenu_T *menu);
স্থির পূর্ণসংখ্যা menu_namecmp(char_u *name, char_u *mname);
স্থির পূর্ণসংখ্যা get_menu_cmd_modes(char_u *, পূর্ণসংখ্যা, পূর্ণসংখ্যা *, পূর্ণসংখ্যা *);
স্থির char_u *popup_mode_name(char_u *name, পূর্ণসংখ্যা idx);
স্থির char_u *menu_text(char_u *text, পূর্ণসংখ্যা *mnemonic, char_u **actext);

#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
স্থির শূন্য gui_create_tearoffs_recurse(vimmenu_T *menu, ধ্রুবক char_u *pname, পূর্ণসংখ্যা *pri_tab, পূর্ণসংখ্যা pri_idx);
স্থির শূন্য gui_add_tearoff(char_u *tearpath, পূর্ণসংখ্যা *pri_tab, পূর্ণসংখ্যা pri_idx);
স্থির শূন্য gui_destroy_tearoffs_recurse(vimmenu_T *menu);
স্থির পূর্ণসংখ্যা s_tearoffs = FALSE;
#endif

স্থির পূর্ণসংখ্যা menu_is_hidden(char_u *name);
স্থির পূর্ণসংখ্যা menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
স্থির পূর্ণসংখ্যা menus_locked = 0;

#যদি defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
স্থির char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
স্থির char_u *menutrans_lookup(char_u *name, পূর্ণসংখ্যা len);
স্থির শূন্য menu_unescape_name(char_u	*p);
#endif

স্থির char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
স্থির অক্ষর *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
স্থির ধ্রুবক অক্ষর *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_গুনতি ARRAY_দৈর্ঘ্য(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    স্থির পূর্ণসংখ্যা
menu_is_winbar(char_u *name)
{
    ফেরত (STRNCMP(name, "WinBar", 6) == 0);
}

    পূর্ণসংখ্যা
winbar_height(win_T *wp)
{
    যদি (wp->w_winbar != ফাঁকা && wp->w_winbar->children != ফাঁকা)
	ফেরত 1;
    ফেরত 0;
}

    স্থির vimmenu_T **
get_মূল_menu(char_u *name)
{
    যদি (menu_is_winbar(name))
	ফেরত &curwin->w_winbar;
    ফেরত &মূল_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    স্থির পূর্ণসংখ্যা
is_menus_locked(শূন্য)
{
    যদি (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    শূন্য
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    পূর্ণসংখ্যা		modes;
    char_u	*map_to;
    পূর্ণসংখ্যা		noremap;
    পূর্ণসংখ্যা		silent = FALSE;
    পূর্ণসংখ্যা		special = FALSE;
    পূর্ণসংখ্যা		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    পূর্ণসংখ্যা		i;
#যদি defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    পূর্ণসংখ্যা		old_menu_height;
# যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    পূর্ণসংখ্যা		old_toolbar_height;
# endif
#endif
    পূর্ণসংখ্যা		pri_tab[MENUDEPTH + 1];
    পূর্ণসংখ্যা		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = ফাঁকা;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**মূল_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    জন্য (;;)
    {
	যদি (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    চালু;
	}
	যদি (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    চালু;
	}
	যদি (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    চালু;
	}
	বিরতি;
    }


    // Locate an optional "icon=filename" argument.
    যদি (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	যখন (*arg != NUL && *arg != ' ')
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
    জন্য (p = arg; *p; ++p)
	যদি (!VIM_ISDIGIT(*p) && *p != '.')
	    বিরতি;
    যদি (VIM_ISWHITE(*p))
    {
	জন্য (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
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
    যখন (i < MENUDEPTH)
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
	ফেরত;
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
		    menuarg.iconidx = atoi((অক্ষর *)menu_path + 7);
		    যদি (menuarg.iconidx >= (পূর্ণসংখ্যা)TOOLBAR_NAME_গুনতি)
			menuarg.iconidx = -1;
		    অন্যথা
			menuarg.icon_builtin = TRUE;
		}
	    }
	    অন্যথা
	    {
		জন্য (i = 0; i < (পূর্ণসংখ্যা)TOOLBAR_NAME_গুনতি; ++i)
		    যদি (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			বিরতি;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    যদি (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	যাও theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    যদি (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	যাও theend;
    }
    অন্যথা যদি (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	যাও theend;
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
	    জন্য (i = 0; i < MENU_সূচক_TIP; ++i)
		যদি (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    যদি (p != ফাঁকা)
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
	    যাও theend;

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
	    জন্য (i = 0; i < MENU_সূচক_TIP; ++i)
		যদি (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    যদি (p != ফাঁকা)
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
	    যাও theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	যদি (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = ফাঁকা;
	}
	অন্যথা যদি (modes & MENU_TIP_MODE)
	    map_buf = ফাঁকা;	// Menu tips are plain text.
	অন্যথা
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ফাঁকা);
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
	    জন্য (i = 0; i < MENU_সূচক_TIP; ++i)
		যদি (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    যদি (p != ফাঁকা)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = ফাঁকা;
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
	পূর্ণসংখ্যা h = winbar_height(curwin);

	যদি (h != curwin->w_winbar_height)
	{
	    যদি (h == 0)
		++curwin->w_height;
	    অন্যথা যদি (curwin->w_height > 0)
		--curwin->w_height;
	    curwin->w_winbar_height = h;
	}
	curwin->w_পূর্ব_height = curwin->w_height;
    }

theend:
    ;
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    স্থির পূর্ণসংখ্যা
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    পূর্ণসংখ্যা		*pri_tab,
    char_u	*call_তথ্য
#ifdef FEAT_GUI_MSWIN
    , পূর্ণসংখ্যা	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    পূর্ণসংখ্যা		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = ফাঁকা;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*পরবর্তী_name;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		d;
#ifdef FEAT_GUI
    পূর্ণসংখ্যা		idx;
    পূর্ণসংখ্যা		new_idx;
#endif
    পূর্ণসংখ্যা		pri_idx = 0;
    পূর্ণসংখ্যা		old_modes = 0;
    পূর্ণসংখ্যা		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = ফাঁকা;
#endif
    vimmenu_T	**মূল_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    যদি (path_name == ফাঁকা)
	ফেরত FAIL;
    মূল_menu_ptr = get_মূল_menu(menu_path);
    menup = মূল_menu_ptr;
    parent = ফাঁকা;
    name = path_name;
    যখন (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	পরবর্তী_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (পূর্ণসংখ্যা)STRLEN(name));
	যদি (map_to != ফাঁকা)
	{
	    en_name = name;
	    name = map_to;
	}
	অন্যথা
	    en_name = ফাঁকা;
#endif
	dname = menu_text(name, ফাঁকা, ফাঁকা);
	যদি (dname == ফাঁকা)
	    যাও erret;
	যদি (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    যাও erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	যখন (menu != ফাঁকা)
	{
	    যদি (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		যদি (*পরবর্তী_name == NUL && menu->children != ফাঁকা)
		{
		    যদি (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    যাও erret;
		}
		যদি (*পরবর্তী_name != NUL && menu->children == ফাঁকা
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    যদি (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    যাও erret;
		}
		বিরতি;
	    }
	    menup = &menu->পরবর্তী;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    যদি (parent != ফাঁকা || menu_is_menubar(menu->name))
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
	    menu = menu->পরবর্তী;
	}

	যদি (menu == ফাঁকা)
	{
	    যদি (*পরবর্তী_name == NUL && parent == ফাঁকা)
	    {
		emsg(_(e_must_not_add_menu_বস্তুসমূহ_directly_to_menu_bar));
		যাও erret;
	    }

	    যদি (menu_is_separator(dname) && *পরবর্তী_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		যাও erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    যদি (menu == ফাঁকা)
		যাও erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    যদি (en_name != ফাঁকা)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, ফাঁকা, ফাঁকা);
	    }
	    অন্যথা
	    {
		menu->en_name = ফাঁকা;
		menu->en_dname = ফাঁকা;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = ফাঁকা;
#endif
	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->পরবর্তী = *lower_pri;
	    *lower_pri = menu;

	    old_modes = 0;

#ifdef FEAT_TOOLBAR
	    menu->iconidx = menuarg->iconidx;
	    menu->icon_builtin = menuarg->icon_builtin;
	    যদি (*পরবর্তী_name == NUL && menuarg->iconfile != ফাঁকা)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    যদি (parent != ফাঁকা && menu_is_tearoff(parent->children->dname))
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
		যদি (*পরবর্তী_name == NUL)
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
		    && *পরবর্তী_name
		    && vim_strchr(p_go, GO_TEAROFF) != ফাঁকা
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
		যদি (tearpath != ফাঁকা)
		{
		    char_u  *s;
		    পূর্ণসংখ্যা	    len;

		    STRCPY(tearpath, menu_path);
		    len = (পূর্ণসংখ্যা)(পরবর্তী_name - path_name - 1);
		    জন্য (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
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
	name = পরবর্তী_name;
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

    যদি (menu != ফাঁকা && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_তথ্য == ফাঁকা) ? ফাঁকা : vim_strsave(call_তথ্য);

	// loop over all modes, may add more than one
	জন্য (i = 0; i < MENU_MODES; ++i)
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
		যদি (amenu && call_তথ্য != ফাঁকা && *call_তথ্য != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    নির্বাচন (1 << i)
		    {
			ক্ষেত্র MENU_VISUAL_MODE:
			ক্ষেত্র MENU_SELECT_MODE:
			ক্ষেত্র MENU_OP_PENDING_MODE:
			ক্ষেত্র MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    বিরতি;
			ক্ষেত্র MENU_ঢোকাও_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    বিরতি;
		    }
		}

		যদি (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_তথ্য) + 5);
		    যদি (menu->strings[i] != ফাঁকা)
		    {
			menu->strings[i][0] = c;
			যদি (d == 0)
			    STRCPY(menu->strings[i] + 1, call_তথ্য);
			অন্যথা
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_তথ্য);
			}
			যদি (c == Ctrl_C)
			{
			    পূর্ণসংখ্যা	    len = (পূর্ণসংখ্যা)STRLEN(menu->strings[i]);

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
    ফেরত OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    যখন (parent != ফাঁকা && parent->children == ফাঁকা)
    {
	যদি (parent->parent == ফাঁকা)
	    menup = মূল_menu_ptr;
	অন্যথা
	    menup = &parent->parent->children;
	জন্য ( ; *menup != ফাঁকা && *menup != parent; menup = &((*menup)->পরবর্তী))
	    ;
	যদি (*menup == ফাঁকা) // safety check
	    বিরতি;
	parent = parent->parent;
	free_menu(menup);
    }
    ফেরত FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    স্থির পূর্ণসংখ্যা
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    পূর্ণসংখ্যা		modes,
    পূর্ণসংখ্যা		enable)
{
    char_u	*p;

    যদি (menu == ফাঁকা)
	ফেরত OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    যখন (menu != ফাঁকা)
    {
	যদি (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    যদি (*p != NUL)
	    {
		যদি (menu->children == ফাঁকা)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    ফেরত FAIL;
		}
		যদি (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    ফেরত FAIL;
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
		বিরতি;
	}
	menu = menu->পরবর্তী;
    }
    যদি (*name != NUL && *name != '*' && menu == ফাঁকা)
    {
	semsg(_(e_no_menu_str), name);
	ফেরত FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    ফেরত OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    স্থির পূর্ণসংখ্যা
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    পূর্ণসংখ্যা		modes,
    পূর্ণসংখ্যা		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    যদি (*menup == ফাঁকা)
	ফেরত OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    যখন ((menu = *menup) != ফাঁকা)
    {
	যদি (*name == NUL || menu_name_equal(name, menu))
	{
	    যদি (*p != NUL && menu->children == ফাঁকা)
	    {
		যদি (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		ফেরত FAIL;
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
		    ফেরত FAIL;
	    }
	    অন্যথা যদি (*name != NUL)
	    {
		যদি (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		ফেরত FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    যদি (*name != NUL)
		বিরতি;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    যদি (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_সূচক_TIP);
	    যদি ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    অন্যথা
		menup = &menu->পরবর্তী;
	}
	অন্যথা
	    menup = &menu->পরবর্তী;
    }
    যদি (*name != NUL)
    {
	যদি (menu == ফাঁকা)
	{
	    যদি (!silent)
		semsg(_(e_no_menu_str), name);
	    ফেরত FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#যদি defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	যদি ((s_tearoffs) && (menu->children != ফাঁকা)) // there's a tear bar.
	    child = menu->children->পরবর্তী; // don't count tearoff bar
	অন্যথা
#endif
	    child = menu->children;
	জন্য ( ; child != ফাঁকা; child = child->পরবর্তী)
	    menu->modes |= child->modes;
	যদি (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_সূচক_TIP);
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
	    যদি (s_tearoffs && menu->children != ফাঁকা) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    ফেরত OK;
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
    পূর্ণসংখ্যা		i;
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
    *menup = menu->পরবর্তী;
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
    জন্য (i = 0; i < MENU_MODES; i++)
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
free_menu_string(vimmenu_T *menu, পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা		গুনতি = 0;
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < MENU_MODES; i++)
	যদি (menu->strings[i] == menu->strings[idx])
	    গুনতি++;
    যদি (গুনতি == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = ফাঁকা;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    স্থির পূর্ণসংখ্যা
show_menus(char_u *path_name, পূর্ণসংখ্যা modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = ফাঁকা;

    name = path_name = vim_strsave(path_name);
    যদি (path_name == ফাঁকা)
	ফেরত FAIL;
    menu = *get_মূল_menu(path_name);

    // First, find the (sub)menu with the given name
    যখন (*name)
    {
	p = menu_name_skip(name);
	যখন (menu != ফাঁকা)
	{
	    যদি (menu_name_equal(name, menu))
	    {
		// Found menu
		যদি (*p != NUL && menu->children == ফাঁকা)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    ফেরত FAIL;
		}
		অন্যথা যদি ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    ফেরত FAIL;
		}
		বিরতি;
	    }
	    menu = menu->পরবর্তী;
	}
	যদি (menu == ফাঁকা)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    ফেরত FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    বার্তা_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    ফেরত OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    স্থির শূন্য
show_menus_recursive(vimmenu_T *menu, পূর্ণসংখ্যা modes, পূর্ণসংখ্যা depth)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		bit;

    যদি (menu != ফাঁকা && (menu->modes & modes) == 0x0)
	ফেরত;

    যদি (menu != ফাঁকা)
    {
	বার্তা_putchar('\n');
	যদি (got_int)		// "q" hit for "--more--"
	    ফেরত;
	জন্য (i = 0; i < depth; i++)
	    বার্তা_puts("  ");
	যদি (menu->priority)
	{
	    বার্তা_outnum((long)menu->priority);
	    বার্তা_puts(" ");
	}
				// Same highlighting as for directories!?
	বার্তা_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    যদি (menu != ফাঁকা && menu->children == ফাঁকা)
    {
	জন্য (bit = 0; bit < MENU_MODES; bit++)
	    যদি ((menu->modes & modes & (1 << bit)) != 0)
	    {
		বার্তা_putchar('\n');
		যদি (got_int)		// "q" hit for "--more--"
		    ফেরত;
		জন্য (i = 0; i < depth + 2; i++)
		    বার্তা_puts("  ");
		বার্তা_puts(menu_mode_chars[bit]);
		যদি (menu->noremap[bit] == REMAP_NONE)
		    বার্তা_putchar('*');
		অন্যথা যদি (menu->noremap[bit] == REMAP_SCRIPT)
		    বার্তা_putchar('&');
		অন্যথা
		    বার্তা_putchar(' ');
		যদি (menu->silent[bit])
		    বার্তা_putchar('s');
		অন্যথা
		    বার্তা_putchar(' ');
		যদি ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    বার্তা_putchar('-');
		অন্যথা
		    বার্তা_putchar(' ');
		বার্তা_puts(" ");
		যদি (*menu->strings[bit] == NUL)
		    বার্তা_puts_attr("<Nop>", HL_ATTR(HLF_8));
		অন্যথা
		    বার্তা_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    অন্যথা
    {
	যদি (menu == ফাঁকা)
	{
	    menu = মূল_menu;
	    depth--;
	}
	অন্যথা
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	জন্য (; menu != ফাঁকা && !got_int; menu = menu->পরবর্তী)
	    যদি (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
স্থির vimmenu_T	*expand_menu = ফাঁকা;
স্থির vimmenu_T	*expand_menu_alt = ফাঁকা;
স্থির পূর্ণসংখ্যা		expand_modes = 0x0;
স্থির পূর্ণসংখ্যা		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    পূর্ণসংখ্যা		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = ফাঁকা;
    char_u	*name;
    পূর্ণসংখ্যা		unmenu;
    vimmenu_T	*menu;
    পূর্ণসংখ্যা		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    জন্য (p = arg; *p; ++p)
	যদি (!VIM_ISDIGIT(*p) && *p != '.')
	    বিরতি;

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

    যখন (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    জন্য (; *p && !VIM_ISWHITE(*p); ++p)
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
	ফেরত ফাঁকা;	// TODO: check for next command?
    যদি (*p == NUL)		// Complete the menu name
    {
	পূর্ণসংখ্যা try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, ফাঁকা, &unmenu);
	যদি (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = মূল_menu;
	যদি (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    যদি (path_name == ফাঁকা)
		ফেরত ফাঁকা;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	যখন (name != ফাঁকা && *name)
	{
	    p = menu_name_skip(name);
	    যখন (menu != ফাঁকা)
	    {
		যদি (menu_name_equal(name, menu))
		{
		    // Found menu
		    যদি ((*p != NUL && menu->children == ফাঁকা)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			ফেরত ফাঁকা;
		    }
		    বিরতি;
		}
		menu = menu->পরবর্তী;
		যদি (menu == ফাঁকা && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    যদি (menu == ফাঁকা)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		ফেরত ফাঁকা;
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
	    expand_menu_alt = ফাঁকা;
    }
    অন্যথা			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    ফেরত ফাঁকা;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    স্থির vimmenu_T	*menu = ফাঁকা;
    স্থির পূর্ণসংখ্যা		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    স্থির  পূর্ণসংখ্যা		should_advance = FALSE;
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
    যখন (menu != ফাঁকা && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == ফাঁকা))
    {
	menu = menu->পরবর্তী;
	যদি (menu == ফাঁকা && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    যদি (menu == ফাঁকা)	    // at end of linked list
	ফেরত ফাঁকা;

    যদি (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	যদি (should_advance)
	    str = menu->en_dname;
	অন্যথা
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    যদি (menu->en_dname == ফাঁকা)
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
	menu = menu->পরবর্তী;
	যদি (menu == ফাঁকা && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ফেরত str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    স্থির vimmenu_T	*menu = ফাঁকা;
    স্থির পূর্ণসংখ্যা		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    স্থির char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    স্থির  পূর্ণসংখ্যা		should_advance = FALSE;
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
    যখন (menu != ফাঁকা
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->পরবর্তী;
	যদি (menu == ফাঁকা && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    যদি (menu == ফাঁকা)	    // at end of linked list
	ফেরত ফাঁকা;

    যদি (menu->modes & expand_modes)
    {
	যদি (menu->children != ফাঁকা)
	{
#ifdef FEAT_MULTI_LANG
	    যদি (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    অন্যথা
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		যদি (menu->en_dname == ফাঁকা)
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
		যদি (menu->en_dname == ফাঁকা)
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
	menu = menu->পরবর্তী;
	যদি (menu == ফাঁকা && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ফেরত str;
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

    জন্য (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    যদি (*p == NUL)
		বিরতি;
	}
    }
    যদি (*p)
	*p++ = NUL;
    ফেরত p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    স্থির পূর্ণসংখ্যা
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    যদি (menu->en_name != ফাঁকা
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	ফেরত TRUE;
#endif
    ফেরত menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    স্থির পূর্ণসংখ্যা
menu_namecmp(char_u *name, char_u *mname)
{
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	যদি (name[i] != mname[i])
	    বিরতি;
    ফেরত ((name[i] == NUL || name[i] == TAB)
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
    স্থির পূর্ণসংখ্যা
get_menu_cmd_modes(
    char_u  *cmd,
    পূর্ণসংখ্যা	    forceit,	    // Was there a "!" after the command?
    পূর্ণসংখ্যা	    *noremap,
    পূর্ণসংখ্যা	    *unmenu)
{
    পূর্ণসংখ্যা	    modes;

    নির্বাচন (*cmd++)
    {
	ক্ষেত্র 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    বিরতি;
	ক্ষেত্র 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    বিরতি;
	ক্ষেত্র 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    বিরতি;
	ক্ষেত্র 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    বিরতি;
	ক্ষেত্র 'i':			// imenu
	    modes = MENU_ঢোকাও_MODE;
	    বিরতি;
	ক্ষেত্র 't':
	    যদি (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		বিরতি;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    বিরতি;
	ক্ষেত্র 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    বিরতি;
	ক্ষেত্র 'a':			// amenu
	    modes = MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    বিরতি;
	ক্ষেত্র 'n':
	    যদি (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		বিরতি;
	    }
	    // FALLTHROUGH
	পূর্বনির্ধারিত:
	    --cmd;
	    যদি (forceit)		// menu!!
		modes = MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE;
	    অন্যথা			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    যদি (noremap != ফাঁকা)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    যদি (unmenu != ফাঁকা)
	*unmenu = (*cmd == 'u');
    ফেরত modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    স্থির char_u *
get_menu_mode_str(পূর্ণসংখ্যা modes)
{
    যদি ((modes & (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	ফেরত (char_u *)"a";
    যদি ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	ফেরত (char_u *)" ";
    যদি ((modes & (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE))
	    == (MENU_ঢোকাও_MODE | MENU_CMDLINE_MODE))
	ফেরত (char_u *)"!";
    যদি ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	ফেরত (char_u *)"v";
    যদি (modes & MENU_VISUAL_MODE)
	ফেরত (char_u *)"x";
    যদি (modes & MENU_SELECT_MODE)
	ফেরত (char_u *)"s";
    যদি (modes & MENU_OP_PENDING_MODE)
	ফেরত (char_u *)"o";
    যদি (modes & MENU_ঢোকাও_MODE)
	ফেরত (char_u *)"i";
    যদি (modes & MENU_TERMINAL_MODE)
	ফেরত (char_u *)"tl";
    যদি (modes & MENU_CMDLINE_MODE)
	ফেরত (char_u *)"c";
    যদি (modes & MENU_NORMAL_MODE)
	ফেরত (char_u *)"n";
    যদি (modes & MENU_TIP_MODE)
	ফেরত (char_u *)"t";

    ফেরত (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    স্থির char_u *
popup_mode_name(char_u *name, পূর্ণসংখ্যা idx)
{
    char_u	*p;
    পূর্ণসংখ্যা		len = (পূর্ণসংখ্যা)STRLEN(name);
    অক্ষর	*mode_chars = menu_mode_chars[idx];
    পূর্ণসংখ্যা		mode_chars_len = (পূর্ণসংখ্যা)strlen(mode_chars);
    পূর্ণসংখ্যা		i;

    p = vim_strnsave(name, len + mode_chars_len);
    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    জন্য (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    ফেরত p;
}

#যদি defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    পূর্ণসংখ্যা
get_menu_সূচক(vimmenu_T *menu, পূর্ণসংখ্যা state)
{
    পূর্ণসংখ্যা		idx;

    যদি ((state & MODE_ঢোকাও))
	idx = MENU_সূচক_ঢোকাও;
    অন্যথা যদি (state & MODE_CMDLINE)
	idx = MENU_সূচক_CMDLINE;
# ifdef FEAT_TERMINAL
    অন্যথা যদি (term_use_loop())
	idx = MENU_সূচক_TERMINAL;
# endif
    অন্যথা যদি (VIsual_active)
    {
	যদি (VIsual_select)
	    idx = MENU_সূচক_SELECT;
	অন্যথা
	    idx = MENU_সূচক_VISUAL;
    }
    অন্যথা যদি (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_সূচক_CMDLINE;
    অন্যথা যদি (finish_op)
	idx = MENU_সূচক_OP_PENDING;
    অন্যথা যদি ((state & MODE_NORMAL))
	idx = MENU_সূচক_NORMAL;
    অন্যথা
	idx = MENU_সূচক_INVALID;

    যদি (idx != MENU_সূচক_INVALID && menu->strings[idx] == ফাঁকা)
	idx = MENU_সূচক_INVALID;
    ফেরত idx;
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
menu_text(char_u *str, পূর্ণসংখ্যা *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    যদি (p != ফাঁকা)
    {
	যদি (actext != ফাঁকা)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    অন্যথা
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    জন্য (p = text; p != ফাঁকা; )
    {
	p = vim_strchr(p, '&');
	যদি (p != ফাঁকা)
	{
	    যদি (p[1] == NUL)	    // trailing "&"
		বিরতি;
	    যদি (mnemonic != ফাঁকা && p[1] != '&')
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
		অক্ষর c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    ফেরত text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    পূর্ণসংখ্যা
menu_is_menubar(char_u *name)
{
    ফেরত (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    পূর্ণসংখ্যা
menu_is_popup(char_u *name)
{
    ফেরত (STRNCMP(name, "PopUp", 5) == 0);
}

#যদি defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    পূর্ণসংখ্যা
menu_is_child_of_popup(vimmenu_T *menu)
{
    যখন (menu->parent != ফাঁকা)
	menu = menu->parent;
    ফেরত menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    পূর্ণসংখ্যা
menu_is_toolbar(char_u *name)
{
    ফেরত (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    পূর্ণসংখ্যা
menu_is_separator(char_u *name)
{
    ফেরত (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    স্থির পূর্ণসংখ্যা
menu_is_hidden(char_u *name)
{
    ফেরত (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    স্থির পূর্ণসংখ্যা
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    ফেরত (STRCMP(name, TEAR_STRING) == 0);
#অন্যথা
    ফেরত FALSE;
#endif
}

#যদি defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    স্থির পূর্ণসংখ্যা
get_menu_mode(শূন্য)
{
# ifdef FEAT_TERMINAL
    যদি (term_use_loop())
	ফেরত MENU_সূচক_TERMINAL;
# endif
    যদি (VIsual_active)
    {
	যদি (VIsual_select)
	    ফেরত MENU_সূচক_SELECT;
	ফেরত MENU_সূচক_VISUAL;
    }
    যদি (State & MODE_ঢোকাও)
	ফেরত MENU_সূচক_ঢোকাও;
    যদি ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	ফেরত MENU_সূচক_CMDLINE;
    যদি (finish_op)
	ফেরত MENU_সূচক_OP_PENDING;
    যদি (State & MODE_NORMAL)
	ফেরত MENU_সূচক_NORMAL;
    যদি (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	ফেরত MENU_সূচক_ঢোকাও;
    ফেরত MENU_সূচক_INVALID;
}

    পূর্ণসংখ্যা
get_menu_mode_flag(শূন্য)
{
    পূর্ণসংখ্যা mode = get_menu_mode();

    যদি (mode == MENU_সূচক_INVALID)
	ফেরত 0;
    ফেরত 1 << mode;
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
    পূর্ণসংখ্যা		menu_mode;
    অক্ষর*	mode;
    পূর্ণসংখ্যা		mode_len;

    menu_mode = get_menu_mode();
    যদি (menu_mode == MENU_সূচক_INVALID)
	ফেরত;
    mode = menu_mode_chars[menu_mode];
    mode_len = (পূর্ণসংখ্যা)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, ফাঁকা, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	যদি (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    বিরতি;

    // Only show a popup when it is defined and has entries
    যদি (menu == ফাঁকা || menu->children == ফাঁকা)
	ফেরত;

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
    পূর্ণসংখ্যা
check_menu_pointer(vimmenu_T *মূল, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    জন্য (p = মূল; p != ফাঁকা; p = p->পরবর্তী)
	যদি (p == menu_to_check
		|| (p->children != ফাঁকা
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    ফেরত OK;
    ফেরত FAIL;
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
    পূর্ণসংখ্যা		idx = 0;

    যখন (menu != ফাঁকা)
    {
	// Don't add a menu when only a tip was defined.
	যদি (menu->modes & MENU_ALL_MODES)
	{
	    যদি (menu->children != ফাঁকা)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    অন্যথা
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->পরবর্তী;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    স্থির শূন্য
gui_update_menus_recurse(vimmenu_T *menu, পূর্ণসংখ্যা mode)
{
    পূর্ণসংখ্যা		grey;

    যখন (menu)
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
	যদি (vim_strchr(p_go, GO_GREY) != ফাঁকা || menu->parent == ফাঁকা
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	অন্যথা
	    gui_mch_menu_hidden(menu, grey);
	gui_update_menus_recurse(menu->children, mode);
	menu = menu->পরবর্তী;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    শূন্য
gui_update_menus(পূর্ণসংখ্যা modes)
{
    স্থির পূর্ণসংখ্যা	    পূর্ব_mode = -1;
    পূর্ণসংখ্যা		    mode = 0;

    যদি (modes != 0x0)
	mode = modes;
    অন্যথা
	mode = get_menu_mode_flag();

    যদি (force_menu_update || mode != পূর্ব_mode)
    {
	gui_update_menus_recurse(মূল_menu, mode);
	gui_mch_draw_menubar();
	পূর্ব_mode = mode;
	force_menu_update = FALSE;
    }
}

# যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    পূর্ণসংখ্যা
gui_is_menu_shortcut(পূর্ণসংখ্যা চাবি)
{
    vimmenu_T	*menu;

    যদি (চাবি < 256)
	চাবি = TOLOWER_LOC(চাবি);
    FOR_ALL_MENUS(menu)
	যদি (menu->mnemonic == চাবি
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == চাবি))
	    ফেরত TRUE;
    ফেরত FALSE;
}
# endif
#endif // FEAT_GUI

#যদি defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    শূন্য
gui_mch_toggle_tearoffs(পূর্ণসংখ্যা enable)
{
    পূর্ণসংখ্যা		pri_tab[MENUDEPTH + 1];
    পূর্ণসংখ্যা		i;

    যদি (enable)
    {
	জন্য (i = 0; i < MENUDEPTH; ++i)
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
    ধ্রুবক char_u	*pname,
    পূর্ণসংখ্যা			*pri_tab,
    পূর্ণসংখ্যা			pri_idx)
{
    char_u	*newpname = ফাঁকা;
    পূর্ণসংখ্যা		len;
    char_u	*s;
    char_u	*d;

    যদি (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    যখন (menu != ফাঁকা)
    {
	যদি (menu->children != ফাঁকা && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (পূর্ণসংখ্যা)STRLEN(pname) + (পূর্ণসংখ্যা)STRLEN(menu->name);
	    জন্য (s = menu->name; *s; ++s)
		যদি (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    যদি (newpname != ফাঁকা)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		জন্য (s = menu->name; *s; ++s)
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
	menu = menu->পরবর্তী;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    স্থির শূন্য
gui_add_tearoff(char_u *tearpath, পূর্ণসংখ্যা *pri_tab, পূর্ণসংখ্যা pri_idx)
{
    char_u	*tbuf;
    পূর্ণসংখ্যা		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned পূর্ণসংখ্যা)STRLEN(tearpath));
    যদি (tbuf == ফাঁকা)
	ফেরত;

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
    menuarg.iconfile = ফাঁকা;
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
    যখন (menu)
    {
	যদি (menu->children)
	{
	    // check if tearoff exists
	    যদি (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    যদি (menu->children != ফাঁকা) // if not the last one
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->পরবর্তী;
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
execute_menu(exarg_T *eap, vimmenu_T *menu, পূর্ণসংখ্যা mode_idx)
{
    পূর্ণসংখ্যা		idx = mode_idx;

    যদি (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	যদি (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_সূচক_ঢোকাও;
	}
#ifdef FEAT_TERMINAL
	অন্যথা যদি (term_use_loop())
	{
	    idx = MENU_সূচক_TERMINAL;
	}
#endif
	অন্যথা যদি (VIsual_active)
	{
	    idx = MENU_সূচক_VISUAL;
	}
	অন্যথা যদি (eap != ফাঁকা && eap->addr_গুনতি)
	{
	    pos_T	tpos;

	    idx = MENU_সূচক_VISUAL;

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
    যদি (idx == MENU_সূচক_INVALID || eap == ফাঁকা)
	idx = MENU_সূচক_NORMAL;

    যদি (menu->strings[idx] != ফাঁকা && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	যদি (eap == ফাঁকা || current_sctx.sc_sid != 0)
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
    অন্যথা যদি (eap != ফাঁকা)
    {
	char_u	*mode;

	নির্বাচন (idx)
	{
	    ক্ষেত্র MENU_সূচক_VISUAL:
		mode = (char_u *)"Visual";
		বিরতি;
	    ক্ষেত্র MENU_সূচক_SELECT:
		mode = (char_u *)"Select";
		বিরতি;
	    ক্ষেত্র MENU_সূচক_OP_PENDING:
		mode = (char_u *)"Op-pending";
		বিরতি;
	    ক্ষেত্র MENU_সূচক_TERMINAL:
		mode = (char_u *)"Terminal";
		বিরতি;
	    ক্ষেত্র MENU_সূচক_ঢোকাও:
		mode = (char_u *)"Insert";
		বিরতি;
	    ক্ষেত্র MENU_সূচক_CMDLINE:
		mode = (char_u *)"Cmdline";
		বিরতি;
	    // case MENU_INDEX_TIP: cannot happen
	    পূর্বনির্ধারিত:
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
    পূর্ণসংখ্যা		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    যদি (saved_name == ফাঁকা)
	ফেরত ফাঁকা;

    menu = *get_মূল_menu(saved_name);
    name = saved_name;
    যখন (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	যখন (menu != ফাঁকা)
	{
	    যদি (menu_name_equal(name, menu))
	    {
		যদি (*p == NUL && menu->children != ফাঁকা)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = ফাঁকা;
		}
		অন্যথা যদি (*p != NUL && menu->children == ফাঁকা)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ফাঁকা;
		}
		বিরতি;
	    }
	    menu = menu->পরবর্তী;
	}
	যদি (menu == ফাঁকা || *p == NUL)
	    বিরতি;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    যদি (menu == ফাঁকা)
    {
	যদি (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	ফেরত ফাঁকা;
    }

    ফেরত menu;
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
    পূর্ণসংখ্যা		mode_idx = MENU_সূচক_INVALID;

    যদি (arg[0] && VIM_ISWHITE(arg[1]))
    {
	নির্বাচন (arg[0])
	{
	    ক্ষেত্র 'n': mode_idx = MENU_সূচক_NORMAL; বিরতি;
	    ক্ষেত্র 'v': mode_idx = MENU_সূচক_VISUAL; বিরতি;
	    ক্ষেত্র 's': mode_idx = MENU_সূচক_SELECT; বিরতি;
	    ক্ষেত্র 'o': mode_idx = MENU_সূচক_OP_PENDING; বিরতি;
	    ক্ষেত্র 't': mode_idx = MENU_সূচক_TERMINAL; বিরতি;
	    ক্ষেত্র 'i': mode_idx = MENU_সূচক_ঢোকাও; বিরতি;
	    ক্ষেত্র 'c': mode_idx = MENU_সূচক_CMDLINE; বিরতি;
	    পূর্বনির্ধারিত: semsg(_(e_invalid_argument_str), arg);
		     ফেরত;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    যদি (menu == ফাঁকা)
	ফেরত;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    শূন্য
winbar_click(win_T *wp, পূর্ণসংখ্যা col)
{
    পূর্ণসংখ্যা		idx;

    যদি (wp->w_winbar_বস্তুসমূহ == ফাঁকা)
	ফেরত;
    জন্য (idx = 0; wp->w_winbar_বস্তুসমূহ[idx].wb_menu != ফাঁকা; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_বস্তুসমূহ[idx];

	যদি (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = ফাঁকা;
	    pos_T   save_visual = VIsual;
	    পূর্ণসংখ্যা	    save_visual_active = VIsual_active;
	    পূর্ণসংখ্যা	    save_visual_select = VIsual_select;
	    পূর্ণসংখ্যা	    save_visual_reselect = VIsual_reselect;
	    পূর্ণসংখ্যা	    save_visual_mode = VIsual_mode;

	    যদি (wp != curwin)
	    {
		// Clicking in the window toolbar of a not-current window.
		// Make that window the current one and save Visual mode.
		save_curwin = curwin;
		VIsual_active = FALSE;
		curwin = wp;
		curbuf = curwin->w_বাফার;
		check_cursor();
	    }

	    // Note: the command might close the current window.
	    execute_menu(ফাঁকা, item->wb_menu, -1);

	    যদি (save_curwin != ফাঁকা && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_বাফার;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    যদি (!win_valid(wp))
		বিরতি;
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
    vimmenu_T	*menu = ফাঁকা;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_মূল_menu(path_name);

    saved_name = vim_strsave(path_name);
    যদি (saved_name == ফাঁকা)
	ফেরত ফাঁকা;

    name = saved_name;
    যখন (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	যখন (menu != ফাঁকা)
	{
	    যদি (menu_name_equal(name, menu))
	    {
		যদি (menu->children == ফাঁকা)
		{
		    // found a menu item instead of a sub-menu
		    যদি (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    অন্যথা
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ফাঁকা;
		    যাও theend;
		}
		যদি (*p == NUL)	    // found a full match
		    যাও theend;
		বিরতি;
	    }
	    menu = menu->পরবর্তী;
	}
	যদি (menu == ফাঁকা)	// didn't find it
	    বিরতি;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    যদি (menu == ফাঁকা)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    ফেরত menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

প্রকারলেখ কাঠামো
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

স্থির garray_T menutrans_ga = {0, 0, 0, 0, ফাঁকা};
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
    পূর্ণসংখ্যা			i;
    char_u		*from, *from_noamp, *to;

    যদি (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, আকার(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    যদি (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_তথ্য;
	জন্য (i = 0; i < menutrans_ga.ga_len; ++i)
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
		tp = (menutrans_T *)menutrans_ga.ga_তথ্য;
		from = vim_strsave(from);
		যদি (from != ফাঁকা)
		{
		    from_noamp = menu_text(from, ফাঁকা, ফাঁকা);
		    to = vim_strnsave(to, arg - to);
		    যদি (from_noamp != ফাঁকা && to != ফাঁকা)
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
    যখন (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	যদি ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    ফেরত p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    স্থির char_u *
menutrans_lookup(char_u *name, পূর্ণসংখ্যা len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_তথ্য;
    পূর্ণসংখ্যা			i;
    char_u		*dname;

    জন্য (i = 0; i < menutrans_ga.ga_len; ++i)
	যদি (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    ফেরত tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, ফাঁকা, ফাঁকা);
    name[len] = i;
    যদি (dname == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (i = 0; i < menutrans_ga.ga_len; ++i)
	যদি (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    ফেরত tp[i].to;
	}
    vim_free(dname);

    ফেরত ফাঁকা;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    স্থির শূন্য
menu_unescape_name(char_u *name)
{
    char_u  *p;

    জন্য (p = name; *p && *p != '.'; MB_PTR_ADV(p))
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

    যখন (*arg && !VIM_ISWHITE(*arg))
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

    ফেরত arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    স্থির পূর্ণসংখ্যা
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, পূর্ণসংখ্যা modes, dict_T *dict)
{
    পূর্ণসংখ্যা		status;
    list_T	*l;

    যদি (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	যদি (l == ফাঁকা)
	    ফেরত FAIL;

	যদি (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ফেরত FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	জন্য (topmenu = menu; topmenu != ফাঁকা; topmenu = topmenu->পরবর্তী)
	    যদি (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	ফেরত OK;
    }

    যদি (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	ফেরত OK;

    status = dict_add_string(dict, "name", menu->name);
    যদি (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    যদি (status == OK && menu->actext != ফাঁকা)
	status = dict_add_string(dict, "accel", menu->actext);
    যদি (status == OK)
	status = dict_add_সংখ্যা(dict, "priority", menu->priority);
    যদি (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    যদি (status == OK && menu->iconfile != ফাঁকা)
	status = dict_add_string(dict, "icon", menu->iconfile);
    যদি (status == OK && menu->iconidx >= 0)
	status = dict_add_সংখ্যা(dict, "iconidx", menu->iconidx);
#endif
    যদি (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	পূর্ণসংখ্যা	buflen;

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
    যদি (status == OK && menu->children == ফাঁকা)
    {
	পূর্ণসংখ্যা		bit;

	// Get the first mode in which the menu is available
	জন্য (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	যদি (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    যদি (menu->strings[bit] != ফাঁকা)
	    {
		যদি (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		অন্যথা
		{
		    char_u *tofree = ফাঁকা;

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
    যদি (status == OK && menu->children != ফাঁকা)
    {
	vimmenu_T	*child;

	l = list_alloc();
	যদি (l == ফাঁকা)
	    ফেরত FAIL;

	যদি (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ফেরত FAIL;
	}
	child = menu->children;
	যখন (child)
	{
	    যদি (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->পরবর্তী;
	}
    }

    ফেরত status;
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
    পূর্ণসংখ্যা		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;
    retdict = rettv->vval.v_dict;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    menu_name = tv_get_string_chk(&argvars[0]);
    যদি (menu_name == ফাঁকা)
	ফেরত;

    // menu mode
    যদি (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    অন্যথা
	which = (char_u *)"";	    // Default is modes for "menu"
    যদি (which == ফাঁকা)
	ফেরত;

    modes = get_menu_cmd_modes(which, *which == '!', ফাঁকা, ফাঁকা);

    // Locate the specified menu or menu item
    menu = *get_মূল_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    যদি (saved_name == ফাঁকা)
	ফেরত;
    যদি (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	যখন (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    যখন (menu != ফাঁকা)
	    {
		যদি (menu_name_equal(name, menu))
		    বিরতি;
		menu = menu->পরবর্তী;
	    }
	    যদি (menu == ফাঁকা || *p == NUL)
		বিরতি;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    যদি (menu == ফাঁকা)		// specified menu not found
	ফেরত;

    যদি (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
