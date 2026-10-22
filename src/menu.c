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

#ᬬᬾᬦ᭄ defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
static puur_naa_nka add_menu_path(char_u *, vimmenu_T *, puur_naa_nka *, char_u *, puur_naa_nka);
#ᬮᬶᬬ
static puur_naa_nka add_menu_path(char_u *, vimmenu_T *, puur_naa_nka *, char_u *);
#endif
static puur_naa_nka menu_nable_recurse(vimmenu_T *menu, char_u *name, puur_naa_nka modes, puur_naa_nka enable);
static puur_naa_nka remove_menu(vimmenu_T **, char_u *, puur_naa_nka, puur_naa_nka silent);
static void free_menu(vimmenu_T **menup);
static void free_menu_string(vimmenu_T *, puur_naa_nka);
static puur_naa_nka show_menus(char_u *, puur_naa_nka);
static void show_menus_recursive(vimmenu_T *, puur_naa_nka, puur_naa_nka);
static char_u *menu_name_skip(char_u *name);
static puur_naa_nka menu_name_equal(char_u *name, vimmenu_T *menu);
static puur_naa_nka menu_namecmp(char_u *name, char_u *mname);
static puur_naa_nka get_menu_cmd_modes(char_u *, puur_naa_nka, puur_naa_nka *, puur_naa_nka *);
static char_u *popup_mode_name(char_u *name, puur_naa_nka idx);
static char_u *menu_text(char_u *text, puur_naa_nka *mnemonic, char_u **actext);

#ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
static void gui_create_tearoffs_recurse(vimmenu_T *menu, const char_u *pname, puur_naa_nka *pri_tab, puur_naa_nka pri_idx);
static void gui_add_tearoff(char_u *tearpath, puur_naa_nka *pri_tab, puur_naa_nka pri_idx);
static void gui_destroy_tearoffs_recurse(vimmenu_T *menu);
static puur_naa_nka s_tearoffs = FALSE;
#endif

static puur_naa_nka menu_is_hidden(char_u *name);
static puur_naa_nka menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
static puur_naa_nka menus_locked = 0;

#ᬬᬾᬦ᭄ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
static char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
static char_u *menutrans_lookup(char_u *name, puur_naa_nka len);
static void menu_unescape_name(char_u	*p);
#endif

static char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
static var_na *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
static const var_na *toolbar_names[] =
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
    static puur_naa_nka
menu_is_winbar(char_u *name)
{
    ᬯᬮᬶ (STRNCMP(name, "WinBar", 6) == 0);
}

    puur_naa_nka
winbar_height(win_T *wp)
{
    ᬬᬾᬦ᭄ (wp->w_winbar != NULL && wp->w_winbar->children != NULL)
	ᬯᬮᬶ 1;
    ᬯᬮᬶ 0;
}

    static vimmenu_T **
