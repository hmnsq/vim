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

#அந்தர்பாவ "vim.h"

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

வகைவரை எண்ணிக்கை
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

வகைவரை வெறுமை (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, வெறுமை *udata);

GtkWidget *vim_menu_bar_item_new(மாறிலி எழுத்து *text, VimMenu *menu);
வெறுமை vim_menu_bar_item_set_text(VimMenuBarItem *self, மாறிலி எழுத்து *text);

GtkWidget *vim_menu_bar_new(வெறுமை);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
வெறுமை vim_menu_bar_செருகு_item(VimMenuBar *self, VimMenuBarItem *item, முழு idx);
வெறுமை vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
வெறுமை vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(மாறிலி எழுத்து *text, VimMenuItemFunc func, வெறுமை *udata);
வெறுமை vim_menu_item_set_text(VimMenuItem *self, மாறிலி எழுத்து *text);
வெறுமை vim_menu_item_set_accel(VimMenuItem *self, மாறிலி எழுத்து *accel_text);
வெறுமை vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(வெறுமை);
வெறுமை vim_menu_செருகு_item(VimMenu *self, VimMenuItem *item, முழு idx);
GtkWidget *vim_menu_செருகு_separator(VimMenu *self, முழு idx);
வெறுமை vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
