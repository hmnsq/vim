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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_MENU)

#define MENUDEPTH   10		// maximum depth of menus

#ifdef FEAT_GUI_MSWIN
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_menu_path(char_u *, vimmenu_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
#ଅନ୍ୟଥା
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_menu_path(char_u *, vimmenu_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, char_u *);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା menu_nable_recurse(vimmenu_T *menu, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା remove_menu(vimmenu_T **, char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent);
ସ୍ଥିର ଶୂନ୍ୟ free_menu(vimmenu_T **menup);
ସ୍ଥିର ଶୂନ୍ୟ free_menu_string(vimmenu_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା show_menus(char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ show_menus_recursive(vimmenu_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର char_u *menu_name_skip(char_u *name);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା menu_name_equal(char_u *name, vimmenu_T *menu);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା menu_namecmp(char_u *name, char_u *mname);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_menu_cmd_modes(char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *);
ସ୍ଥିର char_u *popup_mode_name(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ସ୍ଥିର char_u *menu_text(char_u *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *mnemonic, char_u **actext);

#ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
ସ୍ଥିର ଶୂନ୍ୟ gui_create_tearoffs_recurse(vimmenu_T *menu, ଅଚଳ char_u *pname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pri_tab, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pri_idx);
ସ୍ଥିର ଶୂନ୍ୟ gui_add_tearoff(char_u *tearpath, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pri_tab, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pri_idx);
ସ୍ଥିର ଶୂନ୍ୟ gui_destroy_tearoffs_recurse(vimmenu_T *menu);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_tearoffs = FALSE;
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା menu_is_hidden(char_u *name);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା menus_locked = 0;

#ଯଦି defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
ସ୍ଥିର char_u *menu_skip_part(char_u *p);
#endif
#ifdef FEAT_MULTI_LANG
ସ୍ଥିର char_u *menutrans_lookup(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ସ୍ଥିର ଶୂନ୍ୟ menu_unescape_name(char_u	*p);
#endif

ସ୍ଥିର char_u *menu_translate_tab_and_shift(char_u *arg_start);

// The character for each menu mode
ସ୍ଥିର ଅକ୍ଷର *menu_mode_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ifdef FEAT_TOOLBAR
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *toolbar_names[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_ଗଣନା ARRAY_LENGTH(toolbar_names)
#endif

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_winbar(char_u *name)
{
    ଫେରନ୍ତୁ (STRNCMP(name, "WinBar", 6) == 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
winbar_height(win_T *wp)
{
    ଯଦି (wp->w_winbar != ଖାଲି && wp->w_winbar->children != ଖାଲି)
	ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର vimmenu_T **
get_ମୂଳ_menu(char_u *name)
{
    ଯଦି (menu_is_winbar(name))
	ଫେରନ୍ତୁ &curwin->w_winbar;
    ଫେରନ୍ତୁ &ମୂଳ_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_menus_locked(ଶୂନ୍ୟ)
{
    ଯଦି (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    ଶୂନ୍ୟ
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_path;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modes;
    char_u	*map_to;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noremap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		special = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
#ଯଦି defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_menu_height;
# ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_toolbar_height;
# endif
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pri_tab[MENUDEPTH + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ifdef FEAT_TOOLBAR
    char_u	*icon = ଖାଲି;
#endif
    vimmenu_T	menuarg;
    vimmenu_T	**ମୂଳ_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    ପାଇଁ (;;)
    {
	ଯଦି (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    ଜାରି;
	}
	ଯଦି (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    ଜାରି;
	}
	ଯଦି (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    ଜାରି;
	}
	ଭାଙ୍ଗ;
    }


    // Locate an optional "icon=filename" argument.
    ଯଦି (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ifdef FEAT_TOOLBAR
	icon = arg;
#endif
	ଯେତେବେଳେ (*arg != NUL && *arg != ' ')
	{
	    ଯଦି (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	ଯଦି (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    ପାଇଁ (p = arg; *p; ++p)
	ଯଦି (!VIM_ISDIGIT(*p) && *p != '.')
	    ଭାଙ୍ଗ;
    ଯଦି (VIM_ISWHITE(*p))
    {
	ପାଇଁ (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    ଯଦି (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    ଯଦି (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    ଅନ୍ୟଥା ଯଦି (eap->addr_ଗଣନା && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    ଅନ୍ୟଥା
	i = 0;
    ଯେତେବେଳେ (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    ଯଦି (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    ଯଦି (*arg == NUL)
    {
	show_menus(arg, modes);
	ଫେରନ୍ତୁ;
    }

#ifdef FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    ଯଦି (menu_is_toolbar(arg))
    {
	menu_path = menu_skip_part(arg);
	ଯଦି (*menu_path == '.')
	{
	    p = menu_skip_part(++menu_path);
	    ଯଦି (STRNCMP(menu_path, "BuiltIn", 7) == 0)
	    {
		ଯଦି (skipdigits(menu_path + 7) == p)
		{
		    menuarg.iconidx = atoi((ଅକ୍ଷର *)menu_path + 7);
		    ଯଦି (menuarg.iconidx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOOLBAR_NAME_ଗଣନା)
			menuarg.iconidx = -1;
		    ଅନ୍ୟଥା
			menuarg.icon_builtin = TRUE;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TOOLBAR_NAME_ଗଣନା; ++i)
		    ଯଦି (STRNCMP(toolbar_names[i], menu_path, p - menu_path)
									 == 0)
		    {
			menuarg.iconidx = i;
			ଭାଙ୍ଗ;
		    }
	    }
	}
    }
#endif

    menu_path = arg;
    ଯଦି (*menu_path == '.')
    {
	semsg(_(e_invalid_argument_str), menu_path);
	ଯାଅ theend;
    }

    map_to = menu_translate_tab_and_shift(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    ଯଦି (*map_to == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_path, modes);
	ଯାଅ theend;
    }
    ଅନ୍ୟଥା ଯଦି (*map_to != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_to);
	ଯାଅ theend;
    }
#ଯଦି defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_height = gui.menu_height;
# ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_height = gui.toolbar_height;
# endif
#endif

    ମୂଳ_menu_ptr = get_ମୂଳ_menu(menu_path);
    ଯଦି (ମୂଳ_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    ଯଦି (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	ଯଦି (STRCMP(menu_path, "*") == 0)	// meaning: do all menus
	    menu_path = (char_u *)"";

	ଯଦି (menu_is_popup(menu_path))
	{
	    ପାଇଁ (i = 0; i < MENU_INDEX_TIP; ++i)
		ଯଦି (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    ଯଦି (p != ଖାଲି)
		    {
			menu_nable_recurse(*ମୂଳ_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*ମୂଳ_menu_ptr, menu_path, modes, enable);
    }
    ଅନ୍ୟଥା ଯଦି (unmenu)
    {
	ଯଦି (is_menus_locked())
	    ଯାଅ theend;

	/*
	 * Delete menu(s).
	 */
	ଯଦି (STRCMP(menu_path, "*") == 0)	// meaning: remove all menus
	    menu_path = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	ଯଦି (menu_is_popup(menu_path))
	{
	    ପାଇଁ (i = 0; i < MENU_INDEX_TIP; ++i)
		ଯଦି (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    ଯଦି (p != ଖାଲି)
		    {
			remove_menu(ମୂଳ_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(ମୂଳ_menu_ptr, menu_path, modes, FALSE);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (is_menus_locked())
	    ଯାଅ theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	ଯଦି (STRICMP(map_to, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_to = (char_u *)"";
	    map_buf = ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (modes & MENU_TIP_MODE)
	    map_buf = ଖାଲି;	// Menu tips are plain text.
	ଅନ୍ୟଥା
	    map_to = replace_termcodes(map_to, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ଖାଲି);
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
	ଯଦି (menu_is_popup(menu_path))
	{
	    ପାଇଁ (i = 0; i < MENU_INDEX_TIP; ++i)
		ଯଦି (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    ଯଦି (p != ଖାଲି)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ifdef FEAT_TOOLBAR
			menuarg.iconfile = ଖାଲି;
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

#ଯଦି defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    ଯଦି (gui.in_use
	    && (gui.menu_height != old_menu_height
# ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_height != old_toolbar_height
# endif
	    ))
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
#endif
    ଯଦି (ମୂଳ_menu_ptr == &curwin->w_winbar)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା h = winbar_height(curwin);

	ଯଦି (h != curwin->w_winbar_height)
	{
	    ଯଦି (h == 0)
		++curwin->w_height;
	    ଅନ୍ୟଥା ଯଦି (curwin->w_height > 0)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_menu_path(
    char_u	*menu_path,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*pri_tab,
    char_u	*call_data
#ifdef FEAT_GUI_MSWIN
    , ପୂର୍ଣ୍ଣସଂଖ୍ୟା	addtearoff	// may add tearoff item
#endif
    )
{
    char_u	*path_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = ଖାଲି;
    vimmenu_T	*parent;
    vimmenu_T	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		d;
#ifdef FEAT_GUI
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_idx;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pri_idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_modes = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amenu;
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_to = ଖାଲି;
#endif
    vimmenu_T	**ମୂଳ_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    path_name = vim_strsave(menu_path);
    ଯଦି (path_name == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ମୂଳ_menu_ptr = get_ମୂଳ_menu(menu_path);
    menup = ମୂଳ_menu_ptr;
    parent = ଖାଲି;
    name = path_name;
    ଯେତେବେଳେ (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	next_name = menu_name_skip(name);
#ifdef	FEAT_MULTI_LANG
	map_to = menutrans_lookup(name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name));
	ଯଦି (map_to != ଖାଲି)
	{
	    en_name = name;
	    name = map_to;
	}
	ଅନ୍ୟଥା
	    en_name = ଖାଲି;
#endif
	dname = menu_text(name, ଖାଲି, ଖାଲି);
	ଯଦି (dname == ଖାଲି)
	    ଯାଅ erret;
	ଯଦି (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_empty_menu_name));
	    ଯାଅ erret;
	}

	// See if it's already there
	lower_pri = menup;
#ifdef FEAT_GUI
	idx = 0;
	new_idx = 0;
#endif
	menu = *menup;
	ଯେତେବେଳେ (menu != ଖାଲି)
	{
	    ଯଦି (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		ଯଦି (*next_name == NUL && menu->children != ଖାଲି)
		{
		    ଯଦି (!sys_menu)
			emsg(_(e_menu_path_must_not_lead_to_sub_menu));
		    ଯାଅ erret;
		}
		ଯଦି (*next_name != NUL && menu->children == ଖାଲି
#ifdef FEAT_GUI_MSWIN
			&& addtearoff
#endif
			)
		{
		    ଯଦି (!sys_menu)
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    ଯାଅ erret;
		}
		ଭାଙ୍ଗ;
	    }
	    menup = &menu->next;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    ଯଦି (parent != ଖାଲି || menu_is_menubar(menu->name))
	    {
#ifdef FEAT_GUI
		++idx;
#endif
		ଯଦି (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
#ifdef FEAT_GUI
		    new_idx = idx;
#endif
		}
	    }
	    menu = menu->next;
	}

	ଯଦି (menu == ଖାଲି)
	{
	    ଯଦି (*next_name == NUL && parent == ଖାଲି)
	    {
		emsg(_(e_must_not_add_menu_ବସ୍ତୁଗୁଡ଼ିକ_directly_to_menu_bar));
		ଯାଅ erret;
	    }

	    ଯଦି (menu_is_separator(dname) && *next_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_of_menu_path));
		ଯାଅ erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    ଯଦି (menu == ଖାଲି)
		ଯାଅ erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_text(name, &menu->mnemonic, &menu->actext);
#ifdef	FEAT_MULTI_LANG
	    ଯଦି (en_name != ଖାଲି)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_text(en_name, ଖାଲି, ଖାଲି);
	    }
	    ଅନ୍ୟଥା
	    {
		menu->en_name = ଖାଲି;
		menu->en_dname = ଖାଲି;
	    }
#endif
	    menu->priority = pri_tab[pri_idx];
	    menu->parent = parent;
#ifdef FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#endif
#ifdef FEAT_BEVAL_TIP
	    menu->tip = ଖାଲି;
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
	    ଯଦି (*next_name == NUL && menuarg->iconfile != ଖାଲି)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#endif
#ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    ଯଦି (parent != ଖାଲି && menu_is_tearoff(parent->children->dname))
		parent->children->modes |= modes;
#endif
	}
	ଅନ୍ୟଥା
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ifdef FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    ଯଦି (addtearoff)
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
	ଯଦି ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    ଯଦି (gui.in_use)  // Otherwise it will be added when GUI starts
	    {
		ଯଦି (*next_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_add_menu_item(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		ଅନ୍ୟଥା
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_add_menu(menu, new_idx);
		}
	    }

# ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    ଯଦି (	addtearoff
		    && *next_name
		    && vim_strchr(p_go, GO_TEAROFF) != ଖାଲି
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
		ଯଦି (tearpath != ଖାଲି)
		{
		    char_u  *s;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

		    STRCPY(tearpath, menu_path);
		    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(next_name - path_name - 1);
		    ପାଇଁ (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			ଯଦି ((*s == '\\' || *s == Ctrl_V) && s[1])
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
	ଯଦି (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_ଭର୍ତ୍ତି_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_ଭର୍ତ୍ତି_MODE));
    ଯଦି (sys_menu)
	modes &= ~old_modes;

    ଯଦି (menu != ଖାଲି && modes)
    {
#ifdef FEAT_GUI
	menu->cb = gui_menu_cb;
#endif
	p = (call_data == ଖାଲି) ? ଖାଲି : vim_strsave(call_data);

	// loop over all modes, may add more than one
	ପାଇଁ (i = 0; i < MENU_MODES; ++i)
	{
	    ଯଦି (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		ଯଦି (amenu && call_data != ଖାଲି && *call_data != NUL
#ifdef FEAT_GUI_MSWIN
		       && addtearoff
#endif
				       )
		{
		    ବାଛ (1 << i)
		    {
			ସ୍ଥିତି MENU_VISUAL_MODE:
			ସ୍ଥିତି MENU_SELECT_MODE:
			ସ୍ଥିତି MENU_OP_PENDING_MODE:
			ସ୍ଥିତି MENU_CMDLINE_MODE:
			    c = Ctrl_C;
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି MENU_ଭର୍ତ୍ତି_MODE:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    ଭାଙ୍ଗ;
		    }
		}

		ଯଦି (c != 0)
		{
		    menu->strings[i] = alloc(STRLEN(call_data) + 5);
		    ଯଦି (menu->strings[i] != ଖାଲି)
		    {
			menu->strings[i][0] = c;
			ଯଦି (d == 0)
			    STRCPY(menu->strings[i] + 1, call_data);
			ଅନ୍ୟଥା
			{
			    menu->strings[i][1] = d;
			    STRCPY(menu->strings[i] + 2, call_data);
			}
			ଯଦି (c == Ctrl_C)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(menu->strings[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->strings[i][len] = Ctrl_BSL;
			    menu->strings[i][len + 1] = Ctrl_G;
			    menu->strings[i][len + 2] = NUL;
			}
		    }
		}
		ଅନ୍ୟଥା
		    menu->strings[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	ଯଦି (modes & MENU_TIP_MODE)
	    gui_mch_menu_set_tip(menu);
#endif
    }
    ଫେରନ୍ତୁ OK;

erret:
    vim_free(path_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    ଯେତେବେଳେ (parent != ଖାଲି && parent->children == ଖାଲି)
    {
	ଯଦି (parent->parent == ଖାଲି)
	    menup = ମୂଳ_menu_ptr;
	ଅନ୍ୟଥା
	    menup = &parent->parent->children;
	ପାଇଁ ( ; *menup != ଖାଲି && *menup != parent; menup = &((*menup)->next))
	    ;
	ଯଦି (*menup == ଖାଲି) // safety check
	    ଭାଙ୍ଗ;
	parent = parent->parent;
	free_menu(menup);
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modes,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		enable)
{
    char_u	*p;

    ଯଦି (menu == ଖାଲି)
	ଫେରନ୍ତୁ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ଯେତେବେଳେ (menu != ଖାଲି)
    {
	ଯଦି (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    ଯଦି (*p != NUL)
	    {
		ଯଦି (menu->children == ଖାଲି)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
		ଯଦି (enable)
		    menu->enabled |= modes;
		ଅନ୍ୟଥା
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    ଯଦି (*name != NUL && *name != '*')
		ଭାଙ୍ଗ;
	}
	menu = menu->next;
    }
    ଯଦି (*name != NUL && *name != '*' && menu == ଖାଲି)
    {
	semsg(_(e_no_menu_str), name);
	ଫେରନ୍ତୁ FAIL;
    }

#ifdef FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modes,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*child;
    char_u	*p;

    ଯଦି (*menup == ଖାଲି)
	ଫେରନ୍ତୁ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ଯେତେବେଳେ ((menu = *menup) != ଖାଲି)
    {
	ଯଦି (*name == NUL || menu_name_equal(name, menu))
	{
	    ଯଦି (*p != NUL && menu->children == ଖାଲି)
	    {
		ଯଦି (!silent)
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି ((menu->modes & modes) != 0x0)
	    {
#ଯଦି defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		ଯଦି (*p == NUL && modes == MENU_ALL_MODES)
		{
		    ଯଦି (IsWindow(menu->tearoff_handle))
			DestroyWindow(menu->tearoff_handle);
		}
#endif
		ଯଦି (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (*name != NUL)
	    {
		ଯଦି (!silent)
		    emsg(_(e_menu_only_exists_in_another_mode));
		ଫେରନ୍ତୁ FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    ଯଦି (*name != NUL)
		ଭାଙ୍ଗ;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    ଯଦି (modes & MENU_TIP_MODE)
		free_menu_string(menu, MENU_INDEX_TIP);
	    ଯଦି ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    ଅନ୍ୟଥା
		menup = &menu->next;
	}
	ଅନ୍ୟଥା
	    menup = &menu->next;
    }
    ଯଦି (*name != NUL)
    {
	ଯଦି (menu == ଖାଲି)
	{
	    ଯଦି (!silent)
		semsg(_(e_no_menu_str), name);
	    ଫେରନ୍ତୁ FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#ଯଦି defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	ଯଦି ((s_tearoffs) && (menu->children != ଖାଲି)) // there's a tear bar.
	    child = menu->children->next; // don't count tearoff bar
	ଅନ୍ୟଥା
#endif
	    child = menu->children;
	ପାଇଁ ( ; child != ଖାଲି; child = child->next)
	    menu->modes |= child->modes;
	ଯଦି (modes & MENU_TIP_MODE)
	{
	    free_menu_string(menu, MENU_INDEX_TIP);
#ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    ଯଦି (gui.in_use)
		gui_mch_menu_set_tip(menu);
#endif
	}
	ଯଦି ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#ଯଦି defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    ଯଦି (s_tearoffs && menu->children != ଖାଲି) // there's a tear bar.
		free_menu(&menu->children);
#endif
	    *menup = menu;
	    free_menu(menup);
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    ଶୂନ୍ୟ
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_ବସ୍ତୁଗୁଡ଼ିକ);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_menu(vimmenu_T **menup)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    vimmenu_T	*menu;

    menu = *menup;

#ifdef FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    ଯଦି (gui.in_use)
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
    ପାଇଁ (i = 0; i < MENU_MODES; i++)
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
    ସ୍ଥିର ଶୂନ୍ୟ
free_menu_string(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < MENU_MODES; i++)
	ଯଦି (menu->strings[i] == menu->strings[idx])
	    ଗଣନା_2++;
    ଯଦି (ଗଣନା_2 == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = ଖାଲି;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
show_menus(char_u *path_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*parent = ଖାଲି;

    name = path_name = vim_strsave(path_name);
    ଯଦି (path_name == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    menu = *get_ମୂଳ_menu(path_name);

    // First, find the (sub)menu with the given name
    ଯେତେବେଳେ (*name)
    {
	p = menu_name_skip(name);
	ଯେତେବେଳେ (menu != ଖାଲି)
	{
	    ଯଦି (menu_name_equal(name, menu))
	    {
		// Found menu
		ଯଦି (*p != NUL && menu->children == ଖାଲି)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    vim_free(path_name);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା ଯଦି ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_in_another_mode));
		    vim_free(path_name);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଭାଙ୍ଗ;
	    }
	    menu = menu->next;
	}
	ଯଦି (menu == ଖାଲି)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(path_name);
	    ଫେରନ୍ତୁ FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }
    vim_free(path_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    ସନ୍ଦେଶ_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(parent, modes, 0);

    --menus_locked;
    ଫେରନ୍ତୁ OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ସ୍ଥିର ଶୂନ୍ୟ
show_menus_recursive(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modes, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bit;

    ଯଦି (menu != ଖାଲି && (menu->modes & modes) == 0x0)
	ଫେରନ୍ତୁ;

    ଯଦି (menu != ଖାଲି)
    {
	ସନ୍ଦେଶ_putchar('\n');
	ଯଦି (got_int)		// "q" hit for "--more--"
	    ଫେରନ୍ତୁ;
	ପାଇଁ (i = 0; i < depth; i++)
	    ସନ୍ଦେଶ_puts("  ");
	ଯଦି (menu->priority)
	{
	    ସନ୍ଦେଶ_outnum((long)menu->priority);
	    ସନ୍ଦେଶ_puts(" ");
	}
				// Same highlighting as for directories!?
	ସନ୍ଦେଶ_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    ଯଦି (menu != ଖାଲି && menu->children == ଖାଲି)
    {
	ପାଇଁ (bit = 0; bit < MENU_MODES; bit++)
	    ଯଦି ((menu->modes & modes & (1 << bit)) != 0)
	    {
		ସନ୍ଦେଶ_putchar('\n');
		ଯଦି (got_int)		// "q" hit for "--more--"
		    ଫେରନ୍ତୁ;
		ପାଇଁ (i = 0; i < depth + 2; i++)
		    ସନ୍ଦେଶ_puts("  ");
		ସନ୍ଦେଶ_puts(menu_mode_chars[bit]);
		ଯଦି (menu->noremap[bit] == REMAP_NONE)
		    ସନ୍ଦେଶ_putchar('*');
		ଅନ୍ୟଥା ଯଦି (menu->noremap[bit] == REMAP_SCRIPT)
		    ସନ୍ଦେଶ_putchar('&');
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ_putchar(' ');
		ଯଦି (menu->silent[bit])
		    ସନ୍ଦେଶ_putchar('s');
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ_putchar(' ');
		ଯଦି ((menu->modes & menu->enabled & (1 << bit)) == 0)
		    ସନ୍ଦେଶ_putchar('-');
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ_putchar(' ');
		ସନ୍ଦେଶ_puts(" ");
		ଯଦି (*menu->strings[bit] == NUL)
		    ସନ୍ଦେଶ_puts_attr("<Nop>", HL_ATTR(HLF_8));
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ_outtrans_special(menu->strings[bit], FALSE, 0);
	    }
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (menu == ଖାଲି)
	{
	    menu = ମୂଳ_menu;
	    depth--;
	}
	ଅନ୍ୟଥା
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	ପାଇଁ (; menu != ଖାଲି && !got_int; menu = menu->next)
	    ଯଦି (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
ସ୍ଥିର vimmenu_T	*expand_menu = ଖାଲି;
ସ୍ଥିର vimmenu_T	*expand_menu_alt = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		expand_modes = 0x0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
set_context_in_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*path_name = ଖାଲି;
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unmenu;
    vimmenu_T	*menu;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    ପାଇଁ (p = arg; *p; ++p)
	ଯଦି (!VIM_ISDIGIT(*p) && *p != '.')
	    ଭାଙ୍ଗ;

    ଯଦି (!VIM_ISWHITE(*p))
    {
	ଯଦି (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	ଅନ୍ୟଥା ଯଦି (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	ଅନ୍ୟଥା
	    p = arg;
    }

    ଯେତେବେଳେ (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    ପାଇଁ (; *p && !VIM_ISWHITE(*p); ++p)
    {
	ଯଦି ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	ଅନ୍ୟଥା ଯଦି (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    ଯଦି (expand_menus && VIM_ISWHITE(*p))
	ଫେରନ୍ତୁ ଖାଲି;	// TODO: check for next command?
    ଯଦି (*p == NUL)		// Complete the menu name
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, ଖାଲି, &unmenu);
	ଯଦି (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = ମୂଳ_menu;
	ଯଦି (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    ଯଦି (path_name == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;
	    vim_strncpy(path_name, arg, after_dot - arg - 1);
	}
	name = path_name;
	ଯେତେବେଳେ (name != ଖାଲି && *name)
	{
	    p = menu_name_skip(name);
	    ଯେତେବେଳେ (menu != ଖାଲି)
	    {
		ଯଦି (menu_name_equal(name, menu))
		{
		    // Found menu
		    ଯଦି ((*p != NUL && menu->children == ଖାଲି)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			ଫେରନ୍ତୁ ଖାଲି;
		    }
		    ଭାଙ୍ଗ;
		}
		menu = menu->next;
		ଯଦି (menu == ଖାଲି && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    ଯଦି (menu == ଖାଲି)
	    {
		// No menu found with the name we were looking for
		vim_free(path_name);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(path_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_pattern = after_dot;
	expand_menu = menu;
	ଯଦି (expand_menu == ମୂଳ_menu)
	    expand_menu_alt = curwin->w_winbar;
	ଅନ୍ୟଥା
	    expand_menu_alt = ଖାଲି;
    }
    ଅନ୍ୟଥା			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର vimmenu_T	*menu = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_alt_menu = FALSE;
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    ସ୍ଥିର  ପୂର୍ଣ୍ଣସଂଖ୍ୟା		should_advance = FALSE;
#endif

    ଯଦି (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip PopUp[nvoci].
    ଯେତେବେଳେ (menu != ଖାଲି && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == ଖାଲି))
    {
	menu = menu->next;
	ଯଦି (menu == ଖାଲି && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ଯଦି (menu == ଖାଲି)	    // at end of linked list
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (menu->modes & expand_modes)
#ifdef FEAT_MULTI_LANG
	ଯଦି (should_advance)
	    str = menu->en_dname;
	ଅନ୍ୟଥା
	{
#endif
	    str = menu->dname;
#ifdef FEAT_MULTI_LANG
	    ଯଦି (menu->en_dname == ଖାଲି)
		should_advance = TRUE;
	}
#endif
    ଅନ୍ୟଥା
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    ଯଦି (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	ଯଦି (menu == ଖାଲି && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ଫେରନ୍ତୁ str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_names(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର vimmenu_T	*menu = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_alt_menu = FALSE;
#define TBUFFER_LEN 256
    ସ୍ଥିର char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ifdef FEAT_MULTI_LANG
    ସ୍ଥିର  ପୂର୍ଣ୍ଣସଂଖ୍ୟା		should_advance = FALSE;
#endif

    ଯଦି (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ifdef FEAT_MULTI_LANG
	should_advance = FALSE;
#endif
    }

    // Skip Browse-style entries, popup menus and separators.
    ଯେତେବେଳେ (menu != ଖାଲି
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ifndef FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#endif
	       ))
    {
	menu = menu->next;
	ଯଦି (menu == ଖାଲି && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ଯଦି (menu == ଖାଲି)	    // at end of linked list
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (menu->modes & expand_modes)
    {
	ଯଦି (menu->children != ଖାଲି)
	{
#ifdef FEAT_MULTI_LANG
	    ଯଦି (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    ଅନ୍ୟଥା
	    {
#endif
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ifdef FEAT_MULTI_LANG
		ଯଦି (menu->en_dname == ଖାଲି)
		    should_advance = TRUE;
	    }
#endif
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	ଅନ୍ୟଥା
#ifdef FEAT_MULTI_LANG
	{
	    ଯଦି (should_advance)
		str = menu->en_dname;
	    ଅନ୍ୟଥା
	    {
#endif
		str = menu->dname;
#ifdef FEAT_MULTI_LANG
		ଯଦି (menu->en_dname == ଖାଲି)
		    should_advance = TRUE;
	    }
	}
#endif
    }
    ଅନ୍ୟଥା
	str = (char_u *)"";

#ifdef FEAT_MULTI_LANG
    ଯଦି (should_advance)
#endif
    {
	// Advance to next menu entry.
	menu = menu->next;
	ଯଦି (menu == ଖାଲି && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ifdef FEAT_MULTI_LANG
    should_advance = !should_advance;
#endif

    ଫେରନ୍ତୁ str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    ସ୍ଥିର char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    ପାଇଁ (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	}
    }
    ଯଦି (*p)
	*p++ = NUL;
    ଫେରନ୍ତୁ p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ifdef FEAT_MULTI_LANG
    ଯଦି (menu->en_name != ଖାଲି
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	ଫେରନ୍ତୁ TRUE;
#endif
    ଫେରନ୍ତୁ menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_namecmp(char_u *name, char_u *mname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	ଯଦି (name[i] != mname[i])
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ ((name[i] == NUL || name[i] == TAB)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_menu_cmd_modes(
    char_u  *cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    forceit,	    // Was there a "!" after the command?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *noremap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *unmenu)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    modes;

    ବାଛ (*cmd++)
    {
	ସ୍ଥିତି 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_MODE | MENU_SELECT_MODE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_MODE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_MODE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'o':			// omenu
	    modes = MENU_OP_PENDING_MODE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'i':			// imenu
	    modes = MENU_ଭର୍ତ୍ତି_MODE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 't':
	    ଯଦି (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_MODE;
		++cmd;
		ଭାଙ୍ଗ;
	    }
	    modes = MENU_TIP_MODE;	// tmenu
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'c':			// cmenu
	    modes = MENU_CMDLINE_MODE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'a':			// amenu
	    modes = MENU_ଭର୍ତ୍ତି_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_SELECT_MODE
				    | MENU_OP_PENDING_MODE;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'n':
	    ଯଦି (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_NORMAL_MODE;
		ଭାଙ୍ଗ;
	    }
	    // FALLTHROUGH
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    --cmd;
	    ଯଦି (forceit)		// menu!!
		modes = MENU_ଭର୍ତ୍ତି_MODE | MENU_CMDLINE_MODE;
	    ଅନ୍ୟଥା			// menu
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE
						       | MENU_OP_PENDING_MODE;
    }

    ଯଦି (noremap != ଖାଲି)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    ଯଦି (unmenu != ଖାଲି)
	*unmenu = (*cmd == 'u');
    ଫେରନ୍ତୁ modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ସ୍ଥିର char_u *
get_menu_mode_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା modes)
{
    ଯଦି ((modes & (MENU_ଭର୍ତ୍ତି_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		  MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	    == (MENU_ଭର୍ତ୍ତି_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE |
		MENU_VISUAL_MODE | MENU_SELECT_MODE | MENU_OP_PENDING_MODE))
	ଫେରନ୍ତୁ (char_u *)"a";
    ଯଦି ((modes & (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		  MENU_OP_PENDING_MODE))
	    == (MENU_NORMAL_MODE | MENU_VISUAL_MODE | MENU_SELECT_MODE |
		MENU_OP_PENDING_MODE))
	ଫେରନ୍ତୁ (char_u *)" ";
    ଯଦି ((modes & (MENU_ଭର୍ତ୍ତି_MODE | MENU_CMDLINE_MODE))
	    == (MENU_ଭର୍ତ୍ତି_MODE | MENU_CMDLINE_MODE))
	ଫେରନ୍ତୁ (char_u *)"!";
    ଯଦି ((modes & (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	    == (MENU_VISUAL_MODE | MENU_SELECT_MODE))
	ଫେରନ୍ତୁ (char_u *)"v";
    ଯଦି (modes & MENU_VISUAL_MODE)
	ଫେରନ୍ତୁ (char_u *)"x";
    ଯଦି (modes & MENU_SELECT_MODE)
	ଫେରନ୍ତୁ (char_u *)"s";
    ଯଦି (modes & MENU_OP_PENDING_MODE)
	ଫେରନ୍ତୁ (char_u *)"o";
    ଯଦି (modes & MENU_ଭର୍ତ୍ତି_MODE)
	ଫେରନ୍ତୁ (char_u *)"i";
    ଯଦି (modes & MENU_TERMINAL_MODE)
	ଫେରନ୍ତୁ (char_u *)"tl";
    ଯଦି (modes & MENU_CMDLINE_MODE)
	ଫେରନ୍ତୁ (char_u *)"c";
    ଯଦି (modes & MENU_NORMAL_MODE)
	ଫେରନ୍ତୁ (char_u *)"n";
    ଯଦି (modes & MENU_TIP_MODE)
	ଫେରନ୍ତୁ (char_u *)"t";

    ଫେରନ୍ତୁ (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    ସ୍ଥିର char_u *
popup_mode_name(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name);
    ଅକ୍ଷର	*mode_chars = menu_mode_chars[idx];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode_chars_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(mode_chars);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    p = vim_strnsave(name, len + mode_chars_len);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    mch_memmove(p + 5 + mode_chars_len, p + 5, (size_t)(len - 4));
    ପାଇଁ (i = 0; i < mode_chars_len; ++i)
	p[5 + i] = menu_mode_chars[idx][i];
    ଫେରନ୍ତୁ p;
}

#ଯଦି defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_menu_index(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ଯଦି ((state & MODE_ଭର୍ତ୍ତି))
	idx = MENU_INDEX_ଭର୍ତ୍ତି;
    ଅନ୍ୟଥା ଯଦି (state & MODE_CMDLINE)
	idx = MENU_INDEX_CMDLINE;
# ifdef FEAT_TERMINAL
    ଅନ୍ୟଥା ଯଦି (term_use_loop())
	idx = MENU_INDEX_TERMINAL;
# endif
    ଅନ୍ୟଥା ଯଦି (VIsual_active)
    {
	ଯଦି (VIsual_select)
	    idx = MENU_INDEX_SELECT;
	ଅନ୍ୟଥା
	    idx = MENU_INDEX_VISUAL;
    }
    ଅନ୍ୟଥା ଯଦି (state == MODE_HITRETURN || state == MODE_ASKMORE)
	idx = MENU_INDEX_CMDLINE;
    ଅନ୍ୟଥା ଯଦି (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    ଅନ୍ୟଥା ଯଦି ((state & MODE_NORMAL))
	idx = MENU_INDEX_NORMAL;
    ଅନ୍ୟଥା
	idx = MENU_INDEX_INVALID;

    ଯଦି (idx != MENU_INDEX_INVALID && menu->strings[idx] == ଖାଲି)
	idx = MENU_INDEX_INVALID;
    ଫେରନ୍ତୁ idx;
}
#endif

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    ସ୍ଥିର char_u *
menu_text(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*text;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    ଯଦି (p != ଖାଲି)
    {
	ଯଦି (actext != ଖାଲି)
	    *actext = vim_strsave(p + 1);
	text = vim_strnsave(str, p - str);
    }
    ଅନ୍ୟଥା
	text = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    ପାଇଁ (p = text; p != ଖାଲି; )
    {
	p = vim_strchr(p, '&');
	ଯଦି (p != ଖାଲି)
	{
	    ଯଦି (p[1] == NUL)	    // trailing "&"
		ଭାଙ୍ଗ;
	    ଯଦି (mnemonic != ଖାଲି && p[1] != '&')
#ଯଦି !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#ଅନ୍ୟଥା
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		ଅକ୍ଷର c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#endif
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    ଫେରନ୍ତୁ text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_menubar(char_u *name)
{
    ଫେରନ୍ତୁ (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_popup(char_u *name)
{
    ଫେରନ୍ତୁ (STRNCMP(name, "PopUp", 5) == 0);
}

#ଯଦି defined(FEAT_GUI_MOTIF) && (XmVersion <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_child_of_popup(vimmenu_T *menu)
{
    ଯେତେବେଳେ (menu->parent != ଖାଲି)
	menu = menu->parent;
    ଫେରନ୍ତୁ menu_is_popup(menu->name);
}
#endif

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_toolbar(char_u *name)
{
    ଫେରନ୍ତୁ (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_separator(char_u *name)
{
    ଫେରନ୍ତୁ (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_hidden(char_u *name)
{
    ଫେରନ୍ତୁ (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menu_is_tearoff(char_u *name UNUSED)
{
#ifdef FEAT_GUI
    ଫେରନ୍ତୁ (STRCMP(name, TEAR_STRING) == 0);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
#endif
}

#ଯଦି defined(FEAT_GUI) || defined(FEAT_TERM_POPUP_MENU)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_menu_mode(ଶୂନ୍ୟ)
{
# ifdef FEAT_TERMINAL
    ଯଦି (term_use_loop())
	ଫେରନ୍ତୁ MENU_INDEX_TERMINAL;
# endif
    ଯଦି (VIsual_active)
    {
	ଯଦି (VIsual_select)
	    ଫେରନ୍ତୁ MENU_INDEX_SELECT;
	ଫେରନ୍ତୁ MENU_INDEX_VISUAL;
    }
    ଯଦି (State & MODE_ଭର୍ତ୍ତି)
	ଫେରନ୍ତୁ MENU_INDEX_ଭର୍ତ୍ତି;
    ଯଦି ((State & MODE_CMDLINE) || State == MODE_ASKMORE
						    || State == MODE_HITRETURN)
	ଫେରନ୍ତୁ MENU_INDEX_CMDLINE;
    ଯଦି (finish_op)
	ଫେରନ୍ତୁ MENU_INDEX_OP_PENDING;
    ଯଦି (State & MODE_NORMAL)
	ଫେରନ୍ତୁ MENU_INDEX_NORMAL;
    ଯଦି (State & MODE_LANGMAP)	// must be a "r" command, like Insert mode
	ଫେରନ୍ତୁ MENU_INDEX_ଭର୍ତ୍ତି;
    ଫେରନ୍ତୁ MENU_INDEX_INVALID;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_menu_mode_flag(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode = get_menu_mode();

    ଯଦି (mode == MENU_INDEX_INVALID)
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ 1 << mode;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    ଶୂନ୍ୟ
show_popupmenu(ଶୂନ୍ୟ)
{
    vimmenu_T	*menu;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		menu_mode;
    ଅକ୍ଷର*	mode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode_len;

    menu_mode = get_menu_mode();
    ଯଦି (menu_mode == MENU_INDEX_INVALID)
	ଫେରନ୍ତୁ;
    mode = menu_mode_chars[menu_mode];
    mode_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(mode);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)mode, ଖାଲି, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	ଯଦି (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, mode, mode_len) == 0)
	    ଭାଙ୍ଗ;

    // Only show a popup when it is defined and has entries
    ଯଦି (menu == ଖାଲି || menu->children == ଖାଲି)
	ଫେରନ୍ତୁ;

# ଯଦି defined(FEAT_GUI)
    ଯଦି (gui.in_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# ଯଦି defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
    ଅନ୍ୟଥା
# endif
# ଯଦି defined(FEAT_TERM_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#endif

#ଯଦି defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_menu_pointer(vimmenu_T *ମୂଳ, vimmenu_T *menu_to_check)
{
    vimmenu_T	*p;

    ପାଇଁ (p = ମୂଳ; p != ଖାଲି; p = p->next)
	ଯଦି (p == menu_to_check
		|| (p->children != ଖାଲି
		    && check_menu_pointer(p->children, menu_to_check) == OK))
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    ଶୂନ୍ୟ
gui_create_initial_menus(vimmenu_T *menu)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;

    ଯେତେବେଳେ (menu != ଖାଲି)
    {
	// Don't add a menu when only a tip was defined.
	ଯଦି (menu->modes & MENU_ALL_MODES)
	{
	    ଯଦି (menu->children != ଖାଲି)
	    {
		gui_mch_add_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    ଅନ୍ୟଥା
		gui_mch_add_menu_item(menu, idx);
	}
	menu = menu->next;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_update_menus_recurse(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		grey;

    ଯେତେବେଳେ (menu)
    {
	ଯଦି ((menu->modes & menu->enabled & mode)
# ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	ଅନ୍ୟଥା
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	ଯଦି (vim_strchr(p_go, GO_GREY) != ଖାଲି || menu->parent == ଖାଲି
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->parent->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
gui_update_menus(ପୂର୍ଣ୍ଣସଂଖ୍ୟା modes)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_mode = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    mode = 0;

    ଯଦି (modes != 0x0)
	mode = modes;
    ଅନ୍ୟଥା
	mode = get_menu_mode_flag();

    ଯଦି (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(ମୂଳ_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

# ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_is_menu_shortcut(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି)
{
    vimmenu_T	*menu;

    ଯଦି (ଚାବି < 256)
	ଚାବି = TOLOWER_LOC(ଚାବି);
    FOR_ALL_MENUS(menu)
	ଯଦି (menu->mnemonic == ଚାବି
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == ଚାବି))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}
# endif
#endif // FEAT_GUI

#ଯଦି defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    ଶୂନ୍ୟ
gui_mch_toggle_tearoffs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pri_tab[MENUDEPTH + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (enable)
    {
	ପାଇଁ (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(ମୂଳ_menu, (char_u *)"", pri_tab, 0);
    }
    ଅନ୍ୟଥା
	gui_destroy_tearoffs_recurse(ମୂଳ_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ଅଚଳ char_u	*pname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*pri_tab,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			pri_idx)
{
    char_u	*newpname = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*s;
    char_u	*d;

    ଯଦି (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    ଯେତେବେଳେ (menu != ଖାଲି)
    {
	ଯଦି (menu->children != ଖାଲି && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pname) + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(menu->name);
	    ପାଇଁ (s = menu->name; *s; ++s)
		ଯଦି (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    ଯଦି (newpname != ଖାଲି)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		ପାଇଁ (s = menu->name; *s; ++s)
		{
		    ଯଦି (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		ଯଦି (STRCMP(menu->children->name, TEAR_STRING) != 0)
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
    ସ୍ଥିର ଶୂନ୍ୟ
gui_add_tearoff(char_u *tearpath, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pri_tab, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pri_idx)
{
    char_u	*tbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(tearpath));
    ଯଦି (tbuf == ଖାଲି)
	ଫେରନ୍ତୁ;

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
    menuarg.iconfile = ଖାଲି;
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
    ସ୍ଥିର ଶୂନ୍ୟ
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    ଯେତେବେଳେ (menu)
    {
	ଯଦି (menu->children)
	{
	    // check if tearoff exists
	    ଯଦି (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    ଯଦି (menu->children != ଖାଲି) // if not the last one
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
    ଶୂନ୍ୟ
execute_menu(exarg_T *eap, vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = mode_idx;

    ଯଦି (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	ଯଦି (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_INDEX_ଭର୍ତ୍ତି;
	}
#ifdef FEAT_TERMINAL
	ଅନ୍ୟଥା ଯଦି (term_use_loop())
	{
	    idx = MENU_INDEX_TERMINAL;
	}
#endif
	ଅନ୍ୟଥା ଯଦି (VIsual_active)
	{
	    idx = MENU_INDEX_VISUAL;
	}
	ଅନ୍ୟଥା ଯଦି (eap != ଖାଲି && eap->addr_ଗଣନା)
	{
	    pos_T	tpos;

	    idx = MENU_INDEX_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    ଯଦି ((curbuf->b_visual.vi_start.lnum == eap->line1)
		    && (curbuf->b_visual.vi_end.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_mode = curbuf->b_visual.vi_mode;
		tpos = curbuf->b_visual.vi_end;
		curwin->w_cursor = curbuf->b_visual.vi_start;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    ଅନ୍ୟଥା
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
	    ଯଦି (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.col;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    ଯଦି (idx == MENU_INDEX_INVALID || eap == ଖାଲି)
	idx = MENU_INDEX_NORMAL;

    ଯଦି (menu->strings[idx] != ଖାଲି && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	ଯଦି (eap == ଖାଲି || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_normal_busy;
	    ଯଦି (save_current_state(&save_state))
		exec_normal_cmd(menu->strings[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_normal_busy;
	}
	ଅନ୍ୟଥା
	    ins_typebuf(menu->strings[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    ଅନ୍ୟଥା ଯଦି (eap != ଖାଲି)
    {
	char_u	*mode;

	ବାଛ (idx)
	{
	    ସ୍ଥିତି MENU_INDEX_VISUAL:
		mode = (char_u *)"Visual";
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MENU_INDEX_SELECT:
		mode = (char_u *)"Select";
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MENU_INDEX_OP_PENDING:
		mode = (char_u *)"Op-pending";
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MENU_INDEX_TERMINAL:
		mode = (char_u *)"Terminal";
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MENU_INDEX_ଭର୍ତ୍ତି:
		mode = (char_u *)"Insert";
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି MENU_INDEX_CMDLINE:
		mode = (char_u *)"Cmdline";
		ଭାଙ୍ଗ;
	    // case MENU_INDEX_TIP: cannot happen
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		mode = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_mode), mode);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    ସ୍ଥିର vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    ଯଦି (saved_name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    menu = *get_ମୂଳ_menu(saved_name);
    name = saved_name;
    ଯେତେବେଳେ (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	ଯେତେବେଳେ (menu != ଖାଲି)
	{
	    ଯଦି (menu_name_equal(name, menu))
	    {
		ଯଦି (*p == NUL && menu->children != ଖାଲି)
		{
		    emsg(_(e_menu_path_must_lead_to_menu_item));
		    gave_emsg = TRUE;
		    menu = ଖାଲି;
		}
		ଅନ୍ୟଥା ଯଦି (*p != NUL && menu->children == ଖାଲି)
		{
		    emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ଖାଲି;
		}
		ଭାଙ୍ଗ;
	    }
	    menu = menu->next;
	}
	ଯଦି (menu == ଖାଲି || *p == NUL)
	    ଭାଙ୍ଗ;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    ଯଦି (menu == ଖାଲି)
    {
	ଯଦି (!gave_emsg)
	    semsg(_(e_menu_not_found_str), name_arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    ଶୂନ୍ୟ
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode_idx = MENU_INDEX_INVALID;

    ଯଦି (arg[0] && VIM_ISWHITE(arg[1]))
    {
	ବାଛ (arg[0])
	{
	    ସ୍ଥିତି 'n': mode_idx = MENU_INDEX_NORMAL; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'v': mode_idx = MENU_INDEX_VISUAL; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 's': mode_idx = MENU_INDEX_SELECT; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'o': mode_idx = MENU_INDEX_OP_PENDING; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 't': mode_idx = MENU_INDEX_TERMINAL; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'i': mode_idx = MENU_INDEX_ଭର୍ତ୍ତି; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'c': mode_idx = MENU_INDEX_CMDLINE; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: semsg(_(e_invalid_argument_str), arg);
		     ଫେରନ୍ତୁ;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    ଯଦି (menu == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Found the menu, so execute.
    execute_menu(eap, menu, mode_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    ଶୂନ୍ୟ
winbar_click(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ଯଦି (wp->w_winbar_ବସ୍ତୁଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପାଇଁ (idx = 0; wp->w_winbar_ବସ୍ତୁଗୁଡ଼ିକ[idx].wb_menu != ଖାଲି; ++idx)
    {
	winbar_item_T *item = &wp->w_winbar_ବସ୍ତୁଗୁଡ଼ିକ[idx];

	ଯଦି (col >= item->wb_startcol && col <= item->wb_endcol)
	{
	    win_T   *save_curwin = ଖାଲି;
	    pos_T   save_visual = VIsual;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_visual_active = VIsual_active;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_visual_select = VIsual_select;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_visual_reselect = VIsual_reselect;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_visual_mode = VIsual_mode;

	    ଯଦି (wp != curwin)
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
	    execute_menu(ଖାଲି, item->wb_menu, -1);

	    ଯଦି (save_curwin != ଖାଲି && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_buffer;
		VIsual = save_visual;
		VIsual_active = save_visual_active;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_mode = save_visual_mode;
	    }
	    ଯଦି (!win_valid(wp))
		ଭାଙ୍ଗ;
	}
    }
}

#ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_find_menu(char_u *path_name)
{
    vimmenu_T	*menu = ଖାଲି;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_ମୂଳ_menu(path_name);

    saved_name = vim_strsave(path_name);
    ଯଦି (saved_name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    name = saved_name;
    ଯେତେବେଳେ (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	ଯେତେବେଳେ (menu != ଖାଲି)
	{
	    ଯଦି (menu_name_equal(name, menu))
	    {
		ଯଦି (menu->children == ଖାଲି)
		{
		    // found a menu item instead of a sub-menu
		    ଯଦି (*p == NUL)
			emsg(_(e_menu_path_must_lead_to_sub_menu));
		    ଅନ୍ୟଥା
			emsg(_(e_part_of_menu_item_path_is_not_sub_menu));
		    menu = ଖାଲି;
		    ଯାଅ theend;
		}
		ଯଦି (*p == NUL)	    // found a full match
		    ଯାଅ theend;
		ଭାଙ୍ଗ;
	    }
	    menu = menu->next;
	}
	ଯଦି (menu == ଖାଲି)	// didn't find it
	    ଭାଙ୍ଗ;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    ଯଦି (menu == ଖାଲି)
	emsg(_(e_menu_not_found_check_menu_names));
theend:
    vim_free(saved_name);
    ଫେରନ୍ତୁ menu;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*to;		// translated name
} menutrans_T;

ସ୍ଥିର garray_T menutrans_ga = {0, 0, 0, 0, ଖାଲି};
#endif

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    ଶୂନ୍ୟ
ex_menutranslate(exarg_T *eap UNUSED)
{
#ifdef FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    char_u		*from, *from_noamp, *to;

    ଯଦି (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, ଆକାର(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    ଯଦି (STRNCMP(arg, "clear", 5) == 0 && ends_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_data;
	ପାଇଁ (i = 0; i < menutrans_ga.ga_len; ++i)
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
    ଅନ୍ୟଥା
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	to = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(to);
	ଯଦି (arg == to || ends_excmd2(eap->arg, from)
		      || ends_excmd2(eap->arg, to)
		      || !ends_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_argument));
	ଅନ୍ୟଥା
	{
	    ଯଦି (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_data;
		from = vim_strsave(from);
		ଯଦି (from != ଖାଲି)
		{
		    from_noamp = menu_text(from, ଖାଲି, ଖାଲି);
		    to = vim_strnsave(to, arg - to);
		    ଯଦି (from_noamp != ଖାଲି && to != ଖାଲି)
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
		    ଅନ୍ୟଥା
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

#ଯଦି defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ସ୍ଥିର char_u *
menu_skip_part(char_u *p)
{
    ଯେତେବେଳେ (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	ଯଦି ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    ଫେରନ୍ତୁ p;
}
#endif

#ifdef FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    ସ୍ଥିର char_u *
menutrans_lookup(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    char_u		*dname;

    ପାଇଁ (i = 0; i < menutrans_ga.ga_len; ++i)
	ଯଦି (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    ଫେରନ୍ତୁ tp[i].to;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_text(name, ଖାଲି, ଖାଲି);
    name[len] = i;
    ଯଦି (dname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (i = 0; i < menutrans_ga.ga_len; ++i)
	ଯଦି (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    ଫେରନ୍ତୁ tp[i].to;
	}
    vim_free(dname);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
menu_unescape_name(char_u *name)
{
    char_u  *p;

    ପାଇଁ (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	ଯଦି (*p == '\\')
	    STRMOVE(p, p + 1);
}
#endif // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    ସ୍ଥିର char_u *
menu_translate_tab_and_shift(char_u *arg_start)
{
    char_u	*arg = arg_start;

    ଯେତେବେଳେ (*arg && !VIM_ISWHITE(*arg))
    {
	ଯଦି ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	ଅନ୍ୟଥା ଯଦି (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    ଯଦି (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    ଫେରନ୍ତୁ arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modes, dict_T *dict)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    list_T	*l;

    ଯଦି (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	ଯଦି (l == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ଫେରନ୍ତୁ FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	ପାଇଁ (topmenu = menu; topmenu != ଖାଲି; topmenu = topmenu->next)
	    ଯଦି (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	ଫେରନ୍ତୁ OK;

    status = dict_add_string(dict, "name", menu->name);
    ଯଦି (status == OK)
	status = dict_add_string(dict, "display", menu->dname);
    ଯଦି (status == OK && menu->actext != ଖାଲି)
	status = dict_add_string(dict, "accel", menu->actext);
    ଯଦି (status == OK)
	status = dict_add_ସଂଖ୍ୟା(dict, "priority", menu->priority);
    ଯଦି (status == OK)
	status = dict_add_string(dict, "modes",
					get_menu_mode_str(menu->modes));
#ifdef FEAT_TOOLBAR
    ଯଦି (status == OK && menu->iconfile != ଖାଲି)
	status = dict_add_string(dict, "icon", menu->iconfile);
    ଯଦି (status == OK && menu->iconidx >= 0)
	status = dict_add_ସଂଖ୍ୟା(dict, "iconidx", menu->iconidx);
#endif
    ଯଦି (status == OK)
    {
	char_u	buf[NUMBUFLEN];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	buflen;

	ଯଦି (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	ଅନ୍ୟଥା
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	status = dict_add_string_len(dict, "shortcut", buf, buflen);
    }
    ଯଦି (status == OK && menu->children == ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bit;

	// Get the first mode in which the menu is available
	ପାଇଁ (bit = 0; bit < MENU_MODES && !((1 << bit) & modes); bit++)
	    ;
	ଯଦି (bit < MENU_MODES) // just in case, avoid Coverity warning
	{
	    ଯଦି (menu->strings[bit] != ଖାଲି)
	    {
		ଯଦି (*menu->strings[bit] == NUL)
		    status = dict_add_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		ଅନ୍ୟଥା
		{
		    char_u *tofree = ଖାଲି;

		    status = dict_add_string(dict, "rhs",
			tofree = str2special_save(menu->strings[bit], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    ଯଦି (status == OK)
		status = dict_add_bool(dict, "noremenu",
					     menu->noremap[bit] == REMAP_NONE);
	    ଯଦି (status == OK)
		status = dict_add_bool(dict, "script",
					   menu->noremap[bit] == REMAP_SCRIPT);
	    ଯଦି (status == OK)
		status = dict_add_bool(dict, "silent", menu->silent[bit]);
	    ଯଦି (status == OK)
		status = dict_add_bool(dict, "enabled",
					  ((menu->enabled & (1 << bit)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    ଯଦି (status == OK && menu->children != ଖାଲି)
    {
	vimmenu_T	*child;

	l = list_alloc();
	ଯଦି (l == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (dict_add_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ଫେରନ୍ତୁ FAIL;
	}
	child = menu->children;
	ଯେତେବେଳେ (child)
	{
	    ଯଦି (!menu_is_tearoff(child->dname))		// skip tearoff menu
		list_append_string(l, child->dname, -1);
	    child = child->next;
	}
    }

    ଫେରନ୍ତୁ status;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    ଶୂନ୍ୟ
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    retdict = rettv->vval.v_dict;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    menu_name = tv_get_string_chk(&argvars[0]);
    ଯଦି (menu_name == ଖାଲି)
	ଫେରନ୍ତୁ;

    // menu mode
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    ଅନ୍ୟଥା
	which = (char_u *)"";	    // Default is modes for "menu"
    ଯଦି (which == ଖାଲି)
	ଫେରନ୍ତୁ;

    modes = get_menu_cmd_modes(which, *which == '!', ଖାଲି, ଖାଲି);

    // Locate the specified menu or menu item
    menu = *get_ମୂଳ_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    ଯଦି (saved_name == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	ଯେତେବେଳେ (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    ଯେତେବେଳେ (menu != ଖାଲି)
	    {
		ଯଦି (menu_name_equal(name, menu))
		    ଭାଙ୍ଗ;
		menu = menu->next;
	    }
	    ଯଦି (menu == ଖାଲି || *p == NUL)
		ଭାଙ୍ଗ;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    ଯଦି (menu == ଖାଲି)		// specified menu not found
	ଫେରନ୍ତୁ;

    ଯଦି (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#endif // FEAT_MENU
