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

#अन्तर्भाव "vim.h"

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

प्रकारनिर्देश गणना
{
    VIM_MENU_ITEM_CLICKED,
    VIM_MENU_ITEM_SELECTED
} VimMenuItemEvent;

प्रकारनिर्देश शून्य (*VimMenuItemFunc)(VimMenuItem *item, VimMenuItemEvent event, शून्य *udata);

GtkWidget *vim_menu_bar_item_new(ध्रुव वर्ण *text, VimMenu *menu);
शून्य vim_menu_bar_item_set_text(VimMenuBarItem *self, ध्रुव वर्ण *text);

GtkWidget *vim_menu_bar_new(शून्य);
GtkWidget *vim_menu_bar_to_menu(VimMenuBar *self);
शून्य vim_menu_bar_जोड़ो_item(VimMenuBar *self, VimMenuBarItem *item, पूर्णाङ्क idx);
शून्य vim_menu_bar_remove(VimMenuBar *self, GtkWidget *item);
शून्य vim_menu_bar_show(VimMenuBar *self, VimMenuBarItem *item);

GtkWidget *vim_menu_item_new(ध्रुव वर्ण *text, VimMenuItemFunc func, शून्य *udata);
शून्य vim_menu_item_set_text(VimMenuItem *self, ध्रुव वर्ण *text);
शून्य vim_menu_item_set_accel(VimMenuItem *self, ध्रुव वर्ण *accel_text);
शून्य vim_menu_item_set_submenu(VimMenuItem *self, VimMenu *submenu);

GtkWidget *vim_menu_new(शून्य);
शून्य vim_menu_जोड़ो_item(VimMenu *self, VimMenuItem *item, पूर्णाङ्क idx);
GtkWidget *vim_menu_जोड़ो_separator(VimMenu *self, पूर्णाङ्क idx);
शून्य vim_menu_remove(VimMenu *self, GtkWidget *item);
GtkWidget *vim_menu_copy(VimMenu *self);

#endif

#endif
