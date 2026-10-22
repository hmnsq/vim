/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ifndef GUI_GTK4_TB_H
#define GUI_GTK4_TB_H

#ضمن "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

تعريفنوع بنية _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(فراغ);
GtkWidget *vim_toolbar_سك_button(VimToolbar *self, GtkWidget *icon, ثابت حرف *text, صحيح idx);
GtkWidget *vim_toolbar_سك_separator(VimToolbar *self, صحيح idx);
فراغ vim_toolbar_set_style(VimToolbar *self, صحيح style, صحيح iconsize);
فراغ vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
