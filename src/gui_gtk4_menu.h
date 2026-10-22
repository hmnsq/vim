/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#جے_بنا_تعریف GUI_GTK4_MENU_H
#تعریف_کرو GUI_GTK4_MENU_H

#ضمن "vim.h"

#جے_تعریف_شدہ FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_شے (vim_menu_bar_شے_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_Bar_شے_2, vim_menu_bar_شے, VIM, MENU_BAR_شے, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_شے (vim_menu_شے_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_شے, vim_menu_شے, VIM, MENU_شے, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

تعريفنوع تعداد
{
    VIM_MENU_شے_CLICKED,
    VIM_MENU_شے_SELECTED
} Vim_Menu_شے_Event;

تعريفنوع فراغ (*Vim_Menu_شے_Func)(Vim_Menu_شے *شے, Vim_Menu_شے_Event event, فراغ *udata);

GtkWidget *vim_menu_bar_شے_new(ثابت حرف *متن, VimMenu *menu);
فراغ vim_menu_bar_شے_سیٹ_متن(Vim_Menu_Bar_شے_2 *self, ثابت حرف *متن);

GtkWidget *vim_menu_bar_new(فراغ);
GtkWidget *vim_menu_bar_منزل_menu(VimMenuBar *self);
فراغ vim_menu_bar_داخل_کرو_شے(VimMenuBar *self, Vim_Menu_Bar_شے_2 *شے, صحيح idx);
فراغ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *شے);
فراغ vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_شے_2 *شے);

GtkWidget *vim_menu_شے_new(ثابت حرف *متن, Vim_Menu_شے_Func func, فراغ *udata);
فراغ vim_menu_شے_سیٹ_متن(Vim_Menu_شے *self, ثابت حرف *متن);
فراغ vim_menu_شے_سیٹ_accel(Vim_Menu_شے *self, ثابت حرف *accel_متن);
فراغ vim_menu_شے_سیٹ_submenu(Vim_Menu_شے *self, VimMenu *submenu);

GtkWidget *vim_menu_new(فراغ);
فراغ vim_menu_داخل_کرو_شے(VimMenu *self, Vim_Menu_شے *شے, صحيح idx);
GtkWidget *vim_menu_داخل_کرو_separator(VimMenu *self, صحيح idx);
فراغ vim_menu_remove(VimMenu *self, GtkWidget *شے);
GtkWidget *vim_menu_copy(VimMenu *self);

#جے_انت

#جے_انت
