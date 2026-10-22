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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(ಶೂನ್ಯ);
GtkWidget *vim_toolbar_ಸೇರಿಸು_button(VimToolbar *self, GtkWidget *icon, ಧ್ರುವ ವರ್ಣ *text, ಪೂರ್ಣಾಙ್ಕ idx);
GtkWidget *vim_toolbar_ಸೇರಿಸು_separator(VimToolbar *self, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ vim_toolbar_set_style(VimToolbar *self, ಪೂರ್ಣಾಙ್ಕ style, ಪೂರ್ಣಾಙ್ಕ iconsize);
ಶೂನ್ಯ vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
