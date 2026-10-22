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

#जर defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
स्थिर पूर्णांक add_menu_path(char_u *, vimmenu_T *, पूर्णांक *, char_u *, पूर्णांक);
#अन्यथा
स्थिर पूर्णांक add_menu_path(char_u *, vimmenu_T *, पूर्णांक *, char_u *);
#endif
स्थिर पूर्णांक menu_nable_recurse(vimmenu_T *menu, char_u *name, पूर्णांक modes, पूर्णांक enable);
स्थिर पूर्णांक remove_menu(vimmenu_T **, char_u *, पूर्णांक, पूर्णांक silent);
स्थिर शून्य free_menu(vimmenu_T **menup);
स्थिर शून्य free_menu_string(vimmenu_T *, पूर्णांक);
स्थिर पूर्णांक show_menus(char_u *, पूर्णांक);
स्थिर शून्य show_menus_recursive(vimmenu_T *, पूर्णांक, पूर्णांक);
स्थिर char_u *menu_name_skip(char_u *name);
स्थिर पूर्णांक menu_name_equal(char_u *name, vimmenu_T *menu);
स्थिर पूर्णांक menu_namecmp(char_u *name, char_u *mname);
स्थिर पूर्णांक get_menu_cmd_modes(char_u *, पूर्णांक, पूर्णांक *, पूर्णांक *);
स्थिर char_u *popup_mode_name(char_u *name, पूर्णांक idx);
स्थिर char_u *menu_text(char_u *text, पूर्णांक *mnemonic, char_u **actext);

#जर defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
स्थिर शून्य gui_create_tearoffs_recurse(vimmenu_T *menu, स्थिरांक char_u *pname, पूर्णांक *pri_tab, पूर्णांक pri_idx);
स्थिर शून्य gui_add_tearoff(char_u *tearpath, पूर्णांक *pri_tab, पूर्णांक pri_idx);
स्थिर शून्य gui_destroy_tearoffs_recurse(vimmenu_T *menu);
स्थिर पूर्णांक s_tearoffs = FALSE;
#endif

स्थिर पूर्णांक menu_is_hidden(char_u *name);
स्थिर पूर्णांक menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
स्थिर पूर्णांक menus_locked = 0;

#जर defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
स्थिर char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
स्थिर char_u *menutrans_lookup(char_u *name, पूर्णांक len);
स्थिर शून्य menu_unescape_name(char_u	*p);
#endif

स्थिर char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
स्थिर अक्षर *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
स्थिर स्थिरांक अक्षर *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_मोजणी ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    स्थिर पूर्णांक
menu_is_winbar(char_u *name)
{
    परत (STRNCMP(name, "WinBar", 6) == 0);
}

    पूर्णांक
winbar_height(win_T *wp)
{
    जर (wp->w_winbar != रिक्त && wp->w_winbar->children != रिक्त)
	परत 1;
    परत 0;
}

    स्थिर vimmenu_T **
