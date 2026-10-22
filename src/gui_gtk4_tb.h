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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(ଶୂନ୍ୟ);
GtkWidget *vim_toolbar_ଭର୍ତ୍ତି_button(VimToolbar *self, GtkWidget *icon, ଅଚଳ ଅକ୍ଷର *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
GtkWidget *vim_toolbar_ଭର୍ତ୍ତି_separator(VimToolbar *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ vim_toolbar_set_style(VimToolbar *self, ପୂର୍ଣ୍ଣସଂଖ୍ୟା style, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iconsize);
ଶୂନ୍ୟ vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
