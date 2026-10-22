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

#অন্তর্ভাব "vim.h"

#ifdef FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

প্রকারলেখ কাঠামো _VimToolbarItem VimToolbarItem;

GtkWidget *vim_toolbar_new(শূন্য);
GtkWidget *vim_toolbar_ঢোকাও_button(VimToolbar *self, GtkWidget *icon, ধ্রুবক অক্ষর *text, পূর্ণসংখ্যা idx);
GtkWidget *vim_toolbar_ঢোকাও_separator(VimToolbar *self, পূর্ণসংখ্যা idx);
শূন্য vim_toolbar_set_style(VimToolbar *self, পূর্ণসংখ্যা style, পূর্ণসংখ্যা iconsize);
শূন্য vim_toolbar_remove(VimToolbar *self, GtkWidget *item);

#endif

#endif
