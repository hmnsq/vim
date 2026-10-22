/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GUI_GTK4_TB_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_GTK4_TB_H

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR

# include <gtk/gtk.h>

# define VIM_TYPE_TOOLBAR (vim_toolbar_get_type())
G_DECLARE_FINAL_TYPE(VimToolbar, vim_toolbar, VIM, TOOLBAR, GtkWidget)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Vim_Toolbar_ਵਸਤੂ Vim_Toolbar_ਵਸਤੂ_2;

GtkWidget *vim_toolbar_new(ਖਾਲੀ);
GtkWidget *vim_toolbar_ਪਾਓ_button(VimToolbar *self, GtkWidget *icon, ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਪੂਰਨ idx);
GtkWidget *vim_toolbar_ਪਾਓ_separator(VimToolbar *self, ਪੂਰਨ idx);
ਖਾਲੀ vim_toolbar_ਸੈੱਟ_style(VimToolbar *self, ਪੂਰਨ style, ਪੂਰਨ iconsize);
ਖਾਲੀ vim_toolbar_remove(VimToolbar *self, GtkWidget *ਵਸਤੂ);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
