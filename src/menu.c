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

#include "vim.h"

#もし defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
static 整数 add_menu_path(char_u *, vimmenu_T *, 整数 *, char_u *, 整数);
#否则
static 整数 add_menu_path(char_u *, vimmenu_T *, 整数 *, char_u *);
#endif
static 整数 menu_nable_recurse(vimmenu_T *menu, char_u *name, 整数 modes, 整数 enable);
static 整数 remove_menu(vimmenu_T **, char_u *, 整数, 整数 silent);
static void free_menu(vimmenu_T **menup);
static void free_menu_string(vimmenu_T *, 整数);
static 整数 show_menus(char_u *, 整数);
static void show_menus_recursive(vimmenu_T *, 整数, 整数);
static char_u *menu_name_skip(char_u *name);
static 整数 menu_name_equal(char_u *name, vimmenu_T *menu);
static 整数 menu_namecmp(char_u *name, char_u *mname);
static 整数 get_menu_cmd_modes(char_u *, 整数, 整数 *, 整数 *);
static char_u *popup_mode_name(char_u *name, 整数 idx);
static char_u *menu_text(char_u *text, 整数 *mnemonic, char_u **actext);

#もし defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
static void gui_create_tearoffs_recurse(vimmenu_T *menu, const char_u *pname, 整数 *pri_tab, 整数 pri_idx);
static void gui_add_tearoff(char_u *tearpath, 整数 *pri_tab, 整数 pri_idx);
static void gui_destroy_tearoffs_recurse(vimmenu_T *menu);
static 整数 s_tearoffs = FALSE;
#endif

static 整数 menu_is_hidden(char_u *name);
static 整数 menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
static 整数 menus_locked = 0;

#もし defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
static char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
static char_u *menutrans_lookup(char_u *name, 整数 len);
static void menu_unescape_name(char_u	*p);
#endif

static char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
static char *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
static const char *toolbar_names[] =
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
    static 整数
menu_is_winbar(char_u *name)
{
    戻る (STRNCMP(name, "WinBar", 6) == 0);
}

    整数
winbar_height(win_T *wp)
{
    もし (wp->w_winbar != NULL && wp->w_winbar->children != NULL)
	戻る 1;
    戻る 0;
}

    static vimmenu_T **
