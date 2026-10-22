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

#अन्तर्भाव "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

प्रकारनिर्देश संरचना _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(शून्य);
GtkWidget *vim_toolbar_जोड़ो_button(VimToolbar *self, GtkWidget *icon, ध्रुव वर्ण *text, पूर्णाङ्क idx);
GtkWidget *vim_toolbar_जोड़ो_separator(VimToolbar *self, पूर्णाङ्क idx);
शून्य vim_toolbar_set_style(VimToolbar *self, पूर्णाङ्क style, पूर्णाङ्क iconsize);
शून्य vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
