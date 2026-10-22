/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ifndef GUI_GTK4_MENU_H
#define GUI_GTK4_MENU_H

#شامل "vim.h"

#ifdef FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_ITEM (vim_menu_bar_item_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBarItem, vim_menu_bar_item, VIM, MENU_BAR_ITEM, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_ITEM (vim_menu_item_get_type())
G_DECLARE_FINAL_TYPE(VimMenuItem, vim_menu_item, VIM, MENU_ITEM, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

تعریفنوع شمارش
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

تعریفنوع تهی (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, تهی *udata);

GtkWidget *vim_menu_bar_item_new(ثابت نویسه *text, VimMenu *menu);
تهی vim_menu_bar_item_set_text(VimMenuBarItem *self, ثابت نویسه *text);

GtkWidget *vim_menu_bar_new(تهی);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
تهی vim_menu_bar_درج_item(VimMenuBar *self, VimMenuBarItem *item, صحیح idx);
تهی vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
تهی vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(ثابت نویسه *text, VimMenuItemFunc func, تهی *udata);
تهی vim_menu_item_set_text(VimMenuItem *self, ثابت نویسه *text);
تهی vim_menu_item_set_accel(VimMenuItem *self, ثابت نویسه *accel_text);
تهی vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(تهی);
تهی vim_menu_درج_item(VimMenu *self, VimMenuItem *item, صحیح idx);
GtkWidget *vim_menu_درج_separator(VimMenu *self, صحیح idx);
تهی vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
