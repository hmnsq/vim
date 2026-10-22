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

#അന്തര്ഭാവ "vim.h"

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

തരനിർവചനം എണ്ണൽ
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

തരനിർവചനം ശൂന്യം (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, ശൂന്യം *udata);

GtkWidget *vim_menu_bar_item_new(സ്ഥിരം അക്ഷരം *text, VimMenu *menu);
ശൂന്യം vim_menu_bar_item_set_text(VimMenuBarItem *self, സ്ഥിരം അക്ഷരം *text);

GtkWidget *vim_menu_bar_new(ശൂന്യം);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
ശൂന്യം vim_menu_bar_തിരുകുക_item(VimMenuBar *self, VimMenuBarItem *item, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
ശൂന്യം vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(സ്ഥിരം അക്ഷരം *text, VimMenuItemFunc func, ശൂന്യം *udata);
ശൂന്യം vim_menu_item_set_text(VimMenuItem *self, സ്ഥിരം അക്ഷരം *text);
ശൂന്യം vim_menu_item_set_accel(VimMenuItem *self, സ്ഥിരം അക്ഷരം *accel_text);
ശൂന്യം vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(ശൂന്യം);
ശൂന്യം vim_menu_തിരുകുക_item(VimMenu *self, VimMenuItem *item, പൂർണ്ണസംഖ്യ idx);
GtkWidget *vim_menu_തിരുകുക_separator(VimMenu *self, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
