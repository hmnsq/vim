/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#اگر_تعریف_نشده GUI_GTK4_MENU_H
#تعریف_کن GUI_GTK4_MENU_H

#ضمن "vim.h"

#اگر_تعریف_شده FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_قلم (vim_menu_bar_قلم_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_Bar_قلم_2, vim_menu_bar_قلم, VIM, MENU_BAR_قلم, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_قلم (vim_menu_قلم_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_قلم, vim_menu_قلم, VIM, MENU_قلم, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

تعريفنوع تعداد
{
    VIM_MENU_قلم_CLICKED,
    VIM_MENU_قلم_SELECTED
} Vim_Menu_قلم_Event;

تعريفنوع فراغ (*Vim_Menu_قلم_Func)(Vim_Menu_قلم *قلم, Vim_Menu_قلم_Event event, فراغ *udata);

GtkWidget *vim_menu_bar_قلم_new(ثابت حرف *متن, VimMenu *menu);
فراغ vim_menu_bar_قلم_مجموعه_متن(Vim_Menu_Bar_قلم_2 *self, ثابت حرف *متن);

GtkWidget *vim_menu_bar_new(فراغ);
GtkWidget *vim_menu_bar_مقصد_menu(VimMenuBar *self);
فراغ vim_menu_bar_درج_قلم(VimMenuBar *self, Vim_Menu_Bar_قلم_2 *قلم, صحيح idx);
فراغ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *قلم);
فراغ vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_قلم_2 *قلم);

GtkWidget *vim_menu_قلم_new(ثابت حرف *متن, Vim_Menu_قلم_Func func, فراغ *udata);
فراغ vim_menu_قلم_مجموعه_متن(Vim_Menu_قلم *self, ثابت حرف *متن);
فراغ vim_menu_قلم_مجموعه_accel(Vim_Menu_قلم *self, ثابت حرف *accel_متن);
فراغ vim_menu_قلم_مجموعه_submenu(Vim_Menu_قلم *self, VimMenu *submenu);

GtkWidget *vim_menu_new(فراغ);
فراغ vim_menu_درج_قلم(VimMenu *self, Vim_Menu_قلم *قلم, صحيح idx);
GtkWidget *vim_menu_درج_separator(VimMenu *self, صحيح idx);
فراغ vim_menu_remove(VimMenu *self, GtkWidget *قلم);
GtkWidget *vim_menu_copy(VimMenu *self);

#پایان_اگر

#پایان_اگر
