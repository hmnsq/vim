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

#ضمن "vim.h"

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

تعريفنوع تعداد
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

تعريفنوع فراغ (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, فراغ *udata);

GtkWidget *vim_menu_bar_item_new(ثابت حرف *text, VimMenu *menu);
فراغ vim_menu_bar_item_set_text(VimMenuBarItem *self, ثابت حرف *text);

GtkWidget *vim_menu_bar_new(فراغ);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
فراغ vim_menu_bar_قىستۇرۇش_item(VimMenuBar *self, VimMenuBarItem *item, صحيح idx);
فراغ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
فراغ vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(ثابت حرف *text, VimMenuItemFunc func, فراغ *udata);
فراغ vim_menu_item_set_text(VimMenuItem *self, ثابت حرف *text);
فراغ vim_menu_item_set_accel(VimMenuItem *self, ثابت حرف *accel_text);
فراغ vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(فراغ);
فراغ vim_menu_قىستۇرۇش_item(VimMenu *self, VimMenuItem *item, صحيح idx);
GtkWidget *vim_menu_قىستۇرۇش_separator(VimMenu *self, صحيح idx);
فراغ vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
