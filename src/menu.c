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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_MENU)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MENUDEPTH   10		// maximum depth of menus

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_menu_ਰਸਤਾ(char_u *, vimmenu_T *, ਪੂਰਨ *, char_u *, ਪੂਰਨ);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_menu_ਰਸਤਾ(char_u *, vimmenu_T *, ਪੂਰਨ *, char_u *);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ menu_nable_recurse(vimmenu_T *menu, char_u *name, ਪੂਰਨ modes, ਪੂਰਨ enable);
ਸਥਿਰ ਪੂਰਨ remove_menu(vimmenu_T **, char_u *, ਪੂਰਨ, ਪੂਰਨ silent);
ਸਥਿਰ ਖਾਲੀ free_menu(vimmenu_T **menup);
ਸਥਿਰ ਖਾਲੀ free_menu_string(vimmenu_T *, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ show_menus(char_u *, ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ show_menus_recursive(vimmenu_T *, ਪੂਰਨ, ਪੂਰਨ);
ਸਥਿਰ char_u *menu_name_skip(char_u *name);
ਸਥਿਰ ਪੂਰਨ menu_name_equal(char_u *name, vimmenu_T *menu);
ਸਥਿਰ ਪੂਰਨ menu_namecmp(char_u *name, char_u *mname);
ਸਥਿਰ ਪੂਰਨ get_menu_cmd_modes(char_u *, ਪੂਰਨ, ਪੂਰਨ *, ਪੂਰਨ *);
ਸਥਿਰ char_u *popup_ਬਹੁਲਕ_name(char_u *name, ਪੂਰਨ idx);
ਸਥਿਰ char_u *menu_ਪਾਠ(char_u *ਪਾਠ, ਪੂਰਨ *mnemonic, char_u **actext);

#ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
ਸਥਿਰ ਖਾਲੀ gui_create_tearoffs_recurse(vimmenu_T *menu, ਅਚਲ char_u *pname, ਪੂਰਨ *pri_tab, ਪੂਰਨ pri_idx);
ਸਥਿਰ ਖਾਲੀ gui_ਜੋੜੋ_tearoff(char_u *tearpath, ਪੂਰਨ *pri_tab, ਪੂਰਨ pri_idx);
ਸਥਿਰ ਖਾਲੀ gui_destroy_tearoffs_recurse(vimmenu_T *menu);
ਸਥਿਰ ਪੂਰਨ s_tearoffs = FALSE;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ menu_is_hidden(char_u *name);
ਸਥਿਰ ਪੂਰਨ menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
ਸਥਿਰ ਪੂਰਨ menus_locked = 0;

#ਜੇ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
ਸਥਿਰ char_u *menu_skip_part(char_u *p);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
ਸਥਿਰ char_u *menutrans_lookup(char_u *name, ਪੂਰਨ len);
ਸਥਿਰ ਖਾਲੀ menu_unescape_name(char_u	*p);
#ਜੇ_ਅੰਤ

ਸਥਿਰ char_u *menu_translate_tab_and_ਖਿਸਕਾਓ(char_u *arg_ਸ਼ੁਰੂ);

// The character for each menu mode
ਸਥਿਰ ਅੱਖਰ *menu_ਬਹੁਲਕ_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *toolbar_ਨਾਂ[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_ਗਿਣਤੀ ARRAY_ਲੰਮਾਈ(toolbar_ਨਾਂ)
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    ਸਥਿਰ ਪੂਰਨ
menu_is_winbar(char_u *name)
{
    ਵਾਪਸ (STRNCMP(name, "WinBar", 6) == 0);
}

    ਪੂਰਨ
winbar_ਉਚਾਈ(win_T *wp)
{
    ਜੇ (wp->w_winbar != ਨਿਰਾ && wp->w_winbar->children != ਨਿਰਾ)
	ਵਾਪਸ 1;
    ਵਾਪਸ 0;
}

    ਸਥਿਰ vimmenu_T **
get_ਜੜ੍ਹ_menu(char_u *name)
{
    ਜੇ (menu_is_winbar(name))
	ਵਾਪਸ &curwin->w_winbar;
    ਵਾਪਸ &ਜੜ੍ਹ_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
is_menus_locked(ਖਾਲੀ)
{
    ਜੇ (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    ਖਾਲੀ
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_ਰਸਤਾ;
    ਪੂਰਨ		modes;
    char_u	*map_ਮੰਜ਼ਿਲ;
    ਪੂਰਨ		noremap;
    ਪੂਰਨ		silent = FALSE;
    ਪੂਰਨ		special = FALSE;
    ਪੂਰਨ		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    ਪੂਰਨ		i;
#ਜੇ defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    ਪੂਰਨ		old_menu_ਉਚਾਈ;
# ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    ਪੂਰਨ		old_toolbar_ਉਚਾਈ;
# endif
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		pri_tab[MENUDEPTH + 1];
    ਪੂਰਨ		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    char_u	*icon = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    vimmenu_T	menuarg;
    vimmenu_T	**ਜੜ੍ਹ_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    ਲਈ (;;)
    {
	ਜੇ (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    ਜਾਰੀ;
	}
	ਜੇ (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    ਜਾਰੀ;
	}
	ਜੇ (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    ਜਾਰੀ;
	}
	ਰੋਕੋ;
    }


    // Locate an optional "icon=filename" argument.
    ਜੇ (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	icon = arg;
#ਜੇ_ਅੰਤ
	ਜਦੋਂ (*arg != NUL && *arg != ' ')
	{
	    ਜੇ (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	ਜੇ (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    ਲਈ (p = arg; *p; ++p)
	ਜੇ (!VIM_ISDIGIT(*p) && *p != '.')
	    ਰੋਕੋ;
    ਜੇ (VIM_ISWHITE(*p))
    {
	ਲਈ (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    ਜੇ (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    ਜੇ (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap->addr_ਗਿਣਤੀ && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    ਨਹੀਂਤਾਂ
	i = 0;
    ਜਦੋਂ (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    ਜੇ (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    ਜੇ (*arg == NUL)
    {
	show_menus(arg, modes);
	ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    ਜੇ (menu_is_toolbar(arg))
    {
	menu_ਰਸਤਾ = menu_skip_part(arg);
	ਜੇ (*menu_ਰਸਤਾ == '.')
	{
	    p = menu_skip_part(++menu_ਰਸਤਾ);
	    ਜੇ (STRNCMP(menu_ਰਸਤਾ, "BuiltIn", 7) == 0)
	    {
		ਜੇ (skipdigits(menu_ਰਸਤਾ + 7) == p)
		{
		    menuarg.iconidx = atoi((ਅੱਖਰ *)menu_ਰਸਤਾ + 7);
		    ਜੇ (menuarg.iconidx >= (ਪੂਰਨ)TOOLBAR_NAME_ਗਿਣਤੀ)
			menuarg.iconidx = -1;
		    ਨਹੀਂਤਾਂ
			menuarg.icon_builtin = TRUE;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਲਈ (i = 0; i < (ਪੂਰਨ)TOOLBAR_NAME_ਗਿਣਤੀ; ++i)
		    ਜੇ (STRNCMP(toolbar_ਨਾਂ[i], menu_ਰਸਤਾ, p - menu_ਰਸਤਾ)
									 == 0)
		    {
			menuarg.iconidx = i;
			ਰੋਕੋ;
		    }
	    }
	}
    }
#ਜੇ_ਅੰਤ

    menu_ਰਸਤਾ = arg;
    ਜੇ (*menu_ਰਸਤਾ == '.')
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), menu_ਰਸਤਾ);
	ਜਾਓ theend;
    }

    map_ਮੰਜ਼ਿਲ = menu_translate_tab_and_ਖਿਸਕਾਓ(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    ਜੇ (*map_ਮੰਜ਼ਿਲ == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_ਰਸਤਾ, modes);
	ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*map_ਮੰਜ਼ਿਲ != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_ਮੰਜ਼ਿਲ);
	ਜਾਓ theend;
    }
#ਜੇ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_ਉਚਾਈ = gui.menu_ਉਚਾਈ;
# ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_ਉਚਾਈ = gui.toolbar_ਉਚਾਈ;
# endif
#ਜੇ_ਅੰਤ

    ਜੜ੍ਹ_menu_ptr = get_ਜੜ੍ਹ_menu(menu_ਰਸਤਾ);
    ਜੇ (ਜੜ੍ਹ_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    ਜੇ (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	ਜੇ (STRCMP(menu_ਰਸਤਾ, "*") == 0)	// meaning: do all menus
	    menu_ਰਸਤਾ = (char_u *)"";

	ਜੇ (menu_is_popup(menu_ਰਸਤਾ))
	{
	    ਲਈ (i = 0; i < MENU_ਸੂਚਕਾਂਕ_TIP; ++i)
		ਜੇ (modes & (1 << i))
		{
		    p = popup_ਬਹੁਲਕ_name(menu_ਰਸਤਾ, i);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			menu_nable_recurse(*ਜੜ੍ਹ_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*ਜੜ੍ਹ_menu_ptr, menu_ਰਸਤਾ, modes, enable);
    }
    ਨਹੀਂਤਾਂ ਜੇ (unmenu)
    {
	ਜੇ (is_menus_locked())
	    ਜਾਓ theend;

	/*
	 * Delete menu(s).
	 */
	ਜੇ (STRCMP(menu_ਰਸਤਾ, "*") == 0)	// meaning: remove all menus
	    menu_ਰਸਤਾ = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	ਜੇ (menu_is_popup(menu_ਰਸਤਾ))
	{
	    ਲਈ (i = 0; i < MENU_ਸੂਚਕਾਂਕ_TIP; ++i)
		ਜੇ (modes & (1 << i))
		{
		    p = popup_ਬਹੁਲਕ_name(menu_ਰਸਤਾ, i);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			remove_menu(ਜੜ੍ਹ_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(ਜੜ੍ਹ_menu_ptr, menu_ਰਸਤਾ, modes, FALSE);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (is_menus_locked())
	    ਜਾਓ theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	ਜੇ (STRICMP(map_ਮੰਜ਼ਿਲ, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_ਮੰਜ਼ਿਲ = (char_u *)"";
	    map_buf = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (modes & MENU_TIP_ਬਹੁਲਕ)
	    map_buf = ਨਿਰਾ;	// Menu tips are plain text.
	ਨਹੀਂਤਾਂ
	    map_ਮੰਜ਼ਿਲ = replace_termcodes(map_ਮੰਜ਼ਿਲ, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), ਨਿਰਾ);
	menuarg.modes = modes;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	menuarg.iconfile = icon;
#ਜੇ_ਅੰਤ
	menuarg.noremap[0] = noremap;
	menuarg.silent[0] = silent;
	ਜੋੜੋ_menu_ਰਸਤਾ(menu_ਰਸਤਾ, &menuarg, pri_tab, map_ਮੰਜ਼ਿਲ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
		, TRUE
#ਜੇ_ਅੰਤ
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	ਜੇ (menu_is_popup(menu_ਰਸਤਾ))
	{
	    ਲਈ (i = 0; i < MENU_ਸੂਚਕਾਂਕ_TIP; ++i)
		ਜੇ (modes & (1 << i))
		{
		    p = popup_ਬਹੁਲਕ_name(menu_ਰਸਤਾ, i);
		    ਜੇ (p != ਨਿਰਾ)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
			menuarg.iconfile = ਨਿਰਾ;
			menuarg.iconidx = -1;
			menuarg.icon_builtin = FALSE;
#ਜੇ_ਅੰਤ
			ਜੋੜੋ_menu_ਰਸਤਾ(p, &menuarg, pri_tab, map_ਮੰਜ਼ਿਲ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
				, TRUE
#ਜੇ_ਅੰਤ
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

#ਜੇ defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    ਜੇ (gui.ਵਿੱਚ_use
	    && (gui.menu_ਉਚਾਈ != old_menu_ਉਚਾਈ
# ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_ਉਚਾਈ != old_toolbar_ਉਚਾਈ
# endif
	    ))
	gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_VERT);
#ਜੇ_ਅੰਤ
    ਜੇ (ਜੜ੍ਹ_menu_ptr == &curwin->w_winbar)
    {
	ਪੂਰਨ h = winbar_ਉਚਾਈ(curwin);

	ਜੇ (h != curwin->w_winbar_ਉਚਾਈ)
	{
	    ਜੇ (h == 0)
		++curwin->w_ਉਚਾਈ;
	    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_ਉਚਾਈ > 0)
		--curwin->w_ਉਚਾਈ;
	    curwin->w_winbar_ਉਚਾਈ = h;
	}
	curwin->w_prev_ਉਚਾਈ = curwin->w_ਉਚਾਈ;
    }

theend:
    ;
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_menu_ਰਸਤਾ(
    char_u	*menu_ਰਸਤਾ,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    ਪੂਰਨ		*pri_tab,
    char_u	*call_ਅੰਕੜੇ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    , ਪੂਰਨ	addtearoff	// may add tearoff item
#ਜੇ_ਅੰਤ
    )
{
    char_u	*ਰਸਤਾ_name;
    ਪੂਰਨ		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = ਨਿਰਾ;
    vimmenu_T	*ਮਾਪਾ;
    vimmenu_T	**ਹੇਠਲਾ_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*ਅਗਲਾ_name;
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;
    ਪੂਰਨ		d;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ		idx;
    ਪੂਰਨ		new_idx;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		pri_idx = 0;
    ਪੂਰਨ		old_modes = 0;
    ਪੂਰਨ		amenu;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_ਮੰਜ਼ਿਲ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    vimmenu_T	**ਜੜ੍ਹ_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    ਰਸਤਾ_name = vim_strsave(menu_ਰਸਤਾ);
    ਜੇ (ਰਸਤਾ_name == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੜ੍ਹ_menu_ptr = get_ਜੜ੍ਹ_menu(menu_ਰਸਤਾ);
    menup = ਜੜ੍ਹ_menu_ptr;
    ਮਾਪਾ = ਨਿਰਾ;
    name = ਰਸਤਾ_name;
    ਜਦੋਂ (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	ਅਗਲਾ_name = menu_name_skip(name);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ	FEAT_MULTI_LANG
	map_ਮੰਜ਼ਿਲ = menutrans_lookup(name, (ਪੂਰਨ)STRLEN(name));
	ਜੇ (map_ਮੰਜ਼ਿਲ != ਨਿਰਾ)
	{
	    en_name = name;
	    name = map_ਮੰਜ਼ਿਲ;
	}
	ਨਹੀਂਤਾਂ
	    en_name = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	dname = menu_ਪਾਠ(name, ਨਿਰਾ, ਨਿਰਾ);
	ਜੇ (dname == ਨਿਰਾ)
	    ਜਾਓ erret;
	ਜੇ (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_menu_name));
	    ਜਾਓ erret;
	}

	// See if it's already there
	ਹੇਠਲਾ_pri = menup;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	idx = 0;
	new_idx = 0;
#ਜੇ_ਅੰਤ
	menu = *menup;
	ਜਦੋਂ (menu != ਨਿਰਾ)
	{
	    ਜੇ (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		ਜੇ (*ਅਗਲਾ_name == NUL && menu->children != ਨਿਰਾ)
		{
		    ਜੇ (!sys_menu)
			emsg(_(e_menu_ਰਸਤਾ_must_not_lead_ਮੰਜ਼ਿਲ_sub_menu));
		    ਜਾਓ erret;
		}
		ਜੇ (*ਅਗਲਾ_name != NUL && menu->children == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
			&& addtearoff
#ਜੇ_ਅੰਤ
			)
		{
		    ਜੇ (!sys_menu)
			emsg(_(e_part_ਦਾ_menu_ਵਸਤੂ_ਰਸਤਾ_is_not_sub_menu));
		    ਜਾਓ erret;
		}
		ਰੋਕੋ;
	    }
	    menup = &menu->ਅਗਲਾ;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    ਜੇ (ਮਾਪਾ != ਨਿਰਾ || menu_is_menubar(menu->name))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		++idx;
#ਜੇ_ਅੰਤ
		ਜੇ (menu->ਤਰਜੀਹ <= pri_tab[pri_idx])
		{
		    ਹੇਠਲਾ_pri = menup;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    new_idx = idx;
#ਜੇ_ਅੰਤ
		}
	    }
	    menu = menu->ਅਗਲਾ;
	}

	ਜੇ (menu == ਨਿਰਾ)
	{
	    ਜੇ (*ਅਗਲਾ_name == NUL && ਮਾਪਾ == ਨਿਰਾ)
	    {
		emsg(_(e_must_not_ਜੋੜੋ_menu_ਚੀਜ਼ਾਂ_directly_ਮੰਜ਼ਿਲ_menu_bar));
		ਜਾਓ erret;
	    }

	    ਜੇ (menu_is_separator(dname) && *ਅਗਲਾ_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_ਦਾ_menu_ਰਸਤਾ));
		ਜਾਓ erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    ਜੇ (menu == ਨਿਰਾ)
		ਜਾਓ erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_ਪਾਠ(name, &menu->mnemonic, &menu->actext);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ	FEAT_MULTI_LANG
	    ਜੇ (en_name != ਨਿਰਾ)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_ਪਾਠ(en_name, ਨਿਰਾ, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		menu->en_name = ਨਿਰਾ;
		menu->en_dname = ਨਿਰਾ;
	    }
#ਜੇ_ਅੰਤ
	    menu->ਤਰਜੀਹ = pri_tab[pri_idx];
	    menu->ਮਾਪਾ = ਮਾਪਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_TIP
	    menu->tip = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->ਅਗਲਾ = *ਹੇਠਲਾ_pri;
	    *ਹੇਠਲਾ_pri = menu;

	    old_modes = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	    menu->iconidx = menuarg->iconidx;
	    menu->icon_builtin = menuarg->icon_builtin;
	    ਜੇ (*ਅਗਲਾ_name == NUL && menuarg->iconfile != ਨਿਰਾ)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    ਜੇ (ਮਾਪਾ != ਨਿਰਾ && menu_is_tearoff(ਮਾਪਾ->children->dname))
		ਮਾਪਾ->children->modes |= modes;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    ਜੇ (addtearoff)
#ਜੇ_ਅੰਤ
	    {
		menu->modes |= modes;
		menu->enabled |= modes;
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	/*
	 * Add the menu item when it's used in one of the modes, but not when
	 * only a tooltip is defined.
	 */
	ਜੇ ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    ਜੇ (gui.ਵਿੱਚ_use)  // Otherwise it will be added when GUI starts
	    {
		ਜੇ (*ਅਗਲਾ_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_ਜੋੜੋ_menu(menu, new_idx);
		}
	    }

# ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    ਜੇ (	addtearoff
		    && *ਅਗਲਾ_name
		    && vim_strchr(p_go, GO_TEAROFF) != ਨਿਰਾ
		    && menu_is_menubar(name)
#  ifdef VIMDLL
		    && (gui.ਵਿੱਚ_use || gui.starting)
#  endif
		    )
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_ਰਸਤਾ) + TEAR_LEN + 2);
		ਜੇ (tearpath != ਨਿਰਾ)
		{
		    char_u  *s;
		    ਪੂਰਨ	    len;

		    STRCPY(tearpath, menu_ਰਸਤਾ);
		    len = (ਪੂਰਨ)(ਅਗਲਾ_name - ਰਸਤਾ_name - 1);
		    ਲਈ (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			ਜੇ ((*s == '\\' || *s == Ctrl_V) && s[1])
			{
			    ++len;
			    ++s;
			}
		    }
		    tearpath[len] = NUL;
		    gui_ਜੋੜੋ_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
# endif
	}
#ਜੇ_ਅੰਤ // FEAT_GUI

	menup = &menu->children;
	ਮਾਪਾ = menu;
	name = ਅਗਲਾ_name;
	VIM_CLEAR(dname);
	ਜੇ (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(ਰਸਤਾ_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ | MENU_ਪਾਓ_ਬਹੁਲਕ)) ==
				       (MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ | MENU_ਪਾਓ_ਬਹੁਲਕ));
    ਜੇ (sys_menu)
	modes &= ~old_modes;

    ਜੇ (menu != ਨਿਰਾ && modes)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	menu->cb = gui_menu_cb;
#ਜੇ_ਅੰਤ
	p = (call_ਅੰਕੜੇ == ਨਿਰਾ) ? ਨਿਰਾ : vim_strsave(call_ਅੰਕੜੇ);

	// loop over all modes, may add more than one
	ਲਈ (i = 0; i < MENU_MODES; ++i)
	{
	    ਜੇ (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		ਜੇ (amenu && call_ਅੰਕੜੇ != ਨਿਰਾ && *call_ਅੰਕੜੇ != NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
		       && addtearoff
#ਜੇ_ਅੰਤ
				       )
		{
		    ਚੋਣ (1 << i)
		    {
			ਹਾਲਤ MENU_VISUAL_ਬਹੁਲਕ:
			ਹਾਲਤ MENU_SELECT_ਬਹੁਲਕ:
			ਹਾਲਤ MENU_OP_PENDING_ਬਹੁਲਕ:
			ਹਾਲਤ MENU_CMDLINE_ਬਹੁਲਕ:
			    c = Ctrl_C;
			    ਰੋਕੋ;
			ਹਾਲਤ MENU_ਪਾਓ_ਬਹੁਲਕ:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    ਰੋਕੋ;
		    }
		}

		ਜੇ (c != 0)
		{
		    menu->ਲੜੀਆਂ[i] = alloc(STRLEN(call_ਅੰਕੜੇ) + 5);
		    ਜੇ (menu->ਲੜੀਆਂ[i] != ਨਿਰਾ)
		    {
			menu->ਲੜੀਆਂ[i][0] = c;
			ਜੇ (d == 0)
			    STRCPY(menu->ਲੜੀਆਂ[i] + 1, call_ਅੰਕੜੇ);
			ਨਹੀਂਤਾਂ
			{
			    menu->ਲੜੀਆਂ[i][1] = d;
			    STRCPY(menu->ਲੜੀਆਂ[i] + 2, call_ਅੰਕੜੇ);
			}
			ਜੇ (c == Ctrl_C)
			{
			    ਪੂਰਨ	    len = (ਪੂਰਨ)STRLEN(menu->ਲੜੀਆਂ[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->ਲੜੀਆਂ[i][len] = Ctrl_BSL;
			    menu->ਲੜੀਆਂ[i][len + 1] = Ctrl_G;
			    menu->ਲੜੀਆਂ[i][len + 2] = NUL;
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		    menu->ਲੜੀਆਂ[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	ਜੇ (modes & MENU_TIP_ਬਹੁਲਕ)
	    gui_mch_menu_ਸੈੱਟ_tip(menu);
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ OK;

erret:
    vim_free(ਰਸਤਾ_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    ਜਦੋਂ (ਮਾਪਾ != ਨਿਰਾ && ਮਾਪਾ->children == ਨਿਰਾ)
    {
	ਜੇ (ਮਾਪਾ->ਮਾਪਾ == ਨਿਰਾ)
	    menup = ਜੜ੍ਹ_menu_ptr;
	ਨਹੀਂਤਾਂ
	    menup = &ਮਾਪਾ->ਮਾਪਾ->children;
	ਲਈ ( ; *menup != ਨਿਰਾ && *menup != ਮਾਪਾ; menup = &((*menup)->ਅਗਲਾ))
	    ;
	ਜੇ (*menup == ਨਿਰਾ) // safety check
	    ਰੋਕੋ;
	ਮਾਪਾ = ਮਾਪਾ->ਮਾਪਾ;
	free_menu(menup);
    }
    ਵਾਪਸ FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    ਸਥਿਰ ਪੂਰਨ
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    ਪੂਰਨ		modes,
    ਪੂਰਨ		enable)
{
    char_u	*p;

    ਜੇ (menu == ਨਿਰਾ)
	ਵਾਪਸ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ਜਦੋਂ (menu != ਨਿਰਾ)
    {
	ਜੇ (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    ਜੇ (*p != NUL)
	    {
		ਜੇ (menu->children == ਨਿਰਾ)
		{
		    emsg(_(e_part_ਦਾ_menu_ਵਸਤੂ_ਰਸਤਾ_is_not_sub_menu));
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
		ਜੇ (enable)
		    menu->enabled |= modes;
		ਨਹੀਂਤਾਂ
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    ਜੇ (*name != NUL && *name != '*')
		ਰੋਕੋ;
	}
	menu = menu->ਅਗਲਾ;
    }
    ਜੇ (*name != NUL && *name != '*' && menu == ਨਿਰਾ)
    {
	semsg(_(e_no_menu_str), name);
	ਵਾਪਸ FAIL;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    ਸਥਿਰ ਪੂਰਨ
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    ਪੂਰਨ		modes,
    ਪੂਰਨ		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*ਬੱਚਾ;
    char_u	*p;

    ਜੇ (*menup == ਨਿਰਾ)
	ਵਾਪਸ OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    ਜਦੋਂ ((menu = *menup) != ਨਿਰਾ)
    {
	ਜੇ (*name == NUL || menu_name_equal(name, menu))
	{
	    ਜੇ (*p != NUL && menu->children == ਨਿਰਾ)
	    {
		ਜੇ (!silent)
		    emsg(_(e_part_ਦਾ_menu_ਵਸਤੂ_ਰਸਤਾ_is_not_sub_menu));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ ((menu->modes & modes) != 0x0)
	    {
#ਜੇ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		ਜੇ (*p == NUL && modes == MENU_ALL_MODES)
		{
		    ਜੇ (Is_ਖਿੜਕੀ(menu->tearoff_handle))
			Destroy_ਖਿੜਕੀ(menu->tearoff_handle);
		}
#ਜੇ_ਅੰਤ
		ਜੇ (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*name != NUL)
	    {
		ਜੇ (!silent)
		    emsg(_(e_menu_only_exists_ਵਿੱਚ_another_ਬਹੁਲਕ));
		ਵਾਪਸ FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    ਜੇ (*name != NUL)
		ਰੋਕੋ;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    ਜੇ (modes & MENU_TIP_ਬਹੁਲਕ)
		free_menu_string(menu, MENU_ਸੂਚਕਾਂਕ_TIP);
	    ਜੇ ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    ਨਹੀਂਤਾਂ
		menup = &menu->ਅਗਲਾ;
	}
	ਨਹੀਂਤਾਂ
	    menup = &menu->ਅਗਲਾ;
    }
    ਜੇ (*name != NUL)
    {
	ਜੇ (menu == ਨਿਰਾ)
	{
	    ਜੇ (!silent)
		semsg(_(e_no_menu_str), name);
	    ਵਾਪਸ FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#ਜੇ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	ਜੇ ((s_tearoffs) && (menu->children != ਨਿਰਾ)) // there's a tear bar.
	    ਬੱਚਾ = menu->children->ਅਗਲਾ; // don't count tearoff bar
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਬੱਚਾ = menu->children;
	ਲਈ ( ; ਬੱਚਾ != ਨਿਰਾ; ਬੱਚਾ = ਬੱਚਾ->ਅਗਲਾ)
	    menu->modes |= ਬੱਚਾ->modes;
	ਜੇ (modes & MENU_TIP_ਬਹੁਲਕ)
	{
	    free_menu_string(menu, MENU_ਸੂਚਕਾਂਕ_TIP);
#ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    ਜੇ (gui.ਵਿੱਚ_use)
		gui_mch_menu_ਸੈੱਟ_tip(menu);
#ਜੇ_ਅੰਤ
	}
	ਜੇ ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#ਜੇ defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    ਜੇ (s_tearoffs && menu->children != ਨਿਰਾ) // there's a tear bar.
		free_menu(&menu->children);
#ਜੇ_ਅੰਤ
	    *menup = menu;
	    free_menu(menup);
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    ਖਾਲੀ
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_ਚੀਜ਼ਾਂ);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    ਸਥਿਰ ਖਾਲੀ
free_menu(vimmenu_T **menup)
{
    ਪੂਰਨ		i;
    vimmenu_T	*menu;

    menu = *menup;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_mch_destroy_menu(menu);
#ਜੇ_ਅੰਤ

    // Don't change *menup until after calling gui_mch_destroy_menu(). The
    // MacOS code needs the original structure to properly delete the menu.
    *menup = menu->ਅਗਲਾ;
    vim_free(menu->name);
    vim_free(menu->dname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    vim_free(menu->en_name);
    vim_free(menu->en_dname);
#ਜੇ_ਅੰਤ
    vim_free(menu->actext);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    vim_free(menu->iconfile);
# ifdef FEAT_GUI_MOTIF
    vim_free(menu->xpm_fname);
# endif
#ਜੇ_ਅੰਤ
    ਲਈ (i = 0; i < MENU_MODES; i++)
	free_menu_string(menu, i);
    vim_free(menu);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#ਜੇ_ਅੰਤ
}

/*
 * Free the menu->string with the given index.
 */
    ਸਥਿਰ ਖਾਲੀ
free_menu_string(vimmenu_T *menu, ਪੂਰਨ idx)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < MENU_MODES; i++)
	ਜੇ (menu->ਲੜੀਆਂ[i] == menu->ਲੜੀਆਂ[idx])
	    ਗਿਣਤੀ_2++;
    ਜੇ (ਗਿਣਤੀ_2 == 1)
	vim_free(menu->ਲੜੀਆਂ[idx]);
    menu->ਲੜੀਆਂ[idx] = ਨਿਰਾ;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    ਸਥਿਰ ਪੂਰਨ
show_menus(char_u *ਰਸਤਾ_name, ਪੂਰਨ modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*ਮਾਪਾ = ਨਿਰਾ;

    name = ਰਸਤਾ_name = vim_strsave(ਰਸਤਾ_name);
    ਜੇ (ਰਸਤਾ_name == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    menu = *get_ਜੜ੍ਹ_menu(ਰਸਤਾ_name);

    // First, find the (sub)menu with the given name
    ਜਦੋਂ (*name)
    {
	p = menu_name_skip(name);
	ਜਦੋਂ (menu != ਨਿਰਾ)
	{
	    ਜੇ (menu_name_equal(name, menu))
	    {
		// Found menu
		ਜੇ (*p != NUL && menu->children == ਨਿਰਾ)
		{
		    emsg(_(e_part_ਦਾ_menu_ਵਸਤੂ_ਰਸਤਾ_is_not_sub_menu));
		    vim_free(ਰਸਤਾ_name);
		    ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ ਜੇ ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_ਵਿੱਚ_another_ਬਹੁਲਕ));
		    vim_free(ਰਸਤਾ_name);
		    ਵਾਪਸ FAIL;
		}
		ਰੋਕੋ;
	    }
	    menu = menu->ਅਗਲਾ;
	}
	ਜੇ (menu == ਨਿਰਾ)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(ਰਸਤਾ_name);
	    ਵਾਪਸ FAIL;
	}
	name = p;
	ਮਾਪਾ = menu;
	menu = menu->children;
    }
    vim_free(ਰਸਤਾ_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    ਸੁਨੇਹਾ_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(ਮਾਪਾ, modes, 0);

    --menus_locked;
    ਵਾਪਸ OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ਸਥਿਰ ਖਾਲੀ
show_menus_recursive(vimmenu_T *menu, ਪੂਰਨ modes, ਪੂਰਨ ਡੂੰਘਾਈ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਦੋਆਧਾਰੀ_ਅੰਕ;

    ਜੇ (menu != ਨਿਰਾ && (menu->modes & modes) == 0x0)
	ਵਾਪਸ;

    ਜੇ (menu != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_putchar('\n');
	ਜੇ (got_int)		// "q" hit for "--more--"
	    ਵਾਪਸ;
	ਲਈ (i = 0; i < ਡੂੰਘਾਈ; i++)
	    ਸੁਨੇਹਾ_puts("  ");
	ਜੇ (menu->ਤਰਜੀਹ)
	{
	    ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)menu->ਤਰਜੀਹ);
	    ਸੁਨੇਹਾ_puts(" ");
	}
				// Same highlighting as for directories!?
	ਸੁਨੇਹਾ_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    ਜੇ (menu != ਨਿਰਾ && menu->children == ਨਿਰਾ)
    {
	ਲਈ (ਦੋਆਧਾਰੀ_ਅੰਕ = 0; ਦੋਆਧਾਰੀ_ਅੰਕ < MENU_MODES; ਦੋਆਧਾਰੀ_ਅੰਕ++)
	    ਜੇ ((menu->modes & modes & (1 << ਦੋਆਧਾਰੀ_ਅੰਕ)) != 0)
	    {
		ਸੁਨੇਹਾ_putchar('\n');
		ਜੇ (got_int)		// "q" hit for "--more--"
		    ਵਾਪਸ;
		ਲਈ (i = 0; i < ਡੂੰਘਾਈ + 2; i++)
		    ਸੁਨੇਹਾ_puts("  ");
		ਸੁਨੇਹਾ_puts(menu_ਬਹੁਲਕ_chars[ਦੋਆਧਾਰੀ_ਅੰਕ]);
		ਜੇ (menu->noremap[ਦੋਆਧਾਰੀ_ਅੰਕ] == REMAP_NONE)
		    ਸੁਨੇਹਾ_putchar('*');
		ਨਹੀਂਤਾਂ ਜੇ (menu->noremap[ਦੋਆਧਾਰੀ_ਅੰਕ] == REMAP_SCRIPT)
		    ਸੁਨੇਹਾ_putchar('&');
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_putchar(' ');
		ਜੇ (menu->silent[ਦੋਆਧਾਰੀ_ਅੰਕ])
		    ਸੁਨੇਹਾ_putchar('s');
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_putchar(' ');
		ਜੇ ((menu->modes & menu->enabled & (1 << ਦੋਆਧਾਰੀ_ਅੰਕ)) == 0)
		    ਸੁਨੇਹਾ_putchar('-');
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_putchar(' ');
		ਸੁਨੇਹਾ_puts(" ");
		ਜੇ (*menu->ਲੜੀਆਂ[ਦੋਆਧਾਰੀ_ਅੰਕ] == NUL)
		    ਸੁਨੇਹਾ_puts_attr("<Nop>", HL_ATTR(HLF_8));
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_outtrans_special(menu->ਲੜੀਆਂ[ਦੋਆਧਾਰੀ_ਅੰਕ], FALSE, 0);
	    }
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (menu == ਨਿਰਾ)
	{
	    menu = ਜੜ੍ਹ_menu;
	    ਡੂੰਘਾਈ--;
	}
	ਨਹੀਂਤਾਂ
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	ਲਈ (; menu != ਨਿਰਾ && !got_int; menu = menu->ਅਗਲਾ)
	    ਜੇ (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, ਡੂੰਘਾਈ + 1);
    }
}

/*
 * Used when expanding menu names.
 */
ਸਥਿਰ vimmenu_T	*expand_menu = ਨਿਰਾ;
ਸਥਿਰ vimmenu_T	*expand_menu_alt = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ		expand_modes = 0x0;
ਸਥਿਰ ਪੂਰਨ		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
ਸੈੱਟ_context_ਵਿੱਚ_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    ਪੂਰਨ		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*ਰਸਤਾ_name = ਨਿਰਾ;
    char_u	*name;
    ਪੂਰਨ		unmenu;
    vimmenu_T	*menu;
    ਪੂਰਨ		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    ਲਈ (p = arg; *p; ++p)
	ਜੇ (!VIM_ISDIGIT(*p) && *p != '.')
	    ਰੋਕੋ;

    ਜੇ (!VIM_ISWHITE(*p))
    {
	ਜੇ (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	ਨਹੀਂਤਾਂ
	    p = arg;
    }

    ਜਦੋਂ (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    ਲਈ (; *p && !VIM_ISWHITE(*p); ++p)
    {
	ਜੇ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	ਨਹੀਂਤਾਂ ਜੇ (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    ਜੇ (expand_menus && VIM_ISWHITE(*p))
	ਵਾਪਸ ਨਿਰਾ;	// TODO: check for next command?
    ਜੇ (*p == NUL)		// Complete the menu name
    {
	ਪੂਰਨ try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, ਨਿਰਾ, &unmenu);
	ਜੇ (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = ਜੜ੍ਹ_menu;
	ਜੇ (after_dot != arg)
	{
	    ਰਸਤਾ_name = alloc(after_dot - arg);
	    ਜੇ (ਰਸਤਾ_name == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;
	    vim_strncpy(ਰਸਤਾ_name, arg, after_dot - arg - 1);
	}
	name = ਰਸਤਾ_name;
	ਜਦੋਂ (name != ਨਿਰਾ && *name)
	{
	    p = menu_name_skip(name);
	    ਜਦੋਂ (menu != ਨਿਰਾ)
	    {
		ਜੇ (menu_name_equal(name, menu))
		{
		    // Found menu
		    ਜੇ ((*p != NUL && menu->children == ਨਿਰਾ)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(ਰਸਤਾ_name);
			ਵਾਪਸ ਨਿਰਾ;
		    }
		    ਰੋਕੋ;
		}
		menu = menu->ਅਗਲਾ;
		ਜੇ (menu == ਨਿਰਾ && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    ਜੇ (menu == ਨਿਰਾ)
	    {
		// No menu found with the name we were looking for
		vim_free(ਰਸਤਾ_name);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(ਰਸਤਾ_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_ਨਮੂਨਾ = after_dot;
	expand_menu = menu;
	ਜੇ (expand_menu == ਜੜ੍ਹ_menu)
	    expand_menu_alt = curwin->w_winbar;
	ਨਹੀਂਤਾਂ
	    expand_menu_alt = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਸਥਿਰ vimmenu_T	*menu = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ		did_alt_menu = FALSE;
    char_u		*str;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਸਥਿਰ  ਪੂਰਨ		should_advance = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	should_advance = FALSE;
#ਜੇ_ਅੰਤ
    }

    // Skip PopUp[nvoci].
    ਜਦੋਂ (menu != ਨਿਰਾ && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == ਨਿਰਾ))
    {
	menu = menu->ਅਗਲਾ;
	ਜੇ (menu == ਨਿਰਾ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ਜੇ (menu == ਨਿਰਾ)	    // at end of linked list
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (menu->modes & expand_modes)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	ਜੇ (should_advance)
	    str = menu->en_dname;
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਅੰਤ
	    str = menu->dname;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	    ਜੇ (menu->en_dname == ਨਿਰਾ)
		should_advance = TRUE;
	}
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
	str = (char_u *)"";

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਜੇ (should_advance)
#ਜੇ_ਅੰਤ
    {
	// Advance to next menu entry.
	menu = menu->ਅਗਲਾ;
	ਜੇ (menu == ਨਿਰਾ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    should_advance = !should_advance;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_ਨਾਂ(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਸਥਿਰ vimmenu_T	*menu = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ		did_alt_menu = FALSE;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TBUFFER_LEN 256
    ਸਥਿਰ char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਸਥਿਰ  ਪੂਰਨ		should_advance = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	should_advance = FALSE;
#ਜੇ_ਅੰਤ
    }

    // Skip Browse-style entries, popup menus and separators.
    ਜਦੋਂ (menu != ਨਿਰਾ
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#ਜੇ_ਅੰਤ
	       ))
    {
	menu = menu->ਅਗਲਾ;
	ਜੇ (menu == ਨਿਰਾ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    ਜੇ (menu == ਨਿਰਾ)	    // at end of linked list
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (menu->modes & expand_modes)
    {
	ਜੇ (menu->children != ਨਿਰਾ)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	    ਜੇ (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਅੰਤ
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
		ਜੇ (menu->en_dname == ਨਿਰਾ)
		    should_advance = TRUE;
	    }
#ਜੇ_ਅੰਤ
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
	{
	    ਜੇ (should_advance)
		str = menu->en_dname;
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਅੰਤ
		str = menu->dname;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
		ਜੇ (menu->en_dname == ਨਿਰਾ)
		    should_advance = TRUE;
	    }
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
	str = (char_u *)"";

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਜੇ (should_advance)
#ਜੇ_ਅੰਤ
    {
	// Advance to next menu entry.
	menu = menu->ਅਗਲਾ;
	ਜੇ (menu == ਨਿਰਾ && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    should_advance = !should_advance;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    ਸਥਿਰ char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    ਲਈ (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	}
    }
    ਜੇ (*p)
	*p++ = NUL;
    ਵਾਪਸ p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ਸਥਿਰ ਪੂਰਨ
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    ਜੇ (menu->en_name != ਨਿਰਾ
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ਸਥਿਰ ਪੂਰਨ
menu_namecmp(char_u *name, char_u *mname)
{
    ਪੂਰਨ		i;

    ਲਈ (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	ਜੇ (name[i] != mname[i])
	    ਰੋਕੋ;
    ਵਾਪਸ ((name[i] == NUL || name[i] == TAB)
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
    ਸਥਿਰ ਪੂਰਨ
get_menu_cmd_modes(
    char_u  *cmd,
    ਪੂਰਨ	    forceit,	    // Was there a "!" after the command?
    ਪੂਰਨ	    *noremap,
    ਪੂਰਨ	    *unmenu)
{
    ਪੂਰਨ	    modes;

    ਚੋਣ (*cmd++)
    {
	ਹਾਲਤ 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ;
	    ਰੋਕੋ;
	ਹਾਲਤ 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_ਬਹੁਲਕ;
	    ਰੋਕੋ;
	ਹਾਲਤ 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_ਬਹੁਲਕ;
	    ਰੋਕੋ;
	ਹਾਲਤ 'o':			// omenu
	    modes = MENU_OP_PENDING_ਬਹੁਲਕ;
	    ਰੋਕੋ;
	ਹਾਲਤ 'i':			// imenu
	    modes = MENU_ਪਾਓ_ਬਹੁਲਕ;
	    ਰੋਕੋ;
	ਹਾਲਤ 't':
	    ਜੇ (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_ਬਹੁਲਕ;
		++cmd;
		ਰੋਕੋ;
	    }
	    modes = MENU_TIP_ਬਹੁਲਕ;	// tmenu
	    ਰੋਕੋ;
	ਹਾਲਤ 'c':			// cmenu
	    modes = MENU_CMDLINE_ਬਹੁਲਕ;
	    ਰੋਕੋ;
	ਹਾਲਤ 'a':			// amenu
	    modes = MENU_ਪਾਓ_ਬਹੁਲਕ | MENU_CMDLINE_ਬਹੁਲਕ | MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ
				    | MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ
				    | MENU_OP_PENDING_ਬਹੁਲਕ;
	    ਰੋਕੋ;
	ਹਾਲਤ 'n':
	    ਜੇ (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
		ਰੋਕੋ;
	    }
	    // FALLTHROUGH
	ਮੂਲ:
	    --cmd;
	    ਜੇ (forceit)		// menu!!
		modes = MENU_ਪਾਓ_ਬਹੁਲਕ | MENU_CMDLINE_ਬਹੁਲਕ;
	    ਨਹੀਂਤਾਂ			// menu
		modes = MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ | MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ
						       | MENU_OP_PENDING_ਬਹੁਲਕ;
    }

    ਜੇ (noremap != ਨਿਰਾ)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    ਜੇ (unmenu != ਨਿਰਾ)
	*unmenu = (*cmd == 'u');
    ਵਾਪਸ modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ਸਥਿਰ char_u *
get_menu_ਬਹੁਲਕ_str(ਪੂਰਨ modes)
{
    ਜੇ ((modes & (MENU_ਪਾਓ_ਬਹੁਲਕ | MENU_CMDLINE_ਬਹੁਲਕ | MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ |
		  MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ | MENU_OP_PENDING_ਬਹੁਲਕ))
	    == (MENU_ਪਾਓ_ਬਹੁਲਕ | MENU_CMDLINE_ਬਹੁਲਕ | MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ |
		MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ | MENU_OP_PENDING_ਬਹੁਲਕ))
	ਵਾਪਸ (char_u *)"a";
    ਜੇ ((modes & (MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ | MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ |
		  MENU_OP_PENDING_ਬਹੁਲਕ))
	    == (MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ | MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ |
		MENU_OP_PENDING_ਬਹੁਲਕ))
	ਵਾਪਸ (char_u *)" ";
    ਜੇ ((modes & (MENU_ਪਾਓ_ਬਹੁਲਕ | MENU_CMDLINE_ਬਹੁਲਕ))
	    == (MENU_ਪਾਓ_ਬਹੁਲਕ | MENU_CMDLINE_ਬਹੁਲਕ))
	ਵਾਪਸ (char_u *)"!";
    ਜੇ ((modes & (MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ))
	    == (MENU_VISUAL_ਬਹੁਲਕ | MENU_SELECT_ਬਹੁਲਕ))
	ਵਾਪਸ (char_u *)"v";
    ਜੇ (modes & MENU_VISUAL_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"x";
    ਜੇ (modes & MENU_SELECT_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"s";
    ਜੇ (modes & MENU_OP_PENDING_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"o";
    ਜੇ (modes & MENU_ਪਾਓ_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"i";
    ਜੇ (modes & MENU_TERMINAL_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"tl";
    ਜੇ (modes & MENU_CMDLINE_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"c";
    ਜੇ (modes & MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"n";
    ਜੇ (modes & MENU_TIP_ਬਹੁਲਕ)
	ਵਾਪਸ (char_u *)"t";

    ਵਾਪਸ (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    ਸਥਿਰ char_u *
popup_ਬਹੁਲਕ_name(char_u *name, ਪੂਰਨ idx)
{
    char_u	*p;
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(name);
    ਅੱਖਰ	*ਬਹੁਲਕ_chars = menu_ਬਹੁਲਕ_chars[idx];
    ਪੂਰਨ		ਬਹੁਲਕ_chars_len = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(ਬਹੁਲਕ_chars);
    ਪੂਰਨ		i;

    p = vim_strnsave(name, len + ਬਹੁਲਕ_chars_len);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    mch_memmove(p + 5 + ਬਹੁਲਕ_chars_len, p + 5, (size_t)(len - 4));
    ਲਈ (i = 0; i < ਬਹੁਲਕ_chars_len; ++i)
	p[5 + i] = menu_ਬਹੁਲਕ_chars[idx][i];
    ਵਾਪਸ p;
}

#ਜੇ defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    ਪੂਰਨ
get_menu_ਸੂਚਕਾਂਕ(vimmenu_T *menu, ਪੂਰਨ state)
{
    ਪੂਰਨ		idx;

    ਜੇ ((state & ਬਹੁਲਕ_ਪਾਓ))
	idx = MENU_ਸੂਚਕਾਂਕ_ਪਾਓ;
    ਨਹੀਂਤਾਂ ਜੇ (state & ਬਹੁਲਕ_CMDLINE)
	idx = MENU_ਸੂਚਕਾਂਕ_CMDLINE;
# ifdef FEAT_TERMINAL
    ਨਹੀਂਤਾਂ ਜੇ (ਪਦ_use_loop())
	idx = MENU_ਸੂਚਕਾਂਕ_TERMINAL;
# endif
    ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (VIsual_select)
	    idx = MENU_ਸੂਚਕਾਂਕ_SELECT;
	ਨਹੀਂਤਾਂ
	    idx = MENU_ਸੂਚਕਾਂਕ_VISUAL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (state == ਬਹੁਲਕ_HITRETURN || state == ਬਹੁਲਕ_ASKMORE)
	idx = MENU_ਸੂਚਕਾਂਕ_CMDLINE;
    ਨਹੀਂਤਾਂ ਜੇ (ਅੰਤ_op)
	idx = MENU_ਸੂਚਕਾਂਕ_OP_PENDING;
    ਨਹੀਂਤਾਂ ਜੇ ((state & ਬਹੁਲਕ_ਸਾਧਾਰਨ))
	idx = MENU_ਸੂਚਕਾਂਕ_ਸਾਧਾਰਨ;
    ਨਹੀਂਤਾਂ
	idx = MENU_ਸੂਚਕਾਂਕ_INVALID;

    ਜੇ (idx != MENU_ਸੂਚਕਾਂਕ_INVALID && menu->ਲੜੀਆਂ[idx] == ਨਿਰਾ)
	idx = MENU_ਸੂਚਕਾਂਕ_INVALID;
    ਵਾਪਸ idx;
}
#ਜੇ_ਅੰਤ

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    ਸਥਿਰ char_u *
menu_ਪਾਠ(char_u *str, ਪੂਰਨ *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*ਪਾਠ;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    ਜੇ (p != ਨਿਰਾ)
    {
	ਜੇ (actext != ਨਿਰਾ)
	    *actext = vim_strsave(p + 1);
	ਪਾਠ = vim_strnsave(str, p - str);
    }
    ਨਹੀਂਤਾਂ
	ਪਾਠ = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    ਲਈ (p = ਪਾਠ; p != ਨਿਰਾ; )
    {
	p = vim_strchr(p, '&');
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਜੇ (p[1] == NUL)	    // trailing "&"
		ਰੋਕੋ;
	    ਜੇ (mnemonic != ਨਿਰਾ && p[1] != '&')
#ਜੇ !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#ਨਹੀਂਤਾਂ
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		ਅੱਖਰ c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#ਜੇ_ਅੰਤ
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    ਵਾਪਸ ਪਾਠ;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    ਪੂਰਨ
menu_is_menubar(char_u *name)
{
    ਵਾਪਸ (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    ਪੂਰਨ
menu_is_popup(char_u *name)
{
    ਵਾਪਸ (STRNCMP(name, "PopUp", 5) == 0);
}

#ਜੇ defined(FEAT_GUI_MOTIF) && (Xm_ਸੰਸਕਰਣ <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    ਪੂਰਨ
menu_is_ਬੱਚਾ_ਦਾ_popup(vimmenu_T *menu)
{
    ਜਦੋਂ (menu->ਮਾਪਾ != ਨਿਰਾ)
	menu = menu->ਮਾਪਾ;
    ਵਾਪਸ menu_is_popup(menu->name);
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    ਪੂਰਨ
menu_is_toolbar(char_u *name)
{
    ਵਾਪਸ (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    ਪੂਰਨ
menu_is_separator(char_u *name)
{
    ਵਾਪਸ (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ਸਥਿਰ ਪੂਰਨ
menu_is_hidden(char_u *name)
{
    ਵਾਪਸ (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ਸਥਿਰ ਪੂਰਨ
menu_is_tearoff(char_u *name ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਵਾਪਸ (STRCMP(name, TEAR_STRING) == 0);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_ਪਦ_POPUP_MENU)

    ਸਥਿਰ ਪੂਰਨ
get_menu_ਬਹੁਲਕ(ਖਾਲੀ)
{
# ifdef FEAT_TERMINAL
    ਜੇ (ਪਦ_use_loop())
	ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_TERMINAL;
# endif
    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (VIsual_select)
	    ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_SELECT;
	ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_VISUAL;
    }
    ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_ਪਾਓ;
    ਜੇ ((State & ਬਹੁਲਕ_CMDLINE) || State == ਬਹੁਲਕ_ASKMORE
						    || State == ਬਹੁਲਕ_HITRETURN)
	ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_CMDLINE;
    ਜੇ (ਅੰਤ_op)
	ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_OP_PENDING;
    ਜੇ (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
	ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_ਸਾਧਾਰਨ;
    ਜੇ (State & ਬਹੁਲਕ_LANGMAP)	// must be a "r" command, like Insert mode
	ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_ਪਾਓ;
    ਵਾਪਸ MENU_ਸੂਚਕਾਂਕ_INVALID;
}

    ਪੂਰਨ
get_menu_ਬਹੁਲਕ_flag(ਖਾਲੀ)
{
    ਪੂਰਨ ਬਹੁਲਕ = get_menu_ਬਹੁਲਕ();

    ਜੇ (ਬਹੁਲਕ == MENU_ਸੂਚਕਾਂਕ_INVALID)
	ਵਾਪਸ 0;
    ਵਾਪਸ 1 << ਬਹੁਲਕ;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    ਖਾਲੀ
show_popupmenu(ਖਾਲੀ)
{
    vimmenu_T	*menu;
    ਪੂਰਨ		menu_ਬਹੁਲਕ;
    ਅੱਖਰ*	ਬਹੁਲਕ;
    ਪੂਰਨ		ਬਹੁਲਕ_len;

    menu_ਬਹੁਲਕ = get_menu_ਬਹੁਲਕ();
    ਜੇ (menu_ਬਹੁਲਕ == MENU_ਸੂਚਕਾਂਕ_INVALID)
	ਵਾਪਸ;
    ਬਹੁਲਕ = menu_ਬਹੁਲਕ_chars[menu_ਬਹੁਲਕ];
    ਬਹੁਲਕ_len = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(ਬਹੁਲਕ);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)ਬਹੁਲਕ, ਨਿਰਾ, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	ਜੇ (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, ਬਹੁਲਕ, ਬਹੁਲਕ_len) == 0)
	    ਰੋਕੋ;

    // Only show a popup when it is defined and has entries
    ਜੇ (menu == ਨਿਰਾ || menu->children == ਨਿਰਾ)
	ਵਾਪਸ;

# ਜੇ defined(FEAT_GUI)
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# ਜੇ defined(FEAT_GUI) && defined(FEAT_ਪਦ_POPUP_MENU)
    ਨਹੀਂਤਾਂ
# endif
# ਜੇ defined(FEAT_ਪਦ_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    ਪੂਰਨ
check_menu_ਸੂਚਕ(vimmenu_T *ਜੜ੍ਹ, vimmenu_T *menu_ਮੰਜ਼ਿਲ_check)
{
    vimmenu_T	*p;

    ਲਈ (p = ਜੜ੍ਹ; p != ਨਿਰਾ; p = p->ਅਗਲਾ)
	ਜੇ (p == menu_ਮੰਜ਼ਿਲ_check
		|| (p->children != ਨਿਰਾ
		    && check_menu_ਸੂਚਕ(p->children, menu_ਮੰਜ਼ਿਲ_check) == OK))
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    ਖਾਲੀ
gui_create_initial_menus(vimmenu_T *menu)
{
    ਪੂਰਨ		idx = 0;

    ਜਦੋਂ (menu != ਨਿਰਾ)
    {
	// Don't add a menu when only a tip was defined.
	ਜੇ (menu->modes & MENU_ALL_MODES)
	{
	    ਜੇ (menu->children != ਨਿਰਾ)
	    {
		gui_mch_ਜੋੜੋ_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    ਨਹੀਂਤਾਂ
		gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(menu, idx);
	}
	menu = menu->ਅਗਲਾ;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    ਸਥਿਰ ਖਾਲੀ
gui_update_menus_recurse(vimmenu_T *menu, ਪੂਰਨ ਬਹੁਲਕ)
{
    ਪੂਰਨ		grey;

    ਜਦੋਂ (menu)
    {
	ਜੇ ((menu->modes & menu->enabled & ਬਹੁਲਕ)
# ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	ਨਹੀਂਤਾਂ
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	ਜੇ (vim_strchr(p_go, GO_GREY) != ਨਿਰਾ || menu->ਮਾਪਾ == ਨਿਰਾ
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->ਮਾਪਾ->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	ਨਹੀਂਤਾਂ
	    gui_mch_menu_hidden(menu, grey);
	gui_update_menus_recurse(menu->children, ਬਹੁਲਕ);
	menu = menu->ਅਗਲਾ;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    ਖਾਲੀ
gui_update_menus(ਪੂਰਨ modes)
{
    ਸਥਿਰ ਪੂਰਨ	    prev_ਬਹੁਲਕ = -1;
    ਪੂਰਨ		    ਬਹੁਲਕ = 0;

    ਜੇ (modes != 0x0)
	ਬਹੁਲਕ = modes;
    ਨਹੀਂਤਾਂ
	ਬਹੁਲਕ = get_menu_ਬਹੁਲਕ_flag();

    ਜੇ (force_menu_update || ਬਹੁਲਕ != prev_ਬਹੁਲਕ)
    {
	gui_update_menus_recurse(ਜੜ੍ਹ_menu, ਬਹੁਲਕ);
	gui_mch_draw_menubar();
	prev_ਬਹੁਲਕ = ਬਹੁਲਕ;
	force_menu_update = FALSE;
    }
}

# ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    ਪੂਰਨ
gui_is_menu_shortcut(ਪੂਰਨ ਕੁੰਜੀ)
{
    vimmenu_T	*menu;

    ਜੇ (ਕੁੰਜੀ < 256)
	ਕੁੰਜੀ = TOLOWER_LOC(ਕੁੰਜੀ);
    FOR_ALL_MENUS(menu)
	ਜੇ (menu->mnemonic == ਕੁੰਜੀ
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == ਕੁੰਜੀ))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}
# endif
#ਜੇ_ਅੰਤ // FEAT_GUI

#ਜੇ defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    ਖਾਲੀ
gui_mch_toggle_tearoffs(ਪੂਰਨ enable)
{
    ਪੂਰਨ		pri_tab[MENUDEPTH + 1];
    ਪੂਰਨ		i;

    ਜੇ (enable)
    {
	ਲਈ (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(ਜੜ੍ਹ_menu, (char_u *)"", pri_tab, 0);
    }
    ਨਹੀਂਤਾਂ
	gui_destroy_tearoffs_recurse(ਜੜ੍ਹ_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    ਸਥਿਰ ਖਾਲੀ
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ਅਚਲ char_u	*pname,
    ਪੂਰਨ			*pri_tab,
    ਪੂਰਨ			pri_idx)
{
    char_u	*newpname = ਨਿਰਾ;
    ਪੂਰਨ		len;
    char_u	*s;
    char_u	*d;

    ਜੇ (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    ਜਦੋਂ (menu != ਨਿਰਾ)
    {
	ਜੇ (menu->children != ਨਿਰਾ && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (ਪੂਰਨ)STRLEN(pname) + (ਪੂਰਨ)STRLEN(menu->name);
	    ਲਈ (s = menu->name; *s; ++s)
		ਜੇ (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    ਜੇ (newpname != ਨਿਰਾ)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		ਲਈ (s = menu->name; *s; ++s)
		{
		    ਜੇ (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		ਜੇ (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_ਜੋੜੋ_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			// remove TEAR_STRING
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->ਅਗਲਾ;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_ਜੋੜੋ_tearoff(char_u *tearpath, ਪੂਰਨ *pri_tab, ਪੂਰਨ pri_idx)
{
    char_u	*tbuf;
    ਪੂਰਨ		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)STRLEN(tearpath));
    ਜੇ (tbuf == ਨਿਰਾ)
	ਵਾਪਸ;

    tbuf[0] = K_SPECIAL;
    tbuf[1] = K_ਦੂਜਾ(K_TEAROFF);
    tbuf[2] = K_ਤੀਜਾ(K_TEAROFF);
    STRCPY(tbuf + 3, tearpath);
    STRCAT(tbuf + 3, "\r");

    STRCAT(tearpath, ".");
    STRCAT(tearpath, TEAR_STRING);

    // Priority of tear-off is always 1
    t = pri_tab[pri_idx + 1];
    pri_tab[pri_idx + 1] = 1;

# ifdef FEAT_TOOLBAR
    menuarg.iconfile = ਨਿਰਾ;
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
# endif
    menuarg.noremap[0] = REMAP_NONE;
    menuarg.silent[0] = TRUE;

    menuarg.modes = MENU_ALL_MODES;
    ਜੋੜੋ_menu_ਰਸਤਾ(tearpath, &menuarg, pri_tab, tbuf, FALSE);

    menuarg.modes = MENU_TIP_ਬਹੁਲਕ;
    ਜੋੜੋ_menu_ਰਸਤਾ(tearpath, &menuarg, pri_tab,
	    (char_u *)_("Tear off this menu"), FALSE);

    pri_tab[pri_idx + 1] = t;
    vim_free(tbuf);
}

/*
 * Recursively destroy tearoff items
 */
    ਸਥਿਰ ਖਾਲੀ
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    ਜਦੋਂ (menu)
    {
	ਜੇ (menu->children)
	{
	    // check if tearoff exists
	    ਜੇ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    ਜੇ (menu->children != ਨਿਰਾ) // if not the last one
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->ਅਗਲਾ;
    }
}

#ਜੇ_ਅੰਤ // FEAT_GUI_MSWIN && FEAT_TEAROFF

/*
 * Execute "menu".  Use by ":emenu" and the window toolbar.
 * "eap" is NULL for the window toolbar.
 * "mode_idx" specifies a MENU_INDEX_ value, use MENU_INDEX_INVALID to depend
 * on the current state.
 */
    ਖਾਲੀ
execute_menu(exarg_T *eap, vimmenu_T *menu, ਪੂਰਨ ਬਹੁਲਕ_idx)
{
    ਪੂਰਨ		idx = ਬਹੁਲਕ_idx;

    ਜੇ (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	ਜੇ (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_ਸੂਚਕਾਂਕ_ਪਾਓ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਨਹੀਂਤਾਂ ਜੇ (ਪਦ_use_loop())
	{
	    idx = MENU_ਸੂਚਕਾਂਕ_TERMINAL;
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ)
	{
	    idx = MENU_ਸੂਚਕਾਂਕ_VISUAL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (eap != ਨਿਰਾ && eap->addr_ਗਿਣਤੀ)
	{
	    pos_T	tpos;

	    idx = MENU_ਸੂਚਕਾਂਕ_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    ਜੇ ((curbuf->b_visual.vi_ਸ਼ੁਰੂ.lnum == eap->line1)
		    && (curbuf->b_visual.vi_ਸਿਰਾ.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_ਬਹੁਲਕ = curbuf->b_visual.vi_ਬਹੁਲਕ;
		tpos = curbuf->b_visual.vi_ਸਿਰਾ;
		curwin->w_cursor = curbuf->b_visual.vi_ਸ਼ੁਰੂ;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Set it up for line-wise visual mode
		VIsual_ਬਹੁਲਕ = 'V';
		curwin->w_cursor.lnum = eap->line1;
		curwin->w_cursor.ਖਾਨਾ = 1;
		tpos.lnum = eap->line2;
		tpos.ਖਾਨਾ = MAXCOL;
		tpos.coladd = 0;
	    }

	    // Activate visual mode
	    VIsual_ਸਰਗਰਮ = TRUE;
	    VIsual_reselect = TRUE;
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;

	    check_cursor();

	    // Adjust the cursor to make sure it is in the correct pos
	    // for exclusive mode
	    ਜੇ (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.ਖਾਨਾ;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    ਜੇ (idx == MENU_ਸੂਚਕਾਂਕ_INVALID || eap == ਨਿਰਾ)
	idx = MENU_ਸੂਚਕਾਂਕ_ਸਾਧਾਰਨ;

    ਜੇ (menu->ਲੜੀਆਂ[idx] != ਨਿਰਾ && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	ਜੇ (eap == ਨਿਰਾ || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_ਸਾਧਾਰਨ_busy;
	    ਜੇ (save_current_state(&save_state))
		exec_ਸਾਧਾਰਨ_cmd(menu->ਲੜੀਆਂ[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_ਸਾਧਾਰਨ_busy;
	}
	ਨਹੀਂਤਾਂ
	    ins_typebuf(menu->ਲੜੀਆਂ[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap != ਨਿਰਾ)
    {
	char_u	*ਬਹੁਲਕ;

	ਚੋਣ (idx)
	{
	    ਹਾਲਤ MENU_ਸੂਚਕਾਂਕ_VISUAL:
		ਬਹੁਲਕ = (char_u *)"Visual";
		ਰੋਕੋ;
	    ਹਾਲਤ MENU_ਸੂਚਕਾਂਕ_SELECT:
		ਬਹੁਲਕ = (char_u *)"Select";
		ਰੋਕੋ;
	    ਹਾਲਤ MENU_ਸੂਚਕਾਂਕ_OP_PENDING:
		ਬਹੁਲਕ = (char_u *)"Op-pending";
		ਰੋਕੋ;
	    ਹਾਲਤ MENU_ਸੂਚਕਾਂਕ_TERMINAL:
		ਬਹੁਲਕ = (char_u *)"Terminal";
		ਰੋਕੋ;
	    ਹਾਲਤ MENU_ਸੂਚਕਾਂਕ_ਪਾਓ:
		ਬਹੁਲਕ = (char_u *)"Insert";
		ਰੋਕੋ;
	    ਹਾਲਤ MENU_ਸੂਚਕਾਂਕ_CMDLINE:
		ਬਹੁਲਕ = (char_u *)"Cmdline";
		ਰੋਕੋ;
	    // case MENU_INDEX_TIP: cannot happen
	    ਮੂਲ:
		ਬਹੁਲਕ = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_ਬਹੁਲਕ), ਬਹੁਲਕ);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    ਸਥਿਰ vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    ਪੂਰਨ		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    ਜੇ (saved_name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    menu = *get_ਜੜ੍ਹ_menu(saved_name);
    name = saved_name;
    ਜਦੋਂ (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	ਜਦੋਂ (menu != ਨਿਰਾ)
	{
	    ਜੇ (menu_name_equal(name, menu))
	    {
		ਜੇ (*p == NUL && menu->children != ਨਿਰਾ)
		{
		    emsg(_(e_menu_ਰਸਤਾ_must_lead_ਮੰਜ਼ਿਲ_menu_ਵਸਤੂ));
		    gave_emsg = TRUE;
		    menu = ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (*p != NUL && menu->children == ਨਿਰਾ)
		{
		    emsg(_(e_part_ਦਾ_menu_ਵਸਤੂ_ਰਸਤਾ_is_not_sub_menu));
		    menu = ਨਿਰਾ;
		}
		ਰੋਕੋ;
	    }
	    menu = menu->ਅਗਲਾ;
	}
	ਜੇ (menu == ਨਿਰਾ || *p == NUL)
	    ਰੋਕੋ;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    ਜੇ (menu == ਨਿਰਾ)
    {
	ਜੇ (!gave_emsg)
	    semsg(_(e_menu_not_ਮਿਲਿਆ_str), name_arg);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    ਖਾਲੀ
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    ਪੂਰਨ		ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_INVALID;

    ਜੇ (arg[0] && VIM_ISWHITE(arg[1]))
    {
	ਚੋਣ (arg[0])
	{
	    ਹਾਲਤ 'n': ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_ਸਾਧਾਰਨ; ਰੋਕੋ;
	    ਹਾਲਤ 'v': ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_VISUAL; ਰੋਕੋ;
	    ਹਾਲਤ 's': ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_SELECT; ਰੋਕੋ;
	    ਹਾਲਤ 'o': ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_OP_PENDING; ਰੋਕੋ;
	    ਹਾਲਤ 't': ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_TERMINAL; ਰੋਕੋ;
	    ਹਾਲਤ 'i': ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_ਪਾਓ; ਰੋਕੋ;
	    ਹਾਲਤ 'c': ਬਹੁਲਕ_idx = MENU_ਸੂਚਕਾਂਕ_CMDLINE; ਰੋਕੋ;
	    ਮੂਲ: semsg(_(e_invalid_ਦਲੀਲ_str), arg);
		     ਵਾਪਸ;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    ਜੇ (menu == ਨਿਰਾ)
	ਵਾਪਸ;

    // Found the menu, so execute.
    execute_menu(eap, menu, ਬਹੁਲਕ_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    ਖਾਲੀ
winbar_click(win_T *wp, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ		idx;

    ਜੇ (wp->w_winbar_ਚੀਜ਼ਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;
    ਲਈ (idx = 0; wp->w_winbar_ਚੀਜ਼ਾਂ[idx].wb_menu != ਨਿਰਾ; ++idx)
    {
	winbar_ਵਸਤੂ_T *ਵਸਤੂ = &wp->w_winbar_ਚੀਜ਼ਾਂ[idx];

	ਜੇ (ਖਾਨਾ >= ਵਸਤੂ->wb_startcol && ਖਾਨਾ <= ਵਸਤੂ->wb_endcol)
	{
	    win_T   *save_curwin = ਨਿਰਾ;
	    pos_T   save_visual = VIsual;
	    ਪੂਰਨ	    save_visual_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;
	    ਪੂਰਨ	    save_visual_select = VIsual_select;
	    ਪੂਰਨ	    save_visual_reselect = VIsual_reselect;
	    ਪੂਰਨ	    save_visual_ਬਹੁਲਕ = VIsual_ਬਹੁਲਕ;

	    ਜੇ (wp != curwin)
	    {
		// Clicking in the window toolbar of a not-current window.
		// Make that window the current one and save Visual mode.
		save_curwin = curwin;
		VIsual_ਸਰਗਰਮ = FALSE;
		curwin = wp;
		curbuf = curwin->w_ਬਫਰ;
		check_cursor();
	    }

	    // Note: the command might close the current window.
	    execute_menu(ਨਿਰਾ, ਵਸਤੂ->wb_menu, -1);

	    ਜੇ (save_curwin != ਨਿਰਾ && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_ਬਫਰ;
		VIsual = save_visual;
		VIsual_ਸਰਗਰਮ = save_visual_ਸਰਗਰਮ;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_ਬਹੁਲਕ = save_visual_ਬਹੁਲਕ;
	    }
	    ਜੇ (!win_valid(wp))
		ਰੋਕੋ;
	}
    }
}

#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_ਪਦ_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_ਲੱਭੋ_menu(char_u *ਰਸਤਾ_name)
{
    vimmenu_T	*menu = ਨਿਰਾ;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_ਜੜ੍ਹ_menu(ਰਸਤਾ_name);

    saved_name = vim_strsave(ਰਸਤਾ_name);
    ਜੇ (saved_name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    name = saved_name;
    ਜਦੋਂ (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	ਜਦੋਂ (menu != ਨਿਰਾ)
	{
	    ਜੇ (menu_name_equal(name, menu))
	    {
		ਜੇ (menu->children == ਨਿਰਾ)
		{
		    // found a menu item instead of a sub-menu
		    ਜੇ (*p == NUL)
			emsg(_(e_menu_ਰਸਤਾ_must_lead_ਮੰਜ਼ਿਲ_sub_menu));
		    ਨਹੀਂਤਾਂ
			emsg(_(e_part_ਦਾ_menu_ਵਸਤੂ_ਰਸਤਾ_is_not_sub_menu));
		    menu = ਨਿਰਾ;
		    ਜਾਓ theend;
		}
		ਜੇ (*p == NUL)	    // found a full match
		    ਜਾਓ theend;
		ਰੋਕੋ;
	    }
	    menu = menu->ਅਗਲਾ;
	}
	ਜੇ (menu == ਨਿਰਾ)	// didn't find it
	    ਰੋਕੋ;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    ਜੇ (menu == ਨਿਰਾ)
	emsg(_(e_menu_not_ਮਿਲਿਆ_check_menu_ਨਾਂ));
theend:
    vim_free(saved_name);
    ਵਾਪਸ menu;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*ਮੰਜ਼ਿਲ;		// translated name
} menutrans_T;

ਸਥਿਰ garray_T menutrans_ga = {0, 0, 0, 0, ਨਿਰਾ};
#ਜੇ_ਅੰਤ

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    ਖਾਲੀ
ex_menutranslate(exarg_T *eap ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    ਪੂਰਨ			i;
    char_u		*from, *from_noamp, *ਮੰਜ਼ਿਲ;

    ਜੇ (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, ਆਕਾਰ(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    ਜੇ (STRNCMP(arg, "clear", 5) == 0 && ਸਿਰੇ_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_ਅੰਕੜੇ;
	ਲਈ (i = 0; i < menutrans_ga.ga_len; ++i)
	{
	    vim_free(tp[i].from);
	    vim_free(tp[i].from_noamp);
	    vim_free(tp[i].ਮੰਜ਼ਿਲ);
	}
	ga_clear(&menutrans_ga);
# ifdef FEAT_EVAL
	// Delete all "menutrans_" global variables.
	del_menutrans_vars();
# endif
    }
    ਨਹੀਂਤਾਂ
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	ਮੰਜ਼ਿਲ = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(ਮੰਜ਼ਿਲ);
	ਜੇ (arg == ਮੰਜ਼ਿਲ || ਸਿਰੇ_excmd2(eap->arg, from)
		      || ਸਿਰੇ_excmd2(eap->arg, ਮੰਜ਼ਿਲ)
		      || !ਸਿਰੇ_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_ਦਲੀਲ));
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_ਅੰਕੜੇ;
		from = vim_strsave(from);
		ਜੇ (from != ਨਿਰਾ)
		{
		    from_noamp = menu_ਪਾਠ(from, ਨਿਰਾ, ਨਿਰਾ);
		    ਮੰਜ਼ਿਲ = vim_strnsave(ਮੰਜ਼ਿਲ, arg - ਮੰਜ਼ਿਲ);
		    ਜੇ (from_noamp != ਨਿਰਾ && ਮੰਜ਼ਿਲ != ਨਿਰਾ)
		    {
			menu_translate_tab_and_ਖਿਸਕਾਓ(from);
			menu_translate_tab_and_ਖਿਸਕਾਓ(ਮੰਜ਼ਿਲ);
			menu_unescape_name(from);
			menu_unescape_name(ਮੰਜ਼ਿਲ);
			tp[menutrans_ga.ga_len].from = from;
			tp[menutrans_ga.ga_len].from_noamp = from_noamp;
			tp[menutrans_ga.ga_len].ਮੰਜ਼ਿਲ = ਮੰਜ਼ਿਲ;
			++menutrans_ga.ga_len;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			vim_free(from);
			vim_free(from_noamp);
			vim_free(ਮੰਜ਼ਿਲ);
		    }
		}
	    }
	}
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ਸਥਿਰ char_u *
menu_skip_part(char_u *p)
{
    ਜਦੋਂ (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	ਜੇ ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    ਸਥਿਰ char_u *
menutrans_lookup(char_u *name, ਪੂਰਨ len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_ਅੰਕੜੇ;
    ਪੂਰਨ			i;
    char_u		*dname;

    ਲਈ (i = 0; i < menutrans_ga.ga_len; ++i)
	ਜੇ (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    ਵਾਪਸ tp[i].ਮੰਜ਼ਿਲ;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_ਪਾਠ(name, ਨਿਰਾ, ਨਿਰਾ);
    name[len] = i;
    ਜੇ (dname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (i = 0; i < menutrans_ga.ga_len; ++i)
	ਜੇ (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    ਵਾਪਸ tp[i].ਮੰਜ਼ਿਲ;
	}
    vim_free(dname);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ਸਥਿਰ ਖਾਲੀ
menu_unescape_name(char_u *name)
{
    char_u  *p;

    ਲਈ (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	ਜੇ (*p == '\\')
	    STRMOVE(p, p + 1);
}
#ਜੇ_ਅੰਤ // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    ਸਥਿਰ char_u *
menu_translate_tab_and_ਖਿਸਕਾਓ(char_u *arg_ਸ਼ੁਰੂ)
{
    char_u	*arg = arg_ਸ਼ੁਰੂ;

    ਜਦੋਂ (*arg && !VIM_ISWHITE(*arg))
    {
	ਜੇ ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    ਜੇ (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    ਵਾਪਸ arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ਸਥਿਰ ਪੂਰਨ
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, ਪੂਰਨ modes, dict_T *dict)
{
    ਪੂਰਨ		ਸਥਿਤੀ;
    list_T	*l;

    ਜੇ (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	ਜੇ (l == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਜੇ (dict_ਜੋੜੋ_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ਵਾਪਸ FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	ਲਈ (topmenu = menu; topmenu != ਨਿਰਾ; topmenu = topmenu->ਅਗਲਾ)
	    ਜੇ (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	ਵਾਪਸ OK;
    }

    ਜੇ (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	ਵਾਪਸ OK;

    ਸਥਿਤੀ = dict_ਜੋੜੋ_string(dict, "name", menu->name);
    ਜੇ (ਸਥਿਤੀ == OK)
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string(dict, "display", menu->dname);
    ਜੇ (ਸਥਿਤੀ == OK && menu->actext != ਨਿਰਾ)
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string(dict, "accel", menu->actext);
    ਜੇ (ਸਥਿਤੀ == OK)
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "priority", menu->ਤਰਜੀਹ);
    ਜੇ (ਸਥਿਤੀ == OK)
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string(dict, "modes",
					get_menu_ਬਹੁਲਕ_str(menu->modes));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਜੇ (ਸਥਿਤੀ == OK && menu->iconfile != ਨਿਰਾ)
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string(dict, "icon", menu->iconfile);
    ਜੇ (ਸਥਿਤੀ == OK && menu->iconidx >= 0)
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "iconidx", menu->iconidx);
#ਜੇ_ਅੰਤ
    ਜੇ (ਸਥਿਤੀ == OK)
    {
	char_u	buf[NUMBUFLEN];
	ਪੂਰਨ	buflen;

	ਜੇ (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	ਨਹੀਂਤਾਂ
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string_len(dict, "shortcut", buf, buflen);
    }
    ਜੇ (ਸਥਿਤੀ == OK && menu->children == ਨਿਰਾ)
    {
	ਪੂਰਨ		ਦੋਆਧਾਰੀ_ਅੰਕ;

	// Get the first mode in which the menu is available
	ਲਈ (ਦੋਆਧਾਰੀ_ਅੰਕ = 0; ਦੋਆਧਾਰੀ_ਅੰਕ < MENU_MODES && !((1 << ਦੋਆਧਾਰੀ_ਅੰਕ) & modes); ਦੋਆਧਾਰੀ_ਅੰਕ++)
	    ;
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ < MENU_MODES) // just in case, avoid Coverity warning
	{
	    ਜੇ (menu->ਲੜੀਆਂ[ਦੋਆਧਾਰੀ_ਅੰਕ] != ਨਿਰਾ)
	    {
		ਜੇ (*menu->ਲੜੀਆਂ[ਦੋਆਧਾਰੀ_ਅੰਕ] == NUL)
		    ਸਥਿਤੀ = dict_ਜੋੜੋ_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		ਨਹੀਂਤਾਂ
		{
		    char_u *tofree = ਨਿਰਾ;

		    ਸਥਿਤੀ = dict_ਜੋੜੋ_string(dict, "rhs",
			tofree = str2special_save(menu->ਲੜੀਆਂ[ਦੋਆਧਾਰੀ_ਅੰਕ], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    ਜੇ (ਸਥਿਤੀ == OK)
		ਸਥਿਤੀ = dict_ਜੋੜੋ_bool(dict, "noremenu",
					     menu->noremap[ਦੋਆਧਾਰੀ_ਅੰਕ] == REMAP_NONE);
	    ਜੇ (ਸਥਿਤੀ == OK)
		ਸਥਿਤੀ = dict_ਜੋੜੋ_bool(dict, "script",
					   menu->noremap[ਦੋਆਧਾਰੀ_ਅੰਕ] == REMAP_SCRIPT);
	    ਜੇ (ਸਥਿਤੀ == OK)
		ਸਥਿਤੀ = dict_ਜੋੜੋ_bool(dict, "silent", menu->silent[ਦੋਆਧਾਰੀ_ਅੰਕ]);
	    ਜੇ (ਸਥਿਤੀ == OK)
		ਸਥਿਤੀ = dict_ਜੋੜੋ_bool(dict, "enabled",
					  ((menu->enabled & (1 << ਦੋਆਧਾਰੀ_ਅੰਕ)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    ਜੇ (ਸਥਿਤੀ == OK && menu->children != ਨਿਰਾ)
    {
	vimmenu_T	*ਬੱਚਾ;

	l = list_alloc();
	ਜੇ (l == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਜੇ (dict_ਜੋੜੋ_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    ਵਾਪਸ FAIL;
	}
	ਬੱਚਾ = menu->children;
	ਜਦੋਂ (ਬੱਚਾ)
	{
	    ਜੇ (!menu_is_tearoff(ਬੱਚਾ->dname))		// skip tearoff menu
		list_append_string(l, ਬੱਚਾ->dname, -1);
	    ਬੱਚਾ = ਬੱਚਾ->ਅਗਲਾ;
	}
    }

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    ਖਾਲੀ
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    ਪੂਰਨ		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    retdict = rettv->vval.v_dict;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    menu_name = tv_get_string_chk(&argvars[0]);
    ਜੇ (menu_name == ਨਿਰਾ)
	ਵਾਪਸ;

    // menu mode
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    ਨਹੀਂਤਾਂ
	which = (char_u *)"";	    // Default is modes for "menu"
    ਜੇ (which == ਨਿਰਾ)
	ਵਾਪਸ;

    modes = get_menu_cmd_modes(which, *which == '!', ਨਿਰਾ, ਨਿਰਾ);

    // Locate the specified menu or menu item
    menu = *get_ਜੜ੍ਹ_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    ਜੇ (saved_name == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	ਜਦੋਂ (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    ਜਦੋਂ (menu != ਨਿਰਾ)
	    {
		ਜੇ (menu_name_equal(name, menu))
		    ਰੋਕੋ;
		menu = menu->ਅਗਲਾ;
	    }
	    ਜੇ (menu == ਨਿਰਾ || *p == NUL)
		ਰੋਕੋ;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    ਜੇ (menu == ਨਿਰਾ)		// specified menu not found
	ਵਾਪਸ;

    ਜੇ (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#ਜੇ_ਅੰਤ // FEAT_MENU
