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

#அந்தர்பாவ "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

வகைவரை கட்டமைப்பு _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(வெறுமை);
GtkWidget *vim_toolbar_செருகு_button(VimToolbar *self, GtkWidget *icon, மாறிலி எழுத்து *text, முழு idx);
GtkWidget *vim_toolbar_செருகு_separator(VimToolbar *self, முழு idx);
வெறுமை vim_toolbar_set_style(VimToolbar *self, முழு style, முழு iconsize);
வெறுமை vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
