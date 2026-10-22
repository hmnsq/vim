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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_MENU)

#ವ್ಯಾಖ್ಯಾನಿಸು MENUDEPTH   10		// maximum depth of menus

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_menu_ಮಾರ್ಗ(char_u *, vimmenu_T *, ಪೂರ್ಣಾಂಕ *, char_u *, ಪೂರ್ಣಾಂಕ);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_menu_ಮಾರ್ಗ(char_u *, vimmenu_T *, ಪೂರ್ಣಾಂಕ *, char_u *);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ menu_nable_recurse(vimmenu_T *menu, char_u *name, ಪೂರ್ಣಾಂಕ modes, ಪೂರ್ಣಾಂಕ enable);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ remove_menu(vimmenu_T **, char_u *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ silent);
ಸ್ಥಿರ ಶೂನ್ಯ free_menu(vimmenu_T **menup);
ಸ್ಥಿರ ಶೂನ್ಯ free_menu_string(vimmenu_T *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ show_menus(char_u *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ show_menus_recursive(vimmenu_T *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ char_u *menu_name_skip(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ menu_name_equal(char_u *name, vimmenu_T *menu);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ menu_namecmp(char_u *name, char_u *mname);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_menu_cmd_modes(char_u *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ *, ಪೂರ್ಣಾಂಕ *);
ಸ್ಥಿರ char_u *popup_ಬಹುಲಕ_name(char_u *name, ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ char_u *menu_ಪಠ್ಯ(char_u *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ *mnemonic, char_u **actext);

#ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
ಸ್ಥಿರ ಶೂನ್ಯ gui_create_tearoffs_recurse(vimmenu_T *menu, ಸ್ಥಿರಾಂಕ char_u *pname, ಪೂರ್ಣಾಂಕ *pri_tab, ಪೂರ್ಣಾಂಕ pri_idx);
ಸ್ಥಿರ ಶೂನ್ಯ gui_ಕೂಡಿಸು_tearoff(char_u *tearpath, ಪೂರ್ಣಾಂಕ *pri_tab, ಪೂರ್ಣಾಂಕ pri_idx);
ಸ್ಥಿರ ಶೂನ್ಯ gui_destroy_tearoffs_recurse(vimmenu_T *menu);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_tearoffs = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ menu_is_hidden(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ menus_locked = 0;

#ಆದರೆ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
ಸ್ಥಿರ char_u *menu_skip_part(char_u *p);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
ಸ್ಥಿರ char_u *menutrans_lookup(char_u *name, ಪೂರ್ಣಾಂಕ len);
ಸ್ಥಿರ ಶೂನ್ಯ menu_unescape_name(char_u	*p);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ char_u *menu_translate_tab_and_ಸರಿಸು(char_u *arg_ಆರಂಭ);

// The character for each menu mode
ಸ್ಥಿರ ಅಕ್ಷರ *menu_ಬಹುಲಕ_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *toolbar_ಹೆಸರುಗಳು[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_ಎಣಿಕೆ ARRAY_ಉದ್ದಳತೆ(toolbar_ಹೆಸರುಗಳು)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
menu_is_winbar(char_u *name)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(name, "WinBar", 6) == 0);
}

    ಪೂರ್ಣಾಂಕ
winbar_ಎತ್ತರ(win_T *wp)
{
    ಆದರೆ (wp->w_winbar != ಖಾಲಿ && wp->w_winbar->children != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ vimmenu_T **
get_ಬೇರು_menu(char_u *name)
{
    ಆದರೆ (menu_is_winbar(name))
	ಹಿಂತಿರುಗಿ &curwin->w_winbar;
    ಹಿಂತಿರುಗಿ &ಬೇರು_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_menus_locked(ಶೂನ್ಯ)
{
    ಆದರೆ (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    ಶೂನ್ಯ
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_ಮಾರ್ಗ;
    ಪೂರ್ಣಾಂಕ		modes;
    char_u	*map_ಗಮ್ಯ;
    ಪೂರ್ಣಾಂಕ		noremap;
    ಪೂರ್ಣಾಂಕ		silent = FALSE;
    ಪೂರ್ಣಾಂಕ		special = FALSE;
    ಪೂರ್ಣಾಂಕ		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
#ಆದರೆ defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    ಪೂರ್ಣಾಂಕ		old_menu_ಎತ್ತರ;
# ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    ಪೂರ್ಣಾಂಕ		old_toolbar_ಎತ್ತರ;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		pri_tab[MENUDEPTH + 1];
    ಪೂರ್ಣಾಂಕ		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    char_u	*icon = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    vimmenu_T	menuarg;
    vimmenu_T	**ಬೇರು_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }


    // Locate an optional "icon=filename" argument.
    ಆದರೆ (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	icon = arg;
#ಆದರೆ_ಅಂತ್ಯ
	ಆಗ (*arg != NUL && *arg != ' ')
	{
	    ಆದರೆ (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	ಆದರೆ (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    ಗಾಗಿ (p = arg; *p; ++p)
	ಆದರೆ (!VIM_ISDIGIT(*p) && *p != '.')
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (VIM_ISWHITE(*p))
    {
	ಗಾಗಿ (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    ಆದರೆ (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    ಆದರೆ (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->addr_ಎಣಿಕೆ && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	i = 0;
    ಆಗ (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    ಆದರೆ (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    ಆದರೆ (*arg == NUL)
    {
	show_menus(arg, modes);
	ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    ಆದರೆ (menu_is_toolbar(arg))
    {
	menu_ಮಾರ್ಗ = menu_skip_part(arg);
	ಆದರೆ (*menu_ಮಾರ್ಗ == '.')
	{
	    p = menu_skip_part(++menu_ಮಾರ್ಗ);
	    ಆದರೆ (STRNCMP(menu_ಮಾರ್ಗ, "BuiltIn", 7) == 0)
	    {
		ಆದರೆ (skipdigits(menu_ಮಾರ್ಗ + 7) == p)
		{
		    menuarg.iconidx = atoi((ಅಕ್ಷರ *)menu_ಮಾರ್ಗ + 7);
		    ಆದರೆ (menuarg.iconidx >= (ಪೂರ್ಣಾಂಕ)TOOLBAR_NAME_ಎಣಿಕೆ)
			menuarg.iconidx = -1;
		    ಇಲ್ಲದಿದ್ದರೆ
			menuarg.icon_builtin = TRUE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)TOOLBAR_NAME_ಎಣಿಕೆ; ++i)
		    ಆದರೆ (STRNCMP(toolbar_ಹೆಸರುಗಳು[i], menu_ಮಾರ್ಗ, p - menu_ಮಾರ್ಗ)
									 == 0)
		    {
			menuarg.iconidx = i;
			ನಿಲ್ಲಿಸು;
		    }
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    menu_ಮಾರ್ಗ = arg;
    ಆದರೆ (*menu_ಮಾರ್ಗ == '.')
    {
	semsg(_(e_invalid_ವಾದ_str), menu_ಮಾರ್ಗ);
	ಹೋಗು theend;
    }

    map_ಗಮ್ಯ = menu_translate_tab_and_ಸರಿಸು(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    ಆದರೆ (*map_ಗಮ್ಯ == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_ಮಾರ್ಗ, modes);
	ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*map_ಗಮ್ಯ != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_ಗಮ್ಯ);
	ಹೋಗು theend;
    }
#ಆದರೆ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_ಎತ್ತರ = gui.menu_ಎತ್ತರ;
# ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_ಎತ್ತರ = gui.toolbar_ಎತ್ತರ;
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಬೇರು_menu_ptr = get_ಬೇರು_menu(menu_ಮಾರ್ಗ);
    ಆದರೆ (ಬೇರು_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    ಆದರೆ (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	ಆದರೆ (STRCMP(menu_ಮಾರ್ಗ, "*") == 0)	// meaning: do all menus
	    menu_ಮಾರ್ಗ = (char_u *)"";

	ಆದರೆ (menu_is_popup(menu_ಮಾರ್ಗ))
	{
	    ಗಾಗಿ (i = 0; i < MENU_ಸೂಚ್ಯಂಕ_TIP; ++i)
		ಆದರೆ (modes & (1 << i))
		{
		    p = popup_ಬಹುಲಕ_name(menu_ಮಾರ್ಗ, i);
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			menu_nable_recurse(*ಬೇರು_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*ಬೇರು_menu_ptr, menu_ಮಾರ್ಗ, modes, enable);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (unmenu)
    {
	ಆದರೆ (is_menus_locked())
	    ಹೋಗು theend;

	/*
	 * Delete menu(s).
	 */
	ಆದರೆ (STRCMP(menu_ಮಾರ್ಗ, "*") == 0)	// meaning: remove all menus
	    menu_ಮಾರ್ಗ = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	ಆದರೆ (menu_is_popup(menu_ಮಾರ್ಗ))
	{
	    ಗಾಗಿ (i = 0; i < MENU_ಸೂಚ್ಯಂಕ_TIP; ++i)
		ಆದರೆ (modes & (1 << i))
		{
		    p = popup_ಬಹುಲಕ_name(menu_ಮಾರ್ಗ, i);
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			remove_menu(ಬೇರು_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(ಬೇರು_menu_ptr, menu_ಮಾರ್ಗ, modes, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (is_menus_locked())
	    ಹೋಗು theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	ಆದರೆ (STRICMP(map_ಗಮ್ಯ, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_ಗಮ್ಯ = (char_u *)"";
	    map_buf = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modes & MENU_TIP_ಬಹುಲಕ)
	    map_buf = ಖಾಲಿ;	// Menu tips are plain text.
	ಇಲ್ಲದಿದ್ದರೆ
	    map_ಗಮ್ಯ = replace_termcodes(map_ಗಮ್ಯ, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ಖಾಲಿ);
	menuarg.modes = modes;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	menuarg.iconfile = icon;
#ಆದರೆ_ಅಂತ್ಯ
	menuarg.noremap[0] = noremap;
	menuarg.silent[0] = silent;
	ಕೂಡಿಸು_menu_ಮಾರ್ಗ(menu_ಮಾರ್ಗ, &menuarg, pri_tab, map_ಗಮ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
		, TRUE
#ಆದರೆ_ಅಂತ್ಯ
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	ಆದರೆ (menu_is_popup(menu_ಮಾರ್ಗ))
	{
	    ಗಾಗಿ (i = 0; i < MENU_ಸೂಚ್ಯಂಕ_TIP; ++i)
		ಆದರೆ (modes & (1 << i))
		{
		    p = popup_ಬಹುಲಕ_name(menu_ಮಾರ್ಗ, i);
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
			menuarg.iconfile = ಖಾಲಿ;
			menuarg.iconidx = -1;
			menuarg.icon_builtin = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
			ಕೂಡಿಸು_menu_ಮಾರ್ಗ(p, &menuarg, pri_tab, map_ಗಮ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
				, TRUE
#ಆದರೆ_ಅಂತ್ಯ
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

#ಆದರೆ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    ಆದರೆ (gui.ಒಳ_use
	    && (gui.menu_ಎತ್ತರ != old_menu_ಎತ್ತರ
# ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_ಎತ್ತರ != old_toolbar_ಎತ್ತರ
# endif
	    ))
	gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_VERT);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಬೇರು_menu_ptr == &curwin->w_winbar)
    {
	ಪೂರ್ಣಾಂಕ h = winbar_ಎತ್ತರ(curwin);

	ಆದರೆ (h != curwin->w_winbar_ಎತ್ತರ)
	{
	    ಆದರೆ (h == 0)
		++curwin->w_ಎತ್ತರ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_ಎತ್ತರ > 0)
		--curwin->w_ಎತ್ತರ;
	    curwin->w_winbar_ಎತ್ತರ = h;
	}
	curwin->w_prev_ಎತ್ತರ = curwin->w_ಎತ್ತರ;
    }

theend:
    ;
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_menu_ಮಾರ್ಗ(
    char_u	*menu_ಮಾರ್ಗ,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    ಪೂರ್ಣಾಂಕ		*pri_tab,
    char_u	*call_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    , ಪೂರ್ಣಾಂಕ	addtearoff	// may add tearoff item
#ಆದರೆ_ಅಂತ್ಯ
    )
{
    char_u	*ಮಾರ್ಗ_name;
    ಪೂರ್ಣಾಂಕ		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = ಖಾಲಿ;
    vimmenu_T	*ಪೋಷಕ;
    vimmenu_T	**ಅಧೋ_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*ಮುಂದಿನ_name;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		d;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		new_idx;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		pri_idx = 0;
    ಪೂರ್ಣಾಂಕ		old_modes = 0;
    ಪೂರ್ಣಾಂಕ		amenu;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_ಗಮ್ಯ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    vimmenu_T	**ಬೇರು_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    ಮಾರ್ಗ_name = vim_strsave(menu_ಮಾರ್ಗ);
    ಆದರೆ (ಮಾರ್ಗ_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಬೇರು_menu_ptr = get_ಬೇರು_menu(menu_ಮಾರ್ಗ);
    menup = ಬೇರು_menu_ptr;
    ಪೋಷಕ = ಖಾಲಿ;
    name = ಮಾರ್ಗ_name;
    ಆಗ (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	ಮುಂದಿನ_name = menu_name_skip(name);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ	FEAT_MULTI_LANG
	map_ಗಮ್ಯ = menutrans_lookup(name, (ಪೂರ್ಣಾಂಕ)STRLEN(name));
	ಆದರೆ (map_ಗಮ್ಯ != ಖಾಲಿ)
	{
	    en_name = name;
	    name = map_ಗಮ್ಯ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    en_name = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	dname = menu_ಪಠ್ಯ(name, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (dname == ಖಾಲಿ)
	    ಹೋಗು erret;
	ಆದರೆ (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_ಖಾಲಿಗಳು_menu_name));
	    ಹೋಗು erret;
	}

	// See if it's already there
	ಅಧೋ_pri = menup;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	idx = 0;
	new_idx = 0;
#ಆದರೆ_ಅಂತ್ಯ
	menu = *menup;
	ಆಗ (menu != ಖಾಲಿ)
	{
	    ಆದರೆ (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		ಆದರೆ (*ಮುಂದಿನ_name == NUL && menu->children != ಖಾಲಿ)
		{
		    ಆದರೆ (!sys_menu)
			emsg(_(e_menu_ಮಾರ್ಗ_must_not_lead_ಗಮ್ಯ_sub_menu));
		    ಹೋಗು erret;
		}
		ಆದರೆ (*ಮುಂದಿನ_name != NUL && menu->children == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
			&& addtearoff
#ಆದರೆ_ಅಂತ್ಯ
			)
		{
		    ಆದರೆ (!sys_menu)
			emsg(_(e_part_ದ_menu_ಅಂಶ_ಮಾರ್ಗ_is_not_sub_menu));
		    ಹೋಗು erret;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    menup = &menu->ಮುಂದಿನ;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    ಆದರೆ (ಪೋಷಕ != ಖಾಲಿ || menu_is_menubar(menu->name))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		++idx;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (menu->ಆದ್ಯತೆ <= pri_tab[pri_idx])
		{
		    ಅಧೋ_pri = menup;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    new_idx = idx;
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
	    menu = menu->ಮುಂದಿನ;
	}

	ಆದರೆ (menu == ಖಾಲಿ)
	{
	    ಆದರೆ (*ಮುಂದಿನ_name == NUL && ಪೋಷಕ == ಖಾಲಿ)
	    {
		emsg(_(e_must_not_ಕೂಡಿಸು_menu_ವಸ್ತುಗಳು_directly_ಗಮ್ಯ_menu_bar));
		ಹೋಗು erret;
	    }

	    ಆದರೆ (menu_is_separator(dname) && *ಮುಂದಿನ_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_ದ_menu_ಮಾರ್ಗ));
		ಹೋಗು erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    ಆದರೆ (menu == ಖಾಲಿ)
		ಹೋಗು erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_ಪಠ್ಯ(name, &menu->mnemonic, &menu->actext);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ	FEAT_MULTI_LANG
	    ಆದರೆ (en_name != ಖಾಲಿ)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_ಪಠ್ಯ(en_name, ಖಾಲಿ, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		menu->en_name = ಖಾಲಿ;
		menu->en_dname = ಖಾಲಿ;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    menu->ಆದ್ಯತೆ = pri_tab[pri_idx];
	    menu->ಪೋಷಕ = ಪೋಷಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_TIP
	    menu->tip = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->ಮುಂದಿನ = *ಅಧೋ_pri;
	    *ಅಧೋ_pri = menu;

	    old_modes = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	    menu->iconidx = menuarg->iconidx;
	    menu->icon_builtin = menuarg->icon_builtin;
	    ಆದರೆ (*ಮುಂದಿನ_name == NUL && menuarg->iconfile != ಖಾಲಿ)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    ಆದರೆ (ಪೋಷಕ != ಖಾಲಿ && menu_is_tearoff(ಪೋಷಕ->children->dname))
		ಪೋಷಕ->children->modes |= modes;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    ಆದರೆ (addtearoff)
#ಆದರೆ_ಅಂತ್ಯ
	    {
		menu->modes |= modes;
		menu->enabled |= modes;
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	/*
	 * Add the menu item when it's used in one of the modes, but not when
	 * only a tooltip is defined.
	 */
	ಆದರೆ ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    ಆದರೆ (gui.ಒಳ_use)  // Otherwise it will be added when GUI starts
	    {
		ಆದರೆ (*ಮುಂದಿನ_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_ಕೂಡಿಸು_menu(menu, new_idx);
		}
	    }

# ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    ಆದರೆ (	addtearoff
		    && *ಮುಂದಿನ_name
		    && vim_strchr(p_go, GO_TEAROFF) != ಖಾಲಿ
		    && menu_is_menubar(name)
#  ifdef VIMDLL
		    && (gui.ಒಳ_use || gui.starting)
#  endif
		    )
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_ಮಾರ್ಗ) + TEAR_LEN + 2);
		ಆದರೆ (tearpath != ಖಾಲಿ)
		{
		    char_u  *s;
		    ಪೂರ್ಣಾಂಕ	    len;

		    STRCPY(tearpath, menu_ಮಾರ್ಗ);
		    len = (ಪೂರ್ಣಾಂಕ)(ಮುಂದಿನ_name - ಮಾರ್ಗ_name - 1);
		    ಗಾಗಿ (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			ಆದರೆ ((*s == '\\' || *s == Ctrl_V) && s[1])
			{
			    ++len;
			    ++s;
			}
		    }
		    tearpath[len] = NUL;
		    gui_ಕೂಡಿಸು_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
# endif
	}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI

	menup = &menu->children;
	ಪೋಷಕ = menu;
	name = ಮುಂದಿನ_name;
	VIM_CLEAR(dname);
	ಆದರೆ (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(ಮಾರ್ಗ_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ | MENU_ಸೇರಿಸು_ಬಹುಲಕ)) ==
				       (MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ | MENU_ಸೇರಿಸು_ಬಹುಲಕ));
    ಆದರೆ (sys_menu)
	modes &= ~old_modes;

    ಆದರೆ (menu != ಖಾಲಿ && modes)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	menu->cb = gui_menu_cb;
#ಆದರೆ_ಅಂತ್ಯ
	p = (call_ದತ್ತಾಂಶ == ಖಾಲಿ) ? ಖಾಲಿ : vim_strsave(call_ದತ್ತಾಂಶ);

	// loop over all modes, may add more than one
	ಗಾಗಿ (i = 0; i < MENU_MODES; ++i)
	{
	    ಆದರೆ (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		ಆದರೆ (amenu && call_ದತ್ತಾಂಶ != ಖಾಲಿ && *call_ದತ್ತಾಂಶ != NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
		       && addtearoff
#ಆದರೆ_ಅಂತ್ಯ
				       )
		{
		    ಆಯ್ಕೆ (1 << i)
		    {
			ಸ್ಥಿತಿ MENU_VISUAL_ಬಹುಲಕ:
			ಸ್ಥಿತಿ MENU_SELECT_ಬಹುಲಕ:
			ಸ್ಥಿತಿ MENU_OP_PENDING_ಬಹುಲಕ:
			ಸ್ಥಿತಿ MENU_CMDLINE_ಬಹುಲಕ:
			    c = Ctrl_C;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MENU_ಸೇರಿಸು_ಬಹುಲಕ:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    ನಿಲ್ಲಿಸು;
		    }
		}

		ಆದರೆ (c != 0)
		{
		    menu->ಅಕ್ಷರಸರಣಿಗಳು[i] = alloc(STRLEN(call_ದತ್ತಾಂಶ) + 5);
		    ಆದರೆ (menu->ಅಕ್ಷರಸರಣಿಗಳು[i] != ಖಾಲಿ)
		    {
			menu->ಅಕ್ಷರಸರಣಿಗಳು[i][0] = c;
			ಆದರೆ (d == 0)
			    STRCPY(menu->ಅಕ್ಷರಸರಣಿಗಳು[i] + 1, call_ದತ್ತಾಂಶ);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    menu->ಅಕ್ಷರಸರಣಿಗಳು[i][1] = d;
			    STRCPY(menu->ಅಕ್ಷರಸರಣಿಗಳು[i] + 2, call_ದತ್ತಾಂಶ);
			}
			ಆದರೆ (c == Ctrl_C)
			{
			    ಪೂರ್ಣಾಂಕ	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(menu->ಅಕ್ಷರಸರಣಿಗಳು[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->ಅಕ್ಷರಸರಣಿಗಳು[i][len] = Ctrl_BSL;
			    menu->ಅಕ್ಷರಸರಣಿಗಳು[i][len + 1] = Ctrl_G;
			    menu->ಅಕ್ಷರಸರಣಿಗಳು[i][len + 2] = NUL;
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    menu->ಅಕ್ಷರಸರಣಿಗಳು[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	ಆದರೆ (modes & MENU_TIP_ಬಹುಲಕ)
	    gui_mch_menu_ಗಣ_tip(menu);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ OK;

erret:
    vim_free(ಮಾರ್ಗ_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    ಆಗ (ಪೋಷಕ != ಖಾಲಿ && ಪೋಷಕ->children == ಖಾಲಿ)
    {
	ಆದರೆ (ಪೋಷಕ->ಪೋಷಕ == ಖಾಲಿ)
	    menup = ಬೇರು_menu_ptr;
	ಇಲ್ಲದಿದ್ದರೆ
	    menup = &ಪೋಷಕ->ಪೋಷಕ->children;
	ಗಾಗಿ ( ; *menup != ಖಾಲಿ && *menup != ಪೋಷಕ; menup = &((*menup)->ಮುಂದಿನ))
	    ;
	ಆದರೆ (*menup == ಖಾಲಿ) // safety check
	    ನಿಲ್ಲಿಸು;
	ಪೋಷಕ = ಪೋಷಕ->ಪೋಷಕ;
	free_menu(menup);
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		modes,
    ಪೂರ್ಣಾಂಕ		enable)
{
    char_u	*p;

    ಆದರೆ (menu == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ಆಗ (menu != ಖಾಲಿ)
    {
	ಆದರೆ (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    ಆದರೆ (*p != NUL)
	    {
		ಆದರೆ (menu->children == ಖಾಲಿ)
		{
		    emsg(_(e_part_ದ_menu_ಅಂಶ_ಮಾರ್ಗ_is_not_sub_menu));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (enable)
		    menu->enabled |= modes;
		ಇಲ್ಲದಿದ್ದರೆ
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    ಆದರೆ (*name != NUL && *name != '*')
		ನಿಲ್ಲಿಸು;
	}
	menu = menu->ಮುಂದಿನ;
    }
    ಆದರೆ (*name != NUL && *name != '*' && menu == ಖಾಲಿ)
    {
	semsg(_(e_no_menu_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		modes,
    ಪೂರ್ಣಾಂಕ		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*ಮಗು;
    char_u	*p;

    ಆದರೆ (*menup == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ಆಗ ((menu = *menup) != ಖಾಲಿ)
    {
	ಆದರೆ (*name == NUL || menu_name_equal(name, menu))
	{
	    ಆದರೆ (*p != NUL && menu->children == ಖಾಲಿ)
	    {
		ಆದರೆ (!silent)
		    emsg(_(e_part_ದ_menu_ಅಂಶ_ಮಾರ್ಗ_is_not_sub_menu));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ ((menu->modes & modes) != 0x0)
	    {
#ಆದರೆ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		ಆದರೆ (*p == NUL && modes == MENU_ALL_MODES)
		{
		    ಆದರೆ (Is_ಕಿಟಕಿ(menu->tearoff_handle))
			Destroy_ಕಿಟಕಿ(menu->tearoff_handle);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*name != NUL)
	    {
		ಆದರೆ (!silent)
		    emsg(_(e_menu_only_exists_ಒಳ_another_ಬಹುಲಕ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    ಆದರೆ (*name != NUL)
		ನಿಲ್ಲಿಸು;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    ಆದರೆ (modes & MENU_TIP_ಬಹುಲಕ)
		free_menu_string(menu, MENU_ಸೂಚ್ಯಂಕ_TIP);
	    ಆದರೆ ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    ಇಲ್ಲದಿದ್ದರೆ
		menup = &menu->ಮುಂದಿನ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    menup = &menu->ಮುಂದಿನ;
    }
    ಆದರೆ (*name != NUL)
    {
	ಆದರೆ (menu == ಖಾಲಿ)
	{
	    ಆದರೆ (!silent)
		semsg(_(e_no_menu_str), name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#ಆದರೆ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	ಆದರೆ ((s_tearoffs) && (menu->children != ಖಾಲಿ)) // there's a tear bar.
	    ಮಗು = menu->children->ಮುಂದಿನ; // don't count tearoff bar
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಮಗು = menu->children;
	ಗಾಗಿ ( ; ಮಗು != ಖಾಲಿ; ಮಗು = ಮಗು->ಮುಂದಿನ)
	    menu->modes |= ಮಗು->modes;
	ಆದರೆ (modes & MENU_TIP_ಬಹುಲಕ)
	{
	    free_menu_string(menu, MENU_ಸೂಚ್ಯಂಕ_TIP);
#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    ಆದರೆ (gui.ಒಳ_use)
		gui_mch_menu_ಗಣ_tip(menu);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#ಆದರೆ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    ಆದರೆ (s_tearoffs && menu->children != ಖಾಲಿ) // there's a tear bar.
		free_menu(&menu->children);
#ಆದರೆ_ಅಂತ್ಯ
	    *menup = menu;
	    free_menu(menup);
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    ಶೂನ್ಯ
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_ವಸ್ತುಗಳು);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_menu(vimmenu_T **menup)
{
    ಪೂರ್ಣಾಂಕ		i;
    vimmenu_T	*menu;

    menu = *menup;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    ಆದರೆ (gui.ಒಳ_use)
	gui_mch_destroy_menu(menu);
#ಆದರೆ_ಅಂತ್ಯ

    // Don't change *menup until after calling gui_mch_destroy_menu(). The
    // MacOS code needs the original structure to properly delete the menu.
    *menup = menu->ಮುಂದಿನ;
    vim_free(menu->name);
    vim_free(menu->dname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    vim_free(menu->en_name);
    vim_free(menu->en_dname);
#ಆದರೆ_ಅಂತ್ಯ
    vim_free(menu->actext);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    vim_free(menu->iconfile);
# ifdef FEAT_GUI_MOTIF
    vim_free(menu->xpm_fname);
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (i = 0; i < MENU_MODES; i++)
	free_menu_string(menu, i);
    vim_free(menu);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Free the menu->string with the given index.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_menu_string(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < MENU_MODES; i++)
	ಆದರೆ (menu->ಅಕ್ಷರಸರಣಿಗಳು[i] == menu->ಅಕ್ಷರಸರಣಿಗಳು[idx])
	    ಎಣಿಕೆ_2++;
    ಆದರೆ (ಎಣಿಕೆ_2 == 1)
	vim_free(menu->ಅಕ್ಷರಸರಣಿಗಳು[idx]);
    menu->ಅಕ್ಷರಸರಣಿಗಳು[idx] = ಖಾಲಿ;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
show_menus(char_u *ಮಾರ್ಗ_name, ಪೂರ್ಣಾಂಕ modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*ಪೋಷಕ = ಖಾಲಿ;

    name = ಮಾರ್ಗ_name = vim_strsave(ಮಾರ್ಗ_name);
    ಆದರೆ (ಮಾರ್ಗ_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    menu = *get_ಬೇರು_menu(ಮಾರ್ಗ_name);

    // First, find the (sub)menu with the given name
    ಆಗ (*name)
    {
	p = menu_name_skip(name);
	ಆಗ (menu != ಖಾಲಿ)
	{
	    ಆದರೆ (menu_name_equal(name, menu))
	    {
		// Found menu
		ಆದರೆ (*p != NUL && menu->children == ಖಾಲಿ)
		{
		    emsg(_(e_part_ದ_menu_ಅಂಶ_ಮಾರ್ಗ_is_not_sub_menu));
		    vim_free(ಮಾರ್ಗ_name);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_ಒಳ_another_ಬಹುಲಕ));
		    vim_free(ಮಾರ್ಗ_name);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    menu = menu->ಮುಂದಿನ;
	}
	ಆದರೆ (menu == ಖಾಲಿ)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(ಮಾರ್ಗ_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	name = p;
	ಪೋಷಕ = menu;
	menu = menu->children;
    }
    vim_free(ಮಾರ್ಗ_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    ಸಂದೇಶ_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(ಪೋಷಕ, modes, 0);

    --menus_locked;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
show_menus_recursive(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ modes, ಪೂರ್ಣಾಂಕ ಆಳ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಬಿಟ್;

    ಆದರೆ (menu != ಖಾಲಿ && (menu->modes & modes) == 0x0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (menu != ಖಾಲಿ)
    {
	ಸಂದೇಶ_putchar('\n');
	ಆದರೆ (got_int)		// "q" hit for "--more--"
	    ಹಿಂತಿರುಗಿ;
	ಗಾಗಿ (i = 0; i < ಆಳ; i++)
	    ಸಂದೇಶ_puts("  ");
	ಆದರೆ (menu->ಆದ್ಯತೆ)
	{
	    ಸಂದೇಶ_outnum((ದೀರ್ಘ)menu->ಆದ್ಯತೆ);
	    ಸಂದೇಶ_puts(" ");
	}
				// Same highlighting as for directories!?
	ಸಂದೇಶ_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    ಆದರೆ (menu != ಖಾಲಿ && menu->children == ಖಾಲಿ)
    {
	ಗಾಗಿ (ಬಿಟ್ = 0; ಬಿಟ್ < MENU_MODES; ಬಿಟ್++)
	    ಆದರೆ ((menu->modes & modes & (1 << ಬಿಟ್)) != 0)
	    {
		ಸಂದೇಶ_putchar('\n');
		ಆದರೆ (got_int)		// "q" hit for "--more--"
		    ಹಿಂತಿರುಗಿ;
		ಗಾಗಿ (i = 0; i < ಆಳ + 2; i++)
		    ಸಂದೇಶ_puts("  ");
		ಸಂದೇಶ_puts(menu_ಬಹುಲಕ_chars[ಬಿಟ್]);
		ಆದರೆ (menu->noremap[ಬಿಟ್] == REMAP_NONE)
		    ಸಂದೇಶ_putchar('*');
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (menu->noremap[ಬಿಟ್] == REMAP_SCRIPT)
		    ಸಂದೇಶ_putchar('&');
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_putchar(' ');
		ಆದರೆ (menu->silent[ಬಿಟ್])
		    ಸಂದೇಶ_putchar('s');
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_putchar(' ');
		ಆದರೆ ((menu->modes & menu->enabled & (1 << ಬಿಟ್)) == 0)
		    ಸಂದೇಶ_putchar('-');
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_putchar(' ');
		ಸಂದೇಶ_puts(" ");
		ಆದರೆ (*menu->ಅಕ್ಷರಸರಣಿಗಳು[ಬಿಟ್] == NUL)
		    ಸಂದೇಶ_puts_attr("<Nop>", HL_ATTR(HLF_8));
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_outtrans_special(menu->ಅಕ್ಷರಸರಣಿಗಳು[ಬಿಟ್], FALSE, 0);
	    }
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (menu == ಖಾಲಿ)
	{
	    menu = ಬೇರು_menu;
	    ಆಳ--;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	ಗಾಗಿ (; menu != ಖಾಲಿ && !got_int; menu = menu->ಮುಂದಿನ)
	    ಆದರೆ (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, ಆಳ + 1);
    }
}

/*
 * Used when expanding menu names.
 */
ಸ್ಥಿರ vimmenu_T	*expand_menu = ಖಾಲಿ;
ಸ್ಥಿರ vimmenu_T	*expand_menu_alt = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		expand_modes = 0x0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
ಗಣ_context_ಒಳ_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*ಮಾರ್ಗ_name = ಖಾಲಿ;
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		unmenu;
    vimmenu_T	*menu;
    ಪೂರ್ಣಾಂಕ		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    ಗಾಗಿ (p = arg; *p; ++p)
	ಆದರೆ (!VIM_ISDIGIT(*p) && *p != '.')
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (!VIM_ISWHITE(*p))
    {
	ಆದರೆ (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	ಇಲ್ಲದಿದ್ದರೆ
	    p = arg;
    }

    ಆಗ (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    ಗಾಗಿ (; *p && !VIM_ISWHITE(*p); ++p)
    {
	ಆದರೆ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    ಆದರೆ (expand_menus && VIM_ISWHITE(*p))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// TODO: check for next command?
    ಆದರೆ (*p == NUL)		// Complete the menu name
    {
	ಪೂರ್ಣಾಂಕ try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, ಖಾಲಿ, &unmenu);
	ಆದರೆ (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = ಬೇರು_menu;
	ಆದರೆ (after_dot != arg)
	{
	    ಮಾರ್ಗ_name = alloc(after_dot - arg);
	    ಆದರೆ (ಮಾರ್ಗ_name == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    vim_strncpy(ಮಾರ್ಗ_name, arg, after_dot - arg - 1);
	}
	name = ಮಾರ್ಗ_name;
	ಆಗ (name != ಖಾಲಿ && *name)
	{
	    p = menu_name_skip(name);
	    ಆಗ (menu != ಖಾಲಿ)
	    {
		ಆದರೆ (menu_name_equal(name, menu))
		{
		    // Found menu
		    ಆದರೆ ((*p != NUL && menu->children == ಖಾಲಿ)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(ಮಾರ್ಗ_name);
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    }
		    ನಿಲ್ಲಿಸು;
		}
		menu = menu->ಮುಂದಿನ;
		ಆದರೆ (menu == ಖಾಲಿ && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    ಆದರೆ (menu == ಖಾಲಿ)
	    {
		// No menu found with the name we were looking for
		vim_free(ಮಾರ್ಗ_name);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(ಮಾರ್ಗ_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_ನಮೂನೆ = after_dot;
	expand_menu = menu;
	ಆದರೆ (expand_menu == ಬೇರು_menu)
	    expand_menu_alt = curwin->w_winbar;
	ಇಲ್ಲದಿದ್ದರೆ
	    expand_menu_alt = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ vimmenu_T	*menu = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		did_alt_menu = FALSE;
    char_u		*str;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಸ್ಥಿರ  ಪೂರ್ಣಾಂಕ		should_advance = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	should_advance = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Skip PopUp[nvoci].
    ಆಗ (menu != ಖಾಲಿ && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == ಖಾಲಿ))
    {
	menu = menu->ಮುಂದಿನ;
	ಆದರೆ (menu == ಖಾಲಿ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ಆದರೆ (menu == ಖಾಲಿ)	    // at end of linked list
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (menu->modes & expand_modes)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	ಆದರೆ (should_advance)
	    str = menu->en_dname;
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ಆದರೆ_ಅಂತ್ಯ
	    str = menu->dname;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	    ಆದರೆ (menu->en_dname == ಖಾಲಿ)
		should_advance = TRUE;
	}
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
	str = (char_u *)"";

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಆದರೆ (should_advance)
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Advance to next menu entry.
	menu = menu->ಮುಂದಿನ;
	ಆದರೆ (menu == ಖಾಲಿ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    should_advance = !should_advance;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_ಹೆಸರುಗಳು(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ vimmenu_T	*menu = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		did_alt_menu = FALSE;
#ವ್ಯಾಖ್ಯಾನಿಸು TBUFFER_LEN 256
    ಸ್ಥಿರ char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಸ್ಥಿರ  ಪೂರ್ಣಾಂಕ		should_advance = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	should_advance = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Skip Browse-style entries, popup menus and separators.
    ಆಗ (menu != ಖಾಲಿ
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#ಆದರೆ_ಅಂತ್ಯ
	       ))
    {
	menu = menu->ಮುಂದಿನ;
	ಆದರೆ (menu == ಖಾಲಿ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ಆದರೆ (menu == ಖಾಲಿ)	    // at end of linked list
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (menu->modes & expand_modes)
    {
	ಆದರೆ (menu->children != ಖಾಲಿ)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	    ಆದರೆ (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ಆದರೆ_ಅಂತ್ಯ
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
		ಆದರೆ (menu->en_dname == ಖಾಲಿ)
		    should_advance = TRUE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
	{
	    ಆದರೆ (should_advance)
		str = menu->en_dname;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ಆದರೆ_ಅಂತ್ಯ
		str = menu->dname;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
		ಆದರೆ (menu->en_dname == ಖಾಲಿ)
		    should_advance = TRUE;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
	str = (char_u *)"";

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಆದರೆ (should_advance)
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Advance to next menu entry.
	menu = menu->ಮುಂದಿನ;
	ಆದರೆ (menu == ಖಾಲಿ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    should_advance = !should_advance;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    ಸ್ಥಿರ char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    ಗಾಗಿ (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (*p)
	*p++ = NUL;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    ಆದರೆ (menu->en_name != ಖಾಲಿ
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
menu_namecmp(char_u *name, char_u *mname)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	ಆದರೆ (name[i] != mname[i])
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ ((name[i] == NUL || name[i] == TAB)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_menu_cmd_modes(
    char_u  *cmd,
    ಪೂರ್ಣಾಂಕ	    forceit,	    // Was there a "!" after the command?
    ಪೂರ್ಣಾಂಕ	    *noremap,
    ಪೂರ್ಣಾಂಕ	    *unmenu)
{
    ಪೂರ್ಣಾಂಕ	    modes;

    ಆಯ್ಕೆ (*cmd++)
    {
	ಸ್ಥಿತಿ 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_ಬಹುಲಕ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_ಬಹುಲಕ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'o':			// omenu
	    modes = MENU_OP_PENDING_ಬಹುಲಕ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'i':			// imenu
	    modes = MENU_ಸೇರಿಸು_ಬಹುಲಕ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 't':
	    ಆದರೆ (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_ಬಹುಲಕ;
		++cmd;
		ನಿಲ್ಲಿಸು;
	    }
	    modes = MENU_TIP_ಬಹುಲಕ;	// tmenu
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'c':			// cmenu
	    modes = MENU_CMDLINE_ಬಹುಲಕ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'a':			// amenu
	    modes = MENU_ಸೇರಿಸು_ಬಹುಲಕ | MENU_CMDLINE_ಬಹುಲಕ | MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ
				    | MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ
				    | MENU_OP_PENDING_ಬಹುಲಕ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'n':
	    ಆದರೆ (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
		ನಿಲ್ಲಿಸು;
	    }
	    // FALLTHROUGH
	ಪೂರ್ವನಿಯೋಜಿತ:
	    --cmd;
	    ಆದರೆ (forceit)		// menu!!
		modes = MENU_ಸೇರಿಸು_ಬಹುಲಕ | MENU_CMDLINE_ಬಹುಲಕ;
	    ಇಲ್ಲದಿದ್ದರೆ			// menu
		modes = MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ | MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ
						       | MENU_OP_PENDING_ಬಹುಲಕ;
    }

    ಆದರೆ (noremap != ಖಾಲಿ)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    ಆದರೆ (unmenu != ಖಾಲಿ)
	*unmenu = (*cmd == 'u');
    ಹಿಂತಿರುಗಿ modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ಸ್ಥಿರ char_u *
get_menu_ಬಹುಲಕ_str(ಪೂರ್ಣಾಂಕ modes)
{
    ಆದರೆ ((modes & (MENU_ಸೇರಿಸು_ಬಹುಲಕ | MENU_CMDLINE_ಬಹುಲಕ | MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ |
		  MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ | MENU_OP_PENDING_ಬಹುಲಕ))
	    == (MENU_ಸೇರಿಸು_ಬಹುಲಕ | MENU_CMDLINE_ಬಹುಲಕ | MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ |
		MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ | MENU_OP_PENDING_ಬಹುಲಕ))
	ಹಿಂತಿರುಗಿ (char_u *)"a";
    ಆದರೆ ((modes & (MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ | MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ |
		  MENU_OP_PENDING_ಬಹುಲಕ))
	    == (MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ | MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ |
		MENU_OP_PENDING_ಬಹುಲಕ))
	ಹಿಂತಿರುಗಿ (char_u *)" ";
    ಆದರೆ ((modes & (MENU_ಸೇರಿಸು_ಬಹುಲಕ | MENU_CMDLINE_ಬಹುಲಕ))
	    == (MENU_ಸೇರಿಸು_ಬಹುಲಕ | MENU_CMDLINE_ಬಹುಲಕ))
	ಹಿಂತಿರುಗಿ (char_u *)"!";
    ಆದರೆ ((modes & (MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ))
	    == (MENU_VISUAL_ಬಹುಲಕ | MENU_SELECT_ಬಹುಲಕ))
	ಹಿಂತಿರುಗಿ (char_u *)"v";
    ಆದರೆ (modes & MENU_VISUAL_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"x";
    ಆದರೆ (modes & MENU_SELECT_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"s";
    ಆದರೆ (modes & MENU_OP_PENDING_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"o";
    ಆದರೆ (modes & MENU_ಸೇರಿಸು_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"i";
    ಆದರೆ (modes & MENU_TERMINAL_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"tl";
    ಆದರೆ (modes & MENU_CMDLINE_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"c";
    ಆದರೆ (modes & MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"n";
    ಆದರೆ (modes & MENU_TIP_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ (char_u *)"t";

    ಹಿಂತಿರುಗಿ (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    ಸ್ಥಿರ char_u *
popup_ಬಹುಲಕ_name(char_u *name, ಪೂರ್ಣಾಂಕ idx)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)STRLEN(name);
    ಅಕ್ಷರ	*ಬಹುಲಕ_chars = menu_ಬಹುಲಕ_chars[idx];
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ_chars_len = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಬಹುಲಕ_chars);
    ಪೂರ್ಣಾಂಕ		i;

    p = vim_strnsave(name, len + ಬಹುಲಕ_chars_len);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    mch_memmove(p + 5 + ಬಹುಲಕ_chars_len, p + 5, (size_t)(len - 4));
    ಗಾಗಿ (i = 0; i < ಬಹುಲಕ_chars_len; ++i)
	p[5 + i] = menu_ಬಹುಲಕ_chars[idx][i];
    ಹಿಂತಿರುಗಿ p;
}

#ಆದರೆ defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    ಪೂರ್ಣಾಂಕ
get_menu_ಸೂಚ್ಯಂಕ(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ state)
{
    ಪೂರ್ಣಾಂಕ		idx;

    ಆದರೆ ((state & ಬಹುಲಕ_ಸೇರಿಸು))
	idx = MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state & ಬಹುಲಕ_CMDLINE)
	idx = MENU_ಸೂಚ್ಯಂಕ_CMDLINE;
# ifdef FEAT_TERMINAL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ_use_loop())
	idx = MENU_ಸೂಚ್ಯಂಕ_TERMINAL;
# endif
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (VIsual_select)
	    idx = MENU_ಸೂಚ್ಯಂಕ_SELECT;
	ಇಲ್ಲದಿದ್ದರೆ
	    idx = MENU_ಸೂಚ್ಯಂಕ_VISUAL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state == ಬಹುಲಕ_HITRETURN || state == ಬಹುಲಕ_ASKMORE)
	idx = MENU_ಸೂಚ್ಯಂಕ_CMDLINE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಕ್ತಾಯ_op)
	idx = MENU_ಸೂಚ್ಯಂಕ_OP_PENDING;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((state & ಬಹುಲಕ_ಸಾಮಾನ್ಯ))
	idx = MENU_ಸೂಚ್ಯಂಕ_ಸಾಮಾನ್ಯ;
    ಇಲ್ಲದಿದ್ದರೆ
	idx = MENU_ಸೂಚ್ಯಂಕ_INVALID;

    ಆದರೆ (idx != MENU_ಸೂಚ್ಯಂಕ_INVALID && menu->ಅಕ್ಷರಸರಣಿಗಳು[idx] == ಖಾಲಿ)
	idx = MENU_ಸೂಚ್ಯಂಕ_INVALID;
    ಹಿಂತಿರುಗಿ idx;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    ಸ್ಥಿರ char_u *
menu_ಪಠ್ಯ(char_u *str, ಪೂರ್ಣಾಂಕ *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*ಪಠ್ಯ;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	ಆದರೆ (actext != ಖಾಲಿ)
	    *actext = vim_strsave(p + 1);
	ಪಠ್ಯ = vim_strnsave(str, p - str);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಪಠ್ಯ = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    ಗಾಗಿ (p = ಪಠ್ಯ; p != ಖಾಲಿ; )
    {
	p = vim_strchr(p, '&');
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಆದರೆ (p[1] == NUL)	    // trailing "&"
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (mnemonic != ಖಾಲಿ && p[1] != '&')
#ಆದರೆ !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#ಇಲ್ಲದಿದ್ದರೆ
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		ಅಕ್ಷರ c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    ಹಿಂತಿರುಗಿ ಪಠ್ಯ;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    ಪೂರ್ಣಾಂಕ
menu_is_menubar(char_u *name)
{
    ಹಿಂತಿರುಗಿ (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    ಪೂರ್ಣಾಂಕ
menu_is_popup(char_u *name)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(name, "PopUp", 5) == 0);
}

#ಆದರೆ defined(FEAT_GUI_MOTIF) && (Xm_ಆವೃತ್ತಿ <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    ಪೂರ್ಣಾಂಕ
menu_is_ಮಗು_ದ_popup(vimmenu_T *menu)
{
    ಆಗ (menu->ಪೋಷಕ != ಖಾಲಿ)
	menu = menu->ಪೋಷಕ;
    ಹಿಂತಿರುಗಿ menu_is_popup(menu->name);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    ಪೂರ್ಣಾಂಕ
menu_is_toolbar(char_u *name)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    ಪೂರ್ಣಾಂಕ
menu_is_separator(char_u *name)
{
    ಹಿಂತಿರುಗಿ (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
menu_is_hidden(char_u *name)
{
    ಹಿಂತಿರುಗಿ (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
menu_is_tearoff(char_u *name ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಹಿಂತಿರುಗಿ (STRCMP(name, TEAR_STRING) == 0);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_ಪದ_POPUP_MENU)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_menu_ಬಹುಲಕ(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMINAL
    ಆದರೆ (ಪದ_use_loop())
	ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_TERMINAL;
# endif
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (VIsual_select)
	    ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_SELECT;
	ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_VISUAL;
    }
    ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು;
    ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE) || State == ಬಹುಲಕ_ASKMORE
						    || State == ಬಹುಲಕ_HITRETURN)
	ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_CMDLINE;
    ಆದರೆ (ಮುಕ್ತಾಯ_op)
	ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_OP_PENDING;
    ಆದರೆ (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
	ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_ಸಾಮಾನ್ಯ;
    ಆದರೆ (State & ಬಹುಲಕ_LANGMAP)	// must be a "r" command, like Insert mode
	ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು;
    ಹಿಂತಿರುಗಿ MENU_ಸೂಚ್ಯಂಕ_INVALID;
}

    ಪೂರ್ಣಾಂಕ
get_menu_ಬಹುಲಕ_flag(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ಬಹುಲಕ = get_menu_ಬಹುಲಕ();

    ಆದರೆ (ಬಹುಲಕ == MENU_ಸೂಚ್ಯಂಕ_INVALID)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ 1 << ಬಹುಲಕ;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    ಶೂನ್ಯ
show_popupmenu(ಶೂನ್ಯ)
{
    vimmenu_T	*menu;
    ಪೂರ್ಣಾಂಕ		menu_ಬಹುಲಕ;
    ಅಕ್ಷರ*	ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ_len;

    menu_ಬಹುಲಕ = get_menu_ಬಹುಲಕ();
    ಆದರೆ (menu_ಬಹುಲಕ == MENU_ಸೂಚ್ಯಂಕ_INVALID)
	ಹಿಂತಿರುಗಿ;
    ಬಹುಲಕ = menu_ಬಹುಲಕ_chars[menu_ಬಹುಲಕ];
    ಬಹುಲಕ_len = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಬಹುಲಕ);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)ಬಹುಲಕ, ಖಾಲಿ, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	ಆದರೆ (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, ಬಹುಲಕ, ಬಹುಲಕ_len) == 0)
	    ನಿಲ್ಲಿಸು;

    // Only show a popup when it is defined and has entries
    ಆದರೆ (menu == ಖಾಲಿ || menu->children == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ defined(FEAT_GUI)
    ಆದರೆ (gui.ಒಳ_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# ಆದರೆ defined(FEAT_GUI) && defined(FEAT_ಪದ_POPUP_MENU)
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ಆದರೆ defined(FEAT_ಪದ_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    ಪೂರ್ಣಾಂಕ
check_menu_ಸೂಚಕ(vimmenu_T *ಬೇರು, vimmenu_T *menu_ಗಮ್ಯ_check)
{
    vimmenu_T	*p;

    ಗಾಗಿ (p = ಬೇರು; p != ಖಾಲಿ; p = p->ಮುಂದಿನ)
	ಆದರೆ (p == menu_ಗಮ್ಯ_check
		|| (p->children != ಖಾಲಿ
		    && check_menu_ಸೂಚಕ(p->children, menu_ಗಮ್ಯ_check) == OK))
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    ಶೂನ್ಯ
gui_create_initial_menus(vimmenu_T *menu)
{
    ಪೂರ್ಣಾಂಕ		idx = 0;

    ಆಗ (menu != ಖಾಲಿ)
    {
	// Don't add a menu when only a tip was defined.
	ಆದರೆ (menu->modes & MENU_ALL_MODES)
	{
	    ಆದರೆ (menu->children != ಖಾಲಿ)
	    {
		gui_mch_ಕೂಡಿಸು_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(menu, idx);
	}
	menu = menu->ಮುಂದಿನ;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_update_menus_recurse(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ)
{
    ಪೂರ್ಣಾಂಕ		grey;

    ಆಗ (menu)
    {
	ಆದರೆ ((menu->modes & menu->enabled & ಬಹುಲಕ)
# ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	ಆದರೆ (vim_strchr(p_go, GO_GREY) != ಖಾಲಿ || menu->ಪೋಷಕ == ಖಾಲಿ
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->ಪೋಷಕ->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	ಇಲ್ಲದಿದ್ದರೆ
	    gui_mch_menu_hidden(menu, grey);
	gui_update_menus_recurse(menu->children, ಬಹುಲಕ);
	menu = menu->ಮುಂದಿನ;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    ಶೂನ್ಯ
gui_update_menus(ಪೂರ್ಣಾಂಕ modes)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    prev_ಬಹುಲಕ = -1;
    ಪೂರ್ಣಾಂಕ		    ಬಹುಲಕ = 0;

    ಆದರೆ (modes != 0x0)
	ಬಹುಲಕ = modes;
    ಇಲ್ಲದಿದ್ದರೆ
	ಬಹುಲಕ = get_menu_ಬಹುಲಕ_flag();

    ಆದರೆ (force_menu_update || ಬಹುಲಕ != prev_ಬಹುಲಕ)
    {
	gui_update_menus_recurse(ಬೇರು_menu, ಬಹುಲಕ);
	gui_mch_draw_menubar();
	prev_ಬಹುಲಕ = ಬಹುಲಕ;
	force_menu_update = FALSE;
    }
}

# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    ಪೂರ್ಣಾಂಕ
gui_is_menu_shortcut(ಪೂರ್ಣಾಂಕ ಕೀಲಿ)
{
    vimmenu_T	*menu;

    ಆದರೆ (ಕೀಲಿ < 256)
	ಕೀಲಿ = TOLOWER_LOC(ಕೀಲಿ);
    FOR_ALL_MENUS(menu)
	ಆದರೆ (menu->mnemonic == ಕೀಲಿ
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == ಕೀಲಿ))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}
# endif
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI

#ಆದರೆ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಂಕ enable)
{
    ಪೂರ್ಣಾಂಕ		pri_tab[MENUDEPTH + 1];
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (enable)
    {
	ಗಾಗಿ (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(ಬೇರು_menu, (char_u *)"", pri_tab, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	gui_destroy_tearoffs_recurse(ಬೇರು_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ಸ್ಥಿರಾಂಕ char_u	*pname,
    ಪೂರ್ಣಾಂಕ			*pri_tab,
    ಪೂರ್ಣಾಂಕ			pri_idx)
{
    char_u	*newpname = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*s;
    char_u	*d;

    ಆದರೆ (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    ಆಗ (menu != ಖಾಲಿ)
    {
	ಆದರೆ (menu->children != ಖಾಲಿ && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(pname) + (ಪೂರ್ಣಾಂಕ)STRLEN(menu->name);
	    ಗಾಗಿ (s = menu->name; *s; ++s)
		ಆದರೆ (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    ಆದರೆ (newpname != ಖಾಲಿ)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		ಗಾಗಿ (s = menu->name; *s; ++s)
		{
		    ಆದರೆ (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		ಆದರೆ (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_ಕೂಡಿಸು_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			// remove TEAR_STRING
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->ಮುಂದಿನ;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_ಕೂಡಿಸು_tearoff(char_u *tearpath, ಪೂರ್ಣಾಂಕ *pri_tab, ಪೂರ್ಣಾಂಕ pri_idx)
{
    char_u	*tbuf;
    ಪೂರ್ಣಾಂಕ		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)STRLEN(tearpath));
    ಆದರೆ (tbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    tbuf[0] = K_SPECIAL;
    tbuf[1] = K_ಎರಡನೇ(K_TEAROFF);
    tbuf[2] = K_ಮೂರನೇ(K_TEAROFF);
    STRCPY(tbuf + 3, tearpath);
    STRCAT(tbuf + 3, "\r");

    STRCAT(tearpath, ".");
    STRCAT(tearpath, TEAR_STRING);

    // Priority of tear-off is always 1
    t = pri_tab[pri_idx + 1];
    pri_tab[pri_idx + 1] = 1;

# ifdef FEAT_TOOLBAR
    menuarg.iconfile = ಖಾಲಿ;
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
# endif
    menuarg.noremap[0] = REMAP_NONE;
    menuarg.silent[0] = TRUE;

    menuarg.modes = MENU_ALL_MODES;
    ಕೂಡಿಸು_menu_ಮಾರ್ಗ(tearpath, &menuarg, pri_tab, tbuf, FALSE);

    menuarg.modes = MENU_TIP_ಬಹುಲಕ;
    ಕೂಡಿಸು_menu_ಮಾರ್ಗ(tearpath, &menuarg, pri_tab,
	    (char_u *)_("Tear off this menu"), FALSE);

    pri_tab[pri_idx + 1] = t;
    vim_free(tbuf);
}

/*
 * Recursively destroy tearoff items
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    ಆಗ (menu)
    {
	ಆದರೆ (menu->children)
	{
	    // check if tearoff exists
	    ಆದರೆ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    ಆದರೆ (menu->children != ಖಾಲಿ) // if not the last one
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->ಮುಂದಿನ;
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_MSWIN && FEAT_TEAROFF

/*
 * Execute "menu".  Use by ":emenu" and the window toolbar.
 * "eap" is NULL for the window toolbar.
 * "mode_idx" specifies a MENU_INDEX_ value, use MENU_INDEX_INVALID to depend
 * on the current state.
 */
    ಶೂನ್ಯ
execute_menu(exarg_T *eap, vimmenu_T *menu, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_idx)
{
    ಪೂರ್ಣಾಂಕ		idx = ಬಹುಲಕ_idx;

    ಆದರೆ (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	ಆದರೆ (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ_use_loop())
	{
	    idx = MENU_ಸೂಚ್ಯಂಕ_TERMINAL;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	{
	    idx = MENU_ಸೂಚ್ಯಂಕ_VISUAL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap != ಖಾಲಿ && eap->addr_ಎಣಿಕೆ)
	{
	    pos_T	tpos;

	    idx = MENU_ಸೂಚ್ಯಂಕ_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    ಆದರೆ ((curbuf->b_visual.vi_ಆರಂಭ.lnum == eap->line1)
		    && (curbuf->b_visual.vi_ಕೊನೆ.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_ಬಹುಲಕ = curbuf->b_visual.vi_ಬಹುಲಕ;
		tpos = curbuf->b_visual.vi_ಕೊನೆ;
		curwin->w_cursor = curbuf->b_visual.vi_ಆರಂಭ;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Set it up for line-wise visual mode
		VIsual_ಬಹುಲಕ = 'V';
		curwin->w_cursor.lnum = eap->line1;
		curwin->w_cursor.ಲಂಬಸಾಲು = 1;
		tpos.lnum = eap->line2;
		tpos.ಲಂಬಸಾಲು = MAXCOL;
		tpos.coladd = 0;
	    }

	    // Activate visual mode
	    VIsual_ಸಕ್ರಿಯ = TRUE;
	    VIsual_reselect = TRUE;
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;

	    check_cursor();

	    // Adjust the cursor to make sure it is in the correct pos
	    // for exclusive mode
	    ಆದರೆ (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    ಆದರೆ (idx == MENU_ಸೂಚ್ಯಂಕ_INVALID || eap == ಖಾಲಿ)
	idx = MENU_ಸೂಚ್ಯಂಕ_ಸಾಮಾನ್ಯ;

    ಆದರೆ (menu->ಅಕ್ಷರಸರಣಿಗಳು[idx] != ಖಾಲಿ && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	ಆದರೆ (eap == ಖಾಲಿ || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_ಸಾಮಾನ್ಯ_busy;
	    ಆದರೆ (save_current_state(&save_state))
		exec_ಸಾಮಾನ್ಯ_cmd(menu->ಅಕ್ಷರಸರಣಿಗಳು[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_ಸಾಮಾನ್ಯ_busy;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ins_typebuf(menu->ಅಕ್ಷರಸರಣಿಗಳು[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap != ಖಾಲಿ)
    {
	char_u	*ಬಹುಲಕ;

	ಆಯ್ಕೆ (idx)
	{
	    ಸ್ಥಿತಿ MENU_ಸೂಚ್ಯಂಕ_VISUAL:
		ಬಹುಲಕ = (char_u *)"Visual";
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MENU_ಸೂಚ್ಯಂಕ_SELECT:
		ಬಹುಲಕ = (char_u *)"Select";
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MENU_ಸೂಚ್ಯಂಕ_OP_PENDING:
		ಬಹುಲಕ = (char_u *)"Op-pending";
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MENU_ಸೂಚ್ಯಂಕ_TERMINAL:
		ಬಹುಲಕ = (char_u *)"Terminal";
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು:
		ಬಹುಲಕ = (char_u *)"Insert";
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MENU_ಸೂಚ್ಯಂಕ_CMDLINE:
		ಬಹುಲಕ = (char_u *)"Cmdline";
		ನಿಲ್ಲಿಸು;
	    // case MENU_INDEX_TIP: cannot happen
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಬಹುಲಕ = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_ಬಹುಲಕ), ಬಹುಲಕ);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    ಸ್ಥಿರ vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    ಆದರೆ (saved_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    menu = *get_ಬೇರು_menu(saved_name);
    name = saved_name;
    ಆಗ (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	ಆಗ (menu != ಖಾಲಿ)
	{
	    ಆದರೆ (menu_name_equal(name, menu))
	    {
		ಆದರೆ (*p == NUL && menu->children != ಖಾಲಿ)
		{
		    emsg(_(e_menu_ಮಾರ್ಗ_must_lead_ಗಮ್ಯ_menu_ಅಂಶ));
		    gave_emsg = TRUE;
		    menu = ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != NUL && menu->children == ಖಾಲಿ)
		{
		    emsg(_(e_part_ದ_menu_ಅಂಶ_ಮಾರ್ಗ_is_not_sub_menu));
		    menu = ಖಾಲಿ;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    menu = menu->ಮುಂದಿನ;
	}
	ಆದರೆ (menu == ಖಾಲಿ || *p == NUL)
	    ನಿಲ್ಲಿಸು;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    ಆದರೆ (menu == ಖಾಲಿ)
    {
	ಆದರೆ (!gave_emsg)
	    semsg(_(e_menu_not_ಸಿಕ್ಕಿತು_str), name_arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    ಶೂನ್ಯ
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_INVALID;

    ಆದರೆ (arg[0] && VIM_ISWHITE(arg[1]))
    {
	ಆಯ್ಕೆ (arg[0])
	{
	    ಸ್ಥಿತಿ 'n': ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_ಸಾಮಾನ್ಯ; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'v': ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_VISUAL; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 's': ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_SELECT; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'o': ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_OP_PENDING; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 't': ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_TERMINAL; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'i': ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'c': ಬಹುಲಕ_idx = MENU_ಸೂಚ್ಯಂಕ_CMDLINE; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: semsg(_(e_invalid_ವಾದ_str), arg);
		     ಹಿಂತಿರುಗಿ;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    ಆದರೆ (menu == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Found the menu, so execute.
    execute_menu(eap, menu, ಬಹುಲಕ_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    ಶೂನ್ಯ
winbar_click(win_T *wp, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		idx;

    ಆದರೆ (wp->w_winbar_ವಸ್ತುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (idx = 0; wp->w_winbar_ವಸ್ತುಗಳು[idx].wb_menu != ಖಾಲಿ; ++idx)
    {
	winbar_ಅಂಶ_T *ಅಂಶ = &wp->w_winbar_ವಸ್ತುಗಳು[idx];

	ಆದರೆ (ಲಂಬಸಾಲು >= ಅಂಶ->wb_startcol && ಲಂಬಸಾಲು <= ಅಂಶ->wb_endcol)
	{
	    win_T   *save_curwin = ಖಾಲಿ;
	    pos_T   save_visual = VIsual;
	    ಪೂರ್ಣಾಂಕ	    save_visual_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;
	    ಪೂರ್ಣಾಂಕ	    save_visual_select = VIsual_select;
	    ಪೂರ್ಣಾಂಕ	    save_visual_reselect = VIsual_reselect;
	    ಪೂರ್ಣಾಂಕ	    save_visual_ಬಹುಲಕ = VIsual_ಬಹುಲಕ;

	    ಆದರೆ (wp != curwin)
	    {
		// Clicking in the window toolbar of a not-current window.
		// Make that window the current one and save Visual mode.
		save_curwin = curwin;
		VIsual_ಸಕ್ರಿಯ = FALSE;
		curwin = wp;
		curbuf = curwin->w_ಬಫರ್;
		check_cursor();
	    }

	    // Note: the command might close the current window.
	    execute_menu(ಖಾಲಿ, ಅಂಶ->wb_menu, -1);

	    ಆದರೆ (save_curwin != ಖಾಲಿ && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_ಬಫರ್;
		VIsual = save_visual;
		VIsual_ಸಕ್ರಿಯ = save_visual_ಸಕ್ರಿಯ;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_ಬಹುಲಕ = save_visual_ಬಹುಲಕ;
	    }
	    ಆದರೆ (!win_valid(wp))
		ನಿಲ್ಲಿಸು;
	}
    }
}

#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_ಪದ_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_ಕಂಡುಹಿಡಿ_menu(char_u *ಮಾರ್ಗ_name)
{
    vimmenu_T	*menu = ಖಾಲಿ;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_ಬೇರು_menu(ಮಾರ್ಗ_name);

    saved_name = vim_strsave(ಮಾರ್ಗ_name);
    ಆದರೆ (saved_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    name = saved_name;
    ಆಗ (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	ಆಗ (menu != ಖಾಲಿ)
	{
	    ಆದರೆ (menu_name_equal(name, menu))
	    {
		ಆದರೆ (menu->children == ಖಾಲಿ)
		{
		    // found a menu item instead of a sub-menu
		    ಆದರೆ (*p == NUL)
			emsg(_(e_menu_ಮಾರ್ಗ_must_lead_ಗಮ್ಯ_sub_menu));
		    ಇಲ್ಲದಿದ್ದರೆ
			emsg(_(e_part_ದ_menu_ಅಂಶ_ಮಾರ್ಗ_is_not_sub_menu));
		    menu = ಖಾಲಿ;
		    ಹೋಗು theend;
		}
		ಆದರೆ (*p == NUL)	    // found a full match
		    ಹೋಗು theend;
		ನಿಲ್ಲಿಸು;
	    }
	    menu = menu->ಮುಂದಿನ;
	}
	ಆದರೆ (menu == ಖಾಲಿ)	// didn't find it
	    ನಿಲ್ಲಿಸು;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    ಆದರೆ (menu == ಖಾಲಿ)
	emsg(_(e_menu_not_ಸಿಕ್ಕಿತು_check_menu_ಹೆಸರುಗಳು));
theend:
    vim_free(saved_name);
    ಹಿಂತಿರುಗಿ menu;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*ಗಮ್ಯ;		// translated name
} menutrans_T;

ಸ್ಥಿರ garray_T menutrans_ga = {0, 0, 0, 0, ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    ಶೂನ್ಯ
ex_menutranslate(exarg_T *eap ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    ಪೂರ್ಣಾಂಕ			i;
    char_u		*from, *from_noamp, *ಗಮ್ಯ;

    ಆದರೆ (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, ಗಾತ್ರ(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    ಆದರೆ (STRNCMP(arg, "clear", 5) == 0 && ತುದಿಗಳು_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_ದತ್ತಾಂಶ;
	ಗಾಗಿ (i = 0; i < menutrans_ga.ga_len; ++i)
	{
	    vim_free(tp[i].from);
	    vim_free(tp[i].from_noamp);
	    vim_free(tp[i].ಗಮ್ಯ);
	}
	ga_clear(&menutrans_ga);
# ifdef FEAT_EVAL
	// Delete all "menutrans_" global variables.
	del_menutrans_vars();
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	ಗಮ್ಯ = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(ಗಮ್ಯ);
	ಆದರೆ (arg == ಗಮ್ಯ || ತುದಿಗಳು_excmd2(eap->arg, from)
		      || ತುದಿಗಳು_excmd2(eap->arg, ಗಮ್ಯ)
		      || !ತುದಿಗಳು_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_ವಾದ));
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_ದತ್ತಾಂಶ;
		from = vim_strsave(from);
		ಆದರೆ (from != ಖಾಲಿ)
		{
		    from_noamp = menu_ಪಠ್ಯ(from, ಖಾಲಿ, ಖಾಲಿ);
		    ಗಮ್ಯ = vim_strnsave(ಗಮ್ಯ, arg - ಗಮ್ಯ);
		    ಆದರೆ (from_noamp != ಖಾಲಿ && ಗಮ್ಯ != ಖಾಲಿ)
		    {
			menu_translate_tab_and_ಸರಿಸು(from);
			menu_translate_tab_and_ಸರಿಸು(ಗಮ್ಯ);
			menu_unescape_name(from);
			menu_unescape_name(ಗಮ್ಯ);
			tp[menutrans_ga.ga_len].from = from;
			tp[menutrans_ga.ga_len].from_noamp = from_noamp;
			tp[menutrans_ga.ga_len].ಗಮ್ಯ = ಗಮ್ಯ;
			++menutrans_ga.ga_len;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			vim_free(from);
			vim_free(from_noamp);
			vim_free(ಗಮ್ಯ);
		    }
		}
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ಸ್ಥಿರ char_u *
menu_skip_part(char_u *p)
{
    ಆಗ (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	ಆದರೆ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    ಸ್ಥಿರ char_u *
menutrans_lookup(char_u *name, ಪೂರ್ಣಾಂಕ len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ			i;
    char_u		*dname;

    ಗಾಗಿ (i = 0; i < menutrans_ga.ga_len; ++i)
	ಆದರೆ (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    ಹಿಂತಿರುಗಿ tp[i].ಗಮ್ಯ;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_ಪಠ್ಯ(name, ಖಾಲಿ, ಖಾಲಿ);
    name[len] = i;
    ಆದರೆ (dname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < menutrans_ga.ga_len; ++i)
	ಆದರೆ (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    ಹಿಂತಿರುಗಿ tp[i].ಗಮ್ಯ;
	}
    vim_free(dname);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
menu_unescape_name(char_u *name)
{
    char_u  *p;

    ಗಾಗಿ (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	ಆದರೆ (*p == '\\')
	    STRMOVE(p, p + 1);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    ಸ್ಥಿರ char_u *
menu_translate_tab_and_ಸರಿಸು(char_u *arg_ಆರಂಭ)
{
    char_u	*arg = arg_ಆರಂಭ;

    ಆಗ (*arg && !VIM_ISWHITE(*arg))
    {
	ಆದರೆ ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    ಆದರೆ (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    ಹಿಂತಿರುಗಿ arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, ಪೂರ್ಣಾಂಕ modes, dict_T *dict)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    list_T	*l;

    ಆದರೆ (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	ಗಾಗಿ (topmenu = menu; topmenu != ಖಾಲಿ; topmenu = topmenu->ಮುಂದಿನ)
	    ಆದರೆ (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	ಹಿಂತಿರುಗಿ OK;

    ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string(dict, "name", menu->name);
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string(dict, "display", menu->dname);
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK && menu->actext != ಖಾಲಿ)
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string(dict, "accel", menu->actext);
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "priority", menu->ಆದ್ಯತೆ);
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string(dict, "modes",
					get_menu_ಬಹುಲಕ_str(menu->modes));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK && menu->iconfile != ಖಾಲಿ)
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string(dict, "icon", menu->iconfile);
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK && menu->iconidx >= 0)
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "iconidx", menu->iconidx);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
    {
	char_u	buf[NUMBUFLEN];
	ಪೂರ್ಣಾಂಕ	buflen;

	ಆದರೆ (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string_len(dict, "shortcut", buf, buflen);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK && menu->children == ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ		ಬಿಟ್;

	// Get the first mode in which the menu is available
	ಗಾಗಿ (ಬಿಟ್ = 0; ಬಿಟ್ < MENU_MODES && !((1 << ಬಿಟ್) & modes); ಬಿಟ್++)
	    ;
	ಆದರೆ (ಬಿಟ್ < MENU_MODES) // just in case, avoid Coverity warning
	{
	    ಆದರೆ (menu->ಅಕ್ಷರಸರಣಿಗಳು[ಬಿಟ್] != ಖಾಲಿ)
	    {
		ಆದರೆ (*menu->ಅಕ್ಷರಸರಣಿಗಳು[ಬಿಟ್] == NUL)
		    ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u *tofree = ಖಾಲಿ;

		    ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string(dict, "rhs",
			tofree = str2special_save(menu->ಅಕ್ಷರಸರಣಿಗಳು[ಬಿಟ್], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
		ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_bool(dict, "noremenu",
					     menu->noremap[ಬಿಟ್] == REMAP_NONE);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
		ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_bool(dict, "script",
					   menu->noremap[ಬಿಟ್] == REMAP_SCRIPT);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
		ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_bool(dict, "silent", menu->silent[ಬಿಟ್]);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
		ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_bool(dict, "enabled",
					  ((menu->enabled & (1 << ಬಿಟ್)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK && menu->children != ಖಾಲಿ)
    {
	vimmenu_T	*ಮಗು;

	l = list_alloc();
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (dict_ಕೂಡಿಸು_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಮಗು = menu->children;
	ಆಗ (ಮಗು)
	{
	    ಆದರೆ (!menu_is_tearoff(ಮಗು->dname))		// skip tearoff menu
		list_append_string(l, ಮಗು->dname, -1);
	    ಮಗು = ಮಗು->ಮುಂದಿನ;
	}
    }

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    ಶೂನ್ಯ
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    ಪೂರ್ಣಾಂಕ		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    retdict = rettv->vval.v_dict;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    menu_name = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (menu_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // menu mode
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    ಇಲ್ಲದಿದ್ದರೆ
	which = (char_u *)"";	    // Default is modes for "menu"
    ಆದರೆ (which == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    modes = get_menu_cmd_modes(which, *which == '!', ಖಾಲಿ, ಖಾಲಿ);

    // Locate the specified menu or menu item
    menu = *get_ಬೇರು_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    ಆದರೆ (saved_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	ಆಗ (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    ಆಗ (menu != ಖಾಲಿ)
	    {
		ಆದರೆ (menu_name_equal(name, menu))
		    ನಿಲ್ಲಿಸು;
		menu = menu->ಮುಂದಿನ;
	    }
	    ಆದರೆ (menu == ಖಾಲಿ || *p == NUL)
		ನಿಲ್ಲಿಸು;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    ಆದರೆ (menu == ಖಾಲಿ)		// specified menu not found
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU
