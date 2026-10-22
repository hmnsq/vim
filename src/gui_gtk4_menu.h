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

#અન્તર્ભાવ "vim.h"

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

પ્રકારવ્યાખ્યા ગણના
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

પ્રકારવ્યાખ્યા શૂન્ય (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, શૂન્ય *udata);

GtkWidget *vim_menu_bar_item_new(અચલ અક્ષર *text, VimMenu *menu);
શૂન્ય vim_menu_bar_item_set_text(VimMenuBarItem *self, અચલ અક્ષર *text);

GtkWidget *vim_menu_bar_new(શૂન્ય);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
શૂન્ય vim_menu_bar_ઉમેરો_item(VimMenuBar *self, VimMenuBarItem *item, પૂર્ણાંક idx);
શૂન્ય vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
શૂન્ય vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(અચલ અક્ષર *text, VimMenuItemFunc func, શૂન્ય *udata);
શૂન્ય vim_menu_item_set_text(VimMenuItem *self, અચલ અક્ષર *text);
શૂન્ય vim_menu_item_set_accel(VimMenuItem *self, અચલ અક્ષર *accel_text);
શૂન્ય vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(શૂન્ય);
શૂન્ય vim_menu_ઉમેરો_item(VimMenu *self, VimMenuItem *item, પૂર્ણાંક idx);
GtkWidget *vim_menu_ઉમેરો_separator(VimMenu *self, પૂર્ણાંક idx);
શૂન્ય vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
