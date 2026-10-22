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

#အကယ်၍ defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
static ကိန်းပြည့် add_menu_path(char_u *, vimmenu_T *, ကိန်းပြည့် *, char_u *, ကိန်းပြည့်);
#မဟုတ်ပါက
static ကိန်းပြည့် add_menu_path(char_u *, vimmenu_T *, ကိန်းပြည့် *, char_u *);
#endif
static ကိန်းပြည့် menu_nable_recurse(vimmenu_T *menu, char_u *name, ကိန်းပြည့် modes, ကိန်းပြည့် enable);
static ကိန်းပြည့် remove_menu(vimmenu_T **, char_u *, ကိန်းပြည့်, ကိန်းပြည့် silent);
static void free_menu(vimmenu_T **menup);
static void free_menu_string(vimmenu_T *, ကိန်းပြည့်);
static ကိန်းပြည့် show_menus(char_u *, ကိန်းပြည့်);
static void show_menus_recursive(vimmenu_T *, ကိန်းပြည့်, ကိန်းပြည့်);
static char_u *menu_name_skip(char_u *name);
static ကိန်းပြည့် menu_name_equal(char_u *name, vimmenu_T *menu);
static ကိန်းပြည့် menu_namecmp(char_u *name, char_u *mname);
static ကိန်းပြည့် get_menu_cmd_modes(char_u *, ကိန်းပြည့်, ကိန်းပြည့် *, ကိန်းပြည့် *);
static char_u *popup_mode_name(char_u *name, ကိန်းပြည့် idx);
static char_u *menu_text(char_u *text, ကိန်းပြည့် *mnemonic, char_u **actext);

#အကယ်၍ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
static void gui_create_tearoffs_recurse(vimmenu_T *menu, const char_u *pname, ကိန်းပြည့် *pri_tab, ကိန်းပြည့် pri_idx);
static void gui_add_tearoff(char_u *tearpath, ကိန်းပြည့် *pri_tab, ကိန်းပြည့် pri_idx);
static void gui_destroy_tearoffs_recurse(vimmenu_T *menu);
static ကိန်းပြည့် s_tearoffs = FALSE;
#endif

static ကိန်းပြည့် menu_is_hidden(char_u *name);
static ကိန်းပြည့် menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
static ကိန်းပြည့် menus_locked = 0;

#အကယ်၍ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
static char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
static char_u *menutrans_lookup(char_u *name, ကိန်းပြည့် len);
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
    static ကိန်းပြည့်
menu_is_winbar(char_u *name)
{
    ပြန်ပေး (STRNCMP(name, "WinBar", 6) == 0);
}

    ကိန်းပြည့်
winbar_height(win_T *wp)
{
    အကယ်၍ (wp->w_winbar != NULL && wp->w_winbar->children != NULL)
	ပြန်ပေး 1;
    ပြန်ပေး 0;
}

    static vimmenu_T **
