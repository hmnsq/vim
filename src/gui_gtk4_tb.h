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

#include "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

typedef sa_nracanaa _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(void);
GtkWidget *vim_toolbar_insert_button(VimToolbar *self, GtkWidget *icon, const var_na *text, puur_naa_nka idx);
GtkWidget *vim_toolbar_insert_separator(VimToolbar *self, puur_naa_nka idx);
void vim_toolbar_set_style(VimToolbar *self, puur_naa_nka style, puur_naa_nka iconsize);
void vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
