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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
स्थिर पूर्णाङ्क add_menu_path(char_u *, vimmenu_T *, पूर्णाङ्क *, char_u *, पूर्णाङ्क);
#अन्यथा
स्थिर पूर्णाङ्क add_menu_path(char_u *, vimmenu_T *, पूर्णाङ्क *, char_u *);
#endif
स्थिर पूर्णाङ्क menu_nable_recurse(vimmenu_T *menu, char_u *name, पूर्णाङ्क modes, पूर्णाङ्क enable);
स्थिर पूर्णाङ्क remove_menu(vimmenu_T **, char_u *, पूर्णाङ्क, पूर्णाङ्क silent);
स्थिर शून्य free_menu(vimmenu_T **menup);
स्थिर शून्य free_menu_string(vimmenu_T *, पूर्णाङ्क);
स्थिर पूर्णाङ्क show_menus(char_u *, पूर्णाङ्क);
स्थिर शून्य show_menus_recursive(vimmenu_T *, पूर्णाङ्क, पूर्णाङ्क);
स्थिर char_u *menu_name_skip(char_u *name);
स्थिर पूर्णाङ्क menu_name_equal(char_u *name, vimmenu_T *menu);
स्थिर पूर्णाङ्क menu_namecmp(char_u *name, char_u *mname);
स्थिर पूर्णाङ्क get_menu_cmd_modes(char_u *, पूर्णाङ्क, पूर्णाङ्क *, पूर्णाङ्क *);
स्थिर char_u *popup_mode_name(char_u *name, पूर्णाङ्क idx);
स्थिर char_u *menu_text(char_u *text, पूर्णाङ्क *mnemonic, char_u **actext);

#यदि defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
स्थिर शून्य gui_create_tearoffs_recurse(vimmenu_T *menu, ध्रुव char_u *pname, पूर्णाङ्क *pri_tab, पूर्णाङ्क pri_idx);
स्थिर शून्य gui_add_tearoff(char_u *tearpath, पूर्णाङ्क *pri_tab, पूर्णाङ्क pri_idx);
स्थिर शून्य gui_destroy_tearoffs_recurse(vimmenu_T *menu);
स्थिर पूर्णाङ्क s_tearoffs = FALSE;
#endif

स्थिर पूर्णाङ्क menu_is_hidden(char_u *name);
स्थिर पूर्णाङ्क menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
स्थिर पूर्णाङ्क menus_locked = 0;

#यदि defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
स्थिर char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
स्थिर char_u *menutrans_lookup(char_u *name, पूर्णाङ्क len);
स्थिर शून्य menu_unescape_name(char_u	*p);
#endif

स्थिर char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
स्थिर वर्ण *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
स्थिर ध्रुव वर्ण *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_गिनती ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    स्थिर पूर्णाङ्क
menu_is_winbar(char_u *name)
{
    प्रत्यागम (STRNCMP(name, "WinBar", 6) == 0);
}

    पूर्णाङ्क
winbar_height(win_T *wp)
{
    यदि (wp->w_winbar != रिक्त && wp->w_winbar->children != रिक्त)
	प्रत्यागम 1;
    प्रत्यागम 0;
}

    स्थिर vimmenu_T **
