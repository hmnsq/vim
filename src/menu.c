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

#если defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
static целое add_menu_path(char_u *, vimmenu_T *, целое *, char_u *, целое);
#иначе
static целое add_menu_path(char_u *, vimmenu_T *, целое *, char_u *);
#endif
static целое menu_nable_recurse(vimmenu_T *menu, char_u *name, целое modes, целое enable);
static целое remove_menu(vimmenu_T **, char_u *, целое, целое silent);
static void free_menu(vimmenu_T **menup);
static void free_menu_string(vimmenu_T *, целое);
static целое show_menus(char_u *, целое);
static void show_menus_recursive(vimmenu_T *, целое, целое);
static char_u *menu_name_skip(char_u *name);
static целое menu_name_equal(char_u *name, vimmenu_T *menu);
static целое menu_namecmp(char_u *name, char_u *mname);
static целое get_menu_cmd_modes(char_u *, целое, целое *, целое *);
static char_u *popup_mode_name(char_u *name, целое idx);
static char_u *menu_text(char_u *text, целое *mnemonic, char_u **actext);

#если defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
static void gui_create_tearoffs_recurse(vimmenu_T *menu, const char_u *pname, целое *pri_tab, целое pri_idx);
static void gui_add_tearoff(char_u *tearpath, целое *pri_tab, целое pri_idx);
static void gui_destroy_tearoffs_recurse(vimmenu_T *menu);
static целое s_tearoffs = FALSE;
#endif

static целое menu_is_hidden(char_u *name);
static целое menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
static целое menus_locked = 0;

#если defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
static char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
static char_u *menutrans_lookup(char_u *name, целое len);
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
    static целое
menu_is_winbar(char_u *name)
{
    вернуть (STRNCMP(name, "WinBar", 6) == 0);
}

    целое
winbar_height(win_T *wp)
{
    если (wp->w_winbar != NULL && wp->w_winbar->children != NULL)
	вернуть 1;
    вернуть 0;
}

    static vimmenu_T **
