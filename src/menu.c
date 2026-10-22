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

#ضمن "vim.h"

#إذا defined(FEAT_MENU)

#وصف_ڪريو MENUDEPTH   10		// maximum depth of menus

#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
ساكن صحيح جوڙيو_menu_رستو(char_u *, vimmenu_T *, صحيح *, char_u *, صحيح);
#وإلا
ساكن صحيح جوڙيو_menu_رستو(char_u *, vimmenu_T *, صحيح *, char_u *);
#جيڪڏهن_پورو
ساكن صحيح menu_nable_recurse(vimmenu_T *menu, char_u *name, صحيح modes, صحيح enable);
ساكن صحيح remove_menu(vimmenu_T **, char_u *, صحيح, صحيح silent);
ساكن فراغ free_menu(vimmenu_T **menup);
ساكن فراغ free_menu_string(vimmenu_T *, صحيح);
ساكن صحيح show_menus(char_u *, صحيح);
ساكن فراغ show_menus_recursive(vimmenu_T *, صحيح, صحيح);
ساكن char_u *menu_name_skip(char_u *name);
ساكن صحيح menu_name_equal(char_u *name, vimmenu_T *menu);
ساكن صحيح menu_namecmp(char_u *name, char_u *mname);
ساكن صحيح get_menu_cmd_modes(char_u *, صحيح, صحيح *, صحيح *);
ساكن char_u *popup_گهڻائي_name(char_u *name, صحيح idx);
ساكن char_u *menu_متن(char_u *متن, صحيح *mnemonic, char_u **actext);

#إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
ساكن فراغ gui_create_tearoffs_recurse(vimmenu_T *menu, ثابت char_u *pname, صحيح *pri_tab, صحيح pri_idx);
ساكن فراغ gui_جوڙيو_tearoff(char_u *tearpath, صحيح *pri_tab, صحيح pri_idx);
ساكن فراغ gui_destroy_tearoffs_recurse(vimmenu_T *menu);
ساكن صحيح s_tearoffs = FALSE;
#جيڪڏهن_پورو

ساكن صحيح menu_is_hidden(char_u *name);
ساكن صحيح menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
ساكن صحيح menus_locked = 0;

#إذا defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
ساكن char_u *menu_skip_part(char_u *p);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
ساكن char_u *menutrans_lookup(char_u *name, صحيح len);
ساكن فراغ menu_unescape_name(char_u	*p);
#جيڪڏهن_پورو

ساكن char_u *menu_translate_tab_and_سرڪايو(char_u *arg_شروعات);

