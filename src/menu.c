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

#شامل "vim.h"

#اگر defined(FEAT_MENU)

#تعریف_کرو MENUDEPTH   10		// maximum depth of menus

#اگر_تعریف_شدہ FEAT_GUI_MSWIN
ساکن صحیح جوڑو_menu_راستہ(char_u *, vimmenu_T *, صحیح *, char_u *, صحیح);
#ورنہ
ساکن صحیح جوڑو_menu_راستہ(char_u *, vimmenu_T *, صحیح *, char_u *);
#اگر_ختم
ساکن صحیح menu_nable_recurse(vimmenu_T *menu, char_u *name, صحیح modes, صحیح enable);
ساکن صحیح remove_menu(vimmenu_T **, char_u *, صحیح, صحیح silent);
ساکن خالی free_menu(vimmenu_T **menup);
ساکن خالی free_menu_string(vimmenu_T *, صحیح);
ساکن صحیح show_menus(char_u *, صحیح);
ساکن خالی show_menus_recursive(vimmenu_T *, صحیح, صحیح);
ساکن char_u *menu_name_skip(char_u *name);
ساکن صحیح menu_name_equal(char_u *name, vimmenu_T *menu);
ساکن صحیح menu_namecmp(char_u *name, char_u *mname);
ساکن صحیح get_menu_cmd_modes(char_u *, صحیح, صحیح *, صحیح *);
ساکن char_u *popup_منوال_name(char_u *name, صحیح idx);
ساکن char_u *menu_متن(char_u *متن, صحیح *mnemonic, char_u **actext);

#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
ساکن خالی gui_create_tearoffs_recurse(vimmenu_T *menu, ثابت char_u *pname, صحیح *pri_tab, صحیح pri_idx);
ساکن خالی gui_جوڑو_tearoff(char_u *tearpath, صحیح *pri_tab, صحیح pri_idx);
ساکن خالی gui_destroy_tearoffs_recurse(vimmenu_T *menu);
ساکن صحیح s_tearoffs = FALSE;
#اگر_ختم

ساکن صحیح menu_is_hidden(char_u *name);
ساکن صحیح menu_is_tearoff(char_u *name);

// When non-zero no menu must be added or cleared.  Prevents the list of menus
// changing while listing them.
ساکن صحیح menus_locked = 0;

#اگر defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
ساکن char_u *menu_skip_part(char_u *p);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MULTI_LANG
ساکن char_u *menutrans_lookup(char_u *name, صحیح len);
ساکن خالی menu_unescape_name(char_u	*p);
#اگر_ختم

ساکن char_u *menu_translate_tab_and_سرکاؤ(char_u *arg_آغاز);

// The character for each menu mode
ساکن حرف *menu_منوال_chars[] = {"n", "v", "s", "o", "i", "c", "tl", "t"};

#اگر_تعریف_شدہ FEAT_TOOLBAR
ساکن ثابت حرف *toolbar_نام[] =
{
    /*  0 */ "New", "Open", "Save", "Undo", "Redo",
    /*  5 */ "Cut", "Copy", "Paste", "Print", "Help",
    /* 10 */ "Find", "SaveAll", "SaveSesn", "NewSesn", "LoadSesn",
    /* 15 */ "RunScript", "Replace", "WinClose", "WinMax", "WinMin",
    /* 20 */ "WinSplit", "Shell", "FindPrev", "FindNext", "FindHelp",
    /* 25 */ "Make", "TagJump", "RunCtags", "WinVSplit", "WinMaxWidth",
    /* 30 */ "WinMinWidth", "Exit"
};
# define TOOLBAR_NAME_گنتی ARRAY_لمبائی(toolbar_نام)
#اگر_ختم

/*
 * Return TRUE if "name" is a window toolbar menu name.
 */
    ساکن صحیح
menu_is_winbar(char_u *name)
{
    واپس (STRNCMP(name, "WinBar", 6) == 0);
}

    صحیح
winbar_اونچائی(win_T *wp)
{
    اگر (wp->w_winbar != تہی && wp->w_winbar->children != تہی)
	واپس 1;
    واپس 0;
}

    ساکن vimmenu_T **
get_جڑ_menu(char_u *name)
{
    اگر (menu_is_winbar(name))
	واپس &curwin->w_winbar;
    واپس &جڑ_menu;
}

/*
 * If "menus_locked" is set then give an error and return TRUE.
 * Otherwise return FALSE.
 */
    ساکن صحیح
