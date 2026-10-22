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

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

પ્રકારવ્યાખ્યા સંરચના _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(શૂન્ય);
GtkWidget *vim_toolbar_ઉમેરો_button(VimToolbar *self, GtkWidget *icon, અચલ અક્ષર *text, પૂર્ણાંક idx);
GtkWidget *vim_toolbar_ઉમેરો_separator(VimToolbar *self, પૂર્ણાંક idx);
શૂન્ય vim_toolbar_set_style(VimToolbar *self, પૂર્ણાંક style, પૂર્ણાંક iconsize);
શૂન્ય vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
