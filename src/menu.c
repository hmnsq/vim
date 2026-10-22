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

#만약 defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
static 정수 add_menu_path(char_u *, vimmenu_T *, 정수 *, char_u *, 정수);
#아니면
static 정수 add_menu_path(char_u *, vimmenu_T *, 정수 *, char_u *);
#endif
static 정수 menu_nable_recurse(vimmenu_T *menu, char_u *name, 정수 modes, 정수 enable);
static 정수 remove_menu(vimmenu_T **, char_u *, 정수, 정수 silent);
static void free_menu(vimmenu_T **menup);
static void free_menu_string(vimmenu_T *, 정수);
static 정수 show_menus(char_u *, 정수);
static void show_menus_recursive(vimmenu_T *, 정수, 정수);
static char_u *menu_name_skip(char_u *name);
static 정수 menu_name_equal(char_u *name, vimmenu_T *menu);
static 정수 menu_namecmp(char_u *name, char_u *mname);
static 정수 get_menu_cmd_modes(char_u *, 정수, 정수 *, 정수 *);
static char_u *popup_mode_name(char_u *name, 정수 idx);
static char_u *menu_text(char_u *text, 정수 *mnemonic, char_u **actext);

#만약 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
static void gui_create_tearoffs_recurse(vimmenu_T *menu, const char_u *pname, 정수 *pri_tab, 정수 pri_idx);
static void gui_add_tearoff(char_u *tearpath, 정수 *pri_tab, 정수 pri_idx);
static void gui_destroy_tearoffs_recurse(vimmenu_T *menu);
static 정수 s_tearoffs = FALSE;
#endif

static 정수 menu_is_hidden(char_u *name);
static 정수 menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
static 정수 menus_locked = 0;

#만약 defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
static char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
static char_u *menutrans_lookup(char_u *name, 정수 len);
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
    static 정수
menu_is_winbar(char_u *name)
{
    반환 (STRNCMP(name, "WinBar", 6) == 0);
}

    정수
winbar_height(win_T *wp)
{
    만약 (wp->w_winbar != NULL && wp->w_winbar->children != NULL)
	반환 1;
    반환 0;
}

    static vimmenu_T **