is_menus_locked(خالی)
{
    اگر (menus_locked > 0)
    {
	emsg(_(e_cannot_change_menus_while_listing));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Do the :menu command and relatives.
 */
    خالی
ex_menu(
    exarg_T	*eap)		    // Ex command arguments
{
    char_u	*menu_راستہ;
    صحیح		modes;
    char_u	*map_مقصد;
    صحیح		noremap;
    صحیح		silent = FALSE;
    صحیح		special = FALSE;
    صحیح		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    صحیح		i;
#اگر defined(FEAT_GUI) && !defined(FEAT_GUI_GTK)
    صحیح		old_menu_اونچائی;
# اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    صحیح		old_toolbar_اونچائی;
# endif
#اگر_ختم
    صحیح		pri_tab[MENUDEPTH + 1];
    صحیح		enable = MAYBE;	    // TRUE for "menu enable", FALSE for "menu
				    // disable
#اگر_تعریف_شدہ FEAT_TOOLBAR
    char_u	*icon = تہی;
#اگر_ختم
    vimmenu_T	menuarg;
    vimmenu_T	**جڑ_menu_ptr;

    modes = get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    برائے (;;)
    {
	اگر (STRNCMP(arg, "<script>", 8) == 0)
	{
	    noremap = REMAP_SCRIPT;
	    arg = skipwhite(arg + 8);
	    جاری;
	}
	اگر (STRNCMP(arg, "<silent>", 8) == 0)
	{
	    silent = TRUE;
	    arg = skipwhite(arg + 8);
	    جاری;
	}
	اگر (STRNCMP(arg, "<special>", 9) == 0)
	{
	    special = TRUE;
	    arg = skipwhite(arg + 9);
	    جاری;
	}
	توڑو;
    }


    // Locate an optional "icon=filename" argument.
    اگر (STRNCMP(arg, "icon=", 5) == 0)
    {
	arg += 5;
#اگر_تعریف_شدہ FEAT_TOOLBAR
	icon = arg;
#اگر_ختم
	جب (*arg != NUL && *arg != ' ')
	{
	    اگر (*arg == '\\')
		STRMOVE(arg, arg + 1);
	    MB_PTR_ADV(arg);
	}
	اگر (*arg != NUL)
	{
	    *arg++ = NUL;
	    arg = skipwhite(arg);
	}
    }

    /*
     * Fill in the priority table.
     */
    برائے (p = arg; *p; ++p)
	اگر (!VIM_ISDIGIT(*p) && *p != '.')
	    توڑو;
    اگر (VIM_ISWHITE(*p))
    {
	برائے (i = 0; i < MENUDEPTH && !VIM_ISWHITE(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    اگر (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    اگر (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    ورنہ اگر (eap->addr_گنتی && eap->line2 != 0)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    ورنہ
	i = 0;
    جب (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		// mark end of the table

    /*
     * Check for "disable" or "enable" argument.
     */
    اگر (STRNCMP(arg, "enable", 6) == 0 && VIM_ISWHITE(arg[6]))
    {
	enable = TRUE;
	arg = skipwhite(arg + 6);
    }
    ورنہ اگر (STRNCMP(arg, "disable", 7) == 0 && VIM_ISWHITE(arg[7]))
    {
	enable = FALSE;
	arg = skipwhite(arg + 7);
    }

    /*
     * If there is no argument, display all menus.
     */
    اگر (*arg == NUL)
    {
	show_menus(arg, modes);
	واپس;
    }

#اگر_تعریف_شدہ FEAT_TOOLBAR
    /*
     * Need to get the toolbar icon index before doing the translation.
     */
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
    اگر (menu_is_toolbar(arg))
    {
	menu_راستہ = menu_skip_part(arg);
	اگر (*menu_راستہ == '.')
	{
	    p = menu_skip_part(++menu_راستہ);
	    اگر (STRNCMP(menu_راستہ, "BuiltIn", 7) == 0)
	    {
		اگر (skipdigits(menu_راستہ + 7) == p)
		{
		    menuarg.iconidx = atoi((حرف *)menu_راستہ + 7);
		    اگر (menuarg.iconidx >= (صحیح)TOOLBAR_NAME_گنتی)
			menuarg.iconidx = -1;
		    ورنہ
			menuarg.icon_builtin = TRUE;
		}
	    }
	    ورنہ
	    {
		برائے (i = 0; i < (صحیح)TOOLBAR_NAME_گنتی; ++i)
		    اگر (STRNCMP(toolbar_نام[i], menu_راستہ, p - menu_راستہ)
									 == 0)
		    {
			menuarg.iconidx = i;
			توڑو;
		    }
	    }
	}
    }
#اگر_ختم

    menu_راستہ = arg;
    اگر (*menu_راستہ == '.')
    {
	semsg(_(e_invalid_دلیل_str), menu_راستہ);
	جاؤ theend;
    }

    map_مقصد = menu_translate_tab_and_سرکاؤ(arg);

    /*
     * If there is only a menu name, display menus with that name.
     */
    اگر (*map_مقصد == NUL && !unmenu && enable == MAYBE)
    {
	show_menus(menu_راستہ, modes);
	جاؤ theend;
    }
    ورنہ اگر (*map_مقصد != NUL && (unmenu || enable != MAYBE))
    {
	semsg(_(e_trailing_characters_str), map_مقصد);
	جاؤ theend;
    }
#اگر defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON))
    old_menu_اونچائی = gui.menu_اونچائی;
# اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    old_toolbar_اونچائی = gui.toolbar_اونچائی;
# endif
#اگر_ختم

    جڑ_menu_ptr = get_جڑ_menu(menu_راستہ);
    اگر (جڑ_menu_ptr == &curwin->w_winbar)
	// Assume the window toolbar menu will change.
	redraw_later(UPD_NOT_VALID);

    اگر (enable != MAYBE)
    {
	/*
	 * Change sensitivity of the menu.
	 * For the PopUp menu, remove a menu for each mode separately.
	 * Careful: menu_nable_recurse() changes menu_path.
	 */
	اگر (STRCMP(menu_راستہ, "*") == 0)	// meaning: do all menus
	    menu_راستہ = (char_u *)"";

	اگر (menu_is_popup(menu_راستہ))
	{
	    برائے (i = 0; i < MENU_اشاریہ_TIP; ++i)
		اگر (modes & (1 << i))
		{
		    p = popup_منوال_name(menu_راستہ, i);
		    اگر (p != تہی)
		    {
			menu_nable_recurse(*جڑ_menu_ptr, p, MENU_ALL_MODES,
								      enable);
			vim_free(p);
		    }
		}
	}
	menu_nable_recurse(*جڑ_menu_ptr, menu_راستہ, modes, enable);
    }
    ورنہ اگر (unmenu)
    {
	اگر (is_menus_locked())
	    جاؤ theend;

	/*
	 * Delete menu(s).
	 */
	اگر (STRCMP(menu_راستہ, "*") == 0)	// meaning: remove all menus
	    menu_راستہ = (char_u *)"";

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	اگر (menu_is_popup(menu_راستہ))
	{
	    برائے (i = 0; i < MENU_اشاریہ_TIP; ++i)
		اگر (modes & (1 << i))
		{
		    p = popup_منوال_name(menu_راستہ, i);
		    اگر (p != تہی)
		    {
			remove_menu(جڑ_menu_ptr, p, MENU_ALL_MODES, TRUE);
			vim_free(p);
		    }
		}
	}

	// Careful: remove_menu() changes menu_path
	remove_menu(جڑ_menu_ptr, menu_راستہ, modes, FALSE);
    }
    ورنہ
    {
	اگر (is_menus_locked())
	    جاؤ theend;

	/*
	 * Add menu(s).
	 * Replace special key codes.
	 */
	اگر (STRICMP(map_مقصد, "<nop>") == 0)	// "<Nop>" means nothing
	{
	    map_مقصد = (char_u *)"";
	    map_buf = تہی;
	}
	ورنہ اگر (modes & MENU_TIP_منوال)
	    map_buf = تہی;	// Menu tips are plain text.
	ورنہ
	    map_مقصد = replace_termcodes(map_مقصد, &map_buf, 0,
			REPTERM_DO_LT | (special ? REPTERM_SPECIAL : 0), تہی);
	menuarg.modes = modes;
#اگر_تعریف_شدہ FEAT_TOOLBAR
	menuarg.iconfile = icon;
#اگر_ختم
	menuarg.noremap[0] = noremap;
	menuarg.silent[0] = silent;
	جوڑو_menu_راستہ(menu_راستہ, &menuarg, pri_tab, map_مقصد
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
		, TRUE
#اگر_ختم
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	اگر (menu_is_popup(menu_راستہ))
	{
	    برائے (i = 0; i < MENU_اشاریہ_TIP; ++i)
		اگر (modes & (1 << i))
		{
		    p = popup_منوال_name(menu_راستہ, i);
		    اگر (p != تہی)
		    {
			// Include all modes, to make ":amenu" work
			menuarg.modes = modes;
#اگر_تعریف_شدہ FEAT_TOOLBAR
			menuarg.iconfile = تہی;
			menuarg.iconidx = -1;
			menuarg.icon_builtin = FALSE;
#اگر_ختم
			جوڑو_menu_راستہ(p, &menuarg, pri_tab, map_مقصد
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
				, TRUE
#اگر_ختم
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

#اگر defined(FEAT_GUI) && !(defined(FEAT_GUI_GTK))
    // If the menubar height changed, resize the window
    اگر (gui.اندر_use
	    && (gui.menu_اونچائی != old_menu_اونچائی
# اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
		|| gui.toolbar_اونچائی != old_toolbar_اونچائی
# endif
	    ))
	gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_VERT);
#اگر_ختم
    اگر (جڑ_menu_ptr == &curwin->w_winbar)
    {
	صحیح h = winbar_اونچائی(curwin);

	اگر (h != curwin->w_winbar_اونچائی)
	{
	    اگر (h == 0)
		++curwin->w_اونچائی;
	    ورنہ اگر (curwin->w_اونچائی > 0)
		--curwin->w_اونچائی;
	    curwin->w_winbar_اونچائی = h;
	}
	curwin->w_پچھلا_اونچائی = curwin->w_اونچائی;
    }

theend:
    ;
}

/*
 * Add the menu with the given name to the menu hierarchy
 */
    ساکن صحیح
جوڑو_menu_راستہ(
    char_u	*menu_راستہ,
    vimmenu_T	*menuarg,	// passes modes, iconfile, iconidx,
				// icon_builtin, silent[0], noremap[0]
    صحیح		*pri_tab,
    char_u	*call_ڈیٹا
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    , صحیح	addtearoff	// may add tearoff item
#اگر_ختم
    )
{
    char_u	*راستہ_name;
    صحیح		modes = menuarg->modes;
    vimmenu_T	**menup;
    vimmenu_T	*menu = تہی;
    vimmenu_T	*والد;
    vimmenu_T	**زیریں_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*اگلا_name;
    صحیح		i;
    صحیح		c;
    صحیح		d;
#اگر_تعریف_شدہ FEAT_GUI
    صحیح		idx;
    صحیح		new_idx;
#اگر_ختم
    صحیح		pri_idx = 0;
    صحیح		old_modes = 0;
    صحیح		amenu;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    char_u	*en_name;
    char_u	*map_مقصد = تہی;
#اگر_ختم
    vimmenu_T	**جڑ_menu_ptr;

    // Make a copy so we can stuff around with it, since it could be const
    راستہ_name = vim_strsave(menu_راستہ);
    اگر (راستہ_name == تہی)
	واپس FAIL;
    جڑ_menu_ptr = get_جڑ_menu(menu_راستہ);
    menup = جڑ_menu_ptr;
    والد = تہی;
    name = راستہ_name;
    جب (*name)
    {
	// Get name of this element in the menu hierarchy, and the simplified
	// name (without mnemonic and accelerator text).
	اگلا_name = menu_name_skip(name);
#اگر_تعریف_شدہ	FEAT_MULTI_LANG
	map_مقصد = menutrans_lookup(name, (صحیح)STRLEN(name));
	اگر (map_مقصد != تہی)
	{
	    en_name = name;
	    name = map_مقصد;
	}
	ورنہ
	    en_name = تہی;
#اگر_ختم
	dname = menu_متن(name, تہی, تہی);
	اگر (dname == تہی)
	    جاؤ erret;
	اگر (*dname == NUL)
	{
	    // Only a mnemonic or accelerator is not valid.
	    emsg(_(e_خالی_خانے_menu_name));
	    جاؤ erret;
	}

	// See if it's already there
	زیریں_pri = menup;
#اگر_تعریف_شدہ FEAT_GUI
	idx = 0;
	new_idx = 0;
#اگر_ختم
	menu = *menup;
	جب (menu != تہی)
	{
	    اگر (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		اگر (*اگلا_name == NUL && menu->children != تہی)
		{
		    اگر (!sys_menu)
			emsg(_(e_menu_راستہ_must_not_lead_مقصد_sub_menu));
		    جاؤ erret;
		}
		اگر (*اگلا_name != NUL && menu->children == تہی
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
			&& addtearoff
#اگر_ختم
			)
		{
		    اگر (!sys_menu)
			emsg(_(e_part_کا_menu_شے_راستہ_is_not_sub_menu));
		    جاؤ erret;
		}
		توڑو;
	    }
	    menup = &menu->اگلا;

	    // Count menus, to find where this one needs to be inserted.
	    // Ignore menus that are not in the menubar (PopUp and Toolbar)
	    اگر (والد != تہی || menu_is_menubar(menu->name))
	    {
#اگر_تعریف_شدہ FEAT_GUI
		++idx;
#اگر_ختم
		اگر (menu->ترجیح <= pri_tab[pri_idx])
		{
		    زیریں_pri = menup;
#اگر_تعریف_شدہ FEAT_GUI
		    new_idx = idx;
#اگر_ختم
		}
	    }
	    menu = menu->اگلا;
	}

	اگر (menu == تہی)
	{
	    اگر (*اگلا_name == NUL && والد == تہی)
	    {
		emsg(_(e_must_not_جوڑو_menu_اشیاء_directly_مقصد_menu_bar));
		جاؤ erret;
	    }

	    اگر (menu_is_separator(dname) && *اگلا_name != NUL)
	    {
		emsg(_(e_separator_cannot_be_part_کا_menu_راستہ));
		جاؤ erret;
	    }

	    // Not already there, so let's add it
	    menu = ALLOC_CLEAR_ONE(vimmenu_T);
	    اگر (menu == تہی)
		جاؤ erret;

	    menu->modes = modes;
	    menu->enabled = MENU_ALL_MODES;
	    menu->name = vim_strsave(name);
	    // separate mnemonic and accelerator text from actual menu name
	    menu->dname = menu_متن(name, &menu->mnemonic, &menu->actext);
#اگر_تعریف_شدہ	FEAT_MULTI_LANG
	    اگر (en_name != تہی)
	    {
		menu->en_name = vim_strsave(en_name);
		menu->en_dname = menu_متن(en_name, تہی, تہی);
	    }
	    ورنہ
	    {
		menu->en_name = تہی;
		menu->en_dname = تہی;
	    }
#اگر_ختم
	    menu->ترجیح = pri_tab[pri_idx];
	    menu->والد = والد;
#اگر_تعریف_شدہ FEAT_GUI_MOTIF
	    menu->sensitive = TRUE;	    // the default
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BEVAL_TIP
	    menu->tip = تہی;
#اگر_ختم
	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->اگلا = *زیریں_pri;
	    *زیریں_pri = menu;

	    old_modes = 0;

#اگر_تعریف_شدہ FEAT_TOOLBAR
	    menu->iconidx = menuarg->iconidx;
	    menu->icon_builtin = menuarg->icon_builtin;
	    اگر (*اگلا_name == NUL && menuarg->iconfile != تہی)
		menu->iconfile = vim_strsave(menuarg->iconfile);
#اگر_ختم
#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // the tearoff item must be present in the modes of each item.
	    اگر (والد != تہی && menu_is_tearoff(والد->children->dname))
		والد->children->modes |= modes;
#اگر_ختم
	}
	ورنہ
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     * Also enable a menu when it's created or changed.
	     */
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
	    // If adding a tearbar (addtearoff == FALSE) don't update modes
	    اگر (addtearoff)
#اگر_ختم
	    {
		menu->modes |= modes;
		menu->enabled |= modes;
	    }
	}

#اگر_تعریف_شدہ FEAT_GUI
	/*
	 * Add the menu item when it's used in one of the modes, but not when
	 * only a tooltip is defined.
	 */
	اگر ((old_modes & MENU_ALL_MODES) == 0
		&& (menu->modes & MENU_ALL_MODES) != 0)
	{
	    اگر (gui.اندر_use)  // Otherwise it will be added when GUI starts
	    {
		اگر (*اگلا_name == NUL)
		{
		    // Real menu item, not sub-menu
		    gui_mch_جوڑو_menu_شے(menu, new_idx);

		    // Want to update menus now even if mode not changed
		    force_menu_update = TRUE;
		}
		ورنہ
		{
		    // Sub-menu (not at end of path yet)
		    gui_mch_جوڑو_menu(menu, new_idx);
		}
	    }

# اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
	    // When adding a new submenu, may add a tearoff item
	    اگر (	addtearoff
		    && *اگلا_name
		    && vim_strchr(p_go, GO_TEAROFF) != تہی
		    && menu_is_menubar(name)
#  ifdef VIMDLL
		    && (gui.اندر_use || gui.starting)
#  endif
		    )
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_راستہ) + TEAR_LEN + 2);
		اگر (tearpath != تہی)
		{
		    char_u  *s;
		    صحیح	    len;

		    STRCPY(tearpath, menu_راستہ);
		    len = (صحیح)(اگلا_name - راستہ_name - 1);
		    برائے (s = tearpath; *s && s < tearpath + len; MB_PTR_ADV(s))
		    {
			اگر ((*s == '\\' || *s == Ctrl_V) && s[1])
			{
			    ++len;
			    ++s;
			}
		    }
		    tearpath[len] = NUL;
		    gui_جوڑو_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
# endif
	}
#اگر_ختم // FEAT_GUI

	menup = &menu->children;
	والد = menu;
	name = اگلا_name;
	VIM_CLEAR(dname);
	اگر (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(راستہ_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_معمول_منوال | MENU_داخل_کرو_منوال)) ==
				       (MENU_معمول_منوال | MENU_داخل_کرو_منوال));
    اگر (sys_menu)
	modes &= ~old_modes;

    اگر (menu != تہی && modes)
    {
#اگر_تعریف_شدہ FEAT_GUI
	menu->cb = gui_menu_cb;
#اگر_ختم
	p = (call_ڈیٹا == تہی) ? تہی : vim_strsave(call_ڈیٹا);

	// loop over all modes, may add more than one
	برائے (i = 0; i < MENU_MODES; ++i)
	{
	    اگر (modes & (1 << i))
	    {
		// free any old menu
		free_menu_string(menu, i);

		// For "amenu", may insert an extra character.
		// Don't do this if adding a tearbar (addtearoff == FALSE).
		// Don't do this for "<Nop>".
		c = 0;
		d = 0;
		اگر (amenu && call_ڈیٹا != تہی && *call_ڈیٹا != NUL
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
		       && addtearoff
#اگر_ختم
				       )
		{
		    انتخاب (1 << i)
		    {
			حالت MENU_VISUAL_منوال:
			حالت MENU_SELECT_منوال:
			حالت MENU_OP_PENDING_منوال:
			حالت MENU_CMDLINE_منوال:
			    c = Ctrl_C;
			    توڑو;
			حالت MENU_داخل_کرو_منوال:
			    c = Ctrl_BSL;
			    d = Ctrl_O;
			    توڑو;
		    }
		}

		اگر (c != 0)
		{
		    menu->لڑیاں[i] = alloc(STRLEN(call_ڈیٹا) + 5);
		    اگر (menu->لڑیاں[i] != تہی)
		    {
			menu->لڑیاں[i][0] = c;
			اگر (d == 0)
			    STRCPY(menu->لڑیاں[i] + 1, call_ڈیٹا);
			ورنہ
			{
			    menu->لڑیاں[i][1] = d;
			    STRCPY(menu->لڑیاں[i] + 2, call_ڈیٹا);
			}
			اگر (c == Ctrl_C)
			{
			    صحیح	    len = (صحیح)STRLEN(menu->لڑیاں[i]);

			    // Append CTRL-\ CTRL-G to obey 'insertmode'.
			    menu->لڑیاں[i][len] = Ctrl_BSL;
			    menu->لڑیاں[i][len + 1] = Ctrl_G;
			    menu->لڑیاں[i][len + 2] = NUL;
			}
		    }
		}
		ورنہ
		    menu->لڑیاں[i] = p;
		menu->noremap[i] = menuarg->noremap[0];
		menu->silent[i] = menuarg->silent[0];
	    }
	}
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	&& (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	// Need to update the menu tip.
	اگر (modes & MENU_TIP_منوال)
	    gui_mch_menu_سیٹ_tip(menu);
#اگر_ختم
    }
    واپس OK;

erret:
    vim_free(راستہ_name);
    vim_free(dname);

    // Delete any empty submenu we added before discovering the error.  Repeat
    // for higher levels.
    جب (والد != تہی && والد->children == تہی)
    {
	اگر (والد->والد == تہی)
	    menup = جڑ_menu_ptr;
	ورنہ
	    menup = &والد->والد->children;
	برائے ( ; *menup != تہی && *menup != والد; menup = &((*menup)->اگلا))
	    ;
	اگر (*menup == تہی) // safety check
	    توڑو;
	والد = والد->والد;
	free_menu(menup);
    }
    واپس FAIL;
}

/*
 * Set the (sub)menu with the given name to enabled or disabled.
 * Called recursively.
 */
    ساکن صحیح
menu_nable_recurse(
    vimmenu_T	*menu,
    char_u	*name,
    صحیح		modes,
    صحیح		enable)
{
    char_u	*p;

    اگر (menu == تہی)
	واپس OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    جب (menu != تہی)
    {
	اگر (*name == NUL || *name == '*' || menu_name_equal(name, menu))
	{
	    اگر (*p != NUL)
	    {
		اگر (menu->children == تہی)
		{
		    emsg(_(e_part_کا_menu_شے_راستہ_is_not_sub_menu));
		    واپس FAIL;
		}
		اگر (menu_nable_recurse(menu->children, p, modes, enable)
								      == FAIL)
		    واپس FAIL;
	    }
	    ورنہ
		اگر (enable)
		    menu->enabled |= modes;
		ورنہ
		    menu->enabled &= ~modes;

	    /*
	     * When name is empty, we are doing all menu items for the given
	     * modes, so keep looping, otherwise we are just doing the named
	     * menu item (which has been found) so break here.
	     */
	    اگر (*name != NUL && *name != '*')
		توڑو;
	}
	menu = menu->اگلا;
    }
    اگر (*name != NUL && *name != '*' && menu == تہی)
    {
	semsg(_(e_no_menu_str), name);
	واپس FAIL;
    }

#اگر_تعریف_شدہ FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#اگر_ختم

    واپس OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    ساکن صحیح
remove_menu(
    vimmenu_T	**menup,
    char_u	*name,
    صحیح		modes,
    صحیح		silent)		// don't give error messages
{
    vimmenu_T	*menu;
    vimmenu_T	*بچہ;
    char_u	*p;

    اگر (*menup == تہی)
	واپس OK;		// Got to bottom of hierarchy

    // Get name of this element in the menu hierarchy
    p = menu_name_skip(name);

    // Find the menu
    جب ((menu = *menup) != تہی)
    {
	اگر (*name == NUL || menu_name_equal(name, menu))
	{
	    اگر (*p != NUL && menu->children == تہی)
	    {
		اگر (!silent)
		    emsg(_(e_part_کا_menu_شے_راستہ_is_not_sub_menu));
		واپس FAIL;
	    }
	    اگر ((menu->modes & modes) != 0x0)
	    {
#اگر defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
		/*
		 * If we are removing all entries for this menu,MENU_ALL_MODES,
		 * Then kill any tearoff before we start
		 */
		اگر (*p == NUL && modes == MENU_ALL_MODES)
		{
		    اگر (Is_کھڑکی(menu->tearoff_handle))
			Destroy_کھڑکی(menu->tearoff_handle);
		}
#اگر_ختم
		اگر (remove_menu(&menu->children, p, modes, silent) == FAIL)
		    واپس FAIL;
	    }
	    ورنہ اگر (*name != NUL)
	    {
		اگر (!silent)
		    emsg(_(e_menu_only_exists_اندر_another_منوال));
		واپس FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    اگر (*name != NUL)
		توڑو;

	    // Remove the menu item for the given mode[s].  If the menu item
	    // is no longer valid in ANY mode, delete it
	    menu->modes &= ~modes;
	    اگر (modes & MENU_TIP_منوال)
		free_menu_string(menu, MENU_اشاریہ_TIP);
	    اگر ((menu->modes & MENU_ALL_MODES) == 0)
		free_menu(menup);
	    ورنہ
		menup = &menu->اگلا;
	}
	ورنہ
	    menup = &menu->اگلا;
    }
    اگر (*name != NUL)
    {
	اگر (menu == تہی)
	{
	    اگر (!silent)
		semsg(_(e_no_menu_str), name);
	    واپس FAIL;
	}


	// Recalculate modes for menu based on the new updated children
	menu->modes &= ~modes;
#اگر defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	اگر ((s_tearoffs) && (menu->children != تہی)) // there's a tear bar.
	    بچہ = menu->children->اگلا; // don't count tearoff bar
	ورنہ
#اگر_ختم
	    بچہ = menu->children;
	برائے ( ; بچہ != تہی; بچہ = بچہ->اگلا)
	    menu->modes |= بچہ->modes;
	اگر (modes & MENU_TIP_منوال)
	{
	    free_menu_string(menu, MENU_اشاریہ_TIP);
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN) \
	    && (defined(FEAT_BEVAL_GUI) || defined(FEAT_GUI_GTK))
	    // Need to update the menu tip.
	    اگر (gui.اندر_use)
		gui_mch_menu_سیٹ_tip(menu);
#اگر_ختم
	}
	اگر ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    // The menu item is no longer valid in ANY mode, so delete it
#اگر defined(FEAT_GUI_MSWIN) & defined(FEAT_TEAROFF)
	    اگر (s_tearoffs && menu->children != تہی) // there's a tear bar.
		free_menu(&menu->children);
#اگر_ختم
	    *menup = menu;
	    free_menu(menup);
	}
    }

    واپس OK;
}

/*
 * Remove the WinBar menu from window "wp".
 */
    خالی
remove_winbar(win_T *wp)
{
    remove_menu(&wp->w_winbar, (char_u *)"", MENU_ALL_MODES, TRUE);
    vim_free(wp->w_winbar_اشیاء);
}

/*
 * Free the given menu structure and remove it from the linked list.
 */
    ساکن خالی
free_menu(vimmenu_T **menup)
{
    صحیح		i;
    vimmenu_T	*menu;

    menu = *menup;

#اگر_تعریف_شدہ FEAT_GUI
    // Free machine specific menu structures (only when already created)
    // Also may rebuild a tearoff'ed menu
    اگر (gui.اندر_use)
	gui_mch_destroy_menu(menu);
#اگر_ختم

    // Don't change *menup until after calling gui_mch_destroy_menu(). The
    // MacOS code needs the original structure to properly delete the menu.
    *menup = menu->اگلا;
    vim_free(menu->name);
    vim_free(menu->dname);
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    vim_free(menu->en_name);
    vim_free(menu->en_dname);
#اگر_ختم
    vim_free(menu->actext);
#اگر_تعریف_شدہ FEAT_TOOLBAR
    vim_free(menu->iconfile);
# ifdef FEAT_GUI_MOTIF
    vim_free(menu->xpm_fname);
# endif
#اگر_ختم
    برائے (i = 0; i < MENU_MODES; i++)
	free_menu_string(menu, i);
    vim_free(menu);

#اگر_تعریف_شدہ FEAT_GUI
    // Want to update menus now even if mode not changed
    force_menu_update = TRUE;
#اگر_ختم
}

/*
 * Free the menu->string with the given index.
 */
    ساکن خالی
free_menu_string(vimmenu_T *menu, صحیح idx)
{
    صحیح		گنتی = 0;
    صحیح		i;

    برائے (i = 0; i < MENU_MODES; i++)
	اگر (menu->لڑیاں[i] == menu->لڑیاں[idx])
	    گنتی++;
    اگر (گنتی == 1)
	vim_free(menu->لڑیاں[idx]);
    menu->لڑیاں[idx] = تہی;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    ساکن صحیح
show_menus(char_u *راستہ_name, صحیح modes)
{
    char_u	*p;
    char_u	*name;
    vimmenu_T	*menu;
    vimmenu_T	*والد = تہی;

    name = راستہ_name = vim_strsave(راستہ_name);
    اگر (راستہ_name == تہی)
	واپس FAIL;
    menu = *get_جڑ_menu(راستہ_name);

    // First, find the (sub)menu with the given name
    جب (*name)
    {
	p = menu_name_skip(name);
	جب (menu != تہی)
	{
	    اگر (menu_name_equal(name, menu))
	    {
		// Found menu
		اگر (*p != NUL && menu->children == تہی)
		{
		    emsg(_(e_part_کا_menu_شے_راستہ_is_not_sub_menu));
		    vim_free(راستہ_name);
		    واپس FAIL;
		}
		ورنہ اگر ((menu->modes & modes) == 0x0)
		{
		    emsg(_(e_menu_only_exists_اندر_another_منوال));
		    vim_free(راستہ_name);
		    واپس FAIL;
		}
		توڑو;
	    }
	    menu = menu->اگلا;
	}
	اگر (menu == تہی)
	{
	    semsg(_(e_no_menu_str), name);
	    vim_free(راستہ_name);
	    واپس FAIL;
	}
	name = p;
	والد = menu;
	menu = menu->children;
    }
    vim_free(راستہ_name);

    // make sure the list of menus doesn't change while listing them
    ++menus_locked;

    // list the matching menu mappings
    پیغام_puts_title(_("\n--- Menus ---"));
    show_menus_recursive(والد, modes, 0);

    --menus_locked;
    واپس OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    ساکن خالی
show_menus_recursive(vimmenu_T *menu, صحیح modes, صحیح گہرائی)
{
    صحیح		i;
    صحیح		ثنائی_ہندسہ;

    اگر (menu != تہی && (menu->modes & modes) == 0x0)
	واپس;

    اگر (menu != تہی)
    {
	پیغام_putchar('\n');
	اگر (got_int)		// "q" hit for "--more--"
	    واپس;
	برائے (i = 0; i < گہرائی; i++)
	    پیغام_puts("  ");
	اگر (menu->ترجیح)
	{
	    پیغام_outnum((طویل)menu->ترجیح);
	    پیغام_puts(" ");
	}
				// Same highlighting as for directories!?
	پیغام_outtrans_attr(menu->name, HL_ATTR(HLF_D));
    }

    اگر (menu != تہی && menu->children == تہی)
    {
	برائے (ثنائی_ہندسہ = 0; ثنائی_ہندسہ < MENU_MODES; ثنائی_ہندسہ++)
	    اگر ((menu->modes & modes & (1 << ثنائی_ہندسہ)) != 0)
	    {
		پیغام_putchar('\n');
		اگر (got_int)		// "q" hit for "--more--"
		    واپس;
		برائے (i = 0; i < گہرائی + 2; i++)
		    پیغام_puts("  ");
		پیغام_puts(menu_منوال_chars[ثنائی_ہندسہ]);
		اگر (menu->noremap[ثنائی_ہندسہ] == REMAP_NONE)
		    پیغام_putchar('*');
		ورنہ اگر (menu->noremap[ثنائی_ہندسہ] == REMAP_SCRIPT)
		    پیغام_putchar('&');
		ورنہ
		    پیغام_putchar(' ');
		اگر (menu->silent[ثنائی_ہندسہ])
		    پیغام_putchar('s');
		ورنہ
		    پیغام_putchar(' ');
		اگر ((menu->modes & menu->enabled & (1 << ثنائی_ہندسہ)) == 0)
		    پیغام_putchar('-');
		ورنہ
		    پیغام_putchar(' ');
		پیغام_puts(" ");
		اگر (*menu->لڑیاں[ثنائی_ہندسہ] == NUL)
		    پیغام_puts_attr("<Nop>", HL_ATTR(HLF_8));
		ورنہ
		    پیغام_outtrans_special(menu->لڑیاں[ثنائی_ہندسہ], FALSE, 0);
	    }
    }
    ورنہ
    {
	اگر (menu == تہی)
	{
	    menu = جڑ_menu;
	    گہرائی--;
	}
	ورنہ
	    menu = menu->children;

	// recursively show all children.  Skip PopUp[nvoci].
	برائے (; menu != تہی && !got_int; menu = menu->اگلا)
	    اگر (!menu_is_hidden(menu->dname))
		show_menus_recursive(menu, modes, گہرائی + 1);
    }
}

/*
 * Used when expanding menu names.
 */
ساکن vimmenu_T	*expand_menu = تہی;
ساکن vimmenu_T	*expand_menu_alt = تہی;
ساکن صحیح		expand_modes = 0x0;
ساکن صحیح		expand_emenu;	// TRUE for ":emenu" command

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
سیٹ_context_اندر_menu_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    صحیح		forceit)
{
    char_u	*after_dot;
    char_u	*p;
    char_u	*راستہ_name = تہی;
    char_u	*name;
    صحیح		unmenu;
    vimmenu_T	*menu;
    صحیح		expand_menus;

    xp->xp_context = EXPAND_UNSUCCESSFUL;


    // Check for priority numbers, enable and disable
    برائے (p = arg; *p; ++p)
	اگر (!VIM_ISDIGIT(*p) && *p != '.')
	    توڑو;

    اگر (!VIM_ISWHITE(*p))
    {
	اگر (STRNCMP(arg, "enable", 6) == 0
		&& (arg[6] == NUL ||  VIM_ISWHITE(arg[6])))
	    p = arg + 6;
	ورنہ اگر (STRNCMP(arg, "disable", 7) == 0
		&& (arg[7] == NUL || VIM_ISWHITE(arg[7])))
	    p = arg + 7;
	ورنہ
	    p = arg;
    }

    جب (*p != NUL && VIM_ISWHITE(*p))
	++p;

    arg = after_dot = p;

    برائے (; *p && !VIM_ISWHITE(*p); ++p)
    {
	اگر ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    p++;
	ورنہ اگر (*p == '.')
	    after_dot = p + 1;
    }

    // ":tearoff" and ":popup" only use menus, not entries
    expand_menus = !((*cmd == 't' && cmd[1] == 'e') || *cmd == 'p');
    expand_emenu = (*cmd == 'e');
    اگر (expand_menus && VIM_ISWHITE(*p))
	واپس تہی;	// TODO: check for next command?
    اگر (*p == NUL)		// Complete the menu name
    {
	صحیح try_alt_menu = TRUE;

	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus from other modes too.
	 */
	expand_modes = get_menu_cmd_modes(cmd, forceit, تہی, &unmenu);
	اگر (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = جڑ_menu;
	اگر (after_dot != arg)
	{
	    راستہ_name = alloc(after_dot - arg);
	    اگر (راستہ_name == تہی)
		واپس تہی;
	    vim_strncpy(راستہ_name, arg, after_dot - arg - 1);
	}
	name = راستہ_name;
	جب (name != تہی && *name)
	{
	    p = menu_name_skip(name);
	    جب (menu != تہی)
	    {
		اگر (menu_name_equal(name, menu))
		{
		    // Found menu
		    اگر ((*p != NUL && menu->children == تہی)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(راستہ_name);
			واپس تہی;
		    }
		    توڑو;
		}
		menu = menu->اگلا;
		اگر (menu == تہی && try_alt_menu)
		{
		    menu = curwin->w_winbar;
		    try_alt_menu = FALSE;
		}
	    }
	    اگر (menu == تہی)
	    {
		// No menu found with the name we were looking for
		vim_free(راستہ_name);
		واپس تہی;
	    }
	    name = p;
	    menu = menu->children;
	    try_alt_menu = FALSE;
	}
	vim_free(راستہ_name);

	xp->xp_context = expand_menus ? EXPAND_MENUNAMES : EXPAND_MENUS;
	xp->xp_نمونہ = after_dot;
	expand_menu = menu;
	اگر (expand_menu == جڑ_menu)
	    expand_menu_alt = curwin->w_winbar;
	ورنہ
	    expand_menu_alt = تہی;
    }
    ورنہ			// We're in the mapping part
	xp->xp_context = EXPAND_NOTHING;
    واپس تہی;
}

/*
 * Function given to ExpandGeneric() to obtain the list of (sub)menus (not
 * entries).
 */
    char_u *
get_menu_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    ساکن vimmenu_T	*menu = تہی;
    ساکن صحیح		did_alt_menu = FALSE;
    char_u		*str;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    ساکن  صحیح		should_advance = FALSE;
#اگر_ختم

    اگر (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	should_advance = FALSE;
#اگر_ختم
    }

    // Skip PopUp[nvoci].
    جب (menu != تہی && (menu_is_hidden(menu->dname)
	    || menu_is_separator(menu->dname)
	    || menu_is_tearoff(menu->dname)
	    || menu->children == تہی))
    {
	menu = menu->اگلا;
	اگر (menu == تہی && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    اگر (menu == تہی)	    // at end of linked list
	واپس تہی;

    اگر (menu->modes & expand_modes)
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	اگر (should_advance)
	    str = menu->en_dname;
	ورنہ
	{
#اگر_ختم
	    str = menu->dname;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	    اگر (menu->en_dname == تہی)
		should_advance = TRUE;
	}
#اگر_ختم
    ورنہ
	str = (char_u *)"";

#اگر_تعریف_شدہ FEAT_MULTI_LANG
    اگر (should_advance)
#اگر_ختم
    {
	// Advance to next menu entry.
	menu = menu->اگلا;
	اگر (menu == تہی && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#اگر_تعریف_شدہ FEAT_MULTI_LANG
    should_advance = !should_advance;
#اگر_ختم

    واپس str;
}

/*
 * Function given to ExpandGeneric() to obtain the list of menus and menu
 * entries.
 */
    char_u *
get_menu_نام(expand_T *xp غیر_مستعمل, صحیح idx)
{
    ساکن vimmenu_T	*menu = تہی;
    ساکن صحیح		did_alt_menu = FALSE;
#تعریف_کرو TBUFFER_LEN 256
    ساکن char_u	tbuffer[TBUFFER_LEN]; //hack
    char_u		*str;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    ساکن  صحیح		should_advance = FALSE;
#اگر_ختم

    اگر (idx == 0)	    // first call: start at first item
    {
	menu = expand_menu;
	did_alt_menu = FALSE;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	should_advance = FALSE;
#اگر_ختم
    }

    // Skip Browse-style entries, popup menus and separators.
    جب (menu != تہی
	    && (   menu_is_hidden(menu->dname)
		|| (expand_emenu && menu_is_separator(menu->dname))
		|| menu_is_tearoff(menu->dname)
#اگر_غیر_تعریف_شدہ FEAT_BROWSE
		|| menu->dname[STRLEN(menu->dname) - 1] == '.'
#اگر_ختم
	       ))
    {
	menu = menu->اگلا;
	اگر (menu == تہی && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

    اگر (menu == تہی)	    // at end of linked list
	واپس تہی;

    اگر (menu->modes & expand_modes)
    {
	اگر (menu->children != تہی)
	{
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	    اگر (should_advance)
		vim_strncpy(tbuffer, menu->en_dname, TBUFFER_LEN - 2);
	    ورنہ
	    {
#اگر_ختم
		vim_strncpy(tbuffer, menu->dname,  TBUFFER_LEN - 2);
#اگر_تعریف_شدہ FEAT_MULTI_LANG
		اگر (menu->en_dname == تہی)
		    should_advance = TRUE;
	    }
#اگر_ختم
	    // hack on menu separators:  use a 'magic' char for the separator
	    // so that '.' in names gets escaped properly
	    STRCAT(tbuffer, "\001");
	    str = tbuffer;
	}
	ورنہ
#اگر_تعریف_شدہ FEAT_MULTI_LANG
	{
	    اگر (should_advance)
		str = menu->en_dname;
	    ورنہ
	    {
#اگر_ختم
		str = menu->dname;
#اگر_تعریف_شدہ FEAT_MULTI_LANG
		اگر (menu->en_dname == تہی)
		    should_advance = TRUE;
	    }
	}
#اگر_ختم
    }
    ورنہ
	str = (char_u *)"";

#اگر_تعریف_شدہ FEAT_MULTI_LANG
    اگر (should_advance)
#اگر_ختم
    {
	// Advance to next menu entry.
	menu = menu->اگلا;
	اگر (menu == تہی && !did_alt_menu)
	{
	    menu = expand_menu_alt;
	    did_alt_menu = TRUE;
	}
    }

#اگر_تعریف_شدہ FEAT_MULTI_LANG
    should_advance = !should_advance;
#اگر_ختم

    واپس str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 * "name" may be modified.
 */
    ساکن char_u *
menu_name_skip(char_u *name)
{
    char_u  *p;

    برائے (p = name; *p && *p != '.'; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' || *p == Ctrl_V)
	{
	    STRMOVE(p, p + 1);
	    اگر (*p == NUL)
		توڑو;
	}
    }
    اگر (*p)
	*p++ = NUL;
    واپس p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    ساکن صحیح
menu_name_equal(char_u *name, vimmenu_T *menu)
{
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    اگر (menu->en_name != تہی
	    && (menu_namecmp(name, menu->en_name)
		|| menu_namecmp(name, menu->en_dname)))
	واپس TRUE;
#اگر_ختم
    واپس menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname);
}

    ساکن صحیح
menu_namecmp(char_u *name, char_u *mname)
{
    صحیح		i;

    برائے (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	اگر (name[i] != mname[i])
	    توڑو;
    واپس ((name[i] == NUL || name[i] == TAB)
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
    ساکن صحیح
get_menu_cmd_modes(
    char_u  *cmd,
    صحیح	    forceit,	    // Was there a "!" after the command?
    صحیح	    *noremap,
    صحیح	    *unmenu)
{
    صحیح	    modes;

    انتخاب (*cmd++)
    {
	حالت 'v':			// vmenu, vunmenu, vnoremenu
	    modes = MENU_VISUAL_منوال | MENU_SELECT_منوال;
	    توڑو;
	حالت 'x':			// xmenu, xunmenu, xnoremenu
	    modes = MENU_VISUAL_منوال;
	    توڑو;
	حالت 's':			// smenu, sunmenu, snoremenu
	    modes = MENU_SELECT_منوال;
	    توڑو;
	حالت 'o':			// omenu
	    modes = MENU_OP_PENDING_منوال;
	    توڑو;
	حالت 'i':			// imenu
	    modes = MENU_داخل_کرو_منوال;
	    توڑو;
	حالت 't':
	    اگر (*cmd == 'l')		// tlmenu, tlunmenu, tlnoremenu
	    {
		modes = MENU_TERMINAL_منوال;
		++cmd;
		توڑو;
	    }
	    modes = MENU_TIP_منوال;	// tmenu
	    توڑو;
	حالت 'c':			// cmenu
	    modes = MENU_CMDLINE_منوال;
	    توڑو;
	حالت 'a':			// amenu
	    modes = MENU_داخل_کرو_منوال | MENU_CMDLINE_منوال | MENU_معمول_منوال
				    | MENU_VISUAL_منوال | MENU_SELECT_منوال
				    | MENU_OP_PENDING_منوال;
	    توڑو;
	حالت 'n':
	    اگر (*cmd != 'o')		// nmenu, not noremenu
	    {
		modes = MENU_معمول_منوال;
		توڑو;
	    }
	    // FALLTHROUGH
	طےشدہ:
	    --cmd;
	    اگر (forceit)		// menu!!
		modes = MENU_داخل_کرو_منوال | MENU_CMDLINE_منوال;
	    ورنہ			// menu
		modes = MENU_معمول_منوال | MENU_VISUAL_منوال | MENU_SELECT_منوال
						       | MENU_OP_PENDING_منوال;
    }

    اگر (noremap != تہی)
	*noremap = (*cmd == 'n' ? REMAP_NONE : REMAP_YES);
    اگر (unmenu != تہی)
	*unmenu = (*cmd == 'u');
    واپس modes;
}

/*
 * Return the string representation of the menu modes. Does the opposite
 * of get_menu_cmd_modes().
 */
    ساکن char_u *
get_menu_منوال_str(صحیح modes)
{
    اگر ((modes & (MENU_داخل_کرو_منوال | MENU_CMDLINE_منوال | MENU_معمول_منوال |
		  MENU_VISUAL_منوال | MENU_SELECT_منوال | MENU_OP_PENDING_منوال))
	    == (MENU_داخل_کرو_منوال | MENU_CMDLINE_منوال | MENU_معمول_منوال |
		MENU_VISUAL_منوال | MENU_SELECT_منوال | MENU_OP_PENDING_منوال))
	واپس (char_u *)"a";
    اگر ((modes & (MENU_معمول_منوال | MENU_VISUAL_منوال | MENU_SELECT_منوال |
		  MENU_OP_PENDING_منوال))
	    == (MENU_معمول_منوال | MENU_VISUAL_منوال | MENU_SELECT_منوال |
		MENU_OP_PENDING_منوال))
	واپس (char_u *)" ";
    اگر ((modes & (MENU_داخل_کرو_منوال | MENU_CMDLINE_منوال))
	    == (MENU_داخل_کرو_منوال | MENU_CMDLINE_منوال))
	واپس (char_u *)"!";
    اگر ((modes & (MENU_VISUAL_منوال | MENU_SELECT_منوال))
	    == (MENU_VISUAL_منوال | MENU_SELECT_منوال))
	واپس (char_u *)"v";
    اگر (modes & MENU_VISUAL_منوال)
	واپس (char_u *)"x";
    اگر (modes & MENU_SELECT_منوال)
	واپس (char_u *)"s";
    اگر (modes & MENU_OP_PENDING_منوال)
	واپس (char_u *)"o";
    اگر (modes & MENU_داخل_کرو_منوال)
	واپس (char_u *)"i";
    اگر (modes & MENU_TERMINAL_منوال)
	واپس (char_u *)"tl";
    اگر (modes & MENU_CMDLINE_منوال)
	واپس (char_u *)"c";
    اگر (modes & MENU_معمول_منوال)
	واپس (char_u *)"n";
    اگر (modes & MENU_TIP_منوال)
	واپس (char_u *)"t";

    واپس (char_u *)"";
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    ساکن char_u *
popup_منوال_name(char_u *name, صحیح idx)
{
    char_u	*p;
    صحیح		len = (صحیح)STRLEN(name);
    حرف	*منوال_chars = menu_منوال_chars[idx];
    صحیح		منوال_chars_len = (صحیح)لڑی_لمبائی(منوال_chars);
    صحیح		i;

    p = vim_strnsave(name, len + منوال_chars_len);
    اگر (p == تہی)
	واپس تہی;

    mch_memmove(p + 5 + منوال_chars_len, p + 5, (size_t)(len - 4));
    برائے (i = 0; i < منوال_chars_len; ++i)
	p[5 + i] = menu_منوال_chars[idx][i];
    واپس p;
}

#اگر defined(FEAT_GUI)
/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    صحیح
get_menu_اشاریہ(vimmenu_T *menu, صحیح state)
{
    صحیح		idx;

    اگر ((state & منوال_داخل_کرو))
	idx = MENU_اشاریہ_داخل_کرو;
    ورنہ اگر (state & منوال_CMDLINE)
	idx = MENU_اشاریہ_CMDLINE;
# ifdef FEAT_TERMINAL
    ورنہ اگر (رکن_use_loop())
	idx = MENU_اشاریہ_TERMINAL;
# endif
    ورنہ اگر (VIsual_فعال)
    {
	اگر (VIsual_select)
	    idx = MENU_اشاریہ_SELECT;
	ورنہ
	    idx = MENU_اشاریہ_VISUAL;
    }
    ورنہ اگر (state == منوال_HITRETURN || state == منوال_ASKMORE)
	idx = MENU_اشاریہ_CMDLINE;
    ورنہ اگر (اختتام_op)
	idx = MENU_اشاریہ_OP_PENDING;
    ورنہ اگر ((state & منوال_معمول))
	idx = MENU_اشاریہ_معمول;
    ورنہ
	idx = MENU_اشاریہ_INVALID;

    اگر (idx != MENU_اشاریہ_INVALID && menu->لڑیاں[idx] == تہی)
	idx = MENU_اشاریہ_INVALID;
    واپس idx;
}
#اگر_ختم

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    ساکن char_u *
menu_متن(char_u *str, صحیح *mnemonic, char_u **actext)
{
    char_u	*p;
    char_u	*متن;

    // Locate accelerator text, after the first TAB
    p = vim_strchr(str, TAB);
    اگر (p != تہی)
    {
	اگر (actext != تہی)
	    *actext = vim_strsave(p + 1);
	متن = vim_strnsave(str, p - str);
    }
    ورنہ
	متن = vim_strsave(str);

    // Find mnemonic characters "&a" and reduce "&&" to "&".
    برائے (p = متن; p != تہی; )
    {
	p = vim_strchr(p, '&');
	اگر (p != تہی)
	{
	    اگر (p[1] == NUL)	    // trailing "&"
		توڑو;
	    اگر (mnemonic != تہی && p[1] != '&')
#اگر !defined(__MVS__) || defined(MOTIF390_MNEMONIC_FIXED)
		*mnemonic = p[1];
#ورنہ
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
#اگر_ختم
	    STRMOVE(p, p + 1);
	    p = p + 1;
	}
    }
    واپس متن;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    صحیح
menu_is_menubar(char_u *name)
{
    واپس (!menu_is_popup(name)
	    && !menu_is_toolbar(name)
	    && !menu_is_winbar(name)
	    && *name != MNU_HIDDEN_CHAR);
}

/*
 * Return TRUE if "name" is a popup menu name.
 */
    صحیح
menu_is_popup(char_u *name)
{
    واپس (STRNCMP(name, "PopUp", 5) == 0);
}

#اگر defined(FEAT_GUI_MOTIF) && (Xm_نسخہ <= 1002)
/*
 * Return TRUE if "name" is part of a popup menu.
 */
    صحیح
menu_is_بچہ_کا_popup(vimmenu_T *menu)
{
    جب (menu->والد != تہی)
	menu = menu->والد;
    واپس menu_is_popup(menu->name);
}
#اگر_ختم

/*
 * Return TRUE if "name" is a toolbar menu name.
 */
    صحیح
menu_is_toolbar(char_u *name)
{
    واپس (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Return TRUE if the name is a menu separator identifier: Starts and ends
 * with '-'
 */
    صحیح
menu_is_separator(char_u *name)
{
    واپس (name[0] == '-' && name[STRLEN(name) - 1] == '-');
}

/*
 * Return TRUE if the menu is hidden:  Starts with ']'
 */
    ساکن صحیح
menu_is_hidden(char_u *name)
{
    واپس (name[0] == ']') || (menu_is_popup(name) && name[5] != NUL);
}

/*
 * Return TRUE if the menu is the tearoff menu.
 */
    ساکن صحیح
menu_is_tearoff(char_u *name غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_GUI
    واپس (STRCMP(name, TEAR_STRING) == 0);
#ورنہ
    واپس FALSE;
#اگر_ختم
}

#اگر defined(FEAT_GUI) || defined(FEAT_رکن_POPUP_MENU)

    ساکن صحیح
get_menu_منوال(خالی)
{
# ifdef FEAT_TERMINAL
    اگر (رکن_use_loop())
	واپس MENU_اشاریہ_TERMINAL;
# endif
    اگر (VIsual_فعال)
    {
	اگر (VIsual_select)
	    واپس MENU_اشاریہ_SELECT;
	واپس MENU_اشاریہ_VISUAL;
    }
    اگر (State & منوال_داخل_کرو)
	واپس MENU_اشاریہ_داخل_کرو;
    اگر ((State & منوال_CMDLINE) || State == منوال_ASKMORE
						    || State == منوال_HITRETURN)
	واپس MENU_اشاریہ_CMDLINE;
    اگر (اختتام_op)
	واپس MENU_اشاریہ_OP_PENDING;
    اگر (State & منوال_معمول)
	واپس MENU_اشاریہ_معمول;
    اگر (State & منوال_LANGMAP)	// must be a "r" command, like Insert mode
	واپس MENU_اشاریہ_داخل_کرو;
    واپس MENU_اشاریہ_INVALID;
}

    صحیح
get_menu_منوال_flag(خالی)
{
    صحیح منوال = get_menu_منوال();

    اگر (منوال == MENU_اشاریہ_INVALID)
	واپس 0;
    واپس 1 << منوال;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    خالی
show_popupmenu(خالی)
{
    vimmenu_T	*menu;
    صحیح		menu_منوال;
    حرف*	منوال;
    صحیح		منوال_len;

    menu_منوال = get_menu_منوال();
    اگر (menu_منوال == MENU_اشاریہ_INVALID)
	واپس;
    منوال = menu_منوال_chars[menu_منوال];
    منوال_len = (صحیح)لڑی_لمبائی(منوال);

    apply_autocmds(EVENT_MENUPOPUP, (char_u*)منوال, تہی, FALSE, curbuf);

    FOR_ALL_MENUS(menu)
	اگر (STRNCMP("PopUp", menu->name, 5) == 0 && STRNCMP(menu->name + 5, منوال, منوال_len) == 0)
	    توڑو;

    // Only show a popup when it is defined and has entries
    اگر (menu == تہی || menu->children == تہی)
	واپس;

# اگر defined(FEAT_GUI)
    اگر (gui.اندر_use)
    {
	// Update the menus now, in case the MenuPopup autocommand did
	// anything.
	gui_update_menus(0);
	gui_mch_show_popupmenu(menu);
    }
# endif
# اگر defined(FEAT_GUI) && defined(FEAT_رکن_POPUP_MENU)
    ورنہ
# endif
# اگر defined(FEAT_رکن_POPUP_MENU)
	pum_show_popupmenu(menu);
# endif
}
#اگر_ختم

#اگر defined(FEAT_GUI)

/*
 * Check that a pointer appears in the menu tree.  Used to protect from using
 * a menu that was deleted after it was selected but before the event was
 * handled.
 * Return OK or FAIL.  Used recursively.
 */
    صحیح
check_menu_اشارہ_گر(vimmenu_T *جڑ, vimmenu_T *menu_مقصد_check)
{
    vimmenu_T	*p;

    برائے (p = جڑ; p != تہی; p = p->اگلا)
	اگر (p == menu_مقصد_check
		|| (p->children != تہی
		    && check_menu_اشارہ_گر(p->children, menu_مقصد_check) == OK))
	    واپس OK;
    واپس FAIL;
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(root_menu);
 */
    خالی
gui_create_initial_menus(vimmenu_T *menu)
{
    صحیح		idx = 0;

    جب (menu != تہی)
    {
	// Don't add a menu when only a tip was defined.
	اگر (menu->modes & MENU_ALL_MODES)
	{
	    اگر (menu->children != تہی)
	    {
		gui_mch_جوڑو_menu(menu, idx);
		gui_create_initial_menus(menu->children);
	    }
	    ورنہ
		gui_mch_جوڑو_menu_شے(menu, idx);
	}
	menu = menu->اگلا;
	++idx;
    }
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    ساکن خالی
gui_update_menus_recurse(vimmenu_T *menu, صحیح منوال)
{
    صحیح		grey;

    جب (menu)
    {
	اگر ((menu->modes & menu->enabled & منوال)
# اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)
		|| menu_is_tearoff(menu->dname)
# endif
	   )
	    grey = FALSE;
	ورنہ
	    grey = TRUE;

	// Never hide a toplevel menu, it may make the menubar resize or
	// disappear. Same problem for ToolBar items.
	اگر (vim_strchr(p_go, GO_GREY) != تہی || menu->والد == تہی
# ifdef FEAT_TOOLBAR
		|| menu_is_toolbar(menu->والد->name)
# endif
		   )
	    gui_mch_menu_grey(menu, grey);
	ورنہ
	    gui_mch_menu_hidden(menu, grey);
	gui_update_menus_recurse(menu->children, منوال);
	menu = menu->اگلا;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    خالی
gui_update_menus(صحیح modes)
{
    ساکن صحیح	    پچھلا_منوال = -1;
    صحیح		    منوال = 0;

    اگر (modes != 0x0)
	منوال = modes;
    ورنہ
	منوال = get_menu_منوال_flag();

    اگر (force_menu_update || منوال != پچھلا_منوال)
    {
	gui_update_menus_recurse(جڑ_menu, منوال);
	gui_mch_draw_menubar();
	پچھلا_منوال = منوال;
	force_menu_update = FALSE;
    }
}

# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    صحیح
gui_is_menu_shortcut(صحیح کلید)
{
    vimmenu_T	*menu;

    اگر (کلید < 256)
	کلید = TOLOWER_LOC(کلید);
    FOR_ALL_MENUS(menu)
	اگر (menu->mnemonic == کلید
		|| (menu->mnemonic < 256 && TOLOWER_LOC(menu->mnemonic) == کلید))
	    واپس TRUE;
    واپس FALSE;
}
# endif
#اگر_ختم // FEAT_GUI

#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_TEAROFF)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    خالی
gui_mch_toggle_tearoffs(صحیح enable)
{
    صحیح		pri_tab[MENUDEPTH + 1];
    صحیح		i;

    اگر (enable)
    {
	برائے (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(جڑ_menu, (char_u *)"", pri_tab, 0);
    }
    ورنہ
	gui_destroy_tearoffs_recurse(جڑ_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    ساکن خالی
gui_create_tearoffs_recurse(
    vimmenu_T		*menu,
    ثابت char_u	*pname,
    صحیح			*pri_tab,
    صحیح			pri_idx)
{
    char_u	*newpname = تہی;
    صحیح		len;
    char_u	*s;
    char_u	*d;

    اگر (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    جب (menu != تہی)
    {
	اگر (menu->children != تہی && menu_is_menubar(menu->name))
	{
	    // Add the menu name to the menu path.  Insert a backslash before
	    // dots (it's used to separate menu names).
	    len = (صحیح)STRLEN(pname) + (صحیح)STRLEN(menu->name);
	    برائے (s = menu->name; *s; ++s)
		اگر (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    اگر (newpname != تہی)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		برائے (s = menu->name; *s; ++s)
		{
		    اگر (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		// check if tearoff already exists
		اگر (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_جوڑو_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			// remove TEAR_STRING
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->اگلا;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    ساکن خالی
gui_جوڑو_tearoff(char_u *tearpath, صحیح *pri_tab, صحیح pri_idx)
{
    char_u	*tbuf;
    صحیح		t;
    vimmenu_T	menuarg;

    tbuf = alloc(5 + (غیرعلامتی صحیح)STRLEN(tearpath));
    اگر (tbuf == تہی)
	واپس;

    tbuf[0] = K_SPECIAL;
    tbuf[1] = K_دوسرا(K_TEAROFF);
    tbuf[2] = K_تیسرا(K_TEAROFF);
    STRCPY(tbuf + 3, tearpath);
    STRCAT(tbuf + 3, "\r");

    STRCAT(tearpath, ".");
    STRCAT(tearpath, TEAR_STRING);

    // Priority of tear-off is always 1
    t = pri_tab[pri_idx + 1];
    pri_tab[pri_idx + 1] = 1;

# ifdef FEAT_TOOLBAR
    menuarg.iconfile = تہی;
    menuarg.iconidx = -1;
    menuarg.icon_builtin = FALSE;
# endif
    menuarg.noremap[0] = REMAP_NONE;
    menuarg.silent[0] = TRUE;

    menuarg.modes = MENU_ALL_MODES;
    جوڑو_menu_راستہ(tearpath, &menuarg, pri_tab, tbuf, FALSE);

    menuarg.modes = MENU_TIP_منوال;
    جوڑو_menu_راستہ(tearpath, &menuarg, pri_tab,
	    (char_u *)_("Tear off this menu"), FALSE);

    pri_tab[pri_idx + 1] = t;
    vim_free(tbuf);
}

/*
 * Recursively destroy tearoff items
 */
    ساکن خالی
gui_destroy_tearoffs_recurse(vimmenu_T *menu)
{
    جب (menu)
    {
	اگر (menu->children)
	{
	    // check if tearoff exists
	    اگر (STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		// Disconnect the item and free the memory
		free_menu(&menu->children);
	    }
	    اگر (menu->children != تہی) // if not the last one
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->اگلا;
    }
}

#اگر_ختم // FEAT_GUI_MSWIN && FEAT_TEAROFF

/*
 * Execute "menu".  Use by ":emenu" and the window toolbar.
 * "eap" is NULL for the window toolbar.
 * "mode_idx" specifies a MENU_INDEX_ value, use MENU_INDEX_INVALID to depend
 * on the current state.
 */
    خالی
execute_menu(exarg_T *eap, vimmenu_T *menu, صحیح منوال_idx)
{
    صحیح		idx = منوال_idx;

    اگر (idx < 0)
    {
	// Use the Insert mode entry when returning to Insert mode.
	اگر (restart_edit && current_sctx.sc_sid == 0)
	{
	    idx = MENU_اشاریہ_داخل_کرو;
	}
#اگر_تعریف_شدہ FEAT_TERMINAL
	ورنہ اگر (رکن_use_loop())
	{
	    idx = MENU_اشاریہ_TERMINAL;
	}
#اگر_ختم
	ورنہ اگر (VIsual_فعال)
	{
	    idx = MENU_اشاریہ_VISUAL;
	}
	ورنہ اگر (eap != تہی && eap->addr_گنتی)
	{
	    pos_T	tpos;

	    idx = MENU_اشاریہ_VISUAL;

	    // GEDDES: This is not perfect - but it is a
	    // quick way of detecting whether we are doing this from a
	    // selection - see if the range matches up with the visual
	    // select start and end.
	    اگر ((curbuf->b_visual.vi_آغاز.lnum == eap->line1)
		    && (curbuf->b_visual.vi_انتہا.lnum) == eap->line2)
	    {
		// Set it up for visual mode - equivalent to gv.
		VIsual_منوال = curbuf->b_visual.vi_منوال;
		tpos = curbuf->b_visual.vi_انتہا;
		curwin->w_cursor = curbuf->b_visual.vi_آغاز;
		curwin->w_curswant = curbuf->b_visual.vi_curswant;
	    }
	    ورنہ
	    {
		// Set it up for line-wise visual mode
		VIsual_منوال = 'V';
		curwin->w_cursor.lnum = eap->line1;
		curwin->w_cursor.ستون = 1;
		tpos.lnum = eap->line2;
		tpos.ستون = MAXCOL;
		tpos.coladd = 0;
	    }

	    // Activate visual mode
	    VIsual_فعال = TRUE;
	    VIsual_reselect = TRUE;
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;

	    check_cursor();

	    // Adjust the cursor to make sure it is in the correct pos
	    // for exclusive mode
	    اگر (*p_sel == 'e' && gchar_cursor() != NUL)
		++curwin->w_cursor.ستون;
	}
    }

    // For the WinBar menu always use the Normal mode menu.
    اگر (idx == MENU_اشاریہ_INVALID || eap == تہی)
	idx = MENU_اشاریہ_معمول;

    اگر (menu->لڑیاں[idx] != تہی && (menu->modes & (1 << idx)))
    {
	// When executing a script or function execute the commands right now.
	// Also for the window toolbar.
	// Otherwise put them in the typeahead buffer.
	اگر (eap == تہی || current_sctx.sc_sid != 0)
	{
	    save_state_T save_state;

	    ++ex_معمول_busy;
	    اگر (save_current_state(&save_state))
		exec_معمول_cmd(menu->لڑیاں[idx], menu->noremap[idx],
							   menu->silent[idx]);
	    restore_current_state(&save_state);
	    --ex_معمول_busy;
	}
	ورنہ
	    ins_typebuf(menu->لڑیاں[idx], menu->noremap[idx], 0,
						     TRUE, menu->silent[idx]);
    }
    ورنہ اگر (eap != تہی)
    {
	char_u	*منوال;

	انتخاب (idx)
	{
	    حالت MENU_اشاریہ_VISUAL:
		منوال = (char_u *)"Visual";
		توڑو;
	    حالت MENU_اشاریہ_SELECT:
		منوال = (char_u *)"Select";
		توڑو;
	    حالت MENU_اشاریہ_OP_PENDING:
		منوال = (char_u *)"Op-pending";
		توڑو;
	    حالت MENU_اشاریہ_TERMINAL:
		منوال = (char_u *)"Terminal";
		توڑو;
	    حالت MENU_اشاریہ_داخل_کرو:
		منوال = (char_u *)"Insert";
		توڑو;
	    حالت MENU_اشاریہ_CMDLINE:
		منوال = (char_u *)"Cmdline";
		توڑو;
	    // case MENU_INDEX_TIP: cannot happen
	    طےشدہ:
		منوال = (char_u *)"Normal";
	}
	semsg(_(e_menu_not_defined_for_str_منوال), منوال);
    }
}

/*
 * Lookup a menu by the descriptor name e.g. "File.New"
 * Returns NULL if the menu is not found
 */
    ساکن vimmenu_T *
menu_getbyname(char_u *name_arg)
{
    char_u	*name;
    char_u	*saved_name;
    vimmenu_T	*menu;
    char_u	*p;
    صحیح		gave_emsg = FALSE;

    saved_name = vim_strsave(name_arg);
    اگر (saved_name == تہی)
	واپس تہی;

    menu = *get_جڑ_menu(saved_name);
    name = saved_name;
    جب (*name)
    {
	// Find in the menu hierarchy
	p = menu_name_skip(name);

	جب (menu != تہی)
	{
	    اگر (menu_name_equal(name, menu))
	    {
		اگر (*p == NUL && menu->children != تہی)
		{
		    emsg(_(e_menu_راستہ_must_lead_مقصد_menu_شے));
		    gave_emsg = TRUE;
		    menu = تہی;
		}
		ورنہ اگر (*p != NUL && menu->children == تہی)
		{
		    emsg(_(e_part_کا_menu_شے_راستہ_is_not_sub_menu));
		    menu = تہی;
		}
		توڑو;
	    }
	    menu = menu->اگلا;
	}
	اگر (menu == تہی || *p == NUL)
	    توڑو;
	menu = menu->children;
	name = p;
    }
    vim_free(saved_name);
    اگر (menu == تہی)
    {
	اگر (!gave_emsg)
	    semsg(_(e_menu_not_ملا_str), name_arg);
	واپس تہی;
    }

    واپس menu;
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * execute it.
 */
    خالی
ex_emenu(exarg_T *eap)
{
    vimmenu_T	*menu;
    char_u	*arg = eap->arg;
    صحیح		منوال_idx = MENU_اشاریہ_INVALID;

    اگر (arg[0] && VIM_ISWHITE(arg[1]))
    {
	انتخاب (arg[0])
	{
	    حالت 'n': منوال_idx = MENU_اشاریہ_معمول; توڑو;
	    حالت 'v': منوال_idx = MENU_اشاریہ_VISUAL; توڑو;
	    حالت 's': منوال_idx = MENU_اشاریہ_SELECT; توڑو;
	    حالت 'o': منوال_idx = MENU_اشاریہ_OP_PENDING; توڑو;
	    حالت 't': منوال_idx = MENU_اشاریہ_TERMINAL; توڑو;
	    حالت 'i': منوال_idx = MENU_اشاریہ_داخل_کرو; توڑو;
	    حالت 'c': منوال_idx = MENU_اشاریہ_CMDLINE; توڑو;
	    طےشدہ: semsg(_(e_invalid_دلیل_str), arg);
		     واپس;
	}
	arg = skipwhite(arg + 2);
    }

    menu = menu_getbyname(arg);
    اگر (menu == تہی)
	واپس;

    // Found the menu, so execute.
    execute_menu(eap, menu, منوال_idx);
}

/*
 * Handle a click in the window toolbar of "wp" at column "col".
 */
    خالی
winbar_click(win_T *wp, صحیح ستون)
{
    صحیح		idx;

    اگر (wp->w_winbar_اشیاء == تہی)
	واپس;
    برائے (idx = 0; wp->w_winbar_اشیاء[idx].wb_menu != تہی; ++idx)
    {
	winbar_شے_T *شے = &wp->w_winbar_اشیاء[idx];

	اگر (ستون >= شے->wb_startcol && ستون <= شے->wb_endcol)
	{
	    win_T   *save_curwin = تہی;
	    pos_T   save_visual = VIsual;
	    صحیح	    save_visual_فعال = VIsual_فعال;
	    صحیح	    save_visual_select = VIsual_select;
	    صحیح	    save_visual_reselect = VIsual_reselect;
	    صحیح	    save_visual_منوال = VIsual_منوال;

	    اگر (wp != curwin)
	    {
		// Clicking in the window toolbar of a not-current window.
		// Make that window the current one and save Visual mode.
		save_curwin = curwin;
		VIsual_فعال = FALSE;
		curwin = wp;
		curbuf = curwin->w_بفر;
		check_cursor();
	    }

	    // Note: the command might close the current window.
	    execute_menu(تہی, شے->wb_menu, -1);

	    اگر (save_curwin != تہی && win_valid(save_curwin))
	    {
		curwin = save_curwin;
		curbuf = curwin->w_بفر;
		VIsual = save_visual;
		VIsual_فعال = save_visual_فعال;
		VIsual_select = save_visual_select;
		VIsual_reselect = save_visual_reselect;
		VIsual_منوال = save_visual_منوال;
	    }
	    اگر (!win_valid(wp))
		توڑو;
	}
    }
}

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_رکن_POPUP_MENU) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_BEVAL_TIP)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy.
 */
    vimmenu_T *
gui_ڈھونڈو_menu(char_u *راستہ_name)
{
    vimmenu_T	*menu = تہی;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menu = *get_جڑ_menu(راستہ_name);

    saved_name = vim_strsave(راستہ_name);
    اگر (saved_name == تہی)
	واپس تہی;

    name = saved_name;
    جب (*name)
    {
	// find the end of one dot-separated name and put a NUL at the dot
	p = menu_name_skip(name);

	جب (menu != تہی)
	{
	    اگر (menu_name_equal(name, menu))
	    {
		اگر (menu->children == تہی)
		{
		    // found a menu item instead of a sub-menu
		    اگر (*p == NUL)
			emsg(_(e_menu_راستہ_must_lead_مقصد_sub_menu));
		    ورنہ
			emsg(_(e_part_کا_menu_شے_راستہ_is_not_sub_menu));
		    menu = تہی;
		    جاؤ theend;
		}
		اگر (*p == NUL)	    // found a full match
		    جاؤ theend;
		توڑو;
	    }
	    menu = menu->اگلا;
	}
	اگر (menu == تہی)	// didn't find it
	    توڑو;

	// Found a match, search the sub-menu.
	menu = menu->children;
	name = p;
    }

    اگر (menu == تہی)
	emsg(_(e_menu_not_ملا_check_menu_نام));
theend:
    vim_free(saved_name);
    واپس menu;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MULTI_LANG
/*
 * Translation of menu names.  Just a simple lookup table.
 */

قسمتعریف ساخت
{
    char_u	*from;		// English name
    char_u	*from_noamp;	// same, without '&'
    char_u	*مقصد;		// translated name
} menutrans_T;

ساکن garray_T menutrans_ga = {0, 0, 0, 0, تہی};
#اگر_ختم

/*
 * ":menutrans".
 * This function is also defined without the +multi_lang feature, in which
 * case the commands are ignored.
 */
    خالی
ex_menutranslate(exarg_T *eap غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_MULTI_LANG
    char_u		*arg = eap->arg;
    menutrans_T		*tp;
    صحیح			i;
    char_u		*from, *from_noamp, *مقصد;

    اگر (menutrans_ga.ga_itemsize == 0)
	ga_init2(&menutrans_ga, حجم(menutrans_T), 5);

    /*
     * ":menutrans clear": clear all translations.
     */
    اگر (STRNCMP(arg, "clear", 5) == 0 && سرے_excmd2(arg, skipwhite(arg + 5)))
    {
	tp = (menutrans_T *)menutrans_ga.ga_ڈیٹا;
	برائے (i = 0; i < menutrans_ga.ga_len; ++i)
	{
	    vim_free(tp[i].from);
	    vim_free(tp[i].from_noamp);
	    vim_free(tp[i].مقصد);
	}
	ga_clear(&menutrans_ga);
# ifdef FEAT_EVAL
	// Delete all "menutrans_" global variables.
	del_menutrans_vars();
# endif
    }
    ورنہ
    {
	// ":menutrans from to": add translation
	from = arg;
	arg = menu_skip_part(arg);
	مقصد = skipwhite(arg);
	*arg = NUL;
	arg = menu_skip_part(مقصد);
	اگر (arg == مقصد || سرے_excmd2(eap->arg, from)
		      || سرے_excmd2(eap->arg, مقصد)
		      || !سرے_excmd2(eap->arg, skipwhite(arg)))
	    emsg(_(e_invalid_دلیل));
	ورنہ
	{
	    اگر (ga_grow(&menutrans_ga, 1) == OK)
	    {
		tp = (menutrans_T *)menutrans_ga.ga_ڈیٹا;
		from = vim_strsave(from);
		اگر (from != تہی)
		{
		    from_noamp = menu_متن(from, تہی, تہی);
		    مقصد = vim_strnsave(مقصد, arg - مقصد);
		    اگر (from_noamp != تہی && مقصد != تہی)
		    {
			menu_translate_tab_and_سرکاؤ(from);
			menu_translate_tab_and_سرکاؤ(مقصد);
			menu_unescape_name(from);
			menu_unescape_name(مقصد);
			tp[menutrans_ga.ga_len].from = from;
			tp[menutrans_ga.ga_len].from_noamp = from_noamp;
			tp[menutrans_ga.ga_len].مقصد = مقصد;
			++menutrans_ga.ga_len;
		    }
		    ورنہ
		    {
			vim_free(from);
			vim_free(from_noamp);
			vim_free(مقصد);
		    }
		}
	    }
	}
    }
#اگر_ختم
}

#اگر defined(FEAT_MULTI_LANG) || defined(FEAT_TOOLBAR)
/*
 * Find the character just after one part of a menu name.
 */
    ساکن char_u *
menu_skip_part(char_u *p)
{
    جب (*p != NUL && *p != '.' && !VIM_ISWHITE(*p))
    {
	اگر ((*p == '\\' || *p == Ctrl_V) && p[1] != NUL)
	    ++p;
	++p;
    }
    واپس p;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MULTI_LANG
/*
 * Lookup part of a menu name in the translations.
 * Return a pointer to the translation or NULL if not found.
 */
    ساکن char_u *
menutrans_lookup(char_u *name, صحیح len)
{
    menutrans_T		*tp = (menutrans_T *)menutrans_ga.ga_ڈیٹا;
    صحیح			i;
    char_u		*dname;

    برائے (i = 0; i < menutrans_ga.ga_len; ++i)
	اگر (STRNICMP(name, tp[i].from, len) == 0 && tp[i].from[len] == NUL)
	    واپس tp[i].مقصد;

    // Now try again while ignoring '&' characters.
    i = name[len];
    name[len] = NUL;
    dname = menu_متن(name, تہی, تہی);
    name[len] = i;
    اگر (dname == تہی)
	واپس تہی;

    برائے (i = 0; i < menutrans_ga.ga_len; ++i)
	اگر (STRICMP(dname, tp[i].from_noamp) == 0)
	{
	    vim_free(dname);
	    واپس tp[i].مقصد;
	}
    vim_free(dname);

    واپس تہی;
}

/*
 * Unescape the name in the translate dictionary table.
 */
    ساکن خالی
menu_unescape_name(char_u *name)
{
    char_u  *p;

    برائے (p = name; *p && *p != '.'; MB_PTR_ADV(p))
	اگر (*p == '\\')
	    STRMOVE(p, p + 1);
}
#اگر_ختم // FEAT_MULTI_LANG

/*
 * Isolate the menu name.
 * Skip the menu name, and translate <Tab> into a real TAB.
 */
    ساکن char_u *
menu_translate_tab_and_سرکاؤ(char_u *arg_آغاز)
{
    char_u	*arg = arg_آغاز;

    جب (*arg && !VIM_ISWHITE(*arg))
    {
	اگر ((*arg == '\\' || *arg == Ctrl_V) && arg[1] != NUL)
	    arg++;
	ورنہ اگر (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    STRMOVE(arg + 1, arg + 5);
	}
	arg++;
    }
    اگر (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);

    واپس arg;
}

/*
 * Get the information about a menu item in mode 'which'
 */
    ساکن صحیح
menuitem_getinfo(char_u *menu_name, vimmenu_T *menu, صحیح modes, dict_T *dict)
{
    صحیح		حالت_2;
    list_T	*l;

    اگر (*menu_name == NUL)
    {
	// Return all the top-level menus
	vimmenu_T	*topmenu;

	l = list_alloc();
	اگر (l == تہی)
	    واپس FAIL;

	اگر (dict_جوڑو_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    واپس FAIL;
	}
	// get all the children.  Skip PopUp[nvoci].
	برائے (topmenu = menu; topmenu != تہی; topmenu = topmenu->اگلا)
	    اگر (!menu_is_hidden(topmenu->dname))
		list_append_string(l, topmenu->dname, -1);
	واپس OK;
    }

    اگر (menu_is_tearoff(menu->dname))		// skip tearoff menu item
	واپس OK;

    حالت_2 = dict_جوڑو_string(dict, "name", menu->name);
    اگر (حالت_2 == OK)
	حالت_2 = dict_جوڑو_string(dict, "display", menu->dname);
    اگر (حالت_2 == OK && menu->actext != تہی)
	حالت_2 = dict_جوڑو_string(dict, "accel", menu->actext);
    اگر (حالت_2 == OK)
	حالت_2 = dict_جوڑو_نمبر(dict, "priority", menu->ترجیح);
    اگر (حالت_2 == OK)
	حالت_2 = dict_جوڑو_string(dict, "modes",
					get_menu_منوال_str(menu->modes));
#اگر_تعریف_شدہ FEAT_TOOLBAR
    اگر (حالت_2 == OK && menu->iconfile != تہی)
	حالت_2 = dict_جوڑو_string(dict, "icon", menu->iconfile);
    اگر (حالت_2 == OK && menu->iconidx >= 0)
	حالت_2 = dict_جوڑو_نمبر(dict, "iconidx", menu->iconidx);
#اگر_ختم
    اگر (حالت_2 == OK)
    {
	char_u	buf[NUMBUFLEN];
	صحیح	buflen;

	اگر (has_mbyte)
	    buflen = utf_char2bytes(menu->mnemonic, buf);
	ورنہ
	{
	    buf[0] = (char_u)menu->mnemonic;
	    buflen = (buf[0] == NUL) ? 0 : 1;
	}
	buf[buflen] = NUL;
	حالت_2 = dict_جوڑو_string_len(dict, "shortcut", buf, buflen);
    }
    اگر (حالت_2 == OK && menu->children == تہی)
    {
	صحیح		ثنائی_ہندسہ;

	// Get the first mode in which the menu is available
	برائے (ثنائی_ہندسہ = 0; ثنائی_ہندسہ < MENU_MODES && !((1 << ثنائی_ہندسہ) & modes); ثنائی_ہندسہ++)
	    ;
	اگر (ثنائی_ہندسہ < MENU_MODES) // just in case, avoid Coverity warning
	{
	    اگر (menu->لڑیاں[ثنائی_ہندسہ] != تہی)
	    {
		اگر (*menu->لڑیاں[ثنائی_ہندسہ] == NUL)
		    حالت_2 = dict_جوڑو_string_len(dict, "rhs",
			(char_u *)"<Nop>", STRLEN_LITERAL("<Nop>"));
		ورنہ
		{
		    char_u *tofree = تہی;

		    حالت_2 = dict_جوڑو_string(dict, "rhs",
			tofree = str2special_save(menu->لڑیاں[ثنائی_ہندسہ], FALSE, FALSE));
		    vim_free(tofree);
		}
	    }
	    اگر (حالت_2 == OK)
		حالت_2 = dict_جوڑو_bool(dict, "noremenu",
					     menu->noremap[ثنائی_ہندسہ] == REMAP_NONE);
	    اگر (حالت_2 == OK)
		حالت_2 = dict_جوڑو_bool(dict, "script",
					   menu->noremap[ثنائی_ہندسہ] == REMAP_SCRIPT);
	    اگر (حالت_2 == OK)
		حالت_2 = dict_جوڑو_bool(dict, "silent", menu->silent[ثنائی_ہندسہ]);
	    اگر (حالت_2 == OK)
		حالت_2 = dict_جوڑو_bool(dict, "enabled",
					  ((menu->enabled & (1 << ثنائی_ہندسہ)) != 0));
	}
    }

    // If there are submenus, add all the submenu display names
    اگر (حالت_2 == OK && menu->children != تہی)
    {
	vimmenu_T	*بچہ;

	l = list_alloc();
	اگر (l == تہی)
	    واپس FAIL;

	اگر (dict_جوڑو_list(dict, "submenus", l) == FAIL)
	{
	    list_unref(l);
	    واپس FAIL;
	}
	بچہ = menu->children;
	جب (بچہ)
	{
	    اگر (!menu_is_tearoff(بچہ->dname))		// skip tearoff menu
		list_append_string(l, بچہ->dname, -1);
	    بچہ = بچہ->اگلا;
	}
    }

    واپس حالت_2;
}

/*
 * "menu_info()" function
 * Return information about a menu (including all the child menus)
 */
    خالی
f_menu_info(typval_T *argvars, typval_T *rettv)
{
    char_u	*menu_name;
    char_u	*which;
    صحیح		modes;
    char_u	*saved_name;
    char_u	*name;
    vimmenu_T	*menu;
    dict_T	*retdict;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    retdict = rettv->vval.v_dict;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    menu_name = tv_get_string_chk(&argvars[0]);
    اگر (menu_name == تہی)
	واپس;

    // menu mode
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	which = tv_get_string_chk(&argvars[1]);
    ورنہ
	which = (char_u *)"";	    // Default is modes for "menu"
    اگر (which == تہی)
	واپس;

    modes = get_menu_cmd_modes(which, *which == '!', تہی, تہی);

    // Locate the specified menu or menu item
    menu = *get_جڑ_menu(menu_name);
    saved_name = vim_strsave(menu_name);
    اگر (saved_name == تہی)
	واپس;
    اگر (*saved_name != NUL)
    {
	char_u	*p;

	name = saved_name;
	جب (*name)
	{
	    // Find in the menu hierarchy
	    p = menu_name_skip(name);
	    جب (menu != تہی)
	    {
		اگر (menu_name_equal(name, menu))
		    توڑو;
		menu = menu->اگلا;
	    }
	    اگر (menu == تہی || *p == NUL)
		توڑو;
	    menu = menu->children;
	    name = p;
	}
    }
    vim_free(saved_name);

    اگر (menu == تہی)		// specified menu not found
	واپس;

    اگر (menu->modes & modes)
	menuitem_getinfo(menu_name, menu, modes, retdict);
}

#اگر_ختم // FEAT_MENU