get_मूल_menu(char_u *name)
{
    यदि (menu_is_winbar(name))
	प्रत्यागम &curwin->w_winbar;
    प्रत्यागम &मूल_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णाङ्क
is_menus_locked(शून्य)
{
    यदि (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    शून्य
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    पूर्णाङ्क		modes;
    char_u	*map_to;
    पूर्णाङ्क		noremap;
    पूर्णाङ्क		silent = FALSE;
    पूर्णाङ्क		special = FALSE;
    पूर्णाङ्क		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    पूर्णाङ्क		i;
#यदि defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    पूर्णाङ्क		old_menu_height;
# यदि defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    पूर्णाङ्क		old_toolbar_height;
# endif
#endif
    पूर्णाङ्क		pri_tab[MENUDEPTH + 1];
    पूर्णाङ्क		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = रिक्त;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**मूल_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    प्रति (;;)
    {
	यदि (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    अनुवर्त;
	}
	यदि (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    अनुवर्त;
	}
	यदि (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    अनुवर्त;
	}
	भङ्ग;
    }


    // Locate an optional "icon=filename" argument.
    यदि (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	यावत् (*arg != NUL && *arg != ' ')
	{
	    यदि (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	यदि (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    प्रति (p = arg; *p; ++p)
	यदि (!VIM_ISDIGIT(*p) && *p != '.')
	    भङ्ग;
    यदि (VIM_ISWHITE(*p))
    {
	प्रति (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    यदि (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    यदि (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    अन्यथा यदि (eap->addr_गिनती && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    अन्यथा
	i = 0;
    यावत् (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    यदि (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    अन्यथा यदि (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    यदि (*arg == NUL)
    {
	show_menus(arg, modes);
	प्रत्यागम;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    यदि (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	यदि (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    यदि (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		यदि (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = म_से_प((वर्ण *)menu_path + 7);
		    यदि (menuarg.iconidx >= (पूर्णाङ्क)TOOLBAR_NAME_गिनती)
			menuarg.iconidx = -1;
		    अन्यथा
			menuarg.icon_builtin = TRUE;
		}
	    }
	    अन्यथा
	    {
		प्रति (i = 0; i < (पूर्णाङ्क)TOOLBAR_NAME_गिनती; ++i)
		    यदि (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			भङ्ग;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    यदि (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	गच्छ theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    यदि (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	गच्छ theend;
    }
    अन्यथा यदि (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	गच्छ theend;
    }
#यदि defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# यदि defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    मूल_menu_ptr = get_मूल_menu(menu_path);
    यदि (मूल_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    यदि (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	यदि (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	यदि (menu_is_popup(menu_path))
	{
	    प्रति (i = 0; i < MENU_INDEX_TIP; ++i)
		यदि (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    यदि (p != रिक्त)
		    {
			menu_nable_recurse(*मूल_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*मूल_menu_ptr, menu_path, modes, enable);
    }
    अन्यथा यदि (unmenu)
    {
	यदि (is_menus_locked())
	    गच्छ theend;

	/*
	 * Delete menu(s).
	 */
	यदि (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	यदि (menu_is_popup(menu_path))
	{
	    प्रति (i = 0; i < MENU_INDEX_TIP; ++i)
		यदि (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    यदि (p != रिक्त)
		    {
			remove_menu(मूल_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(मूल_menu_ptr, menu_path, modes, FALSE);
    }
    अन्यथा
    {
	यदि (is_menus_locked())
	    गच्छ theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	यदि (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = रिक्त;
	}
	अन्यथा यदि (modes & MENU_TIP_MODE)
	    map_buf = रिक्त;	// Menu tips are plain text.
	अन्यथा
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), रिक्त);
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
	यदि (menu_is_popup(menu_path))
	{
	    प्रति (i = 0; i < MENU_INDEX_TIP; ++i)
		यदि (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    यदि (p != रिक्त)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = रिक्त;
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

#यदि defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    यदि (gui.in_use
	    && (gui.menu_height != old_menu_height
# यदि defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    यदि (मूल_menu_ptr == &curwin->w_winbar)
    {
	पूर्णाङ्क h = winbar_height(curwin);

	यदि (h != curwin->w_winbar_height)
	{
	    यदि (h == 0)
		++curwin->w_height;
	    अन्यथा यदि (curwin->w_height > 0)
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
    स्थिर पूर्णाङ्क
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    पूर्णाङ्क		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , पूर्णाङ्क	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    पूर्णाङ्क		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = रिक्त;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;
    पूर्णाङ्क		d;
#ifdef FEAT_GUI
    पूर्णाङ्क		idx;
    पूर्णाङ्क		new_idx;
#endif
    पूर्णाङ्क		pri_idx = 0;
    पूर्णाङ्क		old_modes = 0;
    पूर्णाङ्क		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = रिक्त;
#endif
    vimmenu_T	**मूल_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    यदि (path_name == रिक्त)
	प्रत्यागम FAIL;
    मूल_menu_ptr = get_मूल_menu(menu_path);
    menup = मूल_menu_ptr;
    parent = रिक्त;
    name = path_name;
    यावत् (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (पूर्णाङ्क)STRLEN(name));
	यदि (map_to != रिक्त)
	{
	    en_name = name;
	    name = map_to;
	}
	अन्यथा
	    en_name = रिक्त;
#endif
	dname = menu_text(name, रिक्त, रिक्त);
	यदि (dname == रिक्त)
	    गच्छ erret;
	यदि (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    गच्छ erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	यावत् (menu != रिक्त)
	{
	    यदि (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		यदि (*next_name == NUL && menu->children != रिक्त)
		{
		    यदि (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    गच्छ erret;
		}
		यदि (*next_name != NUL && menu->children == रिक्त
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    यदि (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    गच्छ erret;
		}
		भङ्ग;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    यदि (parent != रिक्त || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		यदि (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	यदि (menu == रिक्त)
	{
	    यदि (*next_name == NUL && parent == रिक्त)
	    {
		emsg(_(e_must_not_add_menu_वस्तुएँ_directly_to_menu_bar));
		गच्छ erret;
	    }

	    यदि (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		गच्छ erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    यदि (menu == रिक्त)
		गच्छ erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    यदि (en_name != रिक्त)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, रिक्त, रिक्त);
	    }
	    अन्यथा
	    {
		menu->en_name = रिक्त;
		menu->en_dname = रिक्त;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = रिक्त;
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
	    यदि (*next_name == NUL && menuarg->iconfile != रिक्त)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#यदि defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    यदि (parent != रिक्त && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	अन्यथा
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    यदि (addtearoff)
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
	यदि ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    यदि (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		यदि (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		अन्यथा
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# यदि defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    यदि (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != रिक्त
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
		यदि (tearpath != रिक्त)
		{
		    char_u  *s;
		    पूर्णाङ्क	    len;

		    STRCPY(tearpath, menu_path);
		    len = (पूर्णाङ्क)(next_name - path_name - 1);
		    प्रति (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			यदि ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	यदि (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_जोड़ो_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_जोड़ो_MODE));
    यदि (sys_menu)
	modes &= ~old_modes;

    यदि (menu != रिक्त && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == रिक्त) ? रिक्त : vim_strsave(call_data);

	// loop over all modes, may add more than one
	प्रति (i = 0; i < MENU_MODES; ++i)
	{
	    यदि (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		यदि (amenu && call_data != रिक्त && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    विकल्प (1 << i)
		    {
			पक्ष MENU_VISUAL_MODE:
			पक्ष MENU_SELECT_MODE:
			पक्ष MENU_OP_PENDING_MODE:
			पक्ष MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    भङ्ग;
			पक्ष MENU_जोड़ो_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    भङ्ग;
		    }
		}

		यदि (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    यदि (menu->strings[i] != रिक्त)
		    {
			menu->strings[i][0] = c;
			यदि (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			अन्यथा
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			यदि (c == Ctrl_C)
			{
			    पूर्णाङ्क	    len = (पूर्णाङ्क)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		अन्यथा
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#यदि defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	यदि (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    प्रत्यागम OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    यावत् (parent != रिक्त && parent->children == रिक्त)
    {
	यदि (parent->parent == रिक्त)
	    menup = मूल_menu_ptr;
	अन्यथा
	    menup = &parent->parent->children;
	प्रति ( ; *menup != रिक्त && *menup != parent; menup = &((*menup)->next))
	    ;
	यदि (*menup == रिक्त) // safety check
	    भङ्ग;
	parent = parent->parent;
	free_menu(menup);
    }
    प्रत्यागम FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    स्थिर पूर्णाङ्क
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    पूर्णाङ्क		modes,
    पूर्णाङ्क		enable)
{
    char_u	*p;

    यदि (menu == रिक्त)
	प्रत्यागम OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    यावत् (menu != रिक्त)
    {
	यदि (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    यदि (*p != NUL)
	    {
		यदि (menu->children == रिक्त)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    प्रत्यागम FAIL;
		}
		यदि (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अन्यथा
		यदि (enable)
		    menu->enabled |= modes;
		अन्यथा
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    यदि (*name != NUL && *name != '*')
		भङ्ग;
	}
	menu = menu->next;
    }
    यदि (*name != NUL && *name != '*' && menu == रिक्त)
    {
	semsg(_(e_no_menu_str), name);
	प्रत्यागम FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    प्रत्यागम OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    स्थिर पूर्णाङ्क
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    पूर्णाङ्क		modes,
    पूर्णाङ्क		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    यदि (*menup == रिक्त)
	प्रत्यागम OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    यावत् ((menu = *menup) != रिक्त)
    {
	यदि (*name == NUL || menu_name_equal(name, menu))
	{
	    यदि (*p != NUL && menu->children == रिक्त)
	    {
		यदि (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		प्रत्यागम FAIL;
	    }
	    यदि ((menu->modes & modes) != 0x0)
	    {
#यदि defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		यदि (*p == NUL && modes == MENU_ALL_MODES)
		{
		    यदि (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		यदि (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अन्यथा यदि (*name != NUL)
	    {
		यदि (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		प्रत्यागम FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    यदि (*name != NUL)
		भङ्ग;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    यदि (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    यदि ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    अन्यथा
		menup = &menu->next;
	}
	अन्यथा
	    menup = &menu->next;
    }
    यदि (*name != NUL)
    {
	यदि (menu == रिक्त)
	{
	    यदि (!silent)
		semsg(_(e_no_menu_str), name);
	    प्रत्यागम FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#यदि defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	यदि ((s_tearoffs) && (menu->children != रिक्त)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	अन्यथा
#endif
	    child = menu->children;
	प्रति ( ; child != रिक्त; child = child->next)
	    menu->modes |= child->modes;
	यदि (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#यदि defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    यदि (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	यदि ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#यदि defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    यदि (s_tearoffs && menu->children != रिक्त) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    प्रत्यागम OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    शून्य
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_वस्तुएँ);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    स्थिर शून्य
free_menu(vimmenu_T **menup)
{
    पूर्णाङ्क		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    यदि (gui.in_use)
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
    प्रति (i = 0; i < MENU_MODES; i++)
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
    स्थिर शून्य
free_menu_string(vimmenu_T *menu, पूर्णाङ्क idx)
{
    पूर्णाङ्क		गिनती = 0;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < MENU_MODES; i++)
	यदि (menu->strings[i] == menu->strings[idx])
	    गिनती++;
    यदि (गिनती == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = रिक्त;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    स्थिर पूर्णाङ्क
show_menus(char_u *path_name, पूर्णाङ्क modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = रिक्त;

    name = path_name = vim_strsave(path_name);
    यदि (path_name == रिक्त)
	प्रत्यागम FAIL;
    menu = *get_मूल_menu(path_name);

    // First, find the (sub)menu with the given name
    यावत् (*name)
    {
	p = menu_name_skip(name);
	यावत् (menu != रिक्त)
	{
	    यदि (menu_name_equal(name, menu))
	    {
		// Found menu
		यदि (*p != NUL && menu->children == रिक्त)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    प्रत्यागम FAIL;
		}
		अन्यथा यदि ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    प्रत्यागम FAIL;
		}
		भङ्ग;
	    }
	    menu = menu->next;
	}
	यदि (menu == रिक्त)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    प्रत्यागम FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    संदेश_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    प्रत्यागम OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    स्थिर शून्य
show_menus_recursive(vimmenu_T *menu, पूर्णाङ्क modes, पूर्णाङ्क depth)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		bit;

    यदि (menu != रिक्त && (menu->modes & modes) == 0x0)
	प्रत्यागम;

    यदि (menu != रिक्त)
    {
	संदेश_putchar('\n');
	यदि (got_int)		// "q" hit for "--more--"
	    प्रत्यागम;
	प्रति (i = 0; i < depth; i++)
	    संदेश_puts("  ");
	यदि (menu->priority)
	{
	    संदेश_outnum((दीर्घ)menu->priority);
	    संदेश_puts(" ");
	}
				// Same highlighting as for directories!?
	संदेश_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    यदि (menu != रिक्त && menu->children == रिक्त)
    {
	प्रति (bit = 0; bit < MENU_MODES; bit++)
	    यदि ((menu->modes & modes & (1 << bit)) != 0)
	    {
		संदेश_putchar('\n');
		यदि (got_int)		// "q" hit for "--more--"
		    प्रत्यागम;
		प्रति (i = 0; i < depth + 2; i++)
		    संदेश_puts("  ");
		संदेश_puts(menu_mode_chars[bit]);
		यदि (menu->noremap[bit] == REMAP_NONE)
		    संदेश_putchar('*');
		अन्यथा यदि (menu->noremap[bit] == REMAP_SCRIPT)
		    संदेश_putchar('&');
		अन्यथा
		    संदेश_putchar(' ');
		यदि (menu->silent[bit])
		    संदेश_putchar('s');
		अन्यथा
		    संदेश_putchar(' ');
		यदि ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    संदेश_putchar('-');
		अन्यथा
		    संदेश_putchar(' ');
		संदेश_puts(" ");
		यदि (*menu->strings[bit] == NUL)
		    संदेश_puts_attr("<Nop>", HL_ATTR(HLF_8));
		अन्यथा
		    संदेश_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    अन्यथा
    {
	यदि (menu == रिक्त)
	{
	    menu = मूल_menu;
	    depth--;
	}
	अन्यथा
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	प्रति (; menu != रिक्त && !got_int; menu = menu->next)
	    यदि (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
स्थिर vimmenu_T	*expand_menu = रिक्त;
स्थिर vimmenu_T	*expand_menu_alt = रिक्त;
स्थिर पूर्णाङ्क		expand_modes = 0x0;
स्थिर पूर्णाङ्क		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    पूर्णाङ्क		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = रिक्त;
    char_u	*name;
    पूर्णाङ्क		unmenu;
    vimmenu_T	*menu;
    पूर्णाङ्क		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    प्रति (p = arg; *p; ++p)
	यदि (!VIM_ISDIGIT(*p) && *p != '.')
	    भङ्ग;

    यदि (!VIM_ISWHITE(*p))
    {
	यदि (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	अन्यथा यदि (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	अन्यथा
	    p = arg;
    }

    यावत् (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    प्रति (; *p && !VIM_ISWHITE(*p); ++p)
    {
	यदि ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	अन्यथा यदि (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    यदि (expand_menus && VIM_ISWHITE(*p))
	प्रत्यागम रिक्त;	// TODO: check for next command?
    यदि (*p == NUL)		// Complete the menu name
    {
	पूर्णाङ्क try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, रिक्त, &unmenu);
	यदि (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = मूल_menu;
	यदि (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    यदि (path_name == रिक्त)
		प्रत्यागम रिक्त;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	यावत् (name != रिक्त && *name)
	{
	    p = menu_name_skip(name);
	    यावत् (menu != रिक्त)
	    {
		यदि (menu_name_equal(name, menu))
		{
		    // Found menu
		    यदि ((*p != NUL && menu->children == रिक्त)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			प्रत्यागम रिक्त;
		    }
		    भङ्ग;
		}
		menu = menu->next;
		यदि (menu == रिक्त && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    यदि (menu == रिक्त)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		प्रत्यागम रिक्त;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	यदि (expand_menu == मूल_menu)
	    expand_menu_alt = curwin->w_winbar;
	अन्यथा
	    expand_menu_alt = रिक्त;
    }
    अन्यथा			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    प्रत्यागम रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    स्थिर vimmenu_T	*menu = रिक्त;
    स्थिर पूर्णाङ्क		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    स्थिर  पूर्णाङ्क		should_advance = FALSE;
#endif

    यदि (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    यावत् (menu != रिक्त && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == रिक्त))
    {
	menu = menu->next;
	यदि (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    यदि (menu == रिक्त)	    // at end of linked list
	प्रत्यागम रिक्त;

    यदि (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	यदि (should_advance)
	    str = menu->en_dname;
	अन्यथा
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    यदि (menu->en_dname == रिक्त)
		should_advance = TRUE;
	}
#endif
    अन्यथा
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    यदि (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	यदि (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    प्रत्यागम str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    स्थिर vimmenu_T	*menu = रिक्त;
    स्थिर पूर्णाङ्क		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    स्थिर char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    स्थिर  पूर्णाङ्क		should_advance = FALSE;
#endif

    यदि (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    यावत् (menu != रिक्त
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	यदि (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    यदि (menu == रिक्त)	    // at end of linked list
	प्रत्यागम रिक्त;

    यदि (menu->modes & expand_modes)
    {
	यदि (menu->children != रिक्त)
	{
#ifdef FEAT_MULTI_LANG
	    यदि (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    अन्यथा
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		यदि (menu->en_dname == रिक्त)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	अन्यथा
#ifdef FEAT_MULTI_LANG
	{
	    यदि (should_advance)
		str = menu->en_dname;
	    अन्यथा
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		यदि (menu->en_dname == रिक्त)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    अन्यथा
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    यदि (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	यदि (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    प्रत्यागम str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    स्थिर char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    प्रति (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	यदि (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    यदि (*p == NUL)
		भङ्ग;
	}
    }
    यदि (*p)
	*p++ = NUL;
    प्रत्यागम p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    स्थिर पूर्णाङ्क
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    यदि (menu->en_name != रिक्त
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	प्रत्यागम TRUE;
#endif
    प्रत्यागम menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    स्थिर पूर्णाङ्क
menu_namecmp(char_u *name, char_u *mname)
{
    पूर्णाङ्क		i;

    प्रति (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	यदि (name[i] != mname[i])
	    भङ्ग;
    प्रत्यागम ((name[i] == NUL || name[i] == TAB)
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
    स्थिर पूर्णाङ्क
get_menu_cmd_modes(
    char_u  *cmd,
    पूर्णाङ्क	    forceit,	    // Was there a "!" after the command?
    पूर्णाङ्क	    *noremap,
    पूर्णाङ्क	    *unmenu)
{
    पूर्णाङ्क	    modes;

    विकल्प (*cmd++)
    {
	पक्ष 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    भङ्ग;
	पक्ष 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    भङ्ग;
	पक्ष 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    भङ्ग;
	पक्ष 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    भङ्ग;
	पक्ष 'i':			// imenu
	    modes = MENU_जोड़ो_MODE;
	    भङ्ग;
	पक्ष 't':
	    यदि (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		भङ्ग;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    भङ्ग;
	पक्ष 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    भङ्ग;
	पक्ष 'a':			// amenu
	    modes = MENU_जोड़ो_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    भङ्ग;
	पक्ष 'n':
	    यदि (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		भङ्ग;
	    }
	    // FALLTHROUGH
	स्वभाव:
	    --cmd;
	    यदि (forceit)		// menu!!
		modes = MENU_जोड़ो_MODE | MENU_CMDLINE_MODE;
	    अन्यथा			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    यदि (noremap != रिक्त)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    यदि (unmenu != रिक्त)
	*unmenu = (*cmd == 'u');
    प्रत्यागम modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    स्थिर char_u *
get_menu_mode_str(पूर्णाङ्क modes)
{
    यदि ((modes & (MENU_जोड़ो_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_जोड़ो_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	प्रत्यागम (char_u *)"a";
    यदि ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	प्रत्यागम (char_u *)" ";
    यदि ((modes & (MENU_जोड़ो_MODE | MENU_CMDLINE_MODE))
	    == (MENU_जोड़ो_MODE | MENU_CMDLINE_MODE))
	प्रत्यागम (char_u *)"!";
    यदि ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	प्रत्यागम (char_u *)"v";
    यदि (modes & MENU_VISUAL_MODE)
	प्रत्यागम (char_u *)"x";
    यदि (modes & MENU_SELECT_MODE)
	प्रत्यागम (char_u *)"s";
    यदि (modes & MENU_OP_PENDING_MODE)
	प्रत्यागम (char_u *)"o";
    यदि (modes & MENU_जोड़ो_MODE)
	प्रत्यागम (char_u *)"i";
    यदि (modes & MENU_TERMINAL_MODE)
	प्रत्यागम (char_u *)"tl";
    यदि (modes & MENU_CMDLINE_MODE)
	प्रत्यागम (char_u *)"c";
    यदि (modes & MENU_NORMAL_MODE)
	प्रत्यागम (char_u *)"n";
    यदि (modes & MENU_TIP_MODE)
	प्रत्यागम (char_u *)"t";

    प्रत्यागम (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    स्थिर char_u *
popup_mode_name(char_u *name, पूर्णाङ्क idx)
{
    char_u	*p;
    पूर्णाङ्क		len = (पूर्णाङ्क)STRLEN(name);
    वर्ण	*mode_chars = menu_mode_chars[idx];
    पूर्णाङ्क		mode_chars_len = (पूर्णाङ्क)म_माप(mode_chars);
    पूर्णाङ्क		i;

    p = vim_strnsave(name, len + mode_chars_len);
    यदि (p == रिक्त)
	प्रत्यागम रिक्त;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (माप_प्रकार)(len - 4));
    प्रति (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    प्रत्यागम p;
}

#यदि defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    पूर्णाङ्क
get_menu_index(vimmenu_T *menu, पूर्णाङ्क state)
{
    पूर्णाङ्क		idx;

    यदि ((state & MODE_जोड़ो))
	idx = MENU_INDEX_जोड़ो;
    अन्यथा यदि (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    अन्यथा यदि (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    अन्यथा यदि (VIsual_active)
    {
	यदि (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	अन्यथा
	    idx = MENU_INDEX_VISUAL;
    }
    अन्यथा यदि (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    अन्यथा यदि (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    अन्यथा यदि ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    अन्यथा
	idx = MENU_INDEX_INVALID;

    यदि (idx != MENU_INDEX_INVALID && menu->strings[idx] == रिक्त)
	idx = MENU_INDEX_INVALID;
    प्रत्यागम idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    स्थिर char_u *
menu_text(char_u *str, पूर्णाङ्क *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    यदि (p != रिक्त)
    {
	यदि (actext != रिक्त)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    अन्यथा
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    प्रति (p = text; p != रिक्त; )
    {
	p = vim_strchr(p, '&');
	यदि (p != रिक्त)
	{
	    यदि (p[1] == NUL)	    // trailing "&"
		भङ्ग;
	    यदि (mnemonic != रिक्त && p[1] != '&')
#यदि !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#अन्यथा
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		वर्ण c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    प्रत्यागम text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    पूर्णाङ्क
menu_is_menubar(char_u *name)
{
    प्रत्यागम (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    पूर्णाङ्क
menu_is_popup(char_u *name)
{
    प्रत्यागम (STRNCMP(name, "PopUp", 5) == 0);
}

#यदि defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    पूर्णाङ्क
menu_is_child_of_popup(vimmenu_T *menu)
{
    यावत् (menu->parent != रिक्त)
	menu = menu->parent;
    प्रत्यागम menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    पूर्णाङ्क
menu_is_toolbar(char_u *name)
{
    प्रत्यागम (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    पूर्णाङ्क
menu_is_separator(char_u *name)
{
    प्रत्यागम (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    स्थिर पूर्णाङ्क
menu_is_hidden(char_u *name)
{
    प्रत्यागम (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    स्थिर पूर्णाङ्क
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    प्रत्यागम (STRCMP(name, TEAR_STRING) == 0);
#अन्यथा
    प्रत्यागम FALSE;
#endif
}

#यदि defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    स्थिर पूर्णाङ्क
get_menu_mode(शून्य)
{
# ifdef FEAT_TERMINAL
    यदि (term_use_loop())
	प्रत्यागम MENU_INDEX_TERMINAL;
# endif
    यदि (VIsual_active)
    {
	यदि (VIsual_select)
	    प्रत्यागम MENU_INDEX_SELECT;
	प्रत्यागम MENU_INDEX_VISUAL;
    }
    यदि (State & MODE_जोड़ो)
	प्रत्यागम MENU_INDEX_जोड़ो;
    यदि ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	प्रत्यागम MENU_INDEX_CMDLINE;
    यदि (finish_op)
	प्रत्यागम MENU_INDEX_OP_PENDING;
    यदि (State & MODE_NORMAL)
	प्रत्यागम MENU_INDEX_NORMAL;
    यदि (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	प्रत्यागम MENU_INDEX_जोड़ो;
    प्रत्यागम MENU_INDEX_INVALID;
}

    पूर्णाङ्क
get_menu_mode_flag(शून्य)
{
    पूर्णाङ्क mode = get_menu_mode();

    यदि (mode == MENU_INDEX_INVALID)
	प्रत्यागम 0;
    प्रत्यागम 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    शून्य
show_popupmenu(शून्य)
{
    vimmenu_T	*menu;
    पूर्णाङ्क		menu_mode;
    वर्ण*	mode;
    पूर्णाङ्क		mode_len;

    menu_mode = get_menu_mode();
    यदि (menu_mode == MENU_INDEX_INVALID)
	प्रत्यागम;
    mode = menu_mode_chars[menu_mode];
    mode_len = (पूर्णाङ्क)म_माप(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, रिक्त, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	यदि (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    भङ्ग;

    // Only show a popup when it is defined and has entries
    यदि (menu == रिक्त || menu->children == रिक्त)
	प्रत्यागम;

# यदि defined(FEAT_GUI)
    यदि (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# यदि defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    अन्यथा
# endif
# यदि defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#यदि defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    पूर्णाङ्क
check_menu_pointer(vimmenu_T *मूल, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    प्रति (p = मूल; p != रिक्त; p = p->next)
	यदि (p == menu_to_check
		|| (p->children != रिक्त
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    प्रत्यागम OK;
    प्रत्यागम FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    शून्य
gui_create_initial_menus(vimmenu_T *menu)
{
    पूर्णाङ्क		idx = 0;

    यावत् (menu != रिक्त)
    {
	// Don't add a menu when only a tip was defined.
	यदि (menu->modes & MENU_ALL_MODES)
	{
	    यदि (menu->children != रिक्त)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    अन्यथा
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    स्थिर शून्य
gui_update_menus_recurse(vimmenu_T *menu, पूर्णाङ्क mode)
{
    पूर्णाङ्क		grey;

    यावत् (menu)
    {
	यदि ((menu->modes & menu->enabled & mode)
# यदि defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	अन्यथा
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	यदि (vim_strchr(p_go, GO_GREY) != रिक्त || menu->parent == रिक्त
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	अन्यथा
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
    शून्य
gui_update_menus(पूर्णाङ्क modes)
{
    स्थिर पूर्णाङ्क	    prev_mode = -1;
    पूर्णाङ्क		    mode = 0;

    यदि (modes != 0x0)
	mode = modes;
    अन्यथा
	mode = get_menu_mode_flag();

    यदि (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(मूल_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# यदि defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    पूर्णाङ्क
gui_is_menu_shortcut(पूर्णाङ्क कुंजी)
{
    vimmenu_T	*menu;

    यदि (कुंजी < 256)
	कुंजी = TOLOWER_LOC(कुंजी);
    FOR_ALL_MENUS(menu)
	यदि (menu->mnemonic == कुंजी
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == कुंजी))
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}
# endif
#endif // FEAT_GUI

#यदि defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    शून्य
gui_mch_toggle_tearoffs(पूर्णाङ्क enable)
{
    पूर्णाङ्क		pri_tab[MENUDEPTH + 1];
    पूर्णाङ्क		i;

    यदि (enable)
    {
	प्रति (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(मूल_menu, (char_u *)"", pri_tab, 0);
    }
    अन्यथा
	gui_destroy_tearoffs_recurse(मूल_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    स्थिर शून्य
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ध्रुव char_u	*pname,
    पूर्णाङ्क			*pri_tab,
    पूर्णाङ्क			pri_idx)
{
    char_u	*newpname = रिक्त;
    पूर्णाङ्क		len;
    char_u	*s;
    char_u	*d;

    यदि (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    यावत् (menu != रिक्त)
    {
	यदि (menu->children != रिक्त && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (पूर्णाङ्क)STRLEN(pname) + (पूर्णाङ्क)STRLEN(menu->name);
	    प्रति (s = menu->name; *s; ++s)
		यदि (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    यदि (newpname != रिक्त)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		प्रति (s = menu->name; *s; ++s)
		{
		    यदि (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		यदि (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    स्थिर शून्य
gui_add_tearoff(char_u *tearpath, पूर्णाङ्क *pri_tab, पूर्णाङ्क pri_idx)
{
    char_u	*tbuf;
    पूर्णाङ्क		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (अचिन्हित पूर्णाङ्क)STRLEN(tearpath));
    यदि (tbuf == रिक्त)
	प्रत्यागम;

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
    menuarg.iconfile = रिक्त;
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
    स्थिर शून्य
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    यावत् (menu)
    {
	यदि (menu->children)
	{
	    // check if tearoff exists
	    यदि (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    यदि (menu->children != रिक्त) // if not the last one
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
    शून्य
execute_menu(exarg_T *eap, vimmenu_T *menu, पूर्णाङ्क mode_idx)
{
    पूर्णाङ्क		idx = mode_idx;

    यदि (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	यदि (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_जोड़ो;
	}
#ifdef FEAT_TERMINAL
	अन्यथा यदि (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	अन्यथा यदि (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	अन्यथा यदि (eap != रिक्त && eap->addr_गिनती)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    यदि ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    अन्यथा
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
	    यदि (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    यदि (idx == MENU_INDEX_INVALID || eap == रिक्त)
	idx = MENU_INDEX_NORMAL;

    यदि (menu->strings[idx] != रिक्त && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	यदि (eap == रिक्त || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    यदि (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	अन्यथा
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    अन्यथा यदि (eap != रिक्त)
    {
	char_u	*mode;

	विकल्प (idx)
	{
	    पक्ष MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		भङ्ग;
	    पक्ष MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		भङ्ग;
	    पक्ष MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		भङ्ग;
	    पक्ष MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		भङ्ग;
	    पक्ष MENU_INDEX_जोड़ो:
		mode = (char_u *)"Insert";
		भङ्ग;
	    पक्ष MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		भङ्ग;
	    // case MENU_INDEX_TIP: cannot happen
	    स्वभाव:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    स्थिर vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    पूर्णाङ्क		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    यदि (saved_name == रिक्त)
	प्रत्यागम रिक्त;

    menu = *get_मूल_menu(saved_name);
    name = saved_name;
    यावत् (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	यावत् (menu != रिक्त)
	{
	    यदि (menu_name_equal(name, menu))
	    {
		यदि (*p == NUL && menu->children != रिक्त)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = रिक्त;
		}
		अन्यथा यदि (*p != NUL && menu->children == रिक्त)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = रिक्त;
		}
		भङ्ग;
	    }
	    menu = menu->next;
	}
	यदि (menu == रिक्त || *p == NUL)
	    भङ्ग;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    यदि (menu == रिक्त)
    {
	यदि (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	प्रत्यागम रिक्त;
    }

    प्रत्यागम menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    शून्य
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    पूर्णाङ्क		mode_idx = MENU_INDEX_INVALID;

    यदि (arg[0] && VIM_ISWHITE(arg[1]))
    {
	विकल्प (arg[0])
	{
	    पक्ष 'n': mode_idx = MENU_INDEX_NORMAL; भङ्ग;
	    पक्ष 'v': mode_idx = MENU_INDEX_VISUAL; भङ्ग;
	    पक्ष 's': mode_idx = MENU_INDEX_SELECT; भङ्ग;
	    पक्ष 'o': mode_idx = MENU_INDEX_OP_PENDING; भङ्ग;
	    पक्ष 't': mode_idx = MENU_INDEX_TERMINAL; भङ्ग;
	    पक्ष 'i': mode_idx = MENU_INDEX_जोड़ो; भङ्ग;
	    पक्ष 'c': mode_idx = MENU_INDEX_CMDLINE; भङ्ग;
	    स्वभाव: semsg(_(e_invalid_argument_str), arg);
		     प्रत्यागम;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    यदि (menu == रिक्त)
	प्रत्यागम;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    शून्य
winbar_click(win_T *wp, पूर्णाङ्क col)
{
    पूर्णाङ्क		idx;

    यदि (wp->w_winbar_वस्तुएँ == रिक्त)
	प्रत्यागम;
    प्रति (idx = 0; wp->w_winbar_वस्तुएँ[idx].wb_menu != रिक्त; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_वस्तुएँ[idx];

	यदि (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = रिक्त;
	    pos_T   save_visual = VIsual;
	    पूर्णाङ्क	    save_visual_active = VIsual_active;
	    पूर्णाङ्क	    save_visual_select = VIsual_select;
	    पूर्णाङ्क	    save_visual_reselect = VIsual_reselect;
	    पूर्णाङ्क	    save_visual_mode = VIsual_mode;

	    यदि (wp != curwin)
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
	    execute_menu(रिक्त, item->wb_menu, -1);

	    यदि (save_curwin != रिक्त && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    यदि (!win_valid(wp))
		भङ्ग;
	}
    }
}

#यदि defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = रिक्त;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_मूल_menu(path_name);

    saved_name = vim_strsave(path_name);
    यदि (saved_name == रिक्त)
	प्रत्यागम रिक्त;

    name = saved_name;
    यावत् (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	यावत् (menu != रिक्त)
	{
	    यदि (menu_name_equal(name, menu))
	    {
		यदि (menu->children == रिक्त)
		{
		    // found a menu item instead of a sub-menu
		    यदि (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    अन्यथा
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = रिक्त;
		    गच्छ theend;
		}
		यदि (*p == NUL)	    // found a full match
		    गच्छ theend;
		भङ्ग;
	    }
	    menu = menu->next;
	}
	यदि (menu == रिक्त)	// didn't find it
	    भङ्ग;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    यदि (menu == रिक्त)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    प्रत्यागम menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

प्रकारनिर्देश संरचना
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

स्थिर garray_T menutrans_ga = {0, 0, 0, 0, रिक्त};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    शून्य
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    पूर्णाङ्क			i;
    char_u		*from, *from_noamp, *to;

    यदि (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, परिमाण(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    यदि (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	प्रति (i = 0; i < menutrans_ga.ga_len; ++i)
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
    अन्यथा
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	यदि (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	अन्यथा
	{
	    यदि (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		यदि (from != रिक्त)
		{
		    from_noamp = menu_text(from, रिक्त, रिक्त);
		    to = vim_strnsave(to, arg - to);
		    यदि (from_noamp != रिक्त && to != रिक्त)
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
		    अन्यथा
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

#यदि defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    स्थिर char_u *
menu_skip_part(char_u *p)
{
    यावत् (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	यदि ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    प्रत्यागम p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    स्थिर char_u *
menutrans_lookup(char_u *name, पूर्णाङ्क len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    पूर्णाङ्क			i;
    char_u		*dname;

    प्रति (i = 0; i < menutrans_ga.ga_len; ++i)
	यदि (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    प्रत्यागम tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, रिक्त, रिक्त);
    name[len] = i;
    यदि (dname == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (i = 0; i < menutrans_ga.ga_len; ++i)
	यदि (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    प्रत्यागम tp[i].to;
	}
    vim_free(dname);

    प्रत्यागम रिक्त;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    स्थिर शून्य
menu_unescape_name(char_u *name)
{
    char_u  *p;

    प्रति (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	यदि (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    स्थिर char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    यावत् (*arg && !VIM_ISWHITE(*arg))
    {
	यदि ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	अन्यथा यदि (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    यदि (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    प्रत्यागम arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    स्थिर पूर्णाङ्क
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, पूर्णाङ्क modes, dict_T *dict)
{
    पूर्णाङ्क		status;
    list_T	*l;

    यदि (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	यदि (l == रिक्त)
	    प्रत्यागम FAIL;

	यदि (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    प्रत्यागम FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	प्रति (topmenu = menu; topmenu != रिक्त; topmenu = topmenu->next)
	    यदि (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	प्रत्यागम OK;
    }

    यदि (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	प्रत्यागम OK;

    status = dict_add_string(dict, "name", menu->name);
    यदि (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    यदि (status == OK && menu->actext != रिक्त)
	status = dict_add_string(dict, "accel", menu->actext);
    यदि (status == OK)
	status = dict_add_संख्या(dict, "priority", menu->priority);
    यदि (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    यदि (status == OK && menu->iconfile != रिक्त)
	status = dict_add_string(dict, "icon", menu->iconfile);
    यदि (status == OK && menu->iconidx >= 0)
	status = dict_add_संख्या(dict, "iconidx", menu->iconidx);
#endif
    यदि (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	पूर्णाङ्क	buflen;

	यदि (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	अन्यथा
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    यदि (status == OK && menu->children == रिक्त)
    {
	पूर्णाङ्क		bit;

	// Get the first mode in which the menu is available
	प्रति (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	यदि (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    यदि (menu->strings[bit] != रिक्त)
	    {
		यदि (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		अन्यथा
		{
		    char_u *tofree = रिक्त;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    यदि (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    यदि (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    यदि (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    यदि (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    यदि (status == OK && menu->children != रिक्त)
    {
	vimmenu_T	*child;

	l = list_alloc();
	यदि (l == रिक्त)
	    प्रत्यागम FAIL;

	यदि (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    प्रत्यागम FAIL;
	}
	child = menu->children;
	यावत् (child)
	{
	    यदि (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    प्रत्यागम status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    शून्य
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    पूर्णाङ्क		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    यदि (rettv_dict_alloc(rettv) == FAIL)
	प्रत्यागम;
    retdict = rettv->vval.v_dict;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    menu_name = tv_get_string_chk(&argvars[0]);
    यदि (menu_name == रिक्त)
	प्रत्यागम;

    // menu mode
    यदि (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    अन्यथा
	which = (char_u *)"";	    // Default is modes for "menu"
    यदि (which == रिक्त)
	प्रत्यागम;

    modes = get_menu_cmd_modes(which, *which == '!', रिक्त, रिक्त);

    // Locate the specified menu or menu item
    menu = *get_मूल_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    यदि (saved_name == रिक्त)
	प्रत्यागम;
    यदि (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	यावत् (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    यावत् (menu != रिक्त)
	    {
		यदि (menu_name_equal(name, menu))
		    भङ्ग;
		menu = menu->next;
	    }
	    यदि (menu == रिक्त || *p == NUL)
		भङ्ग;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    यदि (menu == रिक्त)		// specified menu not found
	प्रत्यागम;

    यदि (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