get_root_menu(char_u *name)
{
    만약 (menu_is_winbar(name))
	반환 &curwin->w_winbar;
    반환 &root_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static 정수
is_menus_locked(void)
{
    만약 (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    void
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    정수		modes;
    char_u	*map_to;
    정수		noremap;
    정수		silent = FALSE;
    정수		special = FALSE;
    정수		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    정수		i;
#만약 defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    정수		old_menu_height;
# 만약 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    정수		old_toolbar_height;
# endif
#endif
    정수		pri_tab[MENUDEPTH + 1];
    정수		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = NULL;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**root_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    위해 (;;)
    {
	만약 (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	만약 (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	만약 (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    continue;
	}
	break;
    }


    // Locate an optional "icon=filename" argument.
    만약 (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	동안 (*arg != NUL && *arg != ' ')
	{
	    만약 (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	만약 (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    위해 (p = arg; *p; ++p)
	만약 (!VIM_ISDIGIT(*p) && *p != '.')
	    break;
    만약 (VIM_ISWHITE(*p))
    {
	위해 (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    만약 (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    만약 (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    아니면 만약 (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    아니면
	i = 0;
    동안 (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    만약 (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    아니면 만약 (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    만약 (*arg == NUL)
    {
	show_menus(arg, modes);
	반환;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    만약 (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	만약 (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    만약 (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		만약 (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((char *)menu_path + 7);
		    만약 (menuarg.iconidx >= (정수)TOOLBAR_NAME_COUNT)
			menuarg.iconidx = -1;
		    아니면
			menuarg.icon_builtin = TRUE;
		}
	    }
	    아니면
	    {
		위해 (i = 0; i < (정수)TOOLBAR_NAME_COUNT; ++i)
		    만약 (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
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
    만약 (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	goto theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    만약 (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	goto theend;
    }
    아니면 만약 (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	goto theend;
    }
#만약 defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# 만약 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    root_menu_ptr = get_root_menu(menu_path);
    만약 (root_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    만약 (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	만약 (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	만약 (menu_is_popup(menu_path))
	{
	    위해 (i = 0; i < MENU_INDEX_TIP; ++i)
		만약 (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    만약 (p != NULL)
		    {
			menu_nable_recurse(*root_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*root_menu_ptr, menu_path, modes, enable);
    }
    아니면 만약 (unmenu)
    {
	만약 (is_menus_locked())
	    goto theend;

	/*
	 * Delete menu(s).
	 */
	만약 (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	만약 (menu_is_popup(menu_path))
	{
	    위해 (i = 0; i < MENU_INDEX_TIP; ++i)
		만약 (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    만약 (p != NULL)
		    {
			remove_menu(root_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(root_menu_ptr, menu_path, modes, FALSE);
    }
    아니면
    {
	만약 (is_menus_locked())
	    goto theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	만약 (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = NULL;
	}
	아니면 만약 (modes & MENU_TIP_MODE)
	    map_buf = NULL;	// Menu tips are plain text.
	아니면
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
	만약 (menu_is_popup(menu_path))
	{
	    위해 (i = 0; i < MENU_INDEX_TIP; ++i)
		만약 (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    만약 (p != NULL)
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

#만약 defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    만약 (gui.in_use
	    && (gui.menu_height != old_menu_height
# 만약 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    만약 (root_menu_ptr == &curwin->w_winbar)
    {
	정수 h = winbar_height(curwin);

	만약 (h != curwin->w_winbar_height)
	{
	    만약 (h == 0)
		++curwin->w_height;
	    아니면 만약 (curwin->w_height > 0)
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
    static 정수
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    정수		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , 정수	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    정수		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = NULL;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    정수		i;
    정수		c;
    정수		d;
#ifdef FEAT_GUI
    정수		idx;
    정수		new_idx;
#endif
    정수		pri_idx = 0;
    정수		old_modes = 0;
    정수		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = NULL;
#endif
    vimmenu_T	**root_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    만약 (path_name == NULL)
	반환 FAIL;
    root_menu_ptr = get_root_menu(menu_path);
    menup = root_menu_ptr;
    parent = NULL;
    name = path_name;
    동안 (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (정수)STRLEN(name));
	만약 (map_to != NULL)
	{
	    en_name = name;
	    name = map_to;
	}
	아니면
	    en_name = NULL;
#endif
	dname = menu_text(name, NULL, NULL);
	만약 (dname == NULL)
	    goto erret;
	만약 (*dname == NUL)
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
	동안 (menu != NULL)
	{
	    만약 (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		만약 (*next_name == NUL && menu->children != NULL)
		{
		    만약 (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    goto erret;
		}
		만약 (*next_name != NUL && menu->children == NULL
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    만약 (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    goto erret;
		}
		break;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    만약 (parent != NULL || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		만약 (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	만약 (menu == NULL)
	{
	    만약 (*next_name == NUL && parent == NULL)
	    {
		emsg(_(e_must_not_add_menu_items_directly_to_menu_bar));
		goto erret;
	    }

	    만약 (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		goto erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    만약 (menu == NULL)
		goto erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    만약 (en_name != NULL)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, NULL, NULL);
	    }
	    아니면
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
	    만약 (*next_name == NUL && menuarg->iconfile != NULL)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#만약 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    만약 (parent != NULL && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	아니면
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    만약 (addtearoff)
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
	만약 ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    만약 (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		만약 (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		아니면
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# 만약 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    만약 (	addtearoff
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
		만약 (tearpath != NULL)
		{
		    char_u  *s;
		    정수	    len;

		    STRCPY(tearpath, menu_path);
		    len = (정수)(next_name - path_name - 1);
		    위해 (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			만약 ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	만약 (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    만약 (sys_menu)
	modes &= ~old_modes;

    만약 (menu != NULL && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == NULL) ? NULL : vim_strsave(call_data);

	// loop over all modes, may add more than one
	위해 (i = 0; i < MENU_MODES; ++i)
	{
	    만약 (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		만약 (amenu && call_data != NULL && *call_data != NUL
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

		만약 (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    만약 (menu->strings[i] != NULL)
		    {
			menu->strings[i][0] = c;
			만약 (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			아니면
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			만약 (c == Ctrl_C)
			{
			    정수	    len = (정수)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		아니면
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#만약 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	만약 (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    반환 OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    동안 (parent != NULL && parent->children == NULL)
    {
	만약 (parent->parent == NULL)
	    menup = root_menu_ptr;
	아니면
	    menup = &parent->parent->children;
	위해 ( ; *menup != NULL && *menup != parent; menup = &((*menup)->next))
	    ;
	만약 (*menup == NULL) // safety check
	    break;
	parent = parent->parent;
	free_menu(menup);
    }
    반환 FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    static 정수
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    정수		modes,
    정수		enable)
{
    char_u	*p;

    만약 (menu == NULL)
	반환 OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    동안 (menu != NULL)
    {
	만약 (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    만약 (*p != NUL)
	    {
		만약 (menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    반환 FAIL;
		}
		만약 (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    반환 FAIL;
	    }
	    아니면
		만약 (enable)
		    menu->enabled |= modes;
		아니면
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    만약 (*name != NUL && *name != '*')
		break;
	}
	menu = menu->next;
    }
    만약 (*name != NUL && *name != '*' && menu == NULL)
    {
	semsg(_(e_no_menu_str), name);
	반환 FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    반환 OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    static 정수
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    정수		modes,
    정수		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    만약 (*menup == NULL)
	반환 OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    동안 ((menu = *menup) != NULL)
    {
	만약 (*name == NUL || menu_name_equal(name, menu))
	{
	    만약 (*p != NUL && menu->children == NULL)
	    {
		만약 (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		반환 FAIL;
	    }
	    만약 ((menu->modes & modes) != 0x0)
	    {
#만약 defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		만약 (*p == NUL && modes == MENU_ALL_MODES)
		{
		    만약 (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		만약 (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    반환 FAIL;
	    }
	    아니면 만약 (*name != NUL)
	    {
		만약 (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		반환 FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    만약 (*name != NUL)
		break;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    만약 (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    만약 ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    아니면
		menup = &menu->next;
	}
	아니면
	    menup = &menu->next;
    }
    만약 (*name != NUL)
    {
	만약 (menu == NULL)
	{
	    만약 (!silent)
		semsg(_(e_no_menu_str), name);
	    반환 FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#만약 defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	만약 ((s_tearoffs) && (menu->children != NULL)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	아니면
#endif
	    child = menu->children;
	위해 ( ; child != NULL; child = child->next)
	    menu->modes |= child->modes;
	만약 (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#만약 defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    만약 (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	만약 ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#만약 defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    만약 (s_tearoffs && menu->children != NULL) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    반환 OK;
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
    정수		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    만약 (gui.in_use)
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
    위해 (i = 0; i < MENU_MODES; i++)
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
free_menu_string(vimmenu_T *menu, 정수 idx)
{
    정수		count = 0;
    정수		i;

    위해 (i = 0; i < MENU_MODES; i++)
	만약 (menu->strings[i] == menu->strings[idx])
	    count++;
    만약 (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = NULL;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    static 정수
show_menus(char_u *path_name, 정수 modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = NULL;

    name = path_name = vim_strsave(path_name);
    만약 (path_name == NULL)
	반환 FAIL;
    menu = *get_root_menu(path_name);

    // First, find the (sub)menu with the given name
    동안 (*name)
    {
	p = menu_name_skip(name);
	동안 (menu != NULL)
	{
	    만약 (menu_name_equal(name, menu))
	    {
		// Found menu
		만약 (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    반환 FAIL;
		}
		아니면 만약 ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    반환 FAIL;
		}
		break;
	    }
	    menu = menu->next;
	}
	만약 (menu == NULL)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    반환 FAIL;
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
    반환 OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    static void
show_menus_recursive(vimmenu_T *menu, 정수 modes, 정수 depth)
{
    정수		i;
    정수		bit;

    만약 (menu != NULL && (menu->modes & modes) == 0x0)
	반환;

    만약 (menu != NULL)
    {
	msg_putchar('\n');
	만약 (got_int)		// "q" hit for "--more--"
	    반환;
	위해 (i = 0; i < depth; i++)
	    msg_puts("  ");
	만약 (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    msg_puts(" ");
	}
				// Same highlighting as for directories!?
	msg_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    만약 (menu != NULL && menu->children == NULL)
    {
	위해 (bit = 0; bit < MENU_MODES; bit++)
	    만약 ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		만약 (got_int)		// "q" hit for "--more--"
		    반환;
		위해 (i = 0; i < depth + 2; i++)
		    msg_puts("  ");
		msg_puts(menu_mode_chars[bit]);
		만약 (menu->noremap[bit] == REMAP_NONE)
		    msg_putchar('*');
		아니면 만약 (menu->noremap[bit] == REMAP_SCRIPT)
		    msg_putchar('&');
		아니면
		    msg_putchar(' ');
		만약 (menu->silent[bit])
		    msg_putchar('s');
		아니면
		    msg_putchar(' ');
		만약 ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    msg_putchar('-');
		아니면
		    msg_putchar(' ');
		msg_puts(" ");
		만약 (*menu->strings[bit] == NUL)
		    msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
		아니면
		    msg_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    아니면
    {
	만약 (menu == NULL)
	{
	    menu = root_menu;
	    depth--;
	}
	아니면
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	위해 (; menu != NULL && !got_int; menu = menu->next)
	    만약 (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
static vimmenu_T	*expand_menu = NULL;
static vimmenu_T	*expand_menu_alt = NULL;
static 정수		expand_modes = 0x0;
static 정수		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    정수		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = NULL;
    char_u	*name;
    정수		unmenu;
    vimmenu_T	*menu;
    정수		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    위해 (p = arg; *p; ++p)
	만약 (!VIM_ISDIGIT(*p) && *p != '.')
	    break;

    만약 (!VIM_ISWHITE(*p))
    {
	만약 (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	아니면 만약 (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	아니면
	    p = arg;
    }

    동안 (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    위해 (; *p && !VIM_ISWHITE(*p); ++p)
    {
	만약 ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	아니면 만약 (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    만약 (expand_menus && VIM_ISWHITE(*p))
	반환 NULL;	// TODO: check for next command?
    만약 (*p == NUL)		// Complete the menu name
    {
	정수 try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, NULL, &unmenu);
	만약 (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	만약 (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    만약 (path_name == NULL)
		반환 NULL;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	동안 (name != NULL && *name)
	{
	    p = menu_name_skip(name);
	    동안 (menu != NULL)
	    {
		만약 (menu_name_equal(name, menu))
		{
		    // Found menu
		    만약 ((*p != NUL && menu->children == NULL)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			반환 NULL;
		    }
		    break;
		}
		menu = menu->next;
		만약 (menu == NULL && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    만약 (menu == NULL)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		반환 NULL;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	만약 (expand_menu == root_menu)
	    expand_menu_alt = curwin->w_winbar;
	아니면
	    expand_menu_alt = NULL;
    }
    아니면			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    반환 NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, 정수 idx)
{
    static vimmenu_T	*menu = NULL;
    static 정수		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  정수		should_advance = FALSE;
#endif

    만약 (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    동안 (menu != NULL && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == NULL))
    {
	menu = menu->next;
	만약 (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    만약 (menu == NULL)	    // at end of linked list
	반환 NULL;

    만약 (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	만약 (should_advance)
	    str = menu->en_dname;
	아니면
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    만약 (menu->en_dname == NULL)
		should_advance = TRUE;
	}
#endif
    아니면
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    만약 (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	만약 (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    반환 str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, 정수 idx)
{
    static vimmenu_T	*menu = NULL;
    static 정수		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    static char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  정수		should_advance = FALSE;
#endif

    만약 (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    동안 (menu != NULL
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	만약 (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    만약 (menu == NULL)	    // at end of linked list
	반환 NULL;

    만약 (menu->modes & expand_modes)
    {
	만약 (menu->children != NULL)
	{
#ifdef FEAT_MULTI_LANG
	    만약 (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    아니면
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		만약 (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	아니면
#ifdef FEAT_MULTI_LANG
	{
	    만약 (should_advance)
		str = menu->en_dname;
	    아니면
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		만약 (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    아니면
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    만약 (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	만약 (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    반환 str;
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

    위해 (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	만약 (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    만약 (*p == NUL)
		break;
	}
    }
    만약 (*p)
	*p++ = NUL;
    반환 p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    static 정수
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    만약 (menu->en_name != NULL
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	반환 TRUE;
#endif
    반환 menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    static 정수
menu_namecmp(char_u *name, char_u *mname)
{
    정수		i;

    위해 (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	만약 (name[i] != mname[i])
	    break;
    반환 ((name[i] == NUL || name[i] == TAB)
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
    static 정수
get_menu_cmd_modes(
    char_u  *cmd,
    정수	    forceit,	    // Was there a "!" after the command?
    정수	    *noremap,
    정수	    *unmenu)
{
    정수	    modes;

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
	    만약 (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
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
	    만약 (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		break;
	    }
	    // FALLTHROUGH
	default:
	    --cmd;
	    만약 (forceit)		// menu!!
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    아니면			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    만약 (noremap != NULL)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    만약 (unmenu != NULL)
	*unmenu = (*cmd == 'u');
    반환 modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    static char_u *
get_menu_mode_str(정수 modes)
{
    만약 ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	반환 (char_u *)"a";
    만약 ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	반환 (char_u *)" ";
    만약 ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	반환 (char_u *)"!";
    만약 ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	반환 (char_u *)"v";
    만약 (modes & MENU_VISUAL_MODE)
	반환 (char_u *)"x";
    만약 (modes & MENU_SELECT_MODE)
	반환 (char_u *)"s";
    만약 (modes & MENU_OP_PENDING_MODE)
	반환 (char_u *)"o";
    만약 (modes & MENU_INSERT_MODE)
	반환 (char_u *)"i";
    만약 (modes & MENU_TERMINAL_MODE)
	반환 (char_u *)"tl";
    만약 (modes & MENU_CMDLINE_MODE)
	반환 (char_u *)"c";
    만약 (modes & MENU_NORMAL_MODE)
	반환 (char_u *)"n";
    만약 (modes & MENU_TIP_MODE)
	반환 (char_u *)"t";

    반환 (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    static char_u *
popup_mode_name(char_u *name, 정수 idx)
{
    char_u	*p;
    정수		len = (정수)STRLEN(name);
    char	*mode_chars = menu_mode_chars[idx];
    정수		mode_chars_len = (정수)strlen(mode_chars);
    정수		i;

    p = vim_strnsave(name, len + mode_chars_len);
    만약 (p == NULL)
	반환 NULL;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    위해 (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    반환 p;
}

#만약 defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    정수
get_menu_index(vimmenu_T *menu, 정수 state)
{
    정수		idx;

    만약 ((state & MODE_INSERT))
	idx = MENU_INDEX_INSERT;
    아니면 만약 (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    아니면 만약 (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    아니면 만약 (VIsual_active)
    {
	만약 (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	아니면
	    idx = MENU_INDEX_VISUAL;
    }
    아니면 만약 (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    아니면 만약 (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    아니면 만약 ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    아니면
	idx = MENU_INDEX_INVALID;

    만약 (idx != MENU_INDEX_INVALID && menu->strings[idx] == NULL)
	idx = MENU_INDEX_INVALID;
    반환 idx;
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
menu_text(char_u *str, 정수 *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    만약 (p != NULL)
    {
	만약 (actext != NULL)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    아니면
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    위해 (p = text; p != NULL; )
    {
	p = vim_strchr(p, '&');
	만약 (p != NULL)
	{
	    만약 (p[1] == NUL)	    // trailing "&"
		break;
	    만약 (mnemonic != NULL && p[1] != '&')
#만약 !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#아니면
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
    반환 text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    정수
menu_is_menubar(char_u *name)
{
    반환 (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    정수
menu_is_popup(char_u *name)
{
    반환 (STRNCMP(name, "PopUp", 5) == 0);
}

#만약 defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    정수
menu_is_child_of_popup(vimmenu_T *menu)
{
    동안 (menu->parent != NULL)
	menu = menu->parent;
    반환 menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    정수
menu_is_toolbar(char_u *name)
{
    반환 (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    정수
menu_is_separator(char_u *name)
{
    반환 (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    static 정수
menu_is_hidden(char_u *name)
{
    반환 (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    static 정수
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    반환 (STRCMP(name, TEAR_STRING) == 0);
#아니면
    반환 FALSE;
#endif
}

#만약 defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    static 정수
get_menu_mode(void)
{
# ifdef FEAT_TERMINAL
    만약 (term_use_loop())
	반환 MENU_INDEX_TERMINAL;
# endif
    만약 (VIsual_active)
    {
	만약 (VIsual_select)
	    반환 MENU_INDEX_SELECT;
	반환 MENU_INDEX_VISUAL;
    }
    만약 (State & MODE_INSERT)
	반환 MENU_INDEX_INSERT;
    만약 ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	반환 MENU_INDEX_CMDLINE;
    만약 (finish_op)
	반환 MENU_INDEX_OP_PENDING;
    만약 (State & MODE_NORMAL)
	반환 MENU_INDEX_NORMAL;
    만약 (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	반환 MENU_INDEX_INSERT;
    반환 MENU_INDEX_INVALID;
}

    정수
get_menu_mode_flag(void)
{
    정수 mode = get_menu_mode();

    만약 (mode == MENU_INDEX_INVALID)
	반환 0;
    반환 1 << mode;
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
    정수		menu_mode;
    char*	mode;
    정수		mode_len;

    menu_mode = get_menu_mode();
    만약 (menu_mode == MENU_INDEX_INVALID)
	반환;
    mode = menu_mode_chars[menu_mode];
    mode_len = (정수)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, NULL, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	만약 (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    break;

    // Only show a popup when it is defined and has entries
    만약 (menu == NULL || menu->children == NULL)
	반환;

# 만약 defined(FEAT_GUI)
    만약 (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# 만약 defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    아니면
# endif
# 만약 defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#만약 defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    정수
check_menu_pointer(vimmenu_T *root, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    위해 (p = root; p != NULL; p = p->next)
	만약 (p == menu_to_check
		|| (p->children != NULL
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    반환 OK;
    반환 FAIL;
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
    정수		idx = 0;

    동안 (menu != NULL)
    {
	// Don't add a menu when only a tip was defined.
	만약 (menu->modes & MENU_ALL_MODES)
	{
	    만약 (menu->children != NULL)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    아니면
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
gui_update_menus_recurse(vimmenu_T *menu, 정수 mode)
{
    정수		grey;

    동안 (menu)
    {
	만약 ((menu->modes & menu->enabled & mode)
# 만약 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	아니면
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	만약 (vim_strchr(p_go, GO_GREY) != NULL || menu->parent == NULL
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	아니면
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
gui_update_menus(정수 modes)
{
    static 정수	    prev_mode = -1;
    정수		    mode = 0;

    만약 (modes != 0x0)
	mode = modes;
    아니면
	mode = get_menu_mode_flag();

    만약 (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# 만약 defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    정수
gui_is_menu_shortcut(정수 key)
{
    vimmenu_T	*menu;

    만약 (key < 256)
	key = TOLOWER_LOC(key);
    FOR_ALL_MENUS(menu)
	만약 (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == key))
	    반환 TRUE;
    반환 FALSE;
}
# endif
#endif // FEAT_GUI

#만약 defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    void
gui_mch_toggle_tearoffs(정수 enable)
{
    정수		pri_tab[MENUDEPTH + 1];
    정수		i;

    만약 (enable)
    {
	위해 (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    아니면
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
    정수			*pri_tab,
    정수			pri_idx)
{
    char_u	*newpname = NULL;
    정수		len;
    char_u	*s;
    char_u	*d;

    만약 (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    동안 (menu != NULL)
    {
	만약 (menu->children != NULL && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (정수)STRLEN(pname) + (정수)STRLEN(menu->name);
	    위해 (s = menu->name; *s; ++s)
		만약 (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    만약 (newpname != NULL)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		위해 (s = menu->name; *s; ++s)
		{
		    만약 (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		만약 (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
gui_add_tearoff(char_u *tearpath, 정수 *pri_tab, 정수 pri_idx)
{
    char_u	*tbuf;
    정수		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned 정수)STRLEN(tearpath));
    만약 (tbuf == NULL)
	반환;

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
    동안 (menu)
    {
	만약 (menu->children)
	{
	    // check if tearoff exists
	    만약 (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    만약 (menu->children != NULL) // if not the last one
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
execute_menu(exarg_T *eap, vimmenu_T *menu, 정수 mode_idx)
{
    정수		idx = mode_idx;

    만약 (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	만약 (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_INSERT;
	}
#ifdef FEAT_TERMINAL
	아니면 만약 (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	아니면 만약 (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	아니면 만약 (eap != NULL && eap->addr_count)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    만약 ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    아니면
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
	    만약 (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    만약 (idx == MENU_INDEX_INVALID || eap == NULL)
	idx = MENU_INDEX_NORMAL;

    만약 (menu->strings[idx] != NULL && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	만약 (eap == NULL || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    만약 (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	아니면
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    아니면 만약 (eap != NULL)
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
    정수		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    만약 (saved_name == NULL)
	반환 NULL;

    menu = *get_root_menu(saved_name);
    name = saved_name;
    동안 (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	동안 (menu != NULL)
	{
	    만약 (menu_name_equal(name, menu))
	    {
		만약 (*p == NUL && menu->children != NULL)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = NULL;
		}
		아니면 만약 (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		}
		break;
	    }
	    menu = menu->next;
	}
	만약 (menu == NULL || *p == NUL)
	    break;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    만약 (menu == NULL)
    {
	만약 (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	반환 NULL;
    }

    반환 menu;
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
    정수		mode_idx = MENU_INDEX_INVALID;

    만약 (arg[0] && VIM_ISWHITE(arg[1]))
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
		     반환;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    만약 (menu == NULL)
	반환;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    void
winbar_click(win_T *wp, 정수 col)
{
    정수		idx;

    만약 (wp->w_winbar_items == NULL)
	반환;
    위해 (idx = 0; wp->w_winbar_items[idx].wb_menu != NULL; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_items[idx];

	만약 (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = NULL;
	    pos_T   save_visual = VIsual;
	    정수	    save_visual_active = VIsual_active;
	    정수	    save_visual_select = VIsual_select;
	    정수	    save_visual_reselect = VIsual_reselect;
	    정수	    save_visual_mode = VIsual_mode;

	    만약 (wp != curwin)
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

	    만약 (save_curwin != NULL && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    만약 (!win_valid(wp))
		break;
	}
    }
}

#만약 defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
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
    만약 (saved_name == NULL)
	반환 NULL;

    name = saved_name;
    동안 (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	동안 (menu != NULL)
	{
	    만약 (menu_name_equal(name, menu))
	    {
		만약 (menu->children == NULL)
		{
		    // found a menu item instead of a sub-menu
		    만약 (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    아니면
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		    goto theend;
		}
		만약 (*p == NUL)	    // found a full match
		    goto theend;
		break;
	    }
	    menu = menu->next;
	}
	만약 (menu == NULL)	// didn't find it
	    break;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    만약 (menu == NULL)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    반환 menu;
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
    정수			i;
    char_u		*from, *from_noamp, *to;

    만약 (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, sizeof(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    만약 (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	위해 (i = 0; i < menutrans_ga.ga_len; ++i)
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
    아니면
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	만약 (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	아니면
	{
	    만약 (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		만약 (from != NULL)
		{
		    from_noamp = menu_text(from, NULL, NULL);
		    to = vim_strnsave(to, arg - to);
		    만약 (from_noamp != NULL && to != NULL)
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
		    아니면
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

#만약 defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    static char_u *
menu_skip_part(char_u *p)
{
    동안 (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	만약 ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    반환 p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    static char_u *
menutrans_lookup(char_u *name, 정수 len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    정수			i;
    char_u		*dname;

    위해 (i = 0; i < menutrans_ga.ga_len; ++i)
	만약 (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    반환 tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, NULL, NULL);
    name[len] = i;
    만약 (dname == NULL)
	반환 NULL;

    위해 (i = 0; i < menutrans_ga.ga_len; ++i)
	만약 (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    반환 tp[i].to;
	}
    vim_free(dname);

    반환 NULL;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    static void
menu_unescape_name(char_u *name)
{
    char_u  *p;

    위해 (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	만약 (*p == '\\')
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

    동안 (*arg && !VIM_ISWHITE(*arg))
    {
	만약 ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	아니면 만약 (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    만약 (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    반환 arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    static 정수
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, 정수 modes, dict_T *dict)
{
    정수		status;
    list_T	*l;

    만약 (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	만약 (l == NULL)
	    반환 FAIL;

	만약 (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    반환 FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	위해 (topmenu = menu; topmenu != NULL; topmenu = topmenu->next)
	    만약 (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	반환 OK;
    }

    만약 (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	반환 OK;

    status = dict_add_string(dict, "name", menu->name);
    만약 (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    만약 (status == OK && menu->actext != NULL)
	status = dict_add_string(dict, "accel", menu->actext);
    만약 (status == OK)
	status = dict_add_number(dict, "priority", menu->priority);
    만약 (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    만약 (status == OK && menu->iconfile != NULL)
	status = dict_add_string(dict, "icon", menu->iconfile);
    만약 (status == OK && menu->iconidx >= 0)
	status = dict_add_number(dict, "iconidx", menu->iconidx);
#endif
    만약 (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	정수	buflen;

	만약 (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	아니면
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    만약 (status == OK && menu->children == NULL)
    {
	정수		bit;

	// Get the first mode in which the menu is available
	위해 (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	만약 (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    만약 (menu->strings[bit] != NULL)
	    {
		만약 (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		아니면
		{
		    char_u *tofree = NULL;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    만약 (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    만약 (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    만약 (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    만약 (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    만약 (status == OK && menu->children != NULL)
    {
	vimmenu_T	*child;

	l = list_alloc();
	만약 (l == NULL)
	    반환 FAIL;

	만약 (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    반환 FAIL;
	}
	child = menu->children;
	동안 (child)
	{
	    만약 (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    반환 status;
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
    정수		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    만약 (rettv_dict_alloc(rettv) == FAIL)
	반환;
    retdict = rettv->vval.v_dict;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	반환;

    menu_name = tv_get_string_chk(&argvars[0]);
    만약 (menu_name == NULL)
	반환;

    // menu mode
    만약 (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    아니면
	which = (char_u *)"";	    // Default is modes for "menu"
    만약 (which == NULL)
	반환;

    modes = get_menu_cmd_modes(which, *which == '!', NULL, NULL);

    // Locate the specified menu or menu item
    menu = *get_root_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    만약 (saved_name == NULL)
	반환;
    만약 (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	동안 (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    동안 (menu != NULL)
	    {
		만약 (menu_name_equal(name, menu))
		    break;
		menu = menu->next;
	    }
	    만약 (menu == NULL || *p == NUL)
		break;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    만약 (menu == NULL)		// specified menu not found
	반환;

    만약 (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
