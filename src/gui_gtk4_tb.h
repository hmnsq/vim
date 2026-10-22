/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#اگر_غیر_تعریف_شدہ GUI_GTK4_TB_H
#تعریف_کرو GUI_GTK4_TB_H

#شامل "vim.h"

#اگر_تعریف_شدہ FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

قسمتعریف ساخت Vim_Toolbar_شے Vim_Toolbar_شے_2;

GtkWidget *vim_toolbar_new(خالی);
GtkWidget *vim_toolbar_داخل_کرو_button(VimToolbar *self, GtkWidget *icon, ثابت حرف *متن, صحیح idx);
GtkWidget *vim_toolbar_داخل_کرو_separator(VimToolbar *self, صحیح idx);
خالی vim_toolbar_سیٹ_style(VimToolbar *self, صحیح style, صحیح iconsize);
خالی vim_toolbar_remove(VimToolbar *self, GtkWidget *شے);

#اگر_ختم

#اگر_ختم