get_मूळ_menu(char_u *name)
{
    जर (menu_is_winbar(name))
	परत &curwin->w_winbar;
    परत &मूळ_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णांक
is_menus_locked(शून्य)
{
    जर (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    शून्य
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    पूर्णांक		modes;
    char_u	*map_to;
    पूर्णांक		noremap;
    पूर्णांक		silent = FALSE;
    पूर्णांक		special = FALSE;
    पूर्णांक		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    पूर्णांक		i;
#जर defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    पूर्णांक		old_menu_height;
# जर defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    पूर्णांक		old_toolbar_height;
# endif
#endif
    पूर्णांक		pri_tab[MENUDEPTH + 1];
    पूर्णांक		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = रिक्त;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**मूळ_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    साठी (;;)
    {
	जर (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    सुरूठेवा;
	}
	जर (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    सुरूठेवा;
	}
	जर (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    सुरूठेवा;
	}
	थांबा;
    }


    // Locate an optional "icon=filename" argument.
    जर (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	पर्यंत (*arg != NUL && *arg != ' ')
	{
	    जर (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	जर (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    साठी (p = arg; *p; ++p)
	जर (!VIM_ISDIGIT(*p) && *p != '.')
	    थांबा;
    जर (VIM_ISWHITE(*p))
    {
	साठी (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    जर (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    जर (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    अन्यथा जर (eap->addr_मोजणी && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    अन्यथा
	i = 0;
    पर्यंत (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    जर (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    अन्यथा जर (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    जर (*arg == NUL)
    {
	show_menus(arg, modes);
	परत;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    जर (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	जर (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    जर (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		जर (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = म_से_प((अक्षर *)menu_path + 7);
		    जर (menuarg.iconidx >= (पूर्णांक)TOOLBAR_NAME_मोजणी)
			menuarg.iconidx = -1;
		    अन्यथा
			menuarg.icon_builtin = TRUE;
		}
	    }
	    अन्यथा
	    {
		साठी (i = 0; i < (पूर्णांक)TOOLBAR_NAME_मोजणी; ++i)
		    जर (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			थांबा;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    जर (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	जा theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    जर (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	जा theend;
    }
    अन्यथा जर (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	जा theend;
    }
#जर defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# जर defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    मूळ_menu_ptr = get_मूळ_menu(menu_path);
    जर (मूळ_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    जर (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	जर (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	जर (menu_is_popup(menu_path))
	{
	    साठी (i = 0; i < MENU_INDEX_TIP; ++i)
		जर (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    जर (p != रिक्त)
		    {
			menu_nable_recurse(*मूळ_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*मूळ_menu_ptr, menu_path, modes, enable);
    }
    अन्यथा जर (unmenu)
    {
	जर (is_menus_locked())
	    जा theend;

	/*
	 * Delete menu(s).
	 */
	जर (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	जर (menu_is_popup(menu_path))
	{
	    साठी (i = 0; i < MENU_INDEX_TIP; ++i)
		जर (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    जर (p != रिक्त)
		    {
			remove_menu(मूळ_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(मूळ_menu_ptr, menu_path, modes, FALSE);
    }
    अन्यथा
    {
	जर (is_menus_locked())
	    जा theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	जर (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = रिक्त;
	}
	अन्यथा जर (modes & MENU_TIP_MODE)
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
	जर (menu_is_popup(menu_path))
	{
	    साठी (i = 0; i < MENU_INDEX_TIP; ++i)
		जर (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    जर (p != रिक्त)
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

#जर defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    जर (gui.in_use
	    && (gui.menu_height != old_menu_height
# जर defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    जर (मूळ_menu_ptr == &curwin->w_winbar)
    {
	पूर्णांक h = winbar_height(curwin);

	जर (h != curwin->w_winbar_height)
	{
	    जर (h == 0)
		++curwin->w_height;
	    अन्यथा जर (curwin->w_height > 0)
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
    स्थिर पूर्णांक
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    पूर्णांक		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , पूर्णांक	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    पूर्णांक		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = रिक्त;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    पूर्णांक		i;
    पूर्णांक		c;
    पूर्णांक		d;
#ifdef FEAT_GUI
    पूर्णांक		idx;
    पूर्णांक		new_idx;
#endif
    पूर्णांक		pri_idx = 0;
    पूर्णांक		old_modes = 0;
    पूर्णांक		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = रिक्त;
#endif
    vimmenu_T	**मूळ_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    जर (path_name == रिक्त)
	परत FAIL;
    मूळ_menu_ptr = get_मूळ_menu(menu_path);
    menup = मूळ_menu_ptr;
    parent = रिक्त;
    name = path_name;
    पर्यंत (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (पूर्णांक)STRLEN(name));
	जर (map_to != रिक्त)
	{
	    en_name = name;
	    name = map_to;
	}
	अन्यथा
	    en_name = रिक्त;
#endif
	dname = menu_text(name, रिक्त, रिक्त);
	जर (dname == रिक्त)
	    जा erret;
	जर (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    जा erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	पर्यंत (menu != रिक्त)
	{
	    जर (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		जर (*next_name == NUL && menu->children != रिक्त)
		{
		    जर (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    जा erret;
		}
		जर (*next_name != NUL && menu->children == रिक्त
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    जर (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    जा erret;
		}
		थांबा;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    जर (parent != रिक्त || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		जर (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	जर (menu == रिक्त)
	{
	    जर (*next_name == NUL && parent == रिक्त)
	    {
		emsg(_(e_must_not_add_menu_वस्तुसंच_directly_to_menu_bar));
		जा erret;
	    }

	    जर (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		जा erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    जर (menu == रिक्त)
		जा erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    जर (en_name != रिक्त)
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
	    जर (*next_name == NUL && menuarg->iconfile != रिक्त)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#जर defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    जर (parent != रिक्त && menu_is_tearoff(parent->children->dname))
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
	    जर (addtearoff)
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
	जर ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    जर (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		जर (*next_name == NUL)
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

# जर defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    जर (	addtearoff
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
		जर (tearpath != रिक्त)
		{
		    char_u  *s;
		    पूर्णांक	    len;

		    STRCPY(tearpath, menu_path);
		    len = (पूर्णांक)(next_name - path_name - 1);
		    साठी (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			जर ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	जर (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_घाला_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_घाला_MODE));
    जर (sys_menu)
	modes &= ~old_modes;

    जर (menu != रिक्त && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == रिक्त) ? रिक्त : vim_strsave(call_data);

	// loop over all modes, may add more than one
	साठी (i = 0; i < MENU_MODES; ++i)
	{
	    जर (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		जर (amenu && call_data != रिक्त && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    निवड (1 << i)
		    {
			स्थिती MENU_VISUAL_MODE:
			स्थिती MENU_SELECT_MODE:
			स्थिती MENU_OP_PENDING_MODE:
			स्थिती MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    थांबा;
			स्थिती MENU_घाला_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    थांबा;
		    }
		}

		जर (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    जर (menu->strings[i] != रिक्त)
		    {
			menu->strings[i][0] = c;
			जर (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			अन्यथा
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			जर (c == Ctrl_C)
			{
			    पूर्णांक	    len = (पूर्णांक)STRLEN(menu->strings[i]);

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
#जर defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	जर (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    परत OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    पर्यंत (parent != रिक्त && parent->children == रिक्त)
    {
	जर (parent->parent == रिक्त)
	    menup = मूळ_menu_ptr;
	अन्यथा
	    menup = &parent->parent->children;
	साठी ( ; *menup != रिक्त && *menup != parent; menup = &((*menup)->next))
	    ;
	जर (*menup == रिक्त) // safety check
	    थांबा;
	parent = parent->parent;
	free_menu(menup);
    }
    परत FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    स्थिर पूर्णांक
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    पूर्णांक		modes,
    पूर्णांक		enable)
{
    char_u	*p;

    जर (menu == रिक्त)
	परत OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    पर्यंत (menu != रिक्त)
    {
	जर (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    जर (*p != NUL)
	    {
		जर (menu->children == रिक्त)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    परत FAIL;
		}
		जर (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    परत FAIL;
	    }
	    अन्यथा
		जर (enable)
		    menu->enabled |= modes;
		अन्यथा
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    जर (*name != NUL && *name != '*')
		थांबा;
	}
	menu = menu->next;
    }
    जर (*name != NUL && *name != '*' && menu == रिक्त)
    {
	semsg(_(e_no_menu_str), name);
	परत FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    परत OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    स्थिर पूर्णांक
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    पूर्णांक		modes,
    पूर्णांक		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    जर (*menup == रिक्त)
	परत OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    पर्यंत ((menu = *menup) != रिक्त)
    {
	जर (*name == NUL || menu_name_equal(name, menu))
	{
	    जर (*p != NUL && menu->children == रिक्त)
	    {
		जर (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		परत FAIL;
	    }
	    जर ((menu->modes & modes) != 0x0)
	    {
#जर defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		जर (*p == NUL && modes == MENU_ALL_MODES)
		{
		    जर (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		जर (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    परत FAIL;
	    }
	    अन्यथा जर (*name != NUL)
	    {
		जर (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		परत FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    जर (*name != NUL)
		थांबा;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    जर (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    जर ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    अन्यथा
		menup = &menu->next;
	}
	अन्यथा
	    menup = &menu->next;
    }
    जर (*name != NUL)
    {
	जर (menu == रिक्त)
	{
	    जर (!silent)
		semsg(_(e_no_menu_str), name);
	    परत FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#जर defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	जर ((s_tearoffs) && (menu->children != रिक्त)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	अन्यथा
#endif
	    child = menu->children;
	साठी ( ; child != रिक्त; child = child->next)
	    menu->modes |= child->modes;
	जर (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#जर defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    जर (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	जर ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#जर defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    जर (s_tearoffs && menu->children != रिक्त) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    परत OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    शून्य
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_वस्तुसंच);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    स्थिर शून्य
free_menu(vimmenu_T **menup)
{
    पूर्णांक		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    जर (gui.in_use)
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
    साठी (i = 0; i < MENU_MODES; i++)
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
free_menu_string(vimmenu_T *menu, पूर्णांक idx)
{
    पूर्णांक		मोजणी = 0;
    पूर्णांक		i;

    साठी (i = 0; i < MENU_MODES; i++)
	जर (menu->strings[i] == menu->strings[idx])
	    मोजणी++;
    जर (मोजणी == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = रिक्त;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    स्थिर पूर्णांक
show_menus(char_u *path_name, पूर्णांक modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = रिक्त;

    name = path_name = vim_strsave(path_name);
    जर (path_name == रिक्त)
	परत FAIL;
    menu = *get_मूळ_menu(path_name);

    // First, find the (sub)menu with the given name
    पर्यंत (*name)
    {
	p = menu_name_skip(name);
	पर्यंत (menu != रिक्त)
	{
	    जर (menu_name_equal(name, menu))
	    {
		// Found menu
		जर (*p != NUL && menu->children == रिक्त)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    परत FAIL;
		}
		अन्यथा जर ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    परत FAIL;
		}
		थांबा;
	    }
	    menu = menu->next;
	}
	जर (menu == रिक्त)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    परत FAIL;
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
    परत OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    स्थिर शून्य
show_menus_recursive(vimmenu_T *menu, पूर्णांक modes, पूर्णांक depth)
{
    पूर्णांक		i;
    पूर्णांक		bit;

    जर (menu != रिक्त && (menu->modes & modes) == 0x0)
	परत;

    जर (menu != रिक्त)
    {
	संदेश_putchar('\n');
	जर (got_int)		// "q" hit for "--more--"
	    परत;
	साठी (i = 0; i < depth; i++)
	    संदेश_puts("  ");
	जर (menu->priority)
	{
	    संदेश_outnum((दीर्घ)menu->priority);
	    संदेश_puts(" ");
	}
				// Same highlighting as for directories!?
	संदेश_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    जर (menu != रिक्त && menu->children == रिक्त)
    {
	साठी (bit = 0; bit < MENU_MODES; bit++)
	    जर ((menu->modes & modes & (1 << bit)) != 0)
	    {
		संदेश_putchar('\n');
		जर (got_int)		// "q" hit for "--more--"
		    परत;
		साठी (i = 0; i < depth + 2; i++)
		    संदेश_puts("  ");
		संदेश_puts(menu_mode_chars[bit]);
		जर (menu->noremap[bit] == REMAP_NONE)
		    संदेश_putchar('*');
		अन्यथा जर (menu->noremap[bit] == REMAP_SCRIPT)
		    संदेश_putchar('&');
		अन्यथा
		    संदेश_putchar(' ');
		जर (menu->silent[bit])
		    संदेश_putchar('s');
		अन्यथा
		    संदेश_putchar(' ');
		जर ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    संदेश_putchar('-');
		अन्यथा
		    संदेश_putchar(' ');
		संदेश_puts(" ");
		जर (*menu->strings[bit] == NUL)
		    संदेश_puts_attr("<Nop>", HL_ATTR(HLF_8));
		अन्यथा
		    संदेश_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    अन्यथा
    {
	जर (menu == रिक्त)
	{
	    menu = मूळ_menu;
	    depth--;
	}
	अन्यथा
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	साठी (; menu != रिक्त && !got_int; menu = menu->next)
	    जर (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
स्थिर vimmenu_T	*expand_menu = रिक्त;
स्थिर vimmenu_T	*expand_menu_alt = रिक्त;
स्थिर पूर्णांक		expand_modes = 0x0;
स्थिर पूर्णांक		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    पूर्णांक		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = रिक्त;
    char_u	*name;
    पूर्णांक		unmenu;
    vimmenu_T	*menu;
    पूर्णांक		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    साठी (p = arg; *p; ++p)
	जर (!VIM_ISDIGIT(*p) && *p != '.')
	    थांबा;

    जर (!VIM_ISWHITE(*p))
    {
	जर (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	अन्यथा जर (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	अन्यथा
	    p = arg;
    }

    पर्यंत (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    साठी (; *p && !VIM_ISWHITE(*p); ++p)
    {
	जर ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	अन्यथा जर (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    जर (expand_menus && VIM_ISWHITE(*p))
	परत रिक्त;	// TODO: check for next command?
    जर (*p == NUL)		// Complete the menu name
    {
	पूर्णांक try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, रिक्त, &unmenu);
	जर (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = मूळ_menu;
	जर (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    जर (path_name == रिक्त)
		परत रिक्त;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	पर्यंत (name != रिक्त && *name)
	{
	    p = menu_name_skip(name);
	    पर्यंत (menu != रिक्त)
	    {
		जर (menu_name_equal(name, menu))
		{
		    // Found menu
		    जर ((*p != NUL && menu->children == रिक्त)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			परत रिक्त;
		    }
		    थांबा;
		}
		menu = menu->next;
		जर (menu == रिक्त && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    जर (menu == रिक्त)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		परत रिक्त;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	जर (expand_menu == मूळ_menu)
	    expand_menu_alt = curwin->w_winbar;
	अन्यथा
	    expand_menu_alt = रिक्त;
    }
    अन्यथा			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    परत रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    स्थिर vimmenu_T	*menu = रिक्त;
    स्थिर पूर्णांक		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    स्थिर  पूर्णांक		should_advance = FALSE;
#endif

    जर (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    पर्यंत (menu != रिक्त && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == रिक्त))
    {
	menu = menu->next;
	जर (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    जर (menu == रिक्त)	    // at end of linked list
	परत रिक्त;

    जर (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	जर (should_advance)
	    str = menu->en_dname;
	अन्यथा
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    जर (menu->en_dname == रिक्त)
		should_advance = TRUE;
	}
#endif
    अन्यथा
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    जर (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	जर (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    परत str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, पूर्णांक idx)
{
    स्थिर vimmenu_T	*menu = रिक्त;
    स्थिर पूर्णांक		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    स्थिर char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    स्थिर  पूर्णांक		should_advance = FALSE;
#endif

    जर (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    पर्यंत (menu != रिक्त
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	जर (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    जर (menu == रिक्त)	    // at end of linked list
	परत रिक्त;

    जर (menu->modes & expand_modes)
    {
	जर (menu->children != रिक्त)
	{
#ifdef FEAT_MULTI_LANG
	    जर (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    अन्यथा
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		जर (menu->en_dname == रिक्त)
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
	    जर (should_advance)
		str = menu->en_dname;
	    अन्यथा
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		जर (menu->en_dname == रिक्त)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    अन्यथा
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    जर (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	जर (menu == रिक्त && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    परत str;
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

    साठी (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	जर (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    जर (*p == NUL)
		थांबा;
	}
    }
    जर (*p)
	*p++ = NUL;
    परत p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    स्थिर पूर्णांक
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    जर (menu->en_name != रिक्त
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	परत TRUE;
#endif
    परत menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    स्थिर पूर्णांक
menu_namecmp(char_u *name, char_u *mname)
{
    पूर्णांक		i;

    साठी (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	जर (name[i] != mname[i])
	    थांबा;
    परत ((name[i] == NUL || name[i] == TAB)
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
    स्थिर पूर्णांक
get_menu_cmd_modes(
    char_u  *cmd,
    पूर्णांक	    forceit,	    // Was there a "!" after the command?
    पूर्णांक	    *noremap,
    पूर्णांक	    *unmenu)
{
    पूर्णांक	    modes;

    निवड (*cmd++)
    {
	स्थिती 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    थांबा;
	स्थिती 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    थांबा;
	स्थिती 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    थांबा;
	स्थिती 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    थांबा;
	स्थिती 'i':			// imenu
	    modes = MENU_घाला_MODE;
	    थांबा;
	स्थिती 't':
	    जर (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		थांबा;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    थांबा;
	स्थिती 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    थांबा;
	स्थिती 'a':			// amenu
	    modes = MENU_घाला_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    थांबा;
	स्थिती 'n':
	    जर (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		थांबा;
	    }
	    // FALLTHROUGH
	पूर्वनिर्धारित:
	    --cmd;
	    जर (forceit)		// menu!!
		modes = MENU_घाला_MODE | MENU_CMDLINE_MODE;
	    अन्यथा			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    जर (noremap != रिक्त)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    जर (unmenu != रिक्त)
	*unmenu = (*cmd == 'u');
    परत modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    स्थिर char_u *
get_menu_mode_str(पूर्णांक modes)
{
    जर ((modes & (MENU_घाला_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_घाला_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	परत (char_u *)"a";
    जर ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	परत (char_u *)" ";
    जर ((modes & (MENU_घाला_MODE | MENU_CMDLINE_MODE))
	    == (MENU_घाला_MODE | MENU_CMDLINE_MODE))
	परत (char_u *)"!";
    जर ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	परत (char_u *)"v";
    जर (modes & MENU_VISUAL_MODE)
	परत (char_u *)"x";
    जर (modes & MENU_SELECT_MODE)
	परत (char_u *)"s";
    जर (modes & MENU_OP_PENDING_MODE)
	परत (char_u *)"o";
    जर (modes & MENU_घाला_MODE)
	परत (char_u *)"i";
    जर (modes & MENU_TERMINAL_MODE)
	परत (char_u *)"tl";
    जर (modes & MENU_CMDLINE_MODE)
	परत (char_u *)"c";
    जर (modes & MENU_NORMAL_MODE)
	परत (char_u *)"n";
    जर (modes & MENU_TIP_MODE)
	परत (char_u *)"t";

    परत (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    स्थिर char_u *
popup_mode_name(char_u *name, पूर्णांक idx)
{
    char_u	*p;
    पूर्णांक		len = (पूर्णांक)STRLEN(name);
    अक्षर	*mode_chars = menu_mode_chars[idx];
    पूर्णांक		mode_chars_len = (पूर्णांक)म_माप(mode_chars);
    पूर्णांक		i;

    p = vim_strnsave(name, len + mode_chars_len);
    जर (p == रिक्त)
	परत रिक्त;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (माप_प्रकार)(len - 4));
    साठी (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    परत p;
}

#जर defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    पूर्णांक
get_menu_index(vimmenu_T *menu, पूर्णांक state)
{
    पूर्णांक		idx;

    जर ((state & MODE_घाला))
	idx = MENU_INDEX_घाला;
    अन्यथा जर (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    अन्यथा जर (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    अन्यथा जर (VIsual_active)
    {
	जर (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	अन्यथा
	    idx = MENU_INDEX_VISUAL;
    }
    अन्यथा जर (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    अन्यथा जर (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    अन्यथा जर ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    अन्यथा
	idx = MENU_INDEX_INVALID;

    जर (idx != MENU_INDEX_INVALID && menu->strings[idx] == रिक्त)
	idx = MENU_INDEX_INVALID;
    परत idx;
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
menu_text(char_u *str, पूर्णांक *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    जर (p != रिक्त)
    {
	जर (actext != रिक्त)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    अन्यथा
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    साठी (p = text; p != रिक्त; )
    {
	p = vim_strchr(p, '&');
	जर (p != रिक्त)
	{
	    जर (p[1] == NUL)	    // trailing "&"
		थांबा;
	    जर (mnemonic != रिक्त && p[1] != '&')
#जर !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#अन्यथा
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		अक्षर c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    परत text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    पूर्णांक
menu_is_menubar(char_u *name)
{
    परत (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    पूर्णांक
menu_is_popup(char_u *name)
{
    परत (STRNCMP(name, "PopUp", 5) == 0);
}

#जर defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    पूर्णांक
menu_is_child_of_popup(vimmenu_T *menu)
{
    पर्यंत (menu->parent != रिक्त)
	menu = menu->parent;
    परत menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    पूर्णांक
menu_is_toolbar(char_u *name)
{
    परत (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    पूर्णांक
menu_is_separator(char_u *name)
{
    परत (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    स्थिर पूर्णांक
menu_is_hidden(char_u *name)
{
    परत (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    स्थिर पूर्णांक
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    परत (STRCMP(name, TEAR_STRING) == 0);
#अन्यथा
    परत FALSE;
#endif
}

#जर defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    स्थिर पूर्णांक
get_menu_mode(शून्य)
{
# ifdef FEAT_TERMINAL
    जर (term_use_loop())
	परत MENU_INDEX_TERMINAL;
# endif
    जर (VIsual_active)
    {
	जर (VIsual_select)
	    परत MENU_INDEX_SELECT;
	परत MENU_INDEX_VISUAL;
    }
    जर (State & MODE_घाला)
	परत MENU_INDEX_घाला;
    जर ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	परत MENU_INDEX_CMDLINE;
    जर (finish_op)
	परत MENU_INDEX_OP_PENDING;
    जर (State & MODE_NORMAL)
	परत MENU_INDEX_NORMAL;
    जर (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	परत MENU_INDEX_घाला;
    परत MENU_INDEX_INVALID;
}

    पूर्णांक
get_menu_mode_flag(शून्य)
{
    पूर्णांक mode = get_menu_mode();

    जर (mode == MENU_INDEX_INVALID)
	परत 0;
    परत 1 << mode;
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
    पूर्णांक		menu_mode;
    अक्षर*	mode;
    पूर्णांक		mode_len;

    menu_mode = get_menu_mode();
    जर (menu_mode == MENU_INDEX_INVALID)
	परत;
    mode = menu_mode_chars[menu_mode];
    mode_len = (पूर्णांक)म_माप(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, रिक्त, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	जर (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    थांबा;

    // Only show a popup when it is defined and has entries
    जर (menu == रिक्त || menu->children == रिक्त)
	परत;

# जर defined(FEAT_GUI)
    जर (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# जर defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    अन्यथा
# endif
# जर defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#जर defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    पूर्णांक
check_menu_pointer(vimmenu_T *मूळ, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    साठी (p = मूळ; p != रिक्त; p = p->next)
	जर (p == menu_to_check
		|| (p->children != रिक्त
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    परत OK;
    परत FAIL;
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
    पूर्णांक		idx = 0;

    पर्यंत (menu != रिक्त)
    {
	// Don't add a menu when only a tip was defined.
	जर (menu->modes & MENU_ALL_MODES)
	{
	    जर (menu->children != रिक्त)
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
gui_update_menus_recurse(vimmenu_T *menu, पूर्णांक mode)
{
    पूर्णांक		grey;

    पर्यंत (menu)
    {
	जर ((menu->modes & menu->enabled & mode)
# जर defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	अन्यथा
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	जर (vim_strchr(p_go, GO_GREY) != रिक्त || menu->parent == रिक्त
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
gui_update_menus(पूर्णांक modes)
{
    स्थिर पूर्णांक	    prev_mode = -1;
    पूर्णांक		    mode = 0;

    जर (modes != 0x0)
	mode = modes;
    अन्यथा
	mode = get_menu_mode_flag();

    जर (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(मूळ_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    पूर्णांक
gui_is_menu_shortcut(पूर्णांक किल्ली)
{
    vimmenu_T	*menu;

    जर (किल्ली < 256)
	किल्ली = TOLOWER_LOC(किल्ली);
    FOR_ALL_MENUS(menu)
	जर (menu->mnemonic == किल्ली
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == किल्ली))
	    परत TRUE;
    परत FALSE;
}
# endif
#endif // FEAT_GUI

#जर defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    शून्य
gui_mch_toggle_tearoffs(पूर्णांक enable)
{
    पूर्णांक		pri_tab[MENUDEPTH + 1];
    पूर्णांक		i;

    जर (enable)
    {
	साठी (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(मूळ_menu, (char_u *)"", pri_tab, 0);
    }
    अन्यथा
	gui_destroy_tearoffs_recurse(मूळ_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    स्थिर शून्य
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    स्थिरांक char_u	*pname,
    पूर्णांक			*pri_tab,
    पूर्णांक			pri_idx)
{
    char_u	*newpname = रिक्त;
    पूर्णांक		len;
    char_u	*s;
    char_u	*d;

    जर (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    पर्यंत (menu != रिक्त)
    {
	जर (menu->children != रिक्त && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (पूर्णांक)STRLEN(pname) + (पूर्णांक)STRLEN(menu->name);
	    साठी (s = menu->name; *s; ++s)
		जर (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    जर (newpname != रिक्त)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		साठी (s = menu->name; *s; ++s)
		{
		    जर (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		जर (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
gui_add_tearoff(char_u *tearpath, पूर्णांक *pri_tab, पूर्णांक pri_idx)
{
    char_u	*tbuf;
    पूर्णांक		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (अचिन्हित पूर्णांक)STRLEN(tearpath));
    जर (tbuf == रिक्त)
	परत;

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
    पर्यंत (menu)
    {
	जर (menu->children)
	{
	    // check if tearoff exists
	    जर (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    जर (menu->children != रिक्त) // if not the last one
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
execute_menu(exarg_T *eap, vimmenu_T *menu, पूर्णांक mode_idx)
{
    पूर्णांक		idx = mode_idx;

    जर (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	जर (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_घाला;
	}
#ifdef FEAT_TERMINAL
	अन्यथा जर (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	अन्यथा जर (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	अन्यथा जर (eap != रिक्त && eap->addr_मोजणी)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    जर ((curbuf->b_visual.vi_start.lnum == eap->line1)
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
	    जर (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    जर (idx == MENU_INDEX_INVALID || eap == रिक्त)
	idx = MENU_INDEX_NORMAL;

    जर (menu->strings[idx] != रिक्त && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	जर (eap == रिक्त || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    जर (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	अन्यथा
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    अन्यथा जर (eap != रिक्त)
    {
	char_u	*mode;

	निवड (idx)
	{
	    स्थिती MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		थांबा;
	    स्थिती MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		थांबा;
	    स्थिती MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		थांबा;
	    स्थिती MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		थांबा;
	    स्थिती MENU_INDEX_घाला:
		mode = (char_u *)"Insert";
		थांबा;
	    स्थिती MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		थांबा;
	    // case MENU_INDEX_TIP: cannot happen
	    पूर्वनिर्धारित:
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
    पूर्णांक		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    जर (saved_name == रिक्त)
	परत रिक्त;

    menu = *get_मूळ_menu(saved_name);
    name = saved_name;
    पर्यंत (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	पर्यंत (menu != रिक्त)
	{
	    जर (menu_name_equal(name, menu))
	    {
		जर (*p == NUL && menu->children != रिक्त)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = रिक्त;
		}
		अन्यथा जर (*p != NUL && menu->children == रिक्त)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = रिक्त;
		}
		थांबा;
	    }
	    menu = menu->next;
	}
	जर (menu == रिक्त || *p == NUL)
	    थांबा;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    जर (menu == रिक्त)
    {
	जर (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	परत रिक्त;
    }

    परत menu;
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
    पूर्णांक		mode_idx = MENU_INDEX_INVALID;

    जर (arg[0] && VIM_ISWHITE(arg[1]))
    {
	निवड (arg[0])
	{
	    स्थिती 'n': mode_idx = MENU_INDEX_NORMAL; थांबा;
	    स्थिती 'v': mode_idx = MENU_INDEX_VISUAL; थांबा;
	    स्थिती 's': mode_idx = MENU_INDEX_SELECT; थांबा;
	    स्थिती 'o': mode_idx = MENU_INDEX_OP_PENDING; थांबा;
	    स्थिती 't': mode_idx = MENU_INDEX_TERMINAL; थांबा;
	    स्थिती 'i': mode_idx = MENU_INDEX_घाला; थांबा;
	    स्थिती 'c': mode_idx = MENU_INDEX_CMDLINE; थांबा;
	    पूर्वनिर्धारित: semsg(_(e_invalid_argument_str), arg);
		     परत;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    जर (menu == रिक्त)
	परत;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    शून्य
winbar_click(win_T *wp, पूर्णांक col)
{
    पूर्णांक		idx;

    जर (wp->w_winbar_वस्तुसंच == रिक्त)
	परत;
    साठी (idx = 0; wp->w_winbar_वस्तुसंच[idx].wb_menu != रिक्त; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_वस्तुसंच[idx];

	जर (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = रिक्त;
	    pos_T   save_visual = VIsual;
	    पूर्णांक	    save_visual_active = VIsual_active;
	    पूर्णांक	    save_visual_select = VIsual_select;
	    पूर्णांक	    save_visual_reselect = VIsual_reselect;
	    पूर्णांक	    save_visual_mode = VIsual_mode;

	    जर (wp != curwin)
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

	    जर (save_curwin != रिक्त && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    जर (!win_valid(wp))
		थांबा;
	}
    }
}

#जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
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

    menu = *get_मूळ_menu(path_name);

    saved_name = vim_strsave(path_name);
    जर (saved_name == रिक्त)
	परत रिक्त;

    name = saved_name;
    पर्यंत (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	पर्यंत (menu != रिक्त)
	{
	    जर (menu_name_equal(name, menu))
	    {
		जर (menu->children == रिक्त)
		{
		    // found a menu item instead of a sub-menu
		    जर (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    अन्यथा
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = रिक्त;
		    जा theend;
		}
		जर (*p == NUL)	    // found a full match
		    जा theend;
		थांबा;
	    }
	    menu = menu->next;
	}
	जर (menu == रिक्त)	// didn't find it
	    थांबा;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    जर (menu == रिक्त)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    परत menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

प्रकारव्याख्या संरचना
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
    पूर्णांक			i;
    char_u		*from, *from_noamp, *to;

    जर (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, आकार(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    जर (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	साठी (i = 0; i < menutrans_ga.ga_len; ++i)
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
	जर (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	अन्यथा
	{
	    जर (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		जर (from != रिक्त)
		{
		    from_noamp = menu_text(from, रिक्त, रिक्त);
		    to = vim_strnsave(to, arg - to);
		    जर (from_noamp != रिक्त && to != रिक्त)
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

#जर defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    स्थिर char_u *
menu_skip_part(char_u *p)
{
    पर्यंत (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	जर ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    परत p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    स्थिर char_u *
menutrans_lookup(char_u *name, पूर्णांक len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    पूर्णांक			i;
    char_u		*dname;

    साठी (i = 0; i < menutrans_ga.ga_len; ++i)
	जर (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    परत tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, रिक्त, रिक्त);
    name[len] = i;
    जर (dname == रिक्त)
	परत रिक्त;

    साठी (i = 0; i < menutrans_ga.ga_len; ++i)
	जर (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    परत tp[i].to;
	}
    vim_free(dname);

    परत रिक्त;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    स्थिर शून्य
menu_unescape_name(char_u *name)
{
    char_u  *p;

    साठी (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	जर (*p == '\\')
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

    पर्यंत (*arg && !VIM_ISWHITE(*arg))
    {
	जर ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	अन्यथा जर (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    जर (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    परत arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    स्थिर पूर्णांक
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, पूर्णांक modes, dict_T *dict)
{
    पूर्णांक		status;
    list_T	*l;

    जर (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	जर (l == रिक्त)
	    परत FAIL;

	जर (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    परत FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	साठी (topmenu = menu; topmenu != रिक्त; topmenu = topmenu->next)
	    जर (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	परत OK;
    }

    जर (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	परत OK;

    status = dict_add_string(dict, "name", menu->name);
    जर (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    जर (status == OK && menu->actext != रिक्त)
	status = dict_add_string(dict, "accel", menu->actext);
    जर (status == OK)
	status = dict_add_संख्या(dict, "priority", menu->priority);
    जर (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    जर (status == OK && menu->iconfile != रिक्त)
	status = dict_add_string(dict, "icon", menu->iconfile);
    जर (status == OK && menu->iconidx >= 0)
	status = dict_add_संख्या(dict, "iconidx", menu->iconidx);
#endif
    जर (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	पूर्णांक	buflen;

	जर (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	अन्यथा
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    जर (status == OK && menu->children == रिक्त)
    {
	पूर्णांक		bit;

	// Get the first mode in which the menu is available
	साठी (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	जर (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    जर (menu->strings[bit] != रिक्त)
	    {
		जर (*menu->strings[bit] == NUL)
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
	    जर (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    जर (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    जर (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    जर (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    जर (status == OK && menu->children != रिक्त)
    {
	vimmenu_T	*child;

	l = list_alloc();
	जर (l == रिक्त)
	    परत FAIL;

	जर (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    परत FAIL;
	}
	child = menu->children;
	पर्यंत (child)
	{
	    जर (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    परत status;
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
    पूर्णांक		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    जर (rettv_dict_alloc(rettv) == FAIL)
	परत;
    retdict = rettv->vval.v_dict;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	परत;

    menu_name = tv_get_string_chk(&argvars[0]);
    जर (menu_name == रिक्त)
	परत;

    // menu mode
    जर (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    अन्यथा
	which = (char_u *)"";	    // Default is modes for "menu"
    जर (which == रिक्त)
	परत;

    modes = get_menu_cmd_modes(which, *which == '!', रिक्त, रिक्त);

    // Locate the specified menu or menu item
    menu = *get_मूळ_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    जर (saved_name == रिक्त)
	परत;
    जर (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	पर्यंत (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    पर्यंत (menu != रिक्त)
	    {
		जर (menu_name_equal(name, menu))
		    थांबा;
		menu = menu->next;
	    }
	    जर (menu == रिक्त || *p == NUL)
		थांबा;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    जर (menu == रिक्त)		// specified menu not found
	परत;

    जर (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