get_root_menu(char_u *name)
{
    အကယ်၍ (menu_is_winbar(name))
	ပြန်ပေး &curwin->w_winbar;
    ပြန်ပေး &root_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    static ကိန်းပြည့်
is_menus_locked(void)
{
    အကယ်၍ (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    void
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    ကိန်းပြည့်		modes;
    char_u	*map_to;
    ကိန်းပြည့်		noremap;
    ကိန်းပြည့်		silent = FALSE;
    ကိန်းပြည့်		special = FALSE;
    ကိန်းပြည့်		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    ကိန်းပြည့်		i;
#အကယ်၍ defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    ကိန်းပြည့်		old_menu_height;
# အကယ်၍ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    ကိန်းပြည့်		old_toolbar_height;
# endif
#endif
    ကိန်းပြည့်		pri_tab[MENUDEPTH + 1];
    ကိန်းပြည့်		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = NULL;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**root_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    အတွက် (;;)
    {
	အကယ်၍ (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	အကယ်၍ (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    continue;
	}
	အကယ်၍ (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    continue;
	}
	bha_nga;
    }


    // Locate an optional "icon=filename" argument.
    အကယ်၍ (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	နေစဉ် (*arg != NUL && *arg != ' ')
	{
	    အကယ်၍ (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	အကယ်၍ (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    အတွက် (p = arg; *p; ++p)
	အကယ်၍ (!VIM_ISDIGIT(*p) && *p != '.')
	    bha_nga;
    အကယ်၍ (VIM_ISWHITE(*p))
    {
	အတွက် (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    အကယ်၍ (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    အကယ်၍ (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    မဟုတ်ပါက အကယ်၍ (eap->addr_count && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    မဟုတ်ပါက
	i = 0;
    နေစဉ် (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    အကယ်၍ (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    မဟုတ်ပါက အကယ်၍ (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    အကယ်၍ (*arg == NUL)
    {
	show_menus(arg, modes);
	ပြန်ပေး;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    အကယ်၍ (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	အကယ်၍ (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    အကယ်၍ (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		အကယ်၍ (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((var_na *)menu_path + 7);
		    အကယ်၍ (menuarg.iconidx >= (ကိန်းပြည့်)TOOLBAR_NAME_COUNT)
			menuarg.iconidx = -1;
		    မဟုတ်ပါက
			menuarg.icon_builtin = TRUE;
		}
	    }
	    မဟုတ်ပါက
	    {
		အတွက် (i = 0; i < (ကိန်းပြည့်)TOOLBAR_NAME_COUNT; ++i)
		    အကယ်၍ (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
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
    အကယ်၍ (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	goto theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    အကယ်၍ (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	goto theend;
    }
    မဟုတ်ပါက အကယ်၍ (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	goto theend;
    }
#အကယ်၍ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# အကယ်၍ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    root_menu_ptr = get_root_menu(menu_path);
    အကယ်၍ (root_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    အကယ်၍ (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	အကယ်၍ (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	အကယ်၍ (menu_is_popup(menu_path))
	{
	    အတွက် (i = 0; i < MENU_INDEX_TIP; ++i)
		အကယ်၍ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    အကယ်၍ (p != NULL)
		    {
			menu_nable_recurse(*root_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*root_menu_ptr, menu_path, modes, enable);
    }
    မဟုတ်ပါက အကယ်၍ (unmenu)
    {
	အကယ်၍ (is_menus_locked())
	    goto theend;

	/*
	 * Delete menu(s).
	 */
	အကယ်၍ (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	အကယ်၍ (menu_is_popup(menu_path))
	{
	    အတွက် (i = 0; i < MENU_INDEX_TIP; ++i)
		အကယ်၍ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    အကယ်၍ (p != NULL)
		    {
			remove_menu(root_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(root_menu_ptr, menu_path, modes, FALSE);
    }
    မဟုတ်ပါက
    {
	အကယ်၍ (is_menus_locked())
	    goto theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	အကယ်၍ (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = NULL;
	}
	မဟုတ်ပါက အကယ်၍ (modes & MENU_TIP_MODE)
	    map_buf = NULL;	// Menu tips are plain text.
	မဟုတ်ပါက
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
	အကယ်၍ (menu_is_popup(menu_path))
	{
	    အတွက် (i = 0; i < MENU_INDEX_TIP; ++i)
		အကယ်၍ (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    အကယ်၍ (p != NULL)
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

#အကယ်၍ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    အကယ်၍ (gui.in_use
	    && (gui.menu_height != old_menu_height
# အကယ်၍ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    အကယ်၍ (root_menu_ptr == &curwin->w_winbar)
    {
	ကိန်းပြည့် h = winbar_height(curwin);

	အကယ်၍ (h != curwin->w_winbar_height)
	{
	    အကယ်၍ (h == 0)
		++curwin->w_height;
	    မဟုတ်ပါက အကယ်၍ (curwin->w_height > 0)
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
    static ကိန်းပြည့်
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    ကိန်းပြည့်		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , ကိန်းပြည့်	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    ကိန်းပြည့်		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = NULL;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		d;
#ifdef FEAT_GUI
    ကိန်းပြည့်		idx;
    ကိန်းပြည့်		new_idx;
#endif
    ကိန်းပြည့်		pri_idx = 0;
    ကိန်းပြည့်		old_modes = 0;
    ကိန်းပြည့်		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = NULL;
#endif
    vimmenu_T	**root_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    အကယ်၍ (path_name == NULL)
	ပြန်ပေး FAIL;
    root_menu_ptr = get_root_menu(menu_path);
    menup = root_menu_ptr;
    parent = NULL;
    name = path_name;
    နေစဉ် (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (ကိန်းပြည့်)STRLEN(name));
	အကယ်၍ (map_to != NULL)
	{
	    en_name = name;
	    name = map_to;
	}
	မဟုတ်ပါက
	    en_name = NULL;
#endif
	dname = menu_text(name, NULL, NULL);
	အကယ်၍ (dname == NULL)
	    goto erret;
	အကယ်၍ (*dname == NUL)
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
	နေစဉ် (menu != NULL)
	{
	    အကယ်၍ (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		အကယ်၍ (*next_name == NUL && menu->children != NULL)
		{
		    အကယ်၍ (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    goto erret;
		}
		အကယ်၍ (*next_name != NUL && menu->children == NULL
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    အကယ်၍ (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    goto erret;
		}
		bha_nga;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    အကယ်၍ (parent != NULL || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		အကယ်၍ (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	အကယ်၍ (menu == NULL)
	{
	    အကယ်၍ (*next_name == NUL && parent == NULL)
	    {
		emsg(_(e_must_not_add_menu_items_directly_to_menu_bar));
		goto erret;
	    }

	    အကယ်၍ (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		goto erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    အကယ်၍ (menu == NULL)
		goto erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    အကယ်၍ (en_name != NULL)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, NULL, NULL);
	    }
	    မဟုတ်ပါက
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
	    အကယ်၍ (*next_name == NUL && menuarg->iconfile != NULL)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#အကယ်၍ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    အကယ်၍ (parent != NULL && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	မဟုတ်ပါက
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    အကယ်၍ (addtearoff)
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
	အကယ်၍ ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    အကယ်၍ (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		အကယ်၍ (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		မဟုတ်ပါက
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# အကယ်၍ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    အကယ်၍ (	addtearoff
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
		အကယ်၍ (tearpath != NULL)
		{
		    char_u  *s;
		    ကိန်းပြည့်	    len;

		    STRCPY(tearpath, menu_path);
		    len = (ကိန်းပြည့်)(next_name - path_name - 1);
		    အတွက် (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			အကယ်၍ ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	အကယ်၍ (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    အကယ်၍ (sys_menu)
	modes &= ~old_modes;

    အကယ်၍ (menu != NULL && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == NULL) ? NULL : vim_strsave(call_data);

	// loop over all modes, may add more than one
	အတွက် (i = 0; i < MENU_MODES; ++i)
	{
	    အကယ်၍ (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		အကယ်၍ (amenu && call_data != NULL && *call_data != NUL
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

		အကယ်၍ (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    အကယ်၍ (menu->strings[i] != NULL)
		    {
			menu->strings[i][0] = c;
			အကယ်၍ (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			မဟုတ်ပါက
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			အကယ်၍ (c == Ctrl_C)
			{
			    ကိန်းပြည့်	    len = (ကိန်းပြည့်)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		မဟုတ်ပါက
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#အကယ်၍ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	အကယ်၍ (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    ပြန်ပေး OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    နေစဉ် (parent != NULL && parent->children == NULL)
    {
	အကယ်၍ (parent->parent == NULL)
	    menup = root_menu_ptr;
	မဟုတ်ပါက
	    menup = &parent->parent->children;
	အတွက် ( ; *menup != NULL && *menup != parent; menup = &((*menup)->next))
	    ;
	အကယ်၍ (*menup == NULL) // safety check
	    bha_nga;
	parent = parent->parent;
	free_menu(menup);
    }
    ပြန်ပေး FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    static ကိန်းပြည့်
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    ကိန်းပြည့်		modes,
    ကိန်းပြည့်		enable)
{
    char_u	*p;

    အကယ်၍ (menu == NULL)
	ပြန်ပေး OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    နေစဉ် (menu != NULL)
    {
	အကယ်၍ (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    အကယ်၍ (*p != NUL)
	    {
		အကယ်၍ (menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    ပြန်ပေး FAIL;
		}
		အကယ်၍ (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    ပြန်ပေး FAIL;
	    }
	    မဟုတ်ပါက
		အကယ်၍ (enable)
		    menu->enabled |= modes;
		မဟုတ်ပါက
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    အကယ်၍ (*name != NUL && *name != '*')
		bha_nga;
	}
	menu = menu->next;
    }
    အကယ်၍ (*name != NUL && *name != '*' && menu == NULL)
    {
	semsg(_(e_no_menu_str), name);
	ပြန်ပေး FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    ပြန်ပေး OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    static ကိန်းပြည့်
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    ကိန်းပြည့်		modes,
    ကိန်းပြည့်		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    အကယ်၍ (*menup == NULL)
	ပြန်ပေး OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    နေစဉ် ((menu = *menup) != NULL)
    {
	အကယ်၍ (*name == NUL || menu_name_equal(name, menu))
	{
	    အကယ်၍ (*p != NUL && menu->children == NULL)
	    {
		အကယ်၍ (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		ပြန်ပေး FAIL;
	    }
	    အကယ်၍ ((menu->modes & modes) != 0x0)
	    {
#အကယ်၍ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		အကယ်၍ (*p == NUL && modes == MENU_ALL_MODES)
		{
		    အကယ်၍ (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		အကယ်၍ (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    ပြန်ပေး FAIL;
	    }
	    မဟုတ်ပါက အကယ်၍ (*name != NUL)
	    {
		အကယ်၍ (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		ပြန်ပေး FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    အကယ်၍ (*name != NUL)
		bha_nga;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    အကယ်၍ (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    အကယ်၍ ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    မဟုတ်ပါက
		menup = &menu->next;
	}
	မဟုတ်ပါက
	    menup = &menu->next;
    }
    အကယ်၍ (*name != NUL)
    {
	အကယ်၍ (menu == NULL)
	{
	    အကယ်၍ (!silent)
		semsg(_(e_no_menu_str), name);
	    ပြန်ပေး FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#အကယ်၍ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	အကယ်၍ ((s_tearoffs) && (menu->children != NULL)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	မဟုတ်ပါက
#endif
	    child = menu->children;
	အတွက် ( ; child != NULL; child = child->next)
	    menu->modes |= child->modes;
	အကယ်၍ (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#အကယ်၍ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    အကယ်၍ (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	အကယ်၍ ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#အကယ်၍ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    အကယ်၍ (s_tearoffs && menu->children != NULL) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    ပြန်ပေး OK;
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
    ကိန်းပြည့်		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    အကယ်၍ (gui.in_use)
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
    အတွက် (i = 0; i < MENU_MODES; i++)
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
free_menu_string(vimmenu_T *menu, ကိန်းပြည့် idx)
{
    ကိန်းပြည့်		count = 0;
    ကိန်းပြည့်		i;

    အတွက် (i = 0; i < MENU_MODES; i++)
	အကယ်၍ (menu->strings[i] == menu->strings[idx])
	    count++;
    အကယ်၍ (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = NULL;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    static ကိန်းပြည့်
show_menus(char_u *path_name, ကိန်းပြည့် modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = NULL;

    name = path_name = vim_strsave(path_name);
    အကယ်၍ (path_name == NULL)
	ပြန်ပေး FAIL;
    menu = *get_root_menu(path_name);

    // First, find the (sub)menu with the given name
    နေစဉ် (*name)
    {
	p = menu_name_skip(name);
	နေစဉ် (menu != NULL)
	{
	    အကယ်၍ (menu_name_equal(name, menu))
	    {
		// Found menu
		အကယ်၍ (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    ပြန်ပေး FAIL;
		}
		မဟုတ်ပါက အကယ်၍ ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    ပြန်ပေး FAIL;
		}
		bha_nga;
	    }
	    menu = menu->next;
	}
	အကယ်၍ (menu == NULL)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    ပြန်ပေး FAIL;
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
    ပြန်ပေး OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    static void
show_menus_recursive(vimmenu_T *menu, ကိန်းပြည့် modes, ကိန်းပြည့် depth)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		bit;

    အကယ်၍ (menu != NULL && (menu->modes & modes) == 0x0)
	ပြန်ပေး;

    အကယ်၍ (menu != NULL)
    {
	msg_putchar('\n');
	အကယ်၍ (got_int)		// "q" hit for "--more--"
	    ပြန်ပေး;
	အတွက် (i = 0; i < depth; i++)
	    msg_puts("  ");
	အကယ်၍ (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    msg_puts(" ");
	}
				// Same highlighting as for directories!?
	msg_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    အကယ်၍ (menu != NULL && menu->children == NULL)
    {
	အတွက် (bit = 0; bit < MENU_MODES; bit++)
	    အကယ်၍ ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		အကယ်၍ (got_int)		// "q" hit for "--more--"
		    ပြန်ပေး;
		အတွက် (i = 0; i < depth + 2; i++)
		    msg_puts("  ");
		msg_puts(menu_mode_chars[bit]);
		အကယ်၍ (menu->noremap[bit] == REMAP_NONE)
		    msg_putchar('*');
		မဟုတ်ပါက အကယ်၍ (menu->noremap[bit] == REMAP_SCRIPT)
		    msg_putchar('&');
		မဟုတ်ပါက
		    msg_putchar(' ');
		အကယ်၍ (menu->silent[bit])
		    msg_putchar('s');
		မဟုတ်ပါက
		    msg_putchar(' ');
		အကယ်၍ ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    msg_putchar('-');
		မဟုတ်ပါက
		    msg_putchar(' ');
		msg_puts(" ");
		အကယ်၍ (*menu->strings[bit] == NUL)
		    msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
		မဟုတ်ပါက
		    msg_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    မဟုတ်ပါက
    {
	အကယ်၍ (menu == NULL)
	{
	    menu = root_menu;
	    depth--;
	}
	မဟုတ်ပါက
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	အတွက် (; menu != NULL && !got_int; menu = menu->next)
	    အကယ်၍ (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
static vimmenu_T	*expand_menu = NULL;
static vimmenu_T	*expand_menu_alt = NULL;
static ကိန်းပြည့်		expand_modes = 0x0;
static ကိန်းပြည့်		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    ကိန်းပြည့်		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = NULL;
    char_u	*name;
    ကိန်းပြည့်		unmenu;
    vimmenu_T	*menu;
    ကိန်းပြည့်		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    အတွက် (p = arg; *p; ++p)
	အကယ်၍ (!VIM_ISDIGIT(*p) && *p != '.')
	    bha_nga;

    အကယ်၍ (!VIM_ISWHITE(*p))
    {
	အကယ်၍ (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	မဟုတ်ပါက အကယ်၍ (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	မဟုတ်ပါက
	    p = arg;
    }

    နေစဉ် (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    အတွက် (; *p && !VIM_ISWHITE(*p); ++p)
    {
	အကယ်၍ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	မဟုတ်ပါက အကယ်၍ (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    အကယ်၍ (expand_menus && VIM_ISWHITE(*p))
	ပြန်ပေး NULL;	// TODO: check for next command?
    အကယ်၍ (*p == NUL)		// Complete the menu name
    {
	ကိန်းပြည့် try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, NULL, &unmenu);
	အကယ်၍ (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = root_menu;
	အကယ်၍ (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    အကယ်၍ (path_name == NULL)
		ပြန်ပေး NULL;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	နေစဉ် (name != NULL && *name)
	{
	    p = menu_name_skip(name);
	    နေစဉ် (menu != NULL)
	    {
		အကယ်၍ (menu_name_equal(name, menu))
		{
		    // Found menu
		    အကယ်၍ ((*p != NUL && menu->children == NULL)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			ပြန်ပေး NULL;
		    }
		    bha_nga;
		}
		menu = menu->next;
		အကယ်၍ (menu == NULL && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    အကယ်၍ (menu == NULL)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		ပြန်ပေး NULL;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	အကယ်၍ (expand_menu == root_menu)
	    expand_menu_alt = curwin->w_winbar;
	မဟုတ်ပါက
	    expand_menu_alt = NULL;
    }
    မဟုတ်ပါက			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    ပြန်ပေး NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, ကိန်းပြည့် idx)
{
    static vimmenu_T	*menu = NULL;
    static ကိန်းပြည့်		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  ကိန်းပြည့်		should_advance = FALSE;
#endif

    အကယ်၍ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    နေစဉ် (menu != NULL && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == NULL))
    {
	menu = menu->next;
	အကယ်၍ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    အကယ်၍ (menu == NULL)	    // at end of linked list
	ပြန်ပေး NULL;

    အကယ်၍ (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	အကယ်၍ (should_advance)
	    str = menu->en_dname;
	မဟုတ်ပါက
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    အကယ်၍ (menu->en_dname == NULL)
		should_advance = TRUE;
	}
#endif
    မဟုတ်ပါက
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    အကယ်၍ (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	အကယ်၍ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ပြန်ပေး str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, ကိန်းပြည့် idx)
{
    static vimmenu_T	*menu = NULL;
    static ကိန်းပြည့်		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    static char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    static  ကိန်းပြည့်		should_advance = FALSE;
#endif

    အကယ်၍ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    နေစဉ် (menu != NULL
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	အကယ်၍ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    အကယ်၍ (menu == NULL)	    // at end of linked list
	ပြန်ပေး NULL;

    အကယ်၍ (menu->modes & expand_modes)
    {
	အကယ်၍ (menu->children != NULL)
	{
#ifdef FEAT_MULTI_LANG
	    အကယ်၍ (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    မဟုတ်ပါက
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		အကယ်၍ (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	မဟုတ်ပါက
#ifdef FEAT_MULTI_LANG
	{
	    အကယ်၍ (should_advance)
		str = menu->en_dname;
	    မဟုတ်ပါက
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		အကယ်၍ (menu->en_dname == NULL)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    မဟုတ်ပါက
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    အကယ်၍ (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	အကယ်၍ (menu == NULL && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ပြန်ပေး str;
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

    အတွက် (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	အကယ်၍ (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    အကယ်၍ (*p == NUL)
		bha_nga;
	}
    }
    အကယ်၍ (*p)
	*p++ = NUL;
    ပြန်ပေး p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    static ကိန်းပြည့်
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    အကယ်၍ (menu->en_name != NULL
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	ပြန်ပေး TRUE;
#endif
    ပြန်ပေး menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    static ကိန်းပြည့်
menu_namecmp(char_u *name, char_u *mname)
{
    ကိန်းပြည့်		i;

    အတွက် (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	အကယ်၍ (name[i] != mname[i])
	    bha_nga;
    ပြန်ပေး ((name[i] == NUL || name[i] == TAB)
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
    static ကိန်းပြည့်
get_menu_cmd_modes(
    char_u  *cmd,
    ကိန်းပြည့်	    forceit,	    // Was there a "!" after the command?
    ကိန်းပြည့်	    *noremap,
    ကိန်းပြည့်	    *unmenu)
{
    ကိန်းပြည့်	    modes;

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
	    အကယ်၍ (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
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
	    အကယ်၍ (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		bha_nga;
	    }
	    // FALLTHROUGH
	default:
	    --cmd;
	    အကယ်၍ (forceit)		// menu!!
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    မဟုတ်ပါက			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    အကယ်၍ (noremap != NULL)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    အကယ်၍ (unmenu != NULL)
	*unmenu = (*cmd == 'u');
    ပြန်ပေး modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    static char_u *
get_menu_mode_str(ကိန်းပြည့် modes)
{
    အကယ်၍ ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	ပြန်ပေး (char_u *)"a";
    အကယ်၍ ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	ပြန်ပေး (char_u *)" ";
    အကယ်၍ ((modes & (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	    == (MENU_INSERT_MODE | MENU_CMDLINE_MODE))
	ပြန်ပေး (char_u *)"!";
    အကယ်၍ ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	ပြန်ပေး (char_u *)"v";
    အကယ်၍ (modes & MENU_VISUAL_MODE)
	ပြန်ပေး (char_u *)"x";
    အကယ်၍ (modes & MENU_SELECT_MODE)
	ပြန်ပေး (char_u *)"s";
    အကယ်၍ (modes & MENU_OP_PENDING_MODE)
	ပြန်ပေး (char_u *)"o";
    အကယ်၍ (modes & MENU_INSERT_MODE)
	ပြန်ပေး (char_u *)"i";
    အကယ်၍ (modes & MENU_TERMINAL_MODE)
	ပြန်ပေး (char_u *)"tl";
    အကယ်၍ (modes & MENU_CMDLINE_MODE)
	ပြန်ပေး (char_u *)"c";
    အကယ်၍ (modes & MENU_NORMAL_MODE)
	ပြန်ပေး (char_u *)"n";
    အကယ်၍ (modes & MENU_TIP_MODE)
	ပြန်ပေး (char_u *)"t";

    ပြန်ပေး (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    static char_u *
popup_mode_name(char_u *name, ကိန်းပြည့် idx)
{
    char_u	*p;
    ကိန်းပြည့်		len = (ကိန်းပြည့်)STRLEN(name);
    var_na	*mode_chars = menu_mode_chars[idx];
    ကိန်းပြည့်		mode_chars_len = (ကိန်းပြည့်)strlen(mode_chars);
    ကိန်းပြည့်		i;

    p = vim_strnsave(name, len + mode_chars_len);
    အကယ်၍ (p == NULL)
	ပြန်ပေး NULL;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    အတွက် (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    ပြန်ပေး p;
}

#အကယ်၍ defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    ကိန်းပြည့်
get_menu_index(vimmenu_T *menu, ကိန်းပြည့် state)
{
    ကိန်းပြည့်		idx;

    အကယ်၍ ((state & MODE_INSERT))
	idx = MENU_INDEX_INSERT;
    မဟုတ်ပါက အကယ်၍ (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    မဟုတ်ပါက အကယ်၍ (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    မဟုတ်ပါက အကယ်၍ (VIsual_active)
    {
	အကယ်၍ (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	မဟုတ်ပါက
	    idx = MENU_INDEX_VISUAL;
    }
    မဟုတ်ပါက အကယ်၍ (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    မဟုတ်ပါက အကယ်၍ (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    မဟုတ်ပါက အကယ်၍ ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    မဟုတ်ပါက
	idx = MENU_INDEX_INVALID;

    အကယ်၍ (idx != MENU_INDEX_INVALID && menu->strings[idx] == NULL)
	idx = MENU_INDEX_INVALID;
    ပြန်ပေး idx;
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
menu_text(char_u *str, ကိန်းပြည့် *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    အကယ်၍ (p != NULL)
    {
	အကယ်၍ (actext != NULL)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    မဟုတ်ပါက
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    အတွက် (p = text; p != NULL; )
    {
	p = vim_strchr(p, '&');
	အကယ်၍ (p != NULL)
	{
	    အကယ်၍ (p[1] == NUL)	    // trailing "&"
		bha_nga;
	    အကယ်၍ (mnemonic != NULL && p[1] != '&')
#အကယ်၍ !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#မဟုတ်ပါက
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
    ပြန်ပေး text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    ကိန်းပြည့်
menu_is_menubar(char_u *name)
{
    ပြန်ပေး (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    ကိန်းပြည့်
menu_is_popup(char_u *name)
{
    ပြန်ပေး (STRNCMP(name, "PopUp", 5) == 0);
}

#အကယ်၍ defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    ကိန်းပြည့်
menu_is_child_of_popup(vimmenu_T *menu)
{
    နေစဉ် (menu->parent != NULL)
	menu = menu->parent;
    ပြန်ပေး menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    ကိန်းပြည့်
menu_is_toolbar(char_u *name)
{
    ပြန်ပေး (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    ကိန်းပြည့်
menu_is_separator(char_u *name)
{
    ပြန်ပေး (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    static ကိန်းပြည့်
menu_is_hidden(char_u *name)
{
    ပြန်ပေး (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    static ကိန်းပြည့်
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    ပြန်ပေး (STRCMP(name, TEAR_STRING) == 0);
#မဟုတ်ပါက
    ပြန်ပေး FALSE;
#endif
}

#အကယ်၍ defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    static ကိန်းပြည့်
get_menu_mode(void)
{
# ifdef FEAT_TERMINAL
    အကယ်၍ (term_use_loop())
	ပြန်ပေး MENU_INDEX_TERMINAL;
# endif
    အကယ်၍ (VIsual_active)
    {
	အကယ်၍ (VIsual_select)
	    ပြန်ပေး MENU_INDEX_SELECT;
	ပြန်ပေး MENU_INDEX_VISUAL;
    }
    အကယ်၍ (State & MODE_INSERT)
	ပြန်ပေး MENU_INDEX_INSERT;
    အကယ်၍ ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	ပြန်ပေး MENU_INDEX_CMDLINE;
    အကယ်၍ (finish_op)
	ပြန်ပေး MENU_INDEX_OP_PENDING;
    အကယ်၍ (State & MODE_NORMAL)
	ပြန်ပေး MENU_INDEX_NORMAL;
    အကယ်၍ (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	ပြန်ပေး MENU_INDEX_INSERT;
    ပြန်ပေး MENU_INDEX_INVALID;
}

    ကိန်းပြည့်
get_menu_mode_flag(void)
{
    ကိန်းပြည့် mode = get_menu_mode();

    အကယ်၍ (mode == MENU_INDEX_INVALID)
	ပြန်ပေး 0;
    ပြန်ပေး 1 << mode;
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
    ကိန်းပြည့်		menu_mode;
    var_na*	mode;
    ကိန်းပြည့်		mode_len;

    menu_mode = get_menu_mode();
    အကယ်၍ (menu_mode == MENU_INDEX_INVALID)
	ပြန်ပေး;
    mode = menu_mode_chars[menu_mode];
    mode_len = (ကိန်းပြည့်)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, NULL, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	အကယ်၍ (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    bha_nga;

    // Only show a popup when it is defined and has entries
    အကယ်၍ (menu == NULL || menu->children == NULL)
	ပြန်ပေး;

# အကယ်၍ defined(FEAT_GUI)
    အကယ်၍ (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# အကယ်၍ defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    မဟုတ်ပါက
# endif
# အကယ်၍ defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#အကယ်၍ defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    ကိန်းပြည့်
check_menu_pointer(vimmenu_T *root, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    အတွက် (p = root; p != NULL; p = p->next)
	အကယ်၍ (p == menu_to_check
		|| (p->children != NULL
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    ပြန်ပေး OK;
    ပြန်ပေး FAIL;
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
    ကိန်းပြည့်		idx = 0;

    နေစဉ် (menu != NULL)
    {
	// Don't add a menu when only a tip was defined.
	အကယ်၍ (menu->modes & MENU_ALL_MODES)
	{
	    အကယ်၍ (menu->children != NULL)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    မဟုတ်ပါက
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
gui_update_menus_recurse(vimmenu_T *menu, ကိန်းပြည့် mode)
{
    ကိန်းပြည့်		grey;

    နေစဉ် (menu)
    {
	အကယ်၍ ((menu->modes & menu->enabled & mode)
# အကယ်၍ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	မဟုတ်ပါက
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	အကယ်၍ (vim_strchr(p_go, GO_GREY) != NULL || menu->parent == NULL
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	မဟုတ်ပါက
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
gui_update_menus(ကိန်းပြည့် modes)
{
    static ကိန်းပြည့်	    prev_mode = -1;
    ကိန်းပြည့်		    mode = 0;

    အကယ်၍ (modes != 0x0)
	mode = modes;
    မဟုတ်ပါက
	mode = get_menu_mode_flag();

    အကယ်၍ (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# အကယ်၍ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    ကိန်းပြည့်
gui_is_menu_shortcut(ကိန်းပြည့် key)
{
    vimmenu_T	*menu;

    အကယ်၍ (key < 256)
	key = TOLOWER_LOC(key);
    FOR_ALL_MENUS(menu)
	အကယ်၍ (menu->mnemonic == key
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == key))
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}
# endif
#endif // FEAT_GUI

#အကယ်၍ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    void
gui_mch_toggle_tearoffs(ကိန်းပြည့် enable)
{
    ကိန်းပြည့်		pri_tab[MENUDEPTH + 1];
    ကိန်းပြည့်		i;

    အကယ်၍ (enable)
    {
	အတွက် (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(root_menu, (char_u *)"", pri_tab, 0);
    }
    မဟုတ်ပါက
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
    ကိန်းပြည့်			*pri_tab,
    ကိန်းပြည့်			pri_idx)
{
    char_u	*newpname = NULL;
    ကိန်းပြည့်		len;
    char_u	*s;
    char_u	*d;

    အကယ်၍ (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    နေစဉ် (menu != NULL)
    {
	အကယ်၍ (menu->children != NULL && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (ကိန်းပြည့်)STRLEN(pname) + (ကိန်းပြည့်)STRLEN(menu->name);
	    အတွက် (s = menu->name; *s; ++s)
		အကယ်၍ (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    အကယ်၍ (newpname != NULL)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		အတွက် (s = menu->name; *s; ++s)
		{
		    အကယ်၍ (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		အကယ်၍ (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
gui_add_tearoff(char_u *tearpath, ကိန်းပြည့် *pri_tab, ကိန်းပြည့် pri_idx)
{
    char_u	*tbuf;
    ကိန်းပြည့်		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned ကိန်းပြည့်)STRLEN(tearpath));
    အကယ်၍ (tbuf == NULL)
	ပြန်ပေး;

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
    နေစဉ် (menu)
    {
	အကယ်၍ (menu->children)
	{
	    // check if tearoff exists
	    အကယ်၍ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    အကယ်၍ (menu->children != NULL) // if not the last one
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
execute_menu(exarg_T *eap, vimmenu_T *menu, ကိန်းပြည့် mode_idx)
{
    ကိန်းပြည့်		idx = mode_idx;

    အကယ်၍ (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	အကယ်၍ (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_INSERT;
	}
#ifdef FEAT_TERMINAL
	မဟုတ်ပါက အကယ်၍ (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	မဟုတ်ပါက အကယ်၍ (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	မဟုတ်ပါက အကယ်၍ (eap != NULL && eap->addr_count)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    အကယ်၍ ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    မဟုတ်ပါက
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
	    အကယ်၍ (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    အကယ်၍ (idx == MENU_INDEX_INVALID || eap == NULL)
	idx = MENU_INDEX_NORMAL;

    အကယ်၍ (menu->strings[idx] != NULL && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	အကယ်၍ (eap == NULL || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    အကယ်၍ (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	မဟုတ်ပါက
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    မဟုတ်ပါက အကယ်၍ (eap != NULL)
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
    ကိန်းပြည့်		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    အကယ်၍ (saved_name == NULL)
	ပြန်ပေး NULL;

    menu = *get_root_menu(saved_name);
    name = saved_name;
    နေစဉ် (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	နေစဉ် (menu != NULL)
	{
	    အကယ်၍ (menu_name_equal(name, menu))
	    {
		အကယ်၍ (*p == NUL && menu->children != NULL)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = NULL;
		}
		မဟုတ်ပါက အကယ်၍ (*p != NUL && menu->children == NULL)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		}
		bha_nga;
	    }
	    menu = menu->next;
	}
	အကယ်၍ (menu == NULL || *p == NUL)
	    bha_nga;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    အကယ်၍ (menu == NULL)
    {
	အကယ်၍ (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	ပြန်ပေး NULL;
    }

    ပြန်ပေး menu;
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
    ကိန်းပြည့်		mode_idx = MENU_INDEX_INVALID;

    အကယ်၍ (arg[0] && VIM_ISWHITE(arg[1]))
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
		     ပြန်ပေး;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    အကယ်၍ (menu == NULL)
	ပြန်ပေး;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    void
winbar_click(win_T *wp, ကိန်းပြည့် col)
{
    ကိန်းပြည့်		idx;

    အကယ်၍ (wp->w_winbar_items == NULL)
	ပြန်ပေး;
    အတွက် (idx = 0; wp->w_winbar_items[idx].wb_menu != NULL; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_items[idx];

	အကယ်၍ (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = NULL;
	    pos_T   save_visual = VIsual;
	    ကိန်းပြည့်	    save_visual_active = VIsual_active;
	    ကိန်းပြည့်	    save_visual_select = VIsual_select;
	    ကိန်းပြည့်	    save_visual_reselect = VIsual_reselect;
	    ကိန်းပြည့်	    save_visual_mode = VIsual_mode;

	    အကယ်၍ (wp != curwin)
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

	    အကယ်၍ (save_curwin != NULL && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    အကယ်၍ (!win_valid(wp))
		bha_nga;
	}
    }
}

#အကယ်၍ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
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
    အကယ်၍ (saved_name == NULL)
	ပြန်ပေး NULL;

    name = saved_name;
    နေစဉ် (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	နေစဉ် (menu != NULL)
	{
	    အကယ်၍ (menu_name_equal(name, menu))
	    {
		အကယ်၍ (menu->children == NULL)
		{
		    // found a menu item instead of a sub-menu
		    အကယ်၍ (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    မဟုတ်ပါက
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = NULL;
		    goto theend;
		}
		အကယ်၍ (*p == NUL)	    // found a full match
		    goto theend;
		bha_nga;
	    }
	    menu = menu->next;
	}
	အကယ်၍ (menu == NULL)	// didn't find it
	    bha_nga;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    အကယ်၍ (menu == NULL)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    ပြန်ပေး menu;
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
    ကိန်းပြည့်			i;
    char_u		*from, *from_noamp, *to;

    အကယ်၍ (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, parimaa_na(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    အကယ်၍ (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	အတွက် (i = 0; i < menutrans_ga.ga_len; ++i)
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
    မဟုတ်ပါက
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	အကယ်၍ (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	မဟုတ်ပါက
	{
	    အကယ်၍ (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		အကယ်၍ (from != NULL)
		{
		    from_noamp = menu_text(from, NULL, NULL);
		    to = vim_strnsave(to, arg - to);
		    အကယ်၍ (from_noamp != NULL && to != NULL)
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
		    မဟုတ်ပါက
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

#အကယ်၍ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    static char_u *
menu_skip_part(char_u *p)
{
    နေစဉ် (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	အကယ်၍ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    ပြန်ပေး p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    static char_u *
menutrans_lookup(char_u *name, ကိန်းပြည့် len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    ကိန်းပြည့်			i;
    char_u		*dname;

    အတွက် (i = 0; i < menutrans_ga.ga_len; ++i)
	အကယ်၍ (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    ပြန်ပေး tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, NULL, NULL);
    name[len] = i;
    အကယ်၍ (dname == NULL)
	ပြန်ပေး NULL;

    အတွက် (i = 0; i < menutrans_ga.ga_len; ++i)
	အကယ်၍ (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    ပြန်ပေး tp[i].to;
	}
    vim_free(dname);

    ပြန်ပေး NULL;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    static void
menu_unescape_name(char_u *name)
{
    char_u  *p;

    အတွက် (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	အကယ်၍ (*p == '\\')
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

    နေစဉ် (*arg && !VIM_ISWHITE(*arg))
    {
	အကယ်၍ ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	မဟုတ်ပါက အကယ်၍ (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    အကယ်၍ (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    ပြန်ပေး arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    static ကိန်းပြည့်
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, ကိန်းပြည့် modes, dict_T *dict)
{
    ကိန်းပြည့်		status;
    list_T	*l;

    အကယ်၍ (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	အကယ်၍ (l == NULL)
	    ပြန်ပေး FAIL;

	အကယ်၍ (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ပြန်ပေး FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	အတွက် (topmenu = menu; topmenu != NULL; topmenu = topmenu->next)
	    အကယ်၍ (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	ပြန်ပေး OK;
    }

    အကယ်၍ (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	ပြန်ပေး OK;

    status = dict_add_string(dict, "name", menu->name);
    အကယ်၍ (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    အကယ်၍ (status == OK && menu->actext != NULL)
	status = dict_add_string(dict, "accel", menu->actext);
    အကယ်၍ (status == OK)
	status = dict_add_number(dict, "priority", menu->priority);
    အကယ်၍ (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    အကယ်၍ (status == OK && menu->iconfile != NULL)
	status = dict_add_string(dict, "icon", menu->iconfile);
    အကယ်၍ (status == OK && menu->iconidx >= 0)
	status = dict_add_number(dict, "iconidx", menu->iconidx);
#endif
    အကယ်၍ (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	ကိန်းပြည့်	buflen;

	အကယ်၍ (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	မဟုတ်ပါက
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    အကယ်၍ (status == OK && menu->children == NULL)
    {
	ကိန်းပြည့်		bit;

	// Get the first mode in which the menu is available
	အတွက် (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	အကယ်၍ (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    အကယ်၍ (menu->strings[bit] != NULL)
	    {
		အကယ်၍ (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		မဟုတ်ပါက
		{
		    char_u *tofree = NULL;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    အကယ်၍ (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    အကယ်၍ (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    အကယ်၍ (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    အကယ်၍ (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    အကယ်၍ (status == OK && menu->children != NULL)
    {
	vimmenu_T	*child;

	l = list_alloc();
	အကယ်၍ (l == NULL)
	    ပြန်ပေး FAIL;

	အကယ်၍ (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ပြန်ပေး FAIL;
	}
	child = menu->children;
	နေစဉ် (child)
	{
	    အကယ်၍ (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    ပြန်ပေး status;
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
    ကိန်းပြည့်		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    အကယ်၍ (rettv_dict_alloc(rettv) == FAIL)
	ပြန်ပေး;
    retdict = rettv->vval.v_dict;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    menu_name = tv_get_string_chk(&argvars[0]);
    အကယ်၍ (menu_name == NULL)
	ပြန်ပေး;

    // menu mode
    အကယ်၍ (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    မဟုတ်ပါက
	which = (char_u *)"";	    // Default is modes for "menu"
    အကယ်၍ (which == NULL)
	ပြန်ပေး;

    modes = get_menu_cmd_modes(which, *which == '!', NULL, NULL);

    // Locate the specified menu or menu item
    menu = *get_root_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    အကယ်၍ (saved_name == NULL)
	ပြန်ပေး;
    အကယ်၍ (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	နေစဉ် (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    နေစဉ် (menu != NULL)
	    {
		အကယ်၍ (menu_name_equal(name, menu))
		    bha_nga;
		menu = menu->next;
	    }
	    အကယ်၍ (menu == NULL || *p == NUL)
		bha_nga;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    အကယ်၍ (menu == NULL)		// specified menu not found
	ပြန်ပေး;

    အကယ်၍ (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
