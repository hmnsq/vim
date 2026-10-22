/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GUI_GTK4_MENU_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_GTK4_MENU_H

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_ਵਸਤੂ (vim_menu_bar_ਵਸਤੂ_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_Bar_ਵਸਤੂ_2, vim_menu_bar_ਵਸਤੂ, VIM, MENU_BAR_ਵਸਤੂ, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_ਵਸਤੂ (vim_menu_ਵਸਤੂ_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_ਵਸਤੂ, vim_menu_ਵਸਤੂ, VIM, MENU_ਵਸਤੂ, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    VIM_MENU_ਵਸਤੂ_CLICKED,
    VIM_MENU_ਵਸਤੂ_SELECTED
} Vim_Menu_ਵਸਤੂ_Event;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*Vim_Menu_ਵਸਤੂ_Func)(Vim_Menu_ਵਸਤੂ *ਵਸਤੂ, Vim_Menu_ਵਸਤੂ_Event event, ਖਾਲੀ *udata);

GtkWidget *vim_menu_bar_ਵਸਤੂ_new(ਅਚਲ ਅੱਖਰ *ਪਾਠ, VimMenu *menu);
ਖਾਲੀ vim_menu_bar_ਵਸਤੂ_ਸੈੱਟ_ਪਾਠ(Vim_Menu_Bar_ਵਸਤੂ_2 *self, ਅਚਲ ਅੱਖਰ *ਪਾਠ);

GtkWidget *vim_menu_bar_new(ਖਾਲੀ);
GtkWidget *vim_menu_bar_ਮੰਜ਼ਿਲ_menu(VimMenuBar *self);
ਖਾਲੀ vim_menu_bar_ਪਾਓ_ਵਸਤੂ(VimMenuBar *self, Vim_Menu_Bar_ਵਸਤੂ_2 *ਵਸਤੂ, ਪੂਰਨ idx);
ਖਾਲੀ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *ਵਸਤੂ);
ਖਾਲੀ vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_ਵਸਤੂ_2 *ਵਸਤੂ);

GtkWidget *vim_menu_ਵਸਤੂ_new(ਅਚਲ ਅੱਖਰ *ਪਾਠ, Vim_Menu_ਵਸਤੂ_Func func, ਖਾਲੀ *udata);
ਖਾਲੀ vim_menu_ਵਸਤੂ_ਸੈੱਟ_ਪਾਠ(Vim_Menu_ਵਸਤੂ *self, ਅਚਲ ਅੱਖਰ *ਪਾਠ);
ਖਾਲੀ vim_menu_ਵਸਤੂ_ਸੈੱਟ_accel(Vim_Menu_ਵਸਤੂ *self, ਅਚਲ ਅੱਖਰ *accel_ਪਾਠ);
ਖਾਲੀ vim_menu_ਵਸਤੂ_ਸੈੱਟ_submenu(Vim_Menu_ਵਸਤੂ *self, VimMenu *submenu);

GtkWidget *vim_menu_new(ਖਾਲੀ);
ਖਾਲੀ vim_menu_ਪਾਓ_ਵਸਤੂ(VimMenu *self, Vim_Menu_ਵਸਤੂ *ਵਸਤੂ, ਪੂਰਨ idx);
GtkWidget *vim_menu_ਪਾਓ_separator(VimMenu *self, ਪੂਰਨ idx);
ਖਾਲੀ vim_menu_remove(VimMenu *self, GtkWidget *ਵਸਤੂ);
GtkWidget *vim_menu_copy(VimMenu *self);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