get_root_menu(char_u *name)
{
    ᬬᬾᬦ᭄ (menu_is_winbar(name))
	ᬯᬮᬶ &curwin->w_winbar;
    ᬯᬮᬶ &root_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static puur_naa_nka
is_menus_locked(void)
{
    ᬬᬾᬦ᭄ (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	ᬯᬮᬶ TRUE;
    }
    ᬯᬮᬶ FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    void
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    puur_naa_nka		modes;
    char_u	*map_to;
    puur_naa_nka		noremap;
    puur_naa_nka		silent = FALSE;
    puur_naa_nka		special = FALSE;
    puur_naa_nka		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    puur_naa_nka		i;
#ᬬᬾᬦ᭄ defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    puur_naa_nka		old_menu_height;
# ᬬᬾᬦ᭄ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    puur_naa_nka		old_toolbar_height;
# endif
#endif
    puur_naa_nka		pri_tab[MENUDEPTH + 1];
    puur_naa_nka		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = NULL;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**root_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    for (;;)
    {
	ᬬᬾᬦ᭄ (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	ᬬᬾᬦ᭄ (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	ᬬᬾᬦ᭄ (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    continue;
	}
	bha_nga;
    }


    // Locate an optional "icon=filename" argument.
    ᬬᬾᬦ᭄ (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	ᬲᭂᬤᭂᬮ (*arg != NUL && *arg != ' ')
	{
	    ᬬᬾᬦ᭄ (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	ᬬᬾᬦ᭄ (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    for (p = arg; *p; ++p)
	ᬬᬾᬦ᭄ (!VIM_ISDIGIT(*p) && *p != '.')
	    bha_nga;
    ᬬᬾᬦ᭄ (VIM_ISWHITE(*p))
    {
	for (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    ᬬᬾᬦ᭄ (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    ᬬᬾᬦ᭄ (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    ᬮᬶᬬ
	i = 0;
    ᬲᭂᬤᭂᬮ (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    ᬬᬾᬦ᭄ (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    ᬬᬾᬦ᭄ (*arg == NUL)
    {
	show_menus(arg, modes);
	ᬯᬮᬶ;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    ᬬᬾᬦ᭄ (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	ᬬᬾᬦ᭄ (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    ᬬᬾᬦ᭄ (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		ᬬᬾᬦ᭄ (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((var_na *)menu_path + 7);
		    ᬬᬾᬦ᭄ (menuarg.iconidx >= (puur_naa_nka)TOOLBAR_NAME_COUNT)
			menuarg.iconidx = -1;
		    ᬮᬶᬬ
			menuarg.icon_builtin = TRUE;
		}
	    }
	    ᬮᬶᬬ
	    {
		for (i = 0; i < (puur_naa_nka)TOOLBAR_NAME_COUNT; ++i)
		    ᬬᬾᬦ᭄ (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			bha_nga;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    ᬬᬾᬦ᭄ (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	goto theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    ᬬᬾᬦ᭄ (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	goto theend;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	goto theend;
    }
#ᬬᬾᬦ᭄ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# ᬬᬾᬦ᭄ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    root_menu_ptr = get_root_menu(menu_path);
    ᬬᬾᬦ᭄ (root_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    ᬬᬾᬦ᭄ (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	ᬬᬾᬦ᭄ (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	ᬬᬾᬦ᭄ (menu_is_popup(menu_path))
	{
	    for (i = 0; i < MENU_INDEX_TIP; ++i)
		ᬬᬾᬦ᭄ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    ᬬᬾᬦ᭄ (p != NULL)
		    {
			menu_nable_recurse(*root_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*root_menu_ptr, menu_path, modes, enable);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (unmenu)
    {
	ᬬᬾᬦ᭄ (is_menus_locked())
	    goto theend;

	/*
	 * Delete menu(s).
	 */
	ᬬᬾᬦ᭄ (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	ᬬᬾᬦ᭄ (menu_is_popup(menu_path))
	{
	    for (i = 0; i < MENU_INDEX_TIP; ++i)
		ᬬᬾᬦ᭄ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    ᬬᬾᬦ᭄ (p != NULL)
		    {
			remove_menu(root_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(root_menu_ptr, menu_path, modes, FALSE);
    }
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (is_menus_locked())
	    goto theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	ᬬᬾᬦ᭄ (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = NULL;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (modes & MENU_TIP_MODE)
	    map_buf = NULL;	// Menu tips are plain text.
	ᬮᬶᬬ
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
	ᬬᬾᬦ᭄ (menu_is_popup(menu_path))
	{
	    for (i = 0; i < MENU_INDEX_TIP; ++i)
		ᬬᬾᬦ᭄ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    ᬬᬾᬦ᭄ (p != NULL)
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

#ᬬᬾᬦ᭄ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    ᬬᬾᬦ᭄ (gui.in_use
	    && (gui.menu_height != old_menu_height
# ᬬᬾᬦ᭄ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    ᬬᬾᬦ᭄ (root_menu_ptr == &curwin->w_winbar)
    {
	puur_naa_nka h = winbar_height(curwin);

	ᬬᬾᬦ᭄ (h != curwin->w_winbar_height)
	{
	    ᬬᬾᬦ᭄ (h == 0)
		++curwin->w_height;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (curwin->w_height > 0)
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
    static puur_naa_nka
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    puur_naa_nka		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , puur_naa_nka	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    puur_naa_nka		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = NULL;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    puur_naa_nka		i;
    puur_naa_nka		c;
    puur_naa_nka		d;
#ifdef FEAT_GUI
    puur_naa_nka		idx;
    puur_naa_nka		new_idx;
#endif
    puur_naa_nka		pri_idx = 0;
    puur_naa_nka		old_modes = 0;
    puur_naa_nka		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = NULL;
#endif
    vimmenu_T	**root_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    ᬬᬾᬦ᭄ (path_name == NULL)
	ᬯᬮᬶ FAIL;
    root_menu_ptr = get_root_menu(menu_path);
    menup = root_menu_ptr;
    parent = NULL;
    name = path_name;
    ᬲᭂᬤᭂᬮ (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (puur_naa_nka)STRLEN(name));
	ᬬᬾᬦ᭄ (map_to != NULL)
	{
	    en_name = name;
	    name = map_to;
	}
	ᬮᬶᬬ
	    en_name = NULL;
#endif
	dname = menu_text(name, NULL, NULL);
	ᬬᬾᬦ᭄ (dname == NULL)
	    goto erret;
	ᬬᬾᬦ᭄ (*dname == NUL)
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
	ᬲᭂᬤᭂᬮ (menu != NULL)
	{
	    ᬬᬾᬦ᭄ (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		ᬬᬾᬦ᭄ (*next_name == NUL && menu->children != NULL)
		{
		    ᬬᬾᬦ᭄ (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    goto erret;
		}
		ᬬᬾᬦ᭄ (*next_name != NUL && menu->children == NULL
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    ᬬᬾᬦ᭄ (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    goto erret;
		}
		bha_nga;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    ᬬᬾᬦ᭄ (parent != NULL || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		ᬬᬾᬦ᭄ (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	ᬬᬾᬦ᭄ (menu == NULL)
	{
	    ᬬᬾᬦ᭄ (*next_name == NUL && parent == NULL)
	    {
		emsg(_(e_must_not_add_menu_items_directly_to_menu_bar));
		goto erret;
	    }

	    ᬬᬾᬦ᭄ (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		goto erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    ᬬᬾᬦ᭄ (menu == NULL)
		goto erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    ᬬᬾᬦ᭄ (en_name != NULL)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, NULL, NULL);
	    }
	    ᬮᬶᬬ
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
	    ᬬᬾᬦ᭄ (*next_name == NUL && menuarg->iconfile != NULL)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    ᬬᬾᬦ᭄ (parent != NULL && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	ᬮᬶᬬ
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    ᬬᬾᬦ᭄ (addtearoff)
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
	ᬬᬾᬦ᭄ ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    ᬬᬾᬦ᭄ (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		ᬬᬾᬦ᭄ (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		ᬮᬶᬬ
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    ᬬᬾᬦ᭄ (	addtearoff
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
		ᬬᬾᬦ᭄ (tearpath != NULL)
		{
		    char_u  *s;
		    puur_naa_nka	    len;

		    STRCPY(tearpath, menu_path);
		    len = (puur_naa_nka)(next_name - path_name - 1);
		    for (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			ᬬᬾᬦ᭄ ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	ᬬᬾᬦ᭄ (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    ᬬᬾᬦ᭄ (sys_menu)
	modes &= ~old_modes;

    ᬬᬾᬦ᭄ (menu != NULL && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == NULL) ? NULL : vim_strsave(call_data);

	// loop over all modes, may add more than one
	for (i = 0; i < MENU_MODES; ++i)
	{
	    ᬬᬾᬦ᭄ (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		ᬬᬾᬦ᭄ (amenu && call_data != NULL && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    switch (1 << i)
		    {
			pak_sa MENU_VISUAL_MODE:
			pak_sa MENU_SELECT_MODE:
			pak_sa MENU_OP_PENDING_MODE:
			pak_sa MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    bha_nga;
			pak_sa MENU_INSERT_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    bha_nga;
		    }
		}

		ᬬᬾᬦ᭄ (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    ᬬᬾᬦ᭄ (menu->strings[i] != NULL)
		    {
			menu->strings[i][0] = c;
			ᬬᬾᬦ᭄ (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			ᬮᬶᬬ
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			ᬬᬾᬦ᭄ (c == Ctrl_C)
			{
			    puur_naa_nka	    len = (puur_naa_nka)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		ᬮᬶᬬ
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#ᬬᬾᬦ᭄ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	ᬬᬾᬦ᭄ (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    ᬯᬮᬶ OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    ᬲᭂᬤᭂᬮ (parent != NULL && parent->children == NULL)
    {
	ᬬᬾᬦ᭄ (parent->parent == NULL)
	    menup = root_menu_ptr;
	ᬮᬶᬬ
	    menup = &parent->parent->children;
	for ( ; *menup != NULL && *menup != parent; menup = &((*menup)->next))
	    ;
	ᬬᬾᬦ᭄ (*menup == NULL) // safety check
	    bha_nga;
	parent = parent->parent;
	free_menu(menup);
    }
    ᬯᬮᬶ FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    static puur_naa_nka
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    puur_naa_nka		modes,
    puur_naa_nka		enable)
{
    char_u	*p;

    ᬬᬾᬦ᭄ (menu == NULL)
	ᬯᬮᬶ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ᬲᭂᬤᭂᬮ (menu != NULL)
    {
	ᬬᬾᬦ᭄ (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    ᬬᬾᬦ᭄ (*p != NUL)
	    {
		ᬬᬾᬦ᭄ (menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    ᬯᬮᬶ FAIL;
		}
		ᬬᬾᬦ᭄ (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ
		ᬬᬾᬦ᭄ (enable)
		    menu->enabled |= modes;
		ᬮᬶᬬ
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    ᬬᬾᬦ᭄ (*name != NUL && *name != '*')
		bha_nga;
	}
	menu = menu->next;
    }
    ᬬᬾᬦ᭄ (*name != NUL && *name != '*' && menu == NULL)
    {
	semsg(_(e_no_menu_str), name);
	ᬯᬮᬶ FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    ᬯᬮᬶ OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    static puur_naa_nka
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    puur_naa_nka		modes,
    puur_naa_nka		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    ᬬᬾᬦ᭄ (*menup == NULL)
	ᬯᬮᬶ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ᬲᭂᬤᭂᬮ ((menu = *menup) != NULL)
    {
	ᬬᬾᬦ᭄ (*name == NUL || menu_name_equal(name, menu))
	{
	    ᬬᬾᬦ᭄ (*p != NUL && menu->children == NULL)
	    {
		ᬬᬾᬦ᭄ (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		ᬯᬮᬶ FAIL;
	    }
	    ᬬᬾᬦ᭄ ((menu->modes & modes) != 0x0)
	    {
#ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		ᬬᬾᬦ᭄ (*p == NUL && modes == MENU_ALL_MODES)
		{
		    ᬬᬾᬦ᭄ (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		ᬬᬾᬦ᭄ (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (*name != NUL)
	    {
		ᬬᬾᬦ᭄ (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		ᬯᬮᬶ FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    ᬬᬾᬦ᭄ (*name != NUL)
		bha_nga;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    ᬬᬾᬦ᭄ (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    ᬬᬾᬦ᭄ ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    ᬮᬶᬬ
		menup = &menu->next;
	}
	ᬮᬶᬬ
	    menup = &menu->next;
    }
    ᬬᬾᬦ᭄ (*name != NUL)
    {
	ᬬᬾᬦ᭄ (menu == NULL)
	{
	    ᬬᬾᬦ᭄ (!silent)
		semsg(_(e_no_menu_str), name);
	    ᬯᬮᬶ FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	ᬬᬾᬦ᭄ ((s_tearoffs) && (menu->children != NULL)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	ᬮᬶᬬ
#endif
	    child = menu->children;
	for ( ; child != NULL; child = child->next)
	    menu->modes |= child->modes;
	ᬬᬾᬦ᭄ (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#ᬬᬾᬦ᭄ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    ᬬᬾᬦ᭄ (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	ᬬᬾᬦ᭄ ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    ᬬᬾᬦ᭄ (s_tearoffs && menu->children != NULL) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    ᬯᬮᬶ OK;
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
    puur_naa_nka		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    ᬬᬾᬦ᭄ (gui.in_use)
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
    for (i = 0; i < MENU_MODES; i++)
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
free_menu_string(vimmenu_T *menu, puur_naa_nka idx)
{
    puur_naa_nka		count = 0;
    puur_naa_nka		i;

    for (i = 0; i < MENU_MODES; i++)
	ᬬᬾᬦ᭄ (menu->strings[i] == menu->strings[idx])
	    count++;
    ᬬᬾᬦ᭄ (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = NULL;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    static puur_naa_nka
show_menus(char_u *path_name, puur_naa_nka modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = NULL;

    name = path_name = vim_strsave(path_name);
    ᬬᬾᬦ᭄ (path_name == NULL)
	ᬯᬮᬶ FAIL;
    menu = *get_root_menu(path_name);

    // First, find the (sub)menu with the given name
    ᬲᭂᬤᭂᬮ (*name)
    {
	p = menu_name_skip(name);
	ᬲᭂᬤᭂᬮ (menu != NULL)
	{
	    ᬬᬾᬦ᭄ (menu_name_equal(name, menu))
	    {
		// Found menu
		ᬬᬾᬦ᭄ (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    ᬯᬮᬶ FAIL;
		}
		ᬮᬶᬬ ᬬᬾᬦ᭄ ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    ᬯᬮᬶ FAIL;
		}
		bha_nga;
	    }
	    menu = menu->next;
	}
	ᬬᬾᬦ᭄ (menu == NULL)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    ᬯᬮᬶ FAIL;
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
    ᬯᬮᬶ OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    static void
show_menus_recursive(vimmenu_T *menu, puur_naa_nka modes, puur_naa_nka depth)
{
    puur_naa_nka		i;
    puur_naa_nka		bit;

    ᬬᬾᬦ᭄ (menu != NULL && (menu->modes & modes) == 0x0)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (menu != NULL)
    {
	msg_putchar('\n');
	ᬬᬾᬦ᭄ (got_int)		// "q" hit for "--more--"
	    ᬯᬮᬶ;
	for (i = 0; i < depth; i++)
	    msg_puts("  ");
	ᬬᬾᬦ᭄ (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    msg_puts(" ");
	}
				// Same highlighting as for directories!?
	msg_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    ᬬᬾᬦ᭄ (menu != NULL && menu->children == NULL)
    {
	for (bit = 0; bit < MENU_MODES; bit++)
	    ᬬᬾᬦ᭄ ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		ᬬᬾᬦ᭄ (got_int)		// "q" hit for "--more--"
		    ᬯᬮᬶ;
		for (i = 0; i < depth + 2; i++)
		    msg_puts("  ");
		msg_puts(menu_mode_chars[bit]);
		ᬬᬾᬦ᭄ (menu->noremap[bit] == REMAP_NONE)
		    msg_putchar('*');
		ᬮᬶᬬ ᬬᬾᬦ᭄ (menu->noremap[bit] == REMAP_SCRIPT)
		    msg_putchar('&');
		ᬮᬶᬬ
		    msg_putchar(' ');
		ᬬᬾᬦ᭄ (menu->silent[bit])
		    msg_putchar('s');
		ᬮᬶᬬ
		    msg_putchar(' ');
		ᬬᬾᬦ᭄ ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    msg_putchar('-');
		ᬮᬶᬬ
		    msg_putchar(' ');
		msg_puts(" ");
		ᬬᬾᬦ᭄ (*menu->strings[bit] == NUL)
		    msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
		ᬮᬶᬬ
		    msg_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (menu == NULL)
	{
	    menu = root_menu;
	    depth--;
	}
	ᬮᬶᬬ
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	for (; menu != NULL && !got_int; menu = menu->next)
	    ᬬᬾᬦ᭄ (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
static vimmenu_T	*expand_menu = NULL;
static vimmenu_T	*expand_menu_alt = NULL;
static puur_naa_nka		expand_modes = 0x0;
static puur_naa_nka		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    puur_naa_nka		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = NULL;
    char_u	*name;
    puur_naa_nka		unmenu;
    vimmenu_T	*menu;
    puur_naa_nka		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    for (p = arg; *p; ++p)
	ᬬᬾᬦ᭄ (!VIM_ISDIGIT(*p) && *p != '.')
	    bha_nga;

    ᬬᬾᬦ᭄ (!VIM_ISWHITE(*p))
    {
	ᬬᬾᬦ᭄ (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	ᬮᬶᬬ
	    p = arg;
    }

    ᬲᭂᬤᭂᬮ (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    for (; *p && !VIM_ISWHITE(*p); ++p)
    {
	ᬬᬾᬦ᭄ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    ᬬᬾᬦ᭄ (expand_menus && VIM_ISWHITE(*p))
	ᬯᬮᬶ NULL;	// TODO: check for next command?
    ᬬᬾᬦ᭄ (*p == NUL)		// Complete the menu name
    {
	puur_naa_nka try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, NULL, &unmenu);
	ᬬᬾᬦ᭄ (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	ᬬᬾᬦ᭄ (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    ᬬᬾᬦ᭄ (path_name == NULL)
		ᬯᬮᬶ NULL;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	ᬲᭂᬤᭂᬮ (name != NULL && *name)
	{
	    p = menu_name_skip(name);
	    ᬲᭂᬤᭂᬮ (menu != NULL)
	    {
		ᬬᬾᬦ᭄ (menu_name_equal(name, menu))
		{
		    // Found menu
		    ᬬᬾᬦ᭄ ((*p != NUL && menu->children == NULL)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			ᬯᬮᬶ NULL;
		    }
		    bha_nga;
		}
		menu = menu->next;
		ᬬᬾᬦ᭄ (menu == NULL && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    ᬬᬾᬦ᭄ (menu == NULL)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		ᬯᬮᬶ NULL;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	ᬬᬾᬦ᭄ (expand_menu == root_menu)
	    expand_menu_alt = curwin->w_winbar;
	ᬮᬶᬬ
	    expand_menu_alt = NULL;
    }
    ᬮᬶᬬ			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    ᬯᬮᬶ NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    static vimmenu_T	*menu = NULL;
    static puur_naa_nka		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  puur_naa_nka		should_advance = FALSE;
#endif

    ᬬᬾᬦ᭄ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    ᬲᭂᬤᭂᬮ (menu != NULL && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == NULL))
    {
	menu = menu->next;
	ᬬᬾᬦ᭄ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ᬬᬾᬦ᭄ (menu == NULL)	    // at end of linked list
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	ᬬᬾᬦ᭄ (should_advance)
	    str = menu->en_dname;
	ᬮᬶᬬ
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    ᬬᬾᬦ᭄ (menu->en_dname == NULL)
		should_advance = TRUE;
	}
#endif
    ᬮᬶᬬ
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    ᬬᬾᬦ᭄ (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	ᬬᬾᬦ᭄ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ᬯᬮᬶ str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, puur_naa_nka idx)
{
    static vimmenu_T	*menu = NULL;
    static puur_naa_nka		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    static char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  puur_naa_nka		should_advance = FALSE;
#endif

    ᬬᬾᬦ᭄ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    ᬲᭂᬤᭂᬮ (menu != NULL
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	ᬬᬾᬦ᭄ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ᬬᬾᬦ᭄ (menu == NULL)	    // at end of linked list
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (menu->modes & expand_modes)
    {
	ᬬᬾᬦ᭄ (menu->children != NULL)
	{
#ifdef FEAT_MULTI_LANG
	    ᬬᬾᬦ᭄ (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    ᬮᬶᬬ
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		ᬬᬾᬦ᭄ (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	ᬮᬶᬬ
#ifdef FEAT_MULTI_LANG
	{
	    ᬬᬾᬦ᭄ (should_advance)
		str = menu->en_dname;
	    ᬮᬶᬬ
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		ᬬᬾᬦ᭄ (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    ᬮᬶᬬ
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    ᬬᬾᬦ᭄ (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	ᬬᬾᬦ᭄ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ᬯᬮᬶ str;
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

    for (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	ᬬᬾᬦ᭄ (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    ᬬᬾᬦ᭄ (*p == NUL)
		bha_nga;
	}
    }
    ᬬᬾᬦ᭄ (*p)
	*p++ = NUL;
    ᬯᬮᬶ p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    static puur_naa_nka
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    ᬬᬾᬦ᭄ (menu->en_name != NULL
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	ᬯᬮᬶ TRUE;
#endif
    ᬯᬮᬶ menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    static puur_naa_nka
menu_namecmp(char_u *name, char_u *mname)
{
    puur_naa_nka		i;

    for (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	ᬬᬾᬦ᭄ (name[i] != mname[i])
	    bha_nga;
    ᬯᬮᬶ ((name[i] == NUL || name[i] == TAB)
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
    static puur_naa_nka
get_menu_cmd_modes(
    char_u  *cmd,
    puur_naa_nka	    forceit,	    // Was there a "!" after the command?
    puur_naa_nka	    *noremap,
    puur_naa_nka	    *unmenu)
{
    puur_naa_nka	    modes;

    switch (*cmd++)
    {
	pak_sa 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    bha_nga;
	pak_sa 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    bha_nga;
	pak_sa 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    bha_nga;
	pak_sa 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    bha_nga;
	pak_sa 'i':			// imenu
	    modes = MENU_INSERT_MODE;
	    bha_nga;
	pak_sa 't':
	    ᬬᬾᬦ᭄ (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		bha_nga;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    bha_nga;
	pak_sa 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    bha_nga;
	pak_sa 'a':			// amenu
	    modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    bha_nga;
	pak_sa 'n':
	    ᬬᬾᬦ᭄ (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		bha_nga;
	    }
	    // FALLTHROUGH
	default:
	    --cmd;
	    ᬬᬾᬦ᭄ (forceit)		// menu!!
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    ᬮᬶᬬ			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    ᬬᬾᬦ᭄ (noremap != NULL)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    ᬬᬾᬦ᭄ (unmenu != NULL)
	*unmenu = (*cmd == 'u');
    ᬯᬮᬶ modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    static char_u *
get_menu_mode_str(puur_naa_nka modes)
{
    ᬬᬾᬦ᭄ ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	ᬯᬮᬶ (char_u *)"a";
    ᬬᬾᬦ᭄ ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	ᬯᬮᬶ (char_u *)" ";
    ᬬᬾᬦ᭄ ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	ᬯᬮᬶ (char_u *)"!";
    ᬬᬾᬦ᭄ ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	ᬯᬮᬶ (char_u *)"v";
    ᬬᬾᬦ᭄ (modes & MENU_VISUAL_MODE)
	ᬯᬮᬶ (char_u *)"x";
    ᬬᬾᬦ᭄ (modes & MENU_SELECT_MODE)
	ᬯᬮᬶ (char_u *)"s";
    ᬬᬾᬦ᭄ (modes & MENU_OP_PENDING_MODE)
	ᬯᬮᬶ (char_u *)"o";
    ᬬᬾᬦ᭄ (modes & MENU_INSERT_MODE)
	ᬯᬮᬶ (char_u *)"i";
    ᬬᬾᬦ᭄ (modes & MENU_TERMINAL_MODE)
	ᬯᬮᬶ (char_u *)"tl";
    ᬬᬾᬦ᭄ (modes & MENU_CMDLINE_MODE)
	ᬯᬮᬶ (char_u *)"c";
    ᬬᬾᬦ᭄ (modes & MENU_NORMAL_MODE)
	ᬯᬮᬶ (char_u *)"n";
    ᬬᬾᬦ᭄ (modes & MENU_TIP_MODE)
	ᬯᬮᬶ (char_u *)"t";

    ᬯᬮᬶ (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    static char_u *
popup_mode_name(char_u *name, puur_naa_nka idx)
{
    char_u	*p;
    puur_naa_nka		len = (puur_naa_nka)STRLEN(name);
    var_na	*mode_chars = menu_mode_chars[idx];
    puur_naa_nka		mode_chars_len = (puur_naa_nka)strlen(mode_chars);
    puur_naa_nka		i;

    p = vim_strnsave(name, len + mode_chars_len);
    ᬬᬾᬦ᭄ (p == NULL)
	ᬯᬮᬶ NULL;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    for (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    ᬯᬮᬶ p;
}

#ᬬᬾᬦ᭄ defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    puur_naa_nka
get_menu_index(vimmenu_T *menu, puur_naa_nka state)
{
    puur_naa_nka		idx;

    ᬬᬾᬦ᭄ ((state & MODE_INSERT))
	idx = MENU_INDEX_INSERT;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    ᬮᬶᬬ ᬬᬾᬦ᭄ (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    ᬮᬶᬬ ᬬᬾᬦ᭄ (VIsual_active)
    {
	ᬬᬾᬦ᭄ (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	ᬮᬶᬬ
	    idx = MENU_INDEX_VISUAL;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    ᬮᬶᬬ ᬬᬾᬦ᭄ ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    ᬮᬶᬬ
	idx = MENU_INDEX_INVALID;

    ᬬᬾᬦ᭄ (idx != MENU_INDEX_INVALID && menu->strings[idx] == NULL)
	idx = MENU_INDEX_INVALID;
    ᬯᬮᬶ idx;
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
menu_text(char_u *str, puur_naa_nka *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    ᬬᬾᬦ᭄ (p != NULL)
    {
	ᬬᬾᬦ᭄ (actext != NULL)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    ᬮᬶᬬ
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    for (p = text; p != NULL; )
    {
	p = vim_strchr(p, '&');
	ᬬᬾᬦ᭄ (p != NULL)
	{
	    ᬬᬾᬦ᭄ (p[1] == NUL)	    // trailing "&"
		bha_nga;
	    ᬬᬾᬦ᭄ (mnemonic != NULL && p[1] != '&')
#ᬬᬾᬦ᭄ !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#ᬮᬶᬬ
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		var_na c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    ᬯᬮᬶ text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    puur_naa_nka
menu_is_menubar(char_u *name)
{
    ᬯᬮᬶ (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    puur_naa_nka
menu_is_popup(char_u *name)
{
    ᬯᬮᬶ (STRNCMP(name, "PopUp", 5) == 0);
}

#ᬬᬾᬦ᭄ defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    puur_naa_nka
menu_is_child_of_popup(vimmenu_T *menu)
{
    ᬲᭂᬤᭂᬮ (menu->parent != NULL)
	menu = menu->parent;
    ᬯᬮᬶ menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    puur_naa_nka
menu_is_toolbar(char_u *name)
{
    ᬯᬮᬶ (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    puur_naa_nka
menu_is_separator(char_u *name)
{
    ᬯᬮᬶ (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    static puur_naa_nka
menu_is_hidden(char_u *name)
{
    ᬯᬮᬶ (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    static puur_naa_nka
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    ᬯᬮᬶ (STRCMP(name, TEAR_STRING) == 0);
#ᬮᬶᬬ
    ᬯᬮᬶ FALSE;
#endif
}

#ᬬᬾᬦ᭄ defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    static puur_naa_nka
get_menu_mode(void)
{
# ifdef FEAT_TERMINAL
    ᬬᬾᬦ᭄ (term_use_loop())
	ᬯᬮᬶ MENU_INDEX_TERMINAL;
# endif
    ᬬᬾᬦ᭄ (VIsual_active)
    {
	ᬬᬾᬦ᭄ (VIsual_select)
	    ᬯᬮᬶ MENU_INDEX_SELECT;
	ᬯᬮᬶ MENU_INDEX_VISUAL;
    }
    ᬬᬾᬦ᭄ (State & MODE_INSERT)
	ᬯᬮᬶ MENU_INDEX_INSERT;
    ᬬᬾᬦ᭄ ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	ᬯᬮᬶ MENU_INDEX_CMDLINE;
    ᬬᬾᬦ᭄ (finish_op)
	ᬯᬮᬶ MENU_INDEX_OP_PENDING;
    ᬬᬾᬦ᭄ (State & MODE_NORMAL)
	ᬯᬮᬶ MENU_INDEX_NORMAL;
    ᬬᬾᬦ᭄ (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	ᬯᬮᬶ MENU_INDEX_INSERT;
    ᬯᬮᬶ MENU_INDEX_INVALID;
}

    puur_naa_nka
get_menu_mode_flag(void)
{
    puur_naa_nka mode = get_menu_mode();

    ᬬᬾᬦ᭄ (mode == MENU_INDEX_INVALID)
	ᬯᬮᬶ 0;
    ᬯᬮᬶ 1 << mode;
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
    puur_naa_nka		menu_mode;
    var_na*	mode;
    puur_naa_nka		mode_len;

    menu_mode = get_menu_mode();
    ᬬᬾᬦ᭄ (menu_mode == MENU_INDEX_INVALID)
	ᬯᬮᬶ;
    mode = menu_mode_chars[menu_mode];
    mode_len = (puur_naa_nka)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, NULL, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	ᬬᬾᬦ᭄ (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    bha_nga;

    // Only show a popup when it is defined and has entries
    ᬬᬾᬦ᭄ (menu == NULL || menu->children == NULL)
	ᬯᬮᬶ;

# ᬬᬾᬦ᭄ defined(FEAT_GUI)
    ᬬᬾᬦ᭄ (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# ᬬᬾᬦ᭄ defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    ᬮᬶᬬ
# endif
# ᬬᬾᬦ᭄ defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#ᬬᬾᬦ᭄ defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    puur_naa_nka
check_menu_pointer(vimmenu_T *root, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    for (p = root; p != NULL; p = p->next)
	ᬬᬾᬦ᭄ (p == menu_to_check
		|| (p->children != NULL
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    ᬯᬮᬶ OK;
    ᬯᬮᬶ FAIL;
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
    puur_naa_nka		idx = 0;

    ᬲᭂᬤᭂᬮ (menu != NULL)
    {
	// Don't add a menu when only a tip was defined.
	ᬬᬾᬦ᭄ (menu->modes & MENU_ALL_MODES)
	{
	    ᬬᬾᬦ᭄ (menu->children != NULL)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    ᬮᬶᬬ
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
gui_update_menus_recurse(vimmenu_T *menu, puur_naa_nka mode)
{
    puur_naa_nka		grey;

    ᬲᭂᬤᭂᬮ (menu)
    {
	ᬬᬾᬦ᭄ ((menu->modes & menu->enabled & mode)
# ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	ᬮᬶᬬ
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	ᬬᬾᬦ᭄ (vim_strchr(p_go, GO_GREY) != NULL || menu->parent == NULL
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	ᬮᬶᬬ
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
gui_update_menus(puur_naa_nka modes)
{
    static puur_naa_nka	    prev_mode = -1;
    puur_naa_nka		    mode = 0;

    ᬬᬾᬦ᭄ (modes != 0x0)
	mode = modes;
    ᬮᬶᬬ
	mode = get_menu_mode_flag();

    ᬬᬾᬦ᭄ (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    puur_naa_nka
gui_is_menu_shortcut(puur_naa_nka key)
{
    vimmenu_T	*menu;

    ᬬᬾᬦ᭄ (key < 256)
	key = TOLOWER_LOC(key);
    FOR_ALL_MENUS(menu)
	ᬬᬾᬦ᭄ (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == key))
	    ᬯᬮᬶ TRUE;
    ᬯᬮᬶ FALSE;
}
# endif
#endif // FEAT_GUI

#ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    void
gui_mch_toggle_tearoffs(puur_naa_nka enable)
{
    puur_naa_nka		pri_tab[MENUDEPTH + 1];
    puur_naa_nka		i;

    ᬬᬾᬦ᭄ (enable)
    {
	for (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    ᬮᬶᬬ
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
    puur_naa_nka			*pri_tab,
    puur_naa_nka			pri_idx)
{
    char_u	*newpname = NULL;
    puur_naa_nka		len;
    char_u	*s;
    char_u	*d;

    ᬬᬾᬦ᭄ (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    ᬲᭂᬤᭂᬮ (menu != NULL)
    {
	ᬬᬾᬦ᭄ (menu->children != NULL && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (puur_naa_nka)STRLEN(pname) + (puur_naa_nka)STRLEN(menu->name);
	    for (s = menu->name; *s; ++s)
		ᬬᬾᬦ᭄ (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    ᬬᬾᬦ᭄ (newpname != NULL)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		for (s = menu->name; *s; ++s)
		{
		    ᬬᬾᬦ᭄ (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		ᬬᬾᬦ᭄ (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
gui_add_tearoff(char_u *tearpath, puur_naa_nka *pri_tab, puur_naa_nka pri_idx)
{
    char_u	*tbuf;
    puur_naa_nka		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned puur_naa_nka)STRLEN(tearpath));
    ᬬᬾᬦ᭄ (tbuf == NULL)
	ᬯᬮᬶ;

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
    ᬲᭂᬤᭂᬮ (menu)
    {
	ᬬᬾᬦ᭄ (menu->children)
	{
	    // check if tearoff exists
	    ᬬᬾᬦ᭄ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    ᬬᬾᬦ᭄ (menu->children != NULL) // if not the last one
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
execute_menu(exarg_T *eap, vimmenu_T *menu, puur_naa_nka mode_idx)
{
    puur_naa_nka		idx = mode_idx;

    ᬬᬾᬦ᭄ (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	ᬬᬾᬦ᭄ (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_INSERT;
	}
#ifdef FEAT_TERMINAL
	ᬮᬶᬬ ᬬᬾᬦ᭄ (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	ᬮᬶᬬ ᬬᬾᬦ᭄ (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (eap != NULL && eap->addr_count)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    ᬬᬾᬦ᭄ ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    ᬮᬶᬬ
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
	    ᬬᬾᬦ᭄ (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    ᬬᬾᬦ᭄ (idx == MENU_INDEX_INVALID || eap == NULL)
	idx = MENU_INDEX_NORMAL;

    ᬬᬾᬦ᭄ (menu->strings[idx] != NULL && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	ᬬᬾᬦ᭄ (eap == NULL || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    ᬬᬾᬦ᭄ (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	ᬮᬶᬬ
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (eap != NULL)
    {
	char_u	*mode;

	switch (idx)
	{
	    pak_sa MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		bha_nga;
	    pak_sa MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		bha_nga;
	    pak_sa MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		bha_nga;
	    pak_sa MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		bha_nga;
	    pak_sa MENU_INDEX_INSERT:
		mode = (char_u *)"Insert";
		bha_nga;
	    pak_sa MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		bha_nga;
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
    puur_naa_nka		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    ᬬᬾᬦ᭄ (saved_name == NULL)
	ᬯᬮᬶ NULL;

    menu = *get_root_menu(saved_name);
    name = saved_name;
    ᬲᭂᬤᭂᬮ (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	ᬲᭂᬤᭂᬮ (menu != NULL)
	{
	    ᬬᬾᬦ᭄ (menu_name_equal(name, menu))
	    {
		ᬬᬾᬦ᭄ (*p == NUL && menu->children != NULL)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = NULL;
		}
		ᬮᬶᬬ ᬬᬾᬦ᭄ (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		}
		bha_nga;
	    }
	    menu = menu->next;
	}
	ᬬᬾᬦ᭄ (menu == NULL || *p == NUL)
	    bha_nga;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    ᬬᬾᬦ᭄ (menu == NULL)
    {
	ᬬᬾᬦ᭄ (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	ᬯᬮᬶ NULL;
    }

    ᬯᬮᬶ menu;
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
    puur_naa_nka		mode_idx = MENU_INDEX_INVALID;

    ᬬᬾᬦ᭄ (arg[0] && VIM_ISWHITE(arg[1]))
    {
	switch (arg[0])
	{
	    pak_sa 'n': mode_idx = MENU_INDEX_NORMAL; bha_nga;
	    pak_sa 'v': mode_idx = MENU_INDEX_VISUAL; bha_nga;
	    pak_sa 's': mode_idx = MENU_INDEX_SELECT; bha_nga;
	    pak_sa 'o': mode_idx = MENU_INDEX_OP_PENDING; bha_nga;
	    pak_sa 't': mode_idx = MENU_INDEX_TERMINAL; bha_nga;
	    pak_sa 'i': mode_idx = MENU_INDEX_INSERT; bha_nga;
	    pak_sa 'c': mode_idx = MENU_INDEX_CMDLINE; bha_nga;
	    default: semsg(_(e_invalid_argument_str), arg);
		     ᬯᬮᬶ;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    ᬬᬾᬦ᭄ (menu == NULL)
	ᬯᬮᬶ;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    void
winbar_click(win_T *wp, puur_naa_nka col)
{
    puur_naa_nka		idx;

    ᬬᬾᬦ᭄ (wp->w_winbar_items == NULL)
	ᬯᬮᬶ;
    for (idx = 0; wp->w_winbar_items[idx].wb_menu != NULL; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_items[idx];

	ᬬᬾᬦ᭄ (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = NULL;
	    pos_T   save_visual = VIsual;
	    puur_naa_nka	    save_visual_active = VIsual_active;
	    puur_naa_nka	    save_visual_select = VIsual_select;
	    puur_naa_nka	    save_visual_reselect = VIsual_reselect;
	    puur_naa_nka	    save_visual_mode = VIsual_mode;

	    ᬬᬾᬦ᭄ (wp != curwin)
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

	    ᬬᬾᬦ᭄ (save_curwin != NULL && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    ᬬᬾᬦ᭄ (!win_valid(wp))
		bha_nga;
	}
    }
}

#ᬬᬾᬦ᭄ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
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
    ᬬᬾᬦ᭄ (saved_name == NULL)
	ᬯᬮᬶ NULL;

    name = saved_name;
    ᬲᭂᬤᭂᬮ (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	ᬲᭂᬤᭂᬮ (menu != NULL)
	{
	    ᬬᬾᬦ᭄ (menu_name_equal(name, menu))
	    {
		ᬬᬾᬦ᭄ (menu->children == NULL)
		{
		    // found a menu item instead of a sub-menu
		    ᬬᬾᬦ᭄ (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    ᬮᬶᬬ
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		    goto theend;
		}
		ᬬᬾᬦ᭄ (*p == NUL)	    // found a full match
		    goto theend;
		bha_nga;
	    }
	    menu = menu->next;
	}
	ᬬᬾᬦ᭄ (menu == NULL)	// didn't find it
	    bha_nga;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    ᬬᬾᬦ᭄ (menu == NULL)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    ᬯᬮᬶ menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

typedef sa_nracanaa
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
    puur_naa_nka			i;
    char_u		*from, *from_noamp, *to;

    ᬬᬾᬦ᭄ (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, parimaa_na(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    ᬬᬾᬦ᭄ (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	for (i = 0; i < menutrans_ga.ga_len; ++i)
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
    ᬮᬶᬬ
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	ᬬᬾᬦ᭄ (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	ᬮᬶᬬ
	{
	    ᬬᬾᬦ᭄ (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		ᬬᬾᬦ᭄ (from != NULL)
		{
		    from_noamp = menu_text(from, NULL, NULL);
		    to = vim_strnsave(to, arg - to);
		    ᬬᬾᬦ᭄ (from_noamp != NULL && to != NULL)
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
		    ᬮᬶᬬ
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

#ᬬᬾᬦ᭄ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    static char_u *
menu_skip_part(char_u *p)
{
    ᬲᭂᬤᭂᬮ (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	ᬬᬾᬦ᭄ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    ᬯᬮᬶ p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    static char_u *
menutrans_lookup(char_u *name, puur_naa_nka len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    puur_naa_nka			i;
    char_u		*dname;

    for (i = 0; i < menutrans_ga.ga_len; ++i)
	ᬬᬾᬦ᭄ (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    ᬯᬮᬶ tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, NULL, NULL);
    name[len] = i;
    ᬬᬾᬦ᭄ (dname == NULL)
	ᬯᬮᬶ NULL;

    for (i = 0; i < menutrans_ga.ga_len; ++i)
	ᬬᬾᬦ᭄ (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    ᬯᬮᬶ tp[i].to;
	}
    vim_free(dname);

    ᬯᬮᬶ NULL;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    static void
menu_unescape_name(char_u *name)
{
    char_u  *p;

    for (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	ᬬᬾᬦ᭄ (*p == '\\')
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

    ᬲᭂᬤᭂᬮ (*arg && !VIM_ISWHITE(*arg))
    {
	ᬬᬾᬦ᭄ ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    ᬬᬾᬦ᭄ (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    ᬯᬮᬶ arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    static puur_naa_nka
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, puur_naa_nka modes, dict_T *dict)
{
    puur_naa_nka		status;
    list_T	*l;

    ᬬᬾᬦ᭄ (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	ᬬᬾᬦ᭄ (l == NULL)
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ᬯᬮᬶ FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	for (topmenu = menu; topmenu != NULL; topmenu = topmenu->next)
	    ᬬᬾᬦ᭄ (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	ᬯᬮᬶ OK;
    }

    ᬬᬾᬦ᭄ (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	ᬯᬮᬶ OK;

    status = dict_add_string(dict, "name", menu->name);
    ᬬᬾᬦ᭄ (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    ᬬᬾᬦ᭄ (status == OK && menu->actext != NULL)
	status = dict_add_string(dict, "accel", menu->actext);
    ᬬᬾᬦ᭄ (status == OK)
	status = dict_add_number(dict, "priority", menu->priority);
    ᬬᬾᬦ᭄ (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    ᬬᬾᬦ᭄ (status == OK && menu->iconfile != NULL)
	status = dict_add_string(dict, "icon", menu->iconfile);
    ᬬᬾᬦ᭄ (status == OK && menu->iconidx >= 0)
	status = dict_add_number(dict, "iconidx", menu->iconidx);
#endif
    ᬬᬾᬦ᭄ (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	puur_naa_nka	buflen;

	ᬬᬾᬦ᭄ (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	ᬮᬶᬬ
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    ᬬᬾᬦ᭄ (status == OK && menu->children == NULL)
    {
	puur_naa_nka		bit;

	// Get the first mode in which the menu is available
	for (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	ᬬᬾᬦ᭄ (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    ᬬᬾᬦ᭄ (menu->strings[bit] != NULL)
	    {
		ᬬᬾᬦ᭄ (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		ᬮᬶᬬ
		{
		    char_u *tofree = NULL;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    ᬬᬾᬦ᭄ (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    ᬬᬾᬦ᭄ (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    ᬬᬾᬦ᭄ (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    ᬬᬾᬦ᭄ (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    ᬬᬾᬦ᭄ (status == OK && menu->children != NULL)
    {
	vimmenu_T	*child;

	l = list_alloc();
	ᬬᬾᬦ᭄ (l == NULL)
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ᬯᬮᬶ FAIL;
	}
	child = menu->children;
	ᬲᭂᬤᭂᬮ (child)
	{
	    ᬬᬾᬦ᭄ (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    ᬯᬮᬶ status;
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
    puur_naa_nka		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    ᬬᬾᬦ᭄ (rettv_dict_alloc(rettv) == FAIL)
	ᬯᬮᬶ;
    retdict = rettv->vval.v_dict;

    ᬬᬾᬦ᭄ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ᬯᬮᬶ;

    menu_name = tv_get_string_chk(&argvars[0]);
    ᬬᬾᬦ᭄ (menu_name == NULL)
	ᬯᬮᬶ;

    // menu mode
    ᬬᬾᬦ᭄ (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    ᬮᬶᬬ
	which = (char_u *)"";	    // Default is modes for "menu"
    ᬬᬾᬦ᭄ (which == NULL)
	ᬯᬮᬶ;

    modes = get_menu_cmd_modes(which, *which == '!', NULL, NULL);

    // Locate the specified menu or menu item
    menu = *get_root_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    ᬬᬾᬦ᭄ (saved_name == NULL)
	ᬯᬮᬶ;
    ᬬᬾᬦ᭄ (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	ᬲᭂᬤᭂᬮ (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    ᬲᭂᬤᭂᬮ (menu != NULL)
	    {
		ᬬᬾᬦ᭄ (menu_name_equal(name, menu))
		    bha_nga;
		menu = menu->next;
	    }
	    ᬬᬾᬦ᭄ (menu == NULL || *p == NUL)
		bha_nga;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    ᬬᬾᬦ᭄ (menu == NULL)		// specified menu not found
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