get_root_menu(char_u *name)
{
    もし (menu_is_winbar(name))
	戻る &curwin->w_winbar;
    戻る &root_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static 整数
is_menus_locked(void)
{
    もし (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	戻る TRUE;
    }
    戻る FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    void
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    整数		modes;
    char_u	*map_to;
    整数		noremap;
    整数		silent = FALSE;
    整数		special = FALSE;
    整数		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    整数		i;
#もし defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    整数		old_menu_height;
# もし defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    整数		old_toolbar_height;
# endif
#endif
    整数		pri_tab[MENUDEPTH + 1];
    整数		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = NULL;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**root_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    ために (;;)
    {
	もし (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	もし (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	もし (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    continue;
	}
	break;
    }


    // Locate an optional "icon=filename" argument.
    もし (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	ながら (*arg != NUL && *arg != ' ')
	{
	    もし (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	もし (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    ために (p = arg; *p; ++p)
	もし (!VIM_ISDIGIT(*p) && *p != '.')
	    break;
    もし (VIM_ISWHITE(*p))
    {
	ために (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    もし (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    もし (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    否则 もし (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    否则
	i = 0;
    ながら (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    もし (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    否则 もし (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    もし (*arg == NUL)
    {
	show_menus(arg, modes);
	戻る;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    もし (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	もし (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    もし (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		もし (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((char *)menu_path + 7);
		    もし (menuarg.iconidx >= (整数)TOOLBAR_NAME_COUNT)
			menuarg.iconidx = -1;
		    否则
			menuarg.icon_builtin = TRUE;
		}
	    }
	    否则
	    {
		ために (i = 0; i < (整数)TOOLBAR_NAME_COUNT; ++i)
		    もし (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			break;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    もし (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	goto theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    もし (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	goto theend;
    }
    否则 もし (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	goto theend;
    }
#もし defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# もし defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    root_menu_ptr = get_root_menu(menu_path);
    もし (root_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    もし (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	もし (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	もし (menu_is_popup(menu_path))
	{
	    ために (i = 0; i < MENU_INDEX_TIP; ++i)
		もし (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    もし (p != NULL)
		    {
			menu_nable_recurse(*root_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*root_menu_ptr, menu_path, modes, enable);
    }
    否则 もし (unmenu)
    {
	もし (is_menus_locked())
	    goto theend;

	/*
	 * Delete menu(s).
	 */
	もし (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	もし (menu_is_popup(menu_path))
	{
	    ために (i = 0; i < MENU_INDEX_TIP; ++i)
		もし (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    もし (p != NULL)
		    {
			remove_menu(root_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(root_menu_ptr, menu_path, modes, FALSE);
    }
    否则
    {
	もし (is_menus_locked())
	    goto theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	もし (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = NULL;
	}
	否则 もし (modes & MENU_TIP_MODE)
	    map_buf = NULL;	// Menu tips are plain text.
	否则
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), NULL);
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
	もし (menu_is_popup(menu_path))
	{
	    ために (i = 0; i < MENU_INDEX_TIP; ++i)
		もし (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    もし (p != NULL)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = NULL;
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

#もし defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    もし (gui.in_use
	    && (gui.menu_height != old_menu_height
# もし defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    もし (root_menu_ptr == &curwin->w_winbar)
    {
	整数 h = winbar_height(curwin);

	もし (h != curwin->w_winbar_height)
	{
	    もし (h == 0)
		++curwin->w_height;
	    否则 もし (curwin->w_height > 0)
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
    static 整数
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    整数		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , 整数	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    整数		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = NULL;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    整数		i;
    整数		c;
    整数		d;
#ifdef FEAT_GUI
    整数		idx;
    整数		new_idx;
#endif
    整数		pri_idx = 0;
    整数		old_modes = 0;
    整数		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = NULL;
#endif
    vimmenu_T	**root_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    もし (path_name == NULL)
	戻る FAIL;
    root_menu_ptr = get_root_menu(menu_path);
    menup = root_menu_ptr;
    parent = NULL;
    name = path_name;
    ながら (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (整数)STRLEN(name));
	もし (map_to != NULL)
	{
	    en_name = name;
	    name = map_to;
	}
	否则
	    en_name = NULL;
#endif
	dname = menu_text(name, NULL, NULL);
	もし (dname == NULL)
	    goto erret;
	もし (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    goto erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	ながら (menu != NULL)
	{
	    もし (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		もし (*next_name == NUL && menu->children != NULL)
		{
		    もし (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    goto erret;
		}
		もし (*next_name != NUL && menu->children == NULL
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    もし (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    goto erret;
		}
		break;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    もし (parent != NULL || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		もし (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	もし (menu == NULL)
	{
	    もし (*next_name == NUL && parent == NULL)
	    {
		emsg(_(e_must_not_add_menu_items_directly_to_menu_bar));
		goto erret;
	    }

	    もし (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		goto erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    もし (menu == NULL)
		goto erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    もし (en_name != NULL)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, NULL, NULL);
	    }
	    否则
	    {
		menu->en_name = NULL;
		menu->en_dname = NULL;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = NULL;
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
	    もし (*next_name == NUL && menuarg->iconfile != NULL)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#もし defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    もし (parent != NULL && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	否则
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    もし (addtearoff)
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
	もし ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    もし (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		もし (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		否则
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# もし defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    もし (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != NULL
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
		もし (tearpath != NULL)
		{
		    char_u  *s;
		    整数	    len;

		    STRCPY(tearpath, menu_path);
		    len = (整数)(next_name - path_name - 1);
		    ために (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			もし ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	もし (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    もし (sys_menu)
	modes &= ~old_modes;

    もし (menu != NULL && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == NULL) ? NULL : vim_strsave(call_data);

	// loop over all modes, may add more than one
	ために (i = 0; i < MENU_MODES; ++i)
	{
	    もし (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		もし (amenu && call_data != NULL && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    switch (1 << i)
		    {
			case MENU_VISUAL_MODE:
			case MENU_SELECT_MODE:
			case MENU_OP_PENDING_MODE:
			case MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    break;
			case MENU_INSERT_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    break;
		    }
		}

		もし (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    もし (menu->strings[i] != NULL)
		    {
			menu->strings[i][0] = c;
			もし (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			否则
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			もし (c == Ctrl_C)
			{
			    整数	    len = (整数)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		否则
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#もし defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	もし (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    戻る OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    ながら (parent != NULL && parent->children == NULL)
    {
	もし (parent->parent == NULL)
	    menup = root_menu_ptr;
	否则
	    menup = &parent->parent->children;
	ために ( ; *menup != NULL && *menup != parent; menup = &((*menup)->next))
	    ;
	もし (*menup == NULL) // safety check
	    break;
	parent = parent->parent;
	free_menu(menup);
    }
    戻る FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    static 整数
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    整数		modes,
    整数		enable)
{
    char_u	*p;

    もし (menu == NULL)
	戻る OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ながら (menu != NULL)
    {
	もし (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    もし (*p != NUL)
	    {
		もし (menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    戻る FAIL;
		}
		もし (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    戻る FAIL;
	    }
	    否则
		もし (enable)
		    menu->enabled |= modes;
		否则
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    もし (*name != NUL && *name != '*')
		break;
	}
	menu = menu->next;
    }
    もし (*name != NUL && *name != '*' && menu == NULL)
    {
	semsg(_(e_no_menu_str), name);
	戻る FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    戻る OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    static 整数
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    整数		modes,
    整数		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    もし (*menup == NULL)
	戻る OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ながら ((menu = *menup) != NULL)
    {
	もし (*name == NUL || menu_name_equal(name, menu))
	{
	    もし (*p != NUL && menu->children == NULL)
	    {
		もし (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		戻る FAIL;
	    }
	    もし ((menu->modes & modes) != 0x0)
	    {
#もし defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		もし (*p == NUL && modes == MENU_ALL_MODES)
		{
		    もし (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		もし (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    戻る FAIL;
	    }
	    否则 もし (*name != NUL)
	    {
		もし (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		戻る FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    もし (*name != NUL)
		break;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    もし (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    もし ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    否则
		menup = &menu->next;
	}
	否则
	    menup = &menu->next;
    }
    もし (*name != NUL)
    {
	もし (menu == NULL)
	{
	    もし (!silent)
		semsg(_(e_no_menu_str), name);
	    戻る FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#もし defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	もし ((s_tearoffs) && (menu->children != NULL)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	否则
#endif
	    child = menu->children;
	ために ( ; child != NULL; child = child->next)
	    menu->modes |= child->modes;
	もし (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#もし defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    もし (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	もし ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#もし defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    もし (s_tearoffs && menu->children != NULL) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    戻る OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    void
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_items);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    static void
free_menu(vimmenu_T **menup)
{
    整数		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    もし (gui.in_use)
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
    ために (i = 0; i < MENU_MODES; i++)
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
    static void
free_menu_string(vimmenu_T *menu, 整数 idx)
{
    整数		count = 0;
    整数		i;

    ために (i = 0; i < MENU_MODES; i++)
	もし (menu->strings[i] == menu->strings[idx])
	    count++;
    もし (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = NULL;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    static 整数
show_menus(char_u *path_name, 整数 modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = NULL;

    name = path_name = vim_strsave(path_name);
    もし (path_name == NULL)
	戻る FAIL;
    menu = *get_root_menu(path_name);

    // First, find the (sub)menu with the given name
    ながら (*name)
    {
	p = menu_name_skip(name);
	ながら (menu != NULL)
	{
	    もし (menu_name_equal(name, menu))
	    {
		// Found menu
		もし (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    戻る FAIL;
		}
		否则 もし ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    戻る FAIL;
		}
		break;
	    }
	    menu = menu->next;
	}
	もし (menu == NULL)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    戻る FAIL;
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
    戻る OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    static void
show_menus_recursive(vimmenu_T *menu, 整数 modes, 整数 depth)
{
    整数		i;
    整数		bit;

    もし (menu != NULL && (menu->modes & modes) == 0x0)
	戻る;

    もし (menu != NULL)
    {
	msg_putchar('\n');
	もし (got_int)		// "q" hit for "--more--"
	    戻る;
	ために (i = 0; i < depth; i++)
	    msg_puts("  ");
	もし (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    msg_puts(" ");
	}
				// Same highlighting as for directories!?
	msg_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    もし (menu != NULL && menu->children == NULL)
    {
	ために (bit = 0; bit < MENU_MODES; bit++)
	    もし ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		もし (got_int)		// "q" hit for "--more--"
		    戻る;
		ために (i = 0; i < depth + 2; i++)
		    msg_puts("  ");
		msg_puts(menu_mode_chars[bit]);
		もし (menu->noremap[bit] == REMAP_NONE)
		    msg_putchar('*');
		否则 もし (menu->noremap[bit] == REMAP_SCRIPT)
		    msg_putchar('&');
		否则
		    msg_putchar(' ');
		もし (menu->silent[bit])
		    msg_putchar('s');
		否则
		    msg_putchar(' ');
		もし ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    msg_putchar('-');
		否则
		    msg_putchar(' ');
		msg_puts(" ");
		もし (*menu->strings[bit] == NUL)
		    msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
		否则
		    msg_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    否则
    {
	もし (menu == NULL)
	{
	    menu = root_menu;
	    depth--;
	}
	否则
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	ために (; menu != NULL && !got_int; menu = menu->next)
	    もし (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
static vimmenu_T	*expand_menu = NULL;
static vimmenu_T	*expand_menu_alt = NULL;
static 整数		expand_modes = 0x0;
static 整数		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    整数		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = NULL;
    char_u	*name;
    整数		unmenu;
    vimmenu_T	*menu;
    整数		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    ために (p = arg; *p; ++p)
	もし (!VIM_ISDIGIT(*p) && *p != '.')
	    break;

    もし (!VIM_ISWHITE(*p))
    {
	もし (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	否则 もし (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	否则
	    p = arg;
    }

    ながら (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    ために (; *p && !VIM_ISWHITE(*p); ++p)
    {
	もし ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	否则 もし (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    もし (expand_menus && VIM_ISWHITE(*p))
	戻る NULL;	// TODO: check for next command?
    もし (*p == NUL)		// Complete the menu name
    {
	整数 try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, NULL, &unmenu);
	もし (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	もし (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    もし (path_name == NULL)
		戻る NULL;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	ながら (name != NULL && *name)
	{
	    p = menu_name_skip(name);
	    ながら (menu != NULL)
	    {
		もし (menu_name_equal(name, menu))
		{
		    // Found menu
		    もし ((*p != NUL && menu->children == NULL)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			戻る NULL;
		    }
		    break;
		}
		menu = menu->next;
		もし (menu == NULL && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    もし (menu == NULL)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		戻る NULL;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	もし (expand_menu == root_menu)
	    expand_menu_alt = curwin->w_winbar;
	否则
	    expand_menu_alt = NULL;
    }
    否则			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    戻る NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, 整数 idx)
{
    static vimmenu_T	*menu = NULL;
    static 整数		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  整数		should_advance = FALSE;
#endif

    もし (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    ながら (menu != NULL && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == NULL))
    {
	menu = menu->next;
	もし (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    もし (menu == NULL)	    // at end of linked list
	戻る NULL;

    もし (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	もし (should_advance)
	    str = menu->en_dname;
	否则
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    もし (menu->en_dname == NULL)
		should_advance = TRUE;
	}
#endif
    否则
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    もし (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	もし (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    戻る str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, 整数 idx)
{
    static vimmenu_T	*menu = NULL;
    static 整数		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    static char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  整数		should_advance = FALSE;
#endif

    もし (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    ながら (menu != NULL
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	もし (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    もし (menu == NULL)	    // at end of linked list
	戻る NULL;

    もし (menu->modes & expand_modes)
    {
	もし (menu->children != NULL)
	{
#ifdef FEAT_MULTI_LANG
	    もし (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    否则
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		もし (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	否则
#ifdef FEAT_MULTI_LANG
	{
	    もし (should_advance)
		str = menu->en_dname;
	    否则
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		もし (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    否则
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    もし (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	もし (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    戻る str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    static char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    ために (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	もし (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    もし (*p == NUL)
		break;
	}
    }
    もし (*p)
	*p++ = NUL;
    戻る p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    static 整数
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    もし (menu->en_name != NULL
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	戻る TRUE;
#endif
    戻る menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    static 整数
menu_namecmp(char_u *name, char_u *mname)
{
    整数		i;

    ために (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	もし (name[i] != mname[i])
	    break;
    戻る ((name[i] == NUL || name[i] == TAB)
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
    static 整数
get_menu_cmd_modes(
    char_u  *cmd,
    整数	    forceit,	    // Was there a "!" after the command?
    整数	    *noremap,
    整数	    *unmenu)
{
    整数	    modes;

    switch (*cmd++)
    {
	case 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    break;
	case 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    break;
	case 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    break;
	case 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    break;
	case 'i':			// imenu
	    modes = MENU_INSERT_MODE;
	    break;
	case 't':
	    もし (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		break;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    break;
	case 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    break;
	case 'a':			// amenu
	    modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    break;
	case 'n':
	    もし (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		break;
	    }
	    // FALLTHROUGH
	default:
	    --cmd;
	    もし (forceit)		// menu!!
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    否则			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    もし (noremap != NULL)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    もし (unmenu != NULL)
	*unmenu = (*cmd == 'u');
    戻る modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    static char_u *
get_menu_mode_str(整数 modes)
{
    もし ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	戻る (char_u *)"a";
    もし ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	戻る (char_u *)" ";
    もし ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	戻る (char_u *)"!";
    もし ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	戻る (char_u *)"v";
    もし (modes & MENU_VISUAL_MODE)
	戻る (char_u *)"x";
    もし (modes & MENU_SELECT_MODE)
	戻る (char_u *)"s";
    もし (modes & MENU_OP_PENDING_MODE)
	戻る (char_u *)"o";
    もし (modes & MENU_INSERT_MODE)
	戻る (char_u *)"i";
    もし (modes & MENU_TERMINAL_MODE)
	戻る (char_u *)"tl";
    もし (modes & MENU_CMDLINE_MODE)
	戻る (char_u *)"c";
    もし (modes & MENU_NORMAL_MODE)
	戻る (char_u *)"n";
    もし (modes & MENU_TIP_MODE)
	戻る (char_u *)"t";

    戻る (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    static char_u *
popup_mode_name(char_u *name, 整数 idx)
{
    char_u	*p;
    整数		len = (整数)STRLEN(name);
    char	*mode_chars = menu_mode_chars[idx];
    整数		mode_chars_len = (整数)strlen(mode_chars);
    整数		i;

    p = vim_strnsave(name, len + mode_chars_len);
    もし (p == NULL)
	戻る NULL;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    ために (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    戻る p;
}

#もし defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    整数
get_menu_index(vimmenu_T *menu, 整数 state)
{
    整数		idx;

    もし ((state & MODE_INSERT))
	idx = MENU_INDEX_INSERT;
    否则 もし (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    否则 もし (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    否则 もし (VIsual_active)
    {
	もし (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	否则
	    idx = MENU_INDEX_VISUAL;
    }
    否则 もし (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    否则 もし (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    否则 もし ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    否则
	idx = MENU_INDEX_INVALID;

    もし (idx != MENU_INDEX_INVALID && menu->strings[idx] == NULL)
	idx = MENU_INDEX_INVALID;
    戻る idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    static char_u *
menu_text(char_u *str, 整数 *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    もし (p != NULL)
    {
	もし (actext != NULL)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    否则
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    ために (p = text; p != NULL; )
    {
	p = vim_strchr(p, '&');
	もし (p != NULL)
	{
	    もし (p[1] == NUL)	    // trailing "&"
		break;
	    もし (mnemonic != NULL && p[1] != '&')
#もし !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#否则
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		char c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    戻る text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    整数
menu_is_menubar(char_u *name)
{
    戻る (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    整数
menu_is_popup(char_u *name)
{
    戻る (STRNCMP(name, "PopUp", 5) == 0);
}

#もし defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    整数
menu_is_child_of_popup(vimmenu_T *menu)
{
    ながら (menu->parent != NULL)
	menu = menu->parent;
    戻る menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    整数
menu_is_toolbar(char_u *name)
{
    戻る (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    整数
menu_is_separator(char_u *name)
{
    戻る (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    static 整数
menu_is_hidden(char_u *name)
{
    戻る (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    static 整数
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    戻る (STRCMP(name, TEAR_STRING) == 0);
#否则
    戻る FALSE;
#endif
}

#もし defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    static 整数
get_menu_mode(void)
{
# ifdef FEAT_TERMINAL
    もし (term_use_loop())
	戻る MENU_INDEX_TERMINAL;
# endif
    もし (VIsual_active)
    {
	もし (VIsual_select)
	    戻る MENU_INDEX_SELECT;
	戻る MENU_INDEX_VISUAL;
    }
    もし (State & MODE_INSERT)
	戻る MENU_INDEX_INSERT;
    もし ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	戻る MENU_INDEX_CMDLINE;
    もし (finish_op)
	戻る MENU_INDEX_OP_PENDING;
    もし (State & MODE_NORMAL)
	戻る MENU_INDEX_NORMAL;
    もし (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	戻る MENU_INDEX_INSERT;
    戻る MENU_INDEX_INVALID;
}

    整数
get_menu_mode_flag(void)
{
    整数 mode = get_menu_mode();

    もし (mode == MENU_INDEX_INVALID)
	戻る 0;
    戻る 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    void
show_popupmenu(void)
{
    vimmenu_T	*menu;
    整数		menu_mode;
    char*	mode;
    整数		mode_len;

    menu_mode = get_menu_mode();
    もし (menu_mode == MENU_INDEX_INVALID)
	戻る;
    mode = menu_mode_chars[menu_mode];
    mode_len = (整数)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, NULL, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	もし (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    break;

    // Only show a popup when it is defined and has entries
    もし (menu == NULL || menu->children == NULL)
	戻る;

# もし defined(FEAT_GUI)
    もし (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# もし defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    否则
# endif
# もし defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#もし defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    整数
check_menu_pointer(vimmenu_T *root, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    ために (p = root; p != NULL; p = p->next)
	もし (p == menu_to_check
		|| (p->children != NULL
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    戻る OK;
    戻る FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    void
gui_create_initial_menus(vimmenu_T *menu)
{
    整数		idx = 0;

    ながら (menu != NULL)
    {
	// Don't add a menu when only a tip was defined.
	もし (menu->modes & MENU_ALL_MODES)
	{
	    もし (menu->children != NULL)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    否则
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    static void
gui_update_menus_recurse(vimmenu_T *menu, 整数 mode)
{
    整数		grey;

    ながら (menu)
    {
	もし ((menu->modes & menu->enabled & mode)
# もし defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	否则
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	もし (vim_strchr(p_go, GO_GREY) != NULL || menu->parent == NULL
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	否则
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
    void
gui_update_menus(整数 modes)
{
    static 整数	    prev_mode = -1;
    整数		    mode = 0;

    もし (modes != 0x0)
	mode = modes;
    否则
	mode = get_menu_mode_flag();

    もし (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# もし defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    整数
gui_is_menu_shortcut(整数 key)
{
    vimmenu_T	*menu;

    もし (key < 256)
	key = TOLOWER_LOC(key);
    FOR_ALL_MENUS(menu)
	もし (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == key))
	    戻る TRUE;
    戻る FALSE;
}
# endif
#endif // FEAT_GUI

#もし defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    void
gui_mch_toggle_tearoffs(整数 enable)
{
    整数		pri_tab[MENUDEPTH + 1];
    整数		i;

    もし (enable)
    {
	ために (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    否则
	gui_destroy_tearoffs_recurse(root_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    static void
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    const char_u	*pname,
    整数			*pri_tab,
    整数			pri_idx)
{
    char_u	*newpname = NULL;
    整数		len;
    char_u	*s;
    char_u	*d;

    もし (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    ながら (menu != NULL)
    {
	もし (menu->children != NULL && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (整数)STRLEN(pname) + (整数)STRLEN(menu->name);
	    ために (s = menu->name; *s; ++s)
		もし (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    もし (newpname != NULL)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		ために (s = menu->name; *s; ++s)
		{
		    もし (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		もし (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    static void
gui_add_tearoff(char_u *tearpath, 整数 *pri_tab, 整数 pri_idx)
{
    char_u	*tbuf;
    整数		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned 整数)STRLEN(tearpath));
    もし (tbuf == NULL)
	戻る;

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
    menuarg.iconfile = NULL;
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
    static void
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    ながら (menu)
    {
	もし (menu->children)
	{
	    // check if tearoff exists
	    もし (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    もし (menu->children != NULL) // if not the last one
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
    void
execute_menu(exarg_T *eap, vimmenu_T *menu, 整数 mode_idx)
{
    整数		idx = mode_idx;

    もし (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	もし (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_INSERT;
	}
#ifdef FEAT_TERMINAL
	否则 もし (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	否则 もし (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	否则 もし (eap != NULL && eap->addr_count)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    もし ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    否则
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
	    もし (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    もし (idx == MENU_INDEX_INVALID || eap == NULL)
	idx = MENU_INDEX_NORMAL;

    もし (menu->strings[idx] != NULL && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	もし (eap == NULL || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    もし (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	否则
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    否则 もし (eap != NULL)
    {
	char_u	*mode;

	switch (idx)
	{
	    case MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		break;
	    case MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		break;
	    case MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		break;
	    case MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		break;
	    case MENU_INDEX_INSERT:
		mode = (char_u *)"Insert";
		break;
	    case MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		break;
	    // case MENU_INDEX_TIP: cannot happen
	    default:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    static vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    整数		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    もし (saved_name == NULL)
	戻る NULL;

    menu = *get_root_menu(saved_name);
    name = saved_name;
    ながら (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	ながら (menu != NULL)
	{
	    もし (menu_name_equal(name, menu))
	    {
		もし (*p == NUL && menu->children != NULL)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = NULL;
		}
		否则 もし (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		}
		break;
	    }
	    menu = menu->next;
	}
	もし (menu == NULL || *p == NUL)
	    break;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    もし (menu == NULL)
    {
	もし (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	戻る NULL;
    }

    戻る menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    void
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    整数		mode_idx = MENU_INDEX_INVALID;

    もし (arg[0] && VIM_ISWHITE(arg[1]))
    {
	switch (arg[0])
	{
	    case 'n': mode_idx = MENU_INDEX_NORMAL; break;
	    case 'v': mode_idx = MENU_INDEX_VISUAL; break;
	    case 's': mode_idx = MENU_INDEX_SELECT; break;
	    case 'o': mode_idx = MENU_INDEX_OP_PENDING; break;
	    case 't': mode_idx = MENU_INDEX_TERMINAL; break;
	    case 'i': mode_idx = MENU_INDEX_INSERT; break;
	    case 'c': mode_idx = MENU_INDEX_CMDLINE; break;
	    default: semsg(_(e_invalid_argument_str), arg);
		     戻る;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    もし (menu == NULL)
	戻る;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    void
winbar_click(win_T *wp, 整数 col)
{
    整数		idx;

    もし (wp->w_winbar_items == NULL)
	戻る;
    ために (idx = 0; wp->w_winbar_items[idx].wb_menu != NULL; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_items[idx];

	もし (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = NULL;
	    pos_T   save_visual = VIsual;
	    整数	    save_visual_active = VIsual_active;
	    整数	    save_visual_select = VIsual_select;
	    整数	    save_visual_reselect = VIsual_reselect;
	    整数	    save_visual_mode = VIsual_mode;

	    もし (wp != curwin)
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
	    execute_menu(NULL, item->wb_menu, -1);

	    もし (save_curwin != NULL && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    もし (!win_valid(wp))
		break;
	}
    }
}

#もし defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = NULL;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_root_menu(path_name);

    saved_name = vim_strsave(path_name);
    もし (saved_name == NULL)
	戻る NULL;

    name = saved_name;
    ながら (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	ながら (menu != NULL)
	{
	    もし (menu_name_equal(name, menu))
	    {
		もし (menu->children == NULL)
		{
		    // found a menu item instead of a sub-menu
		    もし (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    否则
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		    goto theend;
		}
		もし (*p == NUL)	    // found a full match
		    goto theend;
		break;
	    }
	    menu = menu->next;
	}
	もし (menu == NULL)	// didn't find it
	    break;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    もし (menu == NULL)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    戻る menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

typedef struct
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

static garray_T menutrans_ga = {0, 0, 0, 0, NULL};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    void
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    整数			i;
    char_u		*from, *from_noamp, *to;

    もし (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, sizeof(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    もし (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	ために (i = 0; i < menutrans_ga.ga_len; ++i)
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
    否则
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	もし (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	否则
	{
	    もし (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		もし (from != NULL)
		{
		    from_noamp = menu_text(from, NULL, NULL);
		    to = vim_strnsave(to, arg - to);
		    もし (from_noamp != NULL && to != NULL)
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
		    否则
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

#もし defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    static char_u *
menu_skip_part(char_u *p)
{
    ながら (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	もし ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    戻る p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    static char_u *
menutrans_lookup(char_u *name, 整数 len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    整数			i;
    char_u		*dname;

    ために (i = 0; i < menutrans_ga.ga_len; ++i)
	もし (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    戻る tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, NULL, NULL);
    name[len] = i;
    もし (dname == NULL)
	戻る NULL;

    ために (i = 0; i < menutrans_ga.ga_len; ++i)
	もし (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    戻る tp[i].to;
	}
    vim_free(dname);

    戻る NULL;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    static void
menu_unescape_name(char_u *name)
{
    char_u  *p;

    ために (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	もし (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    static char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    ながら (*arg && !VIM_ISWHITE(*arg))
    {
	もし ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	否则 もし (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    もし (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    戻る arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    static 整数
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, 整数 modes, dict_T *dict)
{
    整数		status;
    list_T	*l;

    もし (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	もし (l == NULL)
	    戻る FAIL;

	もし (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    戻る FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	ために (topmenu = menu; topmenu != NULL; topmenu = topmenu->next)
	    もし (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	戻る OK;
    }

    もし (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	戻る OK;

    status = dict_add_string(dict, "name", menu->name);
    もし (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    もし (status == OK && menu->actext != NULL)
	status = dict_add_string(dict, "accel", menu->actext);
    もし (status == OK)
	status = dict_add_number(dict, "priority", menu->priority);
    もし (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    もし (status == OK && menu->iconfile != NULL)
	status = dict_add_string(dict, "icon", menu->iconfile);
    もし (status == OK && menu->iconidx >= 0)
	status = dict_add_number(dict, "iconidx", menu->iconidx);
#endif
    もし (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	整数	buflen;

	もし (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	否则
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    もし (status == OK && menu->children == NULL)
    {
	整数		bit;

	// Get the first mode in which the menu is available
	ために (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	もし (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    もし (menu->strings[bit] != NULL)
	    {
		もし (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		否则
		{
		    char_u *tofree = NULL;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    もし (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    もし (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    もし (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    もし (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    もし (status == OK && menu->children != NULL)
    {
	vimmenu_T	*child;

	l = list_alloc();
	もし (l == NULL)
	    戻る FAIL;

	もし (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    戻る FAIL;
	}
	child = menu->children;
	ながら (child)
	{
	    もし (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    戻る status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    void
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    整数		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;
    retdict = rettv->vval.v_dict;

    もし (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	戻る;

    menu_name = tv_get_string_chk(&argvars[0]);
    もし (menu_name == NULL)
	戻る;

    // menu mode
    もし (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    否则
	which = (char_u *)"";	    // Default is modes for "menu"
    もし (which == NULL)
	戻る;

    modes = get_menu_cmd_modes(which, *which == '!', NULL, NULL);

    // Locate the specified menu or menu item
    menu = *get_root_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    もし (saved_name == NULL)
	戻る;
    もし (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	ながら (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    ながら (menu != NULL)
	    {
		もし (menu_name_equal(name, menu))
		    break;
		menu = menu->next;
	    }
	    もし (menu == NULL || *p == NUL)
		break;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    もし (menu == NULL)		// specified menu not found
	戻る;

    もし (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
