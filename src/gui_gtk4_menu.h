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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, ଶୂନ୍ୟ *udata);

GtkWidget *vim_menu_bar_item_new(ଅଚଳ ଅକ୍ଷର *text, VimMenu *menu);
ଶୂନ୍ୟ vim_menu_bar_item_set_text(VimMenuBarItem *self, ଅଚଳ ଅକ୍ଷର *text);

GtkWidget *vim_menu_bar_new(ଶୂନ୍ୟ);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
ଶୂନ୍ୟ vim_menu_bar_ଭର୍ତ୍ତି_item(VimMenuBar *self, VimMenuBarItem *item, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
ଶୂନ୍ୟ vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(ଅଚଳ ଅକ୍ଷର *text, VimMenuItemFunc func, ଶୂନ୍ୟ *udata);
ଶୂନ୍ୟ vim_menu_item_set_text(VimMenuItem *self, ଅଚଳ ଅକ୍ଷର *text);
ଶୂନ୍ୟ vim_menu_item_set_accel(VimMenuItem *self, ଅଚଳ ଅକ୍ଷର *accel_text);
ଶୂନ୍ୟ vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(ଶୂନ୍ୟ);
ଶୂନ୍ୟ vim_menu_ଭର୍ତ୍ତି_item(VimMenu *self, VimMenuItem *item, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
GtkWidget *vim_menu_ଭର୍ତ୍ତି_separator(VimMenu *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
