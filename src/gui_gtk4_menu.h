/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#جيڪڏهن_وصف_نه_ٿيل GUI_GTK4_MENU_H
#وصف_ڪريو GUI_GTK4_MENU_H

#ضمن "vim.h"

#جيڪڏهن_وصف_ٿيل FEAT_MENU

# include <gtk/gtk.h>

# define VIM_TYPE_MENU_BAR_شي (vim_menu_bar_شي_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_Bar_شي_2, vim_menu_bar_شي, VIM, MENU_BAR_شي, GtkButton)

# define VIM_TYPE_MENU_BAR (vim_menu_bar_get_type())
G_DECLARE_FINAL_TYPE(VimMenuBar, vim_menu_bar, VIM, MENU_BAR, GtkWidget)

# define VIM_TYPE_MENU_شي (vim_menu_شي_get_type())
G_DECLARE_FINAL_TYPE(Vim_Menu_شي, vim_menu_شي, VIM, MENU_شي, GtkButton)

# define VIM_TYPE_MENU (vim_menu_get_type())
G_DECLARE_FINAL_TYPE(VimMenu, vim_menu, VIM, MENU, GtkPopover)

تعريفنوع تعداد
{
    VIM_MENU_شي_CLICKED,
    VIM_MENU_شي_SELECTED
} Vim_Menu_شي_Event;

تعريفنوع فراغ (*Vim_Menu_شي_Func)(Vim_Menu_شي *شي, Vim_Menu_شي_Event event, فراغ *udata);

GtkWidget *vim_menu_bar_شي_new(ثابت حرف *متن, VimMenu *menu);
فراغ vim_menu_bar_شي_سيٽ_متن(Vim_Menu_Bar_شي_2 *self, ثابت حرف *متن);

GtkWidget *vim_menu_bar_new(فراغ);
GtkWidget *vim_menu_bar_منزل_menu(VimMenuBar *self);
فراغ vim_menu_bar_وجهو_شي(VimMenuBar *self, Vim_Menu_Bar_شي_2 *شي, صحيح idx);
فراغ vim_menu_bar_remove(VimMenuBar *self, GtkWidget *شي);
فراغ vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_شي_2 *شي);

GtkWidget *vim_menu_شي_new(ثابت حرف *متن, Vim_Menu_شي_Func func, فراغ *udata);
فراغ vim_menu_شي_سيٽ_متن(Vim_Menu_شي *self, ثابت حرف *متن);
فراغ vim_menu_شي_سيٽ_accel(Vim_Menu_شي *self, ثابت حرف *accel_متن);
فراغ vim_menu_شي_سيٽ_submenu(Vim_Menu_شي *self, VimMenu *submenu);

GtkWidget *vim_menu_new(فراغ);
فراغ vim_menu_وجهو_شي(VimMenu *self, Vim_Menu_شي *شي, صحيح idx);
GtkWidget *vim_menu_وجهو_separator(VimMenu *self, صحيح idx);
فراغ vim_menu_remove(VimMenu *self, GtkWidget *شي);
GtkWidget *vim_menu_copy(VimMenu *self);

#جيڪڏهن_پورو

#جيڪڏهن_پورو
