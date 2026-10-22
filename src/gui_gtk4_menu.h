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

#ಅನ್ತರ್ಭಾವ "vim.h"

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, ಶೂನ್ಯ *udata);

GtkWidget *vim_menu_bar_item_new(ಧ್ರುವ ವರ್ಣ *text, VimMenu *menu);
ಶೂನ್ಯ vim_menu_bar_item_set_text(VimMenuBarItem *self, ಧ್ರುವ ವರ್ಣ *text);

GtkWidget *vim_menu_bar_new(ಶೂನ್ಯ);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
ಶೂನ್ಯ vim_menu_bar_ಸೇರಿಸು_item(VimMenuBar *self, VimMenuBarItem *item, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
ಶೂನ್ಯ vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(ಧ್ರುವ ವರ್ಣ *text, VimMenuItemFunc func, ಶೂನ್ಯ *udata);
ಶೂನ್ಯ vim_menu_item_set_text(VimMenuItem *self, ಧ್ರುವ ವರ್ಣ *text);
ಶೂನ್ಯ vim_menu_item_set_accel(VimMenuItem *self, ಧ್ರುವ ವರ್ಣ *accel_text);
ಶೂನ್ಯ vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(ಶೂನ್ಯ);
ಶೂನ್ಯ vim_menu_ಸೇರಿಸು_item(VimMenu *self, VimMenuItem *item, ಪೂರ್ಣಾಙ್ಕ idx);
GtkWidget *vim_menu_ಸೇರಿಸು_separator(VimMenu *self, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
