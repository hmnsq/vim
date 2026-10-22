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

#অন্তর্ভাব "vim.h"

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

প্রকারলেখ গণনা
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

প্রকারলেখ শূন্য (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, শূন্য *udata);

GtkWidget *vim_menu_bar_item_new(ধ্রুবক অক্ষর *text, VimMenu *menu);
শূন্য vim_menu_bar_item_set_text(VimMenuBarItem *self, ধ্রুবক অক্ষর *text);

GtkWidget *vim_menu_bar_new(শূন্য);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
শূন্য vim_menu_bar_ঢোকাও_item(VimMenuBar *self, VimMenuBarItem *item, পূর্ণসংখ্যা idx);
শূন্য vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
শূন্য vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(ধ্রুবক অক্ষর *text, VimMenuItemFunc func, শূন্য *udata);
শূন্য vim_menu_item_set_text(VimMenuItem *self, ধ্রুবক অক্ষর *text);
শূন্য vim_menu_item_set_accel(VimMenuItem *self, ধ্রুবক অক্ষর *accel_text);
শূন্য vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(শূন্য);
শূন্য vim_menu_ঢোকাও_item(VimMenu *self, VimMenuItem *item, পূর্ণসংখ্যা idx);
GtkWidget *vim_menu_ঢোকাও_separator(VimMenu *self, পূর্ণসংখ্যা idx);
শূন্য vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
