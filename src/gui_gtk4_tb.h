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

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

തരനിർവചനം ഘടന _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(ശൂന്യം);
GtkWidget *vim_toolbar_തിരുകുക_button(VimToolbar *self, GtkWidget *icon, സ്ഥിരം അക്ഷരം *text, പൂർണ്ണസംഖ്യ idx);
GtkWidget *vim_toolbar_തിരുകുക_separator(VimToolbar *self, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം vim_toolbar_set_style(VimToolbar *self, പൂർണ്ണസംഖ്യ style, പൂർണ്ണസംഖ്യ iconsize);
ശൂന്യം vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
