/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#که_نه_تعریف_شوی GUI_GTK4_MENU_H
#تعریف_کړه GUI_GTK4_MENU_H

#ضمن "vim.h"

#که_تعریف_شوی FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_توکی (vim_menu_bar_توکی_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_Bar_توکی_2, vim_menu_bar_توکی, VIM, MENU_BAR_توکی, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_توکی (vim_menu_توکی_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_توکی, vim_menu_توکی, VIM, MENU_توکی, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

تعريفنوع تعداد
{
    VIM_MENU_توکی_CLICKED,
    VIM_MENU_توکی_SELECTED
} Vim_Menu_توکی_Event;

تعريفنوع فراغ (*Vim_Menu_توکی_Func)(Vim_Menu_توکی *توکی, Vim_Menu_توکی_Event event, فراغ *udata);

GtkWidget *vim_menu_bar_توکی_new(ثابت حرف *متن, VimMenu *menu);
فراغ vim_menu_bar_توکی_سېټ_متن(Vim_Menu_Bar_توکی_2 *self, ثابت حرف *متن);

GtkWidget *vim_menu_bar_new(فراغ);
GtkWidget *vim_menu_bar_منزل_menu(VimMenuBar *self);
فراغ vim_menu_bar_ورننباسه_توکی(VimMenuBar *self, Vim_Menu_Bar_توکی_2 *توکی, صحيح idx);
فراغ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *توکی);
فراغ vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_توکی_2 *توکی);

GtkWidget *vim_menu_توکی_new(ثابت حرف *متن, Vim_Menu_توکی_Func func, فراغ *udata);
فراغ vim_menu_توکی_سېټ_متن(Vim_Menu_توکی *self, ثابت حرف *متن);
فراغ vim_menu_توکی_سېټ_accel(Vim_Menu_توکی *self, ثابت حرف *accel_متن);
فراغ vim_menu_توکی_سېټ_submenu(Vim_Menu_توکی *self, VimMenu *submenu);

GtkWidget *vim_menu_new(فراغ);
فراغ vim_menu_ورننباسه_توکی(VimMenu *self, Vim_Menu_توکی *توکی, صحيح idx);
GtkWidget *vim_menu_ورننباسه_separator(VimMenu *self, صحيح idx);
فراغ vim_menu_remove(VimMenu *self, GtkWidget *توکی);
GtkWidget *vim_menu_copy(VimMenu *self);

#د_که_پای

#د_که_پای