get_root_menu(char_u *name)
{
    если (menu_is_winbar(name))
	вернуть &curwin->w_winbar;
    вернуть &root_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static целое
is_menus_locked(void)
{
    если (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	вернуть TRUE;
    }
    вернуть FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    void
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    целое		modes;
    char_u	*map_to;
    целое		noremap;
    целое		silent = FALSE;
    целое		special = FALSE;
    целое		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    целое		i;
#если defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    целое		old_menu_height;
# если defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    целое		old_toolbar_height;
# endif
#endif
    целое		pri_tab[MENUDEPTH + 1];
    целое		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = NULL;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**root_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    для (;;)
    {
	если (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	если (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	если (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    continue;
	}
	break;
    }


    // Locate an optional "icon=filename" argument.
    если (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	пока (*arg != NUL && *arg != ' ')
	{
	    если (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	если (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    для (p = arg; *p; ++p)
	если (!VIM_ISDIGIT(*p) && *p != '.')
	    break;
    если (VIM_ISWHITE(*p))
    {
	для (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    если (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    если (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    иначе если (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    иначе
	i = 0;
    пока (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    если (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    иначе если (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    если (*arg == NUL)
    {
	show_menus(arg, modes);
	вернуть;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    если (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	если (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    если (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		если (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((char *)menu_path + 7);
		    если (menuarg.iconidx >= (целое)TOOLBAR_NAME_COUNT)
			menuarg.iconidx = -1;
		    иначе
			menuarg.icon_builtin = TRUE;
		}
	    }
	    иначе
	    {
		для (i = 0; i < (целое)TOOLBAR_NAME_COUNT; ++i)
		    если (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
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
    если (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	goto theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    если (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	goto theend;
    }
    иначе если (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	goto theend;
    }
#если defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# если defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    root_menu_ptr = get_root_menu(menu_path);
    если (root_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    если (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	если (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	если (menu_is_popup(menu_path))
	{
	    для (i = 0; i < MENU_INDEX_TIP; ++i)
		если (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    если (p != NULL)
		    {
			menu_nable_recurse(*root_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*root_menu_ptr, menu_path, modes, enable);
    }
    иначе если (unmenu)
    {
	если (is_menus_locked())
	    goto theend;

	/*
	 * Delete menu(s).
	 */
	если (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	если (menu_is_popup(menu_path))
	{
	    для (i = 0; i < MENU_INDEX_TIP; ++i)
		если (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    если (p != NULL)
		    {
			remove_menu(root_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(root_menu_ptr, menu_path, modes, FALSE);
    }
    иначе
    {
	если (is_menus_locked())
	    goto theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	если (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = NULL;
	}
	иначе если (modes & MENU_TIP_MODE)
	    map_buf = NULL;	// Menu tips are plain text.
	иначе
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
	если (menu_is_popup(menu_path))
	{
	    для (i = 0; i < MENU_INDEX_TIP; ++i)
		если (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    если (p != NULL)
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

#если defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    если (gui.in_use
	    && (gui.menu_height != old_menu_height
# если defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    если (root_menu_ptr == &curwin->w_winbar)
    {
	целое h = winbar_height(curwin);

	если (h != curwin->w_winbar_height)
	{
	    если (h == 0)
		++curwin->w_height;
	    иначе если (curwin->w_height > 0)
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
    static целое
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    целое		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , целое	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    целое		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = NULL;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    целое		i;
    целое		c;
    целое		d;
#ifdef FEAT_GUI
    целое		idx;
    целое		new_idx;
#endif
    целое		pri_idx = 0;
    целое		old_modes = 0;
    целое		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = NULL;
#endif
    vimmenu_T	**root_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    если (path_name == NULL)
	вернуть FAIL;
    root_menu_ptr = get_root_menu(menu_path);
    menup = root_menu_ptr;
    parent = NULL;
    name = path_name;
    пока (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (целое)STRLEN(name));
	если (map_to != NULL)
	{
	    en_name = name;
	    name = map_to;
	}
	иначе
	    en_name = NULL;
#endif
	dname = menu_text(name, NULL, NULL);
	если (dname == NULL)
	    goto erret;
	если (*dname == NUL)
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
	пока (menu != NULL)
	{
	    если (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		если (*next_name == NUL && menu->children != NULL)
		{
		    если (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    goto erret;
		}
		если (*next_name != NUL && menu->children == NULL
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    если (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    goto erret;
		}
		break;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    если (parent != NULL || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		если (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	если (menu == NULL)
	{
	    если (*next_name == NUL && parent == NULL)
	    {
		emsg(_(e_must_not_add_menu_items_directly_to_menu_bar));
		goto erret;
	    }

	    если (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		goto erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    если (menu == NULL)
		goto erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    если (en_name != NULL)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, NULL, NULL);
	    }
	    иначе
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
	    если (*next_name == NUL && menuarg->iconfile != NULL)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#если defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    если (parent != NULL && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	иначе
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    если (addtearoff)
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
	если ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    если (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		если (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		иначе
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# если defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    если (	addtearoff
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
		если (tearpath != NULL)
		{
		    char_u  *s;
		    целое	    len;

		    STRCPY(tearpath, menu_path);
		    len = (целое)(next_name - path_name - 1);
		    для (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			если ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	если (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    если (sys_menu)
	modes &= ~old_modes;

    если (menu != NULL && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == NULL) ? NULL : vim_strsave(call_data);

	// loop over all modes, may add more than one
	для (i = 0; i < MENU_MODES; ++i)
	{
	    если (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		если (amenu && call_data != NULL && *call_data != NUL
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

		если (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    если (menu->strings[i] != NULL)
		    {
			menu->strings[i][0] = c;
			если (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			иначе
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			если (c == Ctrl_C)
			{
			    целое	    len = (целое)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		иначе
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#если defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	если (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    вернуть OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    пока (parent != NULL && parent->children == NULL)
    {
	если (parent->parent == NULL)
	    menup = root_menu_ptr;
	иначе
	    menup = &parent->parent->children;
	для ( ; *menup != NULL && *menup != parent; menup = &((*menup)->next))
	    ;
	если (*menup == NULL) // safety check
	    break;
	parent = parent->parent;
	free_menu(menup);
    }
    вернуть FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    static целое
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    целое		modes,
    целое		enable)
{
    char_u	*p;

    если (menu == NULL)
	вернуть OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    пока (menu != NULL)
    {
	если (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    если (*p != NUL)
	    {
		если (menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    вернуть FAIL;
		}
		если (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    вернуть FAIL;
	    }
	    иначе
		если (enable)
		    menu->enabled |= modes;
		иначе
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    если (*name != NUL && *name != '*')
		break;
	}
	menu = menu->next;
    }
    если (*name != NUL && *name != '*' && menu == NULL)
    {
	semsg(_(e_no_menu_str), name);
	вернуть FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    вернуть OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    static целое
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    целое		modes,
    целое		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    если (*menup == NULL)
	вернуть OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    пока ((menu = *menup) != NULL)
    {
	если (*name == NUL || menu_name_equal(name, menu))
	{
	    если (*p != NUL && menu->children == NULL)
	    {
		если (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		вернуть FAIL;
	    }
	    если ((menu->modes & modes) != 0x0)
	    {
#если defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		если (*p == NUL && modes == MENU_ALL_MODES)
		{
		    если (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		если (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    вернуть FAIL;
	    }
	    иначе если (*name != NUL)
	    {
		если (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		вернуть FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    если (*name != NUL)
		break;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    если (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    если ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    иначе
		menup = &menu->next;
	}
	иначе
	    menup = &menu->next;
    }
    если (*name != NUL)
    {
	если (menu == NULL)
	{
	    если (!silent)
		semsg(_(e_no_menu_str), name);
	    вернуть FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#если defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	если ((s_tearoffs) && (menu->children != NULL)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	иначе
#endif
	    child = menu->children;
	для ( ; child != NULL; child = child->next)
	    menu->modes |= child->modes;
	если (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#если defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    если (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	если ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#если defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    если (s_tearoffs && menu->children != NULL) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    вернуть OK;
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
    целое		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    если (gui.in_use)
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
    для (i = 0; i < MENU_MODES; i++)
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
free_menu_string(vimmenu_T *menu, целое idx)
{
    целое		count = 0;
    целое		i;

    для (i = 0; i < MENU_MODES; i++)
	если (menu->strings[i] == menu->strings[idx])
	    count++;
    если (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = NULL;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    static целое
show_menus(char_u *path_name, целое modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = NULL;

    name = path_name = vim_strsave(path_name);
    если (path_name == NULL)
	вернуть FAIL;
    menu = *get_root_menu(path_name);

    // First, find the (sub)menu with the given name
    пока (*name)
    {
	p = menu_name_skip(name);
	пока (menu != NULL)
	{
	    если (menu_name_equal(name, menu))
	    {
		// Found menu
		если (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    вернуть FAIL;
		}
		иначе если ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    вернуть FAIL;
		}
		break;
	    }
	    menu = menu->next;
	}
	если (menu == NULL)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    вернуть FAIL;
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
    вернуть OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    static void
show_menus_recursive(vimmenu_T *menu, целое modes, целое depth)
{
    целое		i;
    целое		bit;

    если (menu != NULL && (menu->modes & modes) == 0x0)
	вернуть;

    если (menu != NULL)
    {
	msg_putchar('\n');
	если (got_int)		// "q" hit for "--more--"
	    вернуть;
	для (i = 0; i < depth; i++)
	    msg_puts("  ");
	если (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    msg_puts(" ");
	}
				// Same highlighting as for directories!?
	msg_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    если (menu != NULL && menu->children == NULL)
    {
	для (bit = 0; bit < MENU_MODES; bit++)
	    если ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		если (got_int)		// "q" hit for "--more--"
		    вернуть;
		для (i = 0; i < depth + 2; i++)
		    msg_puts("  ");
		msg_puts(menu_mode_chars[bit]);
		если (menu->noremap[bit] == REMAP_NONE)
		    msg_putchar('*');
		иначе если (menu->noremap[bit] == REMAP_SCRIPT)
		    msg_putchar('&');
		иначе
		    msg_putchar(' ');
		если (menu->silent[bit])
		    msg_putchar('s');
		иначе
		    msg_putchar(' ');
		если ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    msg_putchar('-');
		иначе
		    msg_putchar(' ');
		msg_puts(" ");
		если (*menu->strings[bit] == NUL)
		    msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
		иначе
		    msg_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    иначе
    {
	если (menu == NULL)
	{
	    menu = root_menu;
	    depth--;
	}
	иначе
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	для (; menu != NULL && !got_int; menu = menu->next)
	    если (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
static vimmenu_T	*expand_menu = NULL;
static vimmenu_T	*expand_menu_alt = NULL;
static целое		expand_modes = 0x0;
static целое		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    целое		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = NULL;
    char_u	*name;
    целое		unmenu;
    vimmenu_T	*menu;
    целое		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    для (p = arg; *p; ++p)
	если (!VIM_ISDIGIT(*p) && *p != '.')
	    break;

    если (!VIM_ISWHITE(*p))
    {
	если (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	иначе если (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	иначе
	    p = arg;
    }

    пока (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    для (; *p && !VIM_ISWHITE(*p); ++p)
    {
	если ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	иначе если (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    если (expand_menus && VIM_ISWHITE(*p))
	вернуть NULL;	// TODO: check for next command?
    если (*p == NUL)		// Complete the menu name
    {
	целое try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, NULL, &unmenu);
	если (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	если (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    если (path_name == NULL)
		вернуть NULL;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	пока (name != NULL && *name)
	{
	    p = menu_name_skip(name);
	    пока (menu != NULL)
	    {
		если (menu_name_equal(name, menu))
		{
		    // Found menu
		    если ((*p != NUL && menu->children == NULL)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			вернуть NULL;
		    }
		    break;
		}
		menu = menu->next;
		если (menu == NULL && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    если (menu == NULL)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		вернуть NULL;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	если (expand_menu == root_menu)
	    expand_menu_alt = curwin->w_winbar;
	иначе
	    expand_menu_alt = NULL;
    }
    иначе			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    вернуть NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, целое idx)
{
    static vimmenu_T	*menu = NULL;
    static целое		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  целое		should_advance = FALSE;
#endif

    если (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    пока (menu != NULL && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == NULL))
    {
	menu = menu->next;
	если (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    если (menu == NULL)	    // at end of linked list
	вернуть NULL;

    если (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	если (should_advance)
	    str = menu->en_dname;
	иначе
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    если (menu->en_dname == NULL)
		should_advance = TRUE;
	}
#endif
    иначе
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    если (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	если (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    вернуть str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, целое idx)
{
    static vimmenu_T	*menu = NULL;
    static целое		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    static char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  целое		should_advance = FALSE;
#endif

    если (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    пока (menu != NULL
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	если (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    если (menu == NULL)	    // at end of linked list
	вернуть NULL;

    если (menu->modes & expand_modes)
    {
	если (menu->children != NULL)
	{
#ifdef FEAT_MULTI_LANG
	    если (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    иначе
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		если (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	иначе
#ifdef FEAT_MULTI_LANG
	{
	    если (should_advance)
		str = menu->en_dname;
	    иначе
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		если (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    иначе
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    если (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	если (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    вернуть str;
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

    для (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	если (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    если (*p == NUL)
		break;
	}
    }
    если (*p)
	*p++ = NUL;
    вернуть p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    static целое
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    если (menu->en_name != NULL
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	вернуть TRUE;
#endif
    вернуть menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    static целое
menu_namecmp(char_u *name, char_u *mname)
{
    целое		i;

    для (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	если (name[i] != mname[i])
	    break;
    вернуть ((name[i] == NUL || name[i] == TAB)
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
    static целое
get_menu_cmd_modes(
    char_u  *cmd,
    целое	    forceit,	    // Was there a "!" after the command?
    целое	    *noremap,
    целое	    *unmenu)
{
    целое	    modes;

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
	    если (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
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
	    если (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		break;
	    }
	    // FALLTHROUGH
	default:
	    --cmd;
	    если (forceit)		// menu!!
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    иначе			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    если (noremap != NULL)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    если (unmenu != NULL)
	*unmenu = (*cmd == 'u');
    вернуть modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    static char_u *
get_menu_mode_str(целое modes)
{
    если ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	вернуть (char_u *)"a";
    если ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	вернуть (char_u *)" ";
    если ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	вернуть (char_u *)"!";
    если ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	вернуть (char_u *)"v";
    если (modes & MENU_VISUAL_MODE)
	вернуть (char_u *)"x";
    если (modes & MENU_SELECT_MODE)
	вернуть (char_u *)"s";
    если (modes & MENU_OP_PENDING_MODE)
	вернуть (char_u *)"o";
    если (modes & MENU_INSERT_MODE)
	вернуть (char_u *)"i";
    если (modes & MENU_TERMINAL_MODE)
	вернуть (char_u *)"tl";
    если (modes & MENU_CMDLINE_MODE)
	вернуть (char_u *)"c";
    если (modes & MENU_NORMAL_MODE)
	вернуть (char_u *)"n";
    если (modes & MENU_TIP_MODE)
	вернуть (char_u *)"t";

    вернуть (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    static char_u *
popup_mode_name(char_u *name, целое idx)
{
    char_u	*p;
    целое		len = (целое)STRLEN(name);
    char	*mode_chars = menu_mode_chars[idx];
    целое		mode_chars_len = (целое)strlen(mode_chars);
    целое		i;

    p = vim_strnsave(name, len + mode_chars_len);
    если (p == NULL)
	вернуть NULL;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    для (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    вернуть p;
}

#если defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    целое
get_menu_index(vimmenu_T *menu, целое state)
{
    целое		idx;

    если ((state & MODE_INSERT))
	idx = MENU_INDEX_INSERT;
    иначе если (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    иначе если (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    иначе если (VIsual_active)
    {
	если (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	иначе
	    idx = MENU_INDEX_VISUAL;
    }
    иначе если (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    иначе если (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    иначе если ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    иначе
	idx = MENU_INDEX_INVALID;

    если (idx != MENU_INDEX_INVALID && menu->strings[idx] == NULL)
	idx = MENU_INDEX_INVALID;
    вернуть idx;
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
menu_text(char_u *str, целое *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    если (p != NULL)
    {
	если (actext != NULL)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    иначе
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    для (p = text; p != NULL; )
    {
	p = vim_strchr(p, '&');
	если (p != NULL)
	{
	    если (p[1] == NUL)	    // trailing "&"
		break;
	    если (mnemonic != NULL && p[1] != '&')
#если !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#иначе
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
    вернуть text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    целое
menu_is_menubar(char_u *name)
{
    вернуть (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    целое
menu_is_popup(char_u *name)
{
    вернуть (STRNCMP(name, "PopUp", 5) == 0);
}

#если defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    целое
menu_is_child_of_popup(vimmenu_T *menu)
{
    пока (menu->parent != NULL)
	menu = menu->parent;
    вернуть menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    целое
menu_is_toolbar(char_u *name)
{
    вернуть (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    целое
menu_is_separator(char_u *name)
{
    вернуть (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    static целое
menu_is_hidden(char_u *name)
{
    вернуть (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    static целое
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    вернуть (STRCMP(name, TEAR_STRING) == 0);
#иначе
    вернуть FALSE;
#endif
}

#если defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    static целое
get_menu_mode(void)
{
# ifdef FEAT_TERMINAL
    если (term_use_loop())
	вернуть MENU_INDEX_TERMINAL;
# endif
    если (VIsual_active)
    {
	если (VIsual_select)
	    вернуть MENU_INDEX_SELECT;
	вернуть MENU_INDEX_VISUAL;
    }
    если (State & MODE_INSERT)
	вернуть MENU_INDEX_INSERT;
    если ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	вернуть MENU_INDEX_CMDLINE;
    если (finish_op)
	вернуть MENU_INDEX_OP_PENDING;
    если (State & MODE_NORMAL)
	вернуть MENU_INDEX_NORMAL;
    если (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	вернуть MENU_INDEX_INSERT;
    вернуть MENU_INDEX_INVALID;
}

    целое
get_menu_mode_flag(void)
{
    целое mode = get_menu_mode();

    если (mode == MENU_INDEX_INVALID)
	вернуть 0;
    вернуть 1 << mode;
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
    целое		menu_mode;
    char*	mode;
    целое		mode_len;

    menu_mode = get_menu_mode();
    если (menu_mode == MENU_INDEX_INVALID)
	вернуть;
    mode = menu_mode_chars[menu_mode];
    mode_len = (целое)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, NULL, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	если (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    break;

    // Only show a popup when it is defined and has entries
    если (menu == NULL || menu->children == NULL)
	вернуть;

# если defined(FEAT_GUI)
    если (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# если defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    иначе
# endif
# если defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#если defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    целое
check_menu_pointer(vimmenu_T *root, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    для (p = root; p != NULL; p = p->next)
	если (p == menu_to_check
		|| (p->children != NULL
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    вернуть OK;
    вернуть FAIL;
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
    целое		idx = 0;

    пока (menu != NULL)
    {
	// Don't add a menu when only a tip was defined.
	если (menu->modes & MENU_ALL_MODES)
	{
	    если (menu->children != NULL)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    иначе
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
gui_update_menus_recurse(vimmenu_T *menu, целое mode)
{
    целое		grey;

    пока (menu)
    {
	если ((menu->modes & menu->enabled & mode)
# если defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	иначе
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	если (vim_strchr(p_go, GO_GREY) != NULL || menu->parent == NULL
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	иначе
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
gui_update_menus(целое modes)
{
    static целое	    prev_mode = -1;
    целое		    mode = 0;

    если (modes != 0x0)
	mode = modes;
    иначе
	mode = get_menu_mode_flag();

    если (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# если defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    целое
gui_is_menu_shortcut(целое key)
{
    vimmenu_T	*menu;

    если (key < 256)
	key = TOLOWER_LOC(key);
    FOR_ALL_MENUS(menu)
	если (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == key))
	    вернуть TRUE;
    вернуть FALSE;
}
# endif
#endif // FEAT_GUI

#если defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    void
gui_mch_toggle_tearoffs(целое enable)
{
    целое		pri_tab[MENUDEPTH + 1];
    целое		i;

    если (enable)
    {
	для (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    иначе
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
    целое			*pri_tab,
    целое			pri_idx)
{
    char_u	*newpname = NULL;
    целое		len;
    char_u	*s;
    char_u	*d;

    если (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    пока (menu != NULL)
    {
	если (menu->children != NULL && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (целое)STRLEN(pname) + (целое)STRLEN(menu->name);
	    для (s = menu->name; *s; ++s)
		если (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    если (newpname != NULL)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		для (s = menu->name; *s; ++s)
		{
		    если (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		если (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
gui_add_tearoff(char_u *tearpath, целое *pri_tab, целое pri_idx)
{
    char_u	*tbuf;
    целое		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned целое)STRLEN(tearpath));
    если (tbuf == NULL)
	вернуть;

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
    пока (menu)
    {
	если (menu->children)
	{
	    // check if tearoff exists
	    если (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    если (menu->children != NULL) // if not the last one
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
execute_menu(exarg_T *eap, vimmenu_T *menu, целое mode_idx)
{
    целое		idx = mode_idx;

    если (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	если (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_INSERT;
	}
#ifdef FEAT_TERMINAL
	иначе если (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	иначе если (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	иначе если (eap != NULL && eap->addr_count)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    если ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    иначе
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
	    если (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    если (idx == MENU_INDEX_INVALID || eap == NULL)
	idx = MENU_INDEX_NORMAL;

    если (menu->strings[idx] != NULL && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	если (eap == NULL || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    если (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	иначе
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    иначе если (eap != NULL)
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
    целое		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    если (saved_name == NULL)
	вернуть NULL;

    menu = *get_root_menu(saved_name);
    name = saved_name;
    пока (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	пока (menu != NULL)
	{
	    если (menu_name_equal(name, menu))
	    {
		если (*p == NUL && menu->children != NULL)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = NULL;
		}
		иначе если (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		}
		break;
	    }
	    menu = menu->next;
	}
	если (menu == NULL || *p == NUL)
	    break;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    если (menu == NULL)
    {
	если (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	вернуть NULL;
    }

    вернуть menu;
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
    целое		mode_idx = MENU_INDEX_INVALID;

    если (arg[0] && VIM_ISWHITE(arg[1]))
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
		     вернуть;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    если (menu == NULL)
	вернуть;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    void
winbar_click(win_T *wp, целое col)
{
    целое		idx;

    если (wp->w_winbar_items == NULL)
	вернуть;
    для (idx = 0; wp->w_winbar_items[idx].wb_menu != NULL; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_items[idx];

	если (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = NULL;
	    pos_T   save_visual = VIsual;
	    целое	    save_visual_active = VIsual_active;
	    целое	    save_visual_select = VIsual_select;
	    целое	    save_visual_reselect = VIsual_reselect;
	    целое	    save_visual_mode = VIsual_mode;

	    если (wp != curwin)
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

	    если (save_curwin != NULL && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    если (!win_valid(wp))
		break;
	}
    }
}

#если defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
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
    если (saved_name == NULL)
	вернуть NULL;

    name = saved_name;
    пока (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	пока (menu != NULL)
	{
	    если (menu_name_equal(name, menu))
	    {
		если (menu->children == NULL)
		{
		    // found a menu item instead of a sub-menu
		    если (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    иначе
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		    goto theend;
		}
		если (*p == NUL)	    // found a full match
		    goto theend;
		break;
	    }
	    menu = menu->next;
	}
	если (menu == NULL)	// didn't find it
	    break;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    если (menu == NULL)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    вернуть menu;
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
    целое			i;
    char_u		*from, *from_noamp, *to;

    если (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, sizeof(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    если (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	для (i = 0; i < menutrans_ga.ga_len; ++i)
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
    иначе
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	если (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	иначе
	{
	    если (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		если (from != NULL)
		{
		    from_noamp = menu_text(from, NULL, NULL);
		    to = vim_strnsave(to, arg - to);
		    если (from_noamp != NULL && to != NULL)
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
		    иначе
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

#если defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    static char_u *
menu_skip_part(char_u *p)
{
    пока (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	если ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    вернуть p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    static char_u *
menutrans_lookup(char_u *name, целое len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    целое			i;
    char_u		*dname;

    для (i = 0; i < menutrans_ga.ga_len; ++i)
	если (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    вернуть tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, NULL, NULL);
    name[len] = i;
    если (dname == NULL)
	вернуть NULL;

    для (i = 0; i < menutrans_ga.ga_len; ++i)
	если (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    вернуть tp[i].to;
	}
    vim_free(dname);

    вернуть NULL;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    static void
menu_unescape_name(char_u *name)
{
    char_u  *p;

    для (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	если (*p == '\\')
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

    пока (*arg && !VIM_ISWHITE(*arg))
    {
	если ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	иначе если (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    если (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    вернуть arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    static целое
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, целое modes, dict_T *dict)
{
    целое		status;
    list_T	*l;

    если (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	если (l == NULL)
	    вернуть FAIL;

	если (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    вернуть FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	для (topmenu = menu; topmenu != NULL; topmenu = topmenu->next)
	    если (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	вернуть OK;
    }

    если (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	вернуть OK;

    status = dict_add_string(dict, "name", menu->name);
    если (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    если (status == OK && menu->actext != NULL)
	status = dict_add_string(dict, "accel", menu->actext);
    если (status == OK)
	status = dict_add_number(dict, "priority", menu->priority);
    если (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    если (status == OK && menu->iconfile != NULL)
	status = dict_add_string(dict, "icon", menu->iconfile);
    если (status == OK && menu->iconidx >= 0)
	status = dict_add_number(dict, "iconidx", menu->iconidx);
#endif
    если (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	целое	buflen;

	если (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	иначе
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    если (status == OK && menu->children == NULL)
    {
	целое		bit;

	// Get the first mode in which the menu is available
	для (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	если (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    если (menu->strings[bit] != NULL)
	    {
		если (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		иначе
		{
		    char_u *tofree = NULL;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    если (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    если (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    если (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    если (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    если (status == OK && menu->children != NULL)
    {
	vimmenu_T	*child;

	l = list_alloc();
	если (l == NULL)
	    вернуть FAIL;

	если (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    вернуть FAIL;
	}
	child = menu->children;
	пока (child)
	{
	    если (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    вернуть status;
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
    целое		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    если (rettv_dict_alloc(rettv) == FAIL)
	вернуть;
    retdict = rettv->vval.v_dict;

    если (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	вернуть;

    menu_name = tv_get_string_chk(&argvars[0]);
    если (menu_name == NULL)
	вернуть;

    // menu mode
    если (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    иначе
	which = (char_u *)"";	    // Default is modes for "menu"
    если (which == NULL)
	вернуть;

    modes = get_menu_cmd_modes(which, *which == '!', NULL, NULL);

    // Locate the specified menu or menu item
    menu = *get_root_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    если (saved_name == NULL)
	вернуть;
    если (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	пока (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    пока (menu != NULL)
	    {
		если (menu_name_equal(name, menu))
		    break;
		menu = menu->next;
	    }
	    если (menu == NULL || *p == NUL)
		break;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    если (menu == NULL)		// specified menu not found
	вернуть;

    если (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