// The character for each menu mode
ساكن حرف *menu_گهڻائي_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
ساكن ثابت حرف *toolbar_نالا[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_ڳڻپ ARRAY_ڊيگهائي(toolbar_نالا)
#جيڪڏهن_پورو

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    ساكن صحيح
menu_is_winbar(char_u *name)
{
    أرجع (STRNCMP(name, "WinBar", 6) == 0);
}

    صحيح
winbar_اوچائي(win_T *wp)
{
    إذا (wp->w_winbar != عدم && wp->w_winbar->children != عدم)
	أرجع 1;
    أرجع 0;
}

    ساكن vimmenu_T **
get_پاڙ_menu(char_u *name)
{
    إذا (menu_is_winbar(name))
	أرجع &curwin->w_winbar;
    أرجع &پاڙ_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساكن صحيح
is_menus_locked(فراغ)
{
    إذا (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    فراغ
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_رستو;
    صحيح		modes;
    char_u	*map_منزل;
    صحيح		noremap;
    صحيح		silent = FALSE;
    صحيح		special = FALSE;
    صحيح		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    صحيح		i;
#إذا defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    صحيح		old_menu_اوچائي;
# إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    صحيح		old_toolbar_اوچائي;
# endif
#جيڪڏهن_پورو
    صحيح		pri_tab[MENUDEPTH + 1];
    صحيح		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    char_u	*icon = عدم;
#جيڪڏهن_پورو
    vimmenu_T	menuarg;
    vimmenu_T	**پاڙ_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    لكل (;;)
    {
	إذا (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    استمر;
	}
	إذا (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    استمر;
	}
	إذا (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    استمر;
	}
	اكسر;
    }


    // Locate an optional "icon=filename" argument.
    إذا (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
	icon = arg;
#جيڪڏهن_پورو
	طالما (*arg != NUL && *arg != ' ')
	{
	    إذا (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	إذا (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    لكل (p = arg; *p; ++p)
	إذا (!VIM_ISDIGIT(*p) && *p != '.')
	    اكسر;
    إذا (VIM_ISWHITE(*p))
    {
	لكل (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    إذا (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    إذا (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    وإلا إذا (eap->addr_ڳڻپ && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    وإلا
	i = 0;
    طالما (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    إذا (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    وإلا إذا (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    إذا (*arg == NUL)
    {
	show_menus(arg, modes);
	أرجع;
    }

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    إذا (menu_is_toolbar(arg))
    {
	menu_رستو = menu_skip_part(arg);
	إذا (*menu_رستو == '.')
	{
	    p = menu_skip_part(++menu_رستو);
	    إذا (STRNCMP(menu_رستو, "BuiltIn", 7) == 0)
	    {
		إذا (skipdigits(menu_رستو + 7) == p)
		{
		    menuarg.iconidx = atoi((حرف *)menu_رستو + 7);
		    إذا (menuarg.iconidx >= (صحيح)TOOLBAR_NAME_ڳڻپ)
			menuarg.iconidx = -1;
		    وإلا
			menuarg.icon_builtin = TRUE;
		}
	    }
	    وإلا
	    {
		لكل (i = 0; i < (صحيح)TOOLBAR_NAME_ڳڻپ; ++i)
		    إذا (STRNCMP(toolbar_نالا[i], menu_رستو, p - menu_رستو)
									 == 0)
		    {
			menuarg.iconidx = i;
			اكسر;
		    }
	    }
	}
    }
#جيڪڏهن_پورو

    menu_رستو = arg;
    إذا (*menu_رستو == '.')
    {
	semsg(_(e_invalid_دليل_str), menu_رستو);
	اذهب theend;
    }

    map_منزل = menu_translate_tab_and_سرڪايو(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    إذا (*map_منزل == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_رستو, modes);
	اذهب theend;
    }
    وإلا إذا (*map_منزل != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_منزل);
	اذهب theend;
    }
#إذا defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_اوچائي = gui.menu_اوچائي;
# إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_اوچائي = gui.toolbar_اوچائي;
# endif
#جيڪڏهن_پورو

    پاڙ_menu_ptr = get_پاڙ_menu(menu_رستو);
    إذا (پاڙ_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    إذا (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	إذا (STRCMP(menu_رستو, "*") == 0)	// meaning: do all menus
	    menu_رستو = (char_u *)"";

	إذا (menu_is_popup(menu_رستو))
	{
	    لكل (i = 0; i < MENU_اشاريو_TIP; ++i)
		إذا (modes & (1 << i))
		{
		    p = popup_گهڻائي_name(menu_رستو, i);
		    إذا (p != عدم)
		    {
			menu_nable_recurse(*پاڙ_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*پاڙ_menu_ptr, menu_رستو, modes, enable);
    }
    وإلا إذا (unmenu)
    {
	إذا (is_menus_locked())
	    اذهب theend;

	/*
	 * Delete menu(s).
	 */
	إذا (STRCMP(menu_رستو, "*") == 0)	// meaning: remove all menus
	    menu_رستو = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	إذا (menu_is_popup(menu_رستو))
	{
	    لكل (i = 0; i < MENU_اشاريو_TIP; ++i)
		إذا (modes & (1 << i))
		{
		    p = popup_گهڻائي_name(menu_رستو, i);
		    إذا (p != عدم)
		    {
			remove_menu(پاڙ_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(پاڙ_menu_ptr, menu_رستو, modes, FALSE);
    }
    وإلا
    {
	إذا (is_menus_locked())
	    اذهب theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	إذا (STRICMP(map_منزل, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_منزل = (char_u *)"";
	    map_buf = عدم;
	}
	وإلا إذا (modes & MENU_TIP_گهڻائي)
	    map_buf = عدم;	// Menu tips are plain text.
	وإلا
	    map_منزل = replace_termcodes(map_منزل, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), عدم);
	menuarg.modes = modes;
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
	menuarg.iconfile = icon;
#جيڪڏهن_پورو
	menuarg.noremap[0] = noremap;
	menuarg.silent[0] = silent;
	جوڙيو_menu_رستو(menu_رستو, &menuarg, pri_tab, map_منزل
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
		, TRUE
#جيڪڏهن_پورو
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	إذا (menu_is_popup(menu_رستو))
	{
	    لكل (i = 0; i < MENU_اشاريو_TIP; ++i)
		إذا (modes & (1 << i))
		{
		    p = popup_گهڻائي_name(menu_رستو, i);
		    إذا (p != عدم)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
			menuarg.iconfile = عدم;
			menuarg.iconidx = -1;
			menuarg.icon_builtin = FALSE;
#جيڪڏهن_پورو
			جوڙيو_menu_رستو(p, &menuarg, pri_tab, map_منزل
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
				, TRUE
#جيڪڏهن_پورو
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

#إذا defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    إذا (gui.اندريون_use
	    && (gui.menu_اوچائي != old_menu_اوچائي
# إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_اوچائي != old_toolbar_اوچائي
# endif
	    ))
	gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_VERT);
#جيڪڏهن_پورو
    إذا (پاڙ_menu_ptr == &curwin->w_winbar)
    {
	صحيح h = winbar_اوچائي(curwin);

	إذا (h != curwin->w_winbar_اوچائي)
	{
	    إذا (h == 0)
		++curwin->w_اوچائي;
	    وإلا إذا (curwin->w_اوچائي > 0)
		--curwin->w_اوچائي;
	    curwin->w_winbar_اوچائي = h;
	}
	curwin->w_prev_اوچائي = curwin->w_اوچائي;
    }

theend:
    ;
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    ساكن صحيح
جوڙيو_menu_رستو(
    char_u	*menu_رستو,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    صحيح		*pri_tab,
    char_u	*call_معلومات
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
    , صحيح	addtearoff	// may add tearoff item
#جيڪڏهن_پورو
    )
{
    char_u	*رستو_name;
    صحيح		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = عدم;
    vimmenu_T	*والد;
    vimmenu_T	**هيٺاهون_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*ايندڙ_name;
    صحيح		i;
    صحيح		c;
    صحيح		d;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    صحيح		idx;
    صحيح		new_idx;
#جيڪڏهن_پورو
    صحيح		pri_idx = 0;
    صحيح		old_modes = 0;
    صحيح		amenu;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_منزل = عدم;
#جيڪڏهن_پورو
    vimmenu_T	**پاڙ_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    رستو_name = vim_strsave(menu_رستو);
    إذا (رستو_name == عدم)
	أرجع FAIL;
    پاڙ_menu_ptr = get_پاڙ_menu(menu_رستو);
    menup = پاڙ_menu_ptr;
    والد = عدم;
    name = رستو_name;
    طالما (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	ايندڙ_name = menu_name_skip(name);
#جيڪڏهن_وصف_ٿيل	FEAT_MULTI_LANG
	map_منزل = menutrans_lookup(name, (صحيح)STRLEN(name));
	إذا (map_منزل != عدم)
	{
	    en_name = name;
	    name = map_منزل;
	}
	وإلا
	    en_name = عدم;
#جيڪڏهن_پورو
	dname = menu_متن(name, عدم, عدم);
	إذا (dname == عدم)
	    اذهب erret;
	إذا (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_خالي_جايون_menu_name));
	    اذهب erret;
	}

	// See if it's already there
	هيٺاهون_pri = menup;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	idx = 0;
	new_idx = 0;
#جيڪڏهن_پورو
	menu = *menup;
	طالما (menu != عدم)
	{
	    إذا (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		إذا (*ايندڙ_name == NUL && menu->children != عدم)
		{
		    إذا (!sys_menu)
			emsg(_(e_menu_رستو_must_not_lead_منزل_sub_menu));
		    اذهب erret;
		}
		إذا (*ايندڙ_name != NUL && menu->children == عدم
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
			&& addtearoff
#جيڪڏهن_پورو
			)
		{
		    إذا (!sys_menu)
			emsg(_(e_part_جو_menu_شي_رستو_is_not_sub_menu));
		    اذهب erret;
		}
		اكسر;
	    }
	    menup = &menu->ايندڙ;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    إذا (والد != عدم || menu_is_menubar(menu->name))
	    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		++idx;
#جيڪڏهن_پورو
		إذا (menu->ترجيح <= pri_tab[pri_idx])
		{
		    هيٺاهون_pri = menup;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		    new_idx = idx;
#جيڪڏهن_پورو
		}
	    }
	    menu = menu->ايندڙ;
	}

	إذا (menu == عدم)
	{
	    إذا (*ايندڙ_name == NUL && والد == عدم)
	    {
		emsg(_(e_must_not_جوڙيو_menu_شيون_directly_منزل_menu_bar));
		اذهب erret;
	    }

	    إذا (menu_is_separator(dname) && *ايندڙ_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_جو_menu_رستو));
		اذهب erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    إذا (menu == عدم)
		اذهب erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_متن(name, &menu->mnemonic, &menu->actext);
#جيڪڏهن_وصف_ٿيل	FEAT_MULTI_LANG
	    إذا (en_name != عدم)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_متن(en_name, عدم, عدم);
	    }
	    وإلا
	    {
		menu->en_name = عدم;
		menu->en_dname = عدم;
	    }
#جيڪڏهن_پورو
	    menu->ترجيح = pri_tab[pri_idx];
	    menu->والد = والد;
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_TIP
	    menu->tip = عدم;
#جيڪڏهن_پورو
	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->ايندڙ = *هيٺاهون_pri;
	    *هيٺاهون_pri = menu;

	    old_modes = 0;

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
	    menu->iconidx = menuarg->iconidx;
	    menu->icon_builtin = menuarg->icon_builtin;
	    إذا (*ايندڙ_name == NUL && menuarg->iconfile != عدم)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    إذا (والد != عدم && menu_is_tearoff(والد->children->dname))
		والد->children->modes |= modes;
#جيڪڏهن_پورو
	}
	وإلا
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    إذا (addtearoff)
#جيڪڏهن_پورو
	    {
		menu->modes |= modes;
		menu->enabled |= modes;
	    }
	}

#جيڪڏهن_وصف_ٿيل FEAT_GUI
	/*
	 * Add the menu item when it's used in one of the modes, but not when
	 * only a tooltip is defined.
	 */
	إذا ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    إذا (gui.اندريون_use)  // Otherwise it will be added when GUI starts
	    {
		إذا (*ايندڙ_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_جوڙيو_menu_شي(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		وإلا
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_جوڙيو_menu(menu, new_idx);
		}
	    }

# إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    إذا (	addtearoff
		    && *ايندڙ_name
		    && vim_strchr(p_go, GO_TEAROFF) != عدم
		    && menu_is_menubar(name)
#  ifdef VIMDLL
		    && (gui.اندريون_use || gui.starting)
#  endif
		    )
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_رستو) + TEAR_LEN + 2);
		إذا (tearpath != عدم)
		{
		    char_u  *s;
		    صحيح	    len;

		    STRCPY(tearpath, menu_رستو);
		    len = (صحيح)(ايندڙ_name - رستو_name - 1);
		    لكل (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			إذا ((*s == '\\' || *s == Ctrl_V) && s[1])
			{
			    ++len;
			    ++s;
			}
		    }
		    tearpath[len] = NUL;
		    gui_جوڙيو_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
# endif
	}
#جيڪڏهن_پورو // FEAT_GUI

	menup = &menu->children;
	والد = menu;
	name = ايندڙ_name;
	VIM_CLEAR(dname);
	إذا (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(رستو_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_عام_گهڻائي | MENU_وجهو_گهڻائي)) ==
				       (MENU_عام_گهڻائي | MENU_وجهو_گهڻائي));
    إذا (sys_menu)
	modes &= ~old_modes;

    إذا (menu != عدم && modes)
    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	menu->cb = gui_menu_cb;
#جيڪڏهن_پورو
	p = (call_معلومات == عدم) ? عدم : vim_strsave(call_معلومات);

	// loop over all modes, may add more than one
	لكل (i = 0; i < MENU_MODES; ++i)
	{
	    إذا (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		إذا (amenu && call_معلومات != عدم && *call_معلومات != NUL
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
		       && addtearoff
#جيڪڏهن_پورو
				       )
		{
		    بدل (1 << i)
		    {
			حالة MENU_VISUAL_گهڻائي:
			حالة MENU_SELECT_گهڻائي:
			حالة MENU_OP_PENDING_گهڻائي:
			حالة MENU_CMDLINE_گهڻائي:
			    c = Ctrl_C;
			    اكسر;
			حالة MENU_وجهو_گهڻائي:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    اكسر;
		    }
		}

		إذا (c != 0)
		{
		    menu->لڙيون[i] = alloc(STRLEN(call_معلومات) + 5);
		    إذا (menu->لڙيون[i] != عدم)
		    {
			menu->لڙيون[i][0] = c;
			إذا (d == 0)
			    STRCPY(menu->لڙيون[i] + 1, call_معلومات);
			وإلا
			{
			    menu->لڙيون[i][1] = d;
			    STRCPY(menu->لڙيون[i] + 2, call_معلومات);
			}
			إذا (c == Ctrl_C)
			{
			    صحيح	    len = (صحيح)STRLEN(menu->لڙيون[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->لڙيون[i][len] = Ctrl_BSL;
			    menu->لڙيون[i][len + 1] = Ctrl_G;
			    menu->لڙيون[i][len + 2] = NUL;
			}
		    }
		}
		وإلا
		    menu->لڙيون[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	إذا (modes & MENU_TIP_گهڻائي)
	    gui_mch_menu_سيٽ_tip(menu);
#جيڪڏهن_پورو
    }
    أرجع OK;

erret:
    vim_free(رستو_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    طالما (والد != عدم && والد->children == عدم)
    {
	إذا (والد->والد == عدم)
	    menup = پاڙ_menu_ptr;
	وإلا
	    menup = &والد->والد->children;
	لكل ( ; *menup != عدم && *menup != والد; menup = &((*menup)->ايندڙ))
	    ;
	إذا (*menup == عدم) // safety check
	    اكسر;
	والد = والد->والد;
	free_menu(menup);
    }
    أرجع FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    ساكن صحيح
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    صحيح		modes,
    صحيح		enable)
{
    char_u	*p;

    إذا (menu == عدم)
	أرجع OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    طالما (menu != عدم)
    {
	إذا (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    إذا (*p != NUL)
	    {
		إذا (menu->children == عدم)
		{
		    emsg(_(e_part_جو_menu_شي_رستو_is_not_sub_menu));
		    أرجع FAIL;
		}
		إذا (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    أرجع FAIL;
	    }
	    وإلا
		إذا (enable)
		    menu->enabled |= modes;
		وإلا
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    إذا (*name != NUL && *name != '*')
		اكسر;
	}
	menu = menu->ايندڙ;
    }
    إذا (*name != NUL && *name != '*' && menu == عدم)
    {
	semsg(_(e_no_menu_str), name);
	أرجع FAIL;
    }

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#جيڪڏهن_پورو

    أرجع OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    ساكن صحيح
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    صحيح		modes,
    صحيح		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*ٻار;
    char_u	*p;

    إذا (*menup == عدم)
	أرجع OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    طالما ((menu = *menup) != عدم)
    {
	إذا (*name == NUL || menu_name_equal(name, menu))
	{
	    إذا (*p != NUL && menu->children == عدم)
	    {
		إذا (!silent)
		    emsg(_(e_part_جو_menu_شي_رستو_is_not_sub_menu));
		أرجع FAIL;
	    }
	    إذا ((menu->modes & modes) != 0x0)
	    {
#إذا defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		إذا (*p == NUL && modes == MENU_ALL_MODES)
		{
		    إذا (Is_دري(menu->tearoff_handle))
			Destroy_دري(menu->tearoff_handle);
		}
#جيڪڏهن_پورو
		إذا (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (*name != NUL)
	    {
		إذا (!silent)
		    emsg(_(e_menu_only_exists_اندريون_another_گهڻائي));
		أرجع FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    إذا (*name != NUL)
		اكسر;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    إذا (modes & MENU_TIP_گهڻائي)
		free_menu_string(menu, MENU_اشاريو_TIP);
	    إذا ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    وإلا
		menup = &menu->ايندڙ;
	}
	وإلا
	    menup = &menu->ايندڙ;
    }
    إذا (*name != NUL)
    {
	إذا (menu == عدم)
	{
	    إذا (!silent)
		semsg(_(e_no_menu_str), name);
	    أرجع FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#إذا defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	إذا ((s_tearoffs) && (menu->children != عدم)) // there's a tear bar.
	    ٻار = menu->children->ايندڙ; // don't count tearoff bar
	وإلا
#جيڪڏهن_پورو
	    ٻار = menu->children;
	لكل ( ; ٻار != عدم; ٻار = ٻار->ايندڙ)
	    menu->modes |= ٻار->modes;
	إذا (modes & MENU_TIP_گهڻائي)
	{
	    free_menu_string(menu, MENU_اشاريو_TIP);
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    إذا (gui.اندريون_use)
		gui_mch_menu_سيٽ_tip(menu);
#جيڪڏهن_پورو
	}
	إذا ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#إذا defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    إذا (s_tearoffs && menu->children != عدم) // there's a tear bar.
		free_menu(&menu->children);
#جيڪڏهن_پورو
	    *menup = menu;
	    free_menu(menup);
	}
    }

    أرجع OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    فراغ
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_شيون);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    ساكن فراغ
free_menu(vimmenu_T **menup)
{
    صحيح		i;
    vimmenu_T	*menu;

    menu = *menup;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    إذا (gui.اندريون_use)
	gui_mch_destroy_menu(menu);
#جيڪڏهن_پورو

    // Don't change *menup until after calling gui_mch_destroy_menu(). The
    // MacOS code needs the original structure to properly delete the menu.
    *menup = menu->ايندڙ;
    vim_free(menu->name);
    vim_free(menu->dname);
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    vim_free(menu->en_name);
    vim_free(menu->en_dname);
#جيڪڏهن_پورو
    vim_free(menu->actext);
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    vim_free(menu->iconfile);
# ifdef FEAT_GUI_MOTIF
    vim_free(menu->xpm_fname);
# endif
#جيڪڏهن_پورو
    لكل (i = 0; i < MENU_MODES; i++)
	free_menu_string(menu, i);
    vim_free(menu);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#جيڪڏهن_پورو
}

/*
 * Free the menu->string with the given index.
 */
    ساكن فراغ
free_menu_string(vimmenu_T *menu, صحيح idx)
{
    صحيح		ڳڻپ = 0;
    صحيح		i;

    لكل (i = 0; i < MENU_MODES; i++)
	إذا (menu->لڙيون[i] == menu->لڙيون[idx])
	    ڳڻپ++;
    إذا (ڳڻپ == 1)
	vim_free(menu->لڙيون[idx]);
    menu->لڙيون[idx] = عدم;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    ساكن صحيح
show_menus(char_u *رستو_name, صحيح modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*والد = عدم;

    name = رستو_name = vim_strsave(رستو_name);
    إذا (رستو_name == عدم)
	أرجع FAIL;
    menu = *get_پاڙ_menu(رستو_name);

    // First, find the (sub)menu with the given name
    طالما (*name)
    {
	p = menu_name_skip(name);
	طالما (menu != عدم)
	{
	    إذا (menu_name_equal(name, menu))
	    {
		// Found menu
		إذا (*p != NUL && menu->children == عدم)
		{
		    emsg(_(e_part_جو_menu_شي_رستو_is_not_sub_menu));
		    vim_free(رستو_name);
		    أرجع FAIL;
		}
		وإلا إذا ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_اندريون_another_گهڻائي));
		    vim_free(رستو_name);
		    أرجع FAIL;
		}
		اكسر;
	    }
	    menu = menu->ايندڙ;
	}
	إذا (menu == عدم)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(رستو_name);
	    أرجع FAIL;
	}
	name = p;
	والد = menu;
	menu = menu->children;
    }
    vim_free(رستو_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    نياپو_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(والد, modes, 0);

    --menus_locked;
    أرجع OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ساكن فراغ
show_menus_recursive(vimmenu_T *menu, صحيح modes, صحيح اونهائي)
{
    صحيح		i;
    صحيح		ٻٽو_انگ;

    إذا (menu != عدم && (menu->modes & modes) == 0x0)
	أرجع;

    إذا (menu != عدم)
    {
	نياپو_putchar('\n');
	إذا (got_int)		// "q" hit for "--more--"
	    أرجع;
	لكل (i = 0; i < اونهائي; i++)
	    نياپو_puts("  ");
	إذا (menu->ترجيح)
	{
	    نياپو_outnum((ڊگهو)menu->ترجيح);
	    نياپو_puts(" ");
	}
				// Same highlighting as for directories!?
	نياپو_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    إذا (menu != عدم && menu->children == عدم)
    {
	لكل (ٻٽو_انگ = 0; ٻٽو_انگ < MENU_MODES; ٻٽو_انگ++)
	    إذا ((menu->modes & modes & (1 << ٻٽو_انگ)) != 0)
	    {
		نياپو_putchar('\n');
		إذا (got_int)		// "q" hit for "--more--"
		    أرجع;
		لكل (i = 0; i < اونهائي + 2; i++)
		    نياپو_puts("  ");
		نياپو_puts(menu_گهڻائي_chars[ٻٽو_انگ]);
		إذا (menu->noremap[ٻٽو_انگ] == REMAP_NONE)
		    نياپو_putchar('*');
		وإلا إذا (menu->noremap[ٻٽو_انگ] == REMAP_SCRIPT)
		    نياپو_putchar('&');
		وإلا
		    نياپو_putchar(' ');
		إذا (menu->silent[ٻٽو_انگ])
		    نياپو_putchar('s');
		وإلا
		    نياپو_putchar(' ');
		إذا ((menu->modes & menu->enabled & (1 << ٻٽو_انگ)) == 0)
		    نياپو_putchar('-');
		وإلا
		    نياپو_putchar(' ');
		نياپو_puts(" ");
		إذا (*menu->لڙيون[ٻٽو_انگ] == NUL)
		    نياپو_puts_attr("<Nop>", HL_ATTR(HLF_8));
		وإلا
		    نياپو_outtrans_special(menu->لڙيون[ٻٽو_انگ], FALSE, 0);
	    }
    }
    وإلا
    {
	إذا (menu == عدم)
	{
	    menu = پاڙ_menu;
	    اونهائي--;
	}
	وإلا
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	لكل (; menu != عدم && !got_int; menu = menu->ايندڙ)
	    إذا (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, اونهائي + 1);
    }
}

/*
 * Used when expanding menu names.
 */
ساكن vimmenu_T	*expand_menu = عدم;
ساكن vimmenu_T	*expand_menu_alt = عدم;
ساكن صحيح		expand_modes = 0x0;
ساكن صحيح		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
سيٽ_context_اندريون_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    صحيح		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*رستو_name = عدم;
    char_u	*name;
    صحيح		unmenu;
    vimmenu_T	*menu;
    صحيح		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    لكل (p = arg; *p; ++p)
	إذا (!VIM_ISDIGIT(*p) && *p != '.')
	    اكسر;

    إذا (!VIM_ISWHITE(*p))
    {
	إذا (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	وإلا إذا (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	وإلا
	    p = arg;
    }

    طالما (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    لكل (; *p && !VIM_ISWHITE(*p); ++p)
    {
	إذا ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	وإلا إذا (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    إذا (expand_menus && VIM_ISWHITE(*p))
	أرجع عدم;	// TODO: check for next command?
    إذا (*p == NUL)		// Complete the menu name
    {
	صحيح try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, عدم, &unmenu);
	إذا (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = پاڙ_menu;
	إذا (after_dot != arg)
	{
	    رستو_name = alloc(after_dot - arg);
	    إذا (رستو_name == عدم)
		أرجع عدم;
	    vim_strncpy(رستو_name, arg, after_dot - arg - 1);
	}
	name = رستو_name;
	طالما (name != عدم && *name)
	{
	    p = menu_name_skip(name);
	    طالما (menu != عدم)
	    {
		إذا (menu_name_equal(name, menu))
		{
		    // Found menu
		    إذا ((*p != NUL && menu->children == عدم)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(رستو_name);
			أرجع عدم;
		    }
		    اكسر;
		}
		menu = menu->ايندڙ;
		إذا (menu == عدم && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    إذا (menu == عدم)
	    {
		// No menu found with the name we were looking for
		vim_free(رستو_name);
		أرجع عدم;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(رستو_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_نمونو = after_dot;
	expand_menu = menu;
	إذا (expand_menu == پاڙ_menu)
	    expand_menu_alt = curwin->w_winbar;
	وإلا
	    expand_menu_alt = عدم;
    }
    وإلا			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    أرجع عدم;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    ساكن vimmenu_T	*menu = عدم;
    ساكن صحيح		did_alt_menu = FALSE;
    char_u		*str;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    ساكن  صحيح		should_advance = FALSE;
#جيڪڏهن_پورو

    إذا (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	should_advance = FALSE;
#جيڪڏهن_پورو
    }

    // Skip PopUp[nvoci].
    طالما (menu != عدم && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == عدم))
    {
	menu = menu->ايندڙ;
	إذا (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    إذا (menu == عدم)	    // at end of linked list
	أرجع عدم;

    إذا (menu->modes & expand_modes)
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	إذا (should_advance)
	    str = menu->en_dname;
	وإلا
	{
#جيڪڏهن_پورو
	    str = menu->dname;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	    إذا (menu->en_dname == عدم)
		should_advance = TRUE;
	}
#جيڪڏهن_پورو
    وإلا
	str = (char_u *)"";

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    إذا (should_advance)
#جيڪڏهن_پورو
    {
	// Advance to next menu entry.
	menu = menu->ايندڙ;
	إذا (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    should_advance = !should_advance;
#جيڪڏهن_پورو

    أرجع str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_نالا(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    ساكن vimmenu_T	*menu = عدم;
    ساكن صحيح		did_alt_menu = FALSE;
#وصف_ڪريو TBUFFER_LEN 256
    ساكن char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    ساكن  صحيح		should_advance = FALSE;
#جيڪڏهن_پورو

    إذا (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	should_advance = FALSE;
#جيڪڏهن_پورو
    }

    // Skip Browse-style entries, popup menus and separators.
    طالما (menu != عدم
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#جيڪڏهن_وصف_نه_ٿيل FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#جيڪڏهن_پورو
	       ))
    {
	menu = menu->ايندڙ;
	إذا (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    إذا (menu == عدم)	    // at end of linked list
	أرجع عدم;

    إذا (menu->modes & expand_modes)
    {
	إذا (menu->children != عدم)
	{
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	    إذا (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    وإلا
	    {
#جيڪڏهن_پورو
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
		إذا (menu->en_dname == عدم)
		    should_advance = TRUE;
	    }
#جيڪڏهن_پورو
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	وإلا
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
	{
	    إذا (should_advance)
		str = menu->en_dname;
	    وإلا
	    {
#جيڪڏهن_پورو
		str = menu->dname;
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
		إذا (menu->en_dname == عدم)
		    should_advance = TRUE;
	    }
	}
#جيڪڏهن_پورو
    }
    وإلا
	str = (char_u *)"";

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    إذا (should_advance)
#جيڪڏهن_پورو
    {
	// Advance to next menu entry.
	menu = menu->ايندڙ;
	إذا (menu == عدم && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    should_advance = !should_advance;
#جيڪڏهن_پورو

    أرجع str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    ساكن char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    لكل (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	إذا (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    إذا (*p == NUL)
		اكسر;
	}
    }
    إذا (*p)
	*p++ = NUL;
    أرجع p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ساكن صحيح
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    إذا (menu->en_name != عدم
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	أرجع TRUE;
#جيڪڏهن_پورو
    أرجع menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ساكن صحيح
menu_namecmp(char_u *name, char_u *mname)
{
    صحيح		i;

    لكل (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	إذا (name[i] != mname[i])
	    اكسر;
    أرجع ((name[i] == NUL || name[i] == TAB)
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
    ساكن صحيح
get_menu_cmd_modes(
    char_u  *cmd,
    صحيح	    forceit,	    // Was there a "!" after the command?
    صحيح	    *noremap,
    صحيح	    *unmenu)
{
    صحيح	    modes;

    بدل (*cmd++)
    {
	حالة 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي;
	    اكسر;
	حالة 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_گهڻائي;
	    اكسر;
	حالة 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_گهڻائي;
	    اكسر;
	حالة 'o':			// omenu
	    modes = MENU_OP_PENDING_گهڻائي;
	    اكسر;
	حالة 'i':			// imenu
	    modes = MENU_وجهو_گهڻائي;
	    اكسر;
	حالة 't':
	    إذا (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_گهڻائي;
		++cmd;
		اكسر;
	    }
	    modes = MENU_TIP_گهڻائي;	// tmenu
	    اكسر;
	حالة 'c':			// cmenu
	    modes = MENU_CMDLINE_گهڻائي;
	    اكسر;
	حالة 'a':			// amenu
	    modes = MENU_وجهو_گهڻائي | MENU_CMDLINE_گهڻائي | MENU_عام_گهڻائي
				    | MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي
				    | MENU_OP_PENDING_گهڻائي;
	    اكسر;
	حالة 'n':
	    إذا (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_عام_گهڻائي;
		اكسر;
	    }
	    // FALLTHROUGH
	افتراضي:
	    --cmd;
	    إذا (forceit)		// menu!!
		modes = MENU_وجهو_گهڻائي | MENU_CMDLINE_گهڻائي;
	    وإلا			// menu
		modes = MENU_عام_گهڻائي | MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي
						       | MENU_OP_PENDING_گهڻائي;
    }

    إذا (noremap != عدم)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    إذا (unmenu != عدم)
	*unmenu = (*cmd == 'u');
    أرجع modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ساكن char_u *
get_menu_گهڻائي_str(صحيح modes)
{
    إذا ((modes & (MENU_وجهو_گهڻائي | MENU_CMDLINE_گهڻائي | MENU_عام_گهڻائي |
		  MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي | MENU_OP_PENDING_گهڻائي))
	    == (MENU_وجهو_گهڻائي | MENU_CMDLINE_گهڻائي | MENU_عام_گهڻائي |
		MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي | MENU_OP_PENDING_گهڻائي))
	أرجع (char_u *)"a";
    إذا ((modes & (MENU_عام_گهڻائي | MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي |
		  MENU_OP_PENDING_گهڻائي))
	    == (MENU_عام_گهڻائي | MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي |
		MENU_OP_PENDING_گهڻائي))
	أرجع (char_u *)" ";
    إذا ((modes & (MENU_وجهو_گهڻائي | MENU_CMDLINE_گهڻائي))
	    == (MENU_وجهو_گهڻائي | MENU_CMDLINE_گهڻائي))
	أرجع (char_u *)"!";
    إذا ((modes & (MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي))
	    == (MENU_VISUAL_گهڻائي | MENU_SELECT_گهڻائي))
	أرجع (char_u *)"v";
    إذا (modes & MENU_VISUAL_گهڻائي)
	أرجع (char_u *)"x";
    إذا (modes & MENU_SELECT_گهڻائي)
	أرجع (char_u *)"s";
    إذا (modes & MENU_OP_PENDING_گهڻائي)
	أرجع (char_u *)"o";
    إذا (modes & MENU_وجهو_گهڻائي)
	أرجع (char_u *)"i";
    إذا (modes & MENU_TERMINAL_گهڻائي)
	أرجع (char_u *)"tl";
    إذا (modes & MENU_CMDLINE_گهڻائي)
	أرجع (char_u *)"c";
    إذا (modes & MENU_عام_گهڻائي)
	أرجع (char_u *)"n";
    إذا (modes & MENU_TIP_گهڻائي)
	أرجع (char_u *)"t";

    أرجع (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    ساكن char_u *
popup_گهڻائي_name(char_u *name, صحيح idx)
{
    char_u	*p;
    صحيح		len = (صحيح)STRLEN(name);
    حرف	*گهڻائي_chars = menu_گهڻائي_chars[idx];
    صحيح		گهڻائي_chars_len = (صحيح)لڙي_ڊيگهه(گهڻائي_chars);
    صحيح		i;

    p = vim_strnsave(name, len + گهڻائي_chars_len);
    إذا (p == عدم)
	أرجع عدم;

    mch_memmove(p + 5 + گهڻائي_chars_len, p + 5, (size_t)(len - 4));
    لكل (i = 0; i < گهڻائي_chars_len; ++i)
	p[5 + i] = menu_گهڻائي_chars[idx][i];
    أرجع p;
}

#إذا defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    صحيح
get_menu_اشاريو(vimmenu_T *menu, صحيح state)
{
    صحيح		idx;

    إذا ((state & گهڻائي_وجهو))
	idx = MENU_اشاريو_وجهو;
    وإلا إذا (state & گهڻائي_CMDLINE)
	idx = MENU_اشاريو_CMDLINE;
# ifdef FEAT_TERMINAL
    وإلا إذا (رڪن_use_loop())
	idx = MENU_اشاريو_TERMINAL;
# endif
    وإلا إذا (VIsual_سرگرم)
    {
	إذا (VIsual_select)
	    idx = MENU_اشاريو_SELECT;
	وإلا
	    idx = MENU_اشاريو_VISUAL;
    }
    وإلا إذا (state == گهڻائي_HITRETURN || state == گهڻائي_ASKMORE)
	idx = MENU_اشاريو_CMDLINE;
    وإلا إذا (پڄاڻي_op)
	idx = MENU_اشاريو_OP_PENDING;
    وإلا إذا ((state & گهڻائي_عام))
	idx = MENU_اشاريو_عام;
    وإلا
	idx = MENU_اشاريو_INVALID;

    إذا (idx != MENU_اشاريو_INVALID && menu->لڙيون[idx] == عدم)
	idx = MENU_اشاريو_INVALID;
    أرجع idx;
}
#جيڪڏهن_پورو

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    ساكن char_u *
menu_متن(char_u *str, صحيح *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*متن;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    إذا (p != عدم)
    {
	إذا (actext != عدم)
	    *actext = vim_strsave(p + 1);
	متن = vim_strnsave(str, p - str);
    }
    وإلا
	متن = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    لكل (p = متن; p != عدم; )
    {
	p = vim_strchr(p, '&');
	إذا (p != عدم)
	{
	    إذا (p[1] == NUL)	    // trailing "&"
		اكسر;
	    إذا (mnemonic != عدم && p[1] != '&')
#إذا !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#وإلا
	    {
		/*
		 * Well there is a bug in the Motif libraries on OS390 Unix.
		 * The mnemonic keys needs to be converted to ASCII values
		 * first.
		 * This behavior has been seen in 2.8 and 2.9.
		 */
		حرف c = p[1];
		__etoa_l(&c, 1);
		*mnemonic = c;
	    }
#جيڪڏهن_پورو
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    أرجع متن;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    صحيح
menu_is_menubar(char_u *name)
{
    أرجع (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    صحيح
menu_is_popup(char_u *name)
{
    أرجع (STRNCMP(name, "PopUp", 5) == 0);
}

#إذا defined(FEAT_GUI_MOTIF) && (Xm_نسخو <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    صحيح
menu_is_ٻار_جو_popup(vimmenu_T *menu)
{
    طالما (menu->والد != عدم)
	menu = menu->والد;
    أرجع menu_is_popup(menu->name);
}
#جيڪڏهن_پورو

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    صحيح
menu_is_toolbar(char_u *name)
{
    أرجع (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    صحيح
menu_is_separator(char_u *name)
{
    أرجع (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ساكن صحيح
menu_is_hidden(char_u *name)
{
    أرجع (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ساكن صحيح
menu_is_tearoff(char_u *name اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    أرجع (STRCMP(name, TEAR_STRING) == 0);
#وإلا
    أرجع FALSE;
#جيڪڏهن_پورو
}

#إذا defined(FEAT_GUI) || defined(FEAT_رڪن_POPUP_MENU)

    ساكن صحيح
get_menu_گهڻائي(فراغ)
{
# ifdef FEAT_TERMINAL
    إذا (رڪن_use_loop())
	أرجع MENU_اشاريو_TERMINAL;
# endif
    إذا (VIsual_سرگرم)
    {
	إذا (VIsual_select)
	    أرجع MENU_اشاريو_SELECT;
	أرجع MENU_اشاريو_VISUAL;
    }
    إذا (State & گهڻائي_وجهو)
	أرجع MENU_اشاريو_وجهو;
    إذا ((State & گهڻائي_CMDLINE) || State == گهڻائي_ASKMORE
						    || State == گهڻائي_HITRETURN)
	أرجع MENU_اشاريو_CMDLINE;
    إذا (پڄاڻي_op)
	أرجع MENU_اشاريو_OP_PENDING;
    إذا (State & گهڻائي_عام)
	أرجع MENU_اشاريو_عام;
    إذا (State & گهڻائي_LANGMAP)	// must be a "r" command, like Insert mode
	أرجع MENU_اشاريو_وجهو;
    أرجع MENU_اشاريو_INVALID;
}

    صحيح
get_menu_گهڻائي_flag(فراغ)
{
    صحيح گهڻائي = get_menu_گهڻائي();

    إذا (گهڻائي == MENU_اشاريو_INVALID)
	أرجع 0;
    أرجع 1 << گهڻائي;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    فراغ
show_popupmenu(فراغ)
{
    vimmenu_T	*menu;
    صحيح		menu_گهڻائي;
    حرف*	گهڻائي;
    صحيح		گهڻائي_len;

    menu_گهڻائي = get_menu_گهڻائي();
    إذا (menu_گهڻائي == MENU_اشاريو_INVALID)
	أرجع;
    گهڻائي = menu_گهڻائي_chars[menu_گهڻائي];
    گهڻائي_len = (صحيح)لڙي_ڊيگهه(گهڻائي);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)گهڻائي, عدم, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	إذا (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, گهڻائي, گهڻائي_len) == 0)
	    اكسر;

    // Only show a popup when it is defined and has entries
    إذا (menu == عدم || menu->children == عدم)
	أرجع;

# إذا defined(FEAT_GUI)
    إذا (gui.اندريون_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# إذا defined(FEAT_GUI) && defined(FEAT_رڪن_POPUP_MENU)
    وإلا
# endif
# إذا defined(FEAT_رڪن_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    صحيح
check_menu_اشارو(vimmenu_T *پاڙ, vimmenu_T *menu_منزل_check)
{
    vimmenu_T	*p;

    لكل (p = پاڙ; p != عدم; p = p->ايندڙ)
	إذا (p == menu_منزل_check
		|| (p->children != عدم
		    && check_menu_اشارو(p->children, menu_منزل_check) == OK))
	    أرجع OK;
    أرجع FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    فراغ
gui_create_initial_menus(vimmenu_T *menu)
{
    صحيح		idx = 0;

    طالما (menu != عدم)
    {
	// Don't add a menu when only a tip was defined.
	إذا (menu->modes & MENU_ALL_MODES)
	{
	    إذا (menu->children != عدم)
	    {
		gui_mch_جوڙيو_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    وإلا
		gui_mch_جوڙيو_menu_شي(menu, idx);
	}
	menu = menu->ايندڙ;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    ساكن فراغ
gui_update_menus_recurse(vimmenu_T *menu, صحيح گهڻائي)
{
    صحيح		grey;

    طالما (menu)
    {
	إذا ((menu->modes & menu->enabled & گهڻائي)
# إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	وإلا
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	إذا (vim_strchr(p_go, GO_GREY) != عدم || menu->والد == عدم
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->والد->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	وإلا
	    gui_mch_menu_hidden(menu, grey);
	gui_update_menus_recurse(menu->children, گهڻائي);
	menu = menu->ايندڙ;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    فراغ
gui_update_menus(صحيح modes)
{
    ساكن صحيح	    prev_گهڻائي = -1;
    صحيح		    گهڻائي = 0;

    إذا (modes != 0x0)
	گهڻائي = modes;
    وإلا
	گهڻائي = get_menu_گهڻائي_flag();

    إذا (force_menu_update || گهڻائي != prev_گهڻائي)
    {
	gui_update_menus_recurse(پاڙ_menu, گهڻائي);
	gui_mch_draw_menubar();
	prev_گهڻائي = گهڻائي;
	force_menu_update = FALSE;
    }
}

# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    صحيح
gui_is_menu_shortcut(صحيح ڪنجي)
{
    vimmenu_T	*menu;

    إذا (ڪنجي < 256)
	ڪنجي = TOLOWER_LOC(ڪنجي);
    FOR_ALL_MENUS(menu)
	إذا (menu->mnemonic == ڪنجي
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == ڪنجي))
	    أرجع TRUE;
    أرجع FALSE;
}
# endif
#جيڪڏهن_پورو // FEAT_GUI

#إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    صحيح		pri_tab[MENUDEPTH + 1];
    صحيح		i;

    إذا (enable)
    {
	لكل (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(پاڙ_menu, (char_u *)"", pri_tab, 0);
    }
    وإلا
	gui_destroy_tearoffs_recurse(پاڙ_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    ساكن فراغ
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ثابت char_u	*pname,
    صحيح			*pri_tab,
    صحيح			pri_idx)
{
    char_u	*newpname = عدم;
    صحيح		len;
    char_u	*s;
    char_u	*d;

    إذا (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    طالما (menu != عدم)
    {
	إذا (menu->children != عدم && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (صحيح)STRLEN(pname) + (صحيح)STRLEN(menu->name);
	    لكل (s = menu->name; *s; ++s)
		إذا (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    إذا (newpname != عدم)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		لكل (s = menu->name; *s; ++s)
		{
		    إذا (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		إذا (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_جوڙيو_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			// remove TEAR_STRING
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->ايندڙ;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    ساكن فراغ
gui_جوڙيو_tearoff(char_u *tearpath, صحيح *pri_tab, صحيح pri_idx)
{
    char_u	*tbuf;
    صحيح		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (بنانشان صحيح)STRLEN(tearpath));
    إذا (tbuf == عدم)
	أرجع;

    tbuf[0] = K_SPECIAL;
    tbuf[1] = K_ٻيو(K_TEAROFF);
    tbuf[2] = K_ٽيون(K_TEAROFF);
    STRCPY(tbuf + 3, tearpath);
    STRCAT(tbuf + 3, "\r");

    STRCAT(tearpath, ".");
    STRCAT(tearpath, TEAR_STRING);

    // Priority of tear-off is always 1
    t = pri_tab[pri_idx + 1];
    pri_tab[pri_idx + 1] = 1;

# ifdef FEAT_TOOLBAR
    menuarg.iconfile = عدم;
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
# endif
    menuarg.noremap[0] = REMAP_NONE;
    menuarg.silent[0] = TRUE;

    menuarg.modes = MENU_ALL_MODES;
    جوڙيو_menu_رستو(tearpath, &menuarg, pri_tab, tbuf, FALSE);

    menuarg.modes = MENU_TIP_گهڻائي;
    جوڙيو_menu_رستو(tearpath, &menuarg, pri_tab,
	    (char_u *)_("Tear off this menu"), FALSE);

    pri_tab[pri_idx + 1] = t;
    vim_free(tbuf);
}

/*
 * Recursively destroy tearoff items
 */
    ساكن فراغ
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    طالما (menu)
    {
	إذا (menu->children)
	{
	    // check if tearoff exists
	    إذا (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    إذا (menu->children != عدم) // if not the last one
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->ايندڙ;
    }
}

#جيڪڏهن_پورو // FEAT_GUI_MSWIN && FEAT_TEAROFF

/*
 * Execute "menu".  Use by ":emenu" and the window toolbar.
 * "eap" is NULL for the window toolbar.
 * "mode_idx" specifies a MENU_INDEX_ value, use MENU_INDEX_INVALID to depend
 * on the current state.
 */
    فراغ
execute_menu(exarg_T *eap, vimmenu_T *menu, صحيح گهڻائي_idx)
{
    صحيح		idx = گهڻائي_idx;

    إذا (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	إذا (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_اشاريو_وجهو;
	}
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	وإلا إذا (رڪن_use_loop())
	{
	    idx = MENU_اشاريو_TERMINAL;
	}
#جيڪڏهن_پورو
	وإلا إذا (VIsual_سرگرم)
	{
	    idx = MENU_اشاريو_VISUAL;
	}
	وإلا إذا (eap != عدم && eap->addr_ڳڻپ)
	{
	    pos_T	tpos;

	    idx = MENU_اشاريو_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    إذا ((curbuf->b_visual.vi_شروعات.lnum == eap->line1)
		    && (curbuf->b_visual.vi_پڇاڙي.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_گهڻائي = curbuf->b_visual.vi_گهڻائي;
		tpos = curbuf->b_visual.vi_پڇاڙي;
		curwin->w_cursor = curbuf->b_visual.vi_شروعات;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    وإلا
	    {
		// Set it up for line-wise visual mode
		VIsual_گهڻائي = 'V';
		curwin->w_cursor.lnum = eap->line1;
		curwin->w_cursor.کانو = 1;
		tpos.lnum = eap->line2;
		tpos.کانو = MAXCOL;
		tpos.coladd = 0;
	    }

	    // Activate visual mode
	    VIsual_سرگرم = TRUE;
	    VIsual_reselect = TRUE;
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;

	    check_cursor();

	    // Adjust the cursor to make sure it is in the correct pos
	    // for exclusive mode
	    إذا (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.کانو;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    إذا (idx == MENU_اشاريو_INVALID || eap == عدم)
	idx = MENU_اشاريو_عام;

    إذا (menu->لڙيون[idx] != عدم && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	إذا (eap == عدم || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_عام_busy;
	    إذا (save_current_state(&save_state))
		exec_عام_cmd(menu->لڙيون[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_عام_busy;
	}
	وإلا
	    ins_typebuf(menu->لڙيون[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    وإلا إذا (eap != عدم)
    {
	char_u	*گهڻائي;

	بدل (idx)
	{
	    حالة MENU_اشاريو_VISUAL:
		گهڻائي = (char_u *)"Visual";
		اكسر;
	    حالة MENU_اشاريو_SELECT:
		گهڻائي = (char_u *)"Select";
		اكسر;
	    حالة MENU_اشاريو_OP_PENDING:
		گهڻائي = (char_u *)"Op-pending";
		اكسر;
	    حالة MENU_اشاريو_TERMINAL:
		گهڻائي = (char_u *)"Terminal";
		اكسر;
	    حالة MENU_اشاريو_وجهو:
		گهڻائي = (char_u *)"Insert";
		اكسر;
	    حالة MENU_اشاريو_CMDLINE:
		گهڻائي = (char_u *)"Cmdline";
		اكسر;
	    // case MENU_INDEX_TIP: cannot happen
	    افتراضي:
		گهڻائي = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_گهڻائي), گهڻائي);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    ساكن vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    صحيح		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    إذا (saved_name == عدم)
	أرجع عدم;

    menu = *get_پاڙ_menu(saved_name);
    name = saved_name;
    طالما (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	طالما (menu != عدم)
	{
	    إذا (menu_name_equal(name, menu))
	    {
		إذا (*p == NUL && menu->children != عدم)
		{
		    emsg(_(e_menu_رستو_must_lead_منزل_menu_شي));
		    gave_emsg = TRUE;
		    menu = عدم;
		}
		وإلا إذا (*p != NUL && menu->children == عدم)
		{
		    emsg(_(e_part_جو_menu_شي_رستو_is_not_sub_menu));
		    menu = عدم;
		}
		اكسر;
	    }
	    menu = menu->ايندڙ;
	}
	إذا (menu == عدم || *p == NUL)
	    اكسر;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    إذا (menu == عدم)
    {
	إذا (!gave_emsg)
	    semsg(_(e_menu_not_مليو_str), name_arg);
	أرجع عدم;
    }

    أرجع menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    فراغ
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    صحيح		گهڻائي_idx = MENU_اشاريو_INVALID;

    إذا (arg[0] && VIM_ISWHITE(arg[1]))
    {
	بدل (arg[0])
	{
	    حالة 'n': گهڻائي_idx = MENU_اشاريو_عام; اكسر;
	    حالة 'v': گهڻائي_idx = MENU_اشاريو_VISUAL; اكسر;
	    حالة 's': گهڻائي_idx = MENU_اشاريو_SELECT; اكسر;
	    حالة 'o': گهڻائي_idx = MENU_اشاريو_OP_PENDING; اكسر;
	    حالة 't': گهڻائي_idx = MENU_اشاريو_TERMINAL; اكسر;
	    حالة 'i': گهڻائي_idx = MENU_اشاريو_وجهو; اكسر;
	    حالة 'c': گهڻائي_idx = MENU_اشاريو_CMDLINE; اكسر;
	    افتراضي: semsg(_(e_invalid_دليل_str), arg);
		     أرجع;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    إذا (menu == عدم)
	أرجع;

    // Found the menu, so execute.
    execute_menu(eap, menu, گهڻائي_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    فراغ
winbar_click(win_T *wp, صحيح کانو)
{
    صحيح		idx;

    إذا (wp->w_winbar_شيون == عدم)
	أرجع;
    لكل (idx = 0; wp->w_winbar_شيون[idx].wb_menu != عدم; ++idx)
    {
	winbar_شي_T *شي = &wp->w_winbar_شيون[idx];

	إذا (کانو >= شي->wb_startcol && کانو <= شي->wb_endcol)
	{
	    win_T   *save_curwin = عدم;
	    pos_T   save_visual = VIsual;
	    صحيح	    save_visual_سرگرم = VIsual_سرگرم;
	    صحيح	    save_visual_select = VIsual_select;
	    صحيح	    save_visual_reselect = VIsual_reselect;
	    صحيح	    save_visual_گهڻائي = VIsual_گهڻائي;

	    إذا (wp != curwin)
	    {
		// Clicking in the window toolbar of a not-current window.
		// Make that window the current one and save Visual mode.
		save_curwin = curwin;
		VIsual_سرگرم = FALSE;
		curwin = wp;
		curbuf = curwin->w_بفر;
		check_cursor();
	    }

	    // Note: the command might close the current window.
	    execute_menu(عدم, شي->wb_menu, -1);

	    إذا (save_curwin != عدم && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_بفر;
		VIsual = save_visual;
		VIsual_سرگرم = save_visual_سرگرم;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_گهڻائي = save_visual_گهڻائي;
	    }
	    إذا (!win_valid(wp))
		اكسر;
	}
    }
}

#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_رڪن_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_ڳوليو_menu(char_u *رستو_name)
{
    vimmenu_T	*menu = عدم;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_پاڙ_menu(رستو_name);

    saved_name = vim_strsave(رستو_name);
    إذا (saved_name == عدم)
	أرجع عدم;

    name = saved_name;
    طالما (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	طالما (menu != عدم)
	{
	    إذا (menu_name_equal(name, menu))
	    {
		إذا (menu->children == عدم)
		{
		    // found a menu item instead of a sub-menu
		    إذا (*p == NUL)
			emsg(_(e_menu_رستو_must_lead_منزل_sub_menu));
		    وإلا
			emsg(_(e_part_جو_menu_شي_رستو_is_not_sub_menu));
		    menu = عدم;
		    اذهب theend;
		}
		إذا (*p == NUL)	    // found a full match
		    اذهب theend;
		اكسر;
	    }
	    menu = menu->ايندڙ;
	}
	إذا (menu == عدم)	// didn't find it
	    اكسر;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    إذا (menu == عدم)
	emsg(_(e_menu_not_مليو_check_menu_نالا));
theend:
    vim_free(saved_name);
    أرجع menu;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

تعريفنوع بنية
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*منزل;		// translated name
} menutrans_T;

ساكن garray_T menutrans_ga = {0, 0, 0, 0, عدم};
#جيڪڏهن_پورو

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    فراغ
ex_menutranslate(exarg_T *eap اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    صحيح			i;
    char_u		*from, *from_noamp, *منزل;

    إذا (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, حجم(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    إذا (STRNCMP(arg, "clear", 5) == 0 && پڇاڙيون_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_معلومات;
	لكل (i = 0; i < menutrans_ga.ga_len; ++i)
	{
	    vim_free(tp[i].from);
	    vim_free(tp[i].from_noamp);
	    vim_free(tp[i].منزل);
	}
	ga_clear(&menutrans_ga);
# ifdef FEAT_EVAL
	// Delete all "menutrans_" global variables.
	del_menutrans_vars();
# endif
    }
    وإلا
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	منزل = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(منزل);
	إذا (arg == منزل || پڇاڙيون_excmd2(eap->arg, from)
		      || پڇاڙيون_excmd2(eap->arg, منزل)
		      || !پڇاڙيون_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_دليل));
	وإلا
	{
	    إذا (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_معلومات;
		from = vim_strsave(from);
		إذا (from != عدم)
		{
		    from_noamp = menu_متن(from, عدم, عدم);
		    منزل = vim_strnsave(منزل, arg - منزل);
		    إذا (from_noamp != عدم && منزل != عدم)
		    {
			menu_translate_tab_and_سرڪايو(from);
			menu_translate_tab_and_سرڪايو(منزل);
			menu_unescape_name(from);
			menu_unescape_name(منزل);
			tp[menutrans_ga.ga_len].from = from;
			tp[menutrans_ga.ga_len].from_noamp = from_noamp;
			tp[menutrans_ga.ga_len].منزل = منزل;
			++menutrans_ga.ga_len;
		    }
		    وإلا
		    {
			vim_free(from);
			vim_free(from_noamp);
			vim_free(منزل);
		    }
		}
	    }
	}
    }
#جيڪڏهن_پورو
}

#إذا defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ساكن char_u *
menu_skip_part(char_u *p)
{
    طالما (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	إذا ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    أرجع p;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    ساكن char_u *
menutrans_lookup(char_u *name, صحيح len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_معلومات;
    صحيح			i;
    char_u		*dname;

    لكل (i = 0; i < menutrans_ga.ga_len; ++i)
	إذا (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    أرجع tp[i].منزل;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_متن(name, عدم, عدم);
    name[len] = i;
    إذا (dname == عدم)
	أرجع عدم;

    لكل (i = 0; i < menutrans_ga.ga_len; ++i)
	إذا (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    أرجع tp[i].منزل;
	}
    vim_free(dname);

    أرجع عدم;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ساكن فراغ
menu_unescape_name(char_u *name)
{
    char_u  *p;

    لكل (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	إذا (*p == '\\')
	    STRMOVE(p, p + 1);
}
#جيڪڏهن_پورو // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    ساكن char_u *
menu_translate_tab_and_سرڪايو(char_u *arg_شروعات)
{
    char_u	*arg = arg_شروعات;

    طالما (*arg && !VIM_ISWHITE(*arg))
    {
	إذا ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	وإلا إذا (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    إذا (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    أرجع arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ساكن صحيح
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, صحيح modes, dict_T *dict)
{
    صحيح		حالت;
    list_T	*l;

    إذا (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	إذا (l == عدم)
	    أرجع FAIL;

	إذا (dict_جوڙيو_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    أرجع FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	لكل (topmenu = menu; topmenu != عدم; topmenu = topmenu->ايندڙ)
	    إذا (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	أرجع OK;
    }

    إذا (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	أرجع OK;

    حالت = dict_جوڙيو_string(dict, "name", menu->name);
    إذا (حالت == OK)
	حالت = dict_جوڙيو_string(dict, "display", menu->dname);
    إذا (حالت == OK && menu->actext != عدم)
	حالت = dict_جوڙيو_string(dict, "accel", menu->actext);
    إذا (حالت == OK)
	حالت = dict_جوڙيو_عدد(dict, "priority", menu->ترجيح);
    إذا (حالت == OK)
	حالت = dict_جوڙيو_string(dict, "modes",
					get_menu_گهڻائي_str(menu->modes));
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    إذا (حالت == OK && menu->iconfile != عدم)
	حالت = dict_جوڙيو_string(dict, "icon", menu->iconfile);
    إذا (حالت == OK && menu->iconidx >= 0)
	حالت = dict_جوڙيو_عدد(dict, "iconidx", menu->iconidx);
#جيڪڏهن_پورو
    إذا (حالت == OK)
    {
	char_u	buf[NUMBUFLEN];
	صحيح	buflen;

	إذا (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	وإلا
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	حالت = dict_جوڙيو_string_len(dict, "shortcut", buf, buflen);
    }
    إذا (حالت == OK && menu->children == عدم)
    {
	صحيح		ٻٽو_انگ;

	// Get the first mode in which the menu is available
	لكل (ٻٽو_انگ = 0; ٻٽو_انگ < MENU_MODES && !((1 << ٻٽو_انگ) & modes); ٻٽو_انگ++)
	    ;
	إذا (ٻٽو_انگ < MENU_MODES) // just in case, avoid Coverity warning
	{
	    إذا (menu->لڙيون[ٻٽو_انگ] != عدم)
	    {
		إذا (*menu->لڙيون[ٻٽو_انگ] == NUL)
		    حالت = dict_جوڙيو_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		وإلا
		{
		    char_u *tofree = عدم;

		    حالت = dict_جوڙيو_string(dict, "rhs",
			tofree = str2special_save(menu->لڙيون[ٻٽو_انگ], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    إذا (حالت == OK)
		حالت = dict_جوڙيو_bool(dict, "noremenu",
					     menu->noremap[ٻٽو_انگ] == REMAP_NONE);
	    إذا (حالت == OK)
		حالت = dict_جوڙيو_bool(dict, "script",
					   menu->noremap[ٻٽو_انگ] == REMAP_SCRIPT);
	    إذا (حالت == OK)
		حالت = dict_جوڙيو_bool(dict, "silent", menu->silent[ٻٽو_انگ]);
	    إذا (حالت == OK)
		حالت = dict_جوڙيو_bool(dict, "enabled",
					  ((menu->enabled & (1 << ٻٽو_انگ)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    إذا (حالت == OK && menu->children != عدم)
    {
	vimmenu_T	*ٻار;

	l = list_alloc();
	إذا (l == عدم)
	    أرجع FAIL;

	إذا (dict_جوڙيو_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    أرجع FAIL;
	}
	ٻار = menu->children;
	طالما (ٻار)
	{
	    إذا (!menu_is_tearoff(ٻار->dname))		// skip tearoff menu
		list_append_string(l, ٻار->dname, -1);
	    ٻار = ٻار->ايندڙ;
	}
    }

    أرجع حالت;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    فراغ
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    صحيح		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    retdict = rettv->vval.v_dict;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    menu_name = tv_get_string_chk(&argvars[0]);
    إذا (menu_name == عدم)
	أرجع;

    // menu mode
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    وإلا
	which = (char_u *)"";	    // Default is modes for "menu"
    إذا (which == عدم)
	أرجع;

    modes = get_menu_cmd_modes(which, *which == '!', عدم, عدم);

    // Locate the specified menu or menu item
    menu = *get_پاڙ_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    إذا (saved_name == عدم)
	أرجع;
    إذا (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	طالما (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    طالما (menu != عدم)
	    {
		إذا (menu_name_equal(name, menu))
		    اكسر;
		menu = menu->ايندڙ;
	    }
	    إذا (menu == عدم || *p == NUL)
		اكسر;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    إذا (menu == عدم)		// specified menu not found
	أرجع;

    إذا (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#جيڪڏهن_پورو // FEAT_MENU
